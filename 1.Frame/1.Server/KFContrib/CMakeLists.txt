
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

add_subdirectory(KFCore)
add_subdirectory(KFHttp)
add_subdirectory(KFLogger)
add_subdirectory(KFGlobal)
add_subdirectory(KFMath3D)
add_subdirectory(KFMemory)
add_subdirectory(KFNetwork)
add_subdirectory(KFPlugin)
add_subdirectory(KFProtocol)
add_subdirectory(KFUtility)
