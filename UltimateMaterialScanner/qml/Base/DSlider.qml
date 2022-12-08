import QtQuick 2.15
import QtQuick.Controls 2.15
import QtGraphicalEffects 1.15


Slider {
    id: root

    property color backgroundColor: "#e4e4e4"
    property color handleColor: enabled ? "#409eff" : "#ababab"
    property color progressColor: enabled ? "#409eff" : "#ababab"
    property alias backgroundHeight: bgRect.height
    property alias handleHeight: hanldeRect.height

    implicitWidth: horizontal ? 200 : 24
    implicitHeight: horizontal ? 24 : 200

    handle: Rectangle {
        id: hanldeRect

        x: root.leftPadding + position * bgRect.width - width / 2
        y: (root.height - height) / 2
        width: bgRect.height * 2
        height: width

        scale: (mouseArea.containsMouse || root.pressed) ? 1.1 : 1
        color: handleColor
        radius: width / 2

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            hoverEnabled: true

            cursorShape: (containsMouse  || root.pressed) ? Qt.PointingHandCursor : Qt.ForbiddenCursor
            acceptedButtons: Qt.NoButton
        }

        Rectangle {
            anchors.centerIn: parent
            width: parent.width * 0.6
            height: width

            color: "#ffffff"
            radius: width / 2
        }
    }

    background: Rectangle {
        id: bgRect
        x: root.leftPadding
        y: (root.height - height) / 2
        width: root.width - root.leftPadding - root.rightPadding
        height: 10

        color: "#e4e4e4"
        radius: height / 2
        layer.enabled: true
        layer.smooth: true
        layer.effect: OpacityMask {
            maskSource: Rectangle {
                width: bgRect.width
                height: bgRect.height
                radius: bgRect.radius
            }
        }

        Rectangle {
            id: progressRect
            width: position * parent.width
            height: parent.height

            color: progressColor
        }
    }

}
