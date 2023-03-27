#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <errno.h>

#define DEFAULT_PORT 5000
#define BUFFER_SIZE 1024
#define SEND_MSG "all ok"

void data_dump(const uint8_t *bytes, const size_t size) {
    puts("-------------- Data Dump -----------------");
    const size_t width = 16;
    const size_t height = ceil((double)size / 16.0);
    size_t top = 0;
    for (; top < height; top++) {
        printf("0x%-7.4zx", top*width);
        // hex code
        for (size_t left = 0; left < width; left++) {
            if (left)       printf(" ");
            if (left == 8)  printf(" ");
            if (top*width + left < size)
                printf("%.2x", bytes[top*width + left]);
            else
                printf("  ");
        }

        // add padding
        printf("%3s", "");

        // printable characters
        for (size_t left = 0; left < 16 && top*width +left < size; left++) {
            if (left == 8)  printf(" ");
            if (bytes[top*width + left] >= 32 && bytes[top*width + left] < 128)
                putchar(bytes[top*width + left]);
            else
                putchar('.');
        }
        putchar('\n');
    }
    puts("");
}

int main(int argc, char *argv[]) {
    uint16_t port = DEFAULT_PORT;
    if (argc > 1) {
        port = (uint16_t)strtol(argv[1], NULL, 10);
        if (port < 1024) {
            fprintf(stderr, "Cannot use reserved ports! Available ports are in range 1024-65535.\n");
            exit(1);
        }
    }

    int server_fd, client_fd;
    struct sockaddr_in server_si, client_si;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "Cannot create a socket.\n");
        exit(1);
    }

    server_si.sin_family = AF_INET;
    server_si.sin_addr.s_addr = INADDR_ANY;
    server_si.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&server_si, sizeof(server_si)) < 0) {
        fprintf(stderr, "Cannot bind socket to the port!\n");
        exit(1);
    }

    if (listen(server_fd, 2) < 0) {
        fprintf(stderr, "Cannot listen!\n");
        exit(1);
    }

    size_t addr_len = sizeof(struct sockaddr_in);
    uint8_t buffer[BUFFER_SIZE] = {0, };

    while ((client_fd = accept(server_fd, (struct sockaddr *)&client_si, (socklen_t *)&addr_len))) {
        while (1) {
            // clear buffer
            memset(buffer, 0, BUFFER_SIZE);

            // process received msg
            int msg_len = read(client_fd, buffer, BUFFER_SIZE);
            if (msg_len < 1)
                break;
            data_dump(buffer, msg_len);
            puts("=======================================");

            // echo back received data
            write(client_fd, buffer, msg_len);
        }
    }
    close(server_fd);

    return 0;
}
