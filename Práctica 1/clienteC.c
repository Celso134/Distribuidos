
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PUERTO 5000
#define BUFFER 1024

int main(void) {

    int cliente;
    char mensaje[BUFFER];
    char respuesta[BUFFER];

    signal(SIGPIPE, SIG_IGN);

    // 1. Crear socket TCP
    cliente = socket(AF_INET, SOCK_STREAM, 0);

    if (cliente == -1) {
        perror("Error al crear socket");
        return 1;
    }

    // 2. Configurar dirección del servidor Java
    struct sockaddr_in direccion = {0};

    direccion.sin_family = AF_INET;
    direccion.sin_port = htons(PUERTO);

    if (inet_pton(AF_INET, "127.0.0.1",
                  &direccion.sin_addr) != 1) {
        fprintf(stderr, "Direccion IP invalida\n");
        close(cliente);
        return 1;
    }

    // 3. Conectarse al servidor Java
    if (connect(cliente,
                (struct sockaddr *)&direccion,
                sizeof(direccion)) == -1) {
        perror("Error al conectar");
        close(cliente);
        return 1;
    }

    printf("Conectado al servidor Java\n");
    printf("Escribe mensajes (Ctrl+D para salir)\n");

    // 4. Leer mensajes del teclado
    while (fgets(mensaje, sizeof(mensaje), stdin) != NULL) {

        size_t longitud = strlen(mensaje);

        // Asegurar que cada mensaje termine en '\n'
        if (longitud == 0)
            continue;

        if (mensaje[longitud - 1] != '\n') {
            if (longitud == BUFFER - 1) {
                fprintf(stderr, "Mensaje demasiado largo\n");
                // Descartar el resto de la linea
                int c;
                while ((c = getchar()) != '\n' && c != EOF) {}
                continue;
            }
            mensaje[longitud++] = '\n';
            mensaje[longitud] = '\0';
        }

        // 5. Enviar todos los bytes del mensaje
        size_t enviados = 0;

        while (enviados < longitud) {

            ssize_t n = send(cliente,
                             mensaje + enviados,
                             longitud - enviados, 0);

            if (n <= 0) {
                perror("Error en send");
                close(cliente);
                return 1;
            }

            enviados += (size_t)n;
        }

        // 6. Esperar la confirmacion del servidor
        size_t i = 0;
        char caracter;

        while (i < sizeof(respuesta) - 1) {

            ssize_t n = recv(cliente, &caracter, 1, 0);

            if (n <= 0) {
                fprintf(stderr,
                        "Servidor desconectado o error de lectura\n");
                close(cliente);
                return 1;
            }

            respuesta[i++] = caracter;

            if (caracter == '\n')
                break;
        }

        respuesta[i] = '\0';

        // 7. Mostrar confirmacion
        printf("Servidor Java: %s", respuesta);
    }

    close(cliente);
    printf("Conexion finalizada\n");

    return 0;
}
