include(ostk/AddModule)

add_module(
    MODULE_NAME event_feed 
    HEADERS
        EventFeed.h
    SOURCES
        EventFeed.cc
    DEPENDENCIES
        ostk::ostk
)