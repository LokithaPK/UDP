#include <stdio.h>
#include <winsock2.h>
#include <string.h>

#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsa;
    SOCKET serverSocket;
    struct sockaddr_in serverAddr, client1Addr, client2Addr;
    int clientAddrLen = sizeof(struct sockaddr_in), recv_len;
    char buffer[1024];
    int client1Connected = 0, client2Connected = 0;

    // Initialize Winsock
    WSAStartup(MAKEWORD(2, 2), &wsa);

    // Create UDP socket
    serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (serverSocket == SOCKET_ERROR) {
        printf("Socket creation failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }

    // Prepare sockaddr_in structure
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8888);

    // Bind the socket
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Bind failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }

    printf("Server is running...\n");

    // Continuously listen for messages
    while (1) {
        memset(buffer, '\0', sizeof(buffer));

        // Receive a message
        recv_len = recvfrom(serverSocket, buffer, sizeof(buffer), 0, (struct sockaddr *)&client1Addr, &clientAddrLen);
        if (recv_len == SOCKET_ERROR) {
            int err = WSAGetLastError();
            if (err == WSAECONNRESET) {
                printf("Client disconnected unexpectedly (Error 10054). Waiting for new connections...\n");
            } else {
                printf("recvfrom() failed. Error Code: %d\n", err);
            }
            continue;  // Instead of breaking, keep the server running
        }

        // Handle client connections
        if (!client1Connected) {
            client1Addr = client1Addr;
            client1Connected = 1;
            printf("Client 1 connected\n");
        } else if (!client2Connected) {
            client2Addr = client1Addr;
            client2Connected = 1;
            printf("Client 2 connected\n");
        }

        // Forward the message between clients
        if (client1Connected && client2Connected) {
            if (memcmp(&client1Addr, &client1Addr, sizeof(client1Addr)) == 0) {
                sendto(serverSocket, buffer, recv_len, 0, (struct sockaddr *)&client2Addr, clientAddrLen);
            } else {
                sendto(serverSocket, buffer, recv_len, 0, (struct sockaddr *)&client1Addr, clientAddrLen);
            }
        }
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
