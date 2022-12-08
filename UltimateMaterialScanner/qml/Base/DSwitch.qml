import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQml 2.12


AbstractButton {
    id: root

    width: 70
    height: 30

    property color color : "#cccccc"
    property color openColor : "#409eff"

    property color handleColor : "#ffffff"
    property color handleOpenColor : "#ffffff"

    implicitWidth: 70
    implicitHeight: 30
    padding: 5

    checkable: true
    hoverEnabled: true

    background: Rectangle {
        id: bg

        anchors.fill: parent

        radius: root.height / 2
    }

    contentItem: Item {
        id: contentItem

        anchors.fill: parent
        anchors.margins: root.padding

        Rectangle {
            id: handle
            width: parent.height
            height: parent.height

            radius: height / 2
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent

        cursorShape: parent.enabled ? Qt.PointingHandCursor : Qt.ForbiddenCursor
        acceptedButtons: Qt.NoButton
    }

    state: "close"

    states: [
        State {
            name: "open"
            PropertyChanges {
                target: handle
                x: contentItem.width - handle.width
                color: root.handleOpenColor
            }
            PropertyChanges {
                target: bg
                color: root.openColor
            }
        },
        State {
            name: "close"
            PropertyChanges {
                target: handle
                x: 0
                color: root.handleColor
            }
            PropertyChanges {
                target: bg
                color: root.color
            }
        }
    ]

    transitions: [
        Transition {
            ColorAnimation {
                target: handle;
                properties: "color";
                duration: 300;
                easing.type: Easing.InOutQuad
            }
            ColorAnimation {
                target: bg;
                properties: "color";
                duration: 300;
                easing.type: Easing.InOutQuad
            }
            NumberAnimation {
                target: handle
                property: "x"
                duration: 300
                easing.type: Easing.InOutQuad
            }
        }
    ]

    onCheckedChanged: {
        root.state = root.checked ? "open" : "close"
    }
}

