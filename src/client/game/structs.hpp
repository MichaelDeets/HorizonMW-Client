#pragma once
#include <d3d11.h>

#define PROTOCOL 3

#include "database.hpp"

namespace game
{
	typedef float vec_t;
	typedef vec_t vec2_t[2];
	typedef vec_t vec3_t[3];
	typedef vec_t vec4_t[4];

	using namespace database;

	// * scripting

	struct scr_entref_t
	{
		unsigned short entnum;
		unsigned short classnum;
	};

	enum VariableType
	{
		VAR_UNDEFINED = 0x0,
		VAR_BEGIN_REF = 0x1,
		VAR_POINTER = 0x1,
		VAR_STRING = 0x2,
		VAR_ISTRING = 0x3,
		VAR_VECTOR = 0x4,
		VAR_END_REF = 0x5,
		VAR_FLOAT = 0x5,
		VAR_INTEGER = 0x6,
		VAR_CODEPOS = 0x7,
		VAR_PRECODEPOS = 0x8,
		VAR_FUNCTION = 0x9,
		VAR_BUILTIN_FUNCTION = 0xA,
		VAR_BUILTIN_METHOD = 0xB,
		VAR_STACK = 0xC,
		VAR_ANIMATION = 0xD,
		VAR_PRE_ANIMATION = 0xE,
		VAR_THREAD = 0xF,
		VAR_NOTIFY_THREAD = 0x10,
		VAR_TIME_THREAD = 0x11,
		VAR_CHILD_THREAD = 0x12,
		VAR_OBJECT = 0x13,
		VAR_DEAD_ENTITY = 0x14,
		VAR_ENTITY = 0x15,
		VAR_ARRAY = 0x16,
		VAR_DEAD_THREAD = 0x17,
		VAR_COUNT = 0x18,
		VAR_FREE = 0x18,
		VAR_THREAD_LIST = 0x19,
		VAR_ENDON_LIST = 0x1A,
		VAR_TOTAL_COUNT = 0x1B,
	};

	struct VariableStackBuffer
	{
		const char* pos;
		unsigned __int16 size;
		unsigned __int16 bufLen;
		unsigned __int16 localId;
		char time;
		char buf[1];
	};

	union VariableUnion
	{
		int intValue;
		unsigned int uintValue;
		float floatValue;
		unsigned int stringValue;
		const float* vectorValue;
		const char* codePosValue;
		unsigned int pointerValue;
		VariableStackBuffer* stackValue;
		unsigned int entityOffset;
	};

	struct VariableValue
	{
		VariableUnion u;
		int type;
	};

	struct function_stack_t
	{
		const char* pos;
		unsigned int localId;
		unsigned int localVarCount;
		VariableValue* top;
		VariableValue* startTop;
	};

	struct function_frame_t
	{
		function_stack_t fs;
		int topType;
	};

	struct scrVmPub_t
	{
		unsigned int* localVars;
		VariableValue* maxstack;
		int function_count;
		function_frame_t* function_frame;
		VariableValue* top;
		unsigned int inparamcount;
		unsigned int outparamcount;
		function_frame_t function_frame_start[32];
		VariableValue stack[2048];
	};

	struct scr_classStruct_t
	{
		unsigned __int16 id;
		unsigned __int16 entArrayId;
		char charId;
		const char* name;
	};

	struct ObjectVariableChildren
	{
		unsigned __int16 firstChild;
		unsigned __int16 lastChild;
	};

	struct ObjectVariableValue_u_f
	{
		unsigned __int16 prev;
		unsigned __int16 next;
	};

	union ObjectVariableValue_u_o_u
	{
		unsigned __int16 size;
		unsigned __int16 entnum;
		unsigned __int16 nextEntId;
		unsigned __int16 self;
	};

	struct	ObjectVariableValue_u_o
	{
		unsigned __int16 refCount;
		ObjectVariableValue_u_o_u u;
	};

	union ObjectVariableValue_w
	{
		unsigned int type;
		unsigned int classnum;
		unsigned int notifyName;
		unsigned int waitTime;
		unsigned int parentLocalId;
	};

	struct ChildVariableValue_u_f
	{
		unsigned __int16 prev;
		unsigned __int16 next;
	};

	union ChildVariableValue_u
	{
		ChildVariableValue_u_f f;
		VariableUnion u;
	};

	struct ChildBucketMatchKeys_keys
	{
		unsigned __int16 name_hi;
		unsigned __int16 parentId;
	};

	union ChildBucketMatchKeys
	{
		ChildBucketMatchKeys_keys keys;
		unsigned int match;
	};

	struct	ChildVariableValue
	{
		ChildVariableValue_u u;
		unsigned __int16 next;
		char type;
		char name_lo;
		ChildBucketMatchKeys k;
		unsigned __int16 nextSibling;
		unsigned __int16 prevSibling;
	};

	union ObjectVariableValue_u
	{
		ObjectVariableValue_u_f f;
		ObjectVariableValue_u_o o;
	};

	struct ObjectVariableValue
	{
		ObjectVariableValue_u u;
		ObjectVariableValue_w w;
	};

	struct scrVarGlob_t
	{
		ObjectVariableValue objectVariableValue[40960];
		ObjectVariableChildren objectVariableChildren[40960];
		unsigned __int16 childVariableBucket[65536];
		ChildVariableValue childVariableValue[384000];
	};

	enum Sys_Folder
	{
		SF_ZONE = 0x0,
		SF_ZONE_LOC = 0x1,
		SF_VIDEO = 0x2,
		SF_VIDEO_LOC = 0x3,
		SF_PAKFILE = 0x4,
		SF_PAKFILE_LOC = 0x5,
		SF_COUNT = 0x6,
	};

	enum FileSysResult : std::int32_t
	{
		FILESYSRESULT_SUCCESS = 0x0,
		FILESYSRESULT_EOF = 0x1,
		FILESYSRESULT_ERROR = 0x2,
	};

	struct DB_IFileSysFile
	{
		void* file;
		uint64_t last_read;
		uint64_t bytes_read;
	};

	static_assert(sizeof(DB_IFileSysFile) == 24);

	struct DB_FileSysInterface;

	// this is a best guess, interface doesn't match up exactly w/other games (IW8, T9)
	struct DB_FileSysInterface_vtbl
	{
		DB_IFileSysFile* (__fastcall* OpenFile)(DB_FileSysInterface* _this, Sys_Folder folder, const char* filename);
		FileSysResult(__fastcall* Read)(DB_FileSysInterface* _this, DB_IFileSysFile* handle, unsigned __int64 offset, unsigned __int64 size, void* dest);
		FileSysResult(__fastcall* Tell)(DB_FileSysInterface* _this, DB_IFileSysFile* handle, unsigned __int64* bytesRead);
		__int64(__fastcall* Size)(DB_FileSysInterface* _this, DB_IFileSysFile* handle);
		void(__fastcall* Close)(DB_FileSysInterface* _this, DB_IFileSysFile* handle);
		bool(__fastcall* Exists)(DB_FileSysInterface* _this, Sys_Folder folder, const char* filename);
	};

	struct DB_FileSysInterface
	{
		DB_FileSysInterface_vtbl* vftbl;
	};

	enum CodPlayMode
	{
		CODPLAYMODE_NONE = 0x0,
		CODPLAYMODE_SP = 0x1,
		CODPLAYMODE_CORE = 0x2,
		CODPLAYMODE_SURVIVAL = 0x5,
		CODPLAYMODE_ZOMBIES = 0x6,
	};

	enum DWOnlineStatus
	{
		DW_LIVE_DISCONNECTED = 0x0,
		DW_LIVE_CONNECTING = 0x1,
		DW_LIVE_CONNECTED = 0x2,
	};

	enum DWNetStatus
	{
		DW_NET_ERROR_START_FAILED = 0x0,
		DW_NET_ERROR_NO_LOCAL_IP = 0x1,
		DW_NET_NOT_STARTED = 0x2,
		DW_NET_STARTING_LAN = 0x3,
		DW_NET_STARTED_LAN = 0x4,
		DW_NET_STARTING_ONLINE = 0x5,
		DW_NET_STARTED_ONLINE = 0x6,
	};

	enum DWLogonStatus
	{
		DW_LOGON_ERROR = 0x0,
		DW_NP_CONNECTING = 0x1,
		DW_NO_ACCOUNT_SIGNED_IN = 0x2,
		DW_DNS_NOT_RESOLVED = 0x3,
		DW_PC_STEAM_ACQUIRE_DEDICATED_LICENSE = 0x4,
		DW_PC_STEAM_ACQUIRING_DEDICATED_LICENSE = 0x5,
		DW_PC_STEAM_AUTHORIZING_DEDICATED_LICENSE = 0x6,
		DW_PC_STATIC_LICENSE = 0x7,
		DW_PC_AUTHORIZING_STATIC_LICENSE = 0x8,
		DW_LOBBY_CONNECT = 0x9,
		DW_LOBBY_CONNECTING = 0xA,
		DW_LOGON_COMPLETE = 0xB,
	};

