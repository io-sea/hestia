#pragma once

#include "BasicDataPlacementEngine.h"

#ifdef HAS_ROBINHOOD
#include "RobinhoodPlacementEngine.h"
#endif

#include <string>

enum class PlacementEngineType
{
    BASIC,
    ROBINHOOD
};

namespace PlacementEngineRegistry{

    static std::string toString(PlacementEngineType placementEngineType)
    {
        switch(placementEngineType)
        {
            case PlacementEngineType::BASIC:
                return "BASIC";
            case PlacementEngineType::ROBINHOOD:
                return "ROBINHOOD";
            default:
                return "UNKNOWN";
        }
    };

    static bool isPlacementEngineAvailable(PlacementEngineType placementEngineType)
    {
        switch(placementEngineType)
        {
            case PlacementEngineType::BASIC:
                return true;
            case PlacementEngineType::ROBINHOOD:
            #ifdef HAS_ROBINHOOD
                return true;
            #else 
                return false;
            #endif
            default:
                return false;
        }
    };

    static std::unique_ptr<DataPlacementEngine> getEngine(PlacementEngineType placementEngineType)
    {
        switch(placementEngineType)
        {
            case PlacementEngineType::BASIC:
                return std::make_unique<BasicDataPlacementEngine>();
            case PlacementEngineType::ROBINHOOD:
            #ifdef HAS_ROBINHOOD
                return std::make_unique<RobinhoodDataPlacementEngine>();
            #else 
                return nullptr;
            #endif
            default:
                return nullptr;
        }
    };

}