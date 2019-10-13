import QtQuick 2.12
import Qt.labs.platform 1.1

SystemTrayIcon {
    visible: true
    iconSource: "qrc:/image/winIcon.png"
    tooltip: qsTr("远程控制后台运行中...")

    function activeWindow() {
        mainWindow.show();
        mainWindow.raise();
        mainWindow.requestActivate();
    }

    onActivated: activeWindow();
}
