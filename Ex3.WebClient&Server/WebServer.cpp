#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define WIN32_LEAN_AND_MEAN
#define DEFAULT_PORT "80"
#define DEFAULT_BUFLEN 512
using namespace std;
int main(void) {
    WORD mVersion = MAKEWORD(2, 2);
    WSADATA wsadata;

    SOCKET ConnectSocket = INVALID_SOCKET;
    SOCKET ListenSocket = INVALID_SOCKET;
    char ipstringbuffer[46];
    struct sockaddr_in server_addr;
    struct addrinfo *result = NULL, hints;
    int iResult;

    iResult = WSAStartup(mVersion, &wsadata);
    if (iResult != 0) {
        cout << "WSAStartup failed with error: " << WSAGetLastError() << endl;
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        cout << "getaddrinfo failed with error: " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }

    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        cout << "socket failed with error : "  << WSAGetLastError() << endl;
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }
    cout << "socket conencted." << endl;

    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        cout << "bind failed with error: " << WSAGetLastError() << endl;
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }
    cout << "bind finished. " << endl;
    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        cout << "listen failed with error : " << WSAGetLastError() << endl;
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }
    cout << "listen finished " << endl;

    ConnectSocket = accept(ListenSocket, NULL, NULL);
    if (ConnectSocket == INVALID_SOCKET) {
        cout << "accept failed with error : " << WSAGetLastError() << endl;
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }
    cout<<ConnectSocket<<"\n";
    cout << "accept finished ." << endl;
    closesocket(ListenSocket);

    char outbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
    char recvbuf[DEFAULT_BUFLEN];

    iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
    for (int i=0; i < iResult; i++) {
        cout << recvbuf[i];
    }
    cout << endl;
    strcpy_s(outbuf, "<html><body><hr>This is a response <b>message</b> in HTML format. <font color=red>Wow!</font><hr></body></html>");

    cout << outbuf <<endl;
    iResult = send(ConnectSocket, outbuf, strlen(outbuf)+1, 0);//+1
    cout << iResult << endl;
    if (iResult == SOCKET_ERROR) {
        cout << "send failed with error : " << WSAGetLastError() << endl;
    }
    else {
        cout << "send finished." << endl;
    }
    iResult=shutdown(ConnectSocket,SD_SEND);
    printf("%d",iResult);
    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}
