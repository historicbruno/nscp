cmake_minimum_required(VERSION 2.6)

SET(TARGET check_nrpe)

PROJECT(clients)

INCLUDE_DIRECTORIES(${BOOST_INCLUDE_DIRS})
IF(JSON_SPIRIT_FOUND)
	INCLUDE_DIRECTORIES(${JSON_SPIRIT_INCLUDE_DIR})
	ADD_DEFINITIONS(-DHAVE_JSON_SPIRIT)
	SET(JSON_LIB json_spirit)
ELSE(JSON_SPIRIT_FOUND)
	SET(JSON_LIB)
ENDIF(JSON_SPIRIT_FOUND)


LINK_DIRECTORIES(${BOOST_LIB_DIRS})

SET(SRCS ${SRCS}
	check_nrpe.cpp
	../../modules/NRPEClient/nrpe_client.cpp
	${NSCP_INCLUDEDIR}/nrpe/packet.cpp
	${NSCP_INCLUDEDIR}/socket/socket_helpers.cpp
	${NSCP_INCLUDEDIR}/client/command_line_parser.cpp
	${NSCP_INCLUDEDIR}/utils.cpp

	../../resources/nsclient.ico
)

ADD_DEFINITIONS(${NSCP_GLOBAL_DEFINES})
IF(OPENSSL_FOUND)
	ADD_DEFINITIONS(-DUSE_SSL)
	SET(EXTRA_LIBS ${EXTRA_LIBS} ${OPENSSL_LIBRARIES})
	INCLUDE_DIRECTORIES(${OPENSSL_INCLUDE_DIR})
ENDIF(OPENSSL_FOUND)

IF(WIN32)
	SET(SRCS ${SRCS}
		check_nrpe.hpp
		../../modules/NRPEClient/nrpe_client.hpp
		${NSCP_INCLUDEDIR}/utils.h
		${NSCP_INCLUDEDIR}/nrpe/packet.hpp
		${NSCP_INCLUDEDIR}/nrpe/client/nrpe_client_protocol.hpp
		${NSCP_INCLUDEDIR}/swap_bytes.hpp
		${NSCP_INCLUDEDIR}/socket/socket_helpers.hpp
		${NSCP_INCLUDEDIR}/socket/client.hpp
		${NSCP_INCLUDEDIR}/client/command_line_parser.hpp
	)
ENDIF(WIN32)

ADD_DEFINITIONS(${NSCP_GLOBAL_DEFINES})

# Add breakpad
IF(BREAKPAD_FOUND)
	SET(SRCS ${SRCS} ${NSCP_INCLUDEDIR}/breakpad/exception_handler_win32.cpp)
	IF(WIN32)
		SET(SRCS ${SRCS} ${NSCP_INCLUDEDIR}/breakpad/exception_handler_win32.hpp)
	ENDIF(WIN32)
	SET(EXTRA_LIBS ${EXTRA_LIBS} ${BREAKPAD_COMMON_LIBRARY} ${BREAKPAD_EXCEPTION_HANDLER_LIBRARY} ${BREAKPAD_CRASH_GENERATION_CLIENT_LIBRARY})
	INCLUDE_DIRECTORIES(${BREAKPAD_INCLUDE_DIR})
ENDIF(BREAKPAD_FOUND)

ADD_EXECUTABLE(${TARGET} ${SRCS})
IF(WIN32 AND NOT CMAKE_CL_64)
	OPENSSL_LINK_FIX(${TARGET})
ENDIF(WIN32 AND NOT CMAKE_CL_64)

IF(BREAKPAD_FOUND)
	SET_TARGET_PROPERTIES(${TARGET} PROPERTIES COMPILE_FLAGS -DUSE_BREAKPAD)
ENDIF(BREAKPAD_FOUND)

TARGET_LINK_LIBRARIES(${TARGET}
	${Boost_FILESYSTEM_LIBRARY}
	${Boost_PROGRAM_OPTIONS_LIBRARY}
	${Boost_THREAD_LIBRARY}
	${NSCP_DEF_PLUGIN_LIB}
	${EXTRA_LIBS}
)
INSTALL(TARGETS ${TARGET} RUNTIME DESTINATION .)
SET_TARGET_PROPERTIES(${TARGET} PROPERTIES FOLDER "clients")
IF(MSVC11)
	SET_TARGET_PROPERTIES(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BUILD_TARGET_EXE_PATH})
	SET_TARGET_PROPERTIES(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BUILD_TARGET_EXE_PATH})
	SET_TARGET_PROPERTIES(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BUILD_TARGET_EXE_PATH})
	SET_TARGET_PROPERTIES(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${BUILD_TARGET_EXE_PATH})
ENDIF(MSVC11)

SOURCE_GROUP("Client" REGULAR_EXPRESSION .*include/nrpe/.*)
SOURCE_GROUP("Socket" REGULAR_EXPRESSION .*include/socket/.*)
SOURCE_GROUP("NSCP API" REGULAR_EXPRESSION .*include/nscapi/.*)
SOURCE_GROUP("Settings" REGULAR_EXPRESSION .*include/settings/.*)
SOURCE_GROUP("Common Files" REGULAR_EXPRESSION .*include/.*)

