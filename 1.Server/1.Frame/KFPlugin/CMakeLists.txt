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
    add_dependencies(${project_name} KFCore)
    add_dependencies(${project_name} KFHttp)
    add_dependencies(${project_name} KFLogger)
    add_dependencies(${project_name} KFGlobal)
    add_dependencies(${project_name} KFMath3D)
    add_dependencies(${project_name} KFMemory)
    add_dependencies(${project_name} KFNetwork)
    add_dependencies(${project_name} KFPlugin)
    add_dependencies(${project_name} KFProto)
    add_dependencies(${project_name} KFProtocol)
    add_dependencies(${project_name} KFUtility)

    set_target_properties(${project_name} PROPERTIES PREFIX "")
    set_target_properties(${project_name} PROPERTIES OUTPUT_NAME_DEBUG "${project_name}d")
    set_target_properties(${project_name} PROPERTIES
        FOLDER "KFPlugin"
        ARCHIVE_OUTPUT_DIRECTORY ${BIN_DIR}
        RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${BIN_DIR})

LINK_SDK_MACRO(${project_name})
endmacro(BUILD_PLUGIN_MACRO project_name)

add_subdirectory(KFBus)
add_subdirectory(KFConfig)
add_subdirectory(KFHttpClient)
add_subdirectory(KFHttpServer)
add_subdirectory(KFIpAddress)
add_subdirectory(KFMessage)
add_subdirectory(KFTcpClient)
add_subdirectory(KFTcpServer)

add_subdirectory(KFRedis)
add_subdirectory(KFMySQL)
add_subdirectory(KFWorker)

add_subdirectory(KFClusterClient)
add_subdirectory(KFClusterProxy)
add_subdirectory(KFClusterMaster)
add_subdirectory(KFClusterShard)

add_subdirectory(KFRouteClient)
add_subdirectory(KFRouteProxy)
add_subdirectory(KFRouteShard)

add_subdirectory(KFSchedule)
add_subdirectory(KFTimer)
add_subdirectory(KFKernel)