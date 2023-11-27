include(FetchDependencies)

fetch_sqlite()

add_executable(hestia_policy_engine 
    policy_main.cc 
    PolicyEngine.cc
    ${sqlite_SOURCE_DIR}/sqlite3.c)

target_compile_options(hestia_policy_engine PUBLIC -Wno-implicit-fallthrough -Wno-error=implicit-fallthrough)

target_link_libraries(hestia_policy_engine PRIVATE hestia_lib hestia_app)
if(CMAKE_BUILD_TYPE MATCHES Debug)
    target_link_libraries(hestia_policy_engine PRIVATE development_flags)
endif()
if(NOT APPLE)
    target_link_libraries(hestia_policy_engine PRIVATE stdc++fs)
endif()
target_include_directories(hestia_policy_engine SYSTEM PRIVATE ${sqlite_SOURCE_DIR})

set_target_properties(hestia_policy_engine PROPERTIES OUTPUT_NAME hestia_policy_engine)
set_target_properties(hestia_policy_engine PROPERTIES SKIP_BUILD_RPATH FALSE)
set_target_properties(hestia_policy_engine PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE)
set_target_properties(hestia_policy_engine PROPERTIES INSTALL_RPATH "${RPATH_ORIGIN_VAR}/../lib/${PROJECT_NAME};${RPATH_ORIGIN_VAR}/../lib")

add_executable(hestia_sync_tool
    sync_main.cc 
    SyncTool.cc)

target_link_libraries(hestia_sync_tool PRIVATE hestia_lib hestia_app)
if(CMAKE_BUILD_TYPE MATCHES Debug)
    target_link_libraries(hestia_sync_tool PRIVATE development_flags)
endif()
if(NOT APPLE)
    target_link_libraries(hestia_sync_tool PRIVATE stdc++fs)
endif()

set_target_properties(hestia_sync_tool PROPERTIES OUTPUT_NAME hestia_sync_tool)
set_target_properties(hestia_sync_tool PROPERTIES SKIP_BUILD_RPATH FALSE)
set_target_properties(hestia_sync_tool PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE)
set_target_properties(hestia_sync_tool PROPERTIES INSTALL_RPATH "${RPATH_ORIGIN_VAR}/../lib/${PROJECT_NAME};${RPATH_ORIGIN_VAR}/../lib")

add_executable(hestia_datamover
    datamover_main.cc 
    Datamover.cc)

target_link_libraries(hestia_datamover PRIVATE hestia_lib hestia_app)
if(CMAKE_BUILD_TYPE MATCHES Debug)
    target_link_libraries(hestia_datamover PRIVATE development_flags)
endif()
if(NOT APPLE)
    target_link_libraries(hestia_datamover PRIVATE stdc++fs)
endif()

set_target_properties(hestia_datamover PROPERTIES OUTPUT_NAME hestia_datamover)
set_target_properties(hestia_datamover PROPERTIES SKIP_BUILD_RPATH FALSE)
set_target_properties(hestia_datamover PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE)
set_target_properties(hestia_datamover PROPERTIES INSTALL_RPATH "${RPATH_ORIGIN_VAR}/../lib/${PROJECT_NAME};${RPATH_ORIGIN_VAR}/../lib")