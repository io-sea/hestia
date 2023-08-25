macro(add_module)
    set(options SHARED WITH_FILESYSTEM NO_INSTRUMENT)
    set(oneValueArgs MODULE_NAME)
    set(multiValueArgs HEADERS SOURCES INCLUDE_DIRS INTERNAL_INCLUDE_DIRS DEPENDENCIES 
        PRIVATE_DEPENDENCIES INTERNAL_DEPENDENCIES COMPILER_DEFINITIONS)
    cmake_parse_arguments(ADD_MODULE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

    set(TARGET_NAME ${PROJECT_NAME}_${ADD_MODULE_MODULE_NAME})

    if(${ADD_MODULE_SHARED})
        add_library(${TARGET_NAME} SHARED ${ADD_MODULE_HEADERS} ${ADD_MODULE_SOURCES})
    else()
        add_library(${TARGET_NAME} ${ADD_MODULE_HEADERS} ${ADD_MODULE_SOURCES})
    endif()

    list(TRANSFORM ADD_MODULE_INTERNAL_INCLUDE_DIRS PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")
    target_include_directories(${TARGET_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        "$<BUILD_INTERFACE:${ADD_MODULE_INTERNAL_INCLUDE_DIRS}>"
        "$<BUILD_INTERFACE:${ADD_MODULE_INCLUDE_DIRS}>"
    )
    if(NOT PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
        # If we are here another project has included us - we want them to be able to include 
        # our headers with a prefix like the install targets do, i.e. <myProject/myHeader.h>
        # So, we set up a flat directory structure in the build output to allow it.
        add_custom_target(${TARGET_NAME}_copy_source)
        set(MODULE_HEADER_PATHS ${ADD_MODULE_HEADERS})
        list(TRANSFORM MODULE_HEADER_PATHS PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")
        add_custom_command(TARGET ${TARGET_NAME}_copy_source PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/include/${PROJECT_NAME}/
            COMMAND ${CMAKE_COMMAND} -E copy ${MODULE_HEADER_PATHS} ${PROJECT_BINARY_DIR}/include/${PROJECT_NAME}/
            )
        add_dependencies(${TARGET_NAME} ${TARGET_NAME}_copy_source)
        target_include_directories(${TARGET_NAME} PUBLIC
            "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include/>"
        )
    endif()
    
    list(TRANSFORM ADD_MODULE_INTERNAL_DEPENDENCIES PREPEND ${PROJECT_NAME}_)
    target_link_libraries(${TARGET_NAME} PUBLIC ${ADD_MODULE_INTERNAL_DEPENDENCIES} ${ADD_MODULE_DEPENDENCIES})
    target_link_libraries(${TARGET_NAME} PRIVATE ${ADD_MODULE_PRIVATE_DEPENDENCIES})

    if(CMAKE_BUILD_TYPE MATCHES Debug AND (NOT ${ADD_MODULE_NO_INSTRUMENT}))
        target_link_libraries(${TARGET_NAME} PUBLIC development_flags)
    endif()

    if(${ADD_MODULE_WITH_FILESYSTEM})
        if(NOT APPLE)
            target_link_libraries(${TARGET_NAME} PUBLIC stdc++fs)
        endif()
    endif()

    target_compile_definitions(${TARGET_NAME} PUBLIC ${ADD_MODULE_COMPILER_DEFINITIONS})
endmacro()