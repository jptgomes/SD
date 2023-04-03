/*
 * ============================================================================
 *
 *      socket_servidor.c
 *
 *	Observação: (server.sin_addr.s_addr = INADDR_ANY;) esta linha precisa
 *	ser adicionada em propriedades do socket do servidor para testar entre hosts diferentes 
 *	para cliente e servidor, sendo assim o servidor usa o endereço definido no host.
 *
 *	 Adaptado de Gustavo Pantuza 
 *	 Disponível em: https://blog.pantuza.com/artigos/programando-um-protocolo-utilizando-sockets
 *   
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


/* Porta do servidor  */
#define PORT 4242


/* Tamanho do buffer */
#define BUFFER_LENGTH 4096


int
main(void) {

    /* Estruturas do socket do cliente e do servidor */
    struct sockaddr_in client, server;

    /* Arquivos descritores do cliente e servidor*/
    int serverfd, clientfd;

    char buffer[BUFFER_LENGTH];

    fprintf(stdout, "Iniciando servidor!\n");

    /* Cria um socket IPv4 */
    serverfd = socket(AF_INET, SOCK_STREAM, 0);
    if(serverfd == -1) {
        perror("Não é possível criar o socket do servidor:");
        return EXIT_FAILURE;
    }
    fprintf(stdout, "Socket servidor criado com o arquivo: %d\n", serverfd);


    /* Define as propriedades do socket do servidor */
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;   /* Foi inserido esta linha para usar o endereço do host 
    para teste entre hosts diferentes*/
    memset(server.sin_zero, 0x0, 8);


    /* Lidar com o erro da porta já em uso */
    int yes = 1;
    if(setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR,
                  &yes, sizeof(int)) == -1) {
        perror("Erro das opções de socket:");
        return EXIT_FAILURE;
    }


    /* vincular o socket a uma porta */
    if(bind(serverfd, (struct sockaddr*)&server, sizeof(server)) == -1 ) {
        perror("Erro de comunicação de socket:");
        return EXIT_FAILURE;
    }


    /* Aguarda as conexões do cliente */
    if(listen(serverfd, 1) == -1) {
        perror("Erro de escuta:");
        return EXIT_FAILURE;
    }
    fprintf(stdout, "Escutando na porta %d\n", PORT);

    socklen_t client_len = sizeof(client);
    if ((clientfd=accept(serverfd,
        (struct sockaddr *) &client, &client_len )) == -1) {
        perror("Aceitar erro:");
        return EXIT_FAILURE;
    }


    /* Copia para o buffer nossa mensagem de boas-vindas */
    strcpy(buffer, "Olá, cliente!\n\0");


    /* Envia a mensagem para o cliente */
    if (send(clientfd, buffer, strlen(buffer), 0)) {
        fprintf(stdout, "Cliente conectado.\n Aguardando mensagem do cliente...\n");

        /* Comunica-se com o cliente até chegar a mensagem de tchau */
        do {

            /* iniciando buffers */
            memset(buffer, 0x0, BUFFER_LENGTH);

            /* Recebendo mensagem do cliente */
            int message_len;
            if((message_len = recv(clientfd, buffer, BUFFER_LENGTH, 0)) > 0) {
                buffer[message_len - 1] = '\0';
                printf("Cliente diz: %s\n", buffer);
            }


            /* 'tchau' mensagem que finaliza a conexão */
            if(strcmp(buffer, "tchau") == 0) {
                send(clientfd, "tchau", 5, 0);
            } else {
                send(clientfd, "sim\n", 3, 0);
            }

        } while(strcmp(buffer, "tchau"));
    }

    /* Cliente encerra conexão */
    close(clientfd);

    /* Servidor encerra a conexão do socket local */
    close(serverfd);

    printf("Conexão encerrada!\n\n");

    return EXIT_SUCCESS;
}
