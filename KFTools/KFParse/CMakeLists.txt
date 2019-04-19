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

aux_source_directory(. PARSE_SRC)

add_executable(_parse ${PARSE_SRC})
add_dependencies(_parse KFUtility)
add_dependencies(_parse KFExcel)

set_target_properties(KFStartup PROPERTIES OUTPUT_NAME_DEBUG "_parsed")
set_target_properties(KFStartup PROPERTIES
    FOLDER "_parse"
    ARCHIVE_OUTPUT_DIRECTORY ${BIN_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${BIN_DIR})

add_definitions(
    -D_CONSOLE
)

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    target_link_libraries(_parse dl rt pthread iconv PocoJSONd PocoXMLd PocoUtild PocoFoundationd PocoJSONd KFGlobald KFPlugind KFMemoryd KFUtilityd tcmalloc_minimal)
else()
    target_link_libraries(_parse dl rt pthread iconv PocoJSON PocoXML PocoUtil PocoFoundation PocoJSON KFGlobal KFPlugin KFMemory KFUtility tcmalloc_minimal)  
endif()