	enum bdLobbyErrorCode : uint32_t
	{
		BD_NO_ERROR = 0x0,
		BD_TOO_MANY_TASKS = 0x1,
		BD_NOT_CONNECTED = 0x2,
		BD_SEND_FAILED = 0x3,
		BD_HANDLE_TASK_FAILED = 0x4,
		BD_START_TASK_FAILED = 0x5,
		BD_RESULT_EXCEEDS_BUFFER_SIZE = 0x64,
		BD_ACCESS_DENIED = 0x65,
		BD_EXCEPTION_IN_DB = 0x66,
		BD_MALFORMED_TASK_HEADER = 0x67,
		BD_INVALID_ROW = 0x68,
		BD_EMPTY_ARG_LIST = 0x69,
		BD_PARAM_PARSE_ERROR = 0x6A,
		BD_PARAM_MISMATCHED_TYPE = 0x6B,
		BD_SERVICE_NOT_AVAILABLE = 0x6C,
		BD_CONNECTION_RESET = 0x6D,
		BD_INVALID_USER_ID = 0x6E,
		BD_LOBBY_PROTOCOL_VERSION_FAILURE = 0x6F,
		BD_LOBBY_INTERNAL_FAILURE = 0x70,
		BD_LOBBY_PROTOCOL_ERROR = 0x71,
		BD_LOBBY_FAILED_TO_DECODE_UTF8 = 0x72,
		BD_LOBBY_ASCII_EXPECTED = 0x73,
		BD_ASYNCHRONOUS_ERROR = 0xC8,
		BD_STREAMING_COMPLETE = 0xC9,
		BD_MEMBER_NO_PROPOSAL = 0x12C,
		BD_TEAMNAME_ALREADY_EXISTS = 0x12D,
		BD_MAX_TEAM_MEMBERSHIPS_LIMITED = 0x12E,
		BD_MAX_TEAM_OWNERSHIPS_LIMITED = 0x12F,
		BD_NOT_A_TEAM_MEMBER = 0x130,
		BD_INVALID_TEAM_ID = 0x131,
		BD_INVALID_TEAM_NAME = 0x132,
		BD_NOT_A_TEAM_OWNER = 0x133,
		BD_NOT_AN_ADMIN_OR_OWNER = 0x134,
		BD_MEMBER_PROPOSAL_EXISTS = 0x135,
		BD_MEMBER_EXISTS = 0x136,
		BD_TEAM_FULL = 0x137,
		BD_VULGAR_TEAM_NAME = 0x138,
		BD_TEAM_USERID_BANNED = 0x139,
		BD_TEAM_EMPTY = 0x13A,
		BD_INVALID_TEAM_PROFILE_QUERY_ID = 0x13B,
		BD_TEAMNAME_TOO_SHORT = 0x13C,
		BD_UNIQUE_PROFILE_DATA_EXISTS_ALREADY = 0x13D,
		BD_INVALID_LEADERBOARD_ID = 0x190,
		BD_INVALID_STATS_SET = 0x191,
		BD_EMPTY_STATS_SET_IGNORED = 0x193,
		BD_NO_DIRECT_ACCESS_TO_ARBITRATED_LBS = 0x194,
		BD_STATS_WRITE_PERMISSION_DENIED = 0x195,
		BD_STATS_WRITE_TYPE_DATA_TYPE_MISMATCH = 0x196,
		BD_NO_STATS_FOR_USER = 0x197,
		BD_INVALID_ACCESS_TO_UNRANKED_LB = 0x198,
		BD_INVALID_EXTERNAL_TITLE_ID = 0x199,
		BD_DIFFERENT_LEADERBOARD_SCHEMAS = 0x19A,
		BD_TOO_MANY_LEADERBOARDS_REQUESTED = 0x19B,
		BD_ENTITLEMENTS_ERROR = 0x19C,
		BD_ENTITLEMENTS_INVALID_TITLEID = 0x19D,
		BD_ENTITLEMENTS_INVALID_LEADERBOARDID = 0x19E,
		BD_ENTITLEMENTS_INVALID_GET_MODE_FOR_TITLE = 0x19F,
		BD_ENTITLEMENTS_URL_CONNECTION_ERROR = 0x1A0,
		BD_ENTITLEMENTS_CONFIG_ERROR = 0x1A1,
		BD_ENTITLEMENTS_NAMED_PARENT_ERROR = 0x1A2,
		BD_ENTITLEMENTS_NAMED_KEY_ERROR = 0x1A3,
		BD_TOO_MANY_ENTITY_IDS_REQUESTED = 0x1A4,
		BD_STATS_READ_FAILED = 0x1A5,
		BD_INVALID_TITLE_ID = 0x1F4,
		BD_MESSAGING_INVALID_MAIL_ID = 0x258,
		BD_SELF_BLOCK_NOT_ALLOWED = 0x259,
		BD_GLOBAL_MESSAGE_ACCESS_DENIED = 0x25A,
		BD_GLOBAL_MESSAGES_USER_LIMIT_EXCEEDED = 0x25B,
		BD_MESSAGING_SENDER_DOES_NOT_EXIST = 0x25C,
		BD_AUTH_NO_ERROR = 0x2BC,
		BD_AUTH_BAD_REQUEST = 0x2BD,
		BD_AUTH_SERVER_CONFIG_ERROR = 0x2BE,
		BD_AUTH_BAD_TITLE_ID = 0x2BF,
		BD_AUTH_BAD_ACCOUNT = 0x2C0,
		BD_AUTH_ILLEGAL_OPERATION = 0x2C1,
		BD_AUTH_INCORRECT_LICENSE_CODE = 0x2C2,
		BD_AUTH_CREATE_USERNAME_EXISTS = 0x2C3,
		BD_AUTH_CREATE_USERNAME_ILLEGAL = 0x2C4,
		BD_AUTH_CREATE_USERNAME_VULGAR = 0x2C5,
		BD_AUTH_CREATE_MAX_ACC_EXCEEDED = 0x2C6,
		BD_AUTH_MIGRATE_NOT_SUPPORTED = 0x2C7,
		BD_AUTH_TITLE_DISABLED = 0x2C8,
		BD_AUTH_ACCOUNT_EXPIRED = 0x2C9,
		BD_AUTH_ACCOUNT_LOCKED = 0x2CA,
		BD_AUTH_UNKNOWN_ERROR = 0x2CB,
		BD_AUTH_INCORRECT_PASSWORD = 0x2CC,
		BD_AUTH_IP_NOT_IN_ALLOWED_RANGE = 0x2CD,
		BD_AUTH_WII_TOKEN_VERIFICATION_FAILED = 0x2CE,
		BD_AUTH_WII_AUTHENTICATION_FAILED = 0x2CF,
		BD_AUTH_IP_KEY_LIMIT_REACHED = 0x2D0,
		BD_AUTH_INVALID_GSPID = 0x2D1,
		BD_AUTH_INVALID_IP_RANGE_ID = 0x2D2,
		BD_AUTH_3DS_TOKEN_VERIFICATION_FAILED = 0x2D1,
		BD_AUTH_3DS_AUTHENTICATION_FAILED = 0x2D2,
		BD_AUTH_STEAM_APP_ID_MISMATCH = 0x2D3,
		BD_AUTH_ABACCOUNTS_APP_ID_MISMATCH = 0x2D4,
		BD_AUTH_CODO_USERNAME_NOT_SET = 0x2D5,
		BD_AUTH_WIIU_TOKEN_VERIFICATION_FAILED = 0x2D6,
		BD_AUTH_WIIU_AUTHENTICATION_FAILED = 0x2D7,
		BD_AUTH_CODO_USERNAME_NOT_BASE64 = 0x2D8,
		BD_AUTH_CODO_USERNAME_NOT_UTF8 = 0x2D9,
		BD_AUTH_TENCENT_TICKET_EXPIRED = 0x2DA,
		BD_AUTH_PS3_SERVICE_ID_MISMATCH = 0x2DB,
		BD_AUTH_CODOID_NOT_WHITELISTED = 0x2DC,
		BD_AUTH_PLATFORM_TOKEN_ERROR = 0x2DD,
		BD_AUTH_JSON_FORMAT_ERROR = 0x2DE,
		BD_AUTH_REPLY_CONTENT_ERROR = 0x2DF,
		BD_AUTH_THIRD_PARTY_TOKEN_EXPIRED = 0x2E0,
		BD_AUTH_CONTINUING = 0x2E1,
		BD_AUTH_PLATFORM_DEVICE_ID_ERROR = 0x2E4,
		BD_NO_PROFILE_INFO_EXISTS = 0x320,
		BD_FRIENDSHIP_NOT_REQUSTED = 0x384,
		BD_NOT_A_FRIEND = 0x385,
		BD_SELF_FRIENDSHIP_NOT_ALLOWED = 0x387,
		BD_FRIENDSHIP_EXISTS = 0x388,
		BD_PENDING_FRIENDSHIP_EXISTS = 0x389,
		BD_USERID_BANNED = 0x38A,
		BD_FRIENDS_FULL = 0x38C,
		BD_FRIENDS_NO_RICH_PRESENCE = 0x38D,
		BD_RICH_PRESENCE_TOO_LARGE = 0x38E,
		BD_NO_FILE = 0x3E8,
		BD_PERMISSION_DENIED = 0x3E9,
		BD_FILESIZE_LIMIT_EXCEEDED = 0x3EA,
		BD_FILENAME_MAX_LENGTH_EXCEEDED = 0x3EB,
		BD_EXTERNAL_STORAGE_SERVICE_ERROR = 0x3EC,
		BD_CHANNEL_DOES_NOT_EXIST = 0x44D,
		BD_CHANNEL_ALREADY_SUBSCRIBED = 0x44E,
		BD_CHANNEL_NOT_SUBSCRIBED = 0x44F,
		BD_CHANNEL_FULL = 0x450,
		BD_CHANNEL_SUBSCRIPTIONS_FULL = 0x451,
		BD_CHANNEL_NO_SELF_WHISPERING = 0x452,
		BD_CHANNEL_ADMIN_REQUIRED = 0x453,
		BD_CHANNEL_TARGET_NOT_SUBSCRIBED = 0x454,
		BD_CHANNEL_REQUIRES_PASSWORD = 0x455,
		BD_CHANNEL_TARGET_IS_SELF = 0x456,
		BD_CHANNEL_PUBLIC_BAN_NOT_ALLOWED = 0x457,
		BD_CHANNEL_USER_BANNED = 0x458,
		BD_CHANNEL_PUBLIC_PASSWORD_NOT_ALLOWED = 0x459,
		BD_CHANNEL_PUBLIC_KICK_NOT_ALLOWED = 0x45A,
		BD_CHANNEL_MUTED = 0x45B,
		BD_EVENT_DESC_TRUNCATED = 0x4B0,
		BD_CONTENT_UNLOCK_UNKNOWN_ERROR = 0x514,
		BD_UNLOCK_KEY_INVALID = 0x515,
		BD_UNLOCK_KEY_ALREADY_USED_UP = 0x516,
		BD_SHARED_UNLOCK_LIMIT_REACHED = 0x517,
		BD_DIFFERENT_HARDWARE_ID = 0x518,
		BD_INVALID_CONTENT_OWNER = 0x519,
		BD_CONTENT_UNLOCK_INVALID_USER = 0x51A,
		BD_CONTENT_UNLOCK_INVALID_CATEGORY = 0x51B,
		BD_KEY_ARCHIVE_INVALID_WRITE_TYPE = 0x5DC,
		BD_KEY_ARCHIVE_EXCEEDED_MAX_IDS_PER_REQUEST = 0x5DD,
		BD_BANDWIDTH_TEST_TRY_AGAIN = 0x712,
		BD_BANDWIDTH_TEST_STILL_IN_PROGRESS = 0x713,
		BD_BANDWIDTH_TEST_NOT_PROGRESS = 0x714,
		BD_BANDWIDTH_TEST_SOCKET_ERROR = 0x715,
		BD_INVALID_SESSION_NONCE = 0x76D,
		BD_ARBITRATION_FAILURE = 0x76F,
		BD_ARBITRATION_USER_NOT_REGISTERED = 0x771,
		BD_ARBITRATION_NOT_CONFIGURED = 0x772,
		BD_CONTENTSTREAMING_FILE_NOT_AVAILABLE = 0x7D0,
		BD_CONTENTSTREAMING_STORAGE_SPACE_EXCEEDED = 0x7D1,
		BD_CONTENTSTREAMING_NUM_FILES_EXCEEDED = 0x7D2,
		BD_CONTENTSTREAMING_UPLOAD_BANDWIDTH_EXCEEDED = 0x7D3,
		BD_CONTENTSTREAMING_FILENAME_MAX_LENGTH_EXCEEDED = 0x7D4,
		BD_CONTENTSTREAMING_MAX_THUMB_DATA_SIZE_EXCEEDED = 0x7D5,
		BD_CONTENTSTREAMING_DOWNLOAD_BANDWIDTH_EXCEEDED = 0x7D6,
		BD_CONTENTSTREAMING_NOT_ENOUGH_DOWNLOAD_BUFFER_SPACE = 0x7D7,
		BD_CONTENTSTREAMING_SERVER_NOT_CONFIGURED = 0x7D8,
		BD_CONTENTSTREAMING_INVALID_APPLE_RECEIPT = 0x7DA,
		BD_CONTENTSTREAMING_APPLE_STORE_NOT_AVAILABLE = 0x7DB,
		BD_CONTENTSTREAMING_APPLE_RECEIPT_FILENAME_MISMATCH = 0x7DC,
		BD_CONTENTSTREAMING_HTTP_ERROR = 0x7E4,
		BD_CONTENTSTREAMING_FAILED_TO_START_HTTP = 0x7E5,
		BD_CONTENTSTREAMING_LOCALE_INVALID = 0x7E6,
		BD_CONTENTSTREAMING_LOCALE_MISSING = 0x7E7,
		BD_VOTERANK_ERROR_EMPTY_RATING_SUBMISSION = 0x7EE,
		BD_VOTERANK_ERROR_MAX_VOTES_EXCEEDED = 0x7EF,
		BD_VOTERANK_ERROR_INVALID_RATING = 0x7F0,
		BD_MAX_NUM_TAGS_EXCEEDED = 0x82A,
		BD_TAGGED_COLLECTION_DOES_NOT_EXIST = 0x82B,
		BD_EMPTY_TAG_ARRAY = 0x82C,
		BD_INVALID_QUERY_ID = 0x834,
		BD_NO_ENTRY_TO_UPDATE = 0x835,
		BD_SESSION_INVITE_EXISTS = 0x836,
		BD_INVALID_SESSION_ID = 0x837,
		BD_ATTACHMENT_TOO_LARGE = 0x838,
		BD_INVALID_GROUP_ID = 0xAF0,
		BD_MAIL_INVALID_MAIL_ID_ERROR = 0xB55,
		BD_UCD_SERVICE_ERROR = 0xC80,
		BD_UCD_SERVICE_DISABLED = 0xC81,
		BD_UCD_UNINTIALIZED_ERROR = 0xC82,
		BD_UCD_ACCOUNT_ALREADY_REGISTERED = 0xC83,
		BD_UCD_ACCOUNT_NOT_REGISTERED = 0xC84,
		BD_UCD_AUTH_ATTEMPT_FAILED = 0xC85,
		BD_UCD_ACCOUNT_LINKING_ERROR = 0xC86,
		BD_UCD_ENCRYPTION_ERROR = 0xC87,
		BD_UCD_ACCOUNT_DATA_INVALID = 0xC88,
		BD_UCD_ACCOUNT_DATA_INVALID_FIRSTNAME = 0xC89,
		BD_UCD_ACCOUNT_DATA_INVALID_LASTNAME = 0xC8A,
		BD_UCD_ACCOUNT_DATA_INVALID_DOB = 0xC8B,
		BD_UCD_ACCOUNT_DATA_INVALID_EMAIL = 0xC8C,
		BD_UCD_ACCOUNT_DATA_INVALID_COUNTRY = 0xC8D,
		BD_UCD_ACCOUNT_DATA_INVALID_POSTCODE = 0xC8E,
		BD_UCD_ACCOUNT_DATA_INVALID_PASSWORD = 0xC8F,
		BD_UCD_ACCOUNT_NAME_ALREADY_RESISTERED = 0xC94,
		BD_UCD_ACCOUNT_EMAIL_ALREADY_RESISTERED = 0xC95,
		BD_UCD_GUEST_ACCOUNT_AUTH_CONFLICT = 0xC96,
		BD_TWITCH_SERVICE_ERROR = 0xC1D,
		BD_TWITCH_ACCOUNT_ALREADY_LINKED = 0xC1E,
		BD_TWITCH_NO_LINKED_ACCOUNT = 0xC1F,
		BD_YOUTUBE_SERVICE_ERROR = 0xCE5,
		BD_YOUTUBE_SERVICE_COMMUNICATION_ERROR = 0xCE6,
		BD_YOUTUBE_USER_DENIED_AUTHORIZATION = 0xCE7,
		BD_YOUTUBE_AUTH_MAX_TIME_EXCEEDED = 0xCE8,
		BD_YOUTUBE_USER_UNAUTHORIZED = 0xCE9,
		BD_YOUTUBE_UPLOAD_MAX_TIME_EXCEEDED = 0xCEA,
		BD_YOUTUBE_DUPLICATE_UPLOAD = 0xCEB,
		BD_YOUTUBE_FAILED_UPLOAD = 0xCEC,
		BD_YOUTUBE_ACCOUNT_ALREADY_REGISTERED = 0xCED,
		BD_YOUTUBE_ACCOUNT_NOT_REGISTERED = 0xCEE,
		BD_YOUTUBE_CONTENT_SERVER_ERROR = 0xCEF,
		BD_YOUTUBE_UPLOAD_DOES_NOT_EXIST = 0xCF0,
		BD_YOUTUBE_NO_LINKED_ACCOUNT = 0xCF1,
		BD_YOUTUBE_DEVELOPER_TAGS_INVALID = 0xCF2,
		BD_TWITTER_AUTH_ATTEMPT_FAILED = 0xDAD,
		BD_TWITTER_AUTH_TOKEN_INVALID = 0xDAE,
		BD_TWITTER_UPDATE_LIMIT_REACHED = 0xDAF,
		BD_TWITTER_UNAVAILABLE = 0xDB0,
		BD_TWITTER_ERROR = 0xDB1,
		BD_TWITTER_TIMED_OUT = 0xDB2,
		BD_TWITTER_DISABLED_FOR_USER = 0xDB3,
		BD_TWITTER_ACCOUNT_AMBIGUOUS = 0xDB4,
		BD_TWITTER_MAXIMUM_ACCOUNTS_REACHED = 0xDB5,
		BD_TWITTER_ACCOUNT_NOT_REGISTERED = 0xDB6,
		BD_TWITTER_DUPLICATE_STATUS = 0xDB7,
		BD_TWITTER_ACCOUNT_ALREADY_REGISTERED = 0xE1C,
		BD_FACEBOOK_AUTH_ATTEMPT_FAILED = 0xE11,
		BD_FACEBOOK_AUTH_TOKEN_INVALID = 0xE12,
		BD_FACEBOOK_PHOTO_DOES_NOT_EXIST = 0xE13,
		BD_FACEBOOK_PHOTO_INVALID = 0xE14,
		BD_FACEBOOK_PHOTO_ALBUM_FULL = 0xE15,
		BD_FACEBOOK_UNAVAILABLE = 0xE16,
		BD_FACEBOOK_ERROR = 0xE17,
		BD_FACEBOOK_TIMED_OUT = 0xE18,
		BD_FACEBOOK_DISABLED_FOR_USER = 0xE19,
		BD_FACEBOOK_ACCOUNT_AMBIGUOUS = 0xE1A,
		BD_FACEBOOK_MAXIMUM_ACCOUNTS_REACHED = 0xE1B,
		BD_FACEBOOK_INVALID_NUM_PICTURES_REQUESTED = 0xE1C,
		BD_FACEBOOK_VIDEO_DOES_NOT_EXIST = 0xE1D,
		BD_FACEBOOK_ACCOUNT_ALREADY_REGISTERED = 0xE1E,
		BD_APNS_INVALID_PAYLOAD = 0xE74,
		BD_APNS_INVALID_TOKEN_LENGTH_ERROR = 0xE76,
		BD_MAX_CONSOLEID_LENGTH_EXCEEDED = 0xEE1,
		BD_MAX_WHITELIST_LENGTH_EXCEEDED = 0xEE2,
		BD_USERGROUP_NAME_ALREADY_EXISTS = 0x1770,
		BD_INVALID_USERGROUP_ID = 0x1771,
		BD_USER_ALREADY_IN_USERGROUP = 0x1772,
		BD_USER_NOT_IN_USERGROUP = 0x1773,
		BD_INVALID_USERGROUP_MEMBER_TYPE = 0x1774,
		BD_TOO_MANY_MEMBERS_REQUESTED = 0x1775,
		BD_USERGROUP_NAME_TOO_SHORT = 0x1776,
		BD_RICH_PRESENCE_DATA_TOO_LARGE = 0x1A90,
		BD_RICH_PRESENCE_TOO_MANY_USERS = 0x1A91,
		BD_PRESENCE_DATA_TOO_LARGE = 0x283C,
		BD_PRESENCE_TOO_MANY_USERS = 0x283D,
		BD_USER_LOGGED_IN_OTHER_TITLE = 0x283E,
		BD_USER_NOT_LOGGED_IN = 0x283F,
		BD_SUBSCRIPTION_TOO_MANY_USERS = 0x1B58,
		BD_SUBSCRIPTION_TICKET_PARSE_ERROR = 0x1B59,
		BD_CODO_ID_INVALID_DATA = 0x1BBC,
		BD_INVALID_MESSAGE_FORMAT = 0x1BBD,
		BD_TLOG_TOO_MANY_MESSAGES = 0x1BBE,
		BD_CODO_ID_NOT_IN_WHITELIST = 0x1BBF,
		BD_TLOG_MESSAGE_TRANSFORMATION_ERROR = 0x1BC0,
		BD_REWARDS_NOT_ENABLED = 0x1BC1,
		BD_MARKETPLACE_ERROR = 0x1F40,
		BD_MARKETPLACE_RESOURCE_NOT_FOUND = 0x1F41,
		BD_MARKETPLACE_INVALID_CURRENCY = 0x1F42,
		BD_MARKETPLACE_INVALID_PARAMETER = 0x1F43,
		BD_MARKETPLACE_RESOURCE_CONFLICT = 0x1F44,
		BD_MARKETPLACE_STORAGE_ERROR = 0x1F45,
		BD_MARKETPLACE_INTEGRITY_ERROR = 0x1F46,
		BD_MARKETPLACE_INSUFFICIENT_FUNDS_ERROR = 0x1F47,
		BD_MARKETPLACE_MMP_SERVICE_ERROR = 0x1F48,
		BD_MARKETPLACE_PRECONDITION_REQUIRED = 0x1F49,
		BD_MARKETPLACE_ITEM_MULTIPLE_PURCHASE_ERROR = 0x1F4A,
		BD_MARKETPLACE_MISSING_REQUIRED_ENTITLEMENT = 0x1F4B,
		BD_MARKETPLACE_VALIDATION_ERROR = 0x1F4C,
		BD_MARKETPLACE_TENCENT_PAYMENT_ERROR = 0x1F4D,
		BD_MARKETPLACE_SKU_NOT_COUPON_ENABLED_ERROR = 0x1F4E,
		BD_LEAGUE_INVALID_TEAM_SIZE = 0x1FA4,
		BD_LEAGUE_INVALID_TEAM = 0x1FA5,
		BD_LEAGUE_INVALID_SUBDIVISION = 0x1FA6,
		BD_LEAGUE_INVALID_LEAGUE = 0x1FA7,
		BD_LEAGUE_TOO_MANY_RESULTS_REQUESTED = 0x1FA8,
		BD_LEAGUE_METADATA_TOO_LARGE = 0x1FA9,
		BD_LEAGUE_TEAM_ICON_TOO_LARGE = 0x1FAA,
		BD_LEAGUE_TEAM_NAME_TOO_LONG = 0x1FAB,
		BD_LEAGUE_ARRAY_SIZE_MISMATCH = 0x1FAC,
		BD_LEAGUE_SUBDIVISION_MISMATCH = 0x2008,
		BD_LEAGUE_INVALID_WRITE_TYPE = 0x2009,
		BD_LEAGUE_INVALID_STATS_DATA = 0x200A,
		BD_LEAGUE_SUBDIVISION_UNRANKED = 0x200B,
		BD_LEAGUE_CROSS_TEAM_STATS_WRITE_PREVENTED = 0x200C,
		BD_LEAGUE_INVALID_STATS_SEASON = 0x200D,
		BD_COMMERCE_ERROR = 0x206C,
		BD_COMMERCE_RESOURCE_NOT_FOUND = 0x206D,
		BD_COMMERCE_STORAGE_INVALID_PARAMETER = 0x206E,
		BD_COMMERCE_APPLICATION_INVALID_PARAMETER = 0x206F,
		BD_COMMERCE_RESOURCE_CONFLICT = 0x2070,
		BD_COMMERCE_STORAGE_ERROR = 0x2071,
		BD_COMMERCE_INTEGRITY_ERROR = 0x2072,
		BD_COMMERCE_MMP_SERVICE_ERROR = 0x2073,
		BD_COMMERCE_PERMISSION_DENIED = 0x2074,
		BD_COMMERCE_INSUFFICIENT_FUNDS_ERROR = 0x2075,
		BD_COMMERCE_UNKNOWN_CURRENCY = 0x2076,
		BD_COMMERCE_INVALID_RECEIPT = 0x2077,
		BD_COMMERCE_RECEIPT_USED = 0x2078,
		BD_COMMERCE_TRANSACTION_ALREADY_APPLIED = 0x2079,
		BD_COMMERCE_INVALID_CURRENCY_TYPE = 0x207A,
		BD_CONNECTION_COUNTER_ERROR = 0x20D0,
		BD_LINKED_ACCOUNTS_INVALID_CONTEXT = 0x2198,
		BD_LINKED_ACCOUNTS_INVALID_PLATFORM = 0x2199,
		BD_LINKED_ACCOUNTS_LINKED_ACCOUNTS_FETCH_ERROR = 0x219A,
		BD_LINKED_ACCOUNTS_INVALID_ACCOUNT = 0x219B,
		BD_GMSG_INVALID_CATEGORY_ID = 0x27D8,
		BD_GMSG_CATEGORY_MEMBERSHIPS_LIMIT = 0x27D9,
		BD_GMSG_NONMEMBER_POST_DISALLOWED = 0x27DA,
		BD_GMSG_CATEGORY_DISALLOWS_CLIENT_TYPE = 0x27DB,
		BD_GMSG_PAYLOAD_TOO_BIG = 0x27DC,
		BD_GMSG_MEMBER_POST_DISALLOWED = 0x27DD,
		BD_GMSG_OVERLOADED = 0x27DE,
		BD_GMSG_USER_PERCATEGORY_POST_RATE_EXCEEDED = 0x27DF,
		BD_GMSG_USER_GLOBAL_POST_RATE_EXCEEDED = 0x27E0,
		BD_GMSG_GROUP_POST_RATE_EXCEEDED = 0x27E1,
		BD_MAX_ERROR_CODE = 0x27E2,
	};

