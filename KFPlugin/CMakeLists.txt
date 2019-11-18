set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -rdynamic -shared")

macro(PLUGIN_PROPERTIES_MACRO project_name)
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
    add_dependencies(${project_name} KFZConfig)

    set_target_properties(${project_name} PROPERTIES PREFIX "")
    set_target_properties(${project_name} PROPERTIES OUTPUT_NAME_DEBUG "${project_name}d")
    set_target_properties(${project_name} PROPERTIES
        FOLDER "KFPlugin"
        ARCHIVE_OUTPUT_DIRECTORY ${BIN_DIR}
        RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${BIN_DIR})

endmacro(PLUGIN_PROPERTIES_MACRO project_name)


set(FRAME_LIB_DEBUG KFGlobald KFLoggerd KFPlugind KFMemoryd KFZConfigd KFUtilityd)
set(FRAME_LIB_RELEASE KFGlobal KFLogger KFPlugin KFMemory KFZConfig KFUtility)

macro(LINK_PLUGIN_MACRO project_name)
    if(CMAKE_BUILD_TYPE MATCHES "Debug") 
        target_link_libraries(${project_name} ${FRAME_LIB_DEBUG} ${EXTEND_LIB_DEBUG})
    else()
        target_link_libraries(${project_name} ${FRAME_LIB_RELEASE} ${EXTEND_LIB_RELEASE})
    endif()
endmacro(LINK_PLUGIN_MACRO project_name)

#############################################################################
## frame plugin param
macro(BUILD_PLUGIN_NO_PROTO project_name)
    ## properties
    PLUGIN_PROPERTIES_MACRO(${project_name})

    ## link
    LINK_PLUGIN_MACRO(${project_name})
endmacro(BUILD_PLUGIN_NO_PROTO project_name)
#############################################################################


#############################################################################
## logic plugin
macro(BUILD_PLUGIN_WITH_PROTO project_name)
    ## properties
    PLUGIN_PROPERTIES_MACRO(${project_name})

    ## extend
    set(EXTEND_LIB_DEBUG KFProtod KFProtocold protobuf ${EXTEND_LIB_DEBUG} )
    set(EXTEND_LIB_RELEASE KFProto KFProtocol protobuf ${EXTEND_LIB_RELEASE} )

    ## link
    LINK_PLUGIN_MACRO(${project_name})
endmacro(BUILD_PLUGIN_WITH_PROTO project_name)
#############################################################################

#############################################################################
## logic plugin
macro(BUILD_PLUGIN_WITH_LOGIC project_name)
    ## properties
    PLUGIN_PROPERTIES_MACRO(${project_name})

    ## extend
    set(EXTEND_LIB_DEBUG KFProtod KFProtocold protobuf KFCored KFMath3Dd ${EXTEND_LIB_DEBUG} )
    set(EXTEND_LIB_RELEASE KFProto KFProtocol protobuf KFCore KFMath3D ${EXTEND_LIB_RELEASE} )

    ## link
    LINK_PLUGIN_MACRO(${project_name})
endmacro(BUILD_PLUGIN_WITH_LOGIC project_name)
#############################################################################

############################################################################################
############################################################################################
############################################################################################
############################################################################################
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
add_subdirectory(KFMongo)
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
add_subdirectory(KFDisplay)
add_subdirectory(KFLua)
add_subdirectory(KFCommand)
#add_subdirectory(KFCompound)
add_subdirectory(KFReset)
#add_subdirectory(KFPay)
#add_subdirectory(KFMailClient)
#add_subdirectory(KFMailShard)
add_subdirectory(KFDataClient)
add_subdirectory(KFDataShard)
add_subdirectory(KFPublicClient)
add_subdirectory(KFPublicShard)
add_subdirectory(KFDrop)
add_subdirectory(KFUnlock)
add_subdirectory(KFCondition)
add_subdirectory(KFExecute)