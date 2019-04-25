// Copyright 2019 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.

import QtQuick 2.0

Rectangle
{
    signal detectInput()

    property bool detecting_input : false
    
    id: input_box
    color: Qt.rgba(0, 0, 0, 0.5)
    radius: 15
    width: 80
    height: 25
    z: 1

    function setValue(value)
    {
        if (input_box.detecting_input)
            return;
 
        expression.text = value == "" ? "<Unmapped>" : value;
    }


    function detectedInput()
    {
        input_box.detecting_input = false;
    }

    function setPressed(pressed)
    {
        expression.font.bold = pressed;
    }
    
    Text {
        id: expression
        anchors.centerIn: input_box
        width: input_box.width - 10
        text: "<Uninitialized>"
        color: "white"
        elide: Text.ElideRight
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }


    MouseArea
    {
        anchors.fill: input_box
        onClicked: {
            input_box.detecting_input = true;
            expression.text = "...";
            input_box.detectInput();
        }
    }
}
