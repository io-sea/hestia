set(LINK_MODULES_FOR_EXPORT
        cli 
        common 
        server 
        protocol 
        storage 
        web
        s3app
        hsm
        event_feed
        copytool_lib
        http_client
        hsm_object_store_common
        hsm_object_store_client
        hsm_object_store
        app
        headers
)

set(STANDALONE_MODULES_FOR_EXPORT
        phobos_backend
        phobos_backend_interface_impl
        mock_phobos
        mock_phobos_plugin
        s3_backend
        mocks
        mock_s3
        mock_s3_plugin
        main 
        lib
)

if(HESTIA_WITH_S3_CLIENT)
        list(APPEND STANDALONE_MODULES_FOR_EXPORT s3_plugin)
endif()

if(HESTIA_WITH_PHOBOS)
        if(NOT APPLE)
                list(APPEND STANDALONE_MODULES_FOR_EXPORT phobos_impl phobos_plugin)
        endif()
endif()

list(TRANSFORM STANDALONE_MODULES_FOR_EXPORT PREPEND "${PROJECT_NAME}_")

list(TRANSFORM LINK_MODULES_FOR_EXPORT PREPEND "${PROJECT_NAME}_")

if(CMAKE_BUILD_TYPE MATCHES Debug)
        list(APPEND LINK_MODULES_FOR_EXPORT development_flags)
endif()

list(APPEND LINK_MODULES_FOR_EXPORT nlohmann_json spdlog yaml-cpp hiredis_static)

add_library(${PROJECT_NAME} INTERFACE)
target_link_libraries(${PROJECT_NAME} INTERFACE ${LINK_MODULES_FOR_EXPORT})

add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
add_library(${PROJECT_NAME}::${PROJECT_NAME}_lib ALIAS ${PROJECT_NAME}_lib)

add_library(${PROJECT_NAME}_cmake_modules INTERFACE)
target_include_directories(${PROJECT_NAME}_cmake_modules INTERFACE 
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/infra/cmake>
        $<INSTALL_INTERFACE:lib/cmake/${PROJECT_NAME}/modules>
        )

if(PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
install(DIRECTORY infra/cmake/${PROJECT_NAME}/ DESTINATION lib/cmake/${PROJECT_NAME}/modules/${PROJECT_NAME})

install(DIRECTORY external DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} FILES_MATCHING PATTERN "*.h")

install(TARGETS ${PROJECT_NAME} ${LINK_MODULES_FOR_EXPORT} ${STANDALONE_MODULES_FOR_EXPORT} ${PROJECT_NAME}_cmake_modules
        EXPORT ${PROJECT_NAME}-targets
        LIBRARY DESTINATION lib/${PROJECT_NAME}
        ARCHIVE DESTINATION lib/${PROJECT_NAME}
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
        )

install(EXPORT ${PROJECT_NAME}-targets
        FILE ${PROJECT_NAME}Targets.cmake
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION lib/cmake/${PROJECT_NAME}
         )

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        ${PROJECT_NAME}ConfigVersion.cmake
    VERSION ${CMAKE_PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_file(infra/cmake/${PROJECT_NAME}Config.cmake.in ${PROJECT_NAME}Config.cmake @ONLY)
install(FILES   "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
                "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION lib/cmake/${PROJECT_NAME}
        )

configure_file(infra/cmake/${PROJECT_NAME}.pc.in ${PROJECT_NAME}.pc @ONLY)
install(FILES   "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc"
        DESTINATION lib/pkgconfig
        )

set(CPACK_PACKAGE_VENDOR "Irish Centre for High End Computing (ICHEC)")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Hierarchical Storage Tiers Interface for Applications")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

set(HESTIA_CPACK_GENERATORS TGZ)
if(NOT APPLE)
        set(HESTIA_PACKAGE_TYPE RPM CACHE STRING "Linux package type: DEB or RPM")
        set(CPACK_RPM_PACKAGE_NAME ${PROJECT_NAME})
        set(CPACK_RPM_PACKAGE_REQUIRES "libcurl openssl libxml2")
        set(CPACK_RPM_PACKAGE_AUTOREQ "no")

        set(CPACK_DEBIAN_PACKAGE_NAME ${PROJECT_NAME})
        set(CPACK_DEBIAN_PACKAGE_MAINTAINER "support@ichec.ie")

        list(APPEND HESTIA_CPACK_GENERATORS ${HESTIA_PACKAGE_TYPE})
endif()

set(CPACK_GENERATOR ${HESTIA_CPACK_GENERATORS})

set(CPACK_SOURCE_GENERATOR "TGZ;ZIP")
set(CPACK_SOURCE_IGNORE_FILES
    /.git
    /dist
    /.*build.*
    /\\\\.DS_Store
)

include(CPack)

else()
        message(STATUS "Skipping package targets due to PROJECT_SOURCE_DIR: " ${PROJECT_SOURCE_DIR} " not equal to CMAKE_SOURCE_DIR: " ${CMAKE_SOURCE_DIR}) 
endif()

