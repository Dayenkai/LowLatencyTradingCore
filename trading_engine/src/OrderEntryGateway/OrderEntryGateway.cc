#include "OrderEntryGateway.h"

int OrderEntryGateway::Listen()
{
    int fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (fd < 0) {
        perror("socket");
        return 1;
    }

    sockaddr_in local{};
    local.sin_family = AF_INET;
    local.sin_port = htons(9001);

    // Bind specifically to localhost
    if (inet_pton(AF_INET, "127.0.0.1", &local.sin_addr) != 1) {
        std::cerr << "inet_pton failed\n";
        close(fd);
        return 1;
    }

    if (bind(
        fd,
        reinterpret_cast<sockaddr*>(&local),
        sizeof(local)
    ) < 0) {
        perror("bind");
        close(fd);
        return 1;
    }

    std::cout << "Listening on 127.0.0.1:9001\n";

    while (true)
    {
        char buffer[1024];

        sockaddr_in sender{};
        socklen_t sender_len = sizeof(sender);

        ssize_t received = recvfrom(
            fd,
            buffer,
            sizeof(buffer) - 1,
            0,
            reinterpret_cast<sockaddr*>(&sender),
            &sender_len
        );

        if (received < 0) {
            perror("recvfrom");
            close(fd);
            return 1;
        }

        buffer[received] = '\0';

        char sender_ip[INET_ADDRSTRLEN];

        inet_ntop(
            AF_INET,
            &sender.sin_addr,
            sender_ip,
            sizeof(sender_ip)
        );

        std::cout
            << "Received " << received
            << " bytes from "
            << sender_ip
            << ":"
            << ntohs(sender.sin_port)
            << "\n";

        std::cout << "Data: " << buffer << '\n';
    }

    close(fd);
}

int OrderEntryGateway::SendOrder()
{
    int fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (fd < 0) {
        perror("socket");
        return 1;
    }

    sockaddr_in local{};
    local.sin_family = AF_INET;
    local.sin_port = htons(9001);

    // Bind specifically to localhost
    if (inet_pton(AF_INET, "127.0.0.1", &local.sin_addr) != 1) {
        std::cerr << "inet_pton failed\n";
        close(fd);
        return 1;
    }

    if (bind(
        fd,
        reinterpret_cast<sockaddr*>(&local),
        sizeof(local)
    ) < 0) {
        perror("bind");
        close(fd);
        return 1;
    }

    std::cout << "Listening on 127.0.0.1:9001\n";

    while (true)
    {
        char buffer[1024];

        sockaddr_in sender{};
        socklen_t sender_len = sizeof(sender);

        ssize_t received = recvfrom(
            fd,
            buffer,
            sizeof(buffer) - 1,
            0,
            reinterpret_cast<sockaddr*>(&sender),
            &sender_len
        );

        if (received < 0) {
            perror("recvfrom");
            close(fd);
            return 1;
        }

        buffer[received] = '\0';

        char sender_ip[INET_ADDRSTRLEN];

        inet_ntop(
            AF_INET,
            &sender.sin_addr,
            sender_ip,
            sizeof(sender_ip)
        );

        std::cout
            << "Received " << received
            << " bytes from "
            << sender_ip
            << ":"
            << ntohs(sender.sin_port)
            << "\n";

        std::cout << "Data: " << buffer << '\n';
    }

    close(fd);
}