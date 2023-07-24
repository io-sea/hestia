#include "hestia/hestia.h"


int main(int argc, char** argv)
{
    if (argc == 2) {
        hestia_initialize(argv[1], NULL, NULL);
    }
    else {
        hestia_initialize(NULL, NULL, NULL);
    }

    hestia_finish();

    return 0;

    /*
    char oid[HESTIA_UUID_SIZE] = {0};

    int rc = hestia_object_create(oid);

    char content[] = "The quick brown fox jumps over the lazy dog";

    hestia_object_put((const char*)&oid, (void*)&content, 0, 44, 0);

    hestia_object_move((const char*)&oid, 0, 1);

    char buffer[2048] = {0};

    hestia_object_get((const char*)&oid, &buffer, 0, 2048, 1);

    int ok = 1;
    for (size_t i = 0; i < 44; i++)
    {
        if (content[i] != buffer[i])
        {
            ok = 0;
            break;
        }
    }
    
    hestia_finish();

    if (ok == 1) {
        return 0;
    }
    else {
        return -1;
    }
    */
}

