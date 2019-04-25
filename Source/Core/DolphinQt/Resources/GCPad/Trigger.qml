import QtQuick 2.0
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.5

Image {
    property string name
    property int box_x : image.width / 2
    property int box_y : 0

    id: image
    
    source: name+".svg"

    objectName: "trigger"

    signal detectInput(string name)
    
    ColorOverlay
    {
        id: hover_overlay
        anchors.fill: image
        source: image
        color: "transparent"
    }

    ProgressBar
    {
        id: progress
        value: 0
        y: -25
        anchors.horizontalCenter: image.horizontalCenter
        width: image.width
    }

    function detectedInput()
    {
        input_box.detectedInput();
    }
    
    function triggerPressed(value, expression)
    {
        if (expression != null)
            input_box.setValue(expression)
        
        progress.value = value;

        var pressed = value == 1

        hover_overlay.color = pressed ? Qt.rgba(1, 1, 0, 0.6) : "transparent";

        input_box.setPressed(value)
    }

    InputBox
    {
        onDetectInput: image.detectInput(image.name)
        id: input_box
        x: box_x - width/2
        y: box_y - height - 20
        z: 1
    }

}
