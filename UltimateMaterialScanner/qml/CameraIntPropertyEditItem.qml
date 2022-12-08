import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import Ultimate.models 1.0

import "Base"

Item {
    width: parent.width
    height: 40

    property alias text : propertyText.text
    property alias spinBox : spinBox

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

        DSpinBox {
            id: spinBox
            Layout.fillWidth: true
            Layout.preferredHeight: 30

            editable: true
            stepSize: 100

            font.family: g_style.defaultFamily
            font.pixelSize: g_style.camSetTextFontSize

            onEditingFinished: {
                propertyChanged();
            }

        }
    }
}