	enum bdNATType : uint8_t
	{
		BD_NAT_UNKNOWN = 0x0,
		BD_NAT_OPEN = 0x1,
		BD_NAT_MODERATE = 0x2,
		BD_NAT_STRICT = 0x3,
	};

#pragma pack(push, 1)
	struct bdAuthTicket
	{
		unsigned int m_magicNumber;
		char m_type;
		unsigned int m_titleID;
		unsigned int m_timeIssued;
		unsigned int m_timeExpires;
		unsigned long long m_licenseID;
		unsigned long long m_userID;
		char m_username[64];
		char m_sessionKey[24];
		char m_usingHashMagicNumber[3];
		char m_hash[4];
	};
#pragma pack(pop)

	enum keyNum_t
	{
		K_NONE = 0x0,
		K_FIRSTGAMEPADBUTTON_RANGE_1 = 0x1,
		K_BUTTON_A = 0x1,
		K_BUTTON_B = 0x2,
		K_BUTTON_X = 0x3,
		K_BUTTON_Y = 0x4,
		K_BUTTON_LSHLDR = 0x5,
		K_BUTTON_RSHLDR = 0x6,
		K_LASTGAMEPADBUTTON_RANGE_1 = 0x6,
		K_BS = 0x8,
		K_TAB = 0x9,
		K_ENTER = 0xD,
		K_FIRSTGAMEPADBUTTON_RANGE_2 = 0xE,
		K_BUTTON_START = 0xE,
		K_BUTTON_BACK = 0xF,
		K_BUTTON_LSTICK = 0x10,
		K_BUTTON_RSTICK = 0x11,
		K_BUTTON_LTRIG = 0x12,
		K_BUTTON_RTRIG = 0x13,
		K_DPAD_UP = 0x14,
		K_FIRSTDPAD = 0x14,
		K_DPAD_DOWN = 0x15,
		K_DPAD_LEFT = 0x16,
		K_DPAD_RIGHT = 0x17,
		K_BUTTON_LSTICK_ALTIMAGE2 = 0x10,
		K_BUTTON_RSTICK_ALTIMAGE2 = 0x11,
		K_BUTTON_LSTICK_ALTIMAGE = 0xBC,
		K_BUTTON_RSTICK_ALTIMAGE = 0xBD,
		K_LASTDPAD = 0x17,
		K_LASTGAMEPADBUTTON_RANGE_2 = 0x17,
		K_ESCAPE = 0x1B,
		K_FIRSTGAMEPADBUTTON_RANGE_3 = 0x1C,
		K_APAD_UP = 0x1C,
		K_FIRSTAPAD = 0x1C,
		K_APAD_DOWN = 0x1D,
		K_APAD_LEFT = 0x1E,
		K_APAD_RIGHT = 0x1F,
		K_LASTAPAD = 0x1F,
		K_LASTGAMEPADBUTTON_RANGE_3 = 0x1F,
		K_SPACE = 0x20,
		K_GRAVE = 0x60,
		K_TILDE = 0x7E,
		K_BACKSPACE = 0x7F,
		K_ASCII_FIRST = 0x80,
		K_ASCII_181 = 0x80,
		K_ASCII_191 = 0x81,
		K_ASCII_223 = 0x82,
		K_ASCII_224 = 0x83,
		K_ASCII_225 = 0x84,
		K_ASCII_228 = 0x85,
		K_ASCII_229 = 0x86,
		K_ASCII_230 = 0x87,
		K_ASCII_231 = 0x88,
		K_ASCII_232 = 0x89,
		K_ASCII_233 = 0x8A,
		K_ASCII_236 = 0x8B,
		K_ASCII_241 = 0x8C,
		K_ASCII_242 = 0x8D,
		K_ASCII_243 = 0x8E,
		K_ASCII_246 = 0x8F,
		K_ASCII_248 = 0x90,
		K_ASCII_249 = 0x91,
		K_ASCII_250 = 0x92,
		K_ASCII_252 = 0x93,
		K_END_ASCII_CHARS = 0x94,
		K_COMMAND = 0x96,
		K_CAPSLOCK = 0x97,
		K_POWER = 0x98,
		K_PAUSE = 0x99,
		K_UPARROW = 0x9A,
		K_DOWNARROW = 0x9B,
		K_LEFTARROW = 0x9C,
		K_RIGHTARROW = 0x9D,
		K_ALT = 0x9E,
		K_CTRL = 0x9F,
		K_SHIFT = 0xA0,
		K_INS = 0xA1,
		K_DEL = 0xA2,
		K_PGDN = 0xA3,
		K_PGUP = 0xA4,
		K_HOME = 0xA5,
		K_END = 0xA6,
		K_F1 = 0xA7,
		K_F2 = 0xA8,
		K_F3 = 0xA9,
		K_F4 = 0xAA,
		K_F5 = 0xAB,
		K_F6 = 0xAC,
		K_F7 = 0xAD,
		K_F8 = 0xAE,
		K_F9 = 0xAF,
		K_F10 = 0xB0,
		K_F11 = 0xB1,
		K_F12 = 0xB2,
		K_F13 = 0xB3,
		K_F14 = 0xB4,
		K_F15 = 0xB5,
		K_KP_HOME = 0xB6,
		K_KP_UPARROW = 0xB7,
		K_KP_PGUP = 0xB8,
		K_KP_LEFTARROW = 0xB9,
		K_KP_5 = 0xBA,
		K_KP_RIGHTARROW = 0xBB,
		K_KP_END = 0xBC,
		K_KP_DOWNARROW = 0xBD,
		K_KP_PGDN = 0xBE,
		K_KP_ENTER = 0xBF,
		K_KP_INS = 0xC0,
		K_KP_DEL = 0xC1,
		K_KP_SLASH = 0xC2,
		K_KP_MINUS = 0xC3,
		K_KP_PLUS = 0xC4,
		K_KP_NUMLOCK = 0xC5,
		K_KP_STAR = 0xC6,
		K_KP_EQUALS = 0xC7,
		K_MOUSE1 = 0xC8,
		K_MOUSE2 = 0xC9,
		K_MOUSE3 = 0xCA,
		K_MOUSE4 = 0xCB,
		K_MOUSE5 = 0xCC,
		K_MWHEELDOWN = 0xCD,
		K_MWHEELUP = 0xCE,
		K_AUX1 = 0xCF,
		K_AUX2 = 0xD0,
		K_AUX3 = 0xD1,
		K_AUX4 = 0xD2,
		K_AUX5 = 0xD3,
		K_AUX6 = 0xD4,
		K_AUX7 = 0xD5,
		K_AUX8 = 0xD6,
		K_AUX9 = 0xD7,
		K_AUX10 = 0xD8,
		K_AUX11 = 0xD9,
		K_AUX12 = 0xDA,
		K_AUX13 = 0xDB,
		K_AUX14 = 0xDC,
		K_AUX15 = 0xDD,
		K_AUX16 = 0xDE,
		K_LAST_KEY = 0xDF
	};

	struct KeyState
	{
		int down;
		int repeats;
		int binding;
	};

	struct PlayerKeyState
	{
		int overstrikeMode;
		int anyKeyDown;
		KeyState keys[256];
	};

	struct ScreenPlacement
	{
		vec2_t scaleVirtualToReal;
		vec2_t scaleVirtualToFull;
		vec2_t scaleRealToVirtual;
		vec2_t realViewportPosition;
		vec2_t realViewportSize;
		vec2_t virtualViewableMin;
		vec2_t virtualViewableMax;
		vec2_t realViewableMin;
		vec2_t realViewableMax;
		vec2_t virtualAdjustableMin;
		vec2_t virtualAdjustableMax;
		vec2_t realAdjustableMin;
		vec2_t realAdjustableMax;
		vec2_t subScreenLeft;
	};

	enum netadrtype_t
	{
		NA_BOT = 0x0,
		NA_BAD = 0x1,
		NA_LOOPBACK = 0x2,
		NA_BROADCAST = 0x3,
		NA_IP = 0x4,
	};

	enum netsrc_t
	{
		NS_CLIENT1 = 0x0,
		NS_MAXCLIENTS = 0x1,
		NS_SERVER = 0x2,
		NS_PACKET = 0x3,
		NS_INVALID_NETSRC = 0x4,
	};

	struct netadr_s
	{
		netadrtype_t type;
		unsigned char ip[4];
		unsigned __int16 port;
		netsrc_t localNetID;
		unsigned int addrHandleIndex;
	};

	struct msg_t
	{
		int overflowed;
		int readOnly;
		char* data;
		char* splitData;
		int maxsize;
		int cursize;
		int splitSize;
		int readcount;
		int bit;
		int lastEntityRef;
		netsrc_t targetLocalNetID;
		int useZlib;
	};

	enum errorParm
	{
		ERR_FATAL = 0,
		ERR_DROP = 1,
		ERR_SERVERDISCONNECT = 2,
		ERR_DISCONNECT = 3,
		ERR_SCRIPT = 4,
		ERR_SCRIPT_DROP = 5,
		ERR_LOCALIZATION = 6,
		ERR_MAPLOADERRORSUMMARY = 7,
	};

	struct CmdArgs
	{
		int nesting;
		int localClientNum[8];
		int controllerIndex[8];
		int argc[8];
		const char** argv[8];
	};

	struct CmdArgsPrivate
	{
		char textPool[8192];
		const char* argvPool[512];
		int usedTextPool[8];
		int totalUsedArgvPool;
		int totalUsedTextPool;
	};

	struct cmd_function_s
	{
		cmd_function_s* next;
		const char* name;
		void(__cdecl* function)();
	};

	enum DvarSetSource : std::uint32_t
	{
		DVAR_SOURCE_INTERNAL = 0x0,
		DVAR_SOURCE_EXTERNAL = 0x1,
		DVAR_SOURCE_SCRIPT = 0x2,
		DVAR_SOURCE_UISCRIPT = 0x3,
		DVAR_SOURCE_SERVERCMD = 0x4,
		DVAR_SOURCE_NUM = 0x5,
	};

	enum DvarFlags : std::uint32_t
	{
		DVAR_FLAG_NONE = 0,
		DVAR_FLAG_SAVED = 0x1,
		DVAR_FLAG_LATCHED = 0x2,
		DVAR_FLAG_CHEAT = 0x4,
		DVAR_FLAG_EXTERNAL = 0x100,
		DVAR_FLAG_REPLICATED = 0x8,
		DVAR_FLAG_WRITE = 0x800,
		DVAR_FLAG_READ = 0x2000,
	};

	enum dvar_type : std::int8_t
	{
		boolean = 0,
		boolean_hashed = 10,
		value = 1,
		value_hashed = 11,
		vec2 = 2,
		vec3 = 3,
		vec4 = 4,
		integer = 5,
		integer_hashed = 12,
		enumeration = 6,
		string = 7,
		color = 8,
		rgb = 9 // Color without alpha
	};

	union dvar_value
	{
		bool enabled;
		int integer;
		unsigned int unsignedInt;
		float value;
		float vector[4];
		const char* string;
		char color[4];
	};

	struct $A37BA207B3DDD6345C554D4661813EDD
	{
		int stringCount;
		const char* const* strings;
	};

	struct $9CA192F9DB66A3CB7E01DE78A0DEA53D
	{
		int min;
		int max;
	};

	struct $251C2428A496074035CACA7AAF3D55BD
	{
		float min;
		float max;
	};

	union dvar_limits
	{
		$A37BA207B3DDD6345C554D4661813EDD enumeration;
		$9CA192F9DB66A3CB7E01DE78A0DEA53D integer;
		$251C2428A496074035CACA7AAF3D55BD value;
		$251C2428A496074035CACA7AAF3D55BD vector;
	};

	struct dvar_t
	{
		int hash;
		unsigned int flags;
		dvar_type type;
		bool modified;
		dvar_value current;
		dvar_value latched;
		dvar_value reset;
		dvar_limits domain;
		char __pad0[0xC];
	};

	static_assert(sizeof(dvar_t) == 96);

	enum connstate_t
	{
		CA_DISCONNECTED = 0x0,
		CA_CINEMATIC = 0x1,
		CA_LOGO = 0x2,
		CA_CONNECTING = 0x3,
		CA_CHALLENGING = 0x4,
		CA_CONNECTED = 0x5,
		CA_SENDINGSTATS = 0x6,
		CA_SYNCHRONIZING_DATA = 0x7,
		CA_LOADING = 0x8,
		CA_PRIMED = 0x9,
		CA_ACTIVE = 0xA,
	};

	enum svscmd_type
	{
		SV_CMD_CAN_IGNORE = 0x0,
		SV_CMD_RELIABLE = 0x1,
	};

	enum threadType
	{
		THREAD_CONTEXT_MAIN = 0x0,
		THREAD_CONTEXT_BACKEND = 0x1,
		THREAD_CONTEXT_WORKER0 = 0x2,
		THREAD_CONTEXT_WORKER1 = 0x3,
		THREAD_CONTEXT_WORKER2 = 0x4,
		THREAD_CONTEXT_WORKER3 = 0x5,
		THREAD_CONTEXT_WORKER4 = 0x6,
		THREAD_CONTEXT_WORKER5 = 0x7,
		THREAD_CONTEXT_WORKER6 = 0x8,
		THREAD_CONTEXT_WORKER7 = 0x9,
		THREAD_CONTEXT_SERVER = 0xA,
		THREAD_CONTEXT_TRACE_COUNT = 0xB,
		THREAD_CONTEXT_TRACE_LAST = 0xA,
		THREAD_CONTEXT_CINEMATIC = 0xB,
		THREAD_CONTEXT_DATABASE = 0xC,
		THREAD_CONTEXT_STREAM = 0xD,
		THREAD_CONTEXT_SNDSTREAMPACKETCALLBACK = 0xE,
		THREAD_CONTEXT_STATS_WRITE = 0xF,
		THREAD_CONTEXT_COUNT = 0x10,
	};

