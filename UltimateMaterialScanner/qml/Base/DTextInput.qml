import QtQuick 2.15

Rectangle {
    id: root

    property alias textInput: input
    property alias font: input.font
    property alias text: input.text
    property alias padding: input.padding
    property alias leftPadding: input.leftPadding
    property alias rightPadding: input.rightPadding
    property alias topPadding: input.topPadding
    property alias bottomPadding: input.bottomPadding
    property alias validator: input.validator
    property alias readOnly: input.readOnly
    property alias activeFocusOnPress: input.activeFocusOnPress
    property alias hovered: mouseArea.containsMouse


    signal editingFinished();

    implicitWidth: 100
    implicitHeight: 40

    radius: 5
    color: "#aaaaaa"

    MouseArea {
        id: mouseArea

        anchors.fill: parent

        hoverEnabled: true

    }

    TextInput{
        id: input

        anchors.fill: parent
        anchors.margins: 3

        verticalAlignment: TextInput.AlignVCenter
        horizontalAlignment: TextInput.AlignLeft

        font.family: "Microsoft YaHei"
        font.pixelSize: 14
        selectByMouse: true

        clip: true

        Connections {
            function onEditingFinished() {
                editingFinished();
            }
        }
    }
}
