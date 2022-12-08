import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQml 2.12

import "Base"

Item {
    property alias contentEnabled: contentLayout.enabled

    signal closed();

    Rectangle {
        anchors.fill: parent

        color: g_style.lightBgColor

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
            Layout.fillWidth: true
            Layout.preferredHeight: 400

            title: qsTr("常规光源")

            contentComponent: lightControlComp
        }

        TitleItem {
            Layout.fillWidth: true
            Layout.preferredHeight: 180

            enabled: g_devControl.isConnectedStageLight
            title: qsTr("载台底光")
            contentComponent: Item {
                GridLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 20

                    columns: 2
                    rowSpacing: 20
                    columnSpacing: 10

                    Text {
                        Layout.preferredWidth: 80
                        Layout.preferredHeight: 40

                        font.family: g_style.defaultFamily
                        font.pixelSize: g_style.lightCobFontSize
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                        color: g_style.defaultFontColor
                        text: qsTr("亮度:")
                    }

                    DSlider {
                        id: eleSlider
                        Layout.fillWidth: true
                        value: g_devControl.stageLightElectricity
                        from: 0.0
                        to: 10.0
                        stepSize: 0.01

                        onMoved: {
                            g_devControl.stageLightElectricity = value;
                        }
                    }

                    Text {
                        Layout.preferredWidth: 80
                        Layout.preferredHeight: 40

                        font.family: g_style.defaultFamily
                        font.pixelSize: g_style.lightCobFontSize
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                        color: g_style.defaultFontColor
                        text: qsTr("开关:")
                    }

                    DSwitch {
                        checked: g_devControl.stageLightState

                        onClicked: {
                            g_devControl.stageLightState = checked;
                        }
                    }

                    Connections{
                        target: g_devControl

                        function onStageLightConnectStateChanged() {
                            eleSlider.value = g_devControl.stageLightElectricity
                        }
                    }
                }
            }
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }


    // 常规光源
    Component {
        id: lightControlComp

        Item {

            ColumnLayout {
                anchors.fill: parent
                anchors.topMargin: 30
                anchors.leftMargin: 80
                anchors.rightMargin: 80
//                anchors.bottomMargin: 10

                spacing: 50

                RowLayout {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 180
                    Layout.maximumHeight: 180
                    spacing: 50

                    ColumnLayout {
                        Layout.fillHeight: true
                        Layout.preferredWidth: 80

                        Repeater {
                            model: [qsTr("组1:"), qsTr("组2:")]

                            Text {
                                Layout.fillHeight: true
                                Layout.fillWidth: true

                                font.family: g_style.defaultFamily
                                font.pixelSize: g_style.lightCobFontSize
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignHCenter
                                color: g_style.defaultFontColor
                                text: modelData
                            }
                        }

                    }

                    GridLayout {
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        id: lightGrid

                        columns: 6
                        rowSpacing: 30

                        Repeater {
                            id: repeaterLight
                            model: 12

                            delegate: LightButton {
                                width: 80
                                height: 80
                                indexId: g_devControl.lightNumber(index)

                                onClicked: {
                                    g_devControl.controlLightFromIndex(index, !isOpened);
                                }
                            }
                        }

                        Connections {
                            target: g_devControl

                            function onLightStateChanged(id, state) {
                                var index = g_devControl.lightIdToIndex(id);
                                if(index > -1)
                                {
                                    var item = repeaterLight.itemAt(index);
                                    item.isOpened = state;
                                }
                            }
                        }
                    }

                    Item {
                        Layout.fillWidth: true
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
//                    Layout.fillHeight: true
                    Layout.alignment: Qt.AlignVCenter
                    Layout.leftMargin: 20
                    Layout.rightMargin: 20

                    spacing: 30
                    layoutDirection: Qt.RightToLeft

                    DButton {
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignVCenter

                        id: button1
                        height: 40
                        text: qsTr("一键全开")

                        font.pixelSize: g_style.btnFontSize
                        color: enabled ? (hovered | pressed ? g_style.empBtnHoverBgColor : g_style.empBtnBgColor ) : g_style.btnDisableBgColor
                        textColor: enabled ? (hovered | pressed ?  g_style.empBtnHoverColor : g_style.empBtnColor) : g_style.btnDisableColor

                        onClicked: {
                            g_devControl.openAllLight();
                        }

                        Connections{
                            target: g_devControl
                            function onAllLightOpened() {
                                for(var i = 0; i < repeaterLight.count; ++i)
                                {
                                    var item = repeaterLight.itemAt(i);
                                    item.isOpened = true;
                                }
                            }
                        }
                    }

                    DButton {
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter

                        id: button
                        height: 40
                        text: qsTr("一键全关")

                        font.pixelSize: g_style.btnFontSize
                        color: enabled ? (hovered | pressed ? g_style.btnHoverBgColor : g_style.btnBgColor ) : g_style.btnDisableBgColor
                        textColor: enabled ? (hovered | pressed ?  g_style.btnHoverColor : g_style.btnColor) : g_style.btnDisableColor

                        onClicked: {
                            g_devControl.closeAllLight();
                        }

                        Connections{
                            target: g_devControl
                            function onAllLightClosed() {
                                for(var i = 0; i < repeaterLight.count; ++i)
                                {
                                    var item = repeaterLight.itemAt(i);
                                    item.isOpened = false;
                                }
                            }
                        }
                    }

                    DButton {
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignVCenter

                        visible: !g_softConfig.isRelease

                        id: button2
                        height: 40
                        text: g_devControl.isBurnAndLap ? qsTr("结束跑圈") : qsTr("开始跑圈")

                        font.pixelSize: g_style.btnFontSize
                        color: enabled ? (hovered | pressed ? g_style.empBtnHoverBgColor : g_style.empBtnBgColor ) : g_style.btnDisableBgColor
                        textColor: enabled ? (hovered | pressed ?  g_style.empBtnHoverColor : g_style.empBtnColor) : g_style.btnDisableColor

                        onClicked: {
                            if(g_devControl.isBurnAndLap)
                            {
                                g_devControl.endBurnAndLap();
                            }
                            else
                            {
                                g_devControl.startBurnAndLap(true);
                            }
                        }

                    }

                }

                Item {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                }
            }
        }


    }


}
