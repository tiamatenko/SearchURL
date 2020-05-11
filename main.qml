import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.3
import ScanNode 1.0
import ScanEngine 1.0

ApplicationWindow {
    visible: true
    title: qsTr("Search URL")

    width: 800
    height: 600

    ListView {
        anchors.fill: parent
        clip: true
        model: scanEngine.nodes
        delegate: ItemDelegate {
            id: item
            width: parent.width
            height: 30
            text: (index + 1) + ". " + modelData.url + " --> " + scanStatus()
            function scanStatus() {
                switch (modelData.scanStatus) {
                case ScanNode.Deferred:
                    return "Deferred"
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

        Dialog {
            id: resultDialog
            title: "Scan results:"
            visible: false
            contentItem: Item {
                ColumnLayout {
                    anchors.fill: parent
                    Text { text: qsTr("Found: ") + scanEngine.foundDocCount }
                    Text { text: qsTr("Not Found: ") + scanEngine.notFoundDocCount }
                    Text { text: qsTr("Error: ") + scanEngine.errorDocCount }
                }
            }
        }
    }

    header: ProgressBar {
        id: control
        height: 40
        to: scanEngine.maxDocCount
        value: scanEngine.scannedDocs
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

    footer: Rectangle {
        height: 80
        color: 'lightgray'
        RowLayout {
            anchors.fill: parent
            anchors.margins: 5
            GridLayout {
                columns: 2
                Text { text: qsTr("Base URL:") }
                StringInput {
                    id: baseUrl
                    text: "https://football.ua"
                }
                Text {
                    text: qsTr("Search text:")
                }
                StringInput {
                    id: searchText
                    text: "football"
                }
            }
            GridLayout {
                columns: 2
                Text { text: qsTr("Max documents:") }
                IntInput {
                    id: maxDocs
                    Component.onCompleted: text = scanEngine.maxDocCount
                }
                Text { text: qsTr("Max threads:") }
                IntInput {
                    id: maxThreads
                    Component.onCompleted: text = scanEngine.maxThreadCount
                }
            }
            GridLayout {
                columns: 2
                Text { text: qsTr("Scanned docs:") }
                Text { text: scanEngine.scannedDocs }
                Text { text: qsTr("Active threads:") }
                Text { text: scanEngine.activeThreadCount }
                Text { text: qsTr("Deferred threads:") }
                Text { text: scanEngine.deferredThreadCount }
            }
            RowLayout {
                Layout.fillWidth: true
                spacing: 0
                Button {
                    id: startButton
                    Layout.alignment: Qt.AlignRight
                    ButtonGroup.group: radioGroup
                    checkable: true
                    icon.name: "media-playback-start"
                    onToggled: {
                        switch (scanEngine.state) {
                        case ScanEngine.Stopped:
                            if (baseUrl.text.trim().length > 0 && searchText.text.trim().length > 0) {
                                scanEngine.start(baseUrl.text.trim(), searchText.text.trim(), maxDocs.text, maxThreads.text)
                            }
                            else {
                                checked = false
                                messageDialog.showMessage(qsTr("URL and/or search text cannoot be empty"))
                            }
                            break;
                        case ScanEngine.Paused:
                            scanEngine.resume()
                            break;
                        }
                    }
                }
                Button {
                    id: stopButton
                    ButtonGroup.group: radioGroup
                    checkable: true
                    icon.name: "media-playback-stop"
                    onToggled: {
                        if (scanEngine.state !== ScanEngine.Stopped) {
                            scanEngine.stop()
                        }
                        else {
                            checked = false
                            messageDialog.showMessage(qsTr("Scan is not started yet"))
                        }
                    }
                }
                Button {
                    id: pauseButton
                    ButtonGroup.group: radioGroup
                    checkable: true
                    icon.name: "media-playback-pause"
                    onToggled: {
                        if (scanEngine.state === ScanEngine.Running) {
                            scanEngine.pause()
                        }
                        else {
                            checked = false
                            messageDialog.showMessage(qsTr("Scan is not started yet"))
                        }
                    }
                }
            }
        }
        ButtonGroup { id: radioGroup }
    }

    Component.onCompleted: scanEngine.stateChanged.connect(showResults)

    function showResults() {
        if (scanEngine.state === ScanEngine.Stopped) {
            radioGroup.checkedButton.checked = false
            resultDialog.open()
        }
    }

    MessageDialog {
        id: messageDialog
        title: qsTr("Warning")
        function showMessage(message) {
            text = message
            open()
        }
    }
}
