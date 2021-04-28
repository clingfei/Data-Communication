#include <WinSock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <process.h>
#include <iostream>
#undef UNICODE

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_BUFLEN 512

using namespace std;

void recvThread(void* sock);

int main(int argc, char* argv[]) {
    bool isHolder = true;

    if (argc == 2) {
        isHolder = true;
    }
    else if (argc == 4) {
        isHolder = false;
    }
    else {
        cout << "Usage : ip_address sendport listenport" << endl;
        cout << "Usage : listenport" << endl;
        return 1;
    }
    WSADATA wsaData;
    int iResult;

    char hostname[NI_MAXHOST];
    struct addrinfo hints;
    struct addrinfo* result = NULL;
    struct sockaddr_in host_addr;
    char ipstringbuffer[46];
    short port;

    SOCKET ConnectSocket = INVALID_SOCKET;
    struct sockaddr_in server;
    struct sockaddr_in si_other;
    int addrlen = sizeof(si_other);
    char msgbuf[DEFAULT_BUFLEN];

    char *ListenPort, *SendPort;

    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        cout << "WSAStartup failed with error: " << iResult;
        return 1;
    }

    gethostname(hostname, NI_MAXHOST);
    cout << "hostname: " << hostname << endl;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    if (isHolder) {
        ListenPort = argv[1];
        iResult = getaddrinfo(NULL, ListenPort, &hints, &result);
    }
    else {
        ListenPort = argv[2];
        SendPort = argv[3];
        iResult = getaddrinfo(argv[1], SendPort, &hints, &result);
    }

    if (iResult != 0) {
        cout << "getaddrinfo failed with error: " << iResult;
        WSACleanup();
        return 1;
    }

    memcpy(&host_addr, result->ai_addr, sizeof(host_addr));
    inet_ntop(AF_INET, &host_addr.sin_addr, ipstringbuffer, sizeof(ipstringbuffer));
    port = ntohs(host_addr.sin_port);
    cout << "Server Address: " << ipstringbuffer << endl;
    cout << "Server Port Number: " << port << endl;

    ConnectSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ConnectSocket == INVALID_SOCKET) {
        cout << "Could not create socket: " <<  WSAGetLastError() << endl;
        return 1;
    }
    cout << "Create socket finished." << endl;

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = host_addr.sin_addr.s_addr;
    server.sin_port = host_addr.sin_port;

    iResult = bind(ConnectSocket, (struct sockaddr*) & server, sizeof(server));
    if (iResult == SOCKET_ERROR) {
        cout << "Could not Bind the Socket: " << WSAGetLastError() << endl;
        freeaddrinfo(result);
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    if (isHolder) {
        memset(msgbuf, 0, DEFAULT_BUFLEN);
        iResult = recvfrom(ConnectSocket, msgbuf, DEFAULT_BUFLEN, 0, (struct sockaddr*)&si_other, &addrlen);
        if (iResult == SOCKET_ERROR) {
            cout << "recvfrom() fail with error code: " << WSAGetLastError() << endl;
            freeaddrinfo(result);
            closesocket(ConnectSocket);
            WSACleanup();
            return 1;
        }
        inet_ntop(AF_INET, &si_other.sin_addr, ipstringbuffer, 46);
        cout << "Receive from : " << ipstringbuffer << ntohs(si_other.sin_port) << endl;
        cout << msgbuf << endl;
    }
    else {
        memset(msgbuf, 0, DEFAULT_BUFLEN);
        si_other.sin_family = AF_INET;
        sscanf_s(argv[1], "%s", ipstringbuffer, 46);
        sscanf_s(ListenPort, "%hi", &port);
        si_other.sin_port = htons(port);
        inet_pton(AF_INET, ipstringbuffer, &si_other.sin_addr);
    }

    if (_beginthread(recvThread, 4096, (void*)ConnectSocket) < 0)
        cout << "thread error." << endl;
    while (1) {
        memset(msgbuf, 0, DEFAULT_BUFLEN);
        cout << "Enter Sentence:";
        gets_s(msgbuf);
        iResult = sendto(ConnectSocket, msgbuf, int(strlen(msgbuf) + 1), 0, (struct sockaddr*) & si_other, addrlen);
        if (iResult == SOCKET_ERROR) {
            cout << "sendto() fail with error code: " << WSAGetLastError() << endl;
            freeaddrinfo(result);
            closesocket(ConnectSocket);
            WSACleanup();
        }
        if (strcmp(msgbuf, "quit") == 0)
            break;
        if (strcmp(msgbuf, "switch") == 0) {
            char ip_addr[20];
            memset(ip_addr, 0, 20);
            memset(ListenPort, 0, 6);
            cin.getline(ip_addr, 20);
            cin >> ListenPort;
            sscanf_s(ip_addr, "%s", ipstringbuffer, 46);
            sscanf_s(ListenPort, "%hi", &port);
            si_other.sin_port = htons(port);
            inet_pton(AF_INET, ipstringbuffer, &si_other.sin_addr);
        }
    }
    freeaddrinfo(result);
    closesocket(ConnectSocket);
    WSACleanup();
    return 0;
}


void recvThread(void* sock) {
    char msgbuf[DEFAULT_BUFLEN];
    SOCKET ConnectSocket = (SOCKET)sock;
    struct sockaddr_in si_other;
    int addrlen = sizeof(si_other);
    char ipstringbuffer[46];
    memset(ipstringbuffer, 0, 46);
    int iResult;

    while (1) {
        memset(msgbuf, 0, DEFAULT_BUFLEN);
        iResult = recvfrom(ConnectSocket, msgbuf, DEFAULT_BUFLEN, 0, (struct sockaddr*) & si_other, &addrlen);
        if (iResult == SOCKET_ERROR) {
            cout << "recvfrom() fail with error code: " << WSAGetLastError() << endl;
            break;
        }
        inet_ntop(AF_INET, &si_other.sin_addr, ipstringbuffer, 46);
        cout << "Receive from " <<  ipstringbuffer  << ":" << ntohs(si_other.sin_port) << endl;
        cout <<"Data: " << msgbuf << endl;
        cout << "Enter Sentence:";

        if (strcmp(msgbuf, "quit") == 0)
            break;
    }
    closesocket(ConnectSocket);
    _endthread();
}