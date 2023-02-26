#pragma once

#include "IPlug_include_in_plug_hdr.h"
#include "IControls.h"

const int kNumPresets = 16;

enum EParams
{
    kPresetsMode = 0,
    kParamGain,
    kParamMode,
    kParamNoteGlideTime,
    kParamAttack,
    kParamDecay,
    kParamSustain,
    kParamRelease,
    
    mFilterMode,
    mAttenuationMode,
    mFilterCutoff,
    mFilterResonance,
    mFilterAttack,
    mFilterDecay,
    mFilterSustain,
    mFilterRelease,
    mFilterEnvelopeAmount,
    
    kNumParams
};

#if IPLUG_DSP
// will use EParams in a2_MidiNoteFilterMod_DSP.h
#include "a2_MidiNoteFilterMod_DSP.h"
#endif

enum EControlTags
{
    kNoCtrltag = 0,
    kCtrlTagGain,
    kCtrlTagScope,
    kCtrlTagRadioButton,
    kCtrlTagKeyboard,
    kCtrlTagCaption,
    kCtrlTagBrowser,
    kNumCtrlTags
};

using namespace iplug;
using namespace igraphics;

class a2_MidiNoteFilterMod final : public Plugin
{
public:
  a2_MidiNoteFilterMod(const InstanceInfo& info);

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
    a2_MidiNoteFilterModDSP<sample> mDSP {16};
    static constexpr int kScopeBufferSize = 128;
    IBufferSender<2, kScopeBufferSize, kScopeBufferSize*2> mScopeSender;
#endif
};
