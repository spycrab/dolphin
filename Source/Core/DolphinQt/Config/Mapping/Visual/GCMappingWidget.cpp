// Copyright 2019 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.

#include "DolphinQt/Config/Mapping/Visual/GCMappingWidget.h"

#include <QApplication>
#include <QMetaObject>
#include <QQuickItem>
#include <QQuickWidget>
#include <QThread>
#include <QTimer>
#include <QVBoxLayout>

#include "Common/FileUtil.h"

#include "Core/HW/GCPad.h"
#include "Core/HW/GCPadEmu.h"

#include "DolphinQt/Config/Mapping/MappingCommon.h"
#include "DolphinQt/Config/Mapping/MappingWindow.h"
#include "DolphinQt/Settings.h"

#include "InputCommon/ControllerEmu/Control/Control.h"
#include "InputCommon/ControllerEmu/ControlGroup/Buttons.h"
#include "InputCommon/ControllerEmu/ControllerEmu.h"
#include "InputCommon/ControllerEmu/StickGate.h"
#include "InputCommon/ControllerInterface/ControllerInterface.h"
#include "InputCommon/InputConfig.h"

#include <iostream>

VisualGCMappingWidget::VisualGCMappingWidget(MappingWindow* parent, int port)
    : QWidget(parent), m_parent(parent), m_port(port)
{
  CreateWidgets();

  m_timer = new QTimer(this);
  connect(m_timer, &QTimer::timeout, this, &VisualGCMappingWidget::RefreshControls);
  m_timer->start(1000 / INDICATOR_UPDATE_FREQ);
}

void VisualGCMappingWidget::CreateWidgets()
{
  auto* layout = new QVBoxLayout;

  m_widget = new QQuickWidget;
  m_widget->setSource(QUrl(QStringLiteral("qrc:/GCPad/GCPad.qml")));
  m_widget->setResizeMode(QQuickWidget::SizeRootObjectToView);
  m_widget->setClearColor(Qt::transparent);
  m_widget->setAttribute(Qt::WA_TranslucentBackground);
  m_widget->setAttribute(Qt::WA_AlwaysStackOnTop);

  layout->addWidget(m_widget);

  // We need to use the old signal / slot syntax here in order to access the QML signal
  connect(GetQMLController(), SIGNAL(detectInput(QString)), this, SLOT(DetectInput(QString)),
          Qt::QueuedConnection);

  setLayout(layout);
}

void VisualGCMappingWidget::DetectInput(QString name)
{
  std::cout << "NAME: " << name.toStdString() << std::endl;

  auto* buttons = Pad::GetGroup(m_port, PadGroup::Buttons);

  ControllerEmu::Control* map_control = nullptr;

  for (auto& control : buttons->controls)
  {
    if (control->name == name.toStdString())
    {
      map_control = control.get();
      break;
    }
  }

  auto* dpad = Pad::GetGroup(m_port, PadGroup::DPad);
  for (auto& control : dpad->controls)
  {
    if ("DPad_" + control->name == name.toStdString())
    {
      map_control = control.get();
      break;
    }
  }

  auto* triggers = Pad::GetGroup(m_port, PadGroup::Triggers);
  for (auto& control : triggers->controls)
  {
    if ("Trigger_" + control->name == name.toStdString())
    {
      map_control = control.get();
      break;
    }
  }

  auto* main_stick = Pad::GetGroup(m_port, PadGroup::MainStick);
  for (auto& control : main_stick->controls)
  {
    if ("Stick_Main_" + control->name == name.toStdString())
    {
      map_control = control.get();
      break;
    }
  }

  auto* c_stick = Pad::GetGroup(m_port, PadGroup::CStick);
  for (auto& control : c_stick->controls)
  {
    if ("Stick_C_" + control->name == name.toStdString())
    {
      map_control = control.get();
      break;
    }
  }

  if (map_control == nullptr)
    return;

  auto* thread = QThread::create([this, map_control, name] {
    const auto default_device_qualifier = m_parent->GetController()->GetDefaultDevice();

    QString expression;

    if (m_parent->IsMappingAllDevices())
    {
      expression = MappingCommon::DetectExpression(this, g_controller_interface,
                                                   g_controller_interface.GetAllDeviceStrings(),
                                                   default_device_qualifier);
    }
    else
    {
      expression = MappingCommon::DetectExpression(this, g_controller_interface,
                                                   {default_device_qualifier.ToString()},
                                                   default_device_qualifier);
    }

    QMetaObject::invokeMethod(GetQMLController(), "detectedInput", Qt::QueuedConnection,
                              Q_ARG(QVariant, name));

    if (expression.isEmpty())
      return;

    map_control->control_ref->SetExpression(expression.toStdString());
    m_parent->GetController()->UpdateReferences(g_controller_interface);

    Pad::GetConfig()->SaveConfig();
  });

  thread->setParent(this);
  thread->start();
}

