
macro(get_project_sources)
    file(GLOB_RECURSE PROJECT_HEADERS ${PROJECT_SOURCE_DIR}/src/*.h)
    file(GLOB_RECURSE PROJECT_SOURCES ${PROJECT_SOURCE_DIR}/src/*.cc)
    file(GLOB_RECURSE TEST_HEADERS ${PROJECT_SOURCE_DIR}/test/*.h)
    file(GLOB_RECURSE TEST_SOURCES ${PROJECT_SOURCE_DIR}/test/*.cc)
    list(APPEND ALL_SOURCE_FILES ${PROJECT_HEADERS} ${PROJECT_SOURCES} ${TEST_HEADERS} ${TEST_SOURCES})
endmacro()

function(add_clang_format)
    find_program(CLANG_FORMAT_EXE clang-format NAMES clang-format-11
        HINTS /opt/homebrew/opt/clang-format@11/bin/
        )

    if(${CLANG_FORMAT_EXE-NOTFOUND})
        message(WARNING "Failed to find clang-format, not generating format targets.")
    else()
        get_project_sources()

        add_custom_target(${PROJECT_NAME}_check_format)
        add_custom_command(TARGET ${PROJECT_NAME}_check_format PRE_BUILD
            COMMAND ${CLANG_FORMAT_EXE} --dry-run --style=file --Werror -fallback-style=none ${ALL_SOURCE_FILES}
        )

        add_custom_target(${PROJECT_NAME}_do_format)
        add_custom_command(TARGET ${PROJECT_NAME}_do_format PRE_BUILD
            COMMAND ${CLANG_FORMAT_EXE} -i --style=file --Werror -fallback-style=none ${ALL_SOURCE_FILES}
        )   
    endif()
endfunction()

function(add_clang_tidy)
    add_custom_target(${PROJECT_NAME}_fetch_lint_dependencies)
    add_dependencies(${PROJECT_NAME}_fetch_lint_dependencies nlohmann_json spdlog yaml-cpp hiredis)

    if(${HESTIA_BUILD_TESTS})
        fetch_catch2()
        add_dependencies(${PROJECT_NAME}_fetch_lint_dependencies Catch2::Catch2)
    endif()
    if(NOT APPLE AND ${HESTIA_WITH_PHOBOS})
        find_package(phobos QUIET)    
        if(NOT ${phobos_FOUND})
            build_phobos()
            add_dependencies(${PROJECT_NAME}_fetch_lint_dependencies phobos)
        endif()
    endif()

    find_program(CLANG_TIDY_EXE clang-tidy 
        HINTS /opt/homebrew/opt/llvm/bin/
        )

    if(${CLANG_TIDY_EXE-NOTFOUND})
        message(WARNING "Failed to find clang-tidy, not generating tidy targets.")
    else()
        file(GLOB_RECURSE PROJECT_SOURCES ${PROJECT_SOURCE_DIR}/src/*.cc)
        file(GLOB_RECURSE TEST_SOURCES ${PROJECT_SOURCE_DIR}/test/*.cc)
        list(APPEND ALL_SOURCE_FILES ${PROJECT_SOURCES} ${TEST_SOURCES})

        add_custom_target(${PROJECT_NAME}_check_tidy)
        add_custom_command(TARGET ${PROJECT_NAME}_check_tidy PRE_BUILD
            COMMAND find ${ALL_SOURCE_FILES} -print0 | xargs -0 -P 0 ${CLANG_TIDY_EXE} -p ${PROJECT_BINARY_DIR} --quiet --header-filter=.* 
        )

        add_custom_target(${PROJECT_NAME}_do_tidy)
        add_custom_command(TARGET ${PROJECT_NAME}_do_tidy PRE_BUILD
            COMMAND find ${ALL_SOURCE_FILES} -print0 | xargs -0 -P 0 ${CLANG_TIDY_EXE} -p ${PROJECT_BINARY_DIR} --fix-errors --header-filter=.* 
        )   
    endif()
endfunction()

