import QtQuick 2.12
import QtQuick.Controls 2.5
import ScanNode 1.0

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("Search URL")

    Component.onCompleted: dataModel.start("https://www.musicradar.com", 10, "guitar", 30)

    ListView {
        anchors.fill: parent
        model: dataModel.nodes
        delegate: Rectangle {
            width: parent.width
            height: 40
            color: {
                switch (modelData.scanStatus) {
                case ScanNode.Loading:
                    return 'blue'
                case ScanNode.Found:
                    return 'green'
                case ScanNode.NotFound:
                    return 'gray'
                case ScanNode.Error:
                    return 'red'
                }
                return "black"
            }

            Text {
                id: name
                anchors.verticalCenter: parent.verticalCenter
                leftPadding: 10
                text: (index + 1) + ". " + modelData.url + ": " + scanStatus()
                color: 'white'
                function scanStatus() {
                    switch (modelData.scanStatus) {
                    case ScanNode.Loading:
                        return "Loading"
                    case ScanNode.Found:
                        return "Found"
                    case ScanNode.NotFound:
                        return "Not Found"
                    case ScanNode.Error:
                        return modelData.errorString
                    }
                }
            }
        }
    }
}
