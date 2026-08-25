#include <netinet/in.h>
#include <sys/socket.h>
#include <iostream>

int main()
{
    std::cout << "Exchange Feed processing..." << std::endl;
    int exchangeSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (exchangeSocket == -1)
    {
        std::cout << "Error while creating socket\n";
    }
    return 0;
}