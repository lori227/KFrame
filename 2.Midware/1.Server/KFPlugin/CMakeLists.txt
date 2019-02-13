set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -rdynamic -shared")

message("${CMAKE_CXX_FLAGS}")
macro(LINK_SDK_MACRO project_name)

    if(CMAKE_BUILD_TYPE MATCHES "Debug") 
        target_link_libraries(${project_name} KFGlobald KFLoggerd KFPlugind KFMemoryd KFProtod KFProtocold KFHttpd KFCored KFNetworkd KFUtilityd KFMath3Dd PocoNetd PocoNetSSLd PocoCryptod PocoJSONd PocoUtild PocoFoundationd PocoXMLd PocoDataMySQLd PocoDatad z dl ssl crypto pthread uv luaplus53d iconv protobuf hiredis mysqlclient)
    else()
        target_link_libraries(${project_name} KFGlobal KFLogger KFPlugin KFMemory KFProto KFProtocol KFHttp KFCore KFNetwork KFUtility KFMath3D PocoNet PocoNetSSL PocoCrypto PocoJSON PocoUtil PocoFoundation PocoXML PocoDataMySQL PocoData z dl ssl crypto pthread uv luaplus53 iconv protobuf hiredis mysqlclient)  
    endif()

endmacro(LINK_SDK_MACRO project_name)

macro(BUILD_PLUGIN_MACRO project_name)
    aux_source_directory(. KF_SRC)
    add_library(${project_name} SHARED ${KF_SRC})
    add_dependencies(${project_name} KFProtocol)


    set_target_properties(${project_name} PROPERTIES PREFIX "")
    set_target_properties(${project_name} PROPERTIES OUTPUT_NAME_DEBUG "${project_name}d")
    set_target_properties(${project_name} PROPERTIES
        FOLDER "KFPlugin"
        ARCHIVE_OUTPUT_DIRECTORY ${BIN_DIR}
        RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${BIN_DIR})

LINK_SDK_MACRO(${project_name})
endmacro(BUILD_PLUGIN_MACRO project_name)

add_subdirectory(KFAuth)
add_subdirectory(KFChannel)

add_subdirectory(KFGame)
add_subdirectory(KFGate)
add_subdirectory(KFLogin)
add_subdirectory(KFWorld)
add_subdirectory(KFZone)

add_subdirectory(KFDisplay)
add_subdirectory(KFLua)

add_subdirectory(KFCommand)
add_subdirectory(KFAttribute)
add_subdirectory(KFAchieve)
add_subdirectory(KFActivity)
add_subdirectory(KFCompound)
add_subdirectory(KFTask)
add_subdirectory(KFItem)
add_subdirectory(KFHero)
add_subdirectory(KFSkin)
add_subdirectory(KFSignIn)
add_subdirectory(KFReset)
add_subdirectory(KFEnter)
add_subdirectory(KFLeave)
add_subdirectory(KFStore)

add_subdirectory(KFMailClient)
add_subdirectory(KFMailShard)

add_subdirectory(KFDataClient)
add_subdirectory(KFDataShard)

