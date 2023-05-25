#include "MotrInterface.h"

int main()
{
    ClientConfig config;

    MotrInterface motr_interface;
    motr_interface.initializeClientInstance(config);

    motr_interface.initializeHsm();
    return 0;
}