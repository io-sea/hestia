add_subdirectory(mocks)

if(HESTIA_BUILD_TESTS)
    include(FetchDependencies)
    fetch_catch2()

    file(COPY data/ DESTINATION ${CMAKE_BINARY_DIR}/test_data)
    file(COPY data/web_content DESTINATION ${CMAKE_BINARY_DIR})

    add_subdirectory(utils)
    add_subdirectory(unit_tests)
    add_subdirectory(integration_tests)
    add_subdirectory(e2e_tests)
endif()
