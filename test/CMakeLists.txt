add_subdirectory(mocks)

if(HESTIA_BUILD_TESTS)
    include(external/code-coverage)
    add_code_coverage_all_targets()

    include(FetchDependencies)
    fetch_catch2()

    file(COPY data/ DESTINATION ${CMAKE_BINARY_DIR}/test_data)

    add_subdirectory(utils)
    add_subdirectory(unit_tests)
    add_subdirectory(integration_tests)
    add_subdirectory(e2e_tests)
endif()
