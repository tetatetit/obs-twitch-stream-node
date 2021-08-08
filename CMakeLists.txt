cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

include(CPack)

PROJECT(obs_twitch_stream_node VERSION ${obs-twitch-stream-node_VERSION})
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

SET(NODEJS_URL "https://atom.io/download/atom-shell" CACHE STRING "Node.JS URL")
SET(NODEJS_NAME "iojs" CACHE STRING "Node.JS Name")
SET(NODEJS_VERSION "v10.4.2" CACHE STRING "Node.JS Version")

# Resource VersionInfo
set(PROJECT_PRODUCT_NAME "OBS Studio for Node.JS (Server)")
set(PROJECT_COPYRIGHT "Streamlabs © 2017 - 2018")

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

set(PROJECT_INCLUDE_PATHS
	${LIBOBS_INCLUDE_DIRS}
)

download_project(
	PROJ libobs
	URL "https://obsstudios3.streamlabs.com/libobs-windows64-release-27.1.1.7z"
	UPDATE_DISCONNECTED 1
)

include("${libobs_SOURCE_DIR}/cmake/LibObs/LibObsConfig.cmake")

target_sources(obs_twitch_stream_node PUBLIC ${PROJECT_BINARY_DIR}/version.rc)
target_link_libraries(obs_twitch_stream_node ${LIBOBS_LIBRARIES})


target_include_directories(
	obs_twitch_stream_node
	PUBLIC
		"${CMAKE_SOURCE_DIR}/source/"
		${PROJECT_INCLUDE_PATHS}
)

# Include N-API wrappers
execute_process(COMMAND node -p "require('node-addon-api').include"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE NODE_ADDON_API_DIR
        )
string(REPLACE "\n" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})
string(REPLACE "\"" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${NODE_ADDON_API_DIR})
target_link_libraries(${PROJECT_NAME} ${LIBOBS_LIBRARIES})

# Define NAPI_VERSION
add_definitions(-DNAPI_VERSION=4)

#Define the OSN_VERSION
add_compile_definitions(OSN_VERSION=\"$ENV{tagartifact}\") 
set_target_properties(
	obs_twitch_stream_node
	PROPERTIES
	VERSION ${PROJECT_VERSION}
	SOVERSION ${PROJECT_VERSION}
	PREFIX "" SUFFIX ".node"
)

target_compile_definitions(obs_twitch_stream_node PRIVATE BUILDING_NODE_EXTENSION)

target_compile_definitions(
	obs_twitch_stream_node
	PRIVATE
		WIN32_LEAN_AND_MEAN
		NOMINMAX
		UNICODE
		_UNICODE
)
