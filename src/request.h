#ifndef REQUEST_H
#define REQUEST_H

#include <stddef.h>

#define MAX_HEADER_LENGTH 256

// Defines the functions used to parse and put together requests


// TODO complete this

enum request_method {
    GET,
    POST // TODO
};

struct header_entry {
    char *name;
    char *value;
};

struct request {
    char *url;
    enum request_method method;

    size_t n_headers;
    struct header_entry *entries;

    char *body;
};

struct response {
    int status_code;

    size_t n_headers;
    struct header_entry *entries;

    char *body;
};

int parse_request(char *raw_request, struct request *headers);
struct header_entry *parse_request_headers(char *headers, size_t *n_headers);
void free_header_entries (struct header_entry *begin, size_t n);
void free_header_entry(struct header_entry *p);
void remove_carets(char *p);
struct header_entry *make_entry(const char *name, const char *value);

#endif
