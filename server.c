#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <winsock2.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 5000
#define MAX_CLIENTS 100
#define BUFFER_SIZE 1024

typedef struct
{
    char client_id[64];

    int total;
    int info;
    int warning;
    int error;

    time_t last_seen;
    int online;

} ClientStats;


/* Global statistics */

ClientStats clients[MAX_CLIENTS];

int client_count = 0;

int total_count = 0;
int info_count = 0;
int warning_count = 0;
int error_count = 0;

HANDLE log_mutex;
FILE *log_file;


/* =========================================================
   Timestamp
   ========================================================= */

void get_timestamp(char *buffer, int size)
{
    time_t now;
    struct tm *local_time;

    now = time(NULL);

    local_time = localtime(&now);

    if (local_time != NULL)
    {
        strftime(
            buffer,
            size,
            "%Y-%m-%d %H:%M:%S",
            local_time
        );
    }
}


/* =========================================================
   Find or create client
   ========================================================= */

int get_client_index(const char *client_id)
{
    int i;

    for (i = 0; i < client_count; i++)
    {
        if (strcmp(clients[i].client_id, client_id) == 0)
        {
            return i;
        }
    }

    if (client_count < MAX_CLIENTS)
    {
        strcpy(
            clients[client_count].client_id,
            client_id
        );

        clients[client_count].total = 0;
        clients[client_count].info = 0;
        clients[client_count].warning = 0;
        clients[client_count].error = 0;

        clients[client_count].last_seen = time(NULL);
        clients[client_count].online = 1;

        client_count++;

        return client_count - 1;
    }

    return -1;
}


/* =========================================================
   Display client statistics
   ========================================================= */

void display_client_statistics()
{
    int i;

    printf("\n");
    printf("====================================================\n");
    printf("                CLIENT STATISTICS\n");
    printf("====================================================\n");

    printf(
        "%-15s %-8s %-8s %-10s %-8s\n",
        "Client",
        "Total",
        "INFO",
        "WARNING",
        "ERROR"
    );

    printf("----------------------------------------------------\n");

    for (i = 0; i < client_count; i++)
    {
        printf(
            "%-15s %-8d %-8d %-10d %-8d\n",
            clients[i].client_id,
            clients[i].total,
            clients[i].info,
            clients[i].warning,
            clients[i].error
        );
    }

    printf("====================================================\n");
}


/* =========================================================
   Display health status
   ========================================================= */

void display_health_status()
{
    int i;

    printf("\n");
    printf("====================================================\n");
    printf("                 CLIENT HEALTH\n");
    printf("====================================================\n");

    printf(
        "%-15s %-12s %-20s\n",
        "Client",
        "Status",
        "Last Seen"
    );

    printf("----------------------------------------------------\n");

    for (i = 0; i < client_count; i++)
    {
        char last_seen[64];

        struct tm *time_info;

        time_info = localtime(
            &clients[i].last_seen
        );

        if (time_info != NULL)
        {
            strftime(
                last_seen,
                sizeof(last_seen),
                "%Y-%m-%d %H:%M:%S",
                time_info
            );
        }
        else
        {
            strcpy(last_seen, "Unknown");
        }

        printf(
            "%-15s %-12s %-20s\n",
            clients[i].client_id,
            clients[i].online
                ? "ONLINE"
                : "OFFLINE",
            last_seen
        );
    }

    printf("====================================================\n");
}
void write_log_to_file(const char *message)
{
    char timestamp[64];

    get_timestamp(timestamp, sizeof(timestamp));

    WaitForSingleObject(log_mutex, INFINITE);

    if (log_file != NULL)
    {
        fprintf(
            log_file,
            "[%s] %s\n",
            timestamp,
            message
        );

        fflush(log_file);
    }

    ReleaseMutex(log_mutex);
}


/* =========================================================
   Process one message
   ========================================================= */

