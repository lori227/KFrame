
include_directories(
    ${ROOT_DIR}/KFLibrary/poco/Foundation/include
    ${ROOT_DIR}/KFLibrary/poco/Net/include
    ${ROOT_DIR}/KFLibrary/poco/Crypto/include
    ${ROOT_DIR}/KFLibrary/poco/NetSSL_OpenSSL/include
    ${ROOT_DIR}/KFLibrary/poco/CppUnit/include
    ${ROOT_DIR}/KFLibrary/poco/Util/include
    ${ROOT_DIR}/KFLibrary/poco/Data/MySQL/include
    ${ROOT_DIR}/KFLibrary/poco/Data/ODBC/include
    ${ROOT_DIR}/KFLibrary/poco/Data/SQLite/include
    ${ROOT_DIR}/KFLibrary/json/include
    ${ROOT_DIR}/KFLibrary/lua/lua53
    ${ROOT_DIR}/KFLibrary/google/protobuf/src
    ${ROOT_DIR}/KFLibrary/log4cxx/log4cxx/src/main/include
)

#################################################################################
#################################################################################
macro(BUILD_CONTRIB_MACRO project_name)
aux_source_directory(. KF_SRC)
add_library(${project_name} STATIC ${KF_SRC})

set_target_properties(${project_name} PROPERTIES OUTPUT_NAME_DEBUG "${project_name}d")
set_target_properties(${project_name} PROPERTIES
    FOLDER "KFContrib"
    ARCHIVE_OUTPUT_DIRECTORY ${LIB_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${LIB_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${LIB_DIR})

target_link_libraries(${project_name})
endmacro()
#################################################################################
#################################################################################
macro(BUILD_CONTRIB_MACRO_EX project_name sub_lib)
aux_source_directory(. KF_SRC)
add_library(${project_name} STATIC ${KF_SRC})

set_target_properties(${project_name} PROPERTIES OUTPUT_NAME_DEBUG "${project_name}d")
set_target_properties(${project_name} PROPERTIES
    FOLDER "KFContrib"
    ARCHIVE_OUTPUT_DIRECTORY ${LIB_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${LIB_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${LIB_DIR})
target_link_libraries(${project_name} ${sub_lib})
endmacro()
#################################################################################
#################################################################################

add_subdirectory(KFThread)
add_subdirectory(KFCompress)
add_subdirectory(KFConHash)
add_subdirectory(KFConvert)
add_subdirectory(KFCrypto)
add_subdirectory(KFCore)
add_subdirectory(KFExcel)
add_subdirectory(KFHttp)
add_subdirectory(KFLogger)
add_subdirectory(KFMath3D)
add_subdirectory(KFMemory)
add_subdirectory(KFNetwork)
add_subdirectory(KFPlugin)
add_subdirectory(KFProtocol)
add_subdirectory(KFRand)
add_subdirectory(KFTime)
add_subdirectory(KFUtility)
add_subdirectory(KFXml)