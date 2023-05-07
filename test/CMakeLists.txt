include(FetchDependencies)

add_subdirectory(base/network)
add_subdirectory(base/storage)
add_subdirectory(hsm_object_store)

if(HESTIA_BUILD_TESTS)
fetch_catch2()

add_subdirectory(base/utils)

include(external/code-coverage)

set(UNIT_TEST_MODULE ${PROJECT_NAME}_unit_tests)

set(UNIT_TEST_SOURCES
    base/common/TestBlockList.cc
    base/common/TestExtent.cc
    base/common/TestHashUtils.cc
    base/common/TestJsonUtils.cc
    base/common/TestStream.cc
    base/common/TestStringUtils.cc
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
    hestia_client/put_get.cc
    hestia_client/set_get_attrs.cc
    hestia_client/list_attrs.cc
    hestia_client/remove.cc
    hestia_client/copy.cc
    hestia_client/move.cc
    hestia_client/release.cc
    hestia_client/locate.cc
    hestia_client/list.cc
    hestia_client/get_tiers_info.cc
    hestia_client/list_tiers.cc
    hestia_client/main_tests.cc
    hsm_object_store/motr/TestMockMotrBackend.cc
    hsm_object_store/motr/TestMockMotrHsm.cc
    hsm_object_store/motr/TestMotrHsmClient.cc
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
    hestia_test_utils
    ${PLATFORM_LIBS}
    Catch2::Catch2
)

# There is an inconsistency in clang/gcc application of this - make it explicit
target_compile_options(${UNIT_TEST_MODULE} PRIVATE -Wunused-parameter)

add_dependencies(${UNIT_TEST_MODULE} hestia_mock_phobos_plugin hestia_mock_s3_plugin)

target_code_coverage(hestia_common)
target_code_coverage(hestia_server)
target_code_coverage(hestia_storage)
target_code_coverage(hestia_mock_phobos_plugin)
target_code_coverage(hestia_mock_s3_plugin)

target_code_coverage(${UNIT_TEST_MODULE})
catch_discover_tests(${UNIT_TEST_MODULE})

set(INTEGRATION_TEST_MODULE ${PROJECT_NAME}_integration_tests)

set(INTEGRATION_TEST_SOURCES     
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
    hestia_server
    hestia_web
    hestia_s3app
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

add_custom_target(
    py_integration_tests
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
    ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
)

add_custom_command(
    TARGET py_integration_tests POST_BUILD
    COMMAND python3 -m unittest -f client
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

endif()
