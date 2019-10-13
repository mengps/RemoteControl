import QtQuick 2.12

Item {
    id: root

    Image {
        id: image
        anchors.fill: parent
        sourceSize: Qt.size(parent.width, parent.height)
        cache: false

        property int frame: 0;

        Text {
            id: fpsText
            anchors.left: parent.left
            anchors.leftMargin: 50
            anchors.top: parent.top
            anchors.topMargin: 20
            font.pointSize: 20
            color: "red"
        }

        Timer {
            interval: 1000
            running: true
            repeat:true
            onTriggered: {
                fpsText.text = "FPS :" + image.frame;
                image.frame = 0;
            }
        }
    }

    Connections {
        target: controller
        onNeedUpdate: {
            image.frame++;
            image.source = "image://screen/" + Date.now();
        }
    }

    MouseArea {
        id: controllerArea
        anchors.fill: parent
        onClicked: {
            let ratio = Qt.point(mouse.x / root.width, mouse.y / root.height);
            controller.mouseClicked(ratio);
        }
        onDoubleClicked: {
            let ratio = Qt.point(mouse.x / root.width, mouse.y / root.height);
            controller.mouseDBClicked(ratio);
        }
    }
}