	enum GfxDrawSceneMethod
	{
		GFX_DRAW_SCENE_STANDARD = 0x0,
	};

	struct GfxDrawMethod_s
	{
		int drawScene;
		int baseTechType;
		int emissiveTechType;
		int forceTechType;
	};

	enum TestClientType
	{
		TC_NONE = 0x0,
		TC_TEST_CLIENT = 0x1,
		TC_BOT = 0x2,
		TC_COUNT = 0x3,
	};

	enum LiveClientDropType
	{
		SV_LIVE_DROP_NONE = 0x0,
		SV_LIVE_DROP_DISCONNECT = 0x1,
	};

	struct trace_t
	{
		float fraction;
		float normal[3];
		char __pad0[25];
		bool allsolid;
		bool startsolid;
		char __pad1[0x2C]; // not correct
	};

	enum scriptType_e
	{
		SCRIPT_NONE = 0,
		SCRIPT_OBJECT = 1,
		SCRIPT_STRING = 2,
		SCRIPT_ISTRING = 3,
		SCRIPT_VECTOR = 4,
		SCRIPT_FLOAT = 5,
		SCRIPT_INTEGER = 6,
		SCRIPT_END = 8,
		SCRIPT_FUNCTION = 9,
		SCRIPT_STRUCT = 19,
		SCRIPT_ENTITY = 21,
		SCRIPT_ARRAY = 22,
	};

	struct Bounds
	{
		float midPoint[3];
		float halfSize[3];
	};

	struct rectDef_s
	{
		float x;
		float y;
		float w;
		float h;
		char horzAlign;
		char vertAlign;
	};

	// made up
	struct client_state_t
	{
		char __pad0[0x4A50];
		int ping;
		char __pad1[0x8];
		int num_players;
		char __pad2[48];
		int serverTime;
	};

	static_assert(offsetof(client_state_t, ping) == 0x4A50);
	static_assert(offsetof(client_state_t, num_players) == 0x4A5C);
	static_assert(offsetof(client_state_t, serverTime) == 19088);

	// made up
	struct connect_state_t
	{
		char __pad0[0xC];
		netadr_s address;
	};

	enum PlayerHandIndex
	{
		WEAPON_HAND_DEFAULT = 0x0,
		WEAPON_HAND_RIGHT = 0x0,
		WEAPON_HAND_LEFT = 0x1,
		NUM_WEAPON_HANDS = 0x2,
	};

	union Weapon
	{
		unsigned int data;
	};

	struct map_t
	{
		const char* name;
		int id;
		int unk;
	};

	struct UISpawnPos
	{
		float allySpawnPos[2];
		float axisSpawnPos[2];
		float objectives[5][2];
	};

	struct mapInfo
	{
		char mapName[32];
		char mapLoadName[16];
		char mapDescription[32];
		char mapLoadImage[32];
		char mapCustomKey[32][16];
		char mapCustomValue[32][64];
		int mapCustomCount;
		char mapCamoTypes[2][16];
		int isAliensMap;
		int mapPack;
		int unk1;
		int gametype;
		char __pad0[132];
		UISpawnPos mapSpawnPos[32];
	};

	static_assert(sizeof(mapInfo) == 4648);

	struct ui_info
	{
		char __pad0[16];
		float cursor_x;
		float cursor_y;
		int cursor_time;
		int ingame_cursor_visible;
	};

	enum PMem_Direction
	{
		PHYS_ALLOC_LOW = 0x0,
		PHYS_ALLOC_HIGH = 0x1,
		PHYS_ALLOC_COUNT = 0x2,
	};

	enum PMem_Source
	{
		PMEM_SOURCE_EXTERNAL = 0x0,
		PMEM_SOURCE_DATABASE = 0x1,
		PMEM_SOURCE_DEFAULT_LOW = 0x2,
		PMEM_SOURCE_DEFAULT_HIGH = 0x3,
		PMEM_SOURCE_MOVIE = 0x4,
		PMEM_SOURCE_SCRIPT = 0x5,
		PMEM_SOURCE_UNK5 = 0x5,
		PMEM_SOURCE_UNK6 = 0x6,
		PMEM_SOURCE_UNK7 = 0x7,
		PMEM_SOURCE_UNK8 = 0x8,
		PMEM_SOURCE_CUSTOMIZATION = 0x9,
	};

	enum DemoType : std::int32_t
	{
		DEMO_TYPE_NONE = 0x0,
		DEMO_TYPE_CLIENT = 0x1,
		DEMO_TYPE_SERVER = 0x2,
	};

	struct PhysicalMemoryAllocation
	{
		const char* name;
		char __pad0[16];
		unsigned __int64 pos;
		char __pad1[8];
	}; static_assert(sizeof(PhysicalMemoryAllocation) == 40);

	struct PhysicalMemoryPrim
	{
		const char* name;
		unsigned int allocListCount;
		char __pad0[4];
		unsigned char* buf;
		char __pad1[8];
		int unk1;
		char __pad2[4];
		unsigned __int64 pos;
		PhysicalMemoryAllocation allocList[32];
	}; static_assert(sizeof(PhysicalMemoryPrim) == 1328);

	struct PhysicalMemory
	{
		PhysicalMemoryPrim prim[2];
	}; static_assert(sizeof(PhysicalMemory) == 0xA60);

	struct __declspec(align(4)) ammoindex_t
	{
		int ammoIndex;
		bool isAlternate;
	};

	struct __declspec(align(4)) clipindex_t
	{
		int clipIndex;
		bool isAlternate;
	};

	struct GlobalAmmo
	{
		ammoindex_t ammoType;
		int ammoCount;
	};

	struct ClipAmmo
	{
		clipindex_t clipIndex;
		int ammoCount[2];
		int chargeAmmoCount[2];
	};

	struct PlayerActiveWeaponState
	{
		int weapAnim;
		int weaponTime;
		int weaponDelay;
		int weaponRestrictKickTime;
		int weaponState;
		int weapHandFlags;
		unsigned int weaponShotCount;
	};

	struct PlayerWeaponAnimArrays
	{
		XAnimParts* normalAnimArray[190];
		XAnimParts* altAnimArray[190];
		XAnimParts* leftHandedAnimArray[190];
		XAnimParts* leftHandedAltAnimArray[190];
	};

	struct PlayerWeaponCommonState
	{
		Weapon offHand; // 0x4c0 (size: 4, global: 0xbb43d50)
		Weapon lethalWeapon; // 0x4c4 (size: 4, global: 0xbb44340)
		Weapon tacticalWeapon; // 0x4c8 (size: 4, global: 0xbb44350)
		Weapon weapon; // 0x4cc (size: 4, global: 0xbb43cd0)
		int weapFlags; // 0x4d0 (size: 4, global: 0xbb43c50)
		float fWeaponPosFrac; // 0x4d4 (size: 8, global: 0xbb43bb0)
		float fPreviousWeaponPosFrac;
		float aimSpreadScale; // 0x4dc (size: 4, global: 0xbb43ab0)
		int adsDelayTime; // 0x4e0 (size: 4, global: 0xbb449f0)
		int spreadOverride; // 0x4e4 (size: 4, global: 0xbb448d0)
		int spreadOverrideState; // 0x4e8 (size: 4, global: 0xbb448e0)
		float fAimSpreadMovementScale; // 0x4ec (size: 4, global: 0xbb448f0)
		PlayerHandIndex lastWeaponHand; // 0x4f0 (size: 544, global: 0xbb44320)
		GlobalAmmo ammoNotInClip[15];
		ClipAmmo ammoInClip[15];
		int weapLockFlags; // 0x710 (size: 4, global: 0xbb44900)
		int16_t weapLockedEntnum; // 0x714 (size: 4, global: 0xbb44910)
		vec3_t weapLockedPos; // 0x718 (size: 12, global: 0xbb44920)
		int weaponIdleTime; // 0x724 (size: 6092, global: 0xbb43aa0)
		Weapon lastStowedWeapon;
		PlayerWeaponAnimArrays weaponAnimArrays;
	};

	static_assert(offsetof(PlayerWeaponCommonState, weapon) == 12);
	static_assert(offsetof(PlayerWeaponCommonState, lastWeaponHand) == 48);

	struct PlayerEquippedWeaponState
	{
		bool usedBefore;
		bool dualWielding;
		bool inAltMode;
		bool needsRechamber[2];
		int zoomLevelIndex;
		bool thermalEnabled;
		bool hybridScope;
	};

	struct cachedSnapshot_t
	{
		int archivedFrame;
		int time;
		int num_entities;
		int first_entity;
		int num_clients;
		int first_client;
		int num_agents;
		int first_agent;
		unsigned int scriptableCount;
		unsigned int scriptableFirstIndex;
		int usesDelta;
	};

	enum sessionState_t
	{
		SESS_STATE_PLAYING = 0x0,
		SESS_STATE_DEAD = 0x1,
		SESS_STATE_SPECTATOR = 0x2,
		SESS_STATE_INTERMISSION = 0x3,
	};

	enum team_t
	{
		TEAM_FREE = 0x0,
		TEAM_BAD = 0x0,
		TEAM_AXIS = 0x1,
		TEAM_ALLIES = 0x2,
		TEAM_SPECTATOR = 0x3,
		TEAM_NUM_TEAMS = 0x4,
	};

	enum KillCamEntityType : std::uint32_t
	{
		KC_NO_ENTITY = 0x0,
		KC_HELICOPTER = 0x1,
		KC_AIRSTRIKE = 0x2,
		KC_EXPLOSIVE = 0x3,
		KC_FAST_EXPLOSIVE = 0x4,
		KC_ROCKET = 0x5,
		KC_ROCKET_CORPSE = 0x6,
		KC_TURRET = 0x7,
		KC_JAVELIN = 0x8,
		KC_REMOTE_MISSILE = 0x9,
	};

	enum userbuttons_t
	{
		BUTTON_ATTACK = (1 << 0),
		BUTTON_SPRINT = (1 << 1),
		BUTTON_MELEEZOOM = (1 << 2), // on melee attack
		BUTTON_UNK1 = (1 << 3), // ??
		BUTTON_RELOAD = (1 << 4),
		BUTTON_USERELOAD = (1 << 5), // BUTTON_USERELOAD | BUTTON_UNK1 == BUTTON_USE?
		BUTTON_UNK3 = (1 << 6), // ??
		BUTTON_UNK4 = (1 << 7), // ??
		BUTTON_STANCE = (1 << 8),
		BUTTON_UNK5 = (1 << 9), // on ps->eFlags & EF_DEAD
		BUTTON_GOSTAND = (1 << 10), // jump button
		BUTTON_ADS = (1 << 11),
		BUTTON_UNK6 = (1 << 12), // ??
		BUTTON_UNK7 = (1 << 13), // ??
		BUTTON_FRAG = (1 << 14),
		BUTTON_SMOKE = (1 << 15),
		BUTTON_UNK8 = (1 << 16), // ucmd->selectedLoc and ucmd->selectedLocAngle related
		BUTTON_UNK9 = (1 << 17), // ??
		BUTTON_UNK10 = (1 << 18), // ??
		BUTTON_UNK11 = (1 << 19), // ??
		BUTTON_UNK12 = (1 << 20), // ucmd->remoteControlAngles and ucmd->remoteControlMove related
		BUTTON_UNK13 = (1 << 21), // ??
		BUTTON_UNK14 = (1 << 22), // ??
		BUTTON_UNK15 = (1 << 23), // ??
		BUTTON_UNK16 = (1 << 24), // ??
		BUTTON_UNK17 = (1 << 25), // ??
		BUTTON_UNK18 = (1 << 26), // ??
		BUTTON_UNK19 = (1 << 27), // ??
		BUTTON_UNK20 = (1 << 28), // ??
		BUTTON_UNK21 = (1 << 29), // ??
		BUTTON_BIT_COUNT = 29
	};

	enum playerWeaponFlags_t
	{
		PWF_RELOAD = 0x1,
		PWF_USING_OFFHAND = 0x2,
		PWF_HOLDING_BREATH = 0x4,
		PWF_FRIENDLY_FIRE = 0x8,
		PWF_ENEMY_FIRE = 0x10,
		PWF_NO_ADS = 0x20,
		PWF_USING_NIGHTVISION = 0x40,
		PWF_DISABLE_WEAPONS = 0x80u,
		PWF_TRIGGER_LEFT_FIRE = 0x100,
		PWF_TRIGGER_DOUBLE_FIRE = 0x200,
		PWF_USING_RECOILSCALE = 0x400,
		PWF_DISABLE_WEAPON_SWAPPING = 0x800,
		PWF_DISABLE_OFFHAND_WEAPONS = 0x1000,
		PWF_BLAST_IMPACT = 0x2000,
		PWF_USE_ALTERNATE = 0x4000,
		PWF_DISABLE_WEAPON_PICKUP = 0x8000,
		PWF_DISABLE_AUTO_RELOAD = 0x10000,
		PWF_INSTANT_SWITCH_CAMO = 0x20000,
		PWF_USE_MANUAL_COOLDOWN = 0x40000,
		PWF_SPRINTADS = 0x80000,
		PWF_OFFHAND_MELEE = 0x100000,
		PWF_DISABLE_OFFHAND_SECONDARY = 0x200000,
		PWF_DISABLE_RELOAD = 0x400000,
		PWF_TRIGGER_LEFT_BALL_PASS = 0x800000,
		PWF_SWITCHING_PRIMARIES = 0x1000000,
		PWF_DISABLE_GENERATE_AMMO = 0x2000000,
		PWF_WEAPON_INSPECT_ANIM = 0x4000000
	};

	enum playerOtherFlags_t
	{
		POF_INVULNERABLE = 0x1,
		POF_REMOTE_EYES = 0x2,
		POF_BALL_PASS_ALLOWED = 0x4,
		POF_THERMAL_VISION = 0x8,
		POF_THERMAL_VISION_OVERLAY_FOF = 0x10,
		POF_REMOTE_CAMERA_SOUNDS = 0x20,
		POF_ALT_SCENE_REAR_VIEW = 0x40,
		POF_ALT_SCENE_TAG_VIEW = 0x80,
		POF_SHIELD_ATTACHED_TO_WORLD_MODEL = 0x100,
		POF_DONT_LERP_VIEWANGLES = 0x200,
		POF_EMP_JAMMED_EQUIPMENT = 0x800,
		POF_LASTSTAND = 0x1000,
		POF_SHADOW_OFF = 0x2000,
		POF_FOLLOW = 0x4000,
		POF_PLAYER = 0x8000,
		POF_SPEC_ALLOW_CYCLE = 0x10000,
		POF_SPEC_ALLOW_FREELOOK = 0x20000,
		POF_SPEC_CYCLE_LOADING = 0x40000,
		POF_COMPASS_PING = 0x80000,
		POF_ADS_THIRD_PERSON_TOGGLE = 0x100000,
		POF_AUTOSPOT_OVERLAY = 0x200000,
		POF_REMOTE_TURRET = 0x400000,
		POF_KILLCAM_THERMAL_OFF = 0x800000,
		POF_AGENT = 0x1000000,
		POF_PLATFORM_PUSH = 0x2000000,
		POF_PLATFORM_ALTERNATE_COLLISION = 0x4000000,
		POF_COMPASS_EYES_ON = 0x8000000,
		POF_FOLLOW_FORCE_THIRD = 0x10000000,
		POF_FOLLOW_FORCE_FIRST = 0x20000000,
		POF_AC130 = 0x40000000,
		POF_VIEWMODEL_UFO = 0x80000000
	};

	enum playerLinkFlags_t
	{
		PLF_ANGLES_LOCKED = 0x1,
		PLF_USES_OFFSET = 0x2,
		PLF_WEAPONVIEW_ONLY = 0x4,
		PLF_FIXED_ZNEAR = 0x8,
		PLF_USES_LOCAL_TRANSFORM = 0x10
	};

	enum pmtype_t
	{
		PM_NORMAL = 0x0,
		PM_NORMAL_LINKED = 0x1,
		PM_NOCLIP = 0x2,
		PM_UFO = 0x3,
		PM_SPECTATOR = 0x4,
		PM_INTERMISSION = 0x5,
		PM_LASTSTAND = 0x6,
		PM_DEAD = 0x7,
		PM_DEAD_LINKED = 0x8,
	};

