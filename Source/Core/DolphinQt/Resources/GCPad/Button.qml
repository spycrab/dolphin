import QtQuick 2.0
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.5

Image {
    property string name
    property int box_x : image.width / 2
    property int box_y : image.height - 10

    signal detectInput(string name)
    
    id: image
    source: name+".svg"

    objectName: "button"
    
    ColorOverlay
    {
        id: hover_overlay
        anchors.fill: image
        source: image
        color: "transparent"
    }
    
    function buttonPressed(pressed, expression)
    {
        input_box.setValue(expression)
        input_box.setPressed(pressed)
        hover_overlay.color = pressed ? Qt.rgba(1, 1, 0, 0.6) : "transparent";
    }

    function detectedInput()
    {
        input_box.detectedInput();
    }
    
    InputBox
    {
        onDetectInput: image.detectInput(image.name)
        id: input_box
        x: box_x - width/2
        y: box_y + 10
        z: 1
    }
}
