#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <limits.h>
#include <pthread.h>

#define SERVERPORT 8989
#define BUFSIZE 32768
#define SOCKETERROR (-1)
#define SERVER_BACKLOG 100

typedef struct sockaddr_in SA_IN;
typedef struct sockaddr SA;

void * handle_connection(void* p_client_socket);
int check(int exp, const char *msg);

int main(int argc, char **argv)
{
    int server_socket, client_socket, addr_size;
    SA_IN server_addr, client_addr;

    check((server_socket = socket(AF_INET, SOCK_STREAM, 0)),
            "Failed to create socket");

    // inicializar a struct de endereços
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVERPORT);

    check(bind(server_socket, (SA*)&server_addr, sizeof(server_addr)),
            "Bind failed");
    check(listen(server_socket, SERVER_BACKLOG),
            "Listen failed");

    while (true) {
        // esperar por e aceitar uma ligação a ser recebida
        printf("Waiting for connections...\n");
        addr_size = sizeof(SA_IN);
        check(client_socket =
                accept(server_socket, (SA*)&client_addr, (socklen_t*)&addr_size),
                "Accept failed");
        printf("Connected!\n");

        // handle connection
        pthread_t t;
        int *pclient = malloc(sizeof(int)); // alocar espaço no heap para um inteiro
        *pclient = client_socket; // guardar o valor do client_socket no novo endereço de memória
        //pthread_create(&t, NULL, handle_connection, pclient); // multithread
        handle_connection(pclient); // singlethread
    }

    return 0;
}

int check(int exp, const char *msg) {
    if (exp == SOCKETERROR) {
        perror(msg);
        exit(1);
    }
    return exp;
}

void * handle_connection(void* p_client_socket) {
    int client_socket = *((int*)p_client_socket);
    free (p_client_socket);
    char buffer[BUFSIZE];
    size_t bytes_read;
    int msgsize = 0;
    char actualpath[PATH_MAX+1]; // PATH_MAX está definido em <limits.h>

    // ler a mensagem do client -- o nome do ficheiro a ler do servidor
    while ((bytes_read = read(client_socket, buffer+msgsize, sizeof(buffer)-msgsize-1)) > 0) {
        msgsize += bytes_read;
        if (msgsize > BUFSIZE-1 || buffer[msgsize-1] == '\n') break;
    }
    check(bytes_read, "recv error"); // recv -- chamada usada para receber mensagens de um socket
    buffer[msgsize-1] = 0; // terminar a mensagem em null e remover o \n

    pthread_t ptid = pthread_self();
    printf("PThread ID: %lu\n", ptid);

    printf("REQUEST: %s\n", buffer);
    fflush(stdout);

    // verificar validação
    if (realpath(buffer, actualpath) == NULL) { // realpath() -- resolve a pathname (descrito em POSIX 1997/2008)
        printf("ERROR - bad path: %s\n", buffer);
        close(client_socket);
        return NULL;
    }

    // ler o conteúdo do ficheiro e enviar para o client
    // de modo a tornar o programa seguro, o acesso aos ficheiros deve ser limitado
    FILE *fp = fopen(actualpath, "r");
    if (fp == NULL) {
        printf("ERROR - open file: %s\n", buffer);
        close(client_socket);
        return NULL;
    }
    sleep(1); // testar a acumulação de atrasos em multithreading e singlethreading

    while ((bytes_read = fread(buffer, 1, BUFSIZE, fp)) > 0) {
        printf("Sending %zu bytes\n", bytes_read);
        write(client_socket, buffer, bytes_read);
    }

    close(client_socket);
    fclose(fp);
    printf("Closing connection\n");
    return NULL;

}
