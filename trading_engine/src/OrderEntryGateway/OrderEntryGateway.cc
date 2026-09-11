#include "OrderEntryGateway.h"

uint64_t OrderEntryGateway::seq = 0;

int OrderEntryGateway::ListenMarketDataUDP()
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

    std::cout << "UDP Market Data Listening on 127.0.0.1:9001 [KSE - Kinshasa Stock Exchange]\n";

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

int OrderEntryGateway::ListenOrderFeedbackTCP()
{
    while (client_fd_ < 0) {
        ;
    }

    while (true)
    {
        char buffer[1024];

        ssize_t received = recv(
            client_fd_,
            buffer,
            sizeof(buffer),
            0
        );

        if (received < 0) {
            perror("recv");
            return 1;
        }

        if (received == 0) {
            std::cout << "Exchange disconnected\n";
            return 0;
        }

        std::cout << "Feedback: ";
        std::cout.write(buffer, received);
        std::cout << '\n';
    }
}

int OrderEntryGateway::SendOrderTCP()
{
    int connect_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (connect_fd < 0) {
        perror("socket");
        return 1;
    }

    sockaddr_in exchange{};
    exchange.sin_family = AF_INET;
    exchange.sin_port = htons(9000);

    if (inet_pton(
        AF_INET,
        "127.0.0.1",
        &exchange.sin_addr
    ) != 1) {
        perror("inet_pton");
        close(connect_fd);
        return 1;
    }

    if (connect(
        connect_fd,
        reinterpret_cast<sockaddr*>(&exchange),
        sizeof(exchange)
    ) < 0) {
        perror("connect");
        close(connect_fd);
        return 1;
    }

    // THIS is the connected TCP socket
    client_fd_ = connect_fd;

    std::cout << "Connected to KSE order-entry gateway\n";

    while (true)
    {
        std::string order;
        std::getline(std::cin, order);

        if (order.empty())
            continue;

        ssize_t sent = send(
            client_fd_,
            order.data(),
            order.size(),
            0
        );

        if (sent < 0) {
            perror("send");
            close(client_fd_);
            return 1;
        }

        std::cout << "Sent " << sent << " bytes\n";
    }

    return 0;
}

