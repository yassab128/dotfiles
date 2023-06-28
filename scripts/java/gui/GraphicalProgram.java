import java.awt.*;
import java.awt.TrayIcon.MessageType;
import java.awt.event.*;
import javax.swing.JOptionPane;

public class GraphicalProgram {
	public static void main(String[] args) {
		final SystemTray systemTray = SystemTray.getSystemTray();
		final Image image =
		    Toolkit.getDefaultToolkit().getImage("tray_icon.png");

		final PopupMenu trayPopupMenu = new PopupMenu();

		final MenuItem action = new MenuItem("Action");
		action.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				JOptionPane.showMessageDialog(null,
							      "Action Clicked");
			}
		});
		trayPopupMenu.add(action);

		final MenuItem close = new MenuItem("Close");
		close.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				System.out.println("Exiting ...");
				System.exit(0);
			}
		});
		trayPopupMenu.add(close);

		final TrayIcon trayIcon =
		    new TrayIcon(image, "SystemTrayDemo", trayPopupMenu);

		trayIcon.setImageAutoSize(true);
		final MouseListener mouseListener = new MouseListener() {
			public void mouseClicked(MouseEvent e) {
				System.out.println(
				    "Tray Icon - Mouse clicked!");
			}

			public void mouseEntered(MouseEvent e) {
				System.out.println(
				    "Tray Icon - Mouse entered!");
			}

			public void mouseExited(MouseEvent e) {
				System.out.println("Tray Icon - Mouse exited!");
			}

			public void mousePressed(MouseEvent e) {
				trayIcon.displayMessage(null, "huh",
							MessageType.NONE);
				System.out.println(
				    "Tray Icon - Mouse pressed!");
			}

			public void mouseReleased(MouseEvent e) {
				trayIcon.displayMessage("", "", null);
				System.out.println(
				    "Tray Icon - Mouse released!");
			}
		};
		trayIcon.addMouseListener(mouseListener);

		try {
			systemTray.add(trayIcon);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
}
