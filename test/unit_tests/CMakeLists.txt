
set(UNIT_TEST_MODULE ${PROJECT_NAME}_unit_tests)

set(UNIT_TEST_SOURCES
    base/common/TestBlockList.cc
    base/common/TestExtent.cc
    base/common/TestDictionary.cc
    base/common/TestEnumUtils.cc
    base/common/TestErrorUtils.cc
    base/common/TestFileUtils.cc
    base/common/TestHashUtils.cc
    base/common/TestJsonUtils.cc
    base/common/TestLogger.cc
    base/common/TestStream.cc
    base/common/TestStringUtils.cc
    base/common/TestThreadUtils.cc
    base/common/TestTimeUtils.cc
    base/common/TestUuidUtils.cc
    base/common/TestXmlUtils.cc
    base/common/TestXmlParser.cc
    base/common/TestYamlUtils.cc
    base/common/TestSerialization.cc
    base/network/TestSocket.cc
    base/network/TestTcpServer.cc
    base/network/TestS3Client.cc
    base/protocol/TestHttpRequest.cc
    base/protocol/TestS3Path.cc
    base/protocol/TestS3Status.cc
    base/storage/TestBlockStore.cc
    base/storage/TestInMemoryObjectStoreClient.cc
    base/storage/TestFileObjectStoreClient.cc
    base/storage/TestPhobosClient.cc
    base/storage/TestS3ObjectStoreClient.cc
    base/crud/TestLockableModel.cc
    base/crud/TestCrudService.cc
    base/crud/TestHttpCrudClient.cc
    base/crud/TestCrudEventSink.cc 
    base/crud/TestEventFeed.cc
    base/web/TestWebApp.cc
    base/web/TestUserService.cc
    base/web/TestS3AuthorisationChecker.cc
    hsm/TestMockMotrBackend.cc
    hsm/TestMockMotrHsm.cc
    hsm/TestMotrHsmClient.cc
    hsm/TestStorageTier.cc
    hsm/TestHsmNode.cc
    hsm/TestHsmObject.cc
    hsm/TestTierExtents.cc
    hsm/TestHsmService.cc
    hsm/TestHsmEventSink.cc
    hsm/TestDistributedHsmService.cc
    hsm/TestDistributedHsmObjectStoreClient.cc
    hsm/TestObjectStoreClientManager.cc
    hestia/TestHestiaClient.cc
    hestia/TestHestiaCApi.cc
    hestia/TestHestiaHLCApi.cc
    hestia/TestHestiaCli.cc
    hestia/TestS3WebApp.cc
)

add_executable(${UNIT_TEST_MODULE}
    main.cc
    ${UNIT_TEST_SOURCES})

set(PLATFORM_LIBS)
if(NOT APPLE)
    list(APPEND PLATFORM_LIBS stdc++fs)
endif()

target_link_libraries(${UNIT_TEST_MODULE} PRIVATE
    hestia_lib
    hestia_mock_motr
    hestia_mocks
    hestia_test_utils
    ${PLATFORM_LIBS}
    Catch2::Catch2
)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    target_link_libraries(${UNIT_TEST_MODULE} PRIVATE development_flags)
endif()

add_dependencies(${UNIT_TEST_MODULE} hestia_mocks)

target_code_coverage(hestia_common ALL)
target_code_coverage(hestia_crud ALL)
target_code_coverage(hestia_app ALL)
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

target_code_coverage(hestia_hsm ALL)
target_code_coverage(hestia_main)
target_code_coverage(hestia_lib ALL)
target_code_coverage(${UNIT_TEST_MODULE} ALL)

catch_discover_tests(${UNIT_TEST_MODULE})

