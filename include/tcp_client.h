#ifndef _TCP_CLIENT_H
#define _TCP_CLIENT_H

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>

typedef struct tcp_client {
    int fd;
} tcp_client;

int tcp_client_init(tcp_client* _client);
int tcp_client_connect(tcp_client* _client, const char* _host, const char* _port);
int tcp_client_write(tcp_client* _client, const uint8_t* _buffer, int _len);
int tcp_client_read(tcp_client* _client, uint8_t* _buffer, int _len);
void tcp_client_disconnect(tcp_client* _client);
void tcp_client_dispose(tcp_client* _client);

#endif