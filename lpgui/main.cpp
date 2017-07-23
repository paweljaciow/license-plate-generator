/**
* This file is part of the License Plate Generator project
* modified:    2017-07-23
* author:      Pawe³ Jaciów
* e-mail:      pawel.jaciow@gmail.com
*/

#include <wx/wxprec.h>
#include <wx/valnum.h>
#include <wx/busyinfo.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "LicensePlateGenerator.h"
#include <memory>
#include <thread>


class LicensePlateGui : public wxApp
{
public:
    virtual bool OnInit();
};

class PatternTextCtrl : public wxTextCtrl
{
    std::unique_ptr<licenseplate::LicensePlatePattern> pattern;

public:
    PatternTextCtrl(wxWindow* parent, wxWindowID id, const wxString& value = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxValidator& validator = wxDefaultValidator, const wxString& name = wxTextCtrlNameStr)
        : wxTextCtrl(parent, id, value, pos, size, style, validator, name) {}

    licenseplate::LicensePlatePattern GetPattern() const { return *pattern; }
    void SetPattern(const wxString & patternStr);
};

void PatternTextCtrl::SetPattern(const wxString& patternStr)
{
    pattern = std::make_unique<licenseplate::LicensePlatePattern>(licenseplate::LicensePlatePattern(patternStr.ToStdString()));
}

class LpguiFrame : public wxFrame
{
public:
    LpguiFrame();
private:
    const wxString labelCountTextBase = "Number of combinations:     ";
    const wxString labelPatternTextBase = "Pattern-";

    licenseplate::LicensePlateGenerator *lpGenerator;
    std::vector<PatternTextCtrl*> patternTexts;
    std::vector<licenseplate::LicensePlatePattern> patterns;

    wxTextValidator valPattern;
    const wxString valPatternCharIncludes = ",-0123456789?ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    wxPanel *panel;
    wxBoxSizer *vboxMain;
    wxTextCtrl *textboxNumOfPatterns;
    wxButton *buttonPrep;
    wxFlexGridSizer *patternsSizer;
    wxStaticText *labelCount;
    wxButton *buttonGen;
    wxTextCtrl *textboxMaxRes;
    wxTextCtrl *textboxResults;

    void OnPrep(wxCommandEvent& event);
};

enum
{
    ID_PrepText = 1,
    ID_PrepButton,
    ID_MaxResText,
    ID_GenButton,
    ID_PatternText = 100 // this must be last id
};

wxIMPLEMENT_APP(LicensePlateGui);

bool LicensePlateGui::OnInit()
{
    LpguiFrame *frame = new LpguiFrame();
    frame->Show(true);
    return true;
}

