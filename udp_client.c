#include <stdio.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsa;
    SOCKET clientSocket;
    struct sockaddr_in serverAddr;
    int serverAddrLen, recv_len;
    char message[1024], server_reply[1024];

    // Initialize Winsock
    WSAStartup(MAKEWORD(2, 2), &wsa);

    // Create socket
    clientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (clientSocket == SOCKET_ERROR) {
        printf("Socket creation failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }

    // Server address structure
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");  // Replace with the actual server IP if needed
    serverAddr.sin_port = htons(8888);

    serverAddrLen = sizeof(serverAddr);

    // Chat loop
    while (1) {
        printf("Enter message: ");
        fgets(message, sizeof(message), stdin);
        message[strcspn(message, "\n")] = '\0';  // Remove newline character

        // Send the message to the server
        sendto(clientSocket, message, strlen(message), 0, (struct sockaddr *)&serverAddr, serverAddrLen);

        // Receive reply from the other client via the server
        recv_len = recvfrom(clientSocket, server_reply, sizeof(server_reply), 0, (struct sockaddr *)&serverAddr, &serverAddrLen);
        if (recv_len > 0) {
            server_reply[recv_len] = '\0';  // Null-terminate the received message
            printf("Message from other client: %s\n", server_reply);
        }
    }

    // Clean up
    closesocket(clientSocket);
    WSACleanup();
    return 0;
}
