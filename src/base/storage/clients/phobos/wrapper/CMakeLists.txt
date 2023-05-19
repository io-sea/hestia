include(AddModule)

find_package(PkgConfig REQUIRED)
pkg_search_module(GLIB REQUIRED glib-2.0)

add_module(
    MODULE_NAME phobos_impl
    NO_INSTRUMENT
    HEADERS
        PhobosWrapper.h
        PhobosInterfaceImpl.h
        PhobosDescriptor.h
    SOURCES
        PhobosWrapper.c
        PhobosInterfaceImpl.cc
        PhobosDescriptor.cc
    INTERNAL_DEPENDENCIES 
        phobos_backend_interface_impl
    DEPENDENCIES
        ${GLIB_LDFLAGS}
        Phobos::Store
    INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/../
        ${GLIB_INCLUDE_DIRS}
    COMPILER_DEFINITIONS
        HAS_PHOBOS
)