import QtQuick 2.0
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {

    property alias title: titleText.text
    property Component titleComponent
    property Component contentComponent

    implicitHeight: contentLayout.implicitHeight

    ColumnLayout {
        id: contentLayout

        anchors.fill: parent

        RowLayout {
//            Layout.preferredHeight: 30
            Layout.minimumHeight: 30
            Layout.maximumHeight: 30
            Layout.fillWidth: true

            Rectangle {
                Layout.preferredWidth: 6
                Layout.preferredHeight: 20
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

                color: "#0079fe"
            }

            Text {
                id: titleText
                Layout.fillWidth: true
                Layout.fillHeight: true

                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignLeft

                font.family: g_style.defaultFamily
                font.pixelSize: g_style.titleFontSize
                font.bold: true
                color: g_style.defaultFontColor

            }

            Loader {
//                Layout.fillWidth: true

                sourceComponent: titleComponent
            }

        }

        Loader {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.margins: 5

            sourceComponent: contentComponent
        }

    }



}
