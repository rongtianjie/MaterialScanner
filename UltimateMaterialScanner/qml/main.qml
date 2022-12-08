import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.12
import QtQml 2.12

import "Base"


Window {
    id: g_root_window
    width: 1600
    height: 935
    minimumWidth: 1600
    minimumHeight: 800

    visible: true
    visibility: Window.Maximized
    title: qsTr("双目材质扫描仪控制系统")

    function buttonCliecked(clickedBtn) {
        var btnList = [camSetBtn, lightListBtn, analysisListBtn, softSetBtn];
        var index = -1;
        for(var i = 0; i < btnList.length; ++i)
        {
            if(btnList[i] === clickedBtn)
            {
                index = i;
                break;
            }
        }

        var itemList = [camSetItem, lightControl, scanResolutionList, versionInfoItem];
        var popItem = itemList[index];

        for(var j = 0; j < itemList.length; ++j)
        {
            var item = itemList[j];
            if(!popItem.opened && popItem !== item)
            {
                if(item.opened)
                {
                    item.opened = false;
                }
            }
        }

        popItem.opened = !popItem.opened;
    }

    StyleConfig {
        id: g_style
    }

    Image {
        id: image
        anchors.fill: parent
        source: "qrc:/resources/Images/background.png"
        asynchronous: true
        autoTransform: true
        clip: false
        mipmap: false
        fillMode: Image.PreserveAspectCrop
    }

    FastBlur {
        anchors.fill: image
        source: image
        radius: 32
    }

    ColumnLayout {
        anchors.fill: parent

        spacing: 0

        Rectangle {
            Layout.fillWidth: true
            Layout.minimumHeight: 50
            Layout.maximumHeight: 50

            color: g_style.titleBgColor

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 20
                anchors.rightMargin: 20

                Text {
                    Layout.preferredWidth: 50
                    Layout.preferredHeight: 50

                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter

                    font.family: g_style.defaultIconFamily
                    font.pixelSize: 24
                    color: g_style.defaultFontColor
                    text: ""
                }

                Text {
//                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    id: title

                    color: g_style.softTitleColor
                    text: qsTr("双目材质扫描仪控制系统")
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: g_style.softTitleFontSize
                    font.weight: Font.Bold
                    font.bold: true
                    font.family: g_style.defaultFamily
                }

                DButton {
                    Layout.leftMargin: 20
                    id: devStateBtn

                    width: 30
                    height: 30

                    display: AbstractButton.IconOnly

                    color: g_style.closeBtnColor
                    border.color: g_devControl.deviceState ? g_style.devConnectBgColor : g_style.devDisconnectBgColor
                    icon.source: g_devControl.deviceState ? "qrc:/resources/Images/connected.png" : "qrc:/resources/Images/disconnected.png"
                    radius: 25

                    property QtObject msgBox: null
                    onClicked: {
                        if (g_devControl.deviceState)
                        {
                            msgBox = g_message.createObject(g_root_window);
                            msgBox.visibleChanged.connect(funcClose);
                            msgBox.confirmationed.connect(funcConfirmationed);
                            msgBox.title = qsTr("操作确认");
                            msgBox.contentTitle = qsTr("确认断开与设备的连接？");
                            msgBox.content = qsTr("当前正在进行的任务将会中断，且无法恢复。是否确认断开与设备的连接？");
                            msgBox.open();
                        }
                        else
                        {
                            g_devControl.connectDevice();
                        }
                    }

                    function funcClose() {
                        if(!msgBox.visible)
                        {
                            msgBox.destroy();
                            msgBox = null;
                        }
                    }

                    function funcConfirmationed() {
                        g_devControl.disConnectDevice();
                    }
                }

                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }

                DButton {
                    id: camSetBtn
                    width: 150
                    height: 50

                    text: qsTr("相机参数设置")

                    checkable: true
                    checked: camSetItem.opened
                    radius: 0
                    font.bold: true
                    font.pixelSize: g_style.titleBtnFontSize
                    color: (checked || hovered ) ? g_style.titleBtnCheckedBgColor : g_style.titleBtnBgColor
                    textColor: (checked || hovered ) ? g_style.titleBtnCheckedColor : g_style.defaultFontColor
                    font.family: g_style.defaultFamily

                    onClicked: {
                        g_root_window.buttonCliecked(camSetBtn);
                    }
                }

                DButton {
                    id: lightListBtn
                    width: 150
                    height: 50

                    text: qsTr("光源控制")

                    checkable: true
                    checked: lightControl.opened
                    radius: 0
                    font.bold: true
                    font.pixelSize: g_style.titleBtnFontSize
                    color: (checked || hovered ) ? g_style.titleBtnCheckedBgColor : g_style.titleBtnBgColor
                    textColor: (checked || hovered ) ? g_style.titleBtnCheckedColor : g_style.defaultFontColor
                    font.family: g_style.defaultFamily

                    onClicked: {
                        g_root_window.buttonCliecked(lightListBtn);
                    }
                }

                DButton {
                    id: analysisListBtn
                    width: 150
                    height: 50
                    text: qsTr("解析管理")

                    checkable: true
                    checked: scanResolutionList.opened
                    radius: 0
                    font.bold: true
                    font.pixelSize: g_style.titleBtnFontSize
                    color: (checked || hovered ) ? g_style.titleBtnCheckedBgColor : g_style.titleBtnBgColor
                    textColor: (checked || hovered ) ? g_style.titleBtnCheckedColor : g_style.defaultFontColor

                    onClicked: {
                        g_root_window.buttonCliecked(analysisListBtn);
                    }
                }

                DButton {
                    id: softSetBtn
                    width: 50
                    height: 50
                    text: ""

                    checkable: true
                    checked: versionInfoItem.opened
                    radius: 0
                    font.bold: true
                    font.family: g_style.defaultIconFamily
                    font.pixelSize: g_style.titleBtnFontSize
                    color: (checked || hovered ) ? g_style.titleBtnCheckedBgColor : g_style.titleBtnBgColor
                    textColor: (checked || hovered ) ? g_style.titleBtnCheckedColor : g_style.defaultFontColor

                    onClicked: {
                        g_root_window.buttonCliecked(softSetBtn);
                    }
                }
            }
        }

        SettingItem {
            Layout.fillWidth: true
            Layout.fillHeight: true

            id: setringItem

            width: parent.width
            height: parent.height


            ScanResolutionList {
                id: scanResolutionList

                property alias opened: popupAnim.opened

                visible: false
                x: parent.width - width
                y: 10
                width: 600
                height: parent.height - 10

                PopupAnimation {
                    id: popupAnim

                    targetObj: parent
                }

                onClosed: {
                    opened = false;
                }
            }

            CameraSettingsItem {
                id: camSetItem

                property alias opened: camSetPopupAnim.opened

                visible: false
                contentEnabled: (!g_devControl.isBurnAndLap || g_devControl.isOnlyCtrlLight) && g_devControl.hasCamera
                x: parent.width - width
                y: 10
                width: 500
                height: parent.height

                PopupAnimation {
                    id: camSetPopupAnim

                    targetObj: parent
                }

                onClosed: {
                    opened = false;
                }
            }

            LightControlItem {
                id: lightControl

                property alias opened : lightPopupAnim.opened

                x: parent.width - width
                y: 10
                visible: false
                contentEnabled: (!(g_devControl.isBurnAndLap) || g_devControl.isOnlyCtrlLight)
                width: 900
                height: parent.height

                PopupAnimation {
                    id: lightPopupAnim

                    targetObj: parent
                }

                onOpenedChanged: {
                    if(!opened && g_devControl.isBurnAndLap && g_devControl.isOnlyCtrlLight)
                    {
                        g_devControl.endBurnAndLap();
                    }
                }

                onClosed: {
                    opened = false;
                }

            }

            Loader {
                id: versionInfoItem

                property alias opened : versionInfoPopupAnim.opened

                y: 10
                visible: false
                width: parent.width
                height: parent.height

                sourceComponent: versionInfoComp

                PopupAnimation {
                    id: versionInfoPopupAnim

                    targetObj: parent
                }
            }

        }
    }

