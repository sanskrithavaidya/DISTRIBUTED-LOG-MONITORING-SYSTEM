#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 5000
#define SERVER_IP "127.0.0.1"

int main(int argc, char *argv[])
{
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server_addr;

    const char *client_id;

    const char *logs[] = {
        "INFO: Application started",
        "WARNING: Memory usage is high",
        "ERROR: Database connection failed",
        "INFO: Application stopped"
    };

    int log_count = sizeof(logs) / sizeof(logs[0]);

    /* Get client ID */
    if (argc >= 2)
    {
        client_id = argv[1];
    }
    else
    {
        client_id = "CLIENT-01";
    }

    /* Initialize Winsock */
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("WSAStartup failed.\n");
        return 1;
    }

    /* Create socket */
    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock == INVALID_SOCKET)
    {
        printf("Socket creation failed.\n");

        WSACleanup();
        return 1;
    }

    /* Configure server */
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    /* Connect */
    if (connect(
            sock,
            (struct sockaddr *)&server_addr,
            sizeof(server_addr)) == SOCKET_ERROR)
    {
        printf("Connection failed. Make sure the server is running.\n");

        closesocket(sock);
        WSACleanup();

        return 1;
    }

    printf("Connected to log monitoring server.\n");
    printf("Client ID: %s\n\n", client_id);

    /* Send logs */
    for (int i = 0; i < log_count; i++)
    {
        char message[256];

        snprintf(
            message,
            sizeof(message),
            "[%s] %s\n",
            client_id,
            logs[i]
        );

        int bytes_sent = send(
            sock,
            message,
            (int)strlen(message),
            0
        );

        if (bytes_sent == SOCKET_ERROR)
        {
            printf("Failed to send log.\n");
            break;
        }

        printf("Sent: %s", message);

        Sleep(2000);
        /* Send heartbeat */
    const char *heartbeat = "HEARTBEAT\n";

    int heartbeat_sent = send(
        sock,
        heartbeat,
        (int)strlen(heartbeat),
        0
    );

    if (heartbeat_sent == SOCKET_ERROR)
    {
        printf("Failed to send heartbeat.\n");
        break;
    }

    printf("Heartbeat sent.\n");

    }

    printf("\nAll logs sent.\n");

    closesocket(sock);
    WSACleanup();

    return 0;
}