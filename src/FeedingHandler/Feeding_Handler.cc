#include <thread>
#include <iostream>
#include <sstream>

void    printNumber()
{
    //std::ostringstream oss;
    //oss << std::this_thread::get_id() << std::endl;
    for (int i = 0; i < 1000000; ++i) {
        
        std::cout << "Thread with id " << std::this_thread::get_id() << " : Current Value -> " << i << std::endl;        
    }
}

int  feedHandler()
{

}