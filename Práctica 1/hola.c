#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

struct sockaddr_in direccion = {0};

int main() {

	int servidor = socket(AF_INET, SOCK_STREAM, 0);
	if(servidor == -1){
		perror("Error al crear el socket");
	}
	direccion.sin_family = AF_INET;
	direccion.sin_addr.s_addr = htonl(INADDR_ANY);
	direccion.sin_port = htons(5000);
	bind(servidor, (struct sockaddr *)&direccion, sizeof(direccion));
	listen(servidor,5);
	int cliente = accept(servidor, NULL, NULL);
	if (cliente == -1){
		perror("ZAS");
		close(servidor);
		return 1;
	}
	char buffer[1024];
	ssize_t bytesRecibidos;
	while ((bytesRecibidos = recv(cliente, buffer, sizeof(buffer), 0)) > 0){
		printf("Recibidos %zd bytes\n", bytesRecibidos);
		size_t enviados = 0;
		while(enviados < (size_t)bytesRecibidos){
			ssize_t n = send(cliente, buffer + enviados, (size_t)bytesRecibidos - enviados, 0);
			if (n <= 0){
				perror("send");
				close(cliente);
				close(servidor);
				return 1;
			}
			enviados += (size_t)n;
		}
	}
	if (bytesRecibidos == -1) {
		perror("recv");
	} else {
		printf("Cliente desconectado\n");
	}

	close(cliente);
	close(servidor);

	return bytesRecibidos == -1 ? 1 : 0;
}
