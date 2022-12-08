import QtQuick 2.12
import QtQuick.Controls 2.5


RadioButton {
    id: root

    width: 140
    height: 40

    property alias color: text.color
    property color iconColor : "#FFFFFF"
    property color iconSelectColor : "#409EFF"

    checkable: true
    font.bold: true
    font.pixelSize: 30
    font.family: "Microsoft YaHei"

    indicator: Rectangle {
        implicitWidth: root.height * 0.6
        implicitHeight: root.height * 0.6

        x: root.leftPadding
        y: (root.height - height) / 2
        radius: implicitWidth / 2
        border.color: root.checked ? iconSelectColor : iconColor

        Rectangle {
            anchors.centerIn: parent

            width: parent.width * 0.6
            height: parent.height * 0.6

            radius: width / 2
            color: root.checked ? iconSelectColor : iconColor
            visible: root.checked
        }
    }

    contentItem: Text {
        id: text

        leftPadding: root.indicator.width + root.spacing

        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.verticalAlignment

        text: root.text
        font: root.font

        color: "#f0f0f0"
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent

        cursorShape: parent.enabled ? Qt.PointingHandCursor : Qt.ForbiddenCursor
        acceptedButtons: Qt.NoButton
    }

}
