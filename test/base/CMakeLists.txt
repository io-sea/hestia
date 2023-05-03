include(FetchDependencies)

add_subdirectory(network)
add_subdirectory(storage)

if(HESTIA_BUILD_TESTS)

set(PLATFORM_LIBS)
if(NOT APPLE)
    list(APPEND PLATFORM_LIBS stdc++fs)
endif()

fetch_catch2()

include(external/code-coverage)
configure_file(utils/TestDirectories.h.in ${PROJECT_BINARY_DIR}/TestDirectories.h)

set(UNIT_TEST_MODULE ${PROJECT_NAME}_unit_tests)

set(UNIT_TEST_SOURCES
    common/TestBlockList.cc
    common/TestExtent.cc
    common/TestHashUtils.cc
    common/TestJsonUtils.cc
    common/TestStream.cc
    common/TestStringUtils.cc
    network/TestSocket.cc
    network/TestTcpServer.cc
    protocol/TestHttpRequest.cc
    protocol/TestS3Path.cc
    protocol/TestS3Error.cc
    storage/ObjectStoreTestWrapper.cc
    storage/TestBlockStore.cc
    storage/TestInMemoryObjectStoreClient.cc
    storage/TestFileObjectStoreClient.cc
    storage/TestPhobosClient.cc
    storage/TestS3Client.cc
    web/TestWebApp.cc
    web/TestS3Service.cc
    web/TestS3WebApp.cc
    web/TestS3Authorization.cc
)

add_executable(${UNIT_TEST_MODULE} 
    main.cc
    utils/TestContext.h 
    utils/TestContext.cc
    ${UNIT_TEST_SOURCES}
)

target_link_libraries(${UNIT_TEST_MODULE} PRIVATE 
    hestia_server 
    hestia_storage
    hestia_s3app
    Catch2::Catch2
    ${PLATFORM_LIBS}
)

target_include_directories(${UNIT_TEST_MODULE} PRIVATE utils ${PROJECT_BINARY_DIR})
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
    utils/TestContext.h 
    utils/TestContext.cc
    network/TestBasicHttpServer.cc
    network/TestCurlClient.cc
    storage/TestS3Client_Integration.cc
    )

if(${HESTIA_WITH_PROXYGEN})
    LIST(APPEND INTEGRATION_TEST_SOURCES 
        storage/TestProxygenServer.cc
        storage/TestProxygenMockPhobos.cc)
endif()

add_executable(${INTEGRATION_TEST_MODULE} ${INTEGRATION_TEST_SOURCES})

target_link_libraries(${INTEGRATION_TEST_MODULE} PRIVATE 
    hestia_http_client
    hestia_server
    hestia_web
    hestia_s3app
    Catch2::Catch2WithMain
    ${PLATFORM_LIBS})

if(${HESTIA_WITH_PROXYGEN})
    target_link_libraries(${INTEGRATION_TEST_MODULE} PRIVATE hestia_proxygen_plugin)
    target_link_directories(${INTEGRATION_TEST_MODULE} PUBLIC ${zstd_BINARY_DIR})
    target_include_directories(${INTEGRATION_TEST_MODULE} PUBLIC ${PROJECT_BINARY_DIR})
    target_compile_definitions(${INTEGRATION_TEST_MODULE} PUBLIC HAVE_PROXYGEN)
endif()

target_include_directories(${INTEGRATION_TEST_MODULE} PRIVATE utils)
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