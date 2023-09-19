file(COPY run_e2e_tests.py DESTINATION ${CMAKE_BINARY_DIR}/e2e_tests)
file(COPY hestia_tests DESTINATION ${CMAKE_BINARY_DIR}/e2e_tests)

add_custom_target(${PROJECT_NAME}_e2e_tests)

add_custom_command(
    TARGET ${PROJECT_NAME}_e2e_tests POST_BUILD
    COMMAND python3 e2e_tests/run_e2e_tests.py --source_dir ${CMAKE_SOURCE_DIR}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)