void process_message(
    char *message,
    const char *client_id,
    int client_index,
    unsigned long thread_id
)
{
    char timestamp[64];

    get_timestamp(
        timestamp,
        sizeof(timestamp)
    );


    /* Remove CR */

    message[strcspn(message, "\r")] = '\0';


    /* Ignore empty message */

    if (strlen(message) == 0)
    {
        return;
    }
    write_log_to_file(message);


    /* =====================================================
       HEARTBEAT
       ===================================================== */

    if (strcmp(message, "HEARTBEAT") == 0)
    {
        WaitForSingleObject(
            log_mutex,
            INFINITE
        );

        if (client_index >= 0)
        {
            clients[client_index].last_seen =
                time(NULL);

            clients[client_index].online = 1;
        }

        ReleaseMutex(log_mutex);


        printf(
    "[%s] [Thread %lu] %s\n",
    timestamp,
    thread_id,
    message
);

        return;
    }


    /* =====================================================
       Normal log
       ===================================================== */

    WaitForSingleObject(
        log_mutex,
        INFINITE
    );


    if (client_index >= 0)
    {
        clients[client_index].last_seen =
            time(NULL);

        clients[client_index].online = 1;
    }


    total_count++;


    if (client_index >= 0)
    {
        clients[client_index].total++;
    }


    /* =====================================================
       ERROR
       ===================================================== */

    if (strstr(message, "ERROR") != NULL)
    {
        error_count++;

        if (client_index >= 0)
        {
            clients[client_index].error++;
        }

        printf("\n");
        printf(
            "************ ERROR ALERT ************\n"
        );

       printf(
    "[%s] [Thread %lu] %s\n",
    timestamp,
    thread_id,
    message
);

        printf(
            "Client %s generated an ERROR.\n",
            client_id
        );

        printf(
            "*************************************\n"
        );
    }


    /* =====================================================
       WARNING
       ===================================================== */

    else if (
        strstr(message, "WARNING") != NULL
    )
    {
        warning_count++;

        if (client_index >= 0)
        {
            clients[client_index].warning++;
        }

        printf(
    "[%s] [Thread %lu] %s\n",
    timestamp,
    thread_id,
    message
);
    }


    /* =====================================================
       INFO
       ===================================================== */

    else if (
        strstr(message, "INFO") != NULL
    )
    {
        info_count++;

        if (client_index >= 0)
        {
            clients[client_index].info++;
        }

        printf(
    "[%s] [Thread %lu] %s\n",
    timestamp,
    thread_id,
    message
);
    }


    /* =====================================================
       Unknown
       ===================================================== */

    else
    {
        printf(
            "[%s] [Thread %lu] [%s] UNKNOWN: %s\n",
            timestamp,
            thread_id,
            client_id,
            message
        );
    }


    ReleaseMutex(log_mutex);
}


/* =========================================================
   Client handler
   ========================================================= */

DWORD WINAPI handle_client(LPVOID socket_pointer)
{
    SOCKET client_socket;

    char buffer[BUFFER_SIZE];

    char pending_data[BUFFER_SIZE * 2];

    int pending_length = 0;

    char client_id[64] = "UNKNOWN";

    int client_index = -1;

    unsigned long thread_id;

    client_socket = *(SOCKET *)socket_pointer;

    free(socket_pointer);

    thread_id = GetCurrentThreadId();


    printf(
        "\nClient connected. Thread ID: %lu\n",
        thread_id
    );


    pending_data[0] = '\0';


    /* =====================================================
       Receive data
       ===================================================== */

    while (1)
    {
        int received;

        received = recv(
            client_socket,
            buffer,
            BUFFER_SIZE - 1,
            0
        );


        if (received <= 0)
        {
            break;
        }


        buffer[received] = '\0';


        /*
           Add received data to pending buffer.
        */

        if (
            pending_length + received
            >= (int)sizeof(pending_data) - 1
        )
        {
            printf(
                "Message buffer overflow. "
                "Closing client connection.\n"
            );

            break;
        }


        memcpy(
            pending_data + pending_length,
            buffer,
            received
        );

        pending_length += received;

        pending_data[pending_length] = '\0';


        /*
           Process complete lines.
        */

        char *newline_position;

        while (
            (newline_position =
                strchr(pending_data, '\n')) != NULL
        )
        {
            char message[BUFFER_SIZE];

            int message_length;

            message_length =
                (int)(
                    newline_position
                    - pending_data
                );


            if (
                message_length
                >= BUFFER_SIZE
            )
            {
                message_length =
                    BUFFER_SIZE - 1;
            }


            memcpy(
                message,
                pending_data,
                message_length
            );

            message[message_length] = '\0';


            /*
               Move remaining data to beginning.
            */

            {
                int remaining_length;

                remaining_length =
                    pending_length
                    - (message_length + 1);

                memmove(
                    pending_data,
                    newline_position + 1,
                    remaining_length
                );

                pending_length =
                    remaining_length;

                pending_data[pending_length] =
                    '\0';
            }


            /*
               Remove CR.
            */

            message[
                strcspn(message, "\r")
            ] = '\0';


            /*
               Determine client ID.
            */

            if (
                client_index < 0
                &&
                message[0] == '['
            )
            {
                char *closing_bracket;

                closing_bracket =
                    strchr(message, ']');


                if (closing_bracket != NULL)
                {
                    int length;

                    length =
                        (int)(
                            closing_bracket
                            - message
                            - 1
                        );


                    if (
                        length > 0
                        &&
                        length < 64
                    )
                    {
                        strncpy(
                            client_id,
                            message + 1,
                            length
                        );

                        client_id[length] =
                            '\0';


                        WaitForSingleObject(
                            log_mutex,
                            INFINITE
                        );


                        client_index =
                            get_client_index(
                                client_id
                            );


                        if (client_index >= 0)
                        {
                            clients[
                                client_index
                            ].online = 1;

                            clients[
                                client_index
                            ].last_seen =
                                time(NULL);
                        }


                        ReleaseMutex(
                            log_mutex
                        );
                    }
                }
            }


            /*
               Process the message.
            */

            process_message(
                message,
                client_id,
                client_index,
                thread_id
            );
        }
    }


    /* =====================================================
       Client disconnected
       ===================================================== */

    WaitForSingleObject(
        log_mutex,
        INFINITE
    );


    if (client_index >= 0)
    {
        clients[client_index].online = 0;
    }


    printf(
        "\nClient disconnected. Thread ID: %lu\n",
        thread_id
    );


    display_client_statistics();

    display_health_status();


    ReleaseMutex(log_mutex);


    closesocket(client_socket);

    return 0;
}


