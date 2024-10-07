#include <std_include.hpp>
#include "steam.hpp"

#include "component/console.hpp"

#include <utils/nt.hpp>
#include <utils/string.hpp>
#include <utils/io.hpp>

#define GENERIC_RETURN_IF_FAIL(condition) \
	if (condition != S_OK) \
	{ \
		return; \
	}

namespace steam
{
	namespace
	{
		void open_folder_prompt(char* directory)
		{
			if (CoInitializeEx(0, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE) != S_OK)
			{
				MSG_BOX_ERROR("CoInitializeEx failed. This could be because uninitialization failed, try again.");
				return;
			}

			const auto _ = gsl::finally([]()
			{
				CoUninitialize(); // uninitialize on any return so we can use this again
			});

			IFileOpenDialog* dialog;
			GENERIC_RETURN_IF_FAIL(CoCreateInstance(CLSID_FileOpenDialog, 0, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&dialog)));

			FILEOPENDIALOGOPTIONS dw_options;
			GENERIC_RETURN_IF_FAIL(dialog->GetOptions(&dw_options));
			GENERIC_RETURN_IF_FAIL(dialog->SetOptions(dw_options | FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM | FOS_PATHMUSTEXIST));
			GENERIC_RETURN_IF_FAIL(dialog->SetTitle(L"Select a valid Steam install (contains libraries like 'steam_api64.dll')"));

			if (dialog->Show(0) != S_OK) // doesn't returns S_OK when operation is cancelled
			{
				dialog->Release();
				return;
			}

			IShellItem* shell_item_result;
			GENERIC_RETURN_IF_FAIL(dialog->GetResult(&shell_item_result));

			PWSTR tmp = nullptr;
			GENERIC_RETURN_IF_FAIL(shell_item_result->GetDisplayName(SIGDN_FILESYSPATH, &tmp))

			shell_item_result->Release();
			dialog->Release();

			GENERIC_RETURN_IF_FAIL(tmp == nullptr);

