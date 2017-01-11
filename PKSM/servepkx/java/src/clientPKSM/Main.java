package clientPKSM;

public class Main {

	public static void main(String[] args) {

		if (args.length < 2) {
			System.err.println("WRONG COMMAND!");
			throw new IllegalArgumentException(
					"You must use this program with the following arguments:\n\nClientPKSM.exe [ip address] [.pkm file location 1] [.pkm file location 2] ...");
		}

		String host = args[0];
		Client client = new Client(host);
		if (args.length == 2) {
			client.sendPKM(args[1]);
		} else {
			for (int i = 1; i < args.length; i++) {
				client.sendPKM(args[i]);
			}
		}

	}
}
