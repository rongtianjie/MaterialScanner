import QtQuick 2.11
import QtQuick.Controls 2.11
import QtGraphicalEffects 1.15

SpinBox {
    id: root

    property int radius : 3

    property color valueColor: enabled ? "#353535" : "#999999"  // 文本颜色
    property string borderColor: "#8B8B8B"              // 边框颜色
    property string selectdBorderColor: "#363636"       // 选中的边框颜色
//    property color indicatorColor: "#282828"            // 指标器颜色
//    property color indicatorBkColor: "#F0F0F0"          // 指标器背景颜色
//    property color indicatorPressBkColor: "#7E807F"     // 指标器按下背景颜色

    signal editingFinished

    implicitWidth: implicitBackgroundWidth
    implicitHeight: implicitBackgroundHeight

    font.pixelSize: 12;
    font.family: "Microsoft YaHei"

    layer.enabled: true
    layer.smooth: true
    layer.effect: OpacityMask {
        maskSource: Rectangle {
            width: root.width
            height: root.height
            radius: root.radius
        }
    }

    padding: 0

    contentItem: TextField {
        id: textInput

        padding: 0
        z: 2

        selectByMouse: true
        text: root.textFromValue(root.value, root.locale)
        font: root.font
        color: valueColor
        selectionColor: "#5456A1"
        selectedTextColor: "#ffffff"
        horizontalAlignment: Qt.AlignHCenter
        verticalAlignment: Qt.AlignVCenter

        readOnly: !root.editable
        validator: root.validator
        inputMethodHints: Qt.ImhFormattedNumbersOnly
        clip: true

//        placeholderText : hintText

//        background: Rectangle {
//            id: back
//            border.color: root.activeFocus ? selectdBorderColor : borderColor
//        }

        onEditingFinished: {
            var value  = root.valueFromText(text, root.locale);
            if(value !== root.value)
            {
                root.value = value;
                root.valueModified();
            }

            root.editingFinished();
        }
    }

    onFocusChanged: {
        if(focus)
        {
            textInput.selectAll();
        }
    }

}
