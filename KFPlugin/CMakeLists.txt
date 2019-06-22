set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -rdynamic -shared")

message("${CMAKE_CXX_FLAGS}")
macro(LINK_SDK_MACRO project_name)

    FIND_LIBRARY(SSL_LIB ssl ${3RD_LIB_DIR} NO_DEFAULT_PATH)
    FIND_LIBRARY(CRYPTO_LIB crypto ${3RD_LIB_DIR} NO_DEFAULT_PATH)
    link_libraries(${SSL_LIB} ${CRYPTO_LIB})

    if(CMAKE_BUILD_TYPE MATCHES "Debug") 
        target_link_libraries(${project_name} 
        KFGlobald  KFLoggerd KFPlugind KFMemoryd KFProtod KFProtocold KFHttpd KFCored KFNetworkd KFUtilityd KFMath3Dd 
        PocoNetd PocoNetSSLd PocoCryptod PocoJSONd PocoUtild PocoXMLd PocoDataMySQLd PocoDatad PocoFoundationd luaplus53d 
        dl pthread uv iconv protobuf hiredis mysqlclient ssl crypto lz4 )
    else()
        target_link_libraries(${project_name}
        KFGlobal KFLogger KFPlugin KFMemory KFProto KFProtocol KFHttp KFCore KFNetwork KFUtility KFMath3D 
        PocoNet PocoNetSSL PocoCrypto PocoJSON PocoUtil PocoXML PocoDataMySQL PocoData PocoFoundation luaplus53 
        dl pthread uv iconv protobuf hiredis mysqlclient ssl crypto lz4 )  
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
add_subdirectory(KFTcpDiscover)

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
add_subdirectory(KFOption)
add_subdirectory(KFFilter)
add_subdirectory(KFPlayer)

add_subdirectory(KFDeployAgent)
add_subdirectory(KFDeployClient)
add_subdirectory(KFDeployServer)

add_subdirectory(KFLogClient)
add_subdirectory(KFLogShard)


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
add_subdirectory(KFPay)

add_subdirectory(KFMailClient)
add_subdirectory(KFMailShard)

add_subdirectory(KFDataClient)
add_subdirectory(KFDataShard)

add_subdirectory(KFRankClient)
add_subdirectory(KFRankShard)

add_subdirectory(KFPublicClient)
add_subdirectory(KFPublicShard)

add_subdirectory(KFRelationClient)
add_subdirectory(KFRelationShard)