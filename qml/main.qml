import QtQuick 2.12
import QtQuick.Window 2.12

Window
{
    id: root
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    Component.onCompleted:
    {
        controller.requestNewConnection("127.0.0.1");
    }

    Image
    {
        id: image
        anchors.fill: parent
        sourceSize: Qt.size(parent.width, parent.height)
        cache: false

        property int frame: 0;

        Text
        {
            id: fpsText
            anchors.left: parent.left
            anchors.leftMargin: 50
            anchors.top: parent.top
            anchors.topMargin: 50
            font.pointSize: 20
            color: "red"
        }

        Timer
        {
            interval: 1000
            running: true
            repeat:true
            onTriggered:
            {
                fpsText.text = "FPS :" + image.frame;
                image.frame = 0;
            }
        }
    }

    Connections
    {
        target: controller
        onNeedUpdate:
        {
            image.frame++;
            image.source = "image://screen/" + Date.now();
        }
    }

    MouseArea
    {
        anchors.fill: parent
        onClicked:
        {
            let ratio = Qt.point(mouse.x / root.width, mouse.y / root.height);
            controller.mouseClicked(ratio);
        }
        onDoubleClicked:
        {
            let ratio = Qt.point(mouse.x / root.width, mouse.y / root.height);
            controller.mouseDBClicked(ratio);
        }
    }
}
