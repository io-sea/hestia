set(STANDALONE_MODULES_FOR_EXPORT "")

if(HESTIA_WITH_MOTR)
        list(APPEND STANDALONE_MODULES_FOR_EXPORT ${PROJECT_NAME}_motr_plugin)
endif()

if(HESTIA_WITH_PHOBOS)
if(NOT APPLE)
        list(APPEND STANDALONE_MODULES_FOR_EXPORT ${PROJECT_NAME}_phobos_plugin)
endif()
endif()

add_library(${PROJECT_NAME} INTERFACE)
target_link_libraries(${PROJECT_NAME} INTERFACE ${PROJECT_NAME}_lib)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

add_library(${PROJECT_NAME}_cmake_modules INTERFACE)
target_include_directories(${PROJECT_NAME}_cmake_modules INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/infra/cmake>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}/modules>
        )

if(PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)

install(TARGETS 
        ${PROJECT_NAME}_main 
        ${STANDALONE_MODULES_FOR_EXPORT}
        LIBRARY 
                COMPONENT runtime
                DESTINATION ${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}
        RUNTIME 
                COMPONENT runtime
                DESTINATION bin
        )

if(HESTIA_BUILD_TESTS)
        set(TEST_MODULES_FOR_EXPORT
                ${PROJECT_NAME}_unit_tests
                ${PROJECT_NAME}_integration_tests
                ${PROJECT_NAME}_mock_phobos_plugin
                ${PROJECT_NAME}_mock_motr_plugin
                )

        install(TARGETS 
                ${TEST_MODULES_FOR_EXPORT}
                LIBRARY 
                        COMPONENT tests
                        DESTINATION ${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}
                RUNTIME 
                        COMPONENT tests
                        DESTINATION opt/${PROJECT_NAME}/bin
                )
                
        install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/test_data/"
                DESTINATION opt/${PROJECT_NAME}/test_data
                COMPONENT tests
                )

        install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/test/"
                DESTINATION opt/${PROJECT_NAME}/src
                COMPONENT tests
                PATTERN "data" EXCLUDE
                PATTERN "*.txt" EXCLUDE
                )
endif()

set(LINK_MODULES_FOR_EXPORT "")
if(CMAKE_BUILD_TYPE MATCHES Debug)
        list(APPEND LINK_MODULES_FOR_EXPORT development_flags)
        if(HESTIA_ENABLE_SANITIZERS)
                list(APPEND LINK_MODULES_FOR_EXPORT sanitizer_flags)
        endif()
endif()

install(TARGETS 
        ${PROJECT_NAME}_lib 
        ${LINK_MODULES_FOR_EXPORT}
        EXPORT ${PROJECT_NAME}-targets
        FILE_SET HEADERS
                COMPONENT devel
        LIBRARY
                COMPONENT runtime
                NAMELINK_COMPONENT devel
                DESTINATION ${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}
        INCLUDES 
                DESTINATION include/${PROJECT_NAME}
)

install(EXPORT ${PROJECT_NAME}-targets
        FILE ${PROJECT_NAME}Targets.cmake
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
        COMPONENT devel
        )

include(CMakePackageConfigHelpers)
write_basic_package_version_file(${PROJECT_NAME}ConfigVersion.cmake
    VERSION ${CMAKE_PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
        )

configure_file(infra/cmake/${PROJECT_NAME}Config.cmake.in ${PROJECT_NAME}Config.cmake @ONLY)
install(FILES   "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
                "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
        COMPONENT devel
        )

configure_file(infra/cmake/${PROJECT_NAME}.pc.in ${PROJECT_NAME}.pc @ONLY)
install(FILES   "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
        COMPONENT devel
        )

configure_file(infra/cmake/${PROJECT_NAME}d.service.in ${PROJECT_NAME}d.service @ONLY)
install(FILES    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}d.service"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/systemd/system
        COMPONENT runtime
        )

install(DIRECTORY    "${CMAKE_CURRENT_SOURCE_DIR}/bindings/python/hestia/"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}/python/hestia
        COMPONENT runtime
        PATTERN "__pycache__/*" EXCLUDE
        )

install(DIRECTORY    "${CMAKE_CURRENT_SOURCE_DIR}/test/data/configs/"
        DESTINATION "${CMAKE_INSTALL_SYSCONFDIR}/${PROJECT_NAME}/sample-configs"
        COMPONENT runtime
        )

configure_file(infra/cmake/${PROJECT_NAME}d.yaml.in ${PROJECT_NAME}d.yaml @ONLY)
install(FILES    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}d.yaml"
        DESTINATION "${CMAKE_INSTALL_SYSCONFDIR}/${PROJECT_NAME}"
        COMPONENT runtime
        )

set(CPACK_PACKAGE_VENDOR "Irish Centre for High End Computing (ICHEC)")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Hestia (Hierarchical Storage Tiers Interface for Applications)")
set(CPACK_PACKAGE_DESCRIPTION "Hestia (Hierarchical Storage Tiers Interface for Applications)")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE)
set(CPACK_RESOURCE_FILE_README ${CMAKE_CURRENT_SOURCE_DIR}/README.md)
set(CPACK_PACKAGE_HOMEPAGE_URL "https://git.ichec.ie/io-sea-internal/hestia")
set(CPACK_THREADS ${CMAKE_BUILD_PARALLEL_LEVEL})