	enum playerMoveFlags_t
	{
		PMF_PRONE = 0x1,
		PMF_DUCKED = 0x2,
		PMF_MANTLE = 0x4,
		PMF_LADDER = 0x8,
		PMF_SIGHT_AIMING = 0x10,
		PMF_BACKWARDS_RUN = 0x20,
		PMF_WALKING = 0x40,
		PMF_TIME_HARDLANDING = 0x80,
		PMF_TIME_KNOCKBACK = 0x100,
		PMF_PRONEMOVE_OVERRIDDEN = 0x200,
		PMF_RESPAWNED = 0x400,
		PMF_FROZEN = 0x800,
		PMF_LADDER_FALL = 0x1000,
		PMF_JUMPING = 0x2000,
		PMF_SPRINTING = 0x4000,
		PMF_SHELLSHOCKED = 0x8000,
		PMF_MELEE_CHARGE = 0x10000,
		PMF_NO_SPRINT = 0x20000,
		PMF_NO_JUMP = 0x40000,
		PMF_REMOTE_CONTROLLING = 0x80000,
		PMF_NO_STAND = 0x800000,
		PMF_NO_CROUCH = 0x1000000,
		PMF_NO_PRONE = 0x2000000,
		PMF_NO_LEAN = 0x4000000,
		PMF_NO_MELEE = 0x8000000,
		PMF_NO_FIRE = 0x10000000,
		PMF_NO_LADDER = 0x20000000,
		PMF_NO_MANTLE = 0x40000000
	};

	enum weaponstate_t : std::uint32_t
	{
		WEAPON_READY,
		WEAPON_RAISING,
		WEAPON_RAISING_ALTSWITCH,
		WEAPON_DROPPING,
		WEAPON_DROPPING_QUICK,
		WEAPON_DROPPING_ALT,
		WEAPON_FIRING,
		WEAPON_FIRING_BALL_PASS,
		WEAPON_RECHAMBERING,
		WEAPON_RELOADING,
		WEAPON_RELOADING_INTERUPT,
		WEAPON_RELOAD_START,
		WEAPON_RELOAD_START_INTERUPT,
		WEAPON_RELOAD_END,
		WEAPON_MELEE_WAIT_FOR_RESULT,
		WEAPON_MELEE_FIRE,
		WEAPON_MELEE_END,
		WEAPON_OFFHAND_INIT,
		WEAPON_OFFHAND_PREPARE,
		WEAPON_OFFHAND_HOLD,
		WEAPON_OFFHAND_HOLD_PRIMED,
		WEAPON_OFFHAND_FIRE,
		WEAPON_OFFHAND_SWITCH,
		WEAPON_OFFHAND_DETONATE,
		WEAPON_OFFHAND_END,
		WEAPON_DETONATING,
		WEAPON_SPRINT_RAISE,
		WEAPON_SPRINT_LOOP,
		WEAPON_SPRINT_DROP,
		WEAPON_STUNNED_START,
		WEAPON_STUNNED_LOOP,
		WEAPON_STUNNED_END,
		WEAPON_NIGHTVISION_WEAR,
		WEAPON_NIGHTVISION_REMOVE,
		WEAPON_MANTLE_UP,
		WEAPON_MANTLE_OVER,
		WEAPON_BLAST_IMPACT,
		WEAPON_HYBRID_SIGHT_IN,
		WEAPON_HYBRID_SIGHT_OUT,
		WEAPON_HEAT_COOLDOWN_START,
		WEAPON_HEAT_COOLDOWN_END,
		WEAPON_HEAT_COOLDOWN_READY,
		WEAPON_OVERHEAT_END,
		WEAPON_OVERHEAT_READY,
		WEAPON_RIOTSHIELD_PREPARE,
		WEAPON_RIOTSHIELD_HOLD,
		WEAPON_RIOTSHIELD_START,
		WEAPON_RIOTSHIELD_END,
		WEAPON_INSPECTION_ANIM,
		WEAPONSTATES_NUM,
	};

	enum perksEnum : std::uint32_t
	{
		PERK_ACCURACY,
		PERK_FASTRELOAD,
		PERK_RATEOFFIRE,
		PERK_EXTRABREATH,
		PERK_LONGERSPRINT,
		PERK_DETECTEXPLOSIVE,
		PERK_EXPOSEENEMY,
		PERK_EXTRAAMMO,
		PERK_TWOPRIMARIES,
		PERK_ARMORVEST,
		PERK_FRAGGRENADE,
		PERK_SPECIALGRENADE,
		PERK_EXTENDEDMAGS,
		PERK_LIGHTWEIGHT,
		PERK_MARATHON,
		PERK_QUICKDRAW,
		PERK_IMPROVEDEXTRABREATH,
		PERK_FASTSPRINTRECOVERY,
		PERK_EXTENDEDMELEE,
		PERK_FASTOFFHAND,
		PERK_STALKER,
		PERK_EXTRA_TACTICAL,
		PERK_EXTRA_LETHAL,
		PERK_RECOIL,
		PERK_FLINCH,
		PERK_FASTMANTLE,
		PERK_DETECTINTEL,
		PERK_OVERDRIVE,
		PERK_OVERDRIVE2,
		PERK_OVERDRIVE3,
		PERK_RESISTEXPLOSION,
		PERK_RESISTSHELLSHOCK,
		PERK_EXO_BLASTSUPPRESSOR,
		PERK_EXO_SLAMBOOTS,
		PERK_EXTENDED_BATTERY,
		PERK_EXTENDED_BATTERY2,
		PERK_EXTENDED_DETECTGRENADE,
		PERK_HOLDBREATHWHILEADS,
		PERK_LONGERRANGE,
		PERK_FASTERMELEE,
		PERK_REDUCEDSWAY,
		PERK_EXTRABP,
		PERK_GRENADEDEATH,
		PERK_PISTOLDEATH,
		PERK_QUIETMOVE,
		PERK_PARABOLIC,
		PERK_BULLETDMG,
		PERK_SPRINT_AND_FIRE,
		PERK_NOT_USED,
		PERK_EXPLOSIVEBULLETS,
		PERK_SCAVENGER,
		PERK_COLDBLOODED,
		PERK_RADARIMMUNE,
		PERK_BLINDEYE,
		PERK_PLAINSIGHT,
		PERK_NOPLAYERTARGET,
		PERK_HEARTBREAKER,
		PERK_SELECTIVEHEARING,
		PERK_FASTSNIPE,
		PERK_SPYGAME,
		PERK_AUTOMANTLE,
		PERK_QUICKSWAP,
		PERK_LOWPROFILE,
		PERK_HEARTBREAKER_PRO,
		PERK_DETECTMISSILE_MINIMAP,
		PERK_THROWBACK,
		PERK_RADAR_ISREDBLIP,
		PERK_RADAR_ISREDARROW,
		PERK_MORE_MINIMAP,
		PERK_ENHANCED_TRACER,
		PERK_RADAR_ISJUGGERNAUT,
		PERK_SPRINT_AND_RELOAD,
		PERK_ACTIVE_CLOAKING,
		PERK_NOJUMPFRICTION,
		PERK_FASTCLIMB,
		PERK_PERKBREAK_RADAR,
		PERK_HIGHROLLER,
		PERK_HARDSHELL,
		PERK_CLASS_FASTHANDS,
		PERK_CLASS_FLAKJACKET,
		PERK_CLASS_DEXTERITY,
		PERK_SILENTKILL,
		PERK_NOSCOPEOUTLINE,
		PERK_SUPERBP,
		PERK_EXO_DODGETHRUSTER,
		PERK_EXO_SKIDPLATES,
		PERK_EXO_BOOSTSHOCKS,
		PERK_EXO_PING,
		PERK_FASTSPRINTADS,
		PERK_FASTOFFHANDRECOVERY,
		PERK_MINION,
		PERK_EXOPING_IMMUNE,
		PERK_BULLETRESUPPLY,
		PERK_LETHALRESUPPLY,
		PERK_TACTICALRESUPPLY,
		PERK_EXPLOSIVEAMMORESUPPLY,
		PERK_BALLCARRIER,
		PERK_UAV_PAINT,
		PERK_WILDCARD_DUALTACTICALS,
		PERK_WILDCARD_DUALLETHALS,
		PERK_EXO_CREATURE,
		PERK_STOCKPILE,
		PERK_COUNT = 102,
		PERK_UNKNOWN = PERK_COUNT
	};

	struct clientInfo_t
	{
		char clientNum;
		char name[32];
		int rank_mp;
		int prestige_mp;
		int rank_alien;
		int prestige_alien;
		char clanAbbrev[8];
		int location;
		int health;
		unsigned int playerCardPatch;
		unsigned int playerCardBackground;
		unsigned __int8 use_elite_clan_tag;
		char elite_clan_tag_text[5];
		int elite_clan_level;
		char _pad1[20];
		bool braggingRights;
	};

	struct usercmd_s
	{
		int serverTime;
		int buttons;
		int angles[3];
		Weapon weapon;
		Weapon offHandIndex;
		char forwardmove;
		char rightmove;
		uint16_t airburstMarkDistance;
		uint16_t meleeChargeYaw;
		char meleeChargeDist;
		char selectedLoc[2];
		uint8_t selectedLocAngle;
		char remoteControlAngles[2];
		char remoteControlMove[3];
		char sightedClientsMask;
		uint16_t spawnTraceEntIndex;
		uint32_t sightedSpawnsMask[2];
		uint32_t partialSightedSpawnsMask[2];
	};
	static_assert(offsetof(usercmd_s, forwardmove) == 28);
	static_assert(offsetof(usercmd_s, rightmove) == 29);

	struct session_t
	{
		int connected;
		usercmd_s cmd;
	}; // 68

	// Patoke @todo: maybe reverse?
	struct SessionData {};

	struct playerEntity_t
	{
		int bPositionToADS;
		float fLastIdleFactor;
		float baseMoveOrigin[3];
		float baseMoveAngles[3];
	};

	struct EntityState
	{
		uint16_t number;
	}; // size = ?

	struct EntHandle
	{
		uint16_t number;
		uint16_t infoIndex;
	};

	struct SprintState
	{
		int sprintButtonUpRequired;
		int sprintDelay;
		int lastSprintStart;
		int lastSprintEnd;
		int sprintStartMaxLength;
	};

	enum ViewLockTypes : std::int32_t
	{
		PLAYERVIEWLOCK_NONE = 0x0,
		PLAYERVIEWLOCK_FULL = 0x1,
		PLAYERVIEWLOCK_WEAPONJITTER = 0x2,
		PLAYERVIEWLOCKCOUNT = 0x3,
	};

	enum ActionSlotType : std::int32_t
	{
		ACTIONSLOTTYPE_DONOTHING = 0x0,
		ACTIONSLOTTYPE_SPECIFYWEAPON = 0x1,
		ACTIONSLOTTYPE_ALTWEAPONTOGGLE = 0x2,
		ACTIONSLOTTYPE_NIGHTVISION = 0x3,
		ACTIONSLOTTYPECOUNT = 0x4,
	};

	enum DofPhysicalScriptingState : std::int32_t
	{
		DOF_PHYSICAL_SCRIPTING_DISABLED = 0x0,
		DOF_PHYSICAL_SCRIPTING_HIP = 0x1,
		DOF_PHYSICAL_SCRIPTING_ADS = 0x2,
		DOF_PHYSICAL_SCRIPTING_BOTH = 0x3,
		DOF_PHYSICAL_SCRIPTING_COUNT = 0x4,
	};

	enum objectiveState_t : std::int32_t
	{
		OBJST_EMPTY = 0x0,
		OBJST_ACTIVE = 0x1,
		OBJST_INVISIBLE = 0x2,
		OBJST_DONE = 0x3,
		OBJST_CURRENT = 0x4,
		OBJST_FAILED = 0x5,
		OBJST_NUMSTATES = 0x6,
	};

	enum ObjectiveVisMode : std::int32_t
	{
		OBJVIS_BYCLIENT = 0x0,
		OBJVIS_BYTEAM = 0x1,
		OBJVIS_BYMASK = 0x2,
	};

	struct ActionSlotParam_SpecifyWeapon
	{
		Weapon weapon;
	};

	struct ActionSlotParam
	{
		ActionSlotParam_SpecifyWeapon specifyWeapon;
	};

	struct ShieldState_s
	{
		int flags;
	};

	struct compressedAnimData_s
	{
		int flags;
		int animRate;
		int distanceIn2D;
		int distanceOut2D;
		int distanceInZ;
		int distanceOutZ;
		int endScriptAnimTableIndex;
	};

	struct MantleState
	{
		float yaw; // 0x264 (size: 4, global: 0xbb443f0)
		int startPitch; // 0x268 (size: 4, global: 0xbb44400)
		int transIndex; // 0x26c (size: 4, global: 0xbb443a0)
		int flags; // 0x270 (size: 4, global: 0xbb442a0)
		int startTime; // 0x274 (size: 4, global: 0xbb443b0)
		vec3_t startPosition; // 0x278 (size: 12, global: 0xbb443c0)
		compressedAnimData_s compressedAnimData; // 0x284 (size: 28, global: 0xbb44410)
	};
	struct PlayerVehicleState
	{
		int16_t entity; // 0xd8 (size: 4, global: 0xbb43ec0)
		int flags; // 0xdc (size: 4, global: 0xbb43ed0)
		int16_t targetEntity; // 0xe0 (size: 4, global: 0xbb43ee0)
		vec3_t origin; // 0xe4 (size: 12, global: 0xbb43ef0)
		vec3_t angles; // 0xf0 (size: 12, global: 0xbb43f20)
		vec3_t velocity; // 0xfc (size: 12, global: 0xbb43f50)
		vec3_t angVelocity; // 0x108 (size: 12, global: 0xbb43f80)
		vec2_t tilt; // 0x114 (size: 8, global: 0xbb43fb0)
		vec2_t tiltVelocity; // 0x11c (size: 8, global: 0xbb43fd0)
		union
		{
			vec2_t gunAngles; // 0x124 (size: 4, global: 0xbb43ff0)
			vec2_t wheelAngles;
		};
		unsigned int splineId; // 0x12c (size: 4, global: 0xbb44010)
		unsigned int splineNodeIndex; // 0x130 (size: 4, global: 0xbb44020)
		float splineLambda; // 0x134 (size: 4, global: 0xbb44030)
		vec2_t corridorSpeeds; // 0x138 (size: 8, global: 0xbb44040)
		int16_t orbitEnt; // 0x140 (size: 2, global: 0xbb44060)
		int16_t orbitInitialEnt; // 0x142 (size: 2, global: 0xbb44070)
		int16_t orbitLookAtEnt; // 0x144 (size: 4, global: 0xbb44080)
		float orbitYawOffset; // 0x148 (size: 4, global: 0xbb44090)
		float orbitZOffset; // 0x14c (size: 4, global: 0xbb440a0)
		float hoverFrac; // 0x150 (size: 4, global: 0xbb440b0)
		float maxSpeedThrottle; // 0x154 (size: 4, global: 0xbb440c0)
	};
	struct EntityEvent
	{
		int eventType;
		int eventParm;
	};

	struct playerEvents_t
	{
		int eventSequence; // 0x158 (size: 4, global: 0xbb439e0)
		EntityEvent events[4]; // 0x15c (size: 8, global: 0xbb43b40)
		int oldEventSequence; // 0x17c (size: 32, global: 0xbb439f0)
	};

