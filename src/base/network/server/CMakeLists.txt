include(AddModule)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_module(
    MODULE_NAME server 
    HEADERS
        Server.h
        basic_server/BasicHttpServer.h 
        basic_server/TcpServer.h 
        basic_server/Socket.h
        basic_server/SocketState.h         
    SOURCES
        Server.cc
        basic_server/BasicHttpServer.cc 
        basic_server/TcpServer.cc 
        basic_server/Socket.cc
        basic_server/SocketState.cc  
    INTERNAL_INCLUDE_DIRS 
        basic_server
    INTERNAL_DEPENDENCIES 
        common
        web
    DEPENDENCIES
        Threads::Threads
)

if(HESTIA_WITH_PROXYGEN)
    add_subdirectory(proxygen)
endif()