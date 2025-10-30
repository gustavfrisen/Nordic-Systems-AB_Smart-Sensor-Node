#ifndef _HTTP_H
#define _HTTP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stddef.h>

typedef struct http_response_t {
    int status;
    char* reason;
    char* content_type;
    size_t* content_length;
    char* body;
} http_response_t;

int http_response_init(http_response_t* response);
int http_response_init_ptr(http_response_t** response);

// Builds a simple HTTP GET request
int http_build_get(const char* _host, const char* _path, char** _out_request);

// Builds a simple HTTP POST request
int http_build_post(const char* _host, const char* _path, const char* _content_type, const char* _body, char** _out_request);

// Parses a raw HTTP response string into an http_response_t structure
int http_parse_response(const char* _raw_response, http_response_t** _out_response);

void http_response_dispose(http_response_t* _response);
void http_response_dispose_ptr(http_response_t** _response);

#endif