	// Patoke @note: offset, sizes, and the weird global stuff are for h1's leaked build with symbols for the ps4
	// they were generated by dumping netfields and manually filtering, some stuff changed for the pc version, that's why u'll see some misplaced offsets
	// the script used can be found here: https://github.com/Patoke/h1-reverse-tools/
	struct playerState_s
	{
		char clientNum; // 0x0 (size: 1, global: 0xbb43dc0)
		bool cursorHintDualWield; // 0x1 (size: 1, global: 0xbb44290)
		uint8_t pm_type; // 0x2 (size: 1, global: 0xbb44170)
		uint8_t remoteEyesTagname; // 0x3 (size: 1, global: 0xbb44730)
		uint8_t shellshockIndex; // 0x4 (size: 1, global: 0xbb44300)
		uint8_t damageEvent; // 0x5 (size: 1, global: 0xbb43e20)
		uint8_t damageYaw; // 0x6 (size: 1, global: 0xbb43d70)
		uint8_t damagePitch; // 0x7 (size: 1, global: 0xbb440d0)
		uint8_t damageCount; // 0x8 (size: 1, global: 0xbb440e0)
		uint8_t damageFlags; // 0x9 (size: 1, global: 0xbb446a0)
		uint8_t cursorHint; // 0xa (size: 1, global: 0xbb43e30)
		uint8_t meleeChargeDist; // 0xb (size: 1, global: 0xbb44610)
		uint8_t meleeServerResult; // 0xc (size: 1, global: 0xbb44630)
		uint8_t laserIndex; // 0xd (size: 1, global: 0xbb44a20)
		char corpseIndex; // 0xf (size: 1, global: 0xbb441c0)
		bool radarMode; // 0x10 (size: 1, global: 0xbb44770)
		bool enemyRadarMode; // 0x11 (size: 1, global: 0xbb44780)
		uint8_t perkSlots[9]; // 0x12 (size: 1, global: 0xbb44500)
		int16_t remoteEyesEnt; // 0x1c (size: 2, global: 0xbb44720)
		int16_t remoteControlEnt; // 0x1e (size: 2, global: 0xbb44740)
		int16_t throwbackGrenadeOwner; // 0x20 (size: 2, global: 0xbb43d90)
		int16_t viewlocked_entNum; // 0x22 (size: 2, global: 0xbb43dd0)
		int16_t groundEntityNum; // 0x24 (size: 2, global: 0xbb43bc0)
		int16_t linkWeaponEnt; // 0x26 (size: 2, global: 0xbb43e40)
		int16_t cursorHintEntIndex; // 0x28 (size: 2, global: 0xbb44100)
		int16_t meleeChargeEnt; // 0x2a (size: 2, global: 0xbb44620)
		int16_t movingPlatformEntity; // 0x2c (size: 2, global: 0xbb444c0)
		int16_t groundRefEnt; // 0x2e (size: 2, global: 0xbb44a10)
		uint16_t loopSound; // 0x30 (size: 2, global: 0xbb44690)
		int16_t linkFlags; // 0x32 (size: 2, global: 0xbb44650)
		int16_t remoteTurretEnt; // 0x34 (size: 2, global: 0xbb44750)
		int16_t gravity; // 0x36 (size: 2, global: 0xbb43df0)
		int16_t speed; // 0x38 (size: 2, global: 0xbb44ab0)
		uint16_t shellshockFlashDuration; // 0x3a (size: 2, global: 0xbb442e0)
		uint16_t shellshockMoveDuration; // 0x3c (size: 2, global: 0xbb442f0)
		uint16_t viewmodelIndex; // 0x3e (size: 2, global: 0xbb43d80)
		unsigned int cursorHintString; // 0x40 (size: 4, global: 0xbb44130)
		int meleeChargeTime; // 0x44 (size: 4, global: 0xbb445e0)
		int shellshockTime; // 0x48 (size: 4, global: 0xbb442d0)
		int commandTime; // 0x4c (size: 4, global: 0xbb439d0)
		int pm_time; // 0x50 (size: 4, global: 0xbb43a50)
		int pm_flags; // 0x54 (size: 4, global: 0xbb43a90)
		int eFlags; // 0x58 (size: 4, global: 0xbb43be0)
		int otherFlags; // 0x5c (size: 4, global: 0xbb43d60)
		int foliageSoundTime; // 0x60 (size: 4, global: 0xbb447e0)
		int grenadeTimeLeft; // 0x64 (size: 4, global: 0xbb44120)
		int throwbackGrenadeTimeLeft; // 0x68 (size: 4, global: 0xbb44490)
		int jumpTime; // 0x6c (size: 4, global: 0xbb44390)
		float jumpOriginZ; // 0x70 (size: 4, global: 0xbb43bf0)
		uint16_t bobCycle; // 0xe (size: 1, global: 0xbb43af0)
		vec3_t origin; // 0x74 (size: 12, global: 0xbb43ac0)
		vec3_t velocity; // 0x80 (size: 12, global: 0xbb43b10)
		vec3_t delta_angles; // 0x8c (size: 12, global: 0xbb43e50)
		vec3_t vLadderVec; // 0x98 (size: 12, global: 0xbb446b0)
		Weapon throwbackWeapon; // 0xa4 (size: 4, global: 0xbb444a0)
		Weapon cursorHintWeapon; // 0xa8 (size: 4, global: 0xbb44110)
		int legsTimer; // 0xac (size: 4, global: 0xbb43a10)
		int legsAnim; // 0xb0 (size: 4, global: 0xbb43a30)
		int torsoTimer; // 0xb4 (size: 4, global: 0xbb43a20)
		int torsoAnim; // 0xb8 (size: 4, global: 0xbb43ba0)
		int animMoveType; // 0xbc (size: 4, global: 0xbb43a80)
		int damageTimer; // 0xc0 (size: 4, global: 0xbb43ce0)
		int movementDir; // 0xc4 (size: 4, global: 0xbb43b20)
		int turnStartTime; // 0xc8 (size: 4, global: 0xbb43ca0)
		int turnRemaining; // 0xcc (size: 4, global: 0xbb43c90)
		int turnDirection; // 0xd0 (size: 4, global: 0xbb43cb0)
		int flinch; // 0xd4 (size: 4, global: 0xbb440f0)
		playerEvents_t pe; // 0x158 (size: 40, global: 0xbb439e0)
		int unpredictableEventSequence; // 0x180 (size: 4, global: 0xbb43b80)
		int unpredictableEventSequenceOld; // 0x184 (size: 4, global: 0xbb43b90)
		EntityEvent unpredictableEvents[4]; // 0x188 (size: 32, global: 0xbb43d00)
		float viewangles[3]; // 0x1a8 (size: 4, global: 0xbb43c20)
		int viewHeightTarget; // 0x1b4 (size: 4, global: 0xbb43c60)
		float viewHeightCurrent; // 0x1b8 (size: 4, global: 0xbb43bd0)
		int viewHeightLerpTime; // 0x1bc (size: 4, global: 0xbb44260)
		int viewHeightLerpTarget; // 0x1c0 (size: 4, global: 0xbb44160)
		int viewHeightLerpDown; // 0x1c4 (size: 4, global: 0xbb44230)
		vec2_t viewAngleClampBase; // 0x1c8 (size: 8, global: 0xbb44240)
		vec2_t viewAngleClampRange; // 0x1d0 (size: 8, global: 0xbb444e0)
		int stats[4];
		float proneDirection; // 0x1e8 (size: 4, global: 0xbb44280)
		float proneDirectionPitch; // 0x1ec (size: 4, global: 0xbb44600)
		float proneTorsoPitch; // 0x1f0 (size: 4, global: 0xbb445f0)
		ViewLockTypes viewlocked; // 0x1f4 (size: 4, global: 0xbb447f0)
		vec3_t linkAngles; // 0x1f8 (size: 4, global: 0xbb446e0)
		vec3_t linkWeaponAngles; // 0x204 (size: 4, global: 0xbb44660)
		unsigned int cursorHintString2; // 0x210 (size: 4, global: 0xbb44140)
		unsigned int cursorHintString3; // 0x214 (size: 4, global: 0xbb44150)
		int iCompassPlayerInfo; // 0x218 (size: 4, global: 0xbb44810)
		int radarEnabled; // 0x21c (size: 4, global: 0xbb441d0)
		int enemyRadarEnabled; // 0x220 (size: 4, global: 0xbb44790)
		int radarBlocked; // 0x224 (size: 4, global: 0xbb441e0)
		int radarStrength; // 0x228 (size: 4, global: 0xbb441f0)
		int radarShowEnemyDirection; // 0x22c (size: 4, global: 0xbb44200)
		int locationSelectionInfo; // 0x234 (size: 4, global: 0xbb44590)
		SprintState sprintState; // 0x238 (size: 20, global: 0xbb442c0)
		float holdBreathScale; // 0x24c (size: 4, global: 0xbb43c30)
		int holdBreathTimer; // 0x250 (size: 4, global: 0xbb447a0)
		int stationaryZoomTimer; // 0x254 (size: 4, global: 0xbb447b0)
		float stationaryZoomScale; // 0x258 (size: 4, global: 0xbb43c80)
		float moveSpeedScaleMultiplier; // 0x25c (size: 4, global: 0xbb43da0)
		float grenadeCookScale; // 0x260 (size: 4, global: 0xbb43db0)
		MantleState mantleState; // 0x264 (size: 60, global: 0xbb443f0)
		ShieldState_s shieldState; // 0x2a0 (size: 4, global: 0xbb44220)
		PlayerActiveWeaponState weapState[NUM_WEAPON_HANDS]; // 0x2a4 (size: 4, global: 0xbb43a40)
		Weapon weaponsEquipped[15]; // 604
		PlayerEquippedWeaponState weapEquippedData[15]; // 664
		PlayerWeaponCommonState weapCommon; // 0x4c0 (size: 4, global: 0xbb43d50)
		unsigned int perks[4];
		// Patoke @note: no idea if these are right, just added straight in for the lulz
		ActionSlotType actionSlotType[4]; // 0x1f04 (size: 4, global: 0xbb44550)
		ActionSlotParam actionSlotParam[4]; // 0x1f14 (size: 4, global: 0xbb445a0)
		int16_t weaponHudIconOverrides[6]; // 0x1f24 (size: 12, global: 0xbb44820)
		float viewKickScale; // 0x1f30 (size: 4, global: 0xbb44310)
		int chargeTimer; // 0x1f34 (size: 4, global: 0xbb44aa0)
		float dofNearStart; // 0x1f38 (size: 4, global: 0xbb44960)
		float dofNearEnd; // 0x1f3c (size: 4, global: 0xbb445b0)
		float dofFarStart; // 0x1f40 (size: 4, global: 0xbb445c0)
		float dofFarEnd; // 0x1f44 (size: 4, global: 0xbb445d0)
		float dofNearBlur; // 0x1f48 (size: 4, global: 0xbb43e00)
		float dofFarBlur; // 0x1f4c (size: 4, global: 0xbb43e10)
		float dofViewmodelStart; // 0x1f50 (size: 4, global: 0xbb44970)
		float dofViewmodelEnd; // 0x1f54 (size: 4, global: 0xbb44980)
		DofPhysicalScriptingState dofPhysicalScriptingState; // 0x1f58 (size: 4, global: 0xbb44a30)
		float dofPhysicalFstop; // 0x1f5c (size: 4, global: 0xbb44a40)
		float dofPhysicalFocusDistance; // 0x1f60 (size: 4, global: 0xbb44a50)
		float dofPhysicalFocusSpeed; // 0x1f64 (size: 4, global: 0xbb44a60)
		float dofPhysicalApertureSpeed; // 0x1f68 (size: 4, global: 0xbb44a70)
		float dofPhysicalViewModelFstop; // 0x1f6c (size: 4, global: 0xbb44a80)
		float dofPhysicalViewModelFocusDistance; // 0x1f70 (size: 1164, global: 0xbb44a90)
		char _pad1[1340];
		int deltaTime;
		char size_pad2[9496];
	};

	static_assert(offsetof(playerState_s, pm_type) == 2);
	static_assert(offsetof(playerState_s, pm_time) == 80);
	static_assert(offsetof(playerState_s, pm_flags) == 84);
	static_assert(offsetof(playerState_s, eFlags) == 88);
	static_assert(offsetof(playerState_s, otherFlags) == 92);
	static_assert(offsetof(playerState_s, origin) == 120);
	static_assert(offsetof(playerState_s, velocity) == 132);
	static_assert(offsetof(playerState_s, viewangles) == 300);
	static_assert(offsetof(playerState_s, sprintState) == 440);
	static_assert(offsetof(playerState_s, weapState) == 548);
	static_assert(offsetof(playerState_s, weapCommon.ammoInClip) == 1136);
	static_assert(offsetof(playerState_s, perks) == 7608);
	static_assert(offsetof(playerState_s, deltaTime) == 9076);
	static_assert(sizeof(playerState_s) == 0x4890); // 18576

	static_assert(offsetof(playerState_s, bobCycle) == 0x74);

	static_assert(offsetof(playerState_s, locationSelectionInfo) == 0x1B4);
	static_assert(offsetof(playerState_s, radarBlocked) == 0x1A8);
	static_assert(offsetof(playerState_s, radarStrength) == 0x1AC);
	static_assert(offsetof(playerState_s, cursorHintString2) == 0x194);
	static_assert(offsetof(playerState_s, cursorHintString3) == 0x198);
	static_assert(offsetof(playerState_s, cursorHintWeapon) == 0xAC);
	static_assert(offsetof(playerState_s, gravity) == 0x34);
	static_assert(offsetof(playerState_s, stats) == 0x15C);
	static_assert(offsetof(playerState_s, viewHeightTarget) == 0x138);
	static_assert(offsetof(playerState_s, viewHeightCurrent) == 0x13C);
	static_assert(offsetof(playerState_s, perkSlots) == 0x11);
	static_assert(offsetof(playerState_s, groundEntityNum) == 34);
	static_assert(offsetof(playerState_s, linkFlags) == 48);
	static_assert(offsetof(playerState_s, viewlocked_entNum) == 0x20);
	static_assert(offsetof(playerState_s, movingPlatformEntity) == 0x2A);

	struct snapshot_s
	{
		playerState_s ps;
	};

	enum CubemapShot : std::int32_t
	{
		CUBEMAPSHOT_NONE = 0x0,
		CUBEMAPSHOT_RIGHT = 0x1,
		CUBEMAPSHOT_LEFT = 0x2,
		CUBEMAPSHOT_BACK = 0x3,
		CUBEMAPSHOT_FRONT = 0x4,
		CUBEMAPSHOT_UP = 0x5,
		CUBEMAPSHOT_DOWN = 0x6,
		CUBEMAPSHOT_COUNT = 0x7,
	};

	struct GfxViewport
	{
		__int16 x;
		__int16 y;
		__int16 width;
		__int16 height;
	};

	struct RefdefView
	{
		float tanHalfFovX;
		float tanHalfFovY;
		float unk[2];
		float org[3];
		float axis[3][3];
	};

	struct refdef_t_correct
	{
		GfxViewport displayViewport;
		int time;
		int frameTime;
		RefdefView view;
		float viewOffset[3];
		float viewOffsetPrev[3];
	};

	struct refdef_t
	{
		char __pad0[0x10];
		float fovX;
		float fovY;
		char __pad1[0x8];
		vec3_t org;
		vec3_t axis[3];
	};

	struct cg_s
	{
		playerState_s predictedPlayerState;
		/*centity_s*/void* predictedPlayerEntity;
		playerEntity_t playerEntity;
		int predictedErrorTime;
		float predictedError[3];
		char clientNum;
		int localClientNum;
		DemoType demoType;
		CubemapShot cubemapShot;
		int cubemapSize;
		char __pad4[28];
		snapshot_s* nextSnap;
		char __pad1[582400];
		int spectatingThirdPerson;
		int renderingThirdPerson;
		char __pad5[24];
		refdef_t_correct refdef;
		char __pad2[378452];
		int inKillCam;
		char __pad3[16];
		KillCamEntityType killCamEntityType;
	};

	static_assert(offsetof(cg_s, cubemapShot) == 18644);
	static_assert(offsetof(cg_s, cubemapSize) == 18648);
	static_assert(offsetof(cg_s, nextSnap) == 18680);
	static_assert(offsetof(cg_s, spectatingThirdPerson) == 601088);
	static_assert(offsetof(cg_s, renderingThirdPerson) == 601092);
	static_assert(offsetof(cg_s, refdef) == 601120);
	static_assert(offsetof(cg_s, inKillCam) == 979676);
	static_assert(offsetof(cg_s, killCamEntityType) == 979696);

	struct gclient_s
	{
		playerState_s ps;
		sessionState_t sessionState; // 18576
		char __pad3[20]; // 18580
		session_t sess; // 18600
		char __pad3_align[132]; // 18604

		team_t team; // 18800
		char __pad4[30];
		char name[32]; // 18834
		char __pad5[622];
		int flags; // 19488 
	}; // size = ?

	static_assert(offsetof(gclient_s, sessionState) == 18576);
	static_assert(offsetof(gclient_s, team) == 18800);
	static_assert(offsetof(gclient_s, name) == 18834);
	static_assert(offsetof(gclient_s, flags) == 19488);

#pragma pack(push, 1)
	struct gentity_s
	{
		EntityState s;
		char _padding[106];
		vec3_t trBase;
		vec3_t trDelta;
		char _padding1[12];
		vec3_t currentAngles;
		char pad_0008[108];
		Bounds box;
		Bounds absBox;
		vec3_t origin;
		char pad_0144[12];
		short owner;
		char _pad0[6];
		gclient_s* client;
		char _pad6[8];
		gclient_s* agent;
		char _pad1[40];
		scr_string_t script_classname;
		char _pad2[20];
		int flags;
		int eventTime;
		int clipmask; // 440
		char _pad3[60];
		int missile_flags;
		char _pad4[68];
		EntHandle remoteControlledOwner;
		char _pad5[156];
	}; // size = 736
#pragma pack(pop)
	static_assert(offsetof(gentity_s, client) == 0x158);
	static_assert(offsetof(gentity_s, agent) == 0x168);
	static_assert(offsetof(gentity_s, owner) == 0x150);
	static_assert(sizeof(gentity_s) == 736);

