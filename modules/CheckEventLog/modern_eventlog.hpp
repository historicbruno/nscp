#pragma once

#include <handle.hpp>
#include <Windows.h>
#include <sal.h>

namespace eventlog {
	namespace api {
		typedef HANDLE EVT_HANDLE, *PEVT_HANDLE;

		typedef enum _EVT_VARIANT_TYPE
		{
			EvtVarTypeNull        = 0,
			EvtVarTypeString      = 1,
			EvtVarTypeAnsiString  = 2,
			EvtVarTypeSByte       = 3,
			EvtVarTypeByte        = 4,
			EvtVarTypeInt16       = 5,
			EvtVarTypeUInt16      = 6,
			EvtVarTypeInt32       = 7,
			EvtVarTypeUInt32      = 8,
			EvtVarTypeInt64       = 9,
			EvtVarTypeUInt64      = 10,
			EvtVarTypeSingle      = 11,
			EvtVarTypeDouble      = 12,
			EvtVarTypeBoolean     = 13,
			EvtVarTypeBinary      = 14,
			EvtVarTypeGuid        = 15,
			EvtVarTypeSizeT       = 16,
			EvtVarTypeFileTime    = 17,
			EvtVarTypeSysTime     = 18,
			EvtVarTypeSid         = 19,
			EvtVarTypeHexInt32    = 20,
			EvtVarTypeHexInt64    = 21,

			// these types used internally
			EvtVarTypeEvtHandle   = 32,
			EvtVarTypeEvtXml      = 35

		} EVT_VARIANT_TYPE;


#define EVT_VARIANT_TYPE_MASK 0x7f
#define EVT_VARIANT_TYPE_ARRAY 128


		typedef struct _EVT_VARIANT
		{
			union
			{
				BOOL        BooleanVal;
				INT8        SByteVal;
				INT16       Int16Val;
				INT32       Int32Val;
				INT64       Int64Val;
				UINT8       ByteVal;
				UINT16      UInt16Val;
				UINT32      UInt32Val;
				UINT64      UInt64Val;
				float       SingleVal;
				double      DoubleVal;
				ULONGLONG   FileTimeVal;
				SYSTEMTIME* SysTimeVal;
				GUID*       GuidVal;
				LPCWSTR     StringVal;
				LPCSTR      AnsiStringVal;
				PBYTE       BinaryVal;
				PSID        SidVal;
				size_t      SizeTVal;

				// array fields
				BOOL*       BooleanArr;
				INT8*       SByteArr;
				INT16*      Int16Arr;
				INT32*      Int32Arr;
				INT64*      Int64Arr;
				UINT8*      ByteArr;
				UINT16*     UInt16Arr;
				UINT32*     UInt32Arr;
				UINT64*     UInt64Arr;
				float*      SingleArr;
				double*     DoubleArr;
				FILETIME*   FileTimeArr;
				SYSTEMTIME* SysTimeArr;
				GUID*       GuidArr;
				LPWSTR*     StringArr;
				LPSTR*      AnsiStringArr;
				PSID*       SidArr;
				size_t*     SizeTArr;

				// internal fields
				EVT_HANDLE  EvtHandleVal;
				LPCWSTR     XmlVal;
				LPCWSTR*    XmlValArr;
			};

			DWORD Count;   // number of elements (not length) in bytes.
			DWORD Type;

		} EVT_VARIANT, *PEVT_VARIANT;


		typedef enum _EVT_QUERY_FLAGS
		{
			EvtQueryChannelPath                 = 0x1,
			EvtQueryFilePath                    = 0x2,

			EvtQueryForwardDirection            = 0x100,
			EvtQueryReverseDirection            = 0x200,

			EvtQueryTolerateQueryErrors         = 0x1000

		} EVT_QUERY_FLAGS;

		typedef enum _EVT_SEEK_FLAGS
		{
			EvtSeekRelativeToFirst    = 1,
			EvtSeekRelativeToLast     = 2,
			EvtSeekRelativeToCurrent  = 3,
			EvtSeekRelativeToBookmark = 4,
			EvtSeekOriginMask         = 7,

			EvtSeekStrict             = 0x10000,

		} EVT_SEEK_FLAGS;

