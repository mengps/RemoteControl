import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import an.window 1.0

FramelessWindow {
    id: mainWindow

    visible: true
    width: mobile ? Screen.desktopAvailableWidth : 480
    height: mobile ? Screen.desktopAvailableHeight : 320
    minimumWidth: 480
    minimumHeight: 320
    title: qsTr("远程控制")
    color: "#D4E6FF"
    movable: !mobile
    resizable: !mobile
    Component.onCompleted: {
        stackView.push(mainPage);
        flags |= Qt.FramelessWindowHint;
    }

    property bool mobile: Qt.platform.os == "android";
    property bool connected: false;
    property string localIpAddress: Api.getLocalIpAddress();

    Connections {
        target: controlled
        onConnected: {
            mainWindow.connected = true;
            stackView.push("ControlledPage.qml");
        }
        onDisconnected: {
            mainWindow.connected = false;
            stackView.pop();
        }
    }

    Connections {
        target: controller
        onConnected: {
            mainWindow.connected = true;
            stackView.push("ControllerPage.qml");
        }
        onDisconnected: {
            mainWindow.connected = false;
            stackView.pop();
        }
    }

    GlowRectangle {
        id: windowTitle
        clip: true
        radius: 5
        z: 5
        height: mobile && connected ? 0 : 40;   //如果是手机，并且已经连接就不显示
        width: parent.width
        color: "#F4D1B4"
        glowColor: color
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: mobile && connected ? 0 : -radius

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 10
            text: qsTr("远程控制")
        }

        MyButton {
            id: minButton
            visible: enabled
            enabled: !mobile
            fontSize: 12
            widthMargin: 8
            anchors.right: maxButton.left
            anchors.rightMargin: 6
            anchors.top: maxButton.top
            text: " - "
            onClicked: mainWindow.showMinimized();
        }

        MyButton {
            id: maxButton
            visible: enabled
            enabled: !mobile
            fontSize: 12
            widthMargin: 8
            anchors.right: closeButton.left
            anchors.rightMargin: 6
            anchors.top: closeButton.top
            text: " + "
            onClicked: mainWindow.showMaximized();
        }

        MyButton {
            id: closeButton
            fontSize: 12
            widthMargin: 8
            anchors.right: parent.right
            anchors.top: parent.top
            text: " x "
            onClicked:
            {
                if (stackView.depth == 1)
                    mainWindow.close();
                else controller.finish();
            }
        }
    }

    StackView {
        id: stackView
        width: parent.width
        anchors.top: windowTitle.bottom
        anchors.bottom: parent.bottom
    }

    Component {
        id: mainPage

        Item {

            Column {
                anchors.centerIn: parent
                spacing: 32

                Item {
                    width: 300
                    height: 30

                    Text {
                        id: ipAddressText
                        anchors.centerIn: parent
                        font.pointSize: 13
                        color: "red"
                        text: qsTr("本机IP地址：" + localIpAddress)
                    }
                }

                Item {
                    width: 300
                    height: 30

                    Item {
                        width: remoteIpAddressText.width + remoteIpAddressEdit.width
                        height: 30
                        anchors.horizontalCenter: parent.horizontalCenter

                        Text {
                            id: remoteIpAddressText
                            font.pointSize: 13
                            anchors.verticalCenter: parent.verticalCenter
                            text: qsTr("远程IP地址：")
                        }

                        TextField {
                            id: remoteIpAddressEdit
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.left: remoteIpAddressText.right
                            anchors.rightMargin: 10
                            width: 150
                            height: parent.height
                            selectByMouse: true
                            placeholderText: qsTr("输入IPv4 / IPv6 地址")
                            background: Rectangle {
                                radius: 6
                                border.color: "#09A3DC"
                            }
                            validator: RegExpValidator {
                                //IP正则
                                regExp: /(2(5[0-5]|[0-4]\d))|[0-1]?\d{1,2}(\.((2(5[0-5]|[0-4]\d))|[0-1]?\d{1,2})){3}/
                            }
                        }
                    }
                }

                Item {
                    width: 300
                    height: 30

                    MyButton {
                        id: connectButton
                        anchors.horizontalCenter: parent.horizontalCenter
                        heightMargin: 8
                        text: qsTr(" <-连接-> ")
                        onClicked: controller.requestNewConnection(remoteIpAddressEdit.text)
                    }
                }
            }
        }
    }
}
