package clientPKSM;

import java.awt.EventQueue;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;

import javax.swing.JButton;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JTextField;

public class MainWindow {

	private JFrame frmPksmClient;
	private JTextField txtHost;
	private JTextField txtPort;
	private File[] files;
	private String host;
	private int port;

	/**
	 * Launch the application.
	 */
	public static void main(String[] args) {
		EventQueue.invokeLater(new Runnable() {
			public void run() {
				try {
					MainWindow window = new MainWindow();
					window.frmPksmClient.setVisible(true);
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		});
	}

	/**
	 * Create the application.
	 */
	public MainWindow() {
		initialize();
	}

	/**
	 * Initialize the contents of the frame.
	 */
	private void initialize() {
		frmPksmClient = new JFrame();
		frmPksmClient.setTitle("servepkx");
		frmPksmClient.setBounds(100, 100, 562, 401);
		frmPksmClient.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frmPksmClient.getContentPane().setLayout(null);

		txtHost = new JTextField();
		txtHost.setBounds(131, 29, 123, 20);
		frmPksmClient.getContentPane().add(txtHost);
		txtHost.setColumns(10);

		txtPort = new JTextField();
		txtPort.setBounds(369, 29, 123, 20);
		frmPksmClient.getContentPane().add(txtPort);
		txtPort.setColumns(10);

		JLabel lblIpAddress = new JLabel("IP Address:");
		lblIpAddress.setBounds(37, 32, 84, 14);
		frmPksmClient.getContentPane().add(lblIpAddress);

		JLabel lblPort = new JLabel("Port:");
		lblPort.setBounds(305, 32, 57, 14);
		frmPksmClient.getContentPane().add(lblPort);

		JButton btnConfirm = new JButton("Confirm Host and Port");
		btnConfirm.setBounds(184, 90, 191, 52);
		frmPksmClient.getContentPane().add(btnConfirm);
		btnConfirm.addActionListener(new ActionListener() {

			@Override
			public void actionPerformed(ActionEvent e) {

				if (txtHost.getText().isEmpty()) {
					JOptionPane.showMessageDialog(frmPksmClient, "You MUST enter a valid IP address!", "Error",
							JOptionPane.ERROR_MESSAGE);
				} else {
					host = txtHost.getText();
					if (txtPort.getText().isEmpty()) {
						port = 9000;
					} else {
						port = Integer.parseInt(txtPort.getText());
					}
					JOptionPane.showMessageDialog(frmPksmClient,
							"The following host and port are currently set\nHost: " + host + "\nPort: " + port);
				}
			}

		});

		JButton btnSelectFiles = new JButton("Select .pk6/.pk7 files");
		btnSelectFiles.setBounds(184, 170, 191, 52);
		frmPksmClient.getContentPane().add(btnSelectFiles);
		btnSelectFiles.addActionListener(new ActionListener() {

			@Override
			public void actionPerformed(ActionEvent e) {

				final JFileChooser fc = new JFileChooser();
				fc.setMultiSelectionEnabled(true);
				fc.setDragEnabled(true);
				fc.setFileSelectionMode(JFileChooser.FILES_ONLY);
				fc.showOpenDialog(frmPksmClient);

				files = fc.getSelectedFiles();

			}
		});

		JButton btnSendFiles = new JButton("Send .pk6/.pk7 files");
		btnSendFiles.setBounds(184, 250, 191, 52);
		frmPksmClient.getContentPane().add(btnSendFiles);
		
		JLabel lblPksmClientV = new JLabel("servepkx v1.0 - Made by Slownic. To use with PKSM, by Bernardo Giordano. 2017");
		lblPksmClientV.setBounds(39, 338, 497, 14);
		frmPksmClient.getContentPane().add(lblPksmClientV);
		btnSendFiles.addActionListener(new ActionListener() {

			@Override
			public void actionPerformed(ActionEvent e) {

				Client client = new Client(host, port);
				String result = "The following files have been sent:\n";

				for (int i = 0; i < files.length; i++) {
					client.sendPKM(files[i]);
					result = (result + "\n" + files[i].getAbsolutePath());
				}

				JOptionPane.showMessageDialog(frmPksmClient, result, "Files sent successfully!",
						JOptionPane.INFORMATION_MESSAGE);

			}
		});
	}

}
