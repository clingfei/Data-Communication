#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")
#define NI_MAXSERV    32
#define NI_MAXHOST  1025

using namespace std;

int main(int argc, char **argv) {
    WORD mVersion = MAKEWORD(2, 2);
    WSADATA wsadata;
    int iResult;

    iResult = WSAStartup(mVersion, &wsadata);
    if (iResult != 0) {
        cout << "WSAStartup failed: " << iResult << endl;
        return 1;
    }

    if (argc != 2) {
        printf("usage: %s IPv4 address\n", argv[0]);
        printf("  to return local hostname\n");
        printf("       %s 127.0.0.1\n", argv[0]);
        return 1;
    }

    DWORD dwRetval;

    struct sockaddr_in saGHN;
    char hostname[NI_MAXHOST];
    char servInfo[NI_MAXSERV];

    u_short port = 27015;
    saGHN.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &saGHN.sin_addr.s_addr);
    saGHN.sin_port = htons(port);


    dwRetval = getnameinfo((struct sockaddr *) &saGHN,
                           sizeof(struct sockaddr),
                           hostname,
                           NI_MAXHOST, servInfo, NI_MAXSERV, 0);

    if (dwRetval != 0) {
        printf("getnameinfo failed with error # %ld\n", WSAGetLastError());
        return 1;
    }
    else {
        printf("getnameinfo returned hostname = %s\n", hostname);
        return 0;
    }
}
