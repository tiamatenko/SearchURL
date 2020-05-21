import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import ScanNode 1.0

Rectangle {
    property alias cursorHeight: textEdit.cursorRectangle.height
    property alias text: textEdit.text
    color: 'lightyellow'
    clip: true
    TextEdit {
        id: textEdit
        anchors.fill: parent
        selectByMouse: true
    }
}
