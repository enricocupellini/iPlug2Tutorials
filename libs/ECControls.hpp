//
//  ECControls.hpp
//  FactoryPresets-macOS
//
//  Created by Enrico Cupellini on 15/09/2019.
//

#ifndef ECControls_hpp
#define ECControls_hpp

#include "IPlug_include_in_plug_hdr.h"
#include "IControls.h"

using namespace iplug;
using namespace igraphics;

class ImageFileBrowser : public IDirBrowseControlBase
{
private:
    WDL_String mLabel;
    IBitmap mBitmap;
public:
    ImageFileBrowser(const IRECT& bounds)
    : IDirBrowseControlBase(bounds, ".png")
    {
        WDL_String path;
        //    DesktopPath(path);
        path.Set(__FILE__);
        path.remove_filepart();
#ifdef OS_WIN
        path.Append("\\resources\\img\\");
#else
        path.Append("/resources/img/");
#endif
        AddPath(path.Get(), "");
        
        mLabel.Set("Click here to browse png files...");
    }
    
    void Draw(IGraphics& g) override
    {
        g.FillRect(COLOR_TRANSLUCENT, mRECT);
        
        IRECT labelRect = mRECT.GetFromBottom(mText.mSize);
        IRECT bmpRect = mRECT.GetReducedFromBottom(mText.mSize);
        
        if(mBitmap.GetAPIBitmap())
        {
            //if stacked frames, don't try and fit the whole bitmap to the bounds
            if(mBitmap.N())
                g.DrawBitmap(mBitmap, bmpRect, 1);
            else
                g.DrawFittedBitmap(mBitmap, bmpRect);
        }
        
        g.FillRect(COLOR_WHITE, labelRect);
        g.DrawText(mText, mLabel.Get(), labelRect);
    }
    
    void OnMouseDown(float x, float y, const IMouseMod& mod) override
    {
        SetupMenu();
        
        GetUI()->CreatePopupMenu(*this, mMainMenu, x, y);
    }
    
    void OnPopupMenuSelection(IPopupMenu* pSelectedMenu, int valIdx) override
    {
        if(pSelectedMenu)
        {
            IPopupMenu::Item* pItem = pSelectedMenu->GetChosenItem();
            WDL_String* pStr = mFiles.Get(pItem->GetTag());
            mLabel.Set(pStr);
            mBitmap = GetUI()->LoadBitmap(pStr->Get());
            SetTooltip(pStr->Get());
            SetDirty(false);
        }
    }
};

class PresetsBankBrowser : public IDirBrowseControlBase
{
private:
    WDL_String mLabel;
    WDL_String mPreviousPath;
    WDL_String path;
    
public:
    PresetsBankBrowser(const IRECT& bounds)
    : IDirBrowseControlBase(bounds, ".fxb")
    {
        //    DesktopPath(path);
        path.Set(__FILE__);
        path.remove_filepart();
#ifdef OS_WIN
        path.Append("\\resources\\");
#else
        path.Append("/resources/");
#endif
        AddPath(path.Get(), "");
        
        mLabel.Set("Click here to browse preset banks...");
        mPreviousPath = path;
    }
    
    void Draw(IGraphics& g) override
    {
        g.FillRect(COLOR_TRANSLUCENT, mRECT);
        
        IRECT labelRect = mRECT.GetFromBottom(mText.mSize);
        IRECT bmpRect = mRECT.GetReducedFromBottom(mText.mSize);
        WDL_String fileName;
        IPluginBase* mPlug = dynamic_cast<IPluginBase*> (GetDelegate()); fileName.Set(mPlug->GetPresetName(mPlug->GetCurrentPresetIdx()));
        
        g.DrawText(IText(24.f, COLOR_BLACK), fileName.Get(), bmpRect);
        
        g.FillRect(COLOR_WHITE, labelRect);
        g.DrawText(mText, mLabel.Get(), labelRect);
    }
    
    void OnMouseDown(float x, float y, const IMouseMod& mod) override
    {
        SetupMenu();
        
        mMainMenu.AddItem("Save Preset Bank", 0);
        mMainMenu.AddItem("Select Preset Bank Folder", 1);
        mMainMenu.AddSeparator(2);
        GetUI()->CreatePopupMenu(*this, mMainMenu, x, y);
    }
    
    void OnPopupMenuSelection(IPopupMenu* pSelectedMenu, int valIdx) override
    {
        if(pSelectedMenu)
        {
            IPluginBase* mPlug = dynamic_cast<IPluginBase*> (GetDelegate());
            if(pSelectedMenu == &mMainMenu && pSelectedMenu->GetChosenItemIdx() < 2)
            {
                switch (pSelectedMenu->GetChosenItemIdx()) {
                    case 0:     // SAVE preset bank
                    {
                        WDL_String fileName;
                        if (strcmp(mLabel.Get(), "Click here to browse preset banks...") == 0)  // match
                        {
                            fileName.Set("Bank ...");
                        }
                        else{
                            fileName.Set(mLabel.Get());
                        }
                        //                         fileName.Set(mPlug->GetPresetName(mPlug->GetCurrentPresetIdx()));
                        GetUI()->PromptForFile(fileName, mPreviousPath, EFileAction::Save, "fxb");
                        //                        mPlug->SaveProgramAsFXP(fileName.Get());
                        if (strcmp(fileName.Get(), "") != 0)  // not match
                        {
                            mPlug->ModifyCurrentPreset();
                            mPlug->SaveBankAsFXB(fileName.Get());
                            mLabel.Set(fileName.get_filepart());
                            SetDirty(false);
                        }
                        break;
                    }
                    case 1:
                    {
                        GetUI()->PromptForDirectory(mPreviousPath);
                        if (mPreviousPath.GetLength()>0) {
                            mPaths.Empty(true);
                            AddPath(mPreviousPath.Get(), "");
                        }
                        SetDirty(false);
                        break;
                    }
                    default:
                        break;
                }
            }
            else{
                IPopupMenu::Item* pItem = pSelectedMenu->GetChosenItem();
                WDL_String* pStr = mFiles.Get(pItem->GetTag());
                mPlug->LoadBankFromFXB(pStr->Get());
                pStr->remove_fileext();
                mLabel.Set(pStr->get_filepart());
                //                mPlug->LoadProgramFromFXP(pStr->Get());
                SetDirty(false);
            }
        }
    }
    
//    void RestoreDefaultBank() {
//        WDL_String* pStr = mFiles.Get(pItem->GetTag());
//        mPlug->LoadBankFromFXB(pStr->Get());
//        pStr->remove_fileext();
//        mLabel.Set(pStr->get_filepart());
//        //                mPlug->LoadProgramFromFXP(pStr->Get());
//        SetDirty(false);
//    }
};

class CaptionPresets : public ICaptionControl {
    int controlTagToRecal;
public:
    CaptionPresets(const IRECT& bounds, int paramIdx, const IText& text, const IColor& BGColor, int ctrTagToRecal, bool showParamLabel = true)
    :ICaptionControl(bounds, paramIdx, text, BGColor), controlTagToRecal(ctrTagToRecal) {}
    
//    void OnPopupMenuSelection(IPopupMenu* pSelectedMenu, int valIdx) override {
//        if (pSelectedMenu != nullptr) {
//            IPluginBase* mPlug = dynamic_cast<IPluginBase*> (GetDelegate());
//            mPlug->RestorePreset(pSelectedMenu->GetChosenItemIdx());
//            mPlug->GetUI()->GetControlWithTag(controlTagToRecal)->SetDirty();
//        }
//    }
};
#endif /* ECControls_hpp */
