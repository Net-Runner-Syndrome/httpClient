#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstring>
#include "network.h"
#include <stdio.h>
#include <string>

constexpr int BACKLOG = 5;
constexpr auto PORT = "8081";

void Network::PopulateHints()
{
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC; 
    hints.ai_socktype = SOCK_STREAM; 
    hints.ai_flags = AI_PASSIVE; 
}

void Network::SetupHintsServer() 
{
    Network::PopulateHints();

    if ((status = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) 
    {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        return;
    }

    freeaddrinfo(servinfo);
}  

void Network::SetupHintsClient() 
{
    Network::PopulateHints();
    if ((status = getaddrinfo("www.example.com", "http", &hints, &servinfo)) != 0) 
    {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        return;
    }

    freeaddrinfo(servinfo);
}

void Network::HandleIPVersioni() 
{
        //iterator->ai_family = AF_INET;
        if (iterator->ai_family == AF_INET) 
        { // IPv4
            ipv4 = (struct sockaddr_in *)iterator->ai_addr;
            addr = &(ipv4->sin_addr);
            ipver = "IPv4";
        } 
        else 
        { // IPv6
            ipv6 = (struct sockaddr_in6 *)iterator->ai_addr;
            addr = &(ipv6->sin6_addr);
            ipver = "IPv6";
        }
}

void Network::HandleIPVersionh() //I should probably use (Network::HandleIPVersioni())inheritance but I am lazy and this is a small project so whatever
{
        head->ai_family = AF_INET; //I know this is risky but my c++ Kung-Fu is not strong enough to provide a better solution. 
        if (head->ai_family == AF_INET)
        { // IPv4
            ipv4 = (struct sockaddr_in *)head->ai_addr;
            addr = &(ipv4->sin_addr);
            ipver = "IPv4";
        } 
        else 
        { // IPv6
            ipv6 = (struct sockaddr_in6 *)head->ai_addr;
            addr = &(ipv6->sin6_addr);
            ipver = "IPv6";
        }
}

int Network::ShowIpHostname(int argc, char *argv[]) 
{
   if(argc != 2)
   {
    fprintf(stderr, "[!] Please specify 1 hostname to resolve, usage: [binary file] [hostname]\n[-] Example: ./xyz.exe www.example.com\n");
    return 1;
   }

    Network::PopulateHints();
    if ((status = getaddrinfo(argv[1], NULL, &hints, &head)) != 0) 
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 2;
    }
    
    printf("IP addresses for %s:\n\n", argv[1]);
    for(iterator = head; iterator != NULL; iterator = iterator->ai_next) //Linked List shit cause I did not pay attention to DSA in class
    {
        Network::HandleIPVersioni();
        
        // convert the IP to a string
        inet_ntop(iterator->ai_family, addr, ipstr, sizeof ipstr);
        printf("  %s: %s\n", ipver, ipstr);
    }

    freeaddrinfo(head);
    return 0;
}

//[note] All ports below 1024 are RESERVED unless you’re the superuser! You can have any port number above that, right up to 65535 (provided they aren’t already being used by another program).
void Network::PopulateSocket()
{
    Network::PopulateHints();
    if ((status = getaddrinfo(NULL, PORT, &hints, &head)) != 0) 
    {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        return;
    }

    socket_fd = socket(head->ai_family, head->ai_socktype, head->ai_protocol);
    if(bind(socket_fd, head->ai_addr, head->ai_addrlen) == SOCKET_ERROR)
    {
        if(WSAGetLastError() == WSAEADDRINUSE)
        {
            Network::HandleIPVersionh();
            inet_ntop(head->ai_family, addr, ipstr, sizeof ipstr);
            //Allow the socket to reuse the port
            fprintf(stderr, "\n[Warning] Socket address already in use [%s: %s:%s]\n", ipver, ipstr, "8081");

           /*  
            BOOL enable = 1;
            setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&enable, sizeof enable);
            bind(socket_fd, head->ai_addr, head->ai_addrlen);
           */
        }
        else
        {
            fprintf(stderr, "\n[!] Failed to bind socket, WSAGetLastError(): %d\n", WSAGetLastError());
            return;
        }
        return;
    }

    freeaddrinfo(head);
} //Watch HaikYo -> Crunchy Roll -> DumpsterMovie

std::string Network::HandleHTTP(const char *msg, int length)
{
    std::string response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: " + std::to_string(length) + "\r\n"
        "\r\n" +
        std::string(msg);

    return response;
}

void Network::HandleMessage(const char *msg, 
                            SOCKET _socket_, 
                            int flags) 
{
     bytes_recieved = recv(_socket_, buffer, sizeof(buffer) -1, flags);
     if(bytes_recieved > 0)
     {
        buffer[bytes_recieved] = '\0';
        length_r = sizeof(buffer) -1;
        fprintf(stdout, "\n[REC]\n\n   length: %d\n   bytes: %d\n", length_r, bytes_recieved);
     }

     length_w = strlen(msg);
     //bytes_sent = send(_socket_, msg, length_w, flags);
     std::string response =HandleHTTP(msg, length_w);
     bytes_sent = send(_socket_, response.c_str(), response.length(), flags);
     fprintf(stdout, "\n[SND]\n\n   length: %d\n   bytes: %d\n", length_w, bytes_sent);
}

void Network::LoopBounceConnection() 
{
    Network::PopulateHints();
    if ((status = getaddrinfo(NULL, PORT, &hints, &listeninfo)) != 0) 
    {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        return;
    }
    
    socket_fd = socket(listeninfo->ai_family, listeninfo->ai_socktype, listeninfo->ai_protocol);
    if(bind(socket_fd, listeninfo->ai_addr, listeninfo->ai_addrlen) == SOCKET_ERROR)
    {
        if(WSAGetLastError() == WSAEADDRINUSE)
        {
            listeninfo->ai_family = AF_INET;
            Network::HandleIPVersionh();
            inet_ntop(listeninfo->ai_family, addr, ipstr, sizeof ipstr);
            //Allow the socket to reuse the port
            fprintf(stderr, "\n[Warning] Socket address already in use [%s: %s:%s]\n", ipver, ipstr, PORT);

           /* 
            BOOL enable = 1;
            setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&enable, sizeof enable);
            bind(socket_fd, listeninfo->ai_addr, listeninfo->ai_addrlen);
           */
        }
        else
        {
            fprintf(stderr, "\n[!] Failed to bind socket, WSAGetLastError(): %d\n", WSAGetLastError());
            return;
        }
        return;
    }
    if(listen(socket_fd, BACKLOG) == SOCKET_ERROR)
    {
        if(WSAGetLastError() == WSAENETDOWN)
        {
            fprintf(stderr, "\n[CRITICAL] Network subsystem failure: Network is down, WSAGetLastError(): %d -> DDOS Attack... #RIP\n", WSAGetLastError());
        }
        else
        {
            fprintf(stderr, "\n[!] Failed to listen on socket, WSAGetLastError(): %d\n", WSAGetLastError());
            return;
        }
        return;
    }
    fprintf(stdout, "\n[+] Listening:");
    
    SOCKET a_socket_fd = accept(socket_fd, (struct sockaddr *)&addr_2, &addr_size);
    if(a_socket_fd == INVALID_SOCKET)
    {
        printf("accept() failed: %d\n", WSAGetLastError());
        return;
    }

    fprintf(stdout, "\n[+] connection established!");
    Network::HandleMessage("Hello, World!", a_socket_fd, 0);
    
    closesocket(a_socket_fd);
    closesocket(socket_fd);
    freeaddrinfo(listeninfo);
}