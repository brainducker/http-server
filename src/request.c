#include "request.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
 
int are_strings_equal_nocase(const char *a, const char *b) {
    while (*a || *b) {
        if (toupper(*a) != toupper(*b))
            return 0;
        ++a;
        ++b;
    }

    return 1;
}

int parse_request(char *raw_request, struct request *headers)
{
    // Identify the first row of request, split it into words
    char *first_row = strtok(raw_request, "\n");
    char *second_row = first_row + strlen(first_row) + 1;

    remove_carets(first_row);

    char *current_token = strtok(first_row, " ");

    if (!are_strings_equal_nocase(current_token, "get"))
        return 0; // At this point methods other than get are not supported
    
    current_token = strtok(NULL, " ");

    char *buffer = malloc(strlen(current_token) + 1);
    strcpy(buffer, current_token);

    headers->url = buffer;
    headers->entries = parse_request_headers(second_row, &(headers->n_headers));

    return 1;
}

void remove_carets(char *p)
{
    // For convenience
    for (; *p; ++p) {
        if (*p == '\r') {
            *p = 0;
            return;
        }
    }

}


struct header_entry *parse_request_headers(char *headers, size_t *n_headers)
{
    // How many headers are we looking at? 
    *n_headers = 0;
    for (char *p = headers; *p; ++p) {
        if (*p == '\n')
            ++(*n_headers);
    }

    *n_headers -= 2; // To account for the last \n\n at the end

    struct header_entry *ret = malloc(*n_headers * sizeof(struct header_entry));
    for (struct header_entry *p = ret; p < ret + *n_headers; ++p)
        p->name = p->value = NULL;

    int strtok_done = 0;
    
    for (struct header_entry *p = ret; p < ret + *n_headers; ++p) {
        char *char_ptr = strtok((strtok_done) ? NULL: headers, ":");
        strtok_done = 1;
        if (!char_ptr) {
            // Free the mess before anything
            free_header_entries(ret, *n_headers);
            free(ret);
            return NULL;
        }

        remove_carets(char_ptr);
        p->name = malloc(strlen(char_ptr) +1);
        strcpy(p->name, char_ptr);

        if (!(char_ptr = strtok(NULL, "\n"))) {
            free_header_entries(ret, *n_headers);
            free(ret);
            return NULL;
        }

        remove_carets(char_ptr);
        p->value = malloc(strlen(char_ptr) + 1);
        strcpy(p->value, char_ptr);
    }

    return ret;
}

void free_header_entries(struct header_entry *begin, size_t n)
{
    // Another function written merely because it's convenient
    struct header_entry const *end = begin + n;
    for (; begin < end; ++begin)
        free_header_entry(begin);
}

void free_header_entry(struct header_entry *p)
{
    if (p->name)
        free(p->name);
    if (p->value)
        free(p->value);
}

struct header_entry *make_entry(const char *name, const char *value)
{
    // For convenience, allocates all that shit on the heap
    struct header_entry *res = malloc(sizeof(struct header_entry));
    res->name = malloc(strlen(name) + 1);
    strcpy(res->name, name);

    res->value = malloc(strlen(value) + 1);
    strcpy(res->value, value);

    return res;
}
