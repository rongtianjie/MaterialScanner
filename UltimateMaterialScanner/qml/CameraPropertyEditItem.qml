import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import Ultimate.models 1.0

import "Base"


Item {
    width: parent.width
    height: 40

    property alias text : propertyText.text
    property alias comboBox : comboBox

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

        DComboBox {
            id: comboBox
            Layout.fillWidth: true
            Layout.maximumHeight: 30

            textRole: "text"
            valueRole: "value"
            model: PropertyModel{
                id: propertyModel
            }

            currentIndex: propertyModel.currentIndex

            onActivated: {
                propertyChanged();
            }

            onCurrentIndexChanged: {
                propertyModel.currentIndex = currentIndex;
            }

        }
    }
}
