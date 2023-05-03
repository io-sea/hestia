configure_file(ProjectConfig.h.in ${PROJECT_BINARY_DIR}/ProjectConfig.h)
install(FILES ${PROJECT_BINARY_DIR}/ProjectConfig.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

add_subdirectory(common)
add_subdirectory(protocol)
add_subdirectory(network)
add_subdirectory(storage)
add_subdirectory(web)
add_subdirectory(cli)
