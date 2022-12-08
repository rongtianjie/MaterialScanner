import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQml.Models 2.12
import Ultimate.models 1.0

import "Base"


Item {

    property alias contentEnabled: contentLayout.enabled

    signal closed();

    Rectangle {
        anchors.fill: parent

        color: g_style.scanBgColor

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
        }
    }

    ColumnLayout {
        id: contentLayout
        anchors.fill: parent
        anchors.margins: 20

        TitleItem {
            Layout.preferredHeight: 240
            Layout.fillWidth: true
            title: qsTr("关键参数")

            titleComponent: RowLayout {
                spacing: 10

//                DButton {
//                    Layout.alignment: Qt.AlignCenter

//                    width: 30
//                    height: 30

//                    color: g_style.closeBtnColor
//                    display: AbstractButton.IconOnly

//                    icon.source: "qrc:/resources/Images/close.svg"

//                    onClicked: {
//                        closed();
//                    }
//                }

                DButton {
                    Layout.alignment: Qt.AlignCenter

                    width: 120
                    height: 30

                    radius: 3
                    font.pixelSize: g_style.reset2BtnFontSize
                    color: enabled ? (hovered | pressed ? g_style.reset2BtnHoverColor : g_style.reset2BtnColor ) : g_style.btnDisableBgColor
                    textColor: enabled ? (hovered | pressed ?  g_style.resetBtnHoverColor : g_style.resetBtnColor) : g_style.btnDisableColor
                    display: AbstractButton.TextBesideIcon

                    text: qsTr("重置参数")

                    icon.source: "qrc:/resources/Images/refresh.svg"

                    property QtObject msgBox: null
                    onClicked: {
                        msgBox = g_message.createObject(g_root_window);
                        msgBox.visibleChanged.connect(funcClose);
                        msgBox.confirmationed.connect(funcConfirmationed);
                        msgBox.title = qsTr("操作确认");
                        msgBox.contentTitle = qsTr("确认重置当前参数？");
                        msgBox.content = qsTr("相机参数将恢复为默认值。");
                        msgBox.open();
                    }

                    function funcClose() {
                        if(!msgBox.visible)
                        {
                            msgBox.destroy();
                            msgBox = null;
                        }
                    }

                    function funcConfirmationed() {
                        g_cameraOperation.restoreParameter();
                    }
                }
            }

            contentComponent: ColumnLayout {
                anchors.fill: parent
                anchors.margins: 0

                enabled: !g_softConfig.isRelease

                Column {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.margins: 20

                    spacing: 0

                    CameraPropertyEditItem {
                        id: shutterSpeedItem
                        text: qsTr("快门")

                        onPropertyChanged: {
                            g_cameraOperation.setShutterSpeed(comboBox.currentValue);
                        }
                    }

                    CameraPropertyEditItem {
                        id: apertureItem
                        text: qsTr("光圈")

                        onPropertyChanged: {
                            g_cameraOperation.setAperture(comboBox.currentValue);
                        }
                    }

                    CameraPropertyEditItem {
                        id: isoItem
                        text: qsTr("ISO")

                        onPropertyChanged: {
                            g_cameraOperation.setIso(comboBox.currentValue);
                        }
                    }

                    CameraIntPropertyEditItem {
                        id:  colorTempItem
                        text: qsTr("白平衡")

                        spinBox.from: 2500
                        spinBox.to: 10000
                        spinBox.stepSize: 100
                        spinBox.value: g_cameraOperation.colorTemperature

                        onPropertyChanged: {
                            g_cameraOperation.setColorTemperature(spinBox.value);
                        }
                    }

                }

                function updatePropertyInfo()
                {
                    g_cameraOperation.updateShutterSpeedModel(shutterSpeedItem.comboBox.model);
                    g_cameraOperation.updateApertureModel(apertureItem.comboBox.model);
                    g_cameraOperation.updateIsoModel(isoItem.comboBox.model);
                    colorTempItem.spinBox.value = g_cameraOperation.colorTemperature;
                }

                Component.onCompleted: {
                    updatePropertyInfo();
                }

                Connections{
                    target: g_devControl

                    function onUpdatePropertyInfo(){
                        updatePropertyInfo();
                    }
                }
            }
        }

        TitleItem {
            Layout.preferredHeight: 160
            Layout.fillWidth: true

            visible: !g_softConfig.isRelease
            title: qsTr("其它参数")

            contentComponent: ColumnLayout {

                anchors.fill: parent
                anchors.margins: 0

                Column {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.margins: 20

                    spacing: 0

                    CameraPropertyEditItem {
                        id: saveImageItem
                        text: qsTr("图像设置")

                        onPropertyChanged: {
                            g_cameraOperation.setMediaRecord(comboBox.currentValue);
                        }
                    }

//                    CameraPropertyEditItem {
//                        id: shutterTypeItem
//                        text: qsTr("快门模式")

//                        onPropertyChanged: {
//    //                        g_cameraOperation.setShutterSpeed(comboBox.currentValue);
//                        }
//                    }
                }

                function updatePropertyInfo()
                {
                    g_cameraOperation.updateMediaRecordModel(saveImageItem.comboBox.model);
                }

                Component.onCompleted: {
                    updatePropertyInfo();
                }

                Connections{
                    target: g_devControl

                    function onUpdatePropertyInfo(){
                        updatePropertyInfo();
                    }
                }

            }
        }

        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true

            visible: !g_softConfig.isRelease

            DButton {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                anchors.topMargin: 100

                width: 160
                height: 30

                font.pixelSize: 12
                color: enabled ? (hovered | pressed ? g_style.empBtnHoverBgColor : g_style.empBtnBgColor ) : g_style.btnDisableBgColor
                textColor: enabled ? (hovered | pressed ?  g_style.empBtnHoverColor : g_style.empBtnColor) : g_style.btnDisableColor

                text: qsTr("保存为默认参数")

                property QtObject msgBox: null
                onClicked: {
                    msgBox = g_message.createObject(g_root_window);
                    msgBox.visibleChanged.connect(funcClose);
                    msgBox.confirmationed.connect(funcConfirmationed);
                    msgBox.title = qsTr("操作确认");
                    msgBox.contentTitle = qsTr("确认保存当前参数为默认设置？");
                    msgBox.content = qsTr("每次重置操作都将恢复至该默认设置。");
                    msgBox.open();
                }

                function funcClose() {
                    if(!msgBox.visible)
                    {
                        msgBox.destroy();
                        msgBox = null;
                    }
                }

                function funcConfirmationed() {
                    g_cameraOperation.backupParameter();
                }
            }

        }

        Item {
            Layout.fillHeight: true
        }

    }

    component PropertyEdit: Item {
        width: parent.width
        height: 40

        property alias text : propertyText.text
        property alias contentComp : loader.sourceComponent

        signal propertyChanged();

        RowLayout {
            anchors.fill: parent

            Text {
                id: propertyText
                Layout.fillHeight: true
                Layout.minimumWidth: 100
                Layout.maximumWidth: 100

                verticalAlignment: Qt.AlignVCenter
                horizontalAlignment: Qt.AlignLeft
                color: g_style.defaultFontColor
                font.family: g_style.defaultFamily
                font.pixelSize: g_style.camSetTextFontSize
                text: modelData
            }

            Loader {
                Layout.fillHeight: true
                Layout.fillWidth: true

                id: loader
            }
        }
    }


}
