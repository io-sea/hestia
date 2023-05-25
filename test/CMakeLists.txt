include(FetchDependencies)

add_subdirectory(base/network)
add_subdirectory(base/storage)
add_subdirectory(hsm_object_store)

if(HESTIA_BUILD_TESTS)
fetch_catch2()

add_subdirectory(utils)

include(external/code-coverage)

set(UNIT_TEST_MODULE ${PROJECT_NAME}_unit_tests)

set(UNIT_TEST_SOURCES
    base/common/TestBlockList.cc
    base/common/TestExtent.cc
    base/common/TestHashUtils.cc
    base/common/TestJsonUtils.cc
    base/common/TestStream.cc
    base/common/TestStringUtils.cc
    base/common/TestYamlUtils.cc
    base/network/TestSocket.cc
    base/network/TestTcpServer.cc
    base/protocol/TestHttpRequest.cc
    base/protocol/TestS3Path.cc
    base/protocol/TestS3Error.cc
    base/storage/ObjectStoreTestWrapper.cc
    base/storage/TestBlockStore.cc
    base/storage/TestInMemoryObjectStoreClient.cc
    base/storage/TestFileObjectStoreClient.cc
    base/storage/TestPhobosClient.cc
    base/storage/TestS3Client.cc
    base/web/TestWebApp.cc
    base/web/TestS3Service.cc
    base/web/TestS3WebApp.cc
    base/web/TestS3Authorization.cc
    hsm_object_store/motr/TestMockMotrBackend.cc
    hsm_object_store/motr/TestMockMotrHsm.cc
    hsm_object_store/motr/TestMotrHsmClient.cc
    hsm_object_store/HsmObjectStoreTestWrapper.cc
    hestia_client/TestHestiaCppClient.cc
    hsm_service/TestHsmService.cc
    hsm_service/TestObjectService.cc
    event_feed/TestEventFeed.cc
)

add_executable(${UNIT_TEST_MODULE} 
    main.cc
    ${UNIT_TEST_SOURCES})

set(PLATFORM_LIBS)
if(NOT APPLE)
    list(APPEND PLATFORM_LIBS stdc++fs)
endif()

target_link_libraries(${UNIT_TEST_MODULE} PRIVATE 
    hestia_client 
    hestia_motr_plugin_mock
    hestia_lib 
    hestia_server 
    hestia_storage
    hestia_s3app
    hestia_lib
    hestia_event_feed
    hestia_test_utils
    ${PLATFORM_LIBS}
    Catch2::Catch2
)

target_include_directories(${UNIT_TEST_MODULE} PUBLIC hsm_object_store)

# There is an inconsistency in clang/gcc application of this - make it explicit
target_compile_options(${UNIT_TEST_MODULE} PRIVATE -Wunused-parameter)
file(COPY data/ DESTINATION ${CMAKE_BINARY_DIR}/test_data)

add_dependencies(${UNIT_TEST_MODULE} hestia_mock_phobos_plugin hestia_mock_s3_plugin hestia_mock_motr_plugin)

target_code_coverage(hestia_common)
target_code_coverage(hestia_server)
target_code_coverage(hestia_storage)
target_code_coverage(hestia_mock_phobos_plugin)
if(NOT APPLE AND ${HESTIA_WITH_PHOBOS})
target_code_coverage(hestia_phobos_plugin)
endif()
target_code_coverage(hestia_mock_motr_plugin)

if(${HESTIA_WITH_MOTR})
target_code_coverage(hestia_motr_plugin)
endif()

target_code_coverage(hestia_mock_s3_plugin)
target_code_coverage(hestia_s3_plugin)
target_code_coverage(hestia_main)
target_code_coverage(hestia_lib)
# target_code_coverage(hestia_event_feed)

target_code_coverage(${UNIT_TEST_MODULE})
catch_discover_tests(${UNIT_TEST_MODULE})

set(INTEGRATION_TEST_MODULE ${PROJECT_NAME}_integration_tests)

set(INTEGRATION_TEST_SOURCES     
    base/storage/ObjectStoreTestWrapper.cc
    base/network/TestBasicHttpServer.cc
    base/network/TestCurlClient.cc
    base/storage/TestS3Client_Integration.cc
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
    hestia_client
    hestia_server
    hestia_web
    hestia_s3app
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

add_subdirectory(e2e_tests)

endif()
