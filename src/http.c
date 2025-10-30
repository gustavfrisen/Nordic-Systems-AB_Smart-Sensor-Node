#include "http.h"
#include "utils.h"

int http_response_init(http_response_t* response)
{
    if (!response) return -1;

    response->status = -1;
    response->reason = NULL;
    response->content_type = NULL;
    response->content_length = NULL;
    response->body = NULL;

    return 0;
}

int http_response_init_ptr(http_response_t** response)
{
    if (!response) return -1;

    *response = malloc(sizeof(http_response_t));
    if (!*response) return -1;

    http_response_init(*response);

    return 0;
}

int http_build_get(const char* _host, const char* _path, char** _out_request)
{
    if (!_host || !_path || !_out_request) return -1;
    
    size_t host_len = strlen(_host);
    size_t path_len = strlen(_path);
    
    if (host_len == 0 || host_len > 255 || path_len == 0 || path_len > 255) {
        return -1;
    }
    
    size_t required_len = 4 + path_len + 11 + 7 + host_len + 4 + 1;
    
    *_out_request = malloc(required_len);
    if (!*_out_request) return -1;

    int written = snprintf(*_out_request, required_len, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", _path, _host);
    
    if (written < 0 || (size_t)written >= required_len) {
        free(*_out_request);
        *_out_request = NULL;
        return -1;
    }

    return 0;
}

int http_build_post(const char* _host, const char* _path, const char* _content_type, const char* _body, char** _out_request)
{
    if (!_host || !_path || !_content_type || !_body || !_out_request) return -1;
    
    size_t host_len = strlen(_host);
    size_t path_len = strlen(_path);
    size_t content_type_len = strlen(_content_type);
    size_t body_len = strlen(_body);
    
    if (host_len == 0 || host_len > 255 || path_len == 0 || path_len > 255) {
        return -1;
    }
    
    if (content_type_len == 0 || content_type_len > 255) {
        return -1;
    }
    
    char body_len_str[32];
    int len_chars = snprintf(body_len_str, sizeof(body_len_str), "%zu", body_len);
    if (len_chars < 0) return -1;
    
    size_t required_len = 5 + path_len + 11 +           // "POST " + path + " HTTP/1.1\r\n"
                         7 + host_len + 2 +             // "Host: " + host + "\r\n"
                         15 + content_type_len + 2 +    // "Content-Type: " + content_type + "\r\n"
                         16 + len_chars + 2 +           // "Content-Length: " + len + "\r\n"
                         2 +                            // "\r\n"
                         body_len +                     // body
                         1;                             // null terminator
    
    *_out_request = malloc(required_len);
    if (!*_out_request) return -1;

    int written = snprintf(*_out_request, required_len,
        "POST %s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %zu\r\n"
        "\r\n"
        "%s",
        _path, _host, _content_type, body_len, _body);
    
    if (written < 0 || (size_t)written >= required_len) {
        free(*_out_request);
        *_out_request = NULL;
        return -1;
    }

    return 0;
}

int http_parse_response(const char* _raw_response, http_response_t** _out_response)
{
    if (!_raw_response || !_out_response) return -1;

    *_out_response = (http_response_t*)malloc(sizeof(http_response_t));

    http_response_t* resp = *_out_response;
    resp->body = NULL;
    resp->content_length = NULL;
    resp->content_type = NULL;
    resp->reason = NULL;
    resp->status = -1;

    char* header_start = NULL;
    char* header_end = NULL;
    size_t header_len = 0;

    char* body_start = NULL;
    char* body_end = NULL;
    size_t body_len = 0;

    header_start = (char*)_raw_response;
    header_end = strstr(_raw_response, "\r\n\r\n");
    if (header_end == NULL) return -1;

    body_start = header_end + 4;
    body_end = (char*)(_raw_response + strlen(_raw_response));

    header_len = header_end - header_start;
    body_len = body_end - body_start;

    char header_buf[header_len];
    char body_buf[body_len];

    // write status and reason to http response
    if (snprintf(header_buf, header_len, "%s", header_start) > 0) {
        // parse status
        char* status_start = NULL;
        char* status_end = NULL;
        size_t status_len = 0;
        char* reason_start = NULL;
        char* reason_end = NULL;
        size_t reason_len = 0;

        status_start = strchr(header_buf, ' ');
        status_start += 1;
        status_end = status_start;
        status_end = strchr(status_end, ' ');
        status_len = status_end - status_start;
        status_len += 1;

        char status_buf[status_len];
        snprintf(status_buf, status_len, "%s", status_start);
        status_buf[status_len] = '\0';
        
        // parse reason
        reason_start = status_end + 1;
        reason_end = strstr(reason_start, "\r\n");
        reason_len = reason_end - reason_start;
        reason_len += 1;
        char reason_buf[reason_len];
        snprintf(reason_buf, reason_len, "%s", reason_start);
        reason_buf[reason_len] = '\0';

        resp->status = strtol(status_buf, NULL, 10);
        resp->reason = strdup(reason_buf);
    } else return -1;
    
    // write body to http response
    if (snprintf(body_buf, body_len, "%s", body_start) > 0) {
        resp->body = strdup(body_buf);
    } else return -1;

    // parse content type
    char* content_type_start = NULL;
    char* content_type_end = NULL;
    size_t content_type_len = 0;
    char content_type_buf[content_type_len];
    content_type_start = strstr(header_start, "Content-Type: ");
    if (content_type_start != NULL) {
        content_type_start = content_type_start + strlen("Content_Type: ");
        content_type_end = strstr(content_type_start, "\r\n");
        content_type_len = content_type_end - content_type_start;
        content_type_len += 1;
        if (snprintf(content_type_buf, content_type_len, "%s", content_type_start) > 0) {
            content_type_buf[content_type_len] = '\0';
            resp->content_type = strdup(content_type_buf);
        } else {
            resp->content_type = "N/A";
        }
    } else {
        resp->content_type = "N/A";
    }

    // parse content length
    char* content_length_start = NULL;
    char* content_length_end = NULL;
    size_t content_length_len = 0;
    char content_length_buf[content_length_len];
    content_length_start = strstr(header_start, "Content-Length: ");
    if (content_length_start != NULL) {
        content_length_start = content_length_start + strlen("Content-Length: ");
        content_length_end = strstr(content_length_start, "\r\n");
        if (content_length_end == NULL) return -1;
        content_length_len = content_length_end - content_length_start;
        content_length_len += 1;
        if (snprintf(content_length_buf, content_length_len, "%s", content_length_start) > 0) {
            content_length_buf[content_length_len] = '\0';
            resp->content_length = malloc(sizeof(size_t));
            *resp->content_length = (size_t)strtol(content_length_buf, NULL, 10);
        } else {
            resp->content_length = malloc(sizeof(size_t));
            *resp->content_length = (size_t)0;
        }
    } else {
        resp->content_length = malloc(sizeof(size_t));
        *resp->content_length = (size_t)0;
    }

    return 0;
}

void http_response_dispose(http_response_t* _response)
{
    if (!_response) return;
    
    free(_response->reason);
    free(_response->content_type);
    free(_response->body);
    free(_response->content_length);
    
    _response->reason = NULL;
    _response->content_type = NULL;
    _response->body = NULL;
    _response->content_length = NULL;
    _response->status = -1;
}

void http_response_dispose_ptr(http_response_t** _response)
{
    if (!_response || !*_response) return;
    
    http_response_dispose(*_response);
    free(*_response);
    *_response = NULL;
}
