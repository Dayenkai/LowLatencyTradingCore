#include "../../includes/Utils.h"

void        pinThreadToCore(pthread_t &thread, uint32_t   &coreId)
{
    cpu_set_t  cpuset;

    CPU_ZERO(&cpuset);
    CPU_SET(coreId, &cpuset);

    uint32_t rc = pthread_setaffinity_np(thread,
    sizeof(cpuset),&cpuset);

    if (rc != 0)
    {
        std::cerr << "Could not pin the current thread to the Core nb(" << coreId << ")" << std::endl;
    }
}