//    // 加载中界面
//    Loader {
//        anchors.fill: parent

//        sourceComponent: calibration
//    }

    // 标定提示
    Connections{
        target: g_devControl

        property QtObject calibrationBox: null

        function onCalibrationStateChanged() {
            if(g_devControl.calibrationState)
            {
                calibrationBox = calibration.createObject(g_root_window);
                calibrationBox.visible = true;
            }
            else
            {
                calibrationBox.destroy();
                calibrationBox = null;
            }
        }

        property QtObject msgBox: null
        function onCalibrationFinished(ret) {
            msgBox = g_message.createObject(g_root_window);
            msgBox.visibleChanged.connect(funcClose);
            msgBox.title = qsTr("标定结果");
            msgBox.cancelButton.visible = false;
            if(ret)
            {
                msgBox.type = DMessageBox.Type.Information;
                msgBox.contentTitle = qsTr("标定完成，可以开始扫描。");
            }
            else
            {
                msgBox.type = DMessageBox.Type.Warning;
                msgBox.contentTitle = qsTr("标定失败，请确认相机是否连接或者标定板位置是否合适。");
            }

            msgBox.open();
        }

        function funcClose() {
            if(!msgBox.visible)
            {
                msgBox.destroy();
                msgBox = null;
            }
        }
    }

    Component {
        id: versionInfoComp

        Item {

            Rectangle {
                anchors.fill: parent
                color: g_style.lightBgColor

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                }
            }

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20

                TitleItem {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    title: qsTr("软件信息")

                    contentComponent: Item {

                        Text {
                            anchors.left: parent.left
                            anchors.top: parent.top
                            anchors.margins: 20

                            font.family: g_style.defaultFamily
                            font.pixelSize: 20
                            color: g_style.defaultFontColor
                            text: qsTr("当前版本: V") + g_devControl.getVersionInfo();
                        }
                    }
                }
            }
        }
    }

    // 全局提示窗口
    Component {
        id: g_message

        DMessageBox {
        }
    }

    Component {
        id: calibration

        Rectangle {
            anchors.fill: parent

            color: "#aa000000"

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
            }

            DBusyIndicator {
                id: loading
                anchors.centerIn: parent

                width: 100
                height: 100

                running: true
            }

            Text {
                anchors.top: loading.bottom
                anchors.topMargin: 20
                anchors.horizontalCenter: parent.horizontalCenter

                //width: 200
                //height: 60

                text: qsTr("正在标定中...")

                font.family: g_style.defaultFamily
                font.bold: true
                font.pixelSize: 40
                color: "white"
            }
        }
    }

}


