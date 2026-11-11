import java.io.*;
import java.net.*;
import java.nio.charset.StandardCharsets;

public class ServerJava{

    public static void main(String[] args){
        int puerto = 5000;
        try (ServerSocket servidor = new ServerSocket(puerto)){
            System.out.println("Servidor java");
            System.out.println("Que onda raza, andamos en el 5000");
            while(true){
                try(Socket cliente = servidor.accept();
                    BufferedReader entrada = new BufferedReader( new InputStreamReader(cliente.getInputStream(), StandardCharsets.UTF_8));
                    BufferedWriter salida = new BufferedWriter( new OutputStreamWriter(cliente.getOutputStream(), StandardCharsets.UTF_8))){
                    System.out.println("Cliente conectado: " + cliente.getInetAddress());
                    String mensaje;
                    while((mensaje = entrada.readLine()) != null){
                        System.out.println("Cliente C: " + mensaje);
                        salida.write("Recibido!");
                        salida.newLine();
                        salida.flush();
                    }
                    System.out.println("Cliente desconectado");
                }catch(IOException e){
                    System.err.println("Error con el cleinte: " + e.getMessage());
                }
            }
        }catch(IOException e){
            System.err.println("Error del servidor: " + e.getMessage());
        }
    }
}
