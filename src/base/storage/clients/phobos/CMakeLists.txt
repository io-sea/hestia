include(AddModule)

add_module(
    MODULE_NAME phobos_backend_interface_impl
    HEADERS
        IPhobosInterfaceImpl.h
    SOURCES
        IPhobosInterfaceImpl.cc
    INTERNAL_DEPENDENCIES 
        storage
)

add_module(
    MODULE_NAME phobos_backend
    HEADERS
        PhobosInterface.h
        PhobosClient.h
    SOURCES
        PhobosInterface.cc
        PhobosClient.cc
    INTERNAL_DEPENDENCIES 
        storage
)

if(HESTIA_WITH_PHOBOS)
    if(NOT APPLE)
        if(NOT ${phobos_FOUND})
            find_package(phobos QUIET)
        endif()
        if(NOT ${phobos_FOUND})
            build_phobos()
        endif()
        add_subdirectory(wrapper)

        add_module(
            MODULE_NAME phobos_plugin
            SHARED
            SOURCES
                PhobosClientPlugin.cc
            INTERNAL_DEPENDENCIES 
                phobos_backend
                phobos_impl
            COMPILER_DEFINITIONS
                HAS_PHOBOS
        )

        add_dependencies(${PROJECT_NAME}_storage_and_plugins ${PROJECT_NAME}_phobos_plugin)
    endif()
endif()