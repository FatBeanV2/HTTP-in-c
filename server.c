#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdbool.h>


int handle_client(int client_socket) {
    ssize_t n = 0;
    char buffer[1024];
    const char* hello = "HTTP/1.0 200 OK\r\n\r\nHello, World!";

    for (;;) {
        memset(buffer, 0, sizeof(buffer));

        n = read(client_socket, buffer, sizeof(buffer)-1);
        if (n < 0) {
            perror("read()");
            return -1;
        }
        if (n == 0) {
            printf("connection closed\n");
            break;
        }

        printf("REQUEST:\n%s", buffer);
        (void)write(client_socket, hello, strlen(hello));
        close(client_socket);
        break;
    }
    printf("\n---\n");


    return 0;
}

int main(void) {
    /* declare */
    int rc = 0;
    struct sockaddr_in bind_addr;
    int tcp_socket = 0;
    int ret = 0;
    int client_socket = 0;
    int enabled = true;

    /* initialize */
    memset(&bind_addr, 0, sizeof(bind_addr));
    tcp_socket = socket(
        AF_INET, /* IPv4 */
        SOCK_STREAM, /* TCP */
        0
    );

    if (tcp_socket < 0) {
        perror("socket()");
        return 1;

    }
    printf("socket success\n");

    (void)setsockopt(tcp_socket, SOL_SOCKET, SO_REUSEADDR, &enabled, sizeof(enabled));

    bind_addr.sin_port = htons(8456);
    bind_addr.sin_family = AF_INET;
    bind_addr.sin_addr.s_addr = INADDR_ANY;

    rc = bind(tcp_socket,(const struct sockaddr*)&bind_addr, sizeof(bind_addr));
    if (rc < 0) {
        perror("bind()");
        ret = 1;
        goto exit;

    }
    printf("bind succeeded\n");

    rc = listen(tcp_socket, SOMAXCONN);
    if (rc < 0) {
        perror("listen()");
        ret = 1;
        goto exit;
    }

    printf("listen succeeded\n");

    for (;;) {
        printf("waiting for connection... \n");
        client_socket = accept(tcp_socket, NULL, NULL);

        printf("connection successful!\n");
        rc = handle_client(client_socket);

    }

exit:    
    close(tcp_socket);
    return ret;
}

