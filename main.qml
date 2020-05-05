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

    ColumnLayout {
        anchors.fill: parent
        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: dataModel.nodes
            delegate: Item {
                width: parent.width
                height: 40
                Rectangle {
                    anchors.fill: parent
                    anchors.margins: 2
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
                        text: (index + 1) + ". " + modelData.url + " --> " + scanStatus()
                        color: 'white'
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
                    }
                }
            }
        }
        ProgressBar {
            id: control
            Layout.fillWidth: true
            Layout.preferredHeight: 50
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
}
