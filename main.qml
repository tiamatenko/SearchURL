import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import ScanNode 1.0

ApplicationWindow {
    visible: true
    title: qsTr("Search URL")

    Component.onCompleted: {
        showMaximized();
        dataModel.start("https://football.ua", 10, "football", 100)
    }

    ListView {
        anchors.fill: parent
        clip: true
        model: dataModel.nodes
        delegate: ItemDelegate {
            id: item
            width: parent.width
            height: 40
            text: (index + 1) + ". " + modelData.url + " --> " + scanStatus()
            function scanStatus() {
                switch (modelData.scanStatus) {
                case ScanNode.Loading:
                    return "Loading..."
                case ScanNode.Found:
                    return "OK"
                case ScanNode.NotFound:
                    return "Not Found"
                case ScanNode.Error:
                    return modelData.errorString
                }
            }

            background: Rectangle {
                color: 'transparent'

                Rectangle {
                    anchors.fill: parent
                    anchors.margins: 2
                    color: item.down ? Qt.lighter(statusColor(), 1.3) : statusColor()
                    function statusColor() {
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
                        return 'transparent'
                    }
                }
            }
        }
    }

    header: ProgressBar {
        id: control
        height: 50
        to: dataModel.maxDocCount
        value: dataModel.scannedDocs
        padding: 2

        background: Rectangle {
            color: '#e6e6e6'
            radius: 3
        }

        contentItem: Item {
            Rectangle {
                width: control.visualPosition * parent.width
                height: parent.height
                radius: 2
                color: 'darkorange'
            }
            Text {
                anchors.centerIn: parent
                text: control.value + " / " + control.to
            }
        }
    }
}
