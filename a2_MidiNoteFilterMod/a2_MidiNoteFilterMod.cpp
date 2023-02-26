#include "a2_MidiNoteFilterMod.h"
#include "IPlug_include_in_plug_src.h"
#include "ECControls.hpp"
#include "ECFilter.hpp"

a2_MidiNoteFilterMod::a2_MidiNoteFilterMod(const InstanceInfo& info)
: Plugin(info, MakeConfig(kNumParams, kNumPresets))
{
    //Preset
    GetParam(kPresetsMode)->InitEnum("Mode", 0, 16, "Preset", IParam::kFlagsNone, "", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16");
    
    GetParam(kParamGain)->InitDouble("Gain", 100., 0., 100.0, 0.01, "%");
    
    GetParam(kParamMode)->InitEnum("Mode", 0, 4, "Oscillator", IParam::kFlagsNone, "", "sine", "saw", "square", "triangular");
    
    GetParam(kParamAttack)->InitDouble("Attack", 10., 1., 3000., 0.1, "ms", IParam::kFlagsNone, "ADSR", IParam::ShapeExp());
    GetParam(kParamDecay)->InitDouble("Decay", 10., 1., 3000., 0.1, "ms", IParam::kFlagsNone, "ADSR", IParam::ShapeExp());
    GetParam(kParamSustain)->InitDouble("Sustain", 50., 0., 100., 1, "%", IParam::kFlagsNone, "ADSR", IParam::ShapeExp());
    GetParam(kParamRelease)->InitDouble("Release", 10., 2., 3000., 0.1, "ms", IParam::kFlagsNone, "ADSR", IParam::ShapeExp());
    
    // FILTER
    
    GetParam(mFilterMode)->InitEnum("Filter Mode", Filter::FILTER_MODE_LOWPASS, Filter::kNumFilterModes);
    GetParam(mAttenuationMode)->InitEnum("Attenuation", Filter::Db_6, Filter::kNumAttenuationModes, "", IParam::kFlagsNone, "", "6 dB", "12 dB", "18 dB", "24 dB");
    GetParam(mFilterCutoff)->InitDouble("Cutoff", 0.99, 0.01, 0.99, 0.001, "Fr", IParam::kFlagsNone, "", IParam::ShapeExp());
    GetParam(mFilterResonance)->InitDouble("Resonance", 0.01, 0.01, 0.98, 0.001);
    GetParam(mFilterEnvelopeAmount)->InitDouble("Filter Env Amount", 0.0, -1.0, 1.0, 0.001);
    
    GetParam(mFilterAttack)->InitDouble("Filter Env Attack", 10., 1., 3000.0, 0.1, "A", IParam::kFlagsNone, "");
    GetParam(mFilterDecay)->InitDouble("Filter Env Decay", 10, 1., 3000.0, 0.1, "D", IParam::kFlagsNone, "");
    GetParam(mFilterSustain)->InitDouble("Filter Env Sustain", 50, 0., 100.0, 0.1, "S", IParam::kFlagsNone, "", IParam::ShapeExp());
    GetParam(mFilterRelease)->InitDouble("Filter Env Release", 0.5, 0.01, 3000.0, 0.001, "R", IParam::kFlagsNone, "");
    
    MakePreset("preset", 0);
    MakePreset("preset", 1);
    MakePreset("preset", 2);
    MakePreset("preset", 3);
    MakePreset("preset", 4);
    MakePreset("preset", 5);
    MakePreset("preset", 6);
    MakePreset("preset", 7);
    MakePreset("preset", 8);
    MakePreset("preset", 9);
    MakePreset("preset", 10);
    MakePreset("preset", 11);
    MakePreset("preset", 12);
    MakePreset("preset", 13);
    MakePreset("preset", 14);
    MakePreset("preset", 15);
    
#if IPLUG_EDITOR // All UI methods and member variables should be within an IPLUG_EDITOR guard, should you want distributed UI
    mMakeGraphicsFunc = [&]() {
        return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, 1.);
    };
    
    mLayoutFunc = [&](IGraphics* pGraphics) {
        pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
        pGraphics->AttachPanelBackground(COLOR_GRAY);
        pGraphics->EnableMouseOver(true);
        pGraphics->EnableTooltips(true);
        pGraphics->AttachPopupMenuControl(DEFAULT_LABEL_TEXT);
        //    pGraphics->EnableLiveEdit(true);
        pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
        const IRECT b = pGraphics->GetBounds();
        
        const IBitmap bitmap = pGraphics->LoadBitmap(WAVES_FN, 4, false);
        pGraphics->AttachControl(new IBSwitchControl(b.GetGridCell(0, 3, 6), bitmap, kParamMode));
        pGraphics->AttachControl(new IVKnobControl(b.GetGridCell(1, 3, 6).GetPadded(-20), kParamGain), kCtrlTagGain, "vcontrols");
        
        //        sliders
        const IRECT sliders = b.GetGridCell(2, 3, 6).Union(b.GetGridCell(3, 3, 6)).GetPadded(-20);
        pGraphics->AttachControl(new IVSliderControl(sliders.GetGridCell(0, 1, 4).GetMidHPadded(30.), kParamAttack, "Attack"));
        pGraphics->AttachControl(new IVSliderControl(sliders.GetGridCell(1, 1, 4).GetMidHPadded(30.), kParamDecay, "Decay"));
        pGraphics->AttachControl(new IVSliderControl(sliders.GetGridCell(2, 1, 4).GetMidHPadded(30.), kParamSustain, "Sustain"));
        pGraphics->AttachControl(new IVSliderControl(sliders.GetGridCell(3, 1, 4).GetMidHPadded(30.), kParamRelease, "Release"));
        
        // scope
        pGraphics->AttachControl(new IVScopeControl<2, kScopeBufferSize*2>(b.GetGridCell(0, 2, 3, 3).GetPadded(-10), "Oscilloscope", DEFAULT_STYLE.WithColor(kFG, COLOR_BLACK)), kCtrlTagScope, "vcontrols");
        
        // FILTER
        const IRECT filSel = b.GetGridCell(1, 0, 3, 6);
        IBitmap bitmap2 = pGraphics->LoadBitmap(FILTERMODE_FN, 3);
        pGraphics->AttachControl(new IBSwitchControl(filSel.GetGridCell(0, 2, 1), bitmap2, mFilterMode));
        pGraphics->AttachControl(new IVSwitchControl(filSel.GetGridCell(1, 2, 1).GetPadded(-20), mAttenuationMode));
        
        pGraphics->AttachControl(new IVKnobControl(b.GetGridCell(1, 1, 3, 6), mFilterCutoff));
        pGraphics->AttachControl(new IVKnobControl(b.GetGridCell(1, 2, 3, 6), mFilterResonance));
        
        pGraphics->AttachControl(new IVKnobControl(b.GetGridCell(1, 3, 3, 6), mFilterEnvelopeAmount));
        
        const IRECT fltSld = b.GetGridCell(1, 4, 3, 6).Union(b.GetGridCell(1, 5, 3, 6)).GetPadded(-20);
        pGraphics->AttachControl(new IVSliderControl(fltSld.GetGridCell(0, 1, 4), mFilterAttack, "F-Att"));
        pGraphics->AttachControl(new IVSliderControl(fltSld.GetGridCell(1, 1, 4), mFilterDecay, "F-Dec"));
        pGraphics->AttachControl(new IVSliderControl(fltSld.GetGridCell(2, 1, 4), mFilterSustain, "F-Sus"));
        pGraphics->AttachControl(new IVSliderControl(fltSld.GetGridCell(3, 1, 4), mFilterRelease, "F-Rel"));
        
#ifdef OS_IOS
        if(!IsAuv3AppExtension())
        {
            pGraphics->AttachControl(new IVButtonControl(b.GetFromTRHC(100, 100), [pGraphics](IControl* pCaller) {
                dynamic_cast<IGraphicsIOS*>(pGraphics)->LaunchBluetoothMidiDialog(pCaller->GetRECT().L, pCaller->GetRECT().MH());
                SplashClickActionFunc(pCaller);
            }, "BTMIDI"));
        }
#endif
        
        // PRESETS
        
        IRECT lastRow = b.GetGridCell(2, 0, 3, 1).GetPadded(-10);
        
        auto button1action = [pGraphics](IControl* pCaller){
            SplashClickActionFunc(pCaller);
            //            int presetNumber = pGraphics->GetDelegate()->GetParam(kParamMode)->Int();
            dynamic_cast<IPluginBase*> (pGraphics->GetDelegate())->ModifyCurrentPreset();
        };
        
        pGraphics->AttachControl(new IVButtonControl(lastRow.GetGridCell(0, 1, 8).FracRectVertical(0.5, true).GetCentredInside(80.), button1action, "Save Preset", DEFAULT_STYLE, false), kNoTag, "vcontrols");
        pGraphics->AttachControl(new CaptionPresets(lastRow.GetGridCell(0, 1, 8).FracRectVertical(0.5, false).GetMidVPadded(10.f), kPresetsMode, IText(24.f, COLOR_BLACK), COLOR_LIGHT_GRAY, kCtrlTagBrowser, false), kCtrlTagCaption);
        
        
#ifndef OS_WEB
        pGraphics->AttachControl(new ITextControl(lastRow.GetGridCell(2, 1, 8).GetFromTop(20.f), "Preset Browser"));
        pGraphics->AttachControl(new PresetsBankBrowser(lastRow.GetGridCell(2, 1, 8).GetReducedFromTop(20.f)), kCtrlTagBrowser);
#else
        
#endif
        pGraphics->AttachControl(new IVKeyboardControl(lastRow.GetGridCell(1, 1, 2).GetPadded(-10), 2, 120), kCtrlTagKeyboard);
        pGraphics->SetQwertyMidiKeyHandlerFunc([pGraphics](const IMidiMsg& msg) {
            dynamic_cast<IVKeyboardControl*>(pGraphics->GetControlWithTag(kCtrlTagKeyboard))->SetNoteFromMidi(msg.NoteNumber(), msg.StatusMsg() == IMidiMsg::kNoteOn);
        });
    };
    
    CreatePresets();
#endif
}

