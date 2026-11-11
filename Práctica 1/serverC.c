
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PUERTO 5000
#define TAM 128

// Enviar todos los bytes de una cadena
int enviar(int socket, const char *mensaje) {
    size_t longitud = strlen(mensaje);
    size_t enviados = 0;

    while (enviados < longitud) {
        ssize_t n = send(socket, mensaje + enviados,
                         longitud - enviados, 0);

        if (n <= 0) return -1;
        enviados += (size_t)n;
    }

    return 0;
}

// Recibir una linea completa terminada en '\n'
// Devuelve 1 si leyo la linea, 0 si se cerro
// la conexion y -1 si ocurrio un error.
int recibir_linea(int socket, char *buffer, size_t tam) {
    size_t i = 0;
    char caracter;

    while (1) {
        ssize_t n = recv(socket, &caracter, 1, 0);

        if (n == 0) return 0;
        if (n < 0) return -1;

        if (caracter == '\n') {
            buffer[i] = '\0';
            return 1;
        }

        if (i >= tam - 1) return -1;

        buffer[i++] = caracter;
    }
}

int main(void) {
    signal(SIGPIPE, SIG_IGN);

    // 1. Crear socket
    int servidor = socket(AF_INET, SOCK_STREAM, 0);

    if (servidor == -1) {
        perror("socket");
        return 1;
    }

    int opcion = 1;
    setsockopt(servidor, SOL_SOCKET, SO_REUSEADDR,
               &opcion, sizeof(opcion));

    // 2. Configurar direccion
    struct sockaddr_in direccion = {0};

    direccion.sin_family = AF_INET;
    direccion.sin_addr.s_addr = htonl(INADDR_ANY);
    direccion.sin_port = htons(PUERTO);

    // 3. Asociar puerto
    if (bind(servidor, (struct sockaddr *)&direccion,
             sizeof(direccion)) == -1) {
        perror("bind");
        close(servidor);
        return 1;
    }

    // 4. Escuchar conexiones
    if (listen(servidor, 5) == -1) {
        perror("listen");
        close(servidor);
        return 1;
    }

    printf("Servidor C escuchando en puerto %d\n", PUERTO);
    fflush(stdout);

    // 5. Aceptar cliente
    int cliente = accept(servidor, NULL, NULL);

    if (cliente == -1) {
        perror("accept");
        close(servidor);
        return 1;
    }

    printf("Cliente Java conectado\n");

    char buffer[TAM];
    char respuesta[TAM];

    // 6. Procesar numeros recibidos
    while (1) {
        int estado = recibir_linea(cliente, buffer,
                                   sizeof(buffer));

        if (estado <= 0) {
            printf("Conexion finalizada\n");
            break;
        }

        char *fin;
        errno = 0;
        long numero = strtol(buffer, &fin, 10);

        // Comprobar que sea un entero valido
        if (errno == ERANGE || fin == buffer ||
            *fin != '\0' || numero < INT32_MIN ||
            numero > INT32_MAX) {

            if (enviar(cliente, "ERROR\n") == -1)
                break;
            continue;
        }

        printf("Numero recibido: %ld\n", numero);

        // 7. Condicion de salida
        if (numero == 0) {
            enviar(cliente, "FIN\n");
            printf("Cero recibido. Cerrando servidor.\n");
            break;
        }

        // Evitar desbordamiento al sumar
        if (numero == INT32_MAX) {
            if (enviar(cliente, "ERROR\n") == -1)
                break;
            continue;
        }

        // 8. Sumar uno
        long resultado = numero + 1;

        snprintf(respuesta, sizeof(respuesta),
                 "%ld\n", resultado);

        printf("Enviando resultado: %ld\n", resultado);

        if (enviar(cliente, respuesta) == -1) {
            perror("send");
            break;
        }
    }

    // 9. Cerrar sockets
    close(cliente);
    close(servidor);

    printf("Servidor finalizado\n");
    return 0;
}