/* =========================================================
   Main
   ========================================================= */

int main()
{
    WSADATA wsa;

    SOCKET server_socket;

    struct sockaddr_in server_addr;

    HANDLE thread_handle;


    printf(
        "========================================\n"
    );

    printf(
        "   DISTRIBUTED LOG MONITORING SERVER\n"
    );

    printf(
        "========================================\n"
    );


    /* =====================================================
       Initialize Winsock
       ===================================================== */

    if (
        WSAStartup(
            MAKEWORD(2, 2),
            &wsa
        ) != 0
    )
    {
        printf(
            "WSAStartup failed.\n"
        );

        return 1;
    }


    /* =====================================================
       Create socket
       ===================================================== */

    server_socket = socket(
        AF_INET,
        SOCK_STREAM,
        0
    );


    if (
        server_socket == INVALID_SOCKET
    )
    {
        printf(
            "Socket creation failed.\n"
        );

        WSACleanup();

        return 1;
    }


    /* =====================================================
       Configure server
       ===================================================== */

    memset(
        &server_addr,
        0,
        sizeof(server_addr)
    );


    server_addr.sin_family =
        AF_INET;

    server_addr.sin_addr.s_addr =
        INADDR_ANY;

    server_addr.sin_port =
        htons(PORT);


    /* =====================================================
       Bind
       ===================================================== */

    if (
        bind(
            server_socket,
            (struct sockaddr *)&server_addr,
            sizeof(server_addr)
        ) == SOCKET_ERROR
    )
    {
        printf(
            "Bind failed.\n"
        );

        closesocket(
            server_socket
        );

        WSACleanup();

        return 1;
    }


    /* =====================================================
       Listen
       ===================================================== */

    if (
        listen(
            server_socket,
            SOMAXCONN
        ) == SOCKET_ERROR
    )
    {
        printf(
            "Listen failed.\n"
        );

        closesocket(
            server_socket
        );

        WSACleanup();

        return 1;
    }


    /* =====================================================
       Create mutex
       ===================================================== */

    log_mutex =
        CreateMutex(
            NULL,
            FALSE,
            NULL
        );
        log_file = fopen("server.log", "a");

if (log_file == NULL)
{
    printf("Warning: Could not open server.log\n");
}


    if (log_mutex == NULL)
    {
        printf(
            "Failed to create mutex.\n"
        );

        closesocket(
            server_socket
        );
        if (log_file != NULL)
{
    fclose(log_file);
}

        WSACleanup();

        return 1;
    }


    /* =====================================================
       Server status
       ===================================================== */

    printf(
        "Server Status : RUNNING\n"
    );

    printf(
        "Port          : %d\n",
        PORT
    );

    printf(
        "Multi-client  : ENABLED\n"
    );

    printf(
        "Monitoring    : ENABLED\n"
    );

    printf(
        "Statistics    : ENABLED\n"
    );

    printf(
        "Heartbeat     : ENABLED\n"
    );

    printf(
        "========================================\n\n"
    );


    /* =====================================================
       Accept clients
       ===================================================== */

    while (1)
    {
        SOCKET client_socket;

        struct sockaddr_in client_addr;

        int client_addr_size =
            sizeof(client_addr);


        client_socket =
            accept(
                server_socket,
                (struct sockaddr *)&client_addr,
                &client_addr_size
            );


        if (
            client_socket ==
            INVALID_SOCKET
        )
        {
            printf(
                "Accept failed.\n"
            );

            continue;
        }


        SOCKET *socket_pointer =
            malloc(sizeof(SOCKET));


        if (socket_pointer == NULL)
        {
            printf(
                "Memory allocation failed.\n"
            );

            closesocket(
                client_socket
            );

            continue;
        }


        *socket_pointer =
            client_socket;


        thread_handle =
            CreateThread(
                NULL,
                0,
                handle_client,
                socket_pointer,
                0,
                NULL
            );


        if (thread_handle == NULL)
        {
            printf(
                "Failed to create client thread.\n"
            );

            closesocket(
                client_socket
            );

            free(socket_pointer);

            continue;
        }


        CloseHandle(
            thread_handle
        );
    }


    CloseHandle(
        log_mutex
    );

    closesocket(
        server_socket
    );

    WSACleanup();

    return 0;
}