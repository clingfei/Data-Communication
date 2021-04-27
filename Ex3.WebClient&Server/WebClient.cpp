#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <iostream>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
#define DEFAULT_PORT "80"
#define DEFAULT_BUFLEN 512
using namespace std;

int __cdecl main(int argc, char **argv){
    WORD mVersion = MAKEWORD(2, 2);
    WSADATA wsadata;
    SOCKET ConnectSocket = INVALID_SOCKET;

    struct addrinfo * result = NULL,
                    * ptr = NULL,
                    hints;

    int iResult;

    if (argc != 2) {
        printf("usage: server-name\n");
        return 1;
    }

    iResult = WSAStartup(mVersion, &wsadata);
    if (iResult != 0) {
       printf("WSAStartup filed: %u\n", iResult);
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);

    if (iResult != 0) {
        printf("getaddrinfo failed: %u\n", iResult);
        WSACleanup();
        return 1;
    }

    for (ptr = result; ptr!=NULL; ptr = ptr->ai_next) {
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("Error at socket(): %d\n", WSAGetLastError());
            freeaddrinfo(result);
            return 1;
        }
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);

        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }
    const char *sendbuf =  "GET / HTTP/1.0\n\n";

    iResult = send(ConnectSocket, sendbuf, DEFAULT_BUFLEN, 0);
    if (iResult == SOCKET_ERROR) {
       printf("send failed: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }
    printf("Bytes send : %d\n", iResult);

    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown filed: \n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
    memset(recvbuf, '\0', recvbuflen);
    do {
        iResult = recv(ConnectSocket, recvbuf, recvbuflen-4, 0);
        if (iResult > 0) {
            printf("Bytes received: %d\n", iResult);
            printf("%s\n", recvbuf);
        }
        else if (iResult == 0)
            printf("Connection closed... \n");
        else
            printf("receive failed: %d\n", WSAGetLastError());
    }while(iResult > 0);

    closesocket(ConnectSocket);
    WSACleanup();
    return 0;
}
