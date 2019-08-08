import QtQuick 2.12
import Qt.labs.platform 1.1

Item
{
    id: root

    property var systemTray: undefined;

    Component.onCompleted:
    {
        var component = Qt.createComponent("MySystemTray.qml");
        var incubator = component.incubateObject(root);
        if (incubator.status !== Component.Ready)
        {
            incubator.onStatusChanged = function(status) {
                if (status === Component.Ready)
                    root.systemTray = incubator.object;
            }
        }
    }

    Column
    {
        anchors.centerIn: parent
        spacing: 32

        Item
        {
            width: 300
            height: 30

            Text
            {
                anchors.centerIn: parent
                font.pointSize: 13
                color: "red"
                text: qsTr("正在被远程控制")
            }
        }

        Item
        {
            width: 300
            height: 30

            MyButton
            {
                id: disconnectButton
                anchors.centerIn: parent
                heightMargin: 8
                text: qsTr(" <-断开连接-> ")
                onClicked: controlled.finish();
            }
        }

        Item
        {
            width: 300
            height: 30

            MyButton
            {
                id: hidetButton
                anchors.centerIn: parent
                heightMargin: 8
                text: qsTr(" <-最小化托盘-> ")
                onClicked:
                {
                    mainWindow.hide();
                    if (root.systemTray != undefined)
                        root.systemTray.showMessage(qsTr("提示"), qsTr("已最小化到托盘！"));
                }
            }
        }
    }
}
