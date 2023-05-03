include(FindPackageHandleStandardArgs)

if(NOT phobos_FOUND)
    find_path(PHOBOS_INCLUDE_DIR
    HINTS ./  
        /usr/include/
        /usr/include/phobos/
        /usr/local/include/
        /usr/local/include/phobos/
        ${CMAKE_INSTALL_PREFIX}/include/
        ${CMAKE_INSTALL_PREFIX}/include/phobos/
        ${CMAKE_INSTALL_PREFIX}/phobos/include/
        ${CMAKE_INSTALL_PREFIX}/../include/
        ${CMAKE_INSTALL_PREFIX}/../include/phobos/
        ${CMAKE_INSTALL_PREFIX}/../phobos/include/
        ${CMAKE_PREFIX_PATH}/phobos-install/include/
        ${phobos_DIR}/include/
    NAMES phobos_store.h pho_attrs.h
    DOC "Phobos headers"
    )

    find_path(PHOBOS_LIBRARY_DIR
    HINTS ./
        /usr/lib/
        /usr/lib64/
        /usr/local/lib/
        /usr/local/lib64/
        ${CMAKE_INSTALL_PREFIX}/lib/
        ${CMAKE_INSTALL_PREFIX}/lib64/
        ${CMAKE_INSTALL_PREFIX}/phobos/lib/
        ${CMAKE_INSTALL_PREFIX}/phobos/lib64/
        ${CMAKE_INSTALL_PREFIX}/../phobos/lib/
        ${CMAKE_INSTALL_PREFIX}/../phobos/lib64/
        ${CMAKE_PREFIX_PATH}/phobos-install/lib/
        ${phobos_DIR}/lib/
    NAMES libphobos_store.so libphobos_store.la
    DOC "Phobos library"
    )

    find_package_handle_standard_args(phobos REQUIRED_VARS PHOBOS_INCLUDE_DIR PHOBOS_LIBRARY_DIR 
    HANDLE_COMPONENTS)

    if(${phobos_FOUND})
        add_library(Phobos::Store STATIC IMPORTED GLOBAL)
        set_property(TARGET Phobos::Store PROPERTY IMPORTED_LOCATION ${PHOBOS_LIBRARY_DIR}/libphobos_store.so)
        set_property(TARGET Phobos::Store PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${PHOBOS_INCLUDE_DIR})
    endif()
endif()