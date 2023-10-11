add_subdirectory(mocks)

if(HESTIA_BUILD_TESTS)
    include(external/code-coverage)
    add_code_coverage_all_targets()

    include(FetchDependencies)
    fetch_catch2()

    file(COPY data/ DESTINATION ${CMAKE_BINARY_DIR}/test_data)
    file(COPY data/web_content DESTINATION ${CMAKE_BINARY_DIR})

    add_subdirectory(utils)
    add_subdirectory(unit_tests)
    add_subdirectory(integration_tests)
    add_subdirectory(e2e_tests)

    add_custom_target(${PROJECT_NAME}_ci_tests)
    add_custom_command(
        TARGET ${PROJECT_NAME}_ci_tests POST_BUILD
        COMMAND python3 -m pytest --verbose ${CMAKE_SOURCE_DIR}/infra/scripts/ci
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} VERBATIM
    )

endif()
