#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "images.hpp"
#include "console.hpp"
#include "filesystem.hpp"
#include "fastfiles.hpp"
#include "scheduler.hpp"
#include "imagefiles.hpp"

#include "game/game.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>
#include <utils/io.hpp>
#include <utils/concurrency.hpp>

#define CUSTOM_IMAGE_FILE_INDEX 96

namespace imagefiles
{
	namespace
	{
		utils::memory::allocator image_file_allocator;
		std::unordered_map<std::string, game::DB_IFileSysFile*> image_file_handles;

		std::string get_image_file_name()
		{
			return fastfiles::get_current_fastfile();
		}
		
		struct image_file_unk
		{
			char __pad0[120];
		};

		std::unordered_map<std::string, image_file_unk*> image_file_unk_map;

		void* get_image_file_unk_mp(unsigned int index)
		{
			if (index != CUSTOM_IMAGE_FILE_INDEX)
			{
				return &reinterpret_cast<image_file_unk*>(0x6306770_b)[index];
			}

			const auto name = get_image_file_name();
			if (image_file_unk_map.find(name) == image_file_unk_map.end())
			{
				const auto unk = image_file_allocator.allocate<image_file_unk>();
				image_file_unk_map[name] = unk;
				return unk;
			}

			return image_file_unk_map[name];
		}

		game::DB_IFileSysFile* get_image_file_handle(unsigned int index)
		{
			if (index != CUSTOM_IMAGE_FILE_INDEX)
			{
				return reinterpret_cast<game::DB_IFileSysFile**>(0x6306180_b)[index];
			}

			const auto name = get_image_file_name();
			return image_file_handles[name];
		}

		void db_create_gfx_image_stream_stub(utils::hook::assembler& a)
		{
			const auto check_image_file_handle = a.newLabel();
			const auto handle_is_open = a.newLabel();

			a.movzx(eax, cx);
			a.push(rax);
			a.push(rax);
			a.pushad64();
			a.mov(rcx, rax);
			a.call_aligned(get_image_file_unk_mp);
			a.mov(qword_ptr(rsp, 0x80), rax);
			a.popad64();
			a.pop(rax);
			a.mov(rsi, rax);
			a.pop(rax);

			a.push(rax);
			a.push(rax);
			a.pushad64();
			a.mov(rcx, rax);
			a.call_aligned(get_image_file_handle);
			a.mov(qword_ptr(rsp, 0x80), rax);
			a.popad64();
			a.pop(rax);
			a.mov(r12, rax);
			a.pop(rax);

			a.cmp(r12, r13);
			a.jnz(handle_is_open);
			a.jmp(0x3A0CA5_b);

			a.bind(handle_is_open);
			a.jmp(0x3A0CF5_b);
		}

		void* pakfile_open_stub(void* /*handles*/, unsigned int count, int is_imagefile, 
			unsigned int index, short is_localized)
		{
#ifdef DEBUG
			console::debug("Opening %s%d.pak (localized:%d)\n", is_imagefile ? "imagefile" : "soundfile", index, is_localized);
#endif

			if (index != CUSTOM_IMAGE_FILE_INDEX)
			{
				return utils::hook::invoke<void*>(0x5B2030_b, 0x6306180_b, 
					count, is_imagefile, index, is_localized
				);
			}

			const auto name = get_image_file_name();
			const auto db_fs = *game::db_fs;
			const auto handle = db_fs->vftbl->OpenFile(db_fs,
				game::SF_PAKFILE, utils::string::va("%s.pak", name.data()));
			if (handle != nullptr)
			{
				image_file_handles[name] = handle;
			}
			return handle;
		}

		int com_sprintf_stub(char* buffer, const int len, const char* fmt, unsigned int index)
		{
			if (index != CUSTOM_IMAGE_FILE_INDEX)
			{
				return game::Com_sprintf(buffer, len, fmt, index);
			}

			const auto name = get_image_file_name();
			return game::Com_sprintf(buffer, len, "%s.pak", name.data());
		}
	}

	void close_custom_handles()
	{
		const auto db_fs = *game::db_fs;
		for (const auto& handle : image_file_handles)
		{
			if (handle.second != nullptr)
			{
				db_fs->vftbl->Close(db_fs, handle.second);
			}
		}

		image_file_handles.clear();
		image_file_unk_map.clear();
		image_file_allocator.clear();
	}

	void close_handle(const std::string& fastfile)
	{
		if (!image_file_handles.contains(fastfile))
		{
			return;
		}

		const auto db_fs = *game::db_fs;
		const auto handle = image_file_handles[fastfile];
		if (handle != nullptr)
		{
			db_fs->vftbl->Close(db_fs, handle);
		}

		image_file_handles.erase(fastfile);
		image_file_allocator.free(image_file_unk_map[fastfile]);
		image_file_unk_map.erase(fastfile);
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			utils::hook::jump(0x3A0C95_b, utils::hook::assemble(db_create_gfx_image_stream_stub), true);
			utils::hook::call(0x3A0CD7_b, pakfile_open_stub);
			utils::hook::call(0x3A0CB9_b, com_sprintf_stub);
		}
	};
}

REGISTER_COMPONENT(imagefiles::component)