void a2_MidiNoteFilterMod::CreatePresets() {
    MakePreset("clean", 0., 440.0, 1);
}

#if IPLUG_DSP
void a2_MidiNoteFilterMod::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
    mDSP.ProcessBlock(nullptr, outputs, 2, nFrames);
    mScopeSender.ProcessBlock(outputs, nFrames, kCtrlTagScope);
}

void a2_MidiNoteFilterMod::OnIdle()
{
    mScopeSender.TransmitData(*this);
}

void a2_MidiNoteFilterMod::OnReset()
{
    mDSP.Reset(GetSampleRate(), GetBlockSize());
}

void a2_MidiNoteFilterMod::ProcessMidiMsg(const IMidiMsg& msg)
{
    TRACE;
    
    int status = msg.StatusMsg();
    
    switch (status)
    {
        case IMidiMsg::kNoteOn:
        case IMidiMsg::kNoteOff:
        case IMidiMsg::kPolyAftertouch:
        case IMidiMsg::kControlChange:
        case IMidiMsg::kProgramChange:
        case IMidiMsg::kChannelAftertouch:
        case IMidiMsg::kPitchWheel:
        {
            goto handle;
        }
        default:
            return;
    }
    
handle:
    mDSP.ProcessMidiMsg(msg);
    SendMidiMsg(msg);
}

void a2_MidiNoteFilterMod::OnParamChange(int paramIdx)
{
    mDSP.SetParam(paramIdx, GetParam(paramIdx)->Value());
}

bool a2_MidiNoteFilterMod::OnMessage(int msgTag, int ctrlTag, int dataSize, const void* pData)
{
//  if(ctrlTag == kCtrlTagBender && msgTag == IWheelControl::kMessageTagSetPitchBendRange)
//  {
//    const int bendRange = *static_cast<const int*>(pData);
//    mDSP.mSynth.SetPitchBendRange(bendRange);
//  }
  
  return false;
}
#endif
