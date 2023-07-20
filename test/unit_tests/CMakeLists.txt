
set(UNIT_TEST_MODULE ${PROJECT_NAME}_unit_tests)

include(external/code-coverage)

set(UNIT_TEST_SOURCES
    base/common/TestBlockList.cc
    base/common/TestExtent.cc
    base/common/TestDictionary.cc
    base/common/TestHashUtils.cc
    base/common/TestJsonUtils.cc
    base/common/TestStream.cc
    base/common/TestStringUtils.cc
    base/common/TestUuidUtils.cc
    base/common/TestYamlUtils.cc
    base/network/TestSocket.cc
    base/network/TestTcpServer.cc
    base/protocol/TestHttpCrudClient.cc
    base/protocol/TestHttpRequest.cc
    base/protocol/TestS3Path.cc
    base/protocol/TestS3Error.cc
    base/storage/TestBlockStore.cc
    base/storage/TestInMemoryObjectStoreClient.cc
    base/storage/TestFileObjectStoreClient.cc
    base/storage/TestPhobosClient.cc
    base/storage/TestS3Client.cc
    base/web/TestWebApp.cc
    base/web/TestS3Authorization.cc
    base/web/TestUserService.cc
    hestia/TestHestiaCppClient.cc
    hestia/TestS3WebApp.cc
    hsm/TestDistributedHsmService.cc
    hsm/TestEventFeed.cc
    hsm/TestMockMotrBackend.cc
    hsm/TestMockMotrHsm.cc
    hsm/TestMotrHsmClient.cc
    hsm/TestHsmService.cc
    hsm/TestS3HsmService.cc
    hsm/TestHsmNodeAdapter.cc
    hsm/TestObjectService.cc
    hsm/TestTierService.cc
)

add_executable(${UNIT_TEST_MODULE} 
    main.cc
    ${UNIT_TEST_SOURCES})

set(PLATFORM_LIBS)
if(NOT APPLE)
    list(APPEND PLATFORM_LIBS stdc++fs)
endif()

target_link_libraries(${UNIT_TEST_MODULE} PRIVATE 
    hestia_app
    hestia_lib 
    hestia_server 
    hestia_storage
    hestia_event_feed
    hestia_mock_motr
    hestia_mocks
    hestia_test_utils
    ${PLATFORM_LIBS}
    Catch2::Catch2
)

add_dependencies(${UNIT_TEST_MODULE} hestia_mocks)

target_code_coverage(hestia_common ALL)
target_code_coverage(hestia_server ALL)
target_code_coverage(hestia_storage ALL)
target_code_coverage(hestia_mock_phobos_plugin ALL)
if(NOT APPLE AND ${HESTIA_WITH_PHOBOS})
target_code_coverage(hestia_phobos_plugin ALL)
endif()
target_code_coverage(hestia_mock_motr_plugin ALL)

if(${HESTIA_WITH_MOTR})
target_code_coverage(hestia_motr_plugin ALL)
endif()

target_code_coverage(hestia_mock_s3_plugin ALL)
target_code_coverage(hestia_s3_plugin ALL)
target_code_coverage(hestia_main)
target_code_coverage(hestia_lib ALL)
# target_code_coverage(hestia_event_feed)

target_code_coverage(${UNIT_TEST_MODULE} ALL)
catch_discover_tests(${UNIT_TEST_MODULE})
