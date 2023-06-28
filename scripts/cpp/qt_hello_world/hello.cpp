#include <QApplication>
#include <QMenu>
#include <QSystemTrayIcon>

int
main(int argc, char **argv)
{
	qDebug() << "XXXXXXXXXXXXXXXXXXXXXXXX ...";
	QApplication app = QApplication(argc, argv);
	qDebug() << "XXXXXXXXXXXXXXXXXXXXXXXX ...";

	QSystemTrayIcon trayIcon = QSystemTrayIcon();

	QMenu trayMenu = QMenu();

	trayMenu.addAction("Exit", [&app] {
		qDebug() << "Exiting ...";
		app.exit();
	});

	trayIcon.setContextMenu(&trayMenu);

	trayIcon.setIcon(QIcon("tray_icon.png"));

	if (!trayIcon.isSystemTrayAvailable()) {
		trayIcon.setVisible(true);
		trayIcon.showMessage("Error!", "System tray not found.",
		    QSystemTrayIcon::Critical, 0);
		return 1;
	}

#if 0
	QObject::connect(&trayIcon, &QSystemTrayIcon::activated, &app,
	    [](QSystemTrayIcon::ActivationReason reason) {
		    switch (reason) {
		    case QSystemTrayIcon::Trigger:
			    qDebug() << "Trigger";
			    break;
		    case QSystemTrayIcon::Context:
			    qDebug() << "Context";
			    break;
		    case QSystemTrayIcon::DoubleClick:
			    qDebug() << "DoubleClick";
			    break;
		    case QSystemTrayIcon::MiddleClick:
			    qDebug() << "MiddleClick";
			    break;
		    default:
			    break;
		    }
	    });
#endif
	trayIcon.show();
	return app.exec();
}
