import QtQuick 2.12

Item {
    id: root

    property bool isOpened: false
    property bool isControll: false
//    property alias isControll: checkBox.checked

    property int indexId: 0

    signal clicked()

    Image {
        anchors.fill: parent

        scale: mouseArea.containsMouse && !mouseArea.pressed ? 1.1 : 1

        mipmap: true

        source: isOpened ? "qrc:/resources/Images/light_on.svg" : "qrc:/resources/Images/light_off.svg"
        fillMode: Image.PreserveAspectFit

        MouseArea {
            id: mouseArea
            anchors.fill: parent

            hoverEnabled: true

            cursorShape: Qt.PointingHandCursor

            onClicked: root.clicked();
        }
    }

    Text {
        anchors.fill: parent

        color: "white"
        font.family: g_style.defaultFamily
        font.pixelSize: g_style.lightIndexFontSize
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignLeft
        text: root.indexId
    }

//    DCheckBox {
//        id: checkBox

//        width: 38
//        height: 38

//        anchors.right: parent.right
//        anchors.bottom: parent.bottom
//    }


}
