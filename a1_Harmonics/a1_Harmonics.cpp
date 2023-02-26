#include "a1_Harmonics.h"
#include "IPlug_include_in_plug_src.h"
#include "LFO.h"
#include "ECControls.hpp"

a1_Harmonics::a1_Harmonics(const InstanceInfo& info)
: Plugin(info, MakeConfig(kNumParams, kNumPresets))
{
    pFFTAnalyzer = nullptr;
#if IPLUG_EDITOR // http://bit.ly/2S64BDd
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, GetScaleForScreen(PLUG_WIDTH, PLUG_HEIGHT));
  };
  
  mLayoutFunc = [&](IGraphics* pGraphics) {
      
      pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
      pGraphics->AttachPanelBackground(COLOR_GRAY);
      pGraphics->EnableMouseOver(true);
  //    pGraphics->EnableLiveEdit(true);
      pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
      const IRECT b = pGraphics->GetBounds();
  //      pGraphics->AttachControl(new IPanelControl(b, COLOR_MID_GRAY));
        
        const IRECT scopeSection = b.GetGridCell(0, 3, 1);
        //meter, scope
//        pGraphics->AttachControl(new IVMeterControl<1>(scopeSection.GetFromLeft(40).GetReducedFromLeft(20), "a"), kCtrlTagMeter);
        pGraphics->AttachControl(new IVScopeControl<2, kScopeBufferSize*2>(scopeSection.GetGridCell(0, 1, 2).GetHPadded(-60), "oscilloscope"), kCtrlTagScope);
        // FFT
        const IRECT iView = scopeSection.GetGridCell(1, 1, 2);
        int fftSize = 8192;
        pFFTAnalyzer = new gFFTAnalyzer<>(iView.GetReducedFromRight(10), fftSize);
        pGraphics->AttachControl(pFFTAnalyzer, kCtrlTagFFT, "FFT");
        //setting +3dB/octave compensation to the fft display.  Most use +3.  Voxengo Span uses +4.5.
        dynamic_cast<gFFTAnalyzer<>*>(pFFTAnalyzer)->SetOctaveGain(3., true);
      
        //OSCILLATORS
        const IRECT sineSection = b.GetGridCell(1, 3, 1).Union(b.GetGridCell(4, 0, 6, 1));
        for (int i = 0; i<16; i++) {
            attachSineKnob(pGraphics, i, sineSection.GetGridCell(i, 1, 16).GetPadded(-4).GetVPadded(-8));
        }
        
        // LAST ROW
        const IRECT controlSection = b.GetGridCell(5, 0, 6, 1);

      IVStyle style = IVStyle(true, true, {COLOR_RED});
        pGraphics->AttachControl(new IVKnobControl(controlSection.GetGridCell(0, 1, 10), kParamVolume, "", style));
        pGraphics->AttachControl(new IVKnobControl(controlSection.GetGridCell(1, 1, 10), kParamFrequency0, "", style));
        pGraphics->AttachControl(new IVKnobControl(controlSection.GetGridCell(2, 1, 10), kParamGainMult, "", style));
        pGraphics->AttachControl(new IVKnobControl(controlSection.GetGridCell(3, 1, 10), kParamGainExponent));
        // PRESETS
      GetParam(kParamVolume)->InitDouble("Vol", 0.1, 0., 1., 0.01, "vol (%)");
      GetParam(kParamFrequency0)->InitDouble("Freq", 10., 0., 10000.0, 0.01, "freq");
      GetParam(kParamGainMult)->InitDouble("amp mult", 0.1, 0., 2.0, 0.01, " h amp");
      GetParam(kParamGainExponent)->InitDouble("amp exponent", -1., -2, 0.5, 0.001, "");
        //Preset
        GetParam(kPresetsMode)->InitEnum("Mode", 0, 16, "Preset", IParam::kFlagsNone, "", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16");
        auto button1action = [pGraphics](IControl* pCaller){
            SplashClickActionFunc(pCaller);
            //            int presetNumber = pGraphics->GetDelegate()->GetParam(kParamMode)->Int();
            dynamic_cast<IPluginBase*> (pGraphics->GetDelegate())->ModifyCurrentPreset();
        };
        
        pGraphics->AttachControl(new IVButtonControl(controlSection.GetGridCell(2, 1, 4).FracRectVertical(0.5, true).GetCentredInside(80.), button1action, "Save Preset", DEFAULT_STYLE, false), kNoTag, "vcontrols");
        pGraphics->AttachControl(new CaptionPresets(controlSection.GetGridCell(2, 1, 4).FracRectVertical(0.5, false).GetMidVPadded(10.f), kPresetsMode, IText(24.f, COLOR_BLACK), COLOR_LIGHT_GRAY, kCtrlTagBrowser, false), kCtrlTagCaption);
        
  #ifndef OS_WEB
        pGraphics->AttachControl(new ITextControl(controlSection.GetGridCell(3, 1, 4).GetFromTop(20.f), "Preset Browser"));
        pGraphics->AttachControl(new PresetsBankBrowser(controlSection.GetGridCell(3, 1, 4).GetReducedFromTop(20.f)), kCtrlTagBrowser);
  #else
        
  #endif
        
  #ifdef OS_IOS
      if(!IsAuv3AppExtension())
      {
        pGraphics->AttachControl(new IVButtonControl(b.GetFromTRHC(100, 100), [pGraphics](IControl* pCaller) {
                                 dynamic_cast<IGraphicsIOS*>(pGraphics)->LaunchBluetoothMidiDialog(pCaller->GetRECT().L, pCaller->GetRECT().MH());
                                 SplashClickActionFunc(pCaller);
                               }, "BTMIDI"));
      }
  #endif
        loaded = true;
        CreatePresets();
    };
#endif
}

