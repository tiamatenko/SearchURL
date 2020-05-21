import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12

Rectangle {
    property alias cursorHeight: textInput.cursorRectangle.height
    property alias text: textInput.text
    color: 'lightyellow'
    clip: true
    TextInput {
        id: textInput
        anchors.fill: parent
        horizontalAlignment: TextInput.AlignRight
        validator: IntValidator {
            bottom: 1
            top: 100
        }
        selectByMouse: true
    }
}
