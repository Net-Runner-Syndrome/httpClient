#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include "network.h"

//https://beej.us/guide/bgnet/html/split-wide/intro.html
void PrepWinsock() {
    WSADATA wsaData;

    if  (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        fprintf(stderr, "[!] WSAStartup failed: %d\n", WSAGetLastError());
        exit(1);
    }

    if (LOBYTE(wsaData.wVersion) != 2 ||
        HIBYTE(wsaData.wVersion) != 2) {
        fprintf(stderr, "[!] Winsock Version mismatch or not Available\n");
        WSACleanup();
        exit(2);
    }
}

void Netrun(int argc, char *argv[] = NULL)
{
    Network network;
    network.ShowIpHostname(argc, argv);
    //network.PopulateSocket();
    network.LoopBounceConnection();
}

int main(int argc, char *argv[])
{
    PrepWinsock();
    Netrun(argc, argv);
    WSACleanup();
    return 0;
}