void a1_Harmonics::CreatePresets() {
//    MakePreset("adding harmonics", 0);
}

void a1_Harmonics::attachSineKnob(IGraphics* pGraphics, int channel, IRECT rect) {
    const IVStyle styleButtonMuted {
        true, // Show label
        true, // Show value
        {
            DEFAULT_BGCOLOR, // Background
            DEFAULT_FGCOLOR, // Foreground
            COLOR_BLUE, // Pressed
            COLOR_BLACK, // Frame
            DEFAULT_HLCOLOR, // Highlight
            DEFAULT_SHCOLOR, // Shadow
            COLOR_BLACK, // Extra 1
            DEFAULT_X2COLOR, // Extra 2
            DEFAULT_X3COLOR  // Extra 3
        }, // Colors
        IText(12.f, EAlign::Center) // Label text
    };
    const IVStyle styleButtonSolo {
        true, // Show label
        true, // Show value
        {
            DEFAULT_BGCOLOR, // Background
            DEFAULT_FGCOLOR, // Foreground
            COLOR_RED, // Pressed
            COLOR_BLACK, // Frame
            DEFAULT_HLCOLOR, // Highlight
            DEFAULT_SHCOLOR, // Shadow
            COLOR_BLACK, // Extra 1
            DEFAULT_X2COLOR, // Extra 2
            DEFAULT_X3COLOR  // Extra 3
        }, // Colors
        IText(12.f, EAlign::Center) // Label text
    };
    const IVStyle styleButtonPhase {
        true, // Show label
        true, // Show value
        {
            DEFAULT_BGCOLOR, // Background
            DEFAULT_FGCOLOR, // Foreground
            COLOR_YELLOW, // Pressed
            COLOR_BLACK, // Frame
            DEFAULT_HLCOLOR, // Highlight
            DEFAULT_SHCOLOR, // Shadow
            COLOR_BLACK, // Extra 1
            DEFAULT_X2COLOR, // Extra 2
            DEFAULT_X3COLOR  // Extra 3
        }, // Colors
        IText(12.f, EAlign::Center) // Label text
    };
    GetParam(channel)->InitDouble("Gain", 0., 0., 1.0, 0.01, "%");
    GetParam(channel+kParamMute0)->InitBool("", false);
    GetParam(channel+kParamSolo0)->InitBool("", false);
    WDL_String str;
    IRECT soloMuted = rect.GetGridCell(0, 4, 1).GetFromBottom(40);
//    pGraphics->AttachControl(new IVToggleControl(soloMuted.GetGridCell(0, 1, 2).GetCentredInside(30), channel+kParamMute0, "", styleButtonMuted, "m", "M"), channel+kParamMute0);
    pGraphics->GetControl(pGraphics->NControls()-1)->SetActionFunction([&, pGraphics, channel](IControl* pCaller){
        SplashClickActionFunc(pCaller);
        bool val = (bool) pCaller->GetValue();
        mDSP.SetVoiceMuted(channel, val);
    });
//    pGraphics->AttachControl(new IVToggleControl(soloMuted.GetGridCell(1, 1, 2).GetCentredInside(30), channel+kParamSolo0, "", styleButtonSolo, "s", "S"), channel+kParamSolo0);
    pGraphics->GetControl(pGraphics->NControls()-1)->SetActionFunction([&, pGraphics, channel](IControl* pCaller){
        SplashClickActionFunc(pCaller);
        bool val = (bool) pCaller->GetValue();
        mDSP.SetVoiceSolo(channel, val);
    });
    if (channel<9){
        str.SetFormatted(4, "h %i",(int)channel + 1);
    }
    else { str.SetFormatted(4, "h%i",(int)channel + 1);}
//    pGraphics->AttachControl(new IVLabelControl(rect.GetGridCell(0, 4, 1).GetFromBottom(30), str.Get()));
    pGraphics->AttachControl(new IVSliderControl(rect.GetGridCell(1, 4, 1).Union(rect.GetGridCell(2, 4, 1)).GetPadded(-4).GetMidHPadded(10), channel, str.Get(), DEFAULT_STYLE), kParamChannel0+channel /* IVKnobControl(rect.GetGridCell(1, 4, 1), kParam) */);
    GetParam(kParamFrequency0+channel)->InitDouble("freq", 0, 0, 20000, 0.001);
    pGraphics->AttachControl(new IVToggleControl(rect.GetGridCell(3, 4, 1).GetGridCell(0, 2, 1).GetCentredInside(30), [&, pGraphics, channel](IControl* pCaller){
        SplashClickActionFunc(pCaller);
        bool val = (bool) pCaller->GetValue();
        mDSP.SetVoicePhase(channel, !val);
    }, "", styleButtonPhase, "Ø", "Ø"));
    pGraphics->AttachControl(new ICaptionControl(rect.GetGridCell(3, 4, 1).GetGridCell(1, 2, 1), kParamFrequency0+channel), kParamFrequency0+channel);
    
//    dynamic_cast<IVectorBase*>(pGraphics->GetControl(pGraphics->NControls()-1))->SetStyle(style);
    
}

