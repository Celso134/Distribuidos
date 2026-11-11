#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

struct sockaddr_in direccion = {0};

int main(void){
	int cliente = socket(AF_INET, SOCK_STREAM, 0);
	char mensaje[1024];
	char respuesta[1024];
	direccion.sin_family = AF_INET;
	direccion.sin_port = htons(5000);
	inet_pton(AF_INET, "127.0.0.1", &direccion.sin_addr);

	connect(cliente, (struct sockaddr *)&direccion, sizeof(direccion));
	while (fgets(mensaje, sizeof(mensaje), stdin) != NULL){
		size_t longitud = strlen(mensaje);
		size_t enviados = 0;

		while(enviados < longitud) {
			ssize_t n = send(cliente, mensaje + enviados, longitud - enviados, 0);
			if(n<=0) {
				perror("send");
				close(cliente);
				return 1;
			}
			enviados += (size_t)n;
		}
		size_t i = 0;
		char caracter;
		while(i < sizeof(respuesta) -1 ){
			ssize_t n = recv(cliente, &caracter, 1, 0);
			if(n <= 0){
				fprintf(stderr, "Conexion cerrada o error\n");
				close(cliente);
				return 1;
			}
			respuesta[i++] = caracter;
			if(caracter == '\n')
				break;
		}
		respuesta[i] = '\0';
		printf("Eco del servidor: %s", respuesta);
	}
	close(cliente);
	return 0;
}
