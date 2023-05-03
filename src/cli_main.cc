#include "hestia.h"

//#include "HestiaConfig.h"

//#include <hestia/CommandLineArgs.h>

#include <iostream>

/*
TierBackendRegistry gAllFileBackendExample = {
    {000, ObjectStoreClientType::FILE},
    {100, ObjectStoreClientType::FILE},
    {200, ObjectStoreClientType::FILE},
    {300, ObjectStoreClientType::FILE},
    };

TierBackendRegistry gFileS3BackendExample = {
    {000, ObjectStoreClientType::FILE},
    {100, ObjectStoreClientType::FILE},
    {200, ObjectStoreClientType::FILE},
    {300, ObjectStoreClientType::S3},
    };


int main(int argc, char** argv)
{
        hestia::CommandLineArgs arg_parser;
        arg_parser.parseArgs(argc, argv);

        const auto args = arg_parser.getArgs();

        hestia::HestiaConfig config;
        config.mKeyValueStoreType   = KeyValueStoreType::FILE;
        config.mPlacementEngineType = PlacementEngineType::BASIC;
        // config.mTierBackendRegistry = gAllFileBackendExample;

        hestia::HestiaConfigurator configurator;
        configurator.initialize(config);
    return 0;
}

*/