///////////////////////////////////////////////////////////////////////////
// NSClient++ Base Service
// 
// Copyright (c) 2004 MySolutions NORDIC (http://www.medin.name)
//
// Date: 2004-03-13
// Author: Michael Medin (michael@medin.name)
//
// Part of this file is based on work by Bruno Vais (bvais@usa.net)
//
// This software is provided "AS IS", without a warranty of any kind.
// You are free to use/modify this code but leave this header intact.
//
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "NSClient++.h"
#include "core_api.h"
#include <charEx.h>
#include <string.h>
#include <settings/settings_core.hpp>
#include "../helpers/settings_manager/settings_manager_impl.h"
#include <nscapi/nscapi_helper.hpp>
#ifdef _WIN32
#include <ServiceCmd.h>
#endif
#include <nsclient/logger.hpp>

using namespace nscp::helpers;

#define LOG_ERROR_STD(msg) LOG_ERROR(((std::string)msg).c_str())
#define LOG_CRITICAL_STD(msg) LOG_CRITICAL(((std::string)msg).c_str())
#define LOG_MESSAGE_STD(msg) LOG_MESSAGE(((std::string)msg).c_str())
#define LOG_DEBUG_STD(msg) LOG_DEBUG(((std::string)msg).c_str())

#define LOG_ERROR(msg) { nsclient::logging::logger::get_logger()->error("core", __FILE__, __LINE__, msg); }
#define LOG_CRITICAL(msg) { nsclient::logging::logger::get_logger()->error("core", __FILE__, __LINE__, msg); }
#define LOG_MESSAGE(msg) { nsclient::logging::logger::get_logger()->info("core", __FILE__, __LINE__, msg); }
#define LOG_DEBUG(msg) { nsclient::logging::logger::get_logger()->debug("core", __FILE__, __LINE__, msg); }

NSCAPI::errorReturn NSAPIExpandPath(const char* key, char* buffer,unsigned int bufLen) {
	try {
		return nscapi::plugin_helper::wrapReturnString(buffer, bufLen, mainClient.expand_path(key), NSCAPI::isSuccess);
	} catch (...) {
		LOG_ERROR_STD("Failed to getString: " + utf8::cvt<std::string>(key));
		return NSCAPI::hasFailed;
	}
}

NSCAPI::errorReturn NSAPIGetApplicationName(char *buffer, unsigned int bufLen) {
	return nscapi::plugin_helper::wrapReturnString(buffer, bufLen, utf8::cvt<std::string>(APPLICATION_NAME), NSCAPI::isSuccess);
}
NSCAPI::errorReturn NSAPIGetApplicationVersionStr(char *buffer, unsigned int bufLen) {
	return nscapi::plugin_helper::wrapReturnString(buffer, bufLen, utf8::cvt<std::string>(CURRENT_SERVICE_VERSION), NSCAPI::isSuccess);
}
void NSAPISimpleMessage(const char* module, int loglevel, const char* file, int line, const char* message) {
	nsclient::logging::logger::get_logger()->log(module, loglevel, file, line, message);
}
void NSAPIMessage(const char* data, unsigned int count) {
	std::string message(data, count);
	nsclient::logging::logger::get_logger()->raw(message);
}
void NSAPIStopServer(void) {
	mainClient.get_service_control().stop();
}
NSCAPI::nagiosReturn NSAPIInject(const char* command, const char *request_buffer, const unsigned int request_buffer_len, char **response_buffer, unsigned int *response_buffer_len) {
	std::string request (request_buffer, request_buffer_len), response;
	NSCAPI::nagiosReturn ret = mainClient.injectRAW(command, request, response);
	*response_buffer_len = response.size();
	if (response.empty())
		*response_buffer = NULL;
	else {
		*response_buffer = new char[*response_buffer_len + 10];
		memcpy(*response_buffer, response.c_str(), *response_buffer_len);
	}
	return ret;
}

NSCAPI::nagiosReturn NSAPIExecCommand(const char* target, const char* command, const char *request_buffer, const unsigned int request_buffer_len, char **response_buffer, unsigned int *response_buffer_len) {
	std::string request (request_buffer, request_buffer_len), response;
	NSCAPI::nagiosReturn ret = mainClient.exec_command(target, command, request, response);
	*response_buffer_len = response.size();
	if (response.empty())
		*response_buffer = NULL;
	else {
		*response_buffer = new char[*response_buffer_len + 10];
		memcpy(*response_buffer, response.c_str(), *response_buffer_len);
	}
	return ret;
}


NSCAPI::boolReturn NSAPICheckLogMessages(int messageType) {
	return nsclient::logging::logger::get_logger()->should_log(messageType);
}

NSCAPI::errorReturn NSAPIEncrypt(unsigned int algorithm, const wchar_t* inBuffer, unsigned int inBufLen, wchar_t* outBuf, unsigned int *outBufLen) {
	if (algorithm != NSCAPI::encryption_xor) {
		LOG_ERROR("Unknown algortihm requested.");
		return NSCAPI::hasFailed;
	}
	/*
	TODO reimplement this

	std::wstring key = settings_manager::get_settings()->get_string(SETTINGS_KEY(protocol_def::MASTER_KEY));
	int tcharInBufLen = 0;
	char *c = charEx::tchar_to_char(inBuffer, inBufLen, tcharInBufLen);
	std::wstring::size_type j=0;
	for (int i=0;i<tcharInBufLen;i++,j++) {
		if (j > key.size())
			j = 0;
		c[i] ^= key[j];
	}
	size_t cOutBufLen = b64::b64_encode(reinterpret_cast<void*>(c), tcharInBufLen, NULL, NULL);
	if (!outBuf) {
		*outBufLen = static_cast<unsigned int>(cOutBufLen*2); // TODO: Guessing wildly here but no proper way to tell without a lot of extra work
		return NSCAPI::isSuccess;
	}
	char *cOutBuf = new char[cOutBufLen+1];
	size_t len = b64::b64_encode(reinterpret_cast<void*>(c), tcharInBufLen, cOutBuf, cOutBufLen);
	delete [] c;
	if (len == 0) {
		LOG_ERROR(_T("Invalid out buffer length."));
		return NSCAPI::isInvalidBufferLen;
	}
	int realOutLen;
	wchar_t *realOut = charEx::char_to_tchar(cOutBuf, cOutBufLen, realOutLen);
	if (static_cast<unsigned int>(realOutLen) >= *outBufLen) {
		LOG_ERROR_STD(_T("Invalid out buffer length: ") + strEx::itos(realOutLen) + _T(" was needed but only ") + strEx::itos(*outBufLen) + _T(" was allocated."));
		return NSCAPI::isInvalidBufferLen;
	}
	wcsncpy(outBuf, *outBufLen, realOut);
	delete [] realOut;
	outBuf[realOutLen] = 0;
	*outBufLen = static_cast<unsigned int>(realOutLen);
	*/
	return NSCAPI::isSuccess;
}

NSCAPI::errorReturn NSAPIDecrypt(unsigned int algorithm, const wchar_t* inBuffer, unsigned int inBufLen, wchar_t* outBuf, unsigned int *outBufLen) {
	if (algorithm != NSCAPI::encryption_xor) {
		LOG_ERROR("Unknown algortihm requested.");
		return NSCAPI::hasFailed;
	}
	/*
	int inBufLenC = 0;
	char *inBufferC = charEx::tchar_to_char(inBuffer, inBufLen, inBufLenC);
	size_t cOutLen =  b64::b64_decode(inBufferC, inBufLenC, NULL, NULL);
	if (!outBuf) {
		*outBufLen = static_cast<unsigned int>(cOutLen*2); // TODO: Guessing wildly here but no proper way to tell without a lot of extra work
		return NSCAPI::isSuccess;
	}
	char *cOutBuf = new char[cOutLen+1];
	size_t len = b64::b64_decode(inBufferC, inBufLenC, reinterpret_cast<void*>(cOutBuf), cOutLen);
	delete [] inBufferC;
	if (len == 0) {
		LOG_ERROR(_T("Invalid out buffer length."));
		return NSCAPI::isInvalidBufferLen;
	}
	int realOutLen;

	std::wstring key = settings_manager::get_settings()->get_string(SETTINGS_KEY(protocol_def::MASTER_KEY));
	std::wstring::size_type j=0;
	for (int i=0;i<cOutLen;i++,j++) {
		if (j > key.size())
			j = 0;
		cOutBuf[i] ^= key[j];
	}

	wchar_t *realOut = charEx::char_to_tchar(cOutBuf, cOutLen, realOutLen);
	if (static_cast<unsigned int>(realOutLen) >= *outBufLen) {
		LOG_ERROR_STD(_T("Invalid out buffer length: ") + strEx::itos(realOutLen) + _T(" was needed but only ") + strEx::itos(*outBufLen) + _T(" was allocated."));
		return NSCAPI::isInvalidBufferLen;
	}
	wcsncpy(outBuf, *outBufLen, realOut);
	delete [] realOut;
	outBuf[realOutLen] = 0;
	*outBufLen = static_cast<unsigned int>(realOutLen);
	*/
	return NSCAPI::isSuccess;
}