QObject* VisualGCMappingWidget::GetQMLController()
{
  return m_widget->rootObject()->findChild<QObject*>(QStringLiteral("controller"));
}

void VisualGCMappingWidget::RefreshControls()
{
  if (!isActiveWindow())
    return;

  auto* buttons = Pad::GetGroup(m_port, PadGroup::Buttons);

  QObject* object = m_widget->rootObject()->findChild<QObject*>(QStringLiteral("controller"));

  const auto lock = m_parent->GetController()->GetStateLock();
  Settings::Instance().SetControllerStateNeeded(true);

  for (auto& control : buttons->controls)
  {
    auto state = control->control_ref->State();
    const bool activated = state > ControllerEmu::Buttons::ACTIVATION_THRESHOLD;
    const std::string expression = control->control_ref->GetExpression();

    QMetaObject::invokeMethod(object, "updateButton", Qt::QueuedConnection,
                              Q_ARG(QVariant, QString::fromStdString(control->name)),
                              Q_ARG(QVariant, activated),
                              Q_ARG(QVariant, QString::fromStdString(expression)));
  }

  auto* dpad = Pad::GetGroup(m_port, PadGroup::DPad);

  for (auto& control : dpad->controls)
  {
    auto state = control->control_ref->State();
    const bool activated = state > ControllerEmu::Buttons::ACTIVATION_THRESHOLD;
    const std::string expression = control->control_ref->GetExpression();

    QMetaObject::invokeMethod(object, "updateButton", Qt::QueuedConnection,
                              Q_ARG(QVariant, QString::fromStdString("DPad_" + control->name)),
                              Q_ARG(QVariant, activated),
                              Q_ARG(QVariant, QString::fromStdString(expression)));
  }

  auto* triggers = Pad::GetGroup(m_port, PadGroup::Triggers);

  for (size_t i = 2; i < 4; i++)
  {
    auto& control = triggers->controls[i];

    auto state = control->control_ref->State();

    const auto name = control->name.substr(0, 1);

    QMetaObject::invokeMethod(object, "updateTrigger", Qt::QueuedConnection,
                              Q_ARG(QVariant, QString::fromStdString("Trigger_" + name)),
                              Q_ARG(QVariant, state), Q_ARG(QVariant, QVariant()));
  }

  static bool once = true;

  for (size_t i = 0; i < 2; i++)
  {
    auto& control = triggers->controls[i];

    const bool activated =
        control->control_ref->State() > ControllerEmu::Buttons::ACTIVATION_THRESHOLD;

    if (!activated && !once)
      continue;

    const std::string expression = control->control_ref->GetExpression();

    QMetaObject::invokeMethod(object, "updateTrigger", Qt::QueuedConnection,
                              Q_ARG(QVariant, QString::fromStdString("Trigger_" + control->name)),
                              Q_ARG(QVariant, activated),
                              Q_ARG(QVariant, QString::fromStdString(expression)));
  }

  once = false;

  auto* main_stick =
      static_cast<ControllerEmu::ReshapableInput*>(Pad::GetGroup(m_port, PadGroup::MainStick));

  auto main_state = main_stick->GetReshapableState(true);

  QMetaObject::invokeMethod(object, "updateStick",
                            Q_ARG(QVariant, QString::fromStdString("Stick_Main")),
                            Q_ARG(QVariant, main_state.x), Q_ARG(QVariant, main_state.y));

  auto* main_stick_group = Pad::GetGroup(m_port, PadGroup::MainStick);

  for (auto& control : main_stick_group->controls)
  {
    const std::string expression = control->control_ref->GetExpression();
    QMetaObject::invokeMethod(object, "updateStickExpression",
                              Q_ARG(QVariant, QString::fromStdString("Stick_Main")),
                              Q_ARG(QVariant, QString::fromStdString(control->name)),
                              Q_ARG(QVariant, QString::fromStdString(expression)));
  }

  auto* c_stick =
      static_cast<ControllerEmu::ReshapableInput*>(Pad::GetGroup(m_port, PadGroup::CStick));

  auto c_state = c_stick->GetReshapableState(true);

  QMetaObject::invokeMethod(object, "updateStick",
                            Q_ARG(QVariant, QString::fromStdString("Stick_C")),
                            Q_ARG(QVariant, c_state.x), Q_ARG(QVariant, c_state.y));

  auto* c_stick_group = Pad::GetGroup(m_port, PadGroup::CStick);

  for (auto& control : c_stick_group->controls)
  {
    const std::string expression = control->control_ref->GetExpression();
    QMetaObject::invokeMethod(object, "updateStickExpression",
                              Q_ARG(QVariant, QString::fromStdString("Stick_C")),
                              Q_ARG(QVariant, QString::fromStdString(control->name)),
                              Q_ARG(QVariant, QString::fromStdString(expression)));
  }

  Settings::Instance().SetControllerStateNeeded(false);
}
