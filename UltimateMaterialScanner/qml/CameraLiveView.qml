import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.15
import Ultimate.ui 1.0

Item {
//    implicitWidth: 600
//    implicitHeight: 480

    property alias title : titleText.text
    property alias cameraRender: cameraRenderItem

    Rectangle {
        anchors.fill: parent

        color: g_style.settingBgColor
    }

    ColumnLayout {
        anchors.fill: parent

        spacing: 0

        RowLayout {
            Layout.fillWidth: true
            Layout.minimumHeight: 40
            Layout.maximumHeight: 40
            Layout.leftMargin: 10
            Layout.rightMargin: 10

            Text {
                Layout.fillWidth: true
                Layout.fillHeight: true
                id: titleText

                font.family: g_style.defaultFamily
                font.pixelSize: g_style.cameraTitleFontSize
                color: g_style.defaultFontColor

                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignLeft
            }

            BatteryItem {
                width: 50
                Layout.fillHeight: true

                batteryValue: cameraRenderItem.batteryValue
                isCharging: cameraRenderItem.isCharging
                visible: cameraRenderItem.hasCamera
            }
        }

        CameraRenderItem {
            Layout.fillWidth: true
            Layout.fillHeight: true

            id: cameraRenderItem

            isShowCameraImage: true

//            DButton {
//                id: shootBtn

//                width: 30
//                height: 30

//                visible: cameraRenderItem.hasCamera

//                anchors.verticalCenter: parent.verticalCenter
//                anchors.right: parent.right
//                anchors.rightMargin: 20

//                display: AbstractButton.IconOnly

//                color: g_style.shootBgColor
//                radius: 15

//                onClicked: {
//                    cameraRenderItem.shootOne();
//                }
//            }
        }
    }

}
