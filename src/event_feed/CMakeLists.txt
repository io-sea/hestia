include(AddModule)

add_module(
    MODULE_NAME event_feed 
    HEADERS
        EventFeed.h
        RbhEvent.h
    SOURCES
        EventFeed.cc
        RbhEvent.cc
    INTERNAL_DEPENDENCIES
        common
)