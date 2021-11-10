import java.io.*;  
import java.net.*;
import java.util.*;

class ClientReceiveThread extends Thread{

	DataInputStream dataInputStream;
	DispatchMessage dispatchMessage;
	String clientID;
	String name;
	String message;

	Socket clientSocket = null;

	public ClientReceiveThread(Socket clientSocket, DispatchMessage dispatchMessage, String clientID, String name){
		this.clientSocket = clientSocket;
		this.dispatchMessage = dispatchMessage;
		this.clientID = clientID;
		this.name = name;
	}

	public void run(){
		try{
			while(true){
				dataInputStream = new DataInputStream(clientSocket.getInputStream());
				message = (String)dataInputStream.readUTF();
				System.out.println(message);
				dispatchMessage.sendMessageToAll(message, clientID, name);
			}
		}catch(Exception e){
			System.out.println("Error ClientReceiveThread:");
			System.out.println(e.toString());
		}

	}

}

class DispatchMessage{

	public void sendMessageToAll(String message, String clientID, String name){
		Socket clientSocket;
		DataOutputStream dataOutputStream;
		for(int i=0; i<(MyServer.clientList).size(); i++){
			clientSocket = MyServer.clientList.get(i);
			if(!String.valueOf(clientSocket.getPort()).equals(clientID)){
				try{
					dataOutputStream = new DataOutputStream(clientSocket.getOutputStream());
					dataOutputStream.writeUTF("\n" + message);
				}catch(Exception e){
					System.out.println("Error in DispatchMessage Class:");
					System.out.println(e.toString());
				}
			}
		}
	}
}

public class MyServer {  

	public static ArrayList<Socket>clientList = new ArrayList<Socket>();

	public static void main(String[] args){

		ServerSocket serverSocket;
	 	Socket clientSocket;
	 	DispatchMessage dispatchMessage;
	 	String clientID;
	 	String name;

		try{
			serverSocket = new ServerSocket(1234);
			dispatchMessage = new DispatchMessage();
			System.out.println("Server Started on port: 1234");
			while(true){
				clientSocket = serverSocket.accept();

				DataInputStream dataInputStream = new DataInputStream(clientSocket.getInputStream());
				name = (String)dataInputStream.readUTF();

				clientList.add(clientSocket);

				clientID = String.valueOf(clientSocket.getPort());
				ClientReceiveThread receive = new ClientReceiveThread(clientSocket, dispatchMessage, clientID, name);
				receive.start();
			}
		}catch(Exception e){
			System.out.println(e.toString());
		}
	}  
} 