NSCAPI::errorReturn NSAPISettingsQuery(const char *request_buffer, const unsigned int request_buffer_len, char **response_buffer, unsigned int *response_buffer_len) {
	return mainClient.settings_query(request_buffer, request_buffer_len, response_buffer, response_buffer_len);
}
NSCAPI::errorReturn NSAPIRegistryQuery(const char *request_buffer, const unsigned int request_buffer_len, char **response_buffer, unsigned int *response_buffer_len) {
	return mainClient.registry_query(request_buffer, request_buffer_len, response_buffer, response_buffer_len);
}

wchar_t* copyString(const std::wstring &str) {
	int sz = str.size();
	wchar_t *tc = new wchar_t[sz+2];
	wcsncpy(tc, str.c_str(), sz);
	return tc;
}


NSCAPI::errorReturn NSAPIReload(const char *module) {
	try {
		return mainClient.reload(module);
	} catch (...) {
		LOG_ERROR_STD("Reload failed");
		return NSCAPI::hasFailed;
	}
}

LPVOID NSAPILoader(const char* buffer) {
	if (strcmp(buffer, "NSAPIGetApplicationName") == 0)
		return reinterpret_cast<LPVOID>(&NSAPIGetApplicationName);
	if (strcmp(buffer, "NSAPIGetApplicationVersionStr") == 0)
		return reinterpret_cast<LPVOID>(&NSAPIGetApplicationVersionStr);
	if (strcmp(buffer, "NSAPIMessage") == 0)
		return reinterpret_cast<LPVOID>(&NSAPIMessage);
	if (strcmp(buffer, "NSAPISimpleMessage") == 0)
		return reinterpret_cast<LPVOID>(&NSAPISimpleMessage);
	if (strcmp(buffer, "NSAPIInject") == 0)
		return reinterpret_cast<LPVOID>(&NSAPIInject);
	if (strcmp(buffer, "NSAPIExecCommand") == 0)
		return reinterpret_cast<LPVOID>(&NSAPIExecCommand);
	if (strcmp(buffer, "NSAPICheckLogMessages") == 0)
		return reinterpret_cast<LPVOID>(&NSAPICheckLogMessages);
	if (strcmp(buffer, "NSAPIEncrypt") == 0)
		return reinterpret_cast<LPVOID>(&NSAPIEncrypt);
	if (strcmp(buffer, "NSAPIDecrypt") == 0)
		return reinterpret_cast<LPVOID>(&NSAPIDecrypt);
	if (strcmp(buffer, "NSAPINotify") == 0)
		return reinterpret_cast<LPVOID>(&NSAPINotify);
	if (strcmp(buffer, "NSAPIDestroyBuffer") == 0)
		return reinterpret_cast<LPVOID>(&NSAPIDestroyBuffer);
	if (strcmp(buffer, "NSAPIExpandPath") == 0)
		return reinterpret_cast<LPVOID>(&NSAPIExpandPath);
	if (strcmp(buffer, "NSAPIReload") == 0)
		return reinterpret_cast<LPVOID>(&NSAPIReload);
	if (strcmp(buffer, "NSAPIGetLoglevel") == 0)
		return reinterpret_cast<LPVOID>(&NSAPIGetLoglevel);
	if (strcmp(buffer, "NSAPISettingsQuery") == 0)
		return reinterpret_cast<LPVOID>(&NSAPISettingsQuery);
	if (strcmp(buffer, "NSAPIRegistryQuery") == 0)
		return reinterpret_cast<LPVOID>(&NSAPIRegistryQuery);

	LOG_ERROR_STD("Function not found: " + buffer);
	return NULL;
}

NSCAPI::errorReturn NSAPINotify(const char* channel, const char* request_buffer, unsigned int request_buffer_len, char ** response_buffer, unsigned int *response_buffer_len) {
	std::string request (request_buffer, request_buffer_len), response;
	NSCAPI::nagiosReturn ret = mainClient.send_notification(channel, request, response);
	*response_buffer_len = response.size();
	if (response.empty())
		*response_buffer = NULL;
	else {
		*response_buffer = new char[*response_buffer_len + 10];
		memcpy(*response_buffer, response.c_str(), *response_buffer_len);
	}
	return ret;
}

void NSAPIDestroyBuffer(char**buffer) {
	delete [] *buffer;
}

NSCAPI::log_level::level NSAPIGetLoglevel() {
	return nsclient::logging::logger::get_logger()->get_log_level();
}
