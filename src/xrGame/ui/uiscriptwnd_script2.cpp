#include "pch_script.h"

// UI-controls

#include "UIScriptWnd.h"
#include "xruicore/buttons/UIButton.h"
#include "xruicore/MessageBox/UIMessageBox.h"
#include "xruicore/PropertiesBox/UIPropertiesBox.h"
#include "xruicore/buttons/UICheckButton.h"
#include "xruicore/Buttons/UIRadioButton.h"
#include "xruicore/static/UIStatic.h"
#include "xruicore/editbox/UIEditBox.h"
#include "xruicore/Windows/UIFrameWindow.h"
#include "xruicore/Windows/UIFrameLineWnd.h"
#include "xruicore/progressbar/UIProgressBar.h"
#include "xruicore/TabControl/UITabControl.h"

#include "uiscriptwnd_script.h"

using namespace luabind;

#pragma optimize("s", on)
export_class& script_register_ui_window2(export_class& instance)
{
    instance.def("OnKeyboard", &BaseType::OnKeyboardAction, &WrapType::OnKeyboard_static)
        .def("Update", &BaseType::Update, &WrapType::Update_static)
        .def("Dispatch", &BaseType::Dispatch, &WrapType::Dispatch_static)

        ;
    return (instance);
}
