set(INTEGRATION_TEST_MODULE ${PROJECT_NAME}_integration_tests)

set(INTEGRATION_TEST_SOURCES
    base/common/TestSystemUtils.cc
    base/network/TestBasicHttpServer.cc
    base/network/TestCurlClient.cc
    base/network/TestS3ClientRealBackend.cc
    hestia/TestHestiaWebApp.cc
    hestia/TestHestiaS3WebApp.cc
    main.cc
    )

    if(${HESTIA_TEST_REDIS})
    LIST(APPEND INTEGRATION_TEST_SOURCES
        base/storage/TestRedisKeyValueStore.cc
    )
    endif()

    if(${HESTIA_WITH_PHOBOS})
    LIST(APPEND INTEGRATION_TEST_SOURCES
        base/storage/TestPhobosBackend.cc
    )
    endif()

add_executable(${INTEGRATION_TEST_MODULE} ${INTEGRATION_TEST_SOURCES})

target_link_libraries(${INTEGRATION_TEST_MODULE} PRIVATE
    hestia_lib
    hestia_mocks
    hestia_test_utils
    Catch2::Catch2
    ${PLATFORM_LIBS})

if(${HESTIA_TEST_REDIS})
    target_compile_definitions(${INTEGRATION_TEST_MODULE} PUBLIC TEST_REDIS)
endif()

if(${HESTIA_WITH_PHOBOS})
    add_dependencies(${INTEGRATION_TEST_MODULE} hestia_phobos_plugin)
    target_compile_definitions(${INTEGRATION_TEST_MODULE} PUBLIC HAS_PHOBOS)
endif()

add_dependencies(${INTEGRATION_TEST_MODULE} hestia_mock_phobos_plugin)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    target_link_libraries(${INTEGRATION_TEST_MODULE} PRIVATE development_flags)
endif()

target_code_coverage(${INTEGRATION_TEST_MODULE} ALL)

catch_discover_tests(${INTEGRATION_TEST_MODULE})
