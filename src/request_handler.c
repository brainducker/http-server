#include "request_handler.h"
#include "request.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


char *fetch_file_content(const char *file_to_open)
{
    FILE *f = fopen(file_to_open, "r");
    if (!f)
        return NULL;

    // Figure out file size
    fseek(f, 0, SEEK_END);
    size_t file_size = ftell(f);
    fseek(f, 0, SEEK_SET);

    // Allocate buffer, read file line by line

    char *request_buffer = malloc(file_size + 1);
    *request_buffer = 0; // Deals with case where file is empty

    size_t total_bytes_read = 0;
    while (fgets(request_buffer + total_bytes_read, 1024, f))
        total_bytes_read += strlen(request_buffer + total_bytes_read);
    
    fclose(f);
    
    return request_buffer;
}


char *prepare_response(const struct request *request) 
{
    if (!request)
        return generate_standard_response(400, "Bad request");

    char *file_to_open = malloc(20 + strlen(request->url));
    sprintf(file_to_open, "www%s", request->url);
    
    char *file_content = fetch_file_content(file_to_open);
    free(file_to_open);

    if (!file_content)
        return generate_standard_response(404, "Requested file was not found");

    char *res = generate_standard_response(200, file_content);

    free(file_content);

    return res;
}

const char *get_status_message(int status)
{
    switch(status) {
        case 200:
        return "OK";

        case 404:
        return "Not found";

        case 400:
        return "Bad request";

        default:
        return "Unknown status code";
    }
}

char *put_request_together(const struct response *response)
{
    char *buffer = malloc(MAX_HEADER_LENGTH * (1 + response->n_headers) + strlen(response->body));
    char *write_ptr = buffer;

    write_ptr += sprintf(write_ptr, "HTTP/1.1 %d %s\n", response->status_code,
        get_status_message(response->status_code));

    for (struct header_entry *current_entry = response->entries; 
         current_entry < response->entries + response->n_headers;
         ++current_entry)
        write_ptr += sprintf(write_ptr, "%s: %s\n", current_entry->name, current_entry->value);
    
    sprintf(write_ptr, "\n%s", response->body);

    return buffer;
}

char *generate_standard_response(int status, char *message)
{
    struct response res;
    res.status_code = status;
    res.entries = make_entry("Content-type", "text/html");
    res.n_headers = 1;

    res.body = message;

    return put_request_together(&res);
}
