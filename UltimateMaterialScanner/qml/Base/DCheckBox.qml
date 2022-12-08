import QtQuick 2.12
import QtQuick.Controls 2.5


Item {
    id: root

    width: 140
    height: 40

    property alias color: text.color
    property alias text: checkBox.text
    property alias font: checkBox.font
    property alias checked: checkBox.checked
    property alias layoutDirection: content.layoutDirection

    property string checkedIcon: "qrc:/resources/Images/square_check.svg"
    property string uncheckedIcon: "qrc:/resources/Images/square.svg"

    signal clicked()
    signal toggled()

    AbstractButton {
        id: checkBox

        width: root.width
        height: root.height

        checkable: true
        font.bold: true
        font.pixelSize: 30
        font.family: "Microsoft YaHei"

        contentItem: Row {
            id: content

            Image {
                id: image

                mipmap: true

                width: parent.height
                height: parent.height
                source: checkBox.checked ? checkedIcon : uncheckedIcon
//                scale: checkBox.hovered && !checkBox.pressed ? 1.1 : 1
                scale: checkBox.pressed ? 0.9 : 1
            }

            Text {
                id: text

                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.verticalAlignment
                width: parent.width - parent.height
                height: parent.height

                text: checkBox.text
                font: checkBox.font

                color: "#f0f0f0"
            }
        }

        onClicked: root.clicked()
        onToggled: root.toggled()
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent

        cursorShape: parent.enabled ? Qt.PointingHandCursor : Qt.ForbiddenCursor
        acceptedButtons: Qt.NoButton
    }

}