set(HESTIA_CPACK_GENERATORS TGZ)
if(NOT APPLE)
        set(HESTIA_PACKAGE_TYPE RPM CACHE STRING "Linux package type: DEB or RPM")
        set(CPACK_RPM_PACKAGE_NAME ${PROJECT_NAME})

        set(CPACK_RPM_PACKAGE_SUMMARY "Hestia (Hierarchical Storage Tiers Interface for Applications)")
        set(CPACK_RPM_PACKAGE_AUTOREQ "no")
        set(CPACK_RPM_PACKAGE_LICENSE "MIT")
        set(CPACK_RPM_FILE_NAME "RPM-DEFAULT")

        if(HESTIA_USE_SRC_RPM_SPEC)
                set(CPACK_RPM_USER_BINARY_SPECFILE ${CMAKE_SOURCE_DIR}/infra/cmake/hestia.spec.in)
        endif()

        set(HESTIA_COMPONENTS runtime devel)
        cpack_add_component(runtime DESCRIPTION "Hestia Runtime Package")
        cpack_add_component(devel 
                DESCRIPTION "Hestia Development Package"
                DEPENDS runtime)
        
        if(HESTIA_BUILD_TESTS)
        cpack_add_component(tests
                DESCRIPTION "Hestia Unit Tests & Mock Interfaces Package"
                DEPENDS runtime)
        list(APPEND HESTIA_COMPONENTS tests)
        endif()

        if (HESTIA_WITH_PHOBOS)
        cpack_add_component(phobos
                DESCRIPTION "Hestia Phobos Plugin Integration"
                DEPENDS runtime)
        list(APPEND HESTIA_COMPONENTS phobos)
        endif()
        
        set(CPACK_COMPONENTS_ALL ${HESTIA_COMPONENTS})
        set(CPACK_RPM_COMPONENT_INSTALL ON)
        set(CPACK_RPM_MAIN_COMPONENT runtime) 

        set(CPACK_RPM_RUNTIME_PACKAGE_SUMMARY "Hestia HSM object store interface client and server applications and libraries")
        set(CPACK_RPM_DEVEL_PACKAGE_SUMMARY "Hestia HSM object store interface development headers")
        set(CPACK_RPM_PHOBOS_PACKAGE_SUMMARY "Phobos plugin package for the Hestia HSM object store library")

        set(CPACK_RPM_RUNTIME_PACKAGE_DESCRIPTION "Hestia HSM object store interface client and server applications and libraries")
        set(CPACK_RPM_DEVEL_PACKAGE_DESCRIPTION "Hestia HSM object store interface development headers")
        set(CPACK_RPM_PHOBOS_PACKAGE_DESCRIPTION "Phobos plugin package for the Hestia HSM object store library")

        set(CPACK_RPM_BUILDREQUIRES "wget, git, make, cmake, gcc-c++, binutils, elfutils, doxygen, openssl-devel, catch-devel, hiredis-devel, sqlite-devel, curl-devel, libxml2-devel, zlib-devel, json-devel")

        # Broken depends: yaml-cpp-devel, spdlog-devel
        set(CPACK_RPM_RUNTIME_PACKAGE_REQUIRES "openssl, hiredis, yaml-cpp, sqlite, libxml2, libcurl, zlib, spdlog")
        set(CPACK_RPM_DEVEL_PACKAGE_REQUIRES "hestia")
        set(CPACK_RPM_PHOBOS_PACKAGE_REQUIRES "hestia phobos")
        
        if (HESTIA_WITH_PHOBOS)
                set(CPACK_RPM_BUILDREQUIRES "${CPACK_RPM_BUILDREQUIRES} phobos-devel")
        endif()

        set(CPACK_RPM_INSTALL_WITH_EXEC ON)

        # Only build debuginfo if we have set RelWithDebInfo - it is not useful otherwise
        if(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
                set(CPACK_RPM_DEBUGINFO_PACKAGE ON)
        else()
                set(CPACK_RPM_DEBUGINFO_PACKAGE OFF)
        endif()
        message(STATUS "Will do debuginfo: " ${CPACK_RPM_DEBUGINFO_PACKAGE})
        
        set(CPACK_RPM_DEBUGINFO_SINGLE_PACKAGE ON)
        set(CPACK_RPM_CHANGELOG_FILE ${CMAKE_SOURCE_DIR}/infra/cmake/RpmChangelog)
        set(CPACK_RPM_SOURCE_PKG_BUILD_PARAMS "-DCMAKE_BUILD_TYPE=RelWithDebInfo -DHESTIA_BUILD_DOCUMENTATION=ON -DCMAKE_PROJECT_VERSION=${CMAKE_PROJECT_VERSION}")

        set(CPACK_DEBIAN_PACKAGE_NAME ${PROJECT_NAME})
        set(CPACK_DEBIAN_PACKAGE_MAINTAINER "support@ichec.ie")

        list(APPEND HESTIA_CPACK_GENERATORS ${HESTIA_PACKAGE_TYPE})
        list(APPEND HESTIA_SOURCE_GENERATORS ${HESTIA_PACKAGE_TYPE})
else()
        list(APPEND HESTIA_SOURCE_GENERATORS TGZ)
endif()

set(CPACK_GENERATOR ${HESTIA_CPACK_GENERATORS})
set(CPACK_SOURCE_GENERATOR ${HESTIA_SOURCE_GENERATORS})
set(CPACK_SOURCE_IGNORE_FILES
    /.git*
    /.clang*
    /.vscode
    /dist
    /examples
    /infra/containers
    /infra/ansible
    /infra/deps
    build/
    .cache/
    /\\\\.DS_Store
)

include(CPack)

else()
        message(STATUS "Skipping package targets due to PROJECT_SOURCE_DIR: " ${PROJECT_SOURCE_DIR} " not equal to CMAKE_SOURCE_DIR: " ${CMAKE_SOURCE_DIR})
endif()
