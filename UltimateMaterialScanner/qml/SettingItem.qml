import QtQuick 2.15
import QtQuick.Layouts 1.12
import Qt.labs.platform 1.1
import QtQuick.Controls 2.5
import Ultimate.ui 1.0
import Ultimate.control 1.0

import "Base"


Item {
    id: root

    property bool opened : true

    visible: true

    Rectangle {
        id: leftRect

        width: 380

        anchors.left: parent.left
        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.bottom: parent.bottom

        color: g_style.settingBgColor

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
        }

        ScrollView {
            anchors.fill: parent

            Flickable {
                anchors.fill: parent
                anchors.margins: 20

                contentHeight: settingLayout.height
                clip: true

                ColumnLayout {
                    id: settingLayout

                    width: parent.width

                    spacing: 10

                    // 文件存储设置
                    TitleItem {
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignTop | Qt.AlignHCenter

                        title: qsTr("文件存储设置")

                        contentComponent: ColumnLayout {

                           ColumnLayout {
                               id: saveNameIem

                               Layout.fillWidth: true

                               width: parent.width
                               spacing: 0

                               Text{
                                   Layout.fillWidth: true
                                   Layout.preferredHeight: 40

                                   verticalAlignment: Text.AlignVCenter
                                   color: g_style.defaultFontColor

                                   text: qsTr("存储名称")
                                   font.family: g_style.defaultFamily
                                   font.pixelSize: g_style.settingFontSize
                               }

                               DTextInput {
                                   id: saveNameInput

                                   Layout.minimumWidth: 160
                                   Layout.fillWidth: true
                                   Layout.preferredHeight: 40

                                   rightPadding: 30
                                   color: g_style.defaultFontColor
                                   textInput.text: g_softConfig.scanSaveName
                                   validator: RegularExpressionValidator {
                                        regularExpression: /[^/\\:*?"<>|]+/
                                   }

                                   DButton {
                                       width: 20
                                       height: 20

                                       anchors.verticalCenter: parent.verticalCenter
                                       anchors.right: parent.right
                                       anchors.rightMargin: 10

                                       visible: saveNameInput.hovered | saveNameInput.focus | hovered
                                       font.family: g_style.defaultIconFamily
                                       font.pixelSize: 20
                                       color: g_style.closeBtnColor
                                       textColor: g_style.msgBtnColor
                                       text: ""

                                       onClicked: {
                                           saveNameInput.text = "";
                                           saveNameInput.activeFocusOnPress = true;
                                       }
                                   }


                                   Connections {
                                       function onEditingFinished() {
                                           g_softConfig.scanSaveName = saveNameInput.textInput.text;
                                           g_softConfig.writeFile();
                                       }
                                   }
                               }
                           }

                           ColumnLayout {
                                Layout.fillWidth: true

                                id: pathItem
                                spacing: 0

                                Text {
                                    Layout.fillWidth: true
                                    Layout.preferredHeight: 40

                                   horizontalAlignment: Text.AlignLeft
                                   verticalAlignment: Text.AlignVCenter

                                   color: g_style.defaultFontColor
                                   text: qsTr("存储路径")
                                   font.family: g_style.defaultFamily
                                   font.pixelSize: g_style.settingFontSize
                                }

                                RowLayout {
                                    Layout.fillWidth: true

                                    DTextInput {
                                        id: pathInput

                                        Layout.minimumWidth: 160
                                        Layout.fillWidth: true
                                        Layout.preferredHeight: 40

                                        readOnly: true
                                        color: g_style.defaultFontColor
                                        textInput.text: g_softConfig.scanSavePath
                                    }

                                    DButton {
                                        id: pathBtn

                                        Layout.preferredWidth: 40
                                        Layout.preferredHeight: 30

                                        color: enabled ? g_style.closeBtnColor : g_style.btnDisableBgColor
                                        font.pixelSize: g_style.selectBtnFontSize
                                        font.family: g_style.defaultIconFamily
                                        text: ""

                                        onClicked: {
                                            scanPathDolder.folder = "file:///" + g_softConfig.scanSavePath
                                            scanPathDolder.open()
                                        }

                                        FolderDialog {
                                            id: scanPathDolder
                                            onAccepted: {
                                                var text = currentFolder.toString()
                                                var index = text.indexOf("///")
                                                if(index > -1)
                                                {
                                                    g_softConfig.scanSavePath = text.slice(8)
                                                }
                                                else
                                                {
                                                    g_softConfig.scanSavePath = text.slice(5)
                                                }
                                                g_softConfig.writeFile();
                                            }
                                        }
                                    }

                                }
                           }
                        }

                    }

                    // 测试文件存储路径
                    TitleItem {
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignTop | Qt.AlignHCenter

                        title: qsTr("测试文件存储路径")

                        contentComponent: RowLayout {

                            DTextInput {
                                id: testPathInput

                                Layout.preferredHeight: 40
                                Layout.fillWidth: true

                                readOnly: true
                                color: g_style.defaultFontColor
                                textInput.text: g_softConfig.testSavePath
                            }

                            DButton {
                                id: selectTestPathBtn

                                Layout.preferredWidth: 40
                                Layout.preferredHeight: 30

                                color: enabled ? g_style.closeBtnColor : g_style.btnDisableBgColor
                                font.pixelSize: g_style.selectBtnFontSize
                                font.family: g_style.defaultIconFamily
                                text: ""

                                onClicked: {
                                    testPathDolder.currentFolder = "file:///" + g_softConfig.testSavePath;
                                    testPathDolder.open();
                                }

                                FolderDialog {
                                    id: testPathDolder

                                    onAccepted: {
                                        var text = currentFolder.toString()
                                        var index = text.indexOf("///")
                                        if(index > -1)
                                        {
                                            g_softConfig.testSavePath = text.slice(8)
                                        }
                                        else
                                        {
                                            g_softConfig.testSavePath = text.slice(5)
                                        }
                                        g_softConfig.writeFile();
                                    }
                                }
                            }
                        }
                    }

                    // 拍摄设置
                    TitleItem {
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignTop | Qt.AlignHCenter

                        title: qsTr("拍摄设置")

                        contentComponent: ShootSetComp{}
                    }

                    // 电机控制
                    TitleItem {
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignTop | Qt.AlignHCenter

                        visible: true
                        title: qsTr("电机控制")

                        contentComponent: emControlComp
                    }

                    TitleItem {
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignTop | Qt.AlignHCenter

                        title: qsTr("参数标定")
                        contentComponent: calibrationSettingComp
                    }

                    // 参数标定
//                    CalibrationSetting {
//                        Layout.fillWidth: true
//                        Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
//                    }

                }

            }
        }
    }

    // 中间
    Rectangle {
        anchors.left: leftRect.right
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        color: "#00000000"

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
        }

        Loader {
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.margins: 10

            width: 1200
            clip: true

            sourceComponent: editControlComp
        }

    }

    // 中间控制区域
    Component {
        id: editControlComp

        ColumnLayout {

            // 相机显示区
            RowLayout {
                Layout.fillWidth: true
                Layout.maximumHeight: 400

                id: cameraRow

                spacing: 10

                CameraLiveView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    id: leftCamera

                    title: qsTr("主相机(Mid)")
                }

                Item {
                    Layout.minimumWidth: 40

                    DButton {
                        width: 40
                        height: 40

                        anchors.centerIn: parent
                        color: g_style.closeBtnColor
                        display: AbstractButton.IconOnly
                        icon.source: "qrc:/resources/Images/exchange.svg"

                        onClicked: {
                            g_devControl.exchangedCamera();
                        }
                    }
                }

                CameraLiveView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    id: rightCamera

                    title: qsTr("右相机(Right)")
                }

                Connections {
                    target: g_devControl

                    function onLeftCameraChanged(){
                        leftCamera.cameraRender.cameraHolder = g_devControl.leftCamera;
                    }

                    function onRightCameraChanged(){
                        rightCamera.cameraRender.cameraHolder = g_devControl.rightCamera;
                    }
                }

                Component.onCompleted: {
                    leftCamera.cameraRender.cameraHolder = g_devControl.leftCamera;
                    rightCamera.cameraRender.cameraHolder = g_devControl.rightCamera;
                }
            }

            // 扫描控制区
            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true

                anchors.margins: 10

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 30

                    spacing: 20

                    RowLayout {
                        Layout.maximumHeight: 320
                        Layout.fillWidth: true

                        ColumnLayout {
                            Layout.fillWidth: true
                            Layout.fillHeight: true

                            spacing: 40

                            Item {
                                Layout.preferredHeight: 100

                                DRadioButton {
                                    id: normalRBtn
                                    anchors.left: parent.left
                                    anchors.top: parent.top

                                    height: 30

                                    //checked: true
                                    font.family: g_style.defaultFamily
                                    font.pixelSize: g_style.settingFontSize
                                    color: g_style.defaultFontColor
                                    checked: g_softConfig.scanMode === SoftConfig.NormalMode

                                    text: qsTr("常规模式")

                                    onClicked: {
                                        g_softConfig.scanMode = SoftConfig.NormalMode;
                                        g_softConfig.writeFile();
                                    }
                                }

                                Text {
                                    anchors.left: parent.left
                                    anchors.leftMargin: 30
                                    anchors.top: normalRBtn.bottom
                                    anchors.topMargin: 10

                                    height: 30
                                    wrapMode: Text.WordWrap
                                    font.family: g_style.defaultFamily
                                    font.pixelSize: g_style.scanModeTipSize
                                    color: g_style.scanModeTipColor

                                    verticalAlignment: Text.AlignVCenter
                                    horizontalAlignment: Text.AlignLeft

                                    text: qsTr("适用于常规材质的拍摄，如：布料、纸张等。")
                                }
                            }

                            Item {
                                Layout.preferredHeight: 100

                                DRadioButton {
                                    id: underLightRBtn
                                    anchors.left: parent.left
                                    anchors.top: parent.top

                                    height: 30
                                    font.family: g_style.defaultFamily
                                    font.pixelSize: g_style.settingFontSize
                                    color: g_style.defaultFontColor
                                    checked: g_softConfig.scanMode === SoftConfig.BottomLightMode

                                    text: qsTr("3S底光模式")

                                    onClicked: {
                                        g_softConfig.scanMode = SoftConfig.BottomLightMode;
                                        g_softConfig.writeFile();
                                    }
                                }

                                Text {
                                    anchors.left: parent.left
                                    anchors.leftMargin: 30
                                    anchors.top: underLightRBtn.bottom
                                    anchors.topMargin: 10

                                    height: 30
                                    wrapMode: Text.WordWrap
                                    font.family: g_style.defaultFamily
                                    font.pixelSize: g_style.scanModeTipSize
                                    color: g_style.scanModeTipColor

                                    verticalAlignment: Text.AlignVCenter
                                    horizontalAlignment: Text.AlignLeft

                                    text: qsTr("适用于透光、半透材质的拍摄，如：叶片、花卉等。")
                                }
                            }

                            ButtonGroup {
                                id: scanModeGroup
                                buttons: [normalRBtn, underLightRBtn]
                            }

                            Item {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                            }
                        }


                        Item {
                            Layout.fillWidth: true
                            Layout.fillHeight: true

                            DButton {
                                id: scanBtn

                                anchors.centerIn: parent

                                width: 280
                                height: 280

                                radius: 140

                                border.width: 2
                                border.color: g_style.scanBorderColor

                                font.bold: true
                                font.pixelSize: 28
                                color: enabled ? ( g_devControl.isBurnAndLap ? (hovered | pressed ? g_style.scanStartHoverBgColor : g_style.scanStartBgColor) : (hovered | pressed ? g_style.empBtnHoverBgColor : g_style.empBtnBgColor) ) : g_style.btnDisableBgColor
                                textColor: enabled ? (hovered | pressed ?  g_style.empBtnHoverColor : g_style.empBtnColor) : g_style.btnDisableColor
                                text: g_devControl.isBurnAndLap ? qsTr("扫描中...\n(%1张)").arg(g_devControl.loopIndex) : qsTr("开始扫描")

                                property QtObject msgBox: null

                                onClicked: {
                                    if(g_devControl.isBurnAndLap)
                                    {
                                        msgBox = g_message.createObject(g_root_window);
                                        msgBox.visibleChanged.connect(funcClose);
                                        msgBox.confirmationed.connect(funcConfirmationed);
                                        msgBox.title = qsTr("操作确认");
                                        msgBox.contentTitle = qsTr("确认终止本次拍摄？");
                                        msgBox.content = qsTr("当前正在进行的扫描任务将会中断，且无法恢复。");
                                        msgBox.open();
                                    }
                                    else
                                    {
                                        checkBatteryLevel();
                                    }
                                }

                                function checkBatteryLevel(){
                                    if(g_cameraOperation.checkBatteryLevel() == false)
                                    {
                                        msgBox = g_message.createObject(g_root_window);
                                        msgBox.visibleChanged.connect(funcClose);
                                        msgBox.confirmationed.connect(checkSavePath);
                                        msgBox.title = qsTr("操作确认");
                                        msgBox.contentTitle = qsTr("当前相机电量过低，是否继续拍摄？");
                                        msgBox.content = qsTr("继续拍摄可能会导致照片缺失，建议更换电池后再进行拍摄。");
                                        msgBox.contentColor = "red";
                                        msgBox.open();
                                    }
                                    else
                                    {
                                        checkSavePath();
                                    }
                                }

                                function checkSavePath() {
                                    if(g_cameraOperation.checkCameraPara() == false)
                                    {
                                        msgBox = g_message.createObject(g_root_window);
                                        msgBox.visibleChanged.connect(funcClose);
                                        msgBox.confirmationed.connect(funcStartScan);
                                        msgBox.cancelled.connect(funcRestroeParaScan);
                                        msgBox.cancelButton.text = qsTr("调整参数");
                                        msgBox.title = qsTr("提示");
                                        msgBox.contentTitle = qsTr("当前相机参数与默认值不符，是否继续拍摄？");
                                        msgBox.content = qsTr("部分参数和默认参数不同。");
                                        msgBox.open();
                                    }
                                    else
                                    {
                                        g_devControl.startBurnAndLap();
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
                                    g_devControl.endBurnAndLap();
                                }

                                function funcRestroeParaScan() {
                                    g_cameraOperation.restoreParameter();
                                    g_devControl.startBurnAndLap();
                                }

                                function funcStartScan() {
                                    g_devControl.startBurnAndLap();
                                }

                                Connections {
                                    target: g_devControl
                                    function onBurnAndLapChanged() {
                                        cameraBtn.enabled = !g_devControl.isBurnAndLap;
                                        allLightShootBtn.enabled = !g_devControl.isBurnAndLap;
                                        leftRect.enabled = !g_devControl.isBurnAndLap;
                                    }

                                    function onScanTipInfo(text) {
                                        scanTipText.showText(text);
                                    }
                                }
                            }

                        }
                    }


                    Item {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 40

                        Text {
                            id: scanTipText
                            visible: false;
                            anchors.fill: parent

                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter

                            font.pixelSize: 20
                            color: "red"

                            Timer {
                                id: scanTipTimer
                                interval: 2000
                                repeat: false

                                onTriggered: {
                                    scanTipText.visible = false;
                                }
                            }

                            function showText(text) {
                                scanTipText.text = text;
                                scanTipText.visible = true;
                                scanTipTimer.start();
                            }

                        }
                    }

                    RowLayout {
                        Layout.minimumHeight: 60
                        Layout.maximumHeight: 60
                        Layout.fillWidth: true

                        spacing: 50

                        DButton {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            id: cameraBtn
                            text: qsTr("相机单独拍摄一次")

                            font.pixelSize: g_style.btnFontSize
                            border.color: g_style.btnHoverBgColor;
                            color: enabled ? (hovered | pressed ? g_style.btnHoverBgColor : g_style.btnBgColor ) : g_style.btnDisableBgColor
                            textColor: enabled ? (hovered | pressed ?  g_style.btnHoverColor : g_style.btnColor) : g_style.btnDisableColor
                            radius: 10

                            onClicked: {
                                g_devControl.controlCamera();
                            }
                        }

                        DButton {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            id: allLightShootBtn
                            text: qsTr("灯光全亮拍摄一次")

                            font.pixelSize: g_style.btnFontSize
                            border.color: g_style.btnHoverBgColor;
                            color: enabled ? (hovered | pressed ? g_style.btnHoverBgColor : g_style.btnBgColor ) : g_style.btnDisableBgColor
                            textColor: enabled ? (hovered | pressed ?  g_style.btnHoverColor : g_style.btnColor) : g_style.btnDisableColor
                            radius: 10

                            onClicked: {
                                g_devControl.allLightshoot();
                            }
                        }

                        DButton {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            id: stopAllBtn
                            text: qsTr("全部中止&重置")

                            font.pixelSize: g_style.btnFontSize
                            border.color: g_style.stopBtnHoverBgColor;
                            color: enabled ? (hovered | pressed ? g_style.stopBtnHoverBgColor : g_style.stopBtnBgColor ) : g_style.btnDisableBgColor
                            textColor: enabled ? (hovered | pressed ?  g_style.stopBtnHoverColor : g_style.stopBtnColor) : g_style.btnDisableColor
                            radius: 10

                            property QtObject msgBox: null

                            onClicked: {
                                msgBox = g_message.createObject(g_root_window);
                                msgBox.visibleChanged.connect(funcClose);
                                msgBox.confirmationed.connect(funcConfirmationed);
                                msgBox.title = qsTr("操作确认");
                                msgBox.contentTitle = qsTr("确认终止本次拍摄？");
                                msgBox.content = qsTr("当前正在进行的扫描任务将会中断，且无法恢复。");
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
                                g_devControl.endBurnAndLap();
                                g_devControl.closeAllLight();
                            }
                        }
                    }

                    Item {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                    }
                }

                DButton {
                    anchors.top: parent.top
                    anchors.topMargin: 10
                    anchors.right: parent.right
                    anchors.rightMargin: 10

                    id: refrushBtn

                    width: 80
                    height: 30
                    display: DButton.TextBesideIcon
                    enabled: !g_devControl.isBurnAndLap || g_devControl.isOnlyCtrlLight

                    font.family: g_style.defaultFamily
                    font.pixelSize: 12
                    color: enabled ? (hovered | pressed ? g_style.refushBtnHoverBgColor : g_style.refushBtnBgColor ) : g_style.btnDisableBgColor
                    textColor: enabled ? g_style.refushBtnColor : g_style.btnDisableColor
                    border.color: enabled ? g_style.refushBtnColor : g_style.btnDisableColor
                    radius: 5
                    text: qsTr("刷新相机")
                    icon.source: "qrc:/resources/Images/refresh.svg";

                    onClicked: {
                        g_cameraController.asyncScan();
                    }
                }
            }

        }
    }

    // 相机设置组件
    component ShootSetComp: ColumnLayout {
        height: 300

        spacing: g_style.settingSpacing

        RowLayout {
            Layout.preferredHeight: 30
            Layout.preferredWidth: parent.width

            visible: !g_softConfig.isRelease

            Text {
                Layout.preferredWidth: 120
                Layout.fillWidth: true

                color: g_style.defaultFontColor
                font.family: g_style.defaultFamily
                font.pixelSize: g_style.settingFontSize
                text: qsTr("相机与灯光延迟时间X(ms)")
            }

            DSpinBox {
                id: camDelayTime

                Layout.preferredHeight: 30

                font.family: g_style.defaultFamily
                editable: true
                from: -6000
                to: 6000
                stepSize: 100
                value: g_softConfig.x

                onValueModified: {
                    g_softConfig.x = value;
                    g_softConfig.writeFile();
                }
            }
        }

        RowLayout {
            Layout.preferredHeight: 30
            Layout.preferredWidth: parent.width

            visible: !g_softConfig.isRelease

            Text {
                Layout.preferredWidth: 120
                Layout.fillWidth: true

                color: g_style.defaultFontColor
                font.family: g_style.defaultFamily
                font.pixelSize: g_style.settingFontSize
                text: qsTr("灯光关闭间隔时间Y(ms)")
            }

            DSpinBox {
                id: lightCloseDelayTime

                Layout.preferredHeight: 30

                font.family: g_style.defaultFamily
                editable: true
                from: 0
                to: 600000
                stepSize: 100
                value: g_softConfig.y

                onValueModified: {
                    g_softConfig.y = value;
                    g_softConfig.writeFile();
                }
            }
        }

        RowLayout {
            Layout.preferredHeight: 30
            Layout.preferredWidth: parent.width

            visible: !g_softConfig.isRelease

            Text {
                Layout.preferredWidth: 120
                Layout.fillWidth: true

                color: g_style.defaultFontColor
                font.family: g_style.defaultFamily
                font.pixelSize: g_style.settingFontSize
                text: qsTr("下一张相机启动间隔Z(ms)")
            }

            DSpinBox {
                id: nextDelayTime

                Layout.preferredHeight: 30

                font.family: g_style.defaultFamily
                editable: true
                from: 0
                to: 600000
                stepSize: 100
                value: g_softConfig.z

                onValueModified: {
                    g_softConfig.z = value;
                    g_softConfig.writeFile();
                }
            }
        }

        // 旋转角度
        RowLayout {
            Layout.preferredHeight: 30
            Layout.preferredWidth: parent.width

            visible: !g_softConfig.isRelease && g_softConfig.showInsideTestUi

            Text {
                Layout.preferredWidth: 120
                Layout.fillWidth: true

                color: g_style.defaultFontColor
                font.family: g_style.defaultFamily
                font.pixelSize: g_style.settingFontSize
                text: qsTr("每次旋转角度(°)")
            }

            DDoubleSpinBox {
                id: rotationAngleSbox

                Layout.preferredHeight: 30

                font.family: g_style.defaultFamily
                editable: true
                doubleFrom: 10
                doubleTo: 180
                doubleStepSize: 1
                doubleValue: g_devControl.getSingleRotationAngle()

                onDoubleValueChanged: {
                    g_devControl.setSingleRotationAngle(doubleValue);
                }
            }
        }

        RowLayout {
            Layout.preferredHeight: 30
            Layout.preferredWidth: parent.width

            visible: !g_softConfig.isRelease

            Text {
                Layout.preferredWidth: 120
                Layout.fillWidth: true

                color: g_style.defaultFontColor
                font.family: g_style.defaultFamily
                font.pixelSize: g_style.settingFontSize
                text: qsTr("一轮结束补拍一张")
            }

            DCheckBox {
                Layout.preferredHeight: 30
                Layout.preferredWidth: 30

                color: g_style.defaultFontColor
                font.pixelSize: g_style.settingFontSize
                layoutDirection: Qt.RightToLeft

                checked: g_softConfig.endShootOne;

                onClicked: {
                    g_softConfig.endShootOne = checked;
                    g_softConfig.writeFile();
                }
            }
        }

        // 对焦辅助灯
        RowLayout {
            Layout.preferredHeight: 30
            Layout.preferredWidth: parent.width

            Text {
                Layout.fillWidth: true

                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignLeft

                color: g_style.defaultFontColor
                font.family: g_style.defaultFamily
                font.pixelSize: g_style.settingTitleFontSize
                text: qsTr("辅助灯")
            }

            DSwitch {
                id: polarizationBtn

                onClicked: {
                    if (checked)
                    {
                        g_devControl.openAllLight();
                    }
                    else
                    {
                        g_devControl.closeAllLight();
                    }
                }
            }
        }

        // 扫描物体高度
        RowLayout {
            Layout.preferredHeight: 30
            Layout.preferredWidth: parent.width

            Text {
                Layout.fillWidth: true

                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignLeft

                color: g_style.defaultFontColor
                font.family: g_style.defaultFamily
                font.pixelSize: g_style.settingTitleFontSize
                text: qsTr("扫描物体高度")
            }

            DComboBox {
                id: heightComBox

                textRole: "text"
                valueRole: "value"
                model: ListModel {
                    ListElement { text: "0cm"; value: 0 }
                    ListElement { text: "0~1cm"; value: 1 }
                    ListElement { text: "1~2cm"; value: 2 }
                    ListElement { text: "2~3cm"; value: 3 }
                }

                currentIndex: g_softConfig.focusDisIndex

                onActivated: {
                    g_softConfig.focusDisIndex = index;
                    g_softConfig.writeFile();
                    g_cameraOperation.setFocusDistance(index);
                }

                onCurrentIndexChanged: {

                }
            }
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }



    // 电机控制组件
    Component {
        id: emControlComp

        ColumnLayout {

            spacing: g_style.settingSpacing

            // 旋转角度
            RowLayout {
                Layout.preferredHeight: 30
                Layout.preferredWidth: parent.width

                visible: !g_softConfig.isRelease

                Text {
                    Layout.preferredWidth: 120
                    Layout.fillWidth: true

                    color: g_style.defaultFontColor
                    font.family: g_style.defaultFamily
                    font.pixelSize: g_style.settingFontSize
                    text: qsTr("电机角度")
                }

                DSpinBox {
                    id: emAngleSpbo

                    Layout.preferredHeight: 30

                    font.family: g_style.defaultFamily
                    editable: true
                    from: 0
                    to: 180
                    stepSize: 10
                    value: 0

                    onEditingFinished: {
                        g_devControl.setEMRotationAngle(value);
                    }

                }
            }

            // 等级
            RowLayout {
                Layout.preferredHeight: 30
                Layout.preferredWidth: parent.width
                visible: g_softConfig.showInsideTestUi

                Text {
                    Layout.preferredWidth: 120
                    Layout.fillWidth: true

                    color: g_style.defaultFontColor
                    font.family: g_style.defaultFamily
                    font.pixelSize: g_style.settingFontSize
                    text: qsTr("等级")
                }

                DSpinBox {
                    id: emLevelSpbo

                    Layout.preferredHeight: 30

                    font.family: g_style.defaultFamily
                    editable: true
                    from: 1
                    to: 255
                    stepSize: 10
                    value: 50

                    onValueModified: {
                        g_devControl.setEMRotationSpeed(value);
                    }
                }
            }

            // 频率
            RowLayout {
                Layout.preferredHeight: 30
                Layout.preferredWidth: parent.width
                visible: g_softConfig.showInsideTestUi

                Text {
                    Layout.preferredWidth: 120
                    Layout.fillWidth: true

                    color: g_style.defaultFontColor
                    font.family: g_style.defaultFamily
                    font.pixelSize: g_style.settingFontSize
                    text: qsTr("频率")
                }

                Text {
                    id: emFrequencySpbo

                    Layout.preferredHeight: 30

                    property real frequency : 234375 / emLevelSpbo.value

                    color: g_style.defaultFontColor
                    font.family: g_style.defaultFamily
                    font.pixelSize: g_style.settingFontSize
                    text: g_devControl.numberToStr(emFrequencySpbo.frequency) + " hz"
                }
            }

            // 细分
            RowLayout {
                Layout.preferredHeight: 30
                Layout.preferredWidth: parent.width
                visible: g_softConfig.showInsideTestUi

                Text {
                    Layout.preferredWidth: 120
                    Layout.fillWidth: true

                    color: g_style.defaultFontColor
                    font.family: g_style.defaultFamily
                    font.pixelSize: g_style.settingFontSize
                    text: qsTr("细分")
                }

                DSpinBox {
                    id: emPartitionSpbo

                    //property real partition : 235312 / emLevelSpbo.value

                    Layout.preferredHeight: 30

                    font.family: g_style.defaultFamily
                    editable: true
                    from: 1
                    to: 100000
                    stepSize: 10
                    value: 5
                }
            }

            // 步进转速
            RowLayout {
                Layout.preferredHeight: 30
                Layout.preferredWidth: parent.width
                visible: g_softConfig.showInsideTestUi

                Text {
                    Layout.preferredWidth: 120
                    Layout.fillWidth: true

                    color: g_style.defaultFontColor
                    font.family: g_style.defaultFamily
                    font.pixelSize: g_style.settingFontSize
                    text: qsTr("步进转速")
                }

                Text {
                    id: emStepSpeedSpbo

                    Layout.preferredHeight: 30

                    property real stepSpeed : emFrequencySpbo.frequency/emPartitionSpbo.value/200*60

                    color: g_style.defaultFontColor
                    font.family: g_style.defaultFamily
                    font.pixelSize: g_style.settingFontSize
                    text: g_devControl.numberToStr(stepSpeed) + " rpm/min"
                }
            }

            // 减速比
            RowLayout {
                Layout.preferredHeight: 30
                Layout.preferredWidth: parent.width
                visible: g_softConfig.showInsideTestUi

                Text {
                    Layout.preferredWidth: 120
                    Layout.fillWidth: true

                    color: g_style.defaultFontColor
                    font.family: g_style.defaultFamily
                    font.pixelSize: g_style.settingFontSize
                    text: qsTr("减速比")
                }

                DSpinBox {
                    id: emReductionRatioSpbo

                    Layout.preferredHeight: 30

                    font.family: g_style.defaultFamily
                    editable: true
                    from: 1
                    to: 100000
                    stepSize: 10
                    value: 50
                }
            }

            // 输出转速
            RowLayout {
                Layout.preferredHeight: 30
                Layout.preferredWidth: parent.width
                visible: g_softConfig.showInsideTestUi

                Text {
                    Layout.preferredWidth: 120
                    Layout.fillWidth: true

                    color: g_style.defaultFontColor
                    font.family: g_style.defaultFamily
                    font.pixelSize: g_style.settingFontSize
                    text: qsTr("输出转速")
                }

                Text {
                    id: emOutSpeedSpbo

                    Layout.preferredHeight: 30

                    property real outSpeed : emStepSpeedSpbo.stepSpeed/emReductionRatioSpbo.value

                    color: g_style.defaultFontColor
                    font.family: g_style.defaultFamily
                    font.pixelSize: g_style.settingFontSize
                    text: g_devControl.numberToStr(outSpeed) + " rpm/min"
                }
            }

            DButton {
                Layout.fillWidth: true
                Layout.preferredHeight: 45

                id: emResetBtn
                radius: 10
                border.color: g_style.stopBtnHoverBgColor;
                color: enabled ? (hovered | pressed ? g_style.resetBtnHoverBgColor : g_style.resetBtnBgColor ) : g_style.btnDisableBgColor
                textColor: enabled ? (hovered | pressed ?  g_style.resetBtnHoverColor : g_style.resetBtnColor) : g_style.btnDisableColor
                font.bold: true
                font.pixelSize: g_style.resetBtnFontSize
                text: qsTr("重置光源位置")

                property QtObject msgBox: null

                onClicked: {
                    msgBox = g_message.createObject(g_root_window);
                    msgBox.visibleChanged.connect(funcClose);
                    msgBox.confirmationed.connect(funcConfirmationed);
                    msgBox.type = DMessageBox.Question
                    msgBox.title = qsTr("操作确认");
                    msgBox.contentTitle = qsTr("确认重置当前位置？");
                    msgBox.content = qsTr("将恢复至默认位置。");
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
                    g_devControl.resetEM();
                }

            }

        }
    }


    // 标定设置
    Component {
        id: calibrationSettingComp

        ColumnLayout {
            spacing: g_style.settingSpacing

            DButton {
                Layout.fillWidth: true
                Layout.preferredHeight: 45

                id: calibrationBtn
                radius: 10
                border.color: g_style.stopBtnHoverBgColor;
                color: enabled ? (hovered | pressed ? g_style.resetBtnHoverBgColor : g_style.resetBtnBgColor ) : g_style.btnDisableBgColor
                textColor: enabled ? (hovered | pressed ?  g_style.resetBtnHoverColor : g_style.resetBtnColor) : g_style.btnDisableColor
                font.bold: true
                font.pixelSize: g_style.resetBtnFontSize
                text: qsTr("一键标定")

                property QtObject msgBox: null

                onClicked: {
                    msgBox = g_message.createObject(g_root_window);
                    msgBox.visibleChanged.connect(funcClose);
                    msgBox.confirmationed.connect(funcConfirmationed);
                    msgBox.type = DMessageBox.Question
                    msgBox.title = qsTr("操作确认");
                    msgBox.contentTitle = qsTr("是否确定标定，标定时软件将无法操作？");
                    msgBox.content = qsTr("标定前请确认标定板放置合适。");
                    msgBox.contentColor = "red";
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
                    g_devControl.cameraCalibration();
                }
            }
        }
    }
}
