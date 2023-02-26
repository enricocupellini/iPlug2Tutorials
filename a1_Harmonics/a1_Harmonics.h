#pragma once

#include "IPlug_include_in_plug_hdr.h"
#include "IControls.h"
#include "IVMeterControl.h"
#include "IVScopeControl.h"
#include "FFTRect.h"

const int kNumPresets = 16;

enum EParams
{
    kParamChannel0 = 0,
    kParamChannel1,
    kParamChannel2,
    kParamChannel3,
    kParamChannel4,
    kParamChannel5,
    kParamChannel6,
    kParamChannel7,
    kParamChannel8,
    kParamChannel9,
    kParamChannel10,
    kParamChannel11,
    kParamChannel12,
    kParamChannel13,
    kParamChannel14,
    kParamChannel15,
    kParamFundFreq,
    kParamFrequency0,
    kParamFrequency1,
    kParamFrequency2,
    kParamFrequency3,
    kParamFrequency4, // 20
    kParamFrequency5,
    kParamFrequency6,
    kParamFrequency7,
    kParamFrequency8,
    kParamFrequency9,
    kParamFrequency10,
    kParamFrequency11,
    kParamFrequency12,
    kParamFrequency13,
    kParamFrequency14,
    kParamFrequency15,
    kParamMute0,
    kParamMute1,
    kParamMute2,
    kParamMute3,
    kParamMute4, // 36
    kParamMute5,
    kParamMute6,
    kParamMute7,
    kParamMute8,
    kParamMute9,
    kParamMute10,
    kParamMute11,
    kParamMute12,
    kParamMute13,
    kParamMute14,
    kParamMute15,
    kParamSolo0,
    kParamSolo1,
    kParamSolo2, // 50
    kParamSolo3,
    kParamSolo4,
    kParamSolo5,
    kParamSolo6,
    kParamSolo7,
    kParamSolo8,
    kParamSolo9,
    kParamSolo10,
    kParamSolo11,
    kParamSolo12,
    kParamSolo13,
    kParamSolo14,
    kParamSolo15,
    kParamGainMult, // 64
    kParamGainExponent,
    kParamVolume,
    kPresetsMode,
  kNumParams
};

#if IPLUG_DSP
// will use EParams in a1_Harmonics_DSP.h
#include "a1_Harmonics_DSP.h"
#endif

enum EControlTags
{
  kCtrlTagMeter = 1000,
    kCtrlTagScope,
  kCtrlTagFFT,
    kCtrlTagBrowser,
    kCtrlTagCaption,
  kNumCtrlTags
};

using namespace iplug;
using namespace igraphics;

class a1_Harmonics final : public Plugin
{
public:
  a1_Harmonics(const InstanceInfo& info);

#if IPLUG_DSP // http://bit.ly/2S64BDd
public:
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
  void ProcessMidiMsg(const IMidiMsg& msg) override;
  void OnReset() override;
  void OnParamChange(int paramIdx) override;
  void OnIdle() override;
  bool OnMessage(int msgTag, int ctrlTag, int dataSize, const void* pData) override;
    void CreatePresets();

private:
    bool loaded = false;
    bool gateForPresets = false;
    void attachSineKnob(IGraphics* graphics, int kParam, IRECT rect);
  a1_HarmonicsDSP<sample> mDSP {16};
    IPeakSender<2> mMeterSender;
    static constexpr int kScopeBufferSize = 128;
    IBufferSender<2, kScopeBufferSize, kScopeBufferSize*2> mScopeSender;
    // FFT
    gFFTAnalyzer<>* pFFTAnalyzer;
    gFFTAnalyzer<>::Sender mSender{ kCtrlTagFFT };
#endif
};
