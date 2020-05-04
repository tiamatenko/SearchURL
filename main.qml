import QtQuick 2.12
import QtQuick.Controls 2.5

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("Search URL")

    Component.onCompleted: dataModel.start("https://www.musicradar.com", 10, "guitar", 30)

    ListView {
        anchors.fill: parent
        model: dataModel.nodes
        delegate: ItemDelegate {
            text: modelData.url
            width: parent.width
        }
    }
}
