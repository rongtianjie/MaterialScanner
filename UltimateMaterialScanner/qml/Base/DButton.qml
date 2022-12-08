import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQml 2.12


AbstractButton {
    id: root

    width: 60
    height: 30

    property alias color : bg.color
    property alias border: bg.border
    property alias radius: bg.radius
//    property color textColor : g_style.defaultBtnColor
    property color textColor: enabled ? ("#ffffff") : "#d7d7d7"
    property int elide : Text.ElideMiddle

    implicitWidth: 0
    implicitHeight: 0


    icon.width: height * 2 / 3
    icon.height: height * 2 / 3

    scale: root.pressed ? 0.9 : 1

    spacing: 2

    font.bold: false
    font.pixelSize: 16
    font.family: "Microsoft YaHei"
    display: AbstractButton.TextOnly
    hoverEnabled: true

    background: Item {
        Rectangle {
            id: bg

            anchors.fill: parent
//            color: g_style.defaultBtnBgColor
//            border.color: g_style.btnBgColor;
            color: root.enabled ? (root.hovered | root.pressed ? "#cfcfcf" :"#007dff") : "#e0e0e0"

            radius: root.height / 10
        }
    }

    contentItem: Loader {
        id: contentLoader
        sourceComponent  : textComp
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent

        cursorShape: parent.enabled ? Qt.PointingHandCursor : Qt.ForbiddenCursor
        acceptedButtons: Qt.NoButton
    }

    Component {
        id: textComp

        Text {
            id: text

            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            anchors.centerIn: parent

            font: root.font
            text: root.text
            color: root.textColor
            elide: root.elide
         }
    }

    Component {
        id: iconComp

        Item {
            Image {
                id: text
                anchors.centerIn: parent

                width : root.icon.width
                height: root.icon.height
                source: root.icon.source

//                scale: root.pressed ? 0.9 : 1
             }
        }
    }

    Component {
        id: iconTextComp

        RowLayout {
            spacing: root.spacing

            Item {
                Layout.fillHeight: true
                Layout.fillWidth: true
            }

            Image {
                Layout.preferredWidth: root.icon.width
                Layout.preferredHeight: root.icon.height
                id: image

                width: root.icon.width
                height: root.icon.height
                mipmap: true
                source: root.icon.source
            }

            Text {
                Layout.fillHeight: true
                id: text

                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter

                font: root.font
                text: root.text
                color: root.textColor
                elide: root.elide
            }

            Item {
                Layout.fillHeight: true
                Layout.fillWidth: true
            }
        }
    }

    Component {
        id: textUnderComp

        ColumnLayout {
            spacing: root.spacing

//            Item {
//                Layout.fillHeight: true
//                Layout.fillWidth: true
//            }

            Image {
                Layout.preferredWidth: root.icon.width
                Layout.preferredHeight: root.icon.height
                Layout.alignment: Qt.AlignCenter

                id: image

                width: root.icon.width
                height: root.icon.height
                mipmap: true
                source: root.icon.source
            }

            Text {
                Layout.fillHeight: true
                id: text

                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter

                font: root.font
                text: root.text
                color: root.textColor
                elide: root.elide
            }

//            Item {
//                Layout.fillHeight: true
//                Layout.fillWidth: true
//            }
        }
    }

    onDisplayChanged: {
        switch(display)
        {
        case AbstractButton.TextOnly:
            contentLoader.sourceComponent = textComp;
            break;
        case AbstractButton.IconOnly:
            contentLoader.sourceComponent = iconComp;
            break;
        case AbstractButton.TextBesideIcon:
            contentLoader.sourceComponent = iconTextComp;
            break;
        case AbstractButton.TextUnderIcon:
            contentLoader.sourceComponent = textUnderComp;
            break;
        }
    }

}
