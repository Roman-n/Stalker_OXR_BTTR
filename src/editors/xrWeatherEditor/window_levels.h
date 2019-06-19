#pragma once

using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace editor
{
ref class window_levels;
}

namespace editor
{
ref class window_ide;
/// <summary>
/// Summary for window_levels
///
/// WARNING: If you change the name of this class, you will need to change the
///          'Resource File Name' property for the managed resource compiler tool
///          associated with all .resx files this class depends on.  Otherwise,
///          the designers will not be able to interact properly with localized
///          resources associated with this form.
/// </summary>
public
ref class window_levels : public WeifenLuo::WinFormsUI::Docking::DockContent
{
public:
    window_levels(window_ide ^ ide) : m_ide(ide)
    {
        InitializeComponent();
        //
        // TODO: Add the constructor code here
        //
    }

protected:
    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    ~window_levels()
    {
        if (components)
        {
            delete components;
        }
    }

protected:
private: XRay::SdkControls::PropertyGrid ^ PropertyGrid;

private:
    /// <summary>
    /// Required designer variable.
    /// </summary>
    System::ComponentModel::Container ^ components;

#pragma region Windows Form Designer generated code
    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    void InitializeComponent(void)
    {
        this->PropertyGrid = (gcnew XRay::SdkControls::PropertyGrid());
        this->SuspendLayout();
        this->PropertyGrid->Dock = System::Windows::Forms::DockStyle::Fill;
        this->PropertyGrid->Location = System::Drawing::Point(0, 0);
        this->PropertyGrid->Name = L"PropertyGrid";
        this->PropertyGrid->Size = System::Drawing::Size(292, 273);
        this->PropertyGrid->TabIndex = 0;
        this->PropertyGrid->ToolbarVisible = false;
        this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
        this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
        this->ClientSize = System::Drawing::Size(292, 273);
        this->Controls->Add(this->PropertyGrid);
        this->DockAreas = static_cast<WeifenLuo::WinFormsUI::Docking::DockAreas>(((((WeifenLuo::WinFormsUI::Docking::DockAreas::Float | WeifenLuo::WinFormsUI::Docking::DockAreas::DockLeft)
            | WeifenLuo::WinFormsUI::Docking::DockAreas::DockRight)
            | WeifenLuo::WinFormsUI::Docking::DockAreas::DockTop)
            | WeifenLuo::WinFormsUI::Docking::DockAreas::DockBottom));
        this->HideOnClose = true;
        this->Name = L"window_levels";
        this->TabText = L"level weathers";
        this->Text = L"level weathers";
        this->Leave += gcnew System::EventHandler(this, &window_levels::window_levels_Leave);
        this->ResumeLayout(false);

    }
#pragma endregion
private:
    window_ide ^ m_ide;

public:
    inline ::PropertyGrid ^ property_grid() { return this->PropertyGrid; }

private: System::Void window_levels_Leave(System::Object ^ sender, System::EventArgs ^ e);
}; // ref class window_levels
} // namespace editor
