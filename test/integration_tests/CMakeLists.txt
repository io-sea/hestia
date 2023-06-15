set(INTEGRATION_TEST_MODULE ${PROJECT_NAME}_integration_tests)

set(INTEGRATION_TEST_SOURCES     
    base/storage/TestS3Client_Integration.cc
    base/network/TestBasicHttpServer.cc
    base/network/TestCurlClient.cc
    hestia/TestHestiaWebApp.cc
    main.cc
    )

if(${HESTIA_WITH_PROXYGEN})
    LIST(APPEND INTEGRATION_TEST_SOURCES 
        base/storage/TestProxygenServer.cc
        base/storage/TestProxygenMockPhobos.cc
    )
endif()

add_executable(${INTEGRATION_TEST_MODULE} ${INTEGRATION_TEST_SOURCES})

target_link_libraries(${INTEGRATION_TEST_MODULE} PRIVATE 
    hestia_http_client
    hestia_app
    hestia_server
    hestia_web
    hestia_s3app
    hestia_mocks
    hestia_s3_backend
    hestia_event_feed
    hestia_test_utils
    Catch2::Catch2
    ${PLATFORM_LIBS})

if(${HESTIA_WITH_PROXYGEN})
    target_link_libraries(${INTEGRATION_TEST_MODULE} PRIVATE hestia_proxygen_plugin)
    target_link_directories(${INTEGRATION_TEST_MODULE} PUBLIC ${zstd_BINARY_DIR})
    target_include_directories(${INTEGRATION_TEST_MODULE} PUBLIC ${PROJECT_BINARY_DIR})
    target_compile_definitions(${INTEGRATION_TEST_MODULE} PUBLIC HAVE_PROXYGEN)
endif()

add_dependencies(${INTEGRATION_TEST_MODULE} hestia_mock_phobos_plugin hestia_mock_s3_plugin)

target_code_coverage(${INTEGRATION_TEST_MODULE})
catch_discover_tests(${INTEGRATION_TEST_MODULE})