LpguiFrame::LpguiFrame()
    : wxFrame(NULL, wxID_ANY, "License Plate Generator"),
    valPattern(wxFILTER_INCLUDE_CHAR_LIST)
{
    valPattern.SetCharIncludes(valPatternCharIncludes);

    panel = new wxPanel(this, -1);

    vboxMain = new wxBoxSizer(wxVERTICAL);

    auto *hboxPrep = new wxBoxSizer(wxHORIZONTAL);

    auto *labelNumOfPatterns = new wxStaticText(panel, wxID_ANY, wxT("Num. of patterns"));

    wxIntegerValidator<unsigned int> valNumOfPatterns;
    valNumOfPatterns.SetRange(1, 10);
    textboxNumOfPatterns = new wxTextCtrl(panel, ID_PrepText, wxT(""), wxDefaultPosition, wxDefaultSize, 0, valNumOfPatterns, wxTextCtrlNameStr);
    textboxNumOfPatterns->SetHint(wxT("1-10"));
    Bind(wxEVT_TEXT, [=](wxCommandEvent&) {
        textboxNumOfPatterns->IsEmpty() ? buttonPrep->Disable() : buttonPrep->Enable();
    }, ID_PrepText);

    buttonPrep = new wxButton(panel, ID_PrepButton, wxT("Prep."));
    buttonPrep->Disable();
    Bind(wxEVT_BUTTON, &LpguiFrame::OnPrep, this, ID_PrepButton);

    hboxPrep->Add(labelNumOfPatterns, 0, wxLEFT | wxCENTER,0);
    hboxPrep->Add(textboxNumOfPatterns, 0, wxLEFT | wxCENTER, 10);
    hboxPrep->Add(buttonPrep, 0, wxLEFT | wxCENTER, 10);

    vboxMain->Add(hboxPrep, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10);

    patternsSizer = new wxFlexGridSizer(0, 2, 5, 10);
    patternsSizer->AddGrowableCol(1, 1);

    vboxMain->Add(patternsSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10);

    labelCount = new wxStaticText(panel, wxID_ANY, labelCountTextBase + wxT("-"));

    vboxMain->Add(labelCount, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10);

    auto *hboxGen = new wxBoxSizer(wxHORIZONTAL);

    buttonGen = new wxButton(panel, ID_GenButton, wxT("Generate"));
    buttonGen->Disable();
    Bind(wxEVT_BUTTON, [=](wxCommandEvent&) {
        buttonPrep->Disable();
        buttonGen->Disable();
        textboxResults->Clear();
        std::thread generator([&]() {
            wxBusyInfo wait("Please wait, generating...");
            const auto plates = lpGenerator->GenerateAllLicensePlates(wxAtoi(textboxMaxRes->GetValue()));
            for (const auto & p : plates)
            {
                textboxResults->AppendText(p.str() + "\n");
            }
            buttonPrep->Enable();
            buttonGen->Enable();
        });
        generator.detach();
    }, ID_GenButton);

    auto *labelMaxRes = new wxStaticText(panel, wxID_ANY, wxT("Max results shown:"));

    wxIntegerValidator<int> valMaxRes;
    valMaxRes.SetMin(-1);
    textboxMaxRes = new wxTextCtrl(panel, ID_MaxResText, wxT("-1"), wxDefaultPosition, wxDefaultSize, 0, valMaxRes, wxTextCtrlNameStr);
    Bind(wxEVT_TEXT, [=](wxCommandEvent&) {
        (textboxMaxRes->IsEmpty() || textboxMaxRes->GetValue() == wxT("-") || patterns.empty())
            ? buttonGen->Disable()
            : buttonGen->Enable();
    }, ID_MaxResText);

    hboxGen->Add(buttonGen, 0, wxLEFT | wxCENTER, 0);
    hboxGen->Add(labelMaxRes, 0, wxLEFT | wxCENTER, 10);
    hboxGen->Add(textboxMaxRes, 0, wxLEFT | wxCENTER, 10);

    vboxMain->Add(hboxGen, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10);

    auto *vboxResults = new wxBoxSizer(wxVERTICAL);

    auto *labelResults = new wxStaticText(panel, wxID_ANY, wxT("Resulting combinations:"));

    textboxResults = new wxTextCtrl(panel, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY);

    vboxResults->Add(labelResults, 0, wxLEFT, 0);
    vboxResults->Add(textboxResults, 1, wxLEFT | wxEXPAND, 0);
    vboxResults->AddSpacer(10);

    vboxMain->Add(vboxResults, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10);

    panel->SetSizer(vboxMain);

    SetMinSize(wxSize(400, 600));

    Centre();
}

void LpguiFrame::OnPrep(wxCommandEvent& event)
{
    buttonGen->Disable();
    textboxResults->Clear();
    patternTexts.clear();
    patterns.clear();
    patternsSizer->Clear(true);
    const auto nPatterns = wxAtoi(this->textboxNumOfPatterns->GetValue());
    for (auto i = 0; i < nPatterns; ++i)
    {
        auto *labelPattern = new wxStaticText(panel, wxID_ANY, labelPatternTextBase + wxString::Format(wxT("%i"), i + 1));
        auto *textboxPattern = new PatternTextCtrl(panel, ID_PatternText+i, wxT(""), wxDefaultPosition, wxDefaultSize, 0, valPattern);
        textboxPattern->SetHint(wxT("A-Z,0-9"));
        Bind(wxEVT_TEXT, [=](wxCommandEvent&) {
            patterns.clear();
            textboxPattern->SetPattern(textboxPattern->GetValue());
            for (auto j = 0; j < nPatterns; ++j)
            {
                if (!patternTexts[j]->IsEmpty() && patternTexts[j]->GetPattern().symbols().size() > 0)
                {
                    patterns.push_back(patternTexts[j]->GetPattern());
                }
            }
            lpGenerator = new licenseplate::LicensePlateGenerator(patterns);
            wxLongLongNative wxlong(lpGenerator->CountLicensePlates());
            labelCount->SetLabel(labelCountTextBase + wxlong.ToString()/*wxString::Format(wxT("%li"), lpGenerator->CountLicensePlates())*/);
            (textboxMaxRes->IsEmpty() || textboxMaxRes->GetValue() == wxT("-") || patterns.empty())
                ? buttonGen->Disable()
                : buttonGen->Enable();
        }, ID_PatternText+i);
        patternTexts.push_back(textboxPattern);
        patternsSizer->Add(labelPattern, 0, wxLEFT | wxCENTER, 0);
        patternsSizer->Add(textboxPattern, 0, wxLEFT | wxCENTER | wxEXPAND);
    }
    vboxMain->Layout();
}
