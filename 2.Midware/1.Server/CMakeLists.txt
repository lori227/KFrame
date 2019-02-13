cmake_minimum_required(VERSION 3.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(KFrame)

########################################################################

set(ROOT_DIR ${PROJECT_SOURCE_DIR})
include_directories(
    ${ROOT_DIR}
    ${ROOT_DIR}/KFCommon
    ${ROOT_DIR}/KFContrib
    ${ROOT_DIR}/KFLibrary
    ${ROOT_DIR}/KFPlugin
    ${ROOT_DIR}/KFInterface
    ${ROOT_DIR}/KFLibrary/lua/lua53
)

set(3RD_LIB_DIR ${ROOT_DIR}/_lib/linux ${ROOT_DIR}/_lib/linux/3rd)
set(LIB_DIR ${ROOT_DIR}/_lib/linux)
LINK_DIRECTORIES(${3RD_LIB_DIR})


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC -Wall")
if(CMAKE_BUILD_TYPE MATCHES "Debug")
    set(BIN_DIR ${ROOT_DIR}/_bin/bin/debug)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g -ggdb -D_DEBUG")
else()
    set(CMAKE_BUILD_TYPE "Release")
    set(BIN_DIR ${ROOT_DIR}/_bin/bin/release)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
endif()

if(COROUTINE MATCHES "Enable")
    message("use coroutine...")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_COROUTINE")
endif()

message("${CMAKE_CXX_FLAGS}")
ADD_DEFINITIONS(
 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-strict-aliasing -Wno-pmf-conversions
)

set(LDFLAGS "$ENV{LDFLAGS} -stdlib=libc++")

if(UNIX)
    #Set rpath
    SET(CMAKE_INSTALL_RPATH "./lib/" "../lib/")
    SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
endif()

#log
message("root dir=${ROOT_DIR}")
message("lib dir=${LIB_DIR}")
message("bin dir=${BIN_DIR}")

##################################################################
message("start to build all...")
add_subdirectory(KFContrib)
add_subdirectory(KFPlugin)

