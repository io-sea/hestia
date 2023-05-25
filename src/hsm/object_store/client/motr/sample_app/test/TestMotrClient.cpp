#include <catch2/catch.hpp>

#include "MotrInterface.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "module/instance.h"
#include "ut/ut.h"
#include "ut/module.h" 
#include "motr/ut/client.h"
#include "lib/uuid.h"
#ifdef __cplusplus
}
#endif

#define UT_SANDBOX "./ut-sandbox"

class TestMotrInterface : public MotrInterface
{
    public:

    void initializeClientInstance(const ClientConfig& config) override
    {
        const auto ret = ut_m0_client_init(&mClientInstance);
        REQUIRE(ret >= 0);
        ut_m0_client_fini(&mClientInstance);
    }

};

TEST_CASE("Init simple test client")
{
    m0 instance;
    m0_instance_setup(&instance);
    m0_ut_module_type.mt_create(&instance);

    m0_ut_module* ut = (m0_ut_module*)instance.i_moddata[M0_MODULE_UT];
	ut->ut_sandbox = UT_SANDBOX;

	auto rc = m0_module_init(&instance.i_self, M0_LEVEL_INST_ONCE);
    REQUIRE(rc == 0);

    m0_node_uuid_string_set(NULL);

    rc = m0_ut_init(&instance);
    REQUIRE(rc == 0);

    ClientConfig config;

    TestMotrInterface motr_interface;
    motr_interface.initializeClientInstance(config);

    m0_ut_fini();

    //motr_interface.initializeHsm();

    REQUIRE(false);
}