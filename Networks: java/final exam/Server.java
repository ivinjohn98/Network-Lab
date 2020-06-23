// Java implementation of Server side
// It contains two classes : Server and ClientHandler
// Save file as Server.java

import java.io.*;
import java.util.*;
import java.net.*;

// Server class
public class Server
{

	// Vector to store active clients
	static Vector<ClientHandler> ar = new Vector<>();

	// counter for clients
	static int i = 0;

	public static void main(String[] args) throws IOException
	{
		Scanner sc = new Scanner(System.in);

		// server is listening on port 1234
		ServerSocket ss = new ServerSocket(1234);

		Socket s;

		// running infinite loop for getting
		// client request
		while (true)
		{
			// Accept the incoming request
			s = ss.accept();

			System.out.println("New client request received : " + s);

			// obtain input and output streams
			DataInputStream dis = new DataInputStream(s.getInputStream());
			DataOutputStream dos = new DataOutputStream(s.getOutputStream());

			System.out.println("Creating a new handler for this client...");

			// Create a new handler object for handling this request.
			ClientHandler mtch = new ClientHandler(s,"client " + i, dis, dos);

			// Create a new Thread with this object.
			Thread t = new Thread(mtch);

			System.out.println("Adding this client to active client list");

			// add this client to active clients list
			ar.add(mtch);

			// start the thread.
			t.start();

			// increment i for new client.
			// i is used for naming only, and can be replaced
			// by any naming scheme
			i++;

		}
	}
}

// ClientHandler class
class ClientHandler implements Runnable
{
	Scanner scn = new Scanner(System.in);
	private String name;
	final DataInputStream dis;
	final DataOutputStream dos;
	Socket s;
	boolean isloggedin;
	boolean welcome;

	// constructor
	public ClientHandler(Socket s, String name, DataInputStream dis, DataOutputStream dos) {
		this.dis = dis;
		this.dos = dos;
		this.name = name;
		this.s = s;
		this.isloggedin=true;
		this.welcome=true;

	}

	@Override
	public void run() {

		String received;
		while (true)
		{
			try
			{

				//for client joining a room,
				if(this.welcome){
					//client will get list of all other active clients
					this.dos.writeUTF("Welcome, "+"Client {"+this.name.substring(7)+"}");
					for (ClientHandler mc : Server.ar){
						if(mc.name != this.name)
						{
							mc.dos.writeUTF("Client {"+this.name.substring(7)+"} has entered the room");
						}
						if(mc.name != this.name)
						{
							this.dos.writeUTF(mc.name+":"+"is active");
						}
					}
				}
				this.welcome=false;

				// receive the string
				this.dos.writeUTF("Menu item 1");
				this.dos.writeUTF("Menu item 2");
				this.dos.writeUTF("Menu item 3");

				received = dis.readUTF();

				System.out.println(received);
				if(received.equals("3"))
				{
					received = dis.readUTF();
					if(received.equals("logout")){
						this.isloggedin=false;
						this.dos.writeUTF("Goodbye, Client {"+this.name.substring(7)+"}");
						for (ClientHandler mc : Server.ar){
							if(mc.name != this.name)
							{
								mc.dos.writeUTF("Client {"+this.name.substring(7)+"} has left the room");
							}
						}
						this.s.close();
						Server.ar.remove(this);
						break;
					}
				}
				else if(received.equals("2"))
				{
					received = dis.readUTF();
					StringTokenizer st = new StringTokenizer(received, "#");
					String MsgToSend = st.nextToken();
					String recipient = st.nextToken();

					// search for the recipient in the connected devices list.
					// ar is the vector storing client of active users
					for (ClientHandler mc : Server.ar)
					{
						// if the recipient is found, write on its
						// output stream
						if (mc.name.equals(recipient))
						{
							if(this.name.equals(recipient))
								break;
							mc.dos.writeUTF(this.name+" : "+MsgToSend);
							break;
						}
					}
				}
				else if(received.equals("1"))
				{
					for (ClientHandler mc : Server.ar)
					{
						// if the recipient is found, write on its
						// output stream
							this.dos.writeUTF(mc.name+":"+"is active");
					}
				}
				else
				{
					this.dos.writeUTF("invalid option");
				}

				// break the string into message and recipient part
				
			} catch (IOException e) {
				System.exit(0);
				e.printStackTrace();
			}

		}
		try
		{
			// closing resources
			this.dis.close();
			this.dos.close();

		}catch(IOException e){
			System.exit(0);
			e.printStackTrace();
		}
	}
}
