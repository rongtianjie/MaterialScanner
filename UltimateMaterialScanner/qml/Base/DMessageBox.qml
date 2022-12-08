import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtGraphicalEffects 1.15

Rectangle {
    id: root
    enum Type{ Question, Information, Warning  }

    property alias title: titleText.text
    property alias contentTitle: contentTitleText.text
    property alias content: contentText.text
    property alias cancelButton: cancelBtn
    property alias confirmationButton: confirmationBtn
    property color contentColor: "#999999"
    property int type: DMessageBox.Type.Question

    signal confirmationed;
    signal cancelled;

    onTypeChanged: {
        switch(root.type)
        {
        case DMessageBox.Type.Question:
            msgIconText.text = "";
            break;
        case DMessageBox.Type.Information:
            msgIconText.text = "";
            break;
        case DMessageBox.Type.Warning:
            msgIconText.text = "";
            break;
        }
    }

    anchors.fill: parent
    visible: false
    color: "#aa000000"

    SequentialAnimation {
        id: openAnim
        PropertyAction { target: root; property: "visible"; value: true }
        PropertyAnimation{ target: contentRectangle; properties: "scale,opacity"; easing.type: Easing.InOutQuad; from: 0; to: 1; duration: 300  }
    }

    SequentialAnimation {
        id: closeAnim
        PropertyAnimation{ target: contentRectangle; properties: "scale,opacity"; easing.type: Easing.InOutQuad; from: 1; to: 0; duration: 300  }
        PropertyAction { target: root; property: "visible"; value: false }
    }

    function open() {
        openAnim.start();
        closeAnim.stop();
    }

    function close() {
        closeAnim.start();
        openAnim.stop();
    }

//    onVisibleChanged: {
//        if(!visible)
//        {
//            root.destroy()
//        }
//    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
    }

    Rectangle {
        id: contentRectangle

        anchors.centerIn: parent

        width: 450
        height: 250

        clip: true

        layer.enabled: true
        layer.smooth: true
        layer.effect: OpacityMask {
            maskSource: Rectangle {
                width: contentRectangle.width
                height: contentRectangle.height
                radius: 8
            }
        }

        ColumnLayout {
            anchors.fill: parent
            clip: true
            spacing: 0

            // 标题栏
            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: 50

                Rectangle {
                    id: titleBg1
                    anchors.fill: parent
                    color: g_style.msgTitleBgColor
                }

                DButton {
                    id: closeBtn

                    width: 30
                    height: 30
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    anchors.rightMargin: 20

                    font.family: g_style.defaultIconFamily
                    font.pixelSize: 20
                    textColor: g_style.msgBtnColor
                    display: DButton.TextOnly
                    color: g_style.closeBtnColor
                    text: ""

                    onClicked: {
                        cancelled();
                        close();
                    }
                }

                Text {
                    anchors.left: parent.left
                    anchors.leftMargin: 20
                    anchors.top: parent.top
                    anchors.right: closeBtn.left
                    anchors.rightMargin: 10
                    anchors.bottom: parent.bottom

                    id: titleText
                    text: "Title"

                    font.family: g_style.defaultFamily
                    font.pixelSize: g_style.titleBtnFontSize
                    font.bold: true
                    color: g_style.msgTitleColor
                    wrapMode: Text.WrapAnywhere

                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.ALignLeft
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 2

                color: g_style.msgLineColor
            }

            RowLayout {
                Layout.leftMargin: 40
                Layout.rightMargin: 40
                Layout.topMargin: 30
                Layout.bottomMargin: 10
                Layout.fillWidth: true
                Layout.fillHeight: true

                Text {
                    id: msgIconText
                    Layout.preferredWidth: 28
                    Layout.preferredHeight: 28
                    Layout.topMargin: 6
                    Layout.alignment: Qt.AlignTop | Qt.AlignHCenter

                    width: 28
                    height: 28

                    font.family: g_style.defaultIconFamily
                    font.pixelSize: 28
                    color: g_style.msgIconColor
                    text: ""
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.margins: 10

                    Text {
                        Layout.fillWidth: true
                        id: contentTitleText
                        font.family: g_style.defaultFamily
                        font.pixelSize: g_style.titleFontSize
                        font.bold: true
                        color: g_style.msgTitleColor

                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.ALignLeft

                        text: ""
                        wrapMode: Text.WrapAnywhere
                    }

                    Text {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        id: contentText
                        font.family: g_style.defaultFamily
                        font.pixelSize: g_style.msgContentSize
                        color: root.contentColor

                        wrapMode: Text.WrapAnywhere
                        text: ""
                    }
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 2

                color: g_style.msgLineColor
            }

            RowLayout {
                Layout.preferredHeight: 60
                Layout.fillWidth: true
                Layout.rightMargin: 20

                spacing: 10

                Item {
                    Layout.fillWidth: true
                }

                DButton {
                    id: cancelBtn
                    width: 80
                    height: 30

                    font.pixelSize: g_style.msgBtnFontSize
                    border.color: enabled ? (hovered | pressed ? g_style.msgBtnHoverColor : g_style.msgLineColor ) : g_style.btnDisableBgColor
                    color: enabled ? (hovered | pressed ? g_style.msgBtnHoverBgColor : g_style.msgBtnBgColor ) : g_style.btnDisableBgColor
                    textColor: enabled ? (hovered | pressed ?  g_style.msgBtnHoverColor : g_style.msgBtnColor) : g_style.btnDisableBgColor
                    radius: 5

                    text: qsTr("取消")

                    onClicked: {
                        cancelled();
                        close();
                    }
                }

                DButton {
                    id: confirmationBtn
                    width: 80
                    height: 30

                    font.pixelSize: g_style.msgBtnFontSize
                    border.color: enabled ? g_style.msgBtnHoverBgColor : g_style.btnDisableBgColor
                    color: enabled ? (hovered | pressed ? g_style.msgConfirmBtnHoverBgColor : g_style.msgBtnHoverColor) : g_style.btnDisableBgColor
                    textColor: enabled ? g_style.msgBtnBgColor : g_style.btnDisableColor
                    radius: 5

                    text: qsTr("确认")

                    onClicked: {
                        confirmationed();
                        close();
                    }
                }
            }
        }

    }
}
