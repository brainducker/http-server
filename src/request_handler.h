#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H
#include "request.h"

char *put_request_together(const struct response *response);
char *prepare_response(const struct request *request);
char *generate_standard_response(int status, char *message);
char *fetch_file_content(const char *file_to_open);

#endif