		typedef enum _EVT_SYSTEM_PROPERTY_ID
		{
			EvtSystemProviderName = 0,          // EvtVarTypeString             
			EvtSystemProviderGuid,              // EvtVarTypeGuid  
			EvtSystemEventID,                   // EvtVarTypeUInt16  
			EvtSystemQualifiers,                // EvtVarTypeUInt16
			EvtSystemLevel,                     // EvtVarTypeUInt8
			EvtSystemTask,                      // EvtVarTypeUInt16
			EvtSystemOpcode,                    // EvtVarTypeUInt8
			EvtSystemKeywords,                  // EvtVarTypeHexInt64
			EvtSystemTimeCreated,               // EvtVarTypeFileTime
			EvtSystemEventRecordId,             // EvtVarTypeUInt64
			EvtSystemActivityID,                // EvtVarTypeGuid
			EvtSystemRelatedActivityID,         // EvtVarTypeGuid
			EvtSystemProcessID,                 // EvtVarTypeUInt32
			EvtSystemThreadID,                  // EvtVarTypeUInt32
			EvtSystemChannel,                   // EvtVarTypeString 
			EvtSystemComputer,                  // EvtVarTypeString 
			EvtSystemUserID,                    // EvtVarTypeSid
			EvtSystemVersion,                   // EvtVarTypeUInt8
			EvtSystemPropertyIdEND

		} EVT_SYSTEM_PROPERTY_ID;

		typedef enum _EVT_RENDER_CONTEXT_FLAGS
		{
			EvtRenderContextValues = 0,         // Render specific properties
			EvtRenderContextSystem,             // Render all system properties (System)
			EvtRenderContextUser                // Render all user properties (User/EventData)

		} EVT_RENDER_CONTEXT_FLAGS;

		typedef enum _EVT_RENDER_FLAGS
		{
			EvtRenderEventValues = 0,           // Variants
			EvtRenderEventXml,                  // XML
			EvtRenderBookmark                   // Bookmark

		} EVT_RENDER_FLAGS;

		typedef enum _EVT_FORMAT_MESSAGE_FLAGS
		{
			EvtFormatMessageEvent = 1,
			EvtFormatMessageLevel,
			EvtFormatMessageTask,
			EvtFormatMessageOpcode,
			EvtFormatMessageKeyword,
			EvtFormatMessageChannel, 
			EvtFormatMessageProvider, 
			EvtFormatMessageId,
			EvtFormatMessageXml,

		} EVT_FORMAT_MESSAGE_FLAGS;

		typedef EVT_HANDLE (WINAPI *tEvtOpenPublisherEnum)(
			EVT_HANDLE Session,
			DWORD Flags
			);

		typedef BOOL (WINAPI *tEvtNextPublisherId)(
			EVT_HANDLE PublisherEnum,
			DWORD PublisherIdBufferSize,
			__out_ecount_part_opt(PublisherIdBufferSize, *PublisherIdBufferUsed)
			LPWSTR PublisherIdBuffer,
			_Out_ PDWORD PublisherIdBufferUsed
			);

		typedef EVT_HANDLE (WINAPI *tEvtOpenChannelEnum)(
			_In_  EVT_HANDLE Session,
			_In_  DWORD Flags
			);
		typedef BOOL (WINAPI *tEvtNextChannelPath)(
			_In_   EVT_HANDLE ChannelEnum,
			_In_   DWORD ChannelPathBufferSize,
			_In_   LPWSTR ChannelPathBuffer,
			_Out_  PDWORD ChannelPathBufferUsed
			);
		typedef BOOL (WINAPI *tEvtClose)(
			_In_  EVT_HANDLE Object
			);
		typedef EVT_HANDLE (WINAPI *tEvtQuery)(
			EVT_HANDLE Session,
			LPCWSTR Path,
			LPCWSTR Query,
			DWORD Flags
			);
		typedef BOOL (WINAPI *tEvtNext)(
			EVT_HANDLE ResultSet,
			DWORD EventsSize,
			PEVT_HANDLE Events,
			DWORD Timeout,
			DWORD Flags,
			_Out_ PDWORD Returned
			);

