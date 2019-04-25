// Copyright 2019 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.

import QtQuick 2.0

Rectangle
{
    id: rect
    color: "transparent"

    Rectangle
    {
        objectName: "controller"
        id: controller

        signal detectInput(string name)

        function updateButton(name, value, expression)
        {
            for (var i = 0; i < controller.children.length; i++)
            {
                var item = controller.children[i];

                if (item.name == name && item.objectName == "button")
                {
                    item.buttonPressed(value, expression);
                    return;
                }
            }
        }

        function updateTrigger(name, value, expression)
        {
            for (var i = 0; i < controller.children.length; i++)
            {
                var item = controller.children[i];

                if (item.name == name && item.objectName == "trigger")
                {
                    item.triggerPressed(value, expression);
                    return;
                }
            }
        }

        function updateStick(name, x, y)
        {
            for (var i = 0; i < controller.children.length; i++)
            {
                var item = controller.children[i];

                if (item.name == name && item.objectName == "stick")
                {
                    item.setPosition(x, y);
                    return;
                }
            }
        }

        function updateStickExpression(name, direction, expression)
        {
            for (var i = 0; i < controller.children.length; i++)
            {
                var item = controller.children[i];

                if (item.name == name && item.objectName == "stick")
                {
                    item.setExpression(direction, expression);
                    return;
                }
            }
        }

        function detectedInput(name)
        {
            if (name.startsWith("Stick_Main_")) {
                stick_main.detectedInput(name.substr(11))
                return;
            }
            if (name.startsWith("Stick_C_")) {
                stick_c.detectedInput(name.substr(8))
                return;
            }
            for (var i = 0; i < controller.children.length; i++)
            {
                var item = controller.children[i];
                if (item.name == name)
                {
                    item.detectedInput();
                    return;
                }
            }            
        }
        
        Image {
            source: "base.svg"
        }

        x: (rect.width - 650) / 2
        y: (rect.height - 500) / 2        
        
        Trigger {
            name: "Trigger_R"
            x: 505
            y: 20
            z: -2
            onDetectInput: controller.detectInput(name)
        }
        Button {
            name: "Z"
            x: 495
            y: 30
            z: -1
            box_x: 155
            box_y: 20
            onDetectInput: controller.detectInput(name)
        }
        Trigger {
            name: "Trigger_L"
            x: 60
            y: 30
            z: -1
            onDetectInput: controller.detectInput(name)
        }
        Button {
            name: "A"
            x: 510
            y: 135
            onDetectInput: controller.detectInput(name)
        }
        Button {
            name: "B"
            x: 460
            y: 190
            onDetectInput: controller.detectInput(name)
        }
        Button {
            name: "X"
            x: 605
            y: 120
            onDetectInput: controller.detectInput(name)

        }
        Button {
            name: "Y"
            x: 500
            y: 85
            onDetectInput: controller.detectInput(name)
        }
        Button {
            name: "Start"
            x: 320
            y: 165
            onDetectInput: controller.detectInput(name)
        }
        Stick {
            id: stick_c
            name: "Stick_C"
            radius: 50
            x: 420
            y: 300
            shift_input_x: -5
            onDetectInput: controller.detectInput(name)
        }
        Stick {
            id: stick_main
            name: "Stick_Main"
            radius: 60
            x: 75
            y: 128
            onDetectInput: controller.detectInput(name)
        }
        Button {
            name: "DPad_Up"
            x: 214
            y: 293
            box_y: -45
            onDetectInput: controller.detectInput(name)
        }
        Button {
            onDetectInput: controller.detectInput(name)
            name: "DPad_Down"
            x: 214
            y: 358
            box_y: 15
        }
        Button {
            name: "DPad_Left"
            x: 185
            y: 323
            box_x: -50
            box_y: -12
            onDetectInput: controller.detectInput(name)
        }
        Button {
            name: "DPad_Right"
            x: 245
            y: 323
            box_x: 70
            box_y: -12
            onDetectInput: controller.detectInput(name)
        }
    }
}
