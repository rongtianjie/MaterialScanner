import QtQuick 2.15
import QtQuick.Layouts 1.12
import QtQml 2.12
import QtQuick.Controls 2.12
import QtQml.Models 2.12
import Qt.labs.qmlmodels 1.0
import Ultimate.models 1.0

import "Base"

Item {

    signal closed();

    Rectangle {
        anchors.fill: parent

        color: g_style.scanBgColor

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true;
        }
    }

    TitleItem {
        anchors.fill: parent
        anchors.margins: 20

        title: qsTr("解析队列")

        contentComponent: ColumnLayout {
            spacing: 0

            Item {
                id: listItem
                Layout.fillWidth: true
                Layout.preferredHeight: 30

                Row {
                    anchors.fill: parent

                    Repeater {
                        model: [qsTr("文件名称"),qsTr("数量"),qsTr("开始扫描时间"),qsTr("操作"),qsTr("状态") ]

                        Text {
                            width: {
                                var w = 0;
                                switch(index){
                                    case 0: w = listItem.width * 86/380; break;
                                    case 1: w = listItem.width * 56/380; break;
                                    case 2: w = listItem.width * 86/380; break;
                                    case 3: w = listItem.width * 56/380; break;
                                    case 4: w = listItem.width * 96/380; break;
                                }
                            }

                            height: parent.height

                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter
                            font.family: g_style.defaultFamily
                            font.pixelSize: g_style.textFontSize
                            color: g_style.defaultFontColor
                            text: modelData
                        }
                    }
                }

                Rectangle {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom

                    height: 2
                    color: g_style.lineColor
                }
            }

            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true

                ScrollView {
                    anchors.fill: parent
                    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
                    ScrollBar.vertical.interactive: true

                    TableView {
                        id: tabView

    //                    boundsBehavior: Flickable.OvershootBounds
                        flickableDirection: Flickable.VerticalFlick
                        ScrollBar.vertical: ScrollBar { }

                        columnWidthProvider: function (column) {
                            var w = 100;
                            switch(column){
                                case 0: w = width * 86/380; break;
                                case 1: w = width * 56/380; break;
                                case 2: w = width * 86/380; break;
                                case 3: w = width * 56/380; break;
                                case 4: w = width * 96/380; break;
                            }

                            return w;
                        }

                        columnSpacing: 1
                        rowSpacing: 1
                        clip: true

                        model: ScanFileModel {
                            id: scanFileModel
                            scanPath: g_softConfig.scanSavePath
                        }

                        delegate: DelegateChooser {

                            DelegateChoice {
                                column: 0
                                delegate: textComp
                            }

                            DelegateChoice {
                                column: 1
                                delegate: textComp
                            }

                            DelegateChoice {
                                column: 2
                                delegate: textComp
                            }

                            DelegateChoice {
                                column: 3
                                delegate: Item {
                                    DButton {
                                        anchors.fill: parent
                                        anchors.margins: 6

                                        font.pixelSize: g_style.textFontSize
                                        color: "#00000000"
                                        textColor: g_style.parseLookColor
                                        text: qsTr("查看")

                                        onClicked:{
                                            //console.log("state: " + runState + " fileName: " + fileName);
                                            Qt.openUrlExternally("file:///" + filePath);
                                        }
                                    }
                                }
                            }

                            DelegateChoice {
                                column: 4
                                delegate: OperatorDelegate{}
                            }
                        }
                    }
                }
            }
        }

    }

    Component {
        id: textComp

        Item {
            implicitHeight: 40

            Text {
                anchors.fill: parent

                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                font.family: g_style.defaultFamily
                font.pixelSize: g_style.textFontSize
                color: column == 1 ? (runState == 5 ? g_style.parseWarnColor : g_style.defaultFontColor) : g_style.defaultFontColor
                text: display
                wrapMode: Text.WordWrap

                elide: Text.ElideMiddle

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true

                    ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
                    ToolTip.timeout: 5000
                    ToolTip.visible: containsMouse
                    ToolTip.text: display
                }

            }
        }
    }

    component OperatorDelegate: Item {

        RowLayout {
            anchors.fill: parent
            anchors.margins: 5

            Text {
                Layout.fillWidth: true
                Layout.fillHeight: true

                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                font.family: g_style.defaultFamily
                font.pixelSize: g_style.textFontSize
                text: {
                    switch(runState)
                    {
                    case ScanFileInfo.Transferring:
                        return qsTr("传输中");
                    case ScanFileInfo.Analysising:
                        return qsTr("解析中…");
                    case ScanFileInfo.AnalysisFinished:
                        return qsTr("解析完成");
                    case ScanFileInfo.AnalysisFailed:
                        return qsTr("失败");
                    case ScanFileInfo.NumberError:
                        return qsTr("张数不符");

                    default:
                        return "";
                    }
                }
                color: {
                    switch(runState)
                    {
                    case ScanFileInfo.Analysising:
                        return "#ffff00";
                    case ScanFileInfo.AnalysisFailed:
                    case ScanFileInfo.NumberError:
                        return g_style.parseWarnColor;
                    default:
                        return "#ffffff";
                    }
                }

                wrapMode: Text.WordWrap

                visible: {
                    switch(runState)
                    {
                    case ScanFileInfo.Normal:
                        return false;
                    default:
                        return true;
                    }
                }
            }

            DButton {
                Layout.fillWidth: true
                Layout.fillHeight: true

                font.pixelSize: g_style.textFontSize
                font.bold: false
                textColor:{
                    switch(runState)
                    {
                    case ScanFileInfo.Normal:
                    case ScanFileInfo.AnalysisFailed:
                        return g_style.parseStartColor;
                    default:
                        return g_style.parseWarnColor;
                    }
                }

                color: "#00000000"
                text: {
                    switch(runState)
                    {
                    case ScanFileInfo.Normal:
                        return qsTr("开始解析");
                    case ScanFileInfo.Analysising:
                        return qsTr("取消");
                    case ScanFileInfo.AnalysisFailed:
                        return qsTr("开始");
                    default:
                        return qsTr("");
                    }
                }

                visible: {
                    switch(runState)
                    {
                    case ScanFileInfo.Transferring:
                    case ScanFileInfo.AnalysisFinished:
                    case ScanFileInfo.NumberError:
                        return false;
                    default:
                        return true;
                    }
                }

                property QtObject msgBox: null

                onClicked: {
                    console.info("ScanFileInfo.Normal=" + ScanFileInfo.Normal);
                    if(runState == ScanFileInfo.Normal || runState == ScanFileInfo.AnalysisFailed)
                    {
                        tabView.model.analysisImage(row);
                    }
                    else if(runState == ScanFileInfo.Analysising)
                    {
                        msgBox = g_message.createObject(g_root_window);
                        msgBox.visibleChanged.connect(funcClose);
                        msgBox.confirmationed.connect(funcConfirmationed);
                        msgBox.title = qsTr("操作确认");
                        msgBox.contentTitle = qsTr("确认停止当前解析任务？");
                        msgBox.content = qsTr("停止后无法恢复，只能重新解析。");
                        msgBox.open();
                    }
                }

                function funcClose() {
                    if(msgBox.visible === false)
                    {
                        msgBox.destroy();
                        msgBox = null;
                    }
                }

                function funcConfirmationed() {
                    tabView.model.cancelAnalysisImage(row);
                }
            }

            // 删除
            DButton {
                width: 30
                height: 30

                display: DButton.IconOnly
                icon.source: "qrc:/resources/Images/delete.svg"

                color: enabled ? (hovered | pressed ? "#cfcfcf" :"#00007dff") : "#e0e0e0"
                radius: 15

                property QtObject msgBox: null

                onClicked: {
                    if(runState != ScanFileInfo.Analysising && runState != ScanFileInfo.Transferring)
                    {
                        msgBox = g_message.createObject(g_root_window);
                        msgBox.visibleChanged.connect(funcClose);
                        msgBox.confirmationed.connect(funcConfirmationed);
                        msgBox.title = qsTr("操作确认");
                        msgBox.contentTitle = qsTr("确认删除这组图片吗？");
                        msgBox.content = qsTr("删除后无法恢复，请确认后删除。");
                        msgBox.open();
                    }
                }

                function funcClose() {
                    if(msgBox.visible === false)
                    {
                        msgBox.destroy();
                        msgBox = null;
                    }
                }

                function funcConfirmationed(){
                    tabView.model.removeImage(row);
                }

            }

        }


    }




}
