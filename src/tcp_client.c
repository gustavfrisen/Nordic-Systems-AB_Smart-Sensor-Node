#include "tcp_client.h"

int tcp_client_init(tcp_client_t* _client)
{
    _client->fd = -1;
    return 0;
}

int tcp_client_init_ptr(tcp_client_t** _client)
{
    if (!_client) return -1;

    *_client = malloc(sizeof(tcp_client_t));
    if (!*_client) return -1;

    tcp_client_init(*_client);

    return 0;
}

int tcp_client_connect(tcp_client_t* _client, const char* _host, const char* _port)
{
    struct addrinfo hints = {0}, *res = NULL;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(_host, _port, &hints, &res) != 0) return -1;

    int fd = -1;
    for (struct addrinfo *rp = res; rp; rp = rp->ai_next)
    {
        fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (fd < 0) continue;

        if (connect(fd, rp->ai_addr, rp->ai_addrlen) == 0) break;

        close(fd);
        fd = -1;
    }

    freeaddrinfo(res);
    if (fd < 0) return -1;

    _client->fd = fd;

    return 0;
}

int tcp_client_write(tcp_client_t* _client, const uint8_t* _buffer, int _len)
{
    return send(_client->fd, _buffer, _len, MSG_NOSIGNAL);
}

int tcp_client_read(tcp_client_t* _client, uint8_t* _buffer, int _len)
{
    return recv(_client->fd, _buffer, _len, 0);
}

void tcp_client_disconnect(tcp_client_t* _client)
{
    if (_client->fd >= 0) close(_client->fd);
    _client->fd = -1;
}

void tcp_client_dispose(tcp_client_t* _client)
{
    if (!_client) return;

    tcp_client_disconnect(_client);
}

void tcp_client_dispose_ptr(tcp_client_t** _client)
{
    if (!_client || !*_client) return;

    tcp_client_dispose(*_client);
    free(*_client);
    *_client = NULL;
}
