import QtQuick 2.12

Item
{
    id: root

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
                onClicked: mainWindow.hide();
            }
        }
    }
}