	struct pmove_t
	{
		playerState_s* ps;
		usercmd_s cmd;
		usercmd_s oldcmd;
		int tracemask;
		int numtouch;
		int touchents[32];
		Bounds bounds;
		float speed;
		__int16 contactEntity;
		int proneChange;
		bool mantleStarted;
		float mantleEndPos[3];
		int mantleDuration;
		float meleeEntOrigin[3];
		float meleeEntVelocity[3];
		int viewChangeTime;
		float viewChange;
		float fTorsoPitch;
		float fWaistPitch;
		int remoteTurretFireTime;
		int remoteTurretShotCount;
		int lastUpdateCMDServerTime;
		bool boostEventPending;
		unsigned int groundSurfaceType;
		unsigned __int8 handler;
	};

	static_assert(offsetof(pmove_t, touchents) == 144);

	struct pml_t
	{
		float forward[3];
		float right[3];
		float up[3];
		float frametime;
	};

	struct clientHeader_t
	{
		int state;
		char __pad0[44];
		netadr_s remoteAddress;
	}; // size = ?

	struct client_t
	{
		clientHeader_t header;	// 0
		char __pad0[265164];	// 68
		int reliableSequence;	// 265232
		int reliableAcknowledge;// 265236
		char __pad1[397928];	// 265240
		gentity_s* gentity;		// 663168
		char name[32];			// 663176
		int lastPacketTime;		// 663208
		char __pad5[348748];	// 636212
	}; // size = 1011960

	static_assert(sizeof(client_t) == 1011960);
	static_assert(offsetof(client_t, __pad0) == 0x44);

	struct DObj
	{
		char __pad0[15];
		unsigned char numModels;
		char __pad1[199];
		XModel* models;
	};

	static_assert(offsetof(DObj, models) == 216);



	struct Glyph
	{
		unsigned short letter;
		char x0;
		char y0;
		char dx;
		char pixelWidth;
		char pixelHeight;
		float s0;
		float t0;
		float s1;
		float t1;
	};

	struct Font_s
	{
		const char* fontName;
		int pixelHeight;
		int glyphCount;
		Material* material;
		Material* glowMaterial;
		Glyph* glyphs;
	};

	struct DB_AuthSignature
	{
		unsigned char bytes[256];
	};

	struct DB_AuthHash
	{
		unsigned char bytes[32];
	};

	struct XPakHeader
	{
		char header[8];
		std::int32_t version;
		unsigned char unknown[16];
		DB_AuthHash hash;
		DB_AuthSignature signature;
	};

	struct DBFile
	{
		char __pad0[32];
		char name[64];
	};

	namespace hks
	{
		struct lua_State;
		struct HashTable;
		struct cclosure;

		struct GenericChunkHeader
		{
			unsigned __int64 m_flags;
		};

		struct ChunkHeader : GenericChunkHeader
		{
			ChunkHeader* m_next;
		};

		struct UserData : ChunkHeader
		{
			unsigned __int64 m_envAndSizeOffsetHighBits;
			unsigned __int64 m_metaAndSizeOffsetLowBits;
			char m_data[8];
		};

		struct InternString
		{
			unsigned __int64 m_flags;
			unsigned __int64 m_lengthbits;
			unsigned int m_hash;
			char m_data[30];
		};

		union HksValue
		{
			cclosure* cClosure;
			void* closure;
			UserData* userData;
			HashTable* table;
			void* tstruct;
			InternString* str;
			void* thread;
			void* ptr;
			float number;
			long long i64;
			unsigned long long ui64;
			unsigned int native;
			bool boolean;
		};

		enum HksObjectType
		{
			TANY = 0xFFFFFFFE,
			TNONE = 0xFFFFFFFF,
			TNIL = 0x0,
			TBOOLEAN = 0x1,
			TLIGHTUSERDATA = 0x2,
			TNUMBER = 0x3,
			TSTRING = 0x4,
			TTABLE = 0x5,
			TFUNCTION = 0x6,  // idk
			TUSERDATA = 0x7,
			TTHREAD = 0x8,
			TIFUNCTION = 0x9, // Lua function
			TCFUNCTION = 0xA, // C function
			TUI64 = 0xB,
			TSTRUCT = 0xC,
			NUM_TYPE_OBJECTS = 0xE,
		};

		struct HksObject
		{
			HksObjectType t;
			HksValue v;
		};

		const struct hksInstruction
		{
			unsigned int code;
		};

		struct ActivationRecord
		{
			HksObject* m_base;
			const hksInstruction* m_returnAddress;
			__int16 m_tailCallDepth;
			__int16 m_numVarargs;
			int m_numExpectedReturns;
		};

		struct CallStack
		{
			ActivationRecord* m_records;
			ActivationRecord* m_lastrecord;
			ActivationRecord* m_current;
			const hksInstruction* m_current_lua_pc;
			const hksInstruction* m_hook_return_addr;
			int m_hook_level;
		};

		struct ApiStack
		{
			HksObject* top;
			HksObject* base;
			HksObject* alloc_top;
			HksObject* bottom;
		};

		struct UpValue : ChunkHeader
		{
			HksObject m_storage;
			HksObject* loc;
			UpValue* m_next;
		};

		struct CallSite
		{
			_SETJMP_FLOAT128 m_jumpBuffer[16];
			CallSite* m_prev;
		};

		enum Status
		{
			NEW = 0x1,
			RUNNING = 0x2,
			YIELDED = 0x3,
			DEAD_ERROR = 0x4,
		};

		enum HksError
		{
			HKS_NO_ERROR = 0x0,
			HKS_ERRSYNTAX = 0xFFFFFFFC,
			HKS_ERRFILE = 0xFFFFFFFB,
			HKS_ERRRUN = 0xFFFFFF9C,
			HKS_ERRMEM = 0xFFFFFF38,
			HKS_ERRERR = 0xFFFFFED4,
			HKS_THROWING_ERROR = 0xFFFFFE0C,
			HKS_GC_YIELD = 0x1,
		};

		struct lua_Debug
		{
			int event;
			const char* name;
			const char* namewhat;
			const char* what;
			const char* source;
			int currentline;
			int nups;
			int nparams;
			int ishksfunc;
			int linedefined;
			int lastlinedefined;
			char short_src[512];
			int callstack_level;
			int is_tail_call;
		};

		using lua_function = int(__fastcall*)(lua_State*);

		struct luaL_Reg
		{
			const char* name;
			lua_function function;
		};

		struct Node
		{
			HksObject m_key;
			HksObject m_value;
		};

		struct Metatable
		{
		};

		struct HashTable : ChunkHeader
		{
			Metatable* m_meta;
			unsigned int m_version;
			unsigned int m_mask;
			Node* m_hashPart;
			HksObject* m_arrayPart;
			unsigned int m_arraySize;
			Node* m_freeNode;
		};

		struct cclosure : ChunkHeader
		{
			lua_function m_function;
			HashTable* m_env;
			__int16 m_numUpvalues;
			__int16 m_flags;
			InternString* m_name;
			HksObject m_upvalues[1];
		};

		enum HksCompilerSettings_BytecodeSharingFormat
		{
			BYTECODE_DEFAULT = 0x0,
			BYTECODE_INPLACE = 0x1,
			BYTECODE_REFERENCED = 0x2,
		};

		enum HksCompilerSettings_IntLiteralOptions
		{
			INT_LITERALS_NONE = 0x0,
			INT_LITERALS_LUD = 0x1,
			INT_LITERALS_32BIT = 0x1,
			INT_LITERALS_UI64 = 0x2,
			INT_LITERALS_64BIT = 0x2,
			INT_LITERALS_ALL = 0x3,
		};

		struct HksCompilerSettings
		{
			int m_emitStructCode;
			const char** m_stripNames;
			int m_emitGlobalMemoization;
			int _m_isHksGlobalMemoTestingMode;
			HksCompilerSettings_BytecodeSharingFormat m_bytecodeSharingFormat;
			HksCompilerSettings_IntLiteralOptions m_enableIntLiterals;
			int(__fastcall* m_debugMap)(const char*, int);
		};

		enum HksBytecodeSharingMode
		{
			HKS_BYTECODE_SHARING_OFF = 0x0,
			HKS_BYTECODE_SHARING_ON = 0x1,
			HKS_BYTECODE_SHARING_SECURE = 0x2,
		};

		struct HksGcWeights
		{
			int m_removeString;
			int m_finalizeUserdataNoMM;
			int m_finalizeUserdataGcMM;
			int m_cleanCoroutine;
			int m_removeWeak;
			int m_markObject;
			int m_traverseString;
			int m_traverseUserdata;
			int m_traverseCoroutine;
			int m_traverseWeakTable;
			int m_freeChunk;
			int m_sweepTraverse;
		};

		struct GarbageCollector_Stack
		{
			void* m_storage;
			unsigned int m_numEntries;
			unsigned int m_numAllocated;
		};

		struct ProtoList
		{
			void** m_protoList;
			unsigned __int16 m_protoSize;
			unsigned __int16 m_protoAllocSize;
		};

		struct GarbageCollector
		{
			int m_target;
			int m_stepsLeft;
			int m_stepLimit;
			HksGcWeights m_costs;
			int m_unit;
			_SETJMP_FLOAT128(*m_jumpPoint)[16];
			lua_State* m_mainState;
			lua_State* m_finalizerState;
			void* m_memory;
			int m_phase;
			GarbageCollector_Stack m_resumeStack;
			GarbageCollector_Stack m_greyStack;
			GarbageCollector_Stack m_remarkStack;
			GarbageCollector_Stack m_weakStack;
			int m_finalizing;
			HksObject m_safeTableValue;
			lua_State* m_startOfStateStackList;
			lua_State* m_endOfStateStackList;
			lua_State* m_currentState;
			HksObject m_safeValue;
			void* m_compiler;
			void* m_bytecodeReader;
			void* m_bytecodeWriter;
			int m_pauseMultiplier;
			int m_stepMultiplier;
			bool m_stopped;
			int(__fastcall* m_gcPolicy)(lua_State*);
			unsigned __int64 m_pauseTriggerMemoryUsage;
			int m_stepTriggerCountdown;
			unsigned int m_stringTableIndex;
			unsigned int m_stringTableSize;
			UserData* m_lastBlackUD;
			UserData* m_activeUD;
		};

		enum MemoryManager_ChunkColor
		{
			RED = 0x0,
			BLACK = 0x1,
		};

		struct ChunkList
		{
			ChunkHeader m_prevToStart;
		};

		enum Hks_DeleteCheckingMode
		{
			HKS_DELETE_CHECKING_OFF = 0x0,
			HKS_DELETE_CHECKING_ACCURATE = 0x1,
			HKS_DELETE_CHECKING_SAFE = 0x2,
		};

		struct MemoryManager
		{
			void* (__fastcall* m_allocator)(void*, void*, unsigned __int64, unsigned __int64);
			void* m_allocatorUd;
			MemoryManager_ChunkColor m_chunkColor;
			unsigned __int64 m_used;
			unsigned __int64 m_highwatermark;
			ChunkList m_allocationList;
			ChunkList m_sweepList;
			ChunkHeader* m_lastKeptChunk;
			lua_State* m_state;
			ChunkList m_deletedList;
			int m_deleteMode;
			Hks_DeleteCheckingMode m_deleteCheckingMode;
		};

		struct StaticStringCache
		{
			HksObject m_objects[41];
		};

		enum HksBytecodeEndianness
		{
			HKS_BYTECODE_DEFAULT_ENDIAN = 0x0,
			HKS_BYTECODE_BIG_ENDIAN = 0x1,
			HKS_BYTECODE_LITTLE_ENDIAN = 0x2,
		};

		struct RuntimeProfileData_Stats
		{
			unsigned __int64 hksTime;
			unsigned __int64 callbackTime;
			unsigned __int64 gcTime;
			unsigned __int64 cFinalizerTime;
			unsigned __int64 compilerTime;
			unsigned int hkssTimeSamples;
			unsigned int callbackTimeSamples;
			unsigned int gcTimeSamples;
			unsigned int compilerTimeSamples;
			unsigned int num_newuserdata;
			unsigned int num_tablerehash;
			unsigned int num_pushstring;
			unsigned int num_pushcfunction;
			unsigned int num_newtables;
		};

		struct RuntimeProfileData
		{
			__int64 stackDepth;
			__int64 callbackDepth;
			unsigned __int64 lastTimer;
			RuntimeProfileData_Stats frameStats;
			unsigned __int64 gcStartTime;
			unsigned __int64 finalizerStartTime;
			unsigned __int64 compilerStartTime;
			unsigned __int64 compilerStartGCTime;
			unsigned __int64 compilerStartGCFinalizerTime;
			unsigned __int64 compilerCallbackStartTime;
			__int64 compilerDepth;
			void* outFile;
			lua_State* rootState;
		};

		struct HksGlobal
		{
			MemoryManager m_memory;
			GarbageCollector m_collector;
			StringTable m_stringTable;
			HksBytecodeSharingMode m_bytecodeSharingMode;
			unsigned int m_tableVersionInitializer;
			HksObject m_registry;
			ProtoList m_protoList;
			HashTable* m_structProtoByName;
			ChunkList m_userDataList;
			lua_State* m_root;
			StaticStringCache m_staticStringCache;
			void* m_debugger;
			void* m_profiler;
			RuntimeProfileData m_runProfilerData;
			HksCompilerSettings m_compilerSettings;
			int(__fastcall* m_panicFunction)(lua_State*);
			void* m_luaplusObjectList;
			int m_heapAssertionFrequency;
			int m_heapAssertionCount;
			void (*m_logFunction)(lua_State*, const char*, ...);
			HksBytecodeEndianness m_bytecodeDumpEndianness;
		};

		struct lua_State : ChunkHeader
		{
			HksGlobal* m_global;
			CallStack m_callStack;
			ApiStack m_apistack;
			UpValue* pending;
			HksObject globals;
			HksObject m_cEnv;
			CallSite* m_callsites;
			int m_numberOfCCalls;
			void* m_context;
			InternString* m_name;
			lua_State* m_nextState;
			lua_State* m_nextStateStack;
			Status m_status;
			HksError m_error;
		};
	}

	struct ClientVoicePacket_t
	{
		char data[256];
		int dataSize;
	};

	struct voiceCommunication_t
	{
		ClientVoicePacket_t voicePackets[10];	// 0
		int voicePacketCount;					// 2600
		int voicePacketLastTransmit;			// 2604
		int packetsPerSec;
		int packetsPerSecStart;
	};

	struct VoicePacket_t
	{
		char talker;
		char data[256];
		int dataSize;
	};

	enum clientState_t
	{
		CS_FREE = 0x0,
		CS_ZOMBIE = 0x1,
		CS_RECONNECTING = 0x2,
		CS_CONNECTED = 0x3,
		CS_CLIENTLOADING = 0x4,
		CS_ACTIVE = 0x5,
	};

	struct netProfilePacket_t
	{
		int iTime;
		int iSize;
		int bFragment;
	};

	struct netProfileStream_t
	{
		netProfilePacket_t packets[60];
		int iCurrPacket;
		int iBytesPerSecond;
		int iLastBPSCalcTime;
		int iCountedPackets;
		int iCountedFragments;
		int iFragmentPercentage;
		int iLargestPacket;
		int iSmallestPacket;
	};

	struct netProfileInfo_t
	{
		netProfileStream_t send;
		netProfileStream_t recieve;
	};

	struct netchan_t
	{
		int outgoingSequence;
		netsrc_t sock;
		int dropped;
		int incomingSequence;
		netadr_s remoteAddress;
		int fragmentSequence;
		int fragmentLength;
		char* fragmentBuffer;
		int fragmentBufferSize;
		int unsentFragments;
		int unsentFragmentStart;
		int unsentLength;
		char* unsentBuffer;
		int unsentBufferSize;
		netProfileInfo_t prof;
	};

	struct clientConnection_t
	{
		int qport;
		int clientNum;
		int lastPacketTime;
		netadr_s serverAddress;
		int connectLastSendTime;
		int connectPacketCount;
		char serverMessage[256];
		int challenge;
		int checksumFeed;
		int reliableSequence;
		int reliableAcknowledge;
		char reliableCommands[128][1024];
		int serverMessageSequence;
		int serverCommandSequence;
		int lastExecutedServerCommand;
		char serverCommands[128][1024];
		bool isServerRestarting;
		netchan_t netchan;
		char netchanOutgoingBuffer[2048];
		char netchanIncomingBuffer[131072];
		netProfileInfo_t OOBProf;
		unsigned int statPacketsToSend;
		int statPacketSendTime[31];
		unsigned int currentGamestatePacket;
	}; static_assert(offsetof(clientConnection_t, reliableSequence) == 0x130, "not at this offset");

