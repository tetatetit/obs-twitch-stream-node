cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options("/std:c++latest")

set(VERSION_FULL 0.1.1)
set(VERSION_BUILD 0)

project(obs-twitch-stream-node VERSION ${VERSION_FULL}.${VERSION_BUILD})
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

# Resource VersionInfo
set(PROJECT_PRODUCT_NAME "OBS Twitch Stream")
set(PROJECT_COMPANY_NAME "Serhiy Berezin")
set(PROJECT_COPYRIGHT "Serhiy Berezin © 2021 - 2024")
set(PROJECT_LEGAL_TRADEMARKS_1 "")
set(PROJECT_LEGAL_TRADEMARKS_2 "")

configure_file(
	${PROJECT_SOURCE_DIR}/cmake/version.rc.in
	${PROJECT_BINARY_DIR}/version.rc
	@ONLY
)

############################
# Setup global dependencies
############################

include(DownloadProject)

if(MSVC)
	add_definitions(/MP)
	add_definitions(/debug:fastlink)
endif()

PROJECT(obs_twitch_stream_node VERSION ${obs-twitch-stream-node_VERSION})

SET(NODEJS_URL "https://atom.io/download/atom-shell" CACHE STRING "Node.JS URL")
SET(NODEJS_NAME "iojs" CACHE STRING "Node.JS Name")
SET(NODEJS_VERSION "v10.4.2" CACHE STRING "Node.JS Version")

configure_file(
	${obs-twitch-stream-node_SOURCE_DIR}/cmake/version.rc.in
	${PROJECT_BINARY_DIR}/version.rc
	@ONLY
)

include(NodeJS)

set(STREAMLABS_BUILD ON CACHE BOOL "Whether to build for streamlabs-obs")

nodejs_init()

add_nodejs_module(
	obs_twitch_stream_node
	"source/main.cpp"
)

include("${libobs_SOURCE_DIR}/cmake/LibObs/LibObsConfig.cmake")

# Include N-API wrappers
execute_process(COMMAND node -p "require('node-addon-api').include"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE NODE_ADDON_API_DIR
        )
string(REPLACE "\n" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})
string(REPLACE "\"" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})

# Define NAPI_VERSION
add_definitions(-DNAPI_VERSION=4)

set_target_properties(
	${PROJECT_NAME}
	PROPERTIES
	VERSION ${PROJECT_VERSION}
	SOVERSION ${PROJECT_VERSION}
	PREFIX "" SUFFIX ".node"
)

target_link_libraries(${PROJECT_NAME} ${LIBOBS_LIBRARIES})
target_sources(${PROJECT_NAME} PUBLIC ${PROJECT_BINARY_DIR}/version.rc)
target_include_directories(
	${PROJECT_NAME}
	PRIVATE
		${NODE_ADDON_API_DIR}
	PUBLIC
		"${CMAKE_SOURCE_DIR}/source/"
		${LIBOBS_INCLUDE_DIRS}
)
target_compile_definitions(
	${PROJECT_NAME}
	PRIVATE
		BUILDING_NODE_EXTENSION
		WIN32_LEAN_AND_MEAN
		NOMINMAX
		UNICODE
		_UNICODE
)

# Installation

install(FILES $<TARGET_PDB_FILE:obs_twitch_stream_node> DESTINATION ./ OPTIONAL)
install(FILES "${CMAKE_SOURCE_DIR}/package.json" DESTINATION "./")
install(
	DIRECTORY "${PROJECT_SOURCE_DIR}/js/"
	DESTINATION "./"
	PATTERN "*.json" EXCLUDE
)
install(
	TARGETS obs_twitch_stream_node
	RUNTIME DESTINATION "./" COMPONENT Runtime
	LIBRARY DESTINATION "./" COMPONENT Runtime
	ARCHIVE DESTINATION "./" COMPONENT Runtime	
)
install(DIRECTORY "${libobs_SOURCE_DIR}/data/" DESTINATION "./data" USE_SOURCE_PERMISSIONS)
install(
	DIRECTORY "${libobs_SOURCE_DIR}/bin/64bit/"
	DESTINATION "./" PATTERN "*.lib" EXCLUDE
)
install(
	DIRECTORY "${libobs_SOURCE_DIR}/bin/"
	DESTINATION "./" USE_SOURCE_PERMISSIONS PATTERN "*.lib" EXCLUDE
)
install(
	DIRECTORY "${libobs_SOURCE_DIR}/obs-plugins/"
	DESTINATION "./obs-plugins"
)
install(DIRECTORY "${libobs_SOURCE_DIR}/enc-amf_old" DESTINATION "./" OPTIONAL)

include(CPack)