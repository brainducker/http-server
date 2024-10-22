#ifndef SERVER_H
#define SERVER_H

#include <winsock2.h>
#include <windows.h>

int init_sockets();
int run_server(int port);
int handle_inbound_connection(SOCKET client_socket);


#endif
