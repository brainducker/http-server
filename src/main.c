#include <stdio.h>
#include "server.h"

int main()
{
    printf("Let's run the server madafackas !");
    if (!run_server(80))
        perror("");
    return 0;
}
