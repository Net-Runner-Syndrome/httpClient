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
int MapArgv(char *argv[])
{
    if ((strcmp(argv[1], "-T") == 0) || (strcmp(argv[1], "-Test") == 0))
        return 0;

    else if ((strcmp(argv[1], "-R") == 0) || (strcmp(argv[1], "-Resolve") == 0))
        return 1;

    else if ((strcmp(argv[1], "-Netrun") == 0))
        return 2;

    return -1;
}

void Test(int argc) //-T Test
{
    Network network;
    network.PopulateSocket(argc);
    network.SetupHintsServer();
    network.SetupHintsClient();
    exit(0);
}

void Resolve(int argc, char *argv[]) //-R Resolve
{
    Network network;
    network.ShowIpHostname(argc, argv);
    exit(0);
}

void Netrun(int argc, char *argv[]) //Netrun Network run
{
    Network network;
    network.LoopBounceConnection();
}

int main(int argc, char *argv[])
{
    PrepWinsock();

    int Flag = MapArgv(argv);
       switch (Flag)
    {
        case -1:
            fprintf(stderr, "Unknown flag: %s\n", argv[1]);
            break;

        case 0:
            Test(argc);
            break;

        case 1:
            Resolve(argc, argv);
            break;

        case 2:
            Netrun(argc, argv);
            break;

        default:
            fprintf(stderr, "No Idea what you did but it was something fishy\n");
            break;
    }

    WSACleanup();
    return 0;
}