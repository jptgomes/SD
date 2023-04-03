/*
 * ============================================================================
 *
 *      socket_cliente.c
 *
 *	Atenção: é preciso alterar o endereço do servidor em: 
 *	#define SERVER_ADDR "endereço do servidor"
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
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


/* Informa a porta no servidor */
#define PORT 4242


/* Comprimento dos buffers de sockets */
#define LEN 4096


/* endereço do servidor. Como estamos usando uma VM, lembrar de fazer o redirecionamento de porta: 
Criar um nome para o redirecionamento chamado socket, IP do hospedeiro, porta 4242, IP do convidado, porta 4242*/

#define SERVER_ADDR "192.168.0.10"


int 
main(int argc, char *argv[]) {

    /*  socket servidor*/
    struct sockaddr_in server;
    /* Descritor de arquivo do cliente para o socket local */
    int sockfd;

    int len = sizeof(server);
    int slen;

    /* Buffer de recebimento */
    char buffer_in[LEN];
    /* Send buffer */
    char buffer_out[LEN];


    fprintf(stdout, "Iniciando cliente ...\n");


    /* Cria um soquete para o cliente*/
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Erro na criação do socket do cliente:");
        return EXIT_FAILURE;
    }
    fprintf(stdout, "Socket do cliente criado com arquivo: %d\n", sockfd);


    /* Define as propriedades da conexão */
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr(SERVER_ADDR);
    memset(server.sin_zero, 0x0, 8);


    /* Tenta se conectar ao servidor */
    if (connect(sockfd, (struct sockaddr*) &server, len) == -1) {
        perror("Não é possível conectar ao servidor!");
        return EXIT_FAILURE;
    }


    /* Recebe a mensagem de apresentação do servidor */
    if ((slen = recv(sockfd, buffer_in, LEN, 0)) > 0) {
        buffer_in[slen + 1] = '\0';
        fprintf(stdout, "Servidor: %s\n", buffer_in);
    }


    /*
     * Comunique-se com o servidor até que a mensagem de saída chegue
     */
    while (true) {

        /* Zerando os buffers */
        memset(buffer_in, 0x0, LEN);
        memset(buffer_out, 0x0, LEN);

        fprintf(stdout, "Diga algo ao servidor: ");
        fgets(buffer_out, LEN, stdin);

        /* Envia a mensagem lida para o servidor através do socket */
        send(sockfd, buffer_out, strlen(buffer_out), 0);

        /* Recebe uma resposta do servidor */
        slen = recv(sockfd, buffer_in, LEN, 0);
        printf("Resposta do servidor: %s\n", buffer_in);

        /* mensagem 'tchau' finaliza a conexão */
        if(strcmp(buffer_in, "tchau") == 0)
            break;
    }


    /* Fecha a conexão com o servidor */
    close(sockfd);

    fprintf(stdout, "\nConexão encerrada\n\n");

    return EXIT_SUCCESS;
}
