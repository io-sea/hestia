include(AddModule)

add_module(
    MODULE_NAME motr_object_store_client 
    HEADERS
        MotrHsmClient.h
        MotrInterface.h 
        IMotrInterfaceImpl.h
        MotrConfig.h
    SOURCES
        MotrHsmClient.cc
        MotrInterface.cc 
        MotrConfig.cc
    INTERNAL_DEPENDENCIES 
        hsm_object_store_client
)

if(HESTIA_WITH_MOTR)

    include(FindPkgConfig)

    set(MOTR_SRC_DIR "" CACHE PATH "Path to motr src checkout - needed for hsm includes")
    pkg_check_modules(motr REQUIRED motr)

    add_library(${PROJECT_NAME}_motr_plugin SHARED
        MotrInterfaceImpl.h 
        MotrInterfaceImpl.cc
        MotrClientPlugin.cc
        ${MOTR_SRC_DIR}/hsm/m0hsm_api.c
    )

    target_include_directories(${PROJECT_NAME}_motr_plugin PRIVATE 
        ${motr_INCLUDE_DIRS}
        ${MOTR_SRC_DIR}
    )

    target_link_libraries(${PROJECT_NAME}_motr_plugin PRIVATE 
        hestia_motr_object_store_client
        ${motr_LINK_LIBRARIES})

    target_compile_options(${PROJECT_NAME}_motr_plugin PUBLIC ${motr_CFLAGS}
        -Wno-error=unused-parameter 
        -Wno-error=pedantic 
        -Wno-error=sign-compare
        -Wno-error=missing-field-initializers
    )
    target_compile_definitions(${PROJECT_NAME}_motr_plugin PUBLIC HAS_MOTR)

endif()