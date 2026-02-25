#include "../../includes/CppStandard.h"
#include "../../includes/Nic.h"
#include "../../includes/HFTMemoryPool.h"

int     hftEngine()
{
    MemoryPool   pool;
    std::thread  nicThread(Nic, std::ref(pool));

    nicThread.join();
    return 0;
}