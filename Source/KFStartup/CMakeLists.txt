aux_source_directory(. KFSTARTUP_SRC)
list(REMOVE_ITEM KFSTARTUP_SRC "./KFDump.h")
list(REMOVE_ITEM KFSTARTUP_SRC "./KFDump.cpp")

add_executable(KFStartup ${KFSTARTUP_SRC})

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
    target_link_libraries(KFStartup dl rt pthread iconv log4cxx PocoJSONd PocoUtild PocoFoundationd PocoJSONd PocoXMLd KFLoggerd KFXmld KFPlugind KFRandd  KFTimed KFMemoryd KFConvertd KFUtilityd KFThreadd)
else()
    target_link_libraries(KFStartup dl rt pthread iconv log4cxx PocoJSON PocoUtil PocoFoundation PocoJSON PocoXML KFLogger KFXml KFPlugin KFRand KFTime KFMemory KFConvert KFUtility KFThread)  
endif()

