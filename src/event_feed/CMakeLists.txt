include(AddModule)

add_module(
    MODULE_NAME event_feed 
    HEADERS
        EventFeed.h
        RbhEvent.h
    SOURCES
        EventFeed.cc
        RbhEvent.cc
    DEPENDENCIES
        hestia::hestia
    INTERNAL_DEPENDENCIES
        common
)