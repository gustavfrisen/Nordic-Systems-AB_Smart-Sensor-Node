#ifndef _TCP_CLIENT_H
#define _TCP_CLIENT_H

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>

typedef struct tcp_client_t {
    int fd;
} tcp_client_t;

int tcp_client_init(tcp_client_t* _client);
int tcp_client_init_ptr(tcp_client_t** _client);

int tcp_client_connect(tcp_client_t* _client, const char* _host, const char* _port);
int tcp_client_write(tcp_client_t* _client, const uint8_t* _buffer, int _len);
int tcp_client_read(tcp_client_t* _client, uint8_t* _buffer, int _len);
void tcp_client_disconnect(tcp_client_t* _client);

void tcp_client_dispose(tcp_client_t* _client);
void tcp_client_dispose_ptr(tcp_client_t** _client);

#endif