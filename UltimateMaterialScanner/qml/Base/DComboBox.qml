import QtQuick 2.15
import QtQuick.Controls 2.15

ComboBox {
    id: root

    property color fontColor: "#999999";        // 字体颜色
    property color backgroundColor: "#ffffff";  // 背景色
    property color borderColor: "#409eff";      // 边框颜色
    property color disableBackgroundColor: "#E5E5E5";  // 无法编辑背景色
    property int radius : 3

    implicitWidth: implicitBackgroundWidth
    implicitHeight: implicitBackgroundHeight

    font.pixelSize: 12;
    font.family: "Microsoft YaHei"

    indicator: Canvas {
        id: canvas

        x: root.width - width - root.rightPadding - 5
        y: root.topPadding + (root.availableHeight - height) / 2
        width: 10
        height: 6

        contextType: "2d"

        Connections {
            target: root
            function onPressedChanged() {
                canvas.requestPaint();
            }
        }

        onPaint: {
//            var context = getContext("2d");
            context.reset();
            context.moveTo(0, 0);
            context.lineWidth = 2;
            context.lineTo(width / 2, height * 0.8);
            context.lineTo(width, 0);
            context.strokeStyle = root.pressed ? "#EEEFF7" : "#999999";
            context.stroke();
        }
    }


    background: Rectangle {
        implicitWidth: 120
        implicitHeight: 30
        radius: root.radius
        color: root.enabled ? root.backgroundColor : root.disableBackgroundColor
        border.color: root.down ? root.borderColor : root.disableBackgroundColor
        border.width: !root.editable && root.visualFocus ? 2 : 1
        visible: !root.flat || root.down
    }


}
