#include "SessionGateway.h"
#include "Constants.h"


uint64_t SessionGateway::seq = 0;

int SessionGateway::ListenOrderEntryTCP(RxRingBuffer<RxDesc, ExchangeInetConst::TX_RING_SIZE> &order_entry_command_ring, MemoryPool  *memory_pool)
{
    std::cout << "IN " << __func__ << std::endl;
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (listen_fd < 0) {
        perror("socket");
        return 1;
    }

    int yes = 1;

    if (setsockopt(
            listen_fd,
            SOL_SOCKET,
            SO_REUSEADDR,
            &yes,
            sizeof(yes)
    ) < 0) {
        perror("setsockopt");
        close(listen_fd);
        return 1;
    }

    sockaddr_in local{};
    local.sin_family = AF_INET;
    local.sin_port = htons(9000);

    if (inet_pton(AF_INET, "127.0.0.1", &local.sin_addr) != 1) {
        std::cerr << "inet_pton failed\n";
        close(listen_fd);
        return 1;
    }

    std::cout
    << "ABOUT TO BIND TCP 127.0.0.1:9000"
    << " fd=" << listen_fd
    << " thread=" << std::this_thread::get_id()
    << '\n';

    if (bind(
        listen_fd,
        reinterpret_cast<sockaddr*>(&local),
        sizeof(local)
    ) < 0) {
        perror("bind");
        close(listen_fd);
        return 1;
    }

    if (listen(listen_fd, 16) < 0) {
        perror("listen");
        close(listen_fd);
        return 1;
    }

    

    std::cout
        << "TCP Order Entry listening on 127.0.0.1:9000 "
        << "[Supreme Running - Kilibanseke Trading Client]\n";

    sockaddr_in client{};
    socklen_t client_len = sizeof(client);

    client_fd_ = accept(
        listen_fd,
        reinterpret_cast<sockaddr*>(&client),
        &client_len
    );

    std::cout << "ACCEPT RETURNED client_fd_=" << client_fd_ << '\n';

    if (client_fd_ < 0) {
        perror("accept");
        close(listen_fd);
        return 1;
    }

    char client_ip[INET_ADDRSTRLEN];

    inet_ntop(
        AF_INET,
        &client.sin_addr,
        client_ip,
        sizeof(client_ip)
    );

    std::cout
        << "Client connected from "
        << client_ip
        << ":"
        << ntohs(client.sin_port)
        << "\n";

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
            close(client_fd_);
            close(listen_fd);
            return 1;
        }

        if (received == 0) {
            std::cout << "Client disconnected\n";
            break;
        }

        std::cout
            << "Received "
            << received
            << " bytes\n";

        std::cout.write(buffer, received);
        if (((order_entry_command_ring.head + 1) & ExchangeInetConst::TX_RING_SIZE -1) != (order_entry_command_ring.tail))
        {
            memcpy(&order_entry_command_ring.data, buffer, strlen(buffer));
            auto head = order_entry_command_ring.head.load(std::memory_order_acquire);
            order_entry_command_ring.head.store(head+1,std::memory_order_release);
        }
        std::cout << '\n';
    }

    close(client_fd_);
    close(listen_fd);

    return 0;
}

int SessionGateway::SendMarketDataUDP()
{
    std::cout << "IN " << __func__ << std::endl;
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd < 0)
    {
        std::perror("socket");
        return 0;
    }
    
    sockaddr_in dest{};

    dest.sin_family = AF_INET;
    dest.sin_port = htons(9001);

    // Convert "127.0.0.1" from text into binary IPv4 representation
    if (inet_pton(AF_INET, "127.0.0.1", &dest.sin_addr) != 1) {
        perror("inet_pton");
        close(sockfd);
        return 1;
    }

    // 3. Payload
    std::string buffer;
    while (true)
    {
        std::getline(std::cin, buffer);
        const char* msg = buffer.c_str();

        // 4. Send one UDP datagram
        ssize_t sent = sendto(
            sockfd,
            msg,
            std::strlen(msg),
            0,
            reinterpret_cast<sockaddr*>(&dest),
            sizeof(dest)
        );

        if (sent < 0) {
            perror("sendto");
            close(sockfd);
            return 1;
        }

        std::cout << "Sent " << sent << " bytes[UDP]\n";
    }

    // 5. Release the socket kernel resource
    close(sockfd);

    return 0;
}

int SessionGateway::SendOrderFeedbackTCP()
{
    std::cout << "IN " << __func__ << std::endl;
    while (client_fd_ < 0) {
        ;
    }
    std::cout << "Connected to Supreme Running : Kilibanseke Trading Client TCP Feedback Server\n";

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

        std::cout
            << "Sent "
            << sent
            << " bytes[TCP]\n";
    }

    close(client_fd_);

    return 0;
}