		typedef BOOL (WINAPI *tEvtSeek)(
			EVT_HANDLE ResultSet,
			LONGLONG Position,
			EVT_HANDLE Bookmark,
			__reserved DWORD Timeout,           // currently must be 0
			DWORD Flags
			);

		typedef EVT_HANDLE (WINAPI *tEvtCreateRenderContext)(
			DWORD ValuePathsCount,
			LPCWSTR* ValuePaths,
			DWORD Flags                         // EVT_RENDER_CONTEXT_FLAGS
			);

		typedef BOOL (WINAPI *tEvtRender)(
			EVT_HANDLE Context,
			EVT_HANDLE Fragment,
			DWORD Flags,                        // EVT_RENDER_FLAGS
			DWORD BufferSize,
			__out_bcount_part_opt(BufferSize, *BufferUsed) PVOID Buffer,
			_Out_ PDWORD BufferUsed,
			_Out_ PDWORD PropertyCount
			);

		typedef EVT_HANDLE (WINAPI *tEvtOpenPublisherMetadata)(
			EVT_HANDLE Session,
			LPCWSTR PublisherId,
			LPCWSTR LogFilePath,
			LCID Locale,
			DWORD Flags
			);

		typedef BOOL (WINAPI *tEvtFormatMessage)(
			EVT_HANDLE PublisherMetadata,       // Except for forwarded events
			EVT_HANDLE Event,
			DWORD MessageId,
			DWORD ValueCount,
			PEVT_VARIANT Values,
			DWORD Flags,
			DWORD BufferSize,
			__out_ecount_part_opt(BufferSize, *BufferUsed) LPWSTR Buffer,
			_Out_ PDWORD BufferUsed
			);

		void load_procs();
		bool supports_modern();
	}
	BOOL EvtFormatMessage(api::EVT_HANDLE PublisherMetadata, api::EVT_HANDLE Event, DWORD MessageId, DWORD ValueCount, api::PEVT_VARIANT Values, DWORD Flags, DWORD BufferSize, LPWSTR Buffer, PDWORD BufferUsed);
	api::EVT_HANDLE EvtOpenPublisherMetadata(api::EVT_HANDLE Session, LPCWSTR PublisherId, LPCWSTR LogFilePath, LCID Locale, DWORD Flags);
	api::EVT_HANDLE EvtCreateRenderContext(DWORD ValuePathsCount, LPCWSTR* ValuePaths, DWORD Flags);
	BOOL EvtRender(api::EVT_HANDLE Context, api::EVT_HANDLE Fragment, DWORD Flags, DWORD BufferSize, PVOID Buffer, PDWORD BufferUsed, PDWORD PropertyCount);
	BOOL EvtNext(api::EVT_HANDLE ResultSet, DWORD EventsSize, api::PEVT_HANDLE Events, DWORD Timeout, DWORD Flags, PDWORD Returned);
	BOOL EvtSeek(api::EVT_HANDLE ResultSet, LONGLONG Position, api::EVT_HANDLE Bookmark, DWORD Timeout, DWORD Flags);
	api::EVT_HANDLE EvtQuery(api::EVT_HANDLE Session, LPCWSTR Path, LPCWSTR Query, DWORD Flags);
	api::EVT_HANDLE EvtOpenPublisherEnum(api::EVT_HANDLE Session, DWORD Flags);
	BOOL EvtClose(api::EVT_HANDLE Object);
	BOOL EvtNextPublisherId(api::EVT_HANDLE PublisherEnum, DWORD PublisherIdBufferSize, LPWSTR PublisherIdBuffer, PDWORD PublisherIdBufferUsed);
	api::EVT_HANDLE EvtOpenChannelEnum(api::EVT_HANDLE Session, DWORD Flags);
	BOOL EvtNextChannelPath(api::EVT_HANDLE PublisherEnum, DWORD PublisherIdBufferSize, LPWSTR PublisherIdBuffer, PDWORD PublisherIdBufferUsed);

	struct evt_closer {
		static void close(api::EVT_HANDLE &handle) {
			EvtClose(handle);
		}
	};
	typedef hlp::handle<api::EVT_HANDLE, evt_closer> evt_handle;
}