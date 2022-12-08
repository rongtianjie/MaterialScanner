import QtQuick 2.0

Item {
    property bool opened : false
    property var targetObj: null

    SequentialAnimation {
        id: openAnim
        PropertyAction { target: targetObj; property: "visible"; value: true }
        PropertyAnimation{ target: targetObj; property: "x"; easing.type: Easing.InOutQuad; from: targetObj.parent.width; to: targetObj.parent.width-targetObj.width; duration: 500  }
    }

    SequentialAnimation {
        id: closeAnim
        PropertyAnimation{ target: targetObj; property: "x"; easing.type: Easing.InOutQuad; from: targetObj.parent.width-targetObj.width; to: targetObj.parent.width; duration: 500  }
        PropertyAction { target: targetObj; property: "visible"; value: false }
    }

    onOpenedChanged: {
        if(opened)
        {
            closeAnim.stop();
            openAnim.start();
        }
        else
        {
            openAnim.stop();
            closeAnim.start();
        }
    }
}
