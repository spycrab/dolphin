// Copyright 2019 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.

import QtQuick 2.0
import QtGraphicalEffects 1.0
Rectangle {
    id: base

    property string name
    property int radius
    
    property int org_x: x
    property int org_y: y

    property int shift_input_x: 0
    property int shift_input_y: 0
    
    signal detectInput(string name)
    
    objectName: "stick"

    Component.onCompleted: { org_x = org_x; org_y = org_y; }
    
    Image {
        id: image
                
        opacity: 0.5
        
        source: base.name+".svg"
        
        ColorOverlay
        {
            id: hover_overlay
            anchors.fill: image
            source: image
            color: "transparent"
        }
    }
    
    function setPosition(new_x, new_y)
    {
        image.x = new_x * radius;
        image.y = -new_y * radius;
    }

    function detectedInput(direction)
    {
        if (direction == "Up")
            input_box_up.detectedInput();
        if (direction == "Down")
            input_box_down.detectedInput();
        if (direction == "Left")
            input_box_left.detectedInput();
        if (direction == "Right")
            input_box_right.detectedInput();
        if (direction == "Down")
            input_box_down.detectedInput();
    }

    function setExpression(direction, expression)
    {
        if (direction == "Up")
            input_box_up.setValue(expression)
        if (direction == "Down")
            input_box_down.setValue(expression)
        if (direction == "Left")
            input_box_left.setValue(expression)
        if (direction == "Right")
            input_box_right.setValue(expression)
        if (direction == "Down")
            input_box_down.setValue(expression)
    }
    
    InputBox
    {
        id: input_box_up
        x: base.radius - width/2 - 10 + shift_input_x
        y: -30 + shift_input_y
        onDetectInput: base.detectInput(base.name+"_Up")
    }

    InputBox
    {
        id: input_box_down
        x: base.radius - width/2 - 10 + shift_input_x
        y: base.radius*2 + shift_input_y
        onDetectInput: base.detectInput(base.name+"_Down")
    }

    InputBox
    {
        id: input_box_left
        x: -10 - width + shift_input_x
        y: base.radius - height + shift_input_y
        onDetectInput: base.detectInput(base.name+"_Left")
    }

    InputBox
    {
        id: input_box_right
        x: base.radius*2 - 10 + shift_input_x
        y: base.radius - height + shift_input_y
        onDetectInput: base.detectInput(base.name+"_Right")
    }
}
