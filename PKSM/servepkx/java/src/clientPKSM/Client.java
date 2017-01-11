package clientPKSM;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;

public class Client {

	private Socket socket;
	private int port;
	private String host;

	public Client(String host){
		this.host=host;
		this.port=9000;
	}
	
	public Client(String host, int port) {
		this.port = port;
		this.host = host;
	}

	public void sendPKM(File PKM) {
		try {

			this.socket = new Socket(this.host, this.port);
			int count;
			byte[] buffer = new byte[8192];

			InputStream in = new FileInputStream(PKM);
			OutputStream out = socket.getOutputStream();
			while ((count = in.read(buffer)) > 0) {
				out.write(buffer, 0, count);
			}
			out.close();
			in.close();
			socket.close();

		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

	}
}