	struct infoParm_t
	{
		const char* name;
		int clearSolid;
		int surfaceFlags;
		int contents;
		int toolFlags;
	};

	struct fake_centity_s
	{
		int number;
		char _pad0[3732];
		float alias1;
		float alias2;
		float alias3;
	};

	assert_offsetof(fake_centity_s, alias1, 3736);
	assert_offsetof(fake_centity_s, alias2, 3740);
	assert_offsetof(fake_centity_s, alias3, 3744);

	struct fake_entlist
	{
		char _pad0[7608];
		int perk;
	}; assert_offsetof(fake_entlist, perk, 7608);

	union OmnvarValue
	{
		bool enabled;
		int integer;
		unsigned int time;
		float value;
		unsigned int ncsString;
	};

	enum OmnvarType
	{
		OMNVAR_TYPE_INVALID = 0x0,
		OMNVAR_TYPE_BOOL = 0x1,
		OMNVAR_TYPE_FLOAT = 0x2,
		OMNVAR_TYPE_INT = 0x3,
		OMNVAR_TYPE_TIME = 0x4,
		OMNVAR_TYPE_NCS_LUI = 0x5,
	};

	struct __declspec(align(8)) OmnvarDef
	{
		const char* name;
		char flags;
		OmnvarType type;
		OmnvarValue initial;
		const char* ncsString;
		int offset;
		int numbits;
		int minvalue;
		int maxvalue;
		int row;
	};

	struct OmnvarData
	{
		unsigned int timeModified;
		OmnvarValue current;
	};

	enum weapAnimFiles_t : std::int32_t
	{
		WEAP_ANIM_INVALID = -1,
		WEAP_ANIM_ROOT = 0,
		WEAP_ANIM_IDLE = 1,
		WEAP_ANIM_EMPTY_IDLE = 2,
		WEAP_ANIM_FIRE = 3,
		WEAP_ANIM_HOLD_FIRE = 4,
		WEAP_ANIM_LASTSHOT = 5,
		WEAP_ANIM_RECHAMBER = 6,
		WEAP_ANIM_GRENADE_PRIME = 7,
		WEAP_ANIM_GRENADE_PRIME_READY_TO_THROW = 8,
		WEAP_ANIM_MELEE_SWIPE = 9,
		WEAP_ANIM_MELEE_HIT = 10,
		WEAP_ANIM_MELEE_FATAL = 11,
		WEAP_ANIM_MELEE_MISS = 12,
		WEAP_ANIM_MELEE_VICTIM_CROUCHING_HIT = 13,
		WEAP_ANIM_MELEE_VICTIM_CROUCHING_FATAL = 14,
		WEAP_ANIM_MELEE_VICTIM_CROUCHING_MISS = 15,
		WEAP_ANIM_MELEE_ALT_STANDING = 16,
		WEAP_ANIM_MELEE_ALT_CROUCHING = 17,
		WEAP_ANIM_MELEE_ALT_PRONE = 18,
		WEAP_ANIM_MELEE_ALT_JUMPING = 19,
		WEAP_ANIM_MELEE_ALT_STANDING_VICTIM_CROUCHING = 20,
		WEAP_ANIM_MELEE_ALT_CROUCHING_VICTIM_CROUCHING = 21,
		WEAP_ANIM_MELEE_ALT_PRONE_VICTIM_CROUCHING = 22,
		WEAP_ANIM_MELEE_ALT_JUMPING_VICTIM_CROUCHING = 23,
		WEAP_ANIM_RELOAD = 24,
		WEAP_ANIM_RELOAD_EMPTY = 25,
		WEAP_ANIM_RELOAD_START = 26,
		WEAP_ANIM_RELOAD_END = 27,
		WEAP_ANIM_FAST_RELOAD = 28,
		WEAP_ANIM_FAST_RELOAD_EMPTY = 29,
		WEAP_ANIM_FAST_RELOAD_START = 30,
		WEAP_ANIM_FAST_RELOAD_END = 31,
		WEAP_ANIM_DUALMAG_RELOAD = 32,
		WEAP_ANIM_DUALMAG_RELOAD_EMPTY = 33,
		WEAP_ANIM_SPEED_RELOAD = 34,
		WEAP_ANIM_RELOAD_FROM_ALT = 35,
		WEAP_ANIM_RAISE = 36,
		WEAP_ANIM_FIRST_RAISE = 37,
		WEAP_ANIM_BREACH_RAISE = 38,
		WEAP_ANIM_DROP = 39,
		WEAP_ANIM_ALT_RAISE = 40,
		WEAP_ANIM_ALT_DROP = 41,
		WEAP_ANIM_ALT_OVERRIDE = 42,
		WEAP_ANIM_QUICK_RAISE = 43,
		WEAP_ANIM_QUICK_DROP = 44,
		WEAP_ANIM_EMPTY_RAISE = 45,
		WEAP_ANIM_EMPTY_DROP = 46,
		WEAP_ANIM_HYBRID_SIGHT_ON = 47,
		WEAP_ANIM_HYBRID_SIGHT_OFF = 48,
		WEAP_ANIM_SPRINT_IN = 49,
		WEAP_ANIM_SPRINT_IN_FROM_SLIDE = 50,
		WEAP_ANIM_SPRINT_IN_CANCEL = 51,
		WEAP_ANIM_SPRINT_LOOP = 52,
		WEAP_ANIM_SPRINT_OUT = 53,
		WEAP_ANIM_SPRINTANDFIRE_IN = 54,
		WEAP_ANIM_SPRINTANDFIRE_LOOP = 55,
		WEAP_ANIM_SPRINTANDFIRE_OUT = 56,
		WEAP_ANIM_STUNNED_START = 57,
		WEAP_ANIM_STUNNED_LOOP = 58,
		WEAP_ANIM_STUNNED_END = 59,
		WEAP_ANIM_THROWBACK = 60,
		WEAP_ANIM_DETONATE = 61,
		WEAP_ANIM_NIGHTVISION_WEAR = 62,
		WEAP_ANIM_NIGHTVISION_REMOVE = 63,
		WEAP_ANIM_ADS_FIRE = 64,
		WEAP_ANIM_ADS_LASTSHOT = 65,
		WEAP_ANIM_ADS_RECHAMBER = 66,
		WEAP_ANIM_BLAST_FRONT = 67,
		WEAP_ANIM_BLAST_RIGHT = 68,
		WEAP_ANIM_BLAST_BACK = 69,
		WEAP_ANIM_BLAST_LEFT = 70,
		WEAP_ANIM_SLIDE_IN = 71,
		WEAP_ANIM_SLIDE_LOOP = 72,
		WEAP_ANIM_SLIDE_OUT_TO_SPRINT = 73,
		WEAP_ANIM_SLIDE_OUT = 74,
		WEAP_ANIM_SLIDE_AND_FIRE_IN = 75,
		WEAP_ANIM_SLIDE_AND_FIRE_LOOP = 76,
		WEAP_ANIM_SLIDE_AND_FIRE_OUT = 77,
		WEAP_ANIM_HIGH_JUMP_IN = 78,
		WEAP_ANIM_HIGH_JUMP_DROP_IN = 79,
		WEAP_ANIM_HIGH_JUMP_DROP_LOOP = 80,
		WEAP_ANIM_HIGH_JUMP_DROP_LAND = 81,
		WEAP_ANIM_DODGE_GROUND_BACK = 82,
		WEAP_ANIM_DODGE_GROUND_LEFT = 83,
		WEAP_ANIM_DODGE_GROUND_RIGHT = 84,
		WEAP_ANIM_DODGE_AIR_FORWARD = 85,
		WEAP_ANIM_DODGE_AIR_BACK = 86,
		WEAP_ANIM_DODGE_AIR_LEFT = 87,
		WEAP_ANIM_DODGE_AIR_RIGHT = 88,
		WEAP_ANIM_LAND_DIP = 89,
		WEAP_ANIM_RECOIL_SETTLE = 90,
		WEAP_ANIM_SWIM_LOOP = 91,
		WEAP_ANIM_MANTLE_UP_64 = 92,
		WEAP_ANIM_MANTLE_UP_56 = 93,
		WEAP_ANIM_MANTLE_UP_48 = 94,
		WEAP_ANIM_MANTLE_UP_40 = 95,
		WEAP_ANIM_MANTLE_UP_32 = 96,
		WEAP_ANIM_MANTLE_UP_24 = 97,
		WEAP_ANIM_MANTLE_UP_16 = 98,
		WEAP_ANIM_MANTLE_OVER_64 = 99,
		WEAP_ANIM_MANTLE_OVER_56 = 100,
		WEAP_ANIM_MANTLE_OVER_48 = 101,
		WEAP_ANIM_MANTLE_OVER_40 = 102,
		WEAP_ANIM_MANTLE_OVER_32 = 103,
		WEAP_ANIM_MANTLE_OVER_24 = 104,
		WEAP_ANIM_MANTLE_OVER_16 = 105,
		WEAP_ANIM_GOLIATH_ENTRY = 106,
		WEAP_ANIM_OFFHAND_SWITCH = 107,
		WEAP_ANIM_HEAT_COOLDOWN_IN = 108,
		WEAP_ANIM_HEAT_COOLDOWN_OUT = 109,
		WEAP_ANIM_OVERHEAT_OUT = 110,
		WEAP_ANIM_SCRIPTED = 111,
		WEAP_ANIM_INSPECTION = 112,
		WEAP_ANIM_RELOAD_MULTIPLE_1 = 113,
		WEAP_ANIM_RELOAD_MULTIPLE_2 = 114,
		WEAP_ANIM_RELOAD_MULTIPLE_3 = 115,
		WEAP_ANIM_RELOAD_MULTIPLE_4 = 116,
		WEAP_ANIM_RELOAD_MULTIPLE_5 = 117,
		WEAP_ANIM_RELOAD_MULTIPLE_6 = 118,
		WEAP_ANIM_RELOAD_MULTIPLE_7 = 119,
		WEAP_ANIM_RELOAD_MULTIPLE_8 = 120,
		WEAP_ANIM_RELOAD_MULTIPLE_FAST_1 = 121,
		WEAP_ANIM_RELOAD_MULTIPLE_FAST_2 = 122,
		WEAP_ANIM_RELOAD_MULTIPLE_FAST_3 = 123,
		WEAP_ANIM_RELOAD_MULTIPLE_FAST_4 = 124,
		WEAP_ANIM_RELOAD_MULTIPLE_FAST_5 = 125,
		WEAP_ANIM_RELOAD_MULTIPLE_FAST_6 = 126,
		WEAP_ANIM_RELOAD_MULTIPLE_FAST_7 = 127,
		WEAP_ANIM_RELOAD_MULTIPLE_FAST_8 = 128,
		WEAP_ANIM_ADS_UP = 129,
		WEAP_ANIM_ADS_DOWN = 130,
		WEAP_ANIM_RECOIL = 131,
		WEAP_ALT_ANIM_ADJUST = 132,
		WEAP_ANIM_ADDITIVE_ADS_ROOT = 133,
		WEAP_ANIM_ADDITIVE_ADS_UP = 134,
		WEAP_ANIM_ADDITIVE_HYBRID_SIGHT_UP_ROOT = 135,
		WEAP_ANIM_ADDITIVE_HYBRID_SIGHT_UP = 136,
		WEAP_ANIM_ADDITIVE_DRAG_LEFT_ROOT = 137,
		WEAP_ANIM_ADDITIVE_DRAG_LEFT = 138,
		WEAP_ANIM_ADDITIVE_DRAG_RIGHT_ROOT = 139,
		WEAP_ANIM_ADDITIVE_DRAG_RIGHT = 140,
		WEAP_ANIM_ADDITIVE_DRAG_UP_ROOT = 141,
		WEAP_ANIM_ADDITIVE_DRAG_UP = 142,
		WEAP_ANIM_ADDITIVE_DRAG_DOWN_ROOT = 143,
		WEAP_ANIM_ADDITIVE_DRAG_DOWN = 144,
		WEAP_ANIM_ADDITIVE_SWIM_FORWARD_ROOT = 145,
		WEAP_ANIM_ADDITIVE_SWIM_FORWARD = 146,
		WEAP_ANIM_ADDITIVE_SWIM_BACKWARD_ROOT = 147,
		WEAP_ANIM_ADDITIVE_SWIM_BACKWARD = 148,
		WEAP_ANIM_ADDITIVE_JUMP_ROOT = 149,
		WEAP_ANIM_ADDITIVE_JUMP = 150,
		WEAP_ANIM_ADDITIVE_JUMP_BOOST = 151,
		WEAP_ANIM_ADDITIVE_JUMP_LAND_ROOT = 152,
		WEAP_ANIM_ADDITIVE_JUMP_LAND = 153,
		WEAP_ANIM_ADDITIVE_JUMP_LAND_HEAVY = 154,
		WEAP_ANIM_ADDITIVE_WALK_ROOT = 155,
		WEAP_ANIM_ADDITIVE_WALK = 156,
		WEAP_ANIM_ADDITIVE_HEAT_COOLDOWN_LOOP_ROOT = 157,
		WEAP_ANIM_ADDITIVE_HEAT_COOLDOWN_LOOP = 158,
		WEAP_ANIM_ADDITIVE_OVERHEAT_IN_ROOT = 159,
		WEAP_ANIM_ADDITIVE_OVERHEAT_IN = 160,
		WEAP_ANIM_ADDITIVE_OVERHEAT_LOOP_ROOT = 161,
		WEAP_ANIM_ADDITIVE_OVERHEAT_LOOP = 162,
		WEAP_ANIM_ADDITIVE_CRAWL_IN_ROOT = 163,
		WEAP_ANIM_ADDITIVE_CRAWL_IN = 164,
		WEAP_ANIM_ADDITIVE_CRAWL_LOOP_ROOT = 165,
		WEAP_ANIM_ADDITIVE_CRAWL_LOOP = 166,
		WEAP_ANIM_ADDITIVE_CRAWL_LOOP_BACK_ROOT = 167,
		WEAP_ANIM_ADDITIVE_CRAWL_LOOP_BACK = 168,
		WEAP_ANIM_ADDITIVE_CRAWL_LOOP_LEFT_ROOT = 169,
		WEAP_ANIM_ADDITIVE_CRAWL_LOOP_LEFT = 170,
		WEAP_ANIM_ADDITIVE_CRAWL_LOOP_RIGHT_ROOT = 171,
		WEAP_ANIM_ADDITIVE_CRAWL_LOOP_RIGHT = 172,
		WEAP_ANIM_ADDITIVE_PRONE_DROP_ROOT = 173,
		WEAP_ANIM_ADDITIVE_PRONE_DROP = 174,
		WEAP_ANIM_ADDITIVE_EMPTY_ROOT = 175,
		WEAP_ANIM_ADDITIVE_EMPTY = 176,
		WEAP_ANIM_ADDITIVE_MANTLE_ROOT = 177,
		WEAP_ANIM_ADDITIVE_MANTLE = 178,
		WEAP_ANIM_ADDITIVE_LOW_MANTLE_ROOT = 179,
		WEAP_ANIM_ADDITIVE_MANTLE_UP_24 = 180,
		WEAP_ANIM_ADDITIVE_MANTLE_UP_16 = 181,
		WEAP_ANIM_ADDITIVE_MANTLE_OVER_24 = 182,
		WEAP_ANIM_ADDITIVE_MANTLE_OVER_16 = 183,
		WEAP_ANIM_ADDITIVE_SHOT_CHARGE_IN_ROOT = 184,
		WEAP_ANIM_ADDITIVE_SHOT_CHARGE_IN = 185,
		WEAP_ANIM_ADDITIVE_SHOT_CHARGE_LOOP_ROOT = 186,
		WEAP_ANIM_ADDITIVE_SHOT_CHARGE_LOOP = 187,
		WEAP_ANIM_ADDITIVE_SCRIPTED_ROOT = 188,
		WEAP_ANIM_ADDITIVE_SCRIPTED = 189,
		NUM_WEAP_ANIMS = 190,
	};

	enum CustomizationType
	{
		GENDER = 0,
		SHIRT,
		HEAD,
		GLOVES
	};

	// pulled from IW4
	struct DrawClipAmmoParams
	{
		Material* image;
	};

	union SoundFlags
	{
		struct
		{
			std::uint32_t looping : 1;
			std::uint32_t master : 1;
			std::uint32_t slave : 1;
			std::uint32_t full_dry_level : 1;
			std::uint32_t no_wet_level : 1;
			std::uint32_t is_3d : 1;
			std::uint32_t channel : 10;
			std::uint32_t type : 2;
			std::uint32_t unk2 : 14; // shape? random_looping?
		} u;

		std::uint32_t packed;
	};
}