#if IPLUG_DSP
void a1_Harmonics::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  mDSP.ProcessBlock(nullptr, outputs, 1, nFrames, mTimeInfo.mPPQPos, mTimeInfo.mTransportIsRunning);
//  mMeterSender.ProcessBlock(outputs, nFrames, kCtrlTagMeter);
    
//    mDSP.ProcessBlock(nullptr, outputs, 2, nFrames);
//      mMeterSender.ProcessBlock(outputs, nFrames);
  //    for (int i = 0; i<nFrames; i++) {
  //        mScopeSender.Process(&outputs[1][i]);
  //    }
      mScopeSender.ProcessBlock(outputs, nFrames, kCtrlTagScope);
      const int nChans = NOutChansConnected();
      mSender.ProcessBlock(outputs, nFrames, nChans);
}

void a1_Harmonics::OnIdle()
{
//  mMeterSender.TransmitData(*this);
    mScopeSender.TransmitData(*this);
    mSender.TransmitData(*this);
}

void a1_Harmonics::OnReset()
{
  mDSP.Reset(GetSampleRate(), GetBlockSize());
//  mMeterSender.Reset(GetSampleRate());
    if(pFFTAnalyzer != nullptr)
        dynamic_cast<gFFTAnalyzer<>*>(pFFTAnalyzer)->SetSampleRate(this->GetSampleRate());
}

void a1_Harmonics::ProcessMidiMsg(const IMidiMsg& msg)
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

void a1_Harmonics::OnParamChange(int paramIdx)
{
    if (loaded) {
        
        if (paramIdx == kParamGainMult || paramIdx == kParamGainExponent) {
            double g = GetParam(kParamGainMult)->Value();
            double exp = GetParam(kParamGainExponent)->Value();
            for (int i = 0; i < 16; i++) {
                GetParam(i+kParamChannel0)->Set(pow(g*i+1, exp));
                double normalized = GetParam(i+kParamChannel0)->GetNormalized();
                GetUI()->GetControlWithTag(i+kParamChannel0)->SetValue(normalized, 0);
                GetUI()->GetControlWithTag(i+kParamChannel0)->SetDirty(true, 0);
            }
        }
        else if (paramIdx == kParamFrequency0) {
            for (int i = 1; i < 16; i++) {
                GetParam(i+kParamFrequency0)->Set(GetParam(paramIdx)->Value() * (i + 1.));
              
                double normalized = GetParam(i+kParamFrequency0)->GetNormalized();
                GetUI()->GetControlWithTag(i+kParamFrequency0)->SetValue(normalized, 0);
                GetUI()->GetControlWithTag(i+kParamFrequency0)->SetDirty(false, 0);
            }
        }
        else if (paramIdx == kPresetsMode) {
            if (gateForPresets == false) {
                gateForPresets = true;
                RestorePreset(GetParam(paramIdx)->Int());
//                SetCurrentPresetIdx(GetParam(paramIdx)->Int());
                GetUI()->GetControlWithTag(kCtrlTagBrowser)->SetDirty();
                gateForPresets = false;
            }
        }
        else if (paramIdx >= kParamMute0 && paramIdx <= kParamSolo15) {
//            IAnimationFunction getActionFunction() = static_cast<IVToggleControl*>(GetUI()->GetControlWithTag(paramIdx))->GetActionFunction();
//            getActionFunction(*this, GetUI(), paramIdx);
        }
        
        mDSP.SetParam(paramIdx, GetParam(paramIdx)->Value());
    }
}

bool a1_Harmonics::OnMessage(int msgTag, int ctrlTag, int dataSize, const void* pData)
{
//  if(ctrlTag == kCtrlTagBender && msgTag == IWheelControl::kMessageTagSetPitchBendRange)
//  {
//    const int bendRange = *static_cast<const int*>(pData);
//    mDSP.mSynth.SetPitchBendRange(bendRange);
//  }
  
  return false;
}
#endif
