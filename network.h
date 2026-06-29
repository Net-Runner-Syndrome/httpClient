#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

class Network 
{
public:
    const char *ipver;
    char buffer[1024]{};
    char ipstr[INET6_ADDRSTRLEN]{};
    int status{0};
    int length_r{0};
    int length_w{0};
    int bytes_recieved{0};
    int bytes_sent{0};
    SOCKET socket_fd{0};

    struct addrinfo hints{};
    //Could just lowk make an array but nah, this is more fun
    struct addrinfo *head {nullptr}; 
    struct addrinfo *iterator {nullptr};
    struct addrinfo *servinfo {nullptr};
    struct addrinfo *listeninfo {nullptr};
    struct sockaddr_storage addr_2 {0};
    struct sockaddr_in *ipv4 {nullptr};
    struct sockaddr_in6 *ipv6 {nullptr};

    socklen_t addr_size = sizeof addr_2;

    void *addr;
    void ArgcError(int argc);
    void PopulateHints();
    void SetupHintsServer();
    void SetupHintsClient();
    void HandleIPVersioni();
    void HandleIPVersionh(); 
    void PopulateSocket(int argc);
    void LoopBounceConnection();
    void PassMSG(const char* pSTR="Status");
    void HandleMessage(const char *msg="Hello, World!", SOCKET _socket_ = INVALID_SOCKET, int flags = 0);
    std::string HandleHTTP(const char *msg, int length);
    int ShowIpHostname(int argc, char *argv[]);
};