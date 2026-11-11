
import java.io.*;
import java.net.*;
import java.nio.charset.StandardCharsets;

public class ClienteJava {

    public static void main(String[] args) {

        String host = "localhost";
        int puerto = 5000;

        try (
            Socket socket = new Socket(host, puerto);

            BufferedReader servidor = new BufferedReader(
                new InputStreamReader(
                    socket.getInputStream(),
                    StandardCharsets.UTF_8));

            PrintWriter salida = new PrintWriter(
                new OutputStreamWriter(
                    socket.getOutputStream(),
                    StandardCharsets.UTF_8), true);

            BufferedReader teclado = new BufferedReader(
                new InputStreamReader(System.in))
        ) {

            System.out.println("Conectado al servidor C");
            System.out.println("Escribe numeros enteros");
            System.out.println("Introduce 0 para salir");

            while (true) {

                System.out.print("Numero: ");
                String texto = teclado.readLine();

                if (texto == null) {
                    break;
                }

                int numero;

                // Comprobar entrada del usuario
                try {
                    numero = Integer.parseInt(texto.trim());
                } catch (NumberFormatException e) {
                    System.out.println("Introduce un entero valido");
                    continue;
                }

                // Enviar numero al servidor C
                salida.println(numero);

                if (salida.checkError()) {
                    System.out.println("Error al enviar");
                    break;
                }

                // Esperar la respuesta
                String respuesta = servidor.readLine();

                if (respuesta == null) {
                    System.out.println("Servidor desconectado");
                    break;
                }

                // Detectar cierre
                if (numero == 0) {
                    System.out.println("Servidor: " + respuesta);
                    break;
                }

                System.out.println("Resultado: " + respuesta);
            }

            System.out.println("Cliente finalizado");

        } catch (IOException e) {
            System.err.println("Error de conexion: "
                               + e.getMessage());
        }
    }
}
