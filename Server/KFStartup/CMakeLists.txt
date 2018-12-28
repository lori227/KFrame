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
)

aux_source_directory(. KFSTARTUP_SRC)
list(REMOVE_ITEM KFSTARTUP_SRC "./KFDump.h")
list(REMOVE_ITEM KFSTARTUP_SRC "./KFDump.cpp")

add_executable(KFStartup ${KFSTARTUP_SRC})
add_dependencies(KFStartup KFUtility)

set_target_properties(KFStartup PROPERTIES OUTPUT_NAME_DEBUG "KFStartupd")
set_target_properties(KFStartup PROPERTIES
    FOLDER "KFStartup"
    ARCHIVE_OUTPUT_DIRECTORY ${BIN_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${BIN_DIR})

add_definitions(
    -D_CONSOLE
)


if(CMAKE_BUILD_TYPE MATCHES "Debug")
    target_link_libraries(KFStartup dl rt pthread iconv PocoJSONd PocoUtild PocoFoundationd PocoJSONd PocoXMLd KFGlobald KFPlugind  KFMemoryd KFUtilityd tcmalloc_minimal)
else()
    target_link_libraries(KFStartup dl rt pthread iconv PocoJSON PocoUtil PocoFoundation PocoJSON PocoXML KFGlobal KFPlugin KFMemory KFUtility tcmalloc_minimal)  
endif()