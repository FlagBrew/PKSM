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
	private File[] files;
	private String host;

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
		frmPksmClient.setBounds(100, 100, 486, 401);
		frmPksmClient.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frmPksmClient.getContentPane().setLayout(null);

		txtHost = new JTextField();
		txtHost.setBounds(150, 29, 191, 20);
		frmPksmClient.getContentPane().add(txtHost);
		txtHost.setColumns(10);

		JLabel lblIpAddress = new JLabel("IP Address:");
		lblIpAddress.setBounds(39, 32, 84, 14);
		frmPksmClient.getContentPane().add(lblIpAddress);

		JButton btnConfirm = new JButton("Confirm IP Address");
		btnConfirm.setBounds(150, 80, 191, 52);
		frmPksmClient.getContentPane().add(btnConfirm);
		btnConfirm.addActionListener(new ActionListener() {

			@Override
			public void actionPerformed(ActionEvent e) {

				if (txtHost.getText().isEmpty()) {
					JOptionPane.showMessageDialog(frmPksmClient, "You MUST enter a valid IP address!", "Error",
							JOptionPane.ERROR_MESSAGE);
				} else {
					host = txtHost.getText();
					JOptionPane.showMessageDialog(frmPksmClient,
							"The following IP Address is currently set\nHost: " + host);
				}
			}

		});

		JButton btnSelectFiles = new JButton("Select .pk6/.pk7 files");
		btnSelectFiles.setBounds(150, 159, 191, 52);
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
		btnSendFiles.setBounds(150, 240, 191, 52);
		frmPksmClient.getContentPane().add(btnSendFiles);
		
		JLabel lblPksmClientV = new JLabel("servepkx v1.1 - Made by Slownic. To use with PKSM, by Bernardo Giordano.");
		lblPksmClientV.setBounds(20, 338, 450, 14);
		frmPksmClient.getContentPane().add(lblPksmClientV);
		btnSendFiles.addActionListener(new ActionListener() {

			@Override
			public void actionPerformed(ActionEvent e) {

				Client client = new Client(host);
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
