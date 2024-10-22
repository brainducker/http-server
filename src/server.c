#include "server.h"
#include "request.h"
#include "request_handler.h"
#include <winsock2.h>
#include <windows.h>

#include <string.h>
#include <stdio.h>


int init_sockets()
{
    WSADATA wsa_data;
    return WSAStartup(MAKEWORD(2, 2), &wsa_data) == 0;
}


int run_server(int port)
{
    if (!init_sockets())
        return 0;

    SOCKET server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket == INVALID_SOCKET) {
        perror("socket");
        return 0;
    }

    struct sockaddr_in server_address = {0};
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port);
    server_address.sin_family = AF_INET;

    const struct sockaddr* p_server_address = 
        (struct sockaddr*) &server_address;

    if (bind(server_socket,
             p_server_address, 
             sizeof(struct sockaddr_in)) < 0) {
        perror("bind");
        return 0;
    }

    if (listen(server_socket, 1) == SOCKET_ERROR) {
        perror("listen");
        return 0;
    }
    
    struct sockaddr_in client_address;
    int client_addr_size;
    SOCKET client_socket;

    while ((client_socket = accept(server_socket, (struct sockaddr*) &client_address, &client_addr_size)) != INVALID_SOCKET) {

        if (!handle_inbound_connection(client_socket))
            return 0;
    }

    return 1;
}

int is_finished_request_body(const char *buffer, int buffer_size)
{
    // Utility function
    return (buffer[buffer_size-2] == '\r' && buffer[buffer_size-1] == '\n' &&
            buffer[buffer_size-4] == '\r' && buffer[buffer_size-3] == '\n') ||
            (buffer[buffer_size-1] == '\n' && buffer[buffer_size-2] == '\n');
}

char *read_request(SOCKET client_socket, int *bytes_read)
{
    size_t current_buffer_capacity = 0;
    const size_t buffer_increment = 1024;
    char *buffer = NULL;
    *bytes_read = 0;

    do {
        current_buffer_capacity += buffer_increment;
        buffer = realloc(buffer, current_buffer_capacity);
        *bytes_read += recv(client_socket, buffer + *bytes_read, buffer_increment, 0);
        buffer[*bytes_read] = 0;
    } while (!is_finished_request_body(buffer, *bytes_read));

    return buffer;
}

int send_response(SOCKET client_socket, char *response)
{
    printf("Response sent: \n\n ========== \n %s", response);
    return send(client_socket, response, 1+strlen(response), 0);
}

int handle_inbound_connection(SOCKET client_socket)
{
    int request_length;
    char *request = read_request(client_socket, &request_length);
    printf("\n Request received: \n\n ==========\n%s", request);

    struct request parsed_request;
    char *response = prepare_response( (parse_request(request, &parsed_request) ? &parsed_request: NULL));
    free(request);

    // Clear parsed reauest
    free(parsed_request.body);
    free(parsed_request.url);
    free_header_entries(parsed_request.entries, parsed_request.n_headers);

    send_response(client_socket, response);

    free(response);
    closesocket(client_socket);

    return 1;
}
