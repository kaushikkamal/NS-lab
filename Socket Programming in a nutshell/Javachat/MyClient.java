import java.io.*;  
import java.net.*;  
import java.util.Scanner;

class ServerReceiveThread extends Thread{

    DataInputStream dataInputStream;

    Socket socket = null;

    public ServerReceiveThread(Socket socket){
        this.socket = socket;
    }

    public void run(){
        try{
            while(true){
                dataInputStream = new DataInputStream(socket.getInputStream());
                System.out.println((String)dataInputStream.readUTF());
            }
        }catch(Exception e){
            System.out.println("Error ServerReceiveThread:");
            System.out.println(e.toString());
        }

    }

}

class ServerSendThread extends Thread{

    DataOutputStream dataOutputStream;

    Socket socket = null;
    String name = "";

    public ServerSendThread(Socket socket, String name){
        this.socket = socket;
        this.name = name;
    }

    public void run(){
        try{
            Scanner sc = new Scanner(System.in);
            String message = "";

            dataOutputStream = new DataOutputStream(socket.getOutputStream());
            dataOutputStream.writeUTF(name);
            while(true){
                message = name + " >> " + sc.nextLine();
                dataOutputStream.writeUTF(message);
            }
        }catch(Exception e){
            System.out.println("Error ServerSendThread:");
            System.out.println(e.toString());
        }

    }

}

public class MyClient {  
    public static void main(String[] args) {  
        try{
            Scanner sc = new Scanner(System.in);
            String name = "";

            System.out.print("Enter Your Name: ");
            name = sc.nextLine();

            Socket socket = new Socket("127.0.0.1",1234);  

            ServerSendThread send = new ServerSendThread(socket,name);
            send.start();

            ServerReceiveThread receive = new ServerReceiveThread(socket);
            receive.start();
        }catch(Exception e){
            System.out.println(e);
        }  
    }  
}  

