import QtQuick 2.4
import QtQuick.Controls 2.4

Page {
    id: element1
    anchors.fill: parent

    Text {
        id: element
        x: 308
        y: 233
        color: "#f7f6f6"
        text: qsTr("Test Works!")
        styleColor: "#eeecec"
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        font.pixelSize: 12
    }
}




/*##^## Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
 ##^##*/
