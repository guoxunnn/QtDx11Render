import QtQuick 2.12
import QtQuick.Window 2.12
import Test 1.0

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")
    RenderAreaItem {
        width: parent.width
        height: parent.height
    }
}