			std::size_t i;
			wcstombs_s(&i, directory, MAX_PATH, tmp, MAX_PATH);
		}
	}

	uint64_t callbacks::call_id_ = 0;
	std::recursive_mutex callbacks::mutex_;
	std::map<uint64_t, bool> callbacks::calls_;
	std::map<uint64_t, callbacks::base*> callbacks::result_handlers_;
	std::vector<callbacks::result> callbacks::results_;
	std::vector<callbacks::base*> callbacks::callback_list_;

	uint64_t callbacks::register_call()
	{
		std::lock_guard<std::recursive_mutex> _(mutex_);
		calls_[++call_id_] = false;
		return call_id_;
	}

	void callbacks::register_callback(base* handler, const int callback)
	{
		std::lock_guard<std::recursive_mutex> _(mutex_);
		handler->set_i_callback(callback);
		callback_list_.push_back(handler);
	}

	void callbacks::unregister_callback(base* handler)
	{
		std::lock_guard<std::recursive_mutex> _(mutex_);
		for (auto i = callback_list_.begin(); i != callback_list_.end();)
		{
			if (*i == handler)
			{
				i = callback_list_.erase(i);
			}
			else
			{
				++i;
			}
		}
	}

	void callbacks::register_call_result(const uint64_t call, base* result)
	{
		std::lock_guard<std::recursive_mutex> _(mutex_);
		result_handlers_[call] = result;
	}

	void callbacks::unregister_call_result(const uint64_t call, base* /*result*/)
	{
		std::lock_guard<std::recursive_mutex> _(mutex_);
		const auto i = result_handlers_.find(call);
		if (i != result_handlers_.end())
		{
			result_handlers_.erase(i);
		}
	}

	void callbacks::return_call(void* data, const int size, const int type, const uint64_t call)
	{
		std::lock_guard<std::recursive_mutex> _(mutex_);

		result result{};
		result.call = call;
		result.data = data;
		result.size = size;
		result.type = type;

		calls_[call] = true;

		results_.emplace_back(result);
	}

	void callbacks::run_callbacks()
	{
		std::lock_guard<std::recursive_mutex> _(mutex_);

		for (const auto& result : results_)
		{
			if (result_handlers_.find(result.call) != result_handlers_.end())
			{
				result_handlers_[result.call]->run(result.data, false, result.call);
			}

			// someone concluded this can cause issues and from my knowledge, removing this doesn't break anything (unless it does?)
			// if it does and is discovered, please make a issue <3
			/*for (const auto& callback : callback_list_)
			{
				if (callback && callback->get_i_callback() == result.type)
				{
					callback->run(result.data, false, 0);
				}
			}*/

			if (result.data)
			{
				free(result.data);
			}
		}

		results_.clear();
	}

	extern "C" {

	bool SteamAPI_RestartAppIfNecessary()
	{
		return false;
	}

	bool SteamAPI_Init()
	{
		const std::filesystem::path steam_path = steam::SteamAPI_GetSteamInstallPath();
		if (steam_path.empty()) 
		{
			return true;
		}

		::utils::nt::library::load(steam_path / "tier0_s64.dll"); 				// not shipped with Steam on linux
		::utils::nt::library::load(steam_path / "vstdlib_s64.dll"); 			// ^
		::utils::nt::library::load(steam_path / "gameoverlayrenderer64.dll");	// however, this one is shipped
		::utils::nt::library::load(steam_path / "steamclient64.dll");			// and this one too

		return true;
	}

	void SteamAPI_RegisterCallResult(callbacks::base* result, const uint64_t call)
	{
		callbacks::register_call_result(call, result);
	}

	void SteamAPI_RegisterCallback(callbacks::base* handler, const int callback)
	{
		callbacks::register_callback(handler, callback);
	}

	void SteamAPI_RunCallbacks()
	{
		callbacks::run_callbacks();
	}

	void SteamAPI_Shutdown()
	{
	}

	void SteamAPI_UnregisterCallResult(callbacks::base* result, const uint64_t call)
	{
		callbacks::unregister_call_result(call, result);
	}

	void SteamAPI_UnregisterCallback(callbacks::base* handler)
	{
		callbacks::unregister_callback(handler);
	}

	const char* SteamAPI_GetSteamInstallPath()
	{
		static std::string install_path{};
		if (!install_path.empty())
		{
			return install_path.data();
		}

		std::string path_str;
		if (::utils::io::read_file("steam_path.txt", &path_str)) // steam_path.txt in root for directory, manually fixes issues lol
		{
			install_path = path_str;
			return install_path.data();
		}

		// check if Steam contains information in registry for the install path
		HKEY reg_key;
		if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "Software\\WOW6432Node\\Valve\\Steam", 0, KEY_QUERY_VALUE, &reg_key) == ERROR_SUCCESS)
		{
			char path[MAX_PATH]{};
			DWORD length = sizeof(path);
			RegQueryValueExA(reg_key, "InstallPath", nullptr, nullptr, reinterpret_cast<BYTE*>(path), &length);
			RegCloseKey(reg_key);
			install_path = path;
		}

		return install_path.data();
	}

	bool SteamGameServer_Init()
	{
		return true;
	}

	void SteamGameServer_RunCallbacks()
	{
	}

	void SteamGameServer_Shutdown()
	{
	}


	friends* SteamFriends()
	{
		static friends friends;
		return &friends;
	}

	matchmaking* SteamMatchmaking()
	{
		static matchmaking matchmaking;
		return &matchmaking;
	}

	game_server* SteamGameServer()
	{
		static game_server game_server;
		return &game_server;
	}

	networking* SteamNetworking()
	{
		static networking networking;
		return &networking;
	}

	remote_storage* SteamRemoteStorage()
	{
		static remote_storage remote_storage;
		return &remote_storage;
	}

	user* SteamUser()
	{
		static user user;
		return &user;
	}

	utils* SteamUtils()
	{
		static utils utils;
		return &utils;
	}

	apps* SteamApps()
	{
		static apps apps;
		return &apps;
	}

	user_stats* SteamUserStats()
	{
		static user_stats user_stats;
		return &user_stats;
	}

	}
}
