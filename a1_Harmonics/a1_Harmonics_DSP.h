#pragma once

#include "MidiSynth.h"
#include "ECOscillator.h"
#include "ADSREnvelope.h"
#include "Smoothers.h"
#include "MixerUtilities.hpp"

using namespace iplug;
using namespace igraphics;

enum EModulations
{
  kModGainSmoother = 0,
  kModSustainSmoother,
    kNumModulations,
};

template<typename T>
class a1_HarmonicsDSP
{
public:
#pragma mark - Voice
  class Voice : public SynthVoice
  {
  public:
    Voice()
    : mAMPEnv("gain", [&](){ mOSC.Reset(); }) // capture ok on RT thread?
    {
//      DBGMSG("new Voice: %i control inputs.\n", static_cast<int>(mInputs.size()));
    }

    bool GetBusy() const override
    {
      return mAMPEnv.GetBusy();
    }

    void Trigger(double level, bool isRetrigger) override
    {
      mOSC.Reset();
      
      if(isRetrigger)
        mAMPEnv.Retrigger(level);
      else
        mAMPEnv.Start(level);
    }
    
    void Release() override
    {
      mAMPEnv.Release();
    }

    void ProcessSamplesAccumulating(T** inputs, T** outputs, int nInputs, int nOutputs, int startIdx, int nFrames) override
      {
        // inputs to the synthesizer can just fetch a value every block, like this:
  //      double gate = mInputs[kVoiceControlGate].endValue;
//        double pitch = mInputs[kVoiceControlPitch].endValue;
//        double pitchBend = mInputs[kVoiceControlPitchBend].endValue;

        // or write the entire control ramp to a buffer, like this, to get sample-accurate ramps:
  //      mInputs[kVoiceControlTimbre].Write(mTimbreBuffer, startIdx, nFrames);

        // convert from "1v/oct" pitch space to frequency in Hertz
  //      double osc1Freq = 440. * pow(2., pitch + pitchBend);
          mOSC.setFrequency(osc1Freq);
        // make sound output for each output channel
        for(auto i = startIdx; i < startIdx + nFrames; i++)
        {
          // an MPE synth can use pressure here in addition to gain
          outputs[0][i] += mOSC.nextSample() * mGain * phase;
          outputs[1][i] = outputs[0][i];
        }
      }

    void SetSampleRateAndBlockSize(double sampleRate, int blockSize) override
    {
      mOSC.setSampleRate(sampleRate);
      mAMPEnv.SetSampleRate(sampleRate);
      
//      mTimbreBuffer.Resize(blockSize);
    }

    void SetProgramNumber(int pgm) override
    {
      //TODO:
    }

    // this is called by the VoiceAllocator to set generic control values.
    void SetControl(int controlNumber, float value) override
    {
      //TODO:
    }

      void SetAmp(double a) {
          mGain = a;
      }
      
      void SetMuted(bool m) {
          muted = m;
          if (!muted) {
              mOSC.Reset();
          }
      }
      
      void SetPhase(bool p) {
          phase = p?1.:-1.;
      }
      
  public:
      double osc1Freq = 0;
      Oscillator mOSC;// FastSinOscillator<T> mOSC;
    ADSREnvelope<T> mAMPEnv;

  private:
      bool muted;
      double phase = 1;
    // would be allocated dynamically in a real example
    static constexpr int kMaxBlockSize = 1024;

  };

public:
#pragma mark -
  a1_HarmonicsDSP(int nVoices)
  {
    for (auto i = 0; i < nVoices; i++)
    {
      // add a voice to Zone 0.
      mSynth.AddVoice(new Voice(), 0);
    }

    // some MidiSynth API examples:
    // mSynth.SetKeyToPitchFn([](int k){return (k - 69.)/24.;}); // quarter-tone scale
    // mSynth.SetNoteGlideTime(0.5); // portamento
  }

  void ProcessBlock(T** inputs, T** outputs, int nOutputs, int nFrames, double qnPos = 0., bool transportIsRunning = false, double tempo = 120.)
    {
      // clear outputs
      for(auto i = 0; i < nOutputs; i++)
      {
        memset(outputs[i], 0, nFrames * sizeof(T));
      }
      
      mParamSmoother.ProcessBlock(mParamsToSmooth, mModulations.GetList(), nFrames);
      mSynth.ProcessBlock(mModulations.GetList(), outputs, 0, nOutputs, nFrames);
      for(int s=0; s < nFrames;s++)
      {
        T smoothedGain = mModulations.GetList()[kModGainSmoother][s];
        outputs[0][s] *= smoothedGain;
        outputs[1][s] *= smoothedGain;
      }
    }

  void Reset(double sampleRate, int blockSize)
  {
    mSynth.SetSampleRateAndBlockSize(sampleRate, blockSize);
    mSynth.Reset();
   
    mModulationsData.Resize(blockSize * kNumModulations);
    mModulations.Empty();
    
    for(int i = 0; i < kNumModulations; i++)
    {
      mModulations.Add(mModulationsData.Get() + (blockSize * i));
    }
  }

  void ProcessMidiMsg(const IMidiMsg& msg)
  {
    mSynth.AddMidiMsgToQueue(msg);
  }

  void SetParam(int paramIdx, double value)
    {
      using EEnvStage = ADSREnvelope<sample>::EStage;
      
      switch (paramIdx) {
          case kParamChannel0:
          case kParamChannel1:
          case kParamChannel2:
          case kParamChannel3:
          case kParamChannel4:
          case kParamChannel5:
          case kParamChannel6:
          case kParamChannel7:
          case kParamChannel8:
          case kParamChannel9:
          case kParamChannel10:
          case kParamChannel11:
          case kParamChannel12:
          case kParamChannel13:
          case kParamChannel14:
          case kParamChannel15:
          {
              dynamic_cast<a1_HarmonicsDSP::Voice*>(mSynth.GetVoice( paramIdx - kParamChannel0))->SetAmp(value);
          }
              break;
          case kParamVolume:
          {
              mSynth.SetVoicesActive(value > 0.0001);
              mParamsToSmooth[kModGainSmoother] = (T) value ;
          }
              break;
          case kParamFrequency0:
              for (int i = 0; i < 16; i++) {
  //                plugin->GetParam(i+kParamFrequency0)->Set(value * (i + 1.));
  //                dynamic_cast<IGEditorDelegate*>(plugin)->GetUI()->GetControlWithTag(i+kParamFrequency0)->SetDirty();
                  
  //                iGraphics->GetControlWithTag(i+kParamFrequency0)->SetValueFromUserInput(value * (i + 1.));
                  dynamic_cast<a1_HarmonicsDSP::Voice*>(mSynth.GetVoice(i))->osc1Freq = value * (i + 1.);
                  dynamic_cast<a1_HarmonicsDSP::Voice*>(mSynth.GetVoice(i))->mOSC.Reset();
              }
              break;
          case kParamFrequency1:
          case kParamFrequency2:
          case kParamFrequency3:
          case kParamFrequency4:
          case kParamFrequency5:
          case kParamFrequency6:
          case kParamFrequency7:
          case kParamFrequency8:
          case kParamFrequency9:
          case kParamFrequency10:
          case kParamFrequency11:
          case kParamFrequency12:
          case kParamFrequency13:
          case kParamFrequency14:
          case kParamFrequency15:
              dynamic_cast<a1_HarmonicsDSP::Voice*>(mSynth.GetVoice(paramIdx - kParamFrequency0))->osc1Freq = value;
              
  //      case kParamNoteGlideTime:
  //        mSynth.SetNoteGlideTime(value / 1000.);
  //        break;
  //      case kParamGain:
  //        mParamsToSmooth[kModGainSmoother] = (T) value / 100.;
  //        break;
  //      case kParamSustain:
  //        mParamsToSmooth[kModSustainSmoother] = (T) value / 100.;
  //        break;
  //      case kParamAttack:
  //      case kParamDecay:
  //      case kParamRelease:
  //      {
  //        EEnvStage stage = static_cast<EEnvStage>(EEnvStage::kAttack + (paramIdx - kParamAttack));
  //        mSynth.ForEachVoice([stage, value](SynthVoice& voice) {
  //          dynamic_cast<a1_HarmonicsDSP::Voice&>(voice).mAMPEnv.SetStageTime(stage, value);
  //        });
  //        break;
  //      }
        default:
          break;
      }
    }
    
    void SetVoiceSolo(int indx, bool solo) {
        soloMutedUtility.SetSolo(indx, solo);
        for (int i = 0;i<16;i++) {
            dynamic_cast<a1_HarmonicsDSP::Voice*>(mSynth.GetVoice(i))->SetMuted(!soloMutedUtility.activeChannel(i));
        }
    }
    
    void SetVoiceMuted(int indx, bool muted) {
        soloMutedUtility.SetMuted(indx, muted);
        for (int i = 0;i<16;i++) {
            dynamic_cast<a1_HarmonicsDSP::Voice*>(mSynth.GetVoice(i))->SetMuted(!soloMutedUtility.activeChannel(i));
        }
    }
    
    void SetVoicePhase(int indx, bool inPhase) {
        dynamic_cast<a1_HarmonicsDSP::Voice*>(mSynth.GetVoice(indx))->SetPhase(inPhase);
    }

  
public:
  MidiSynth mSynth { VoiceAllocator::kPolyModePoly, MidiSynth::kDefaultBlockSize };
  WDL_TypedBuf<T> mModulationsData; // Sample data for global modulations (e.g. smoothed sustain)
  WDL_PtrList<T> mModulations; // Ptrlist for global modulations
  LogParamSmooth<T, kNumModulations> mParamSmoother;
  sample mParamsToSmooth[kNumModulations];
private:
    SoloMute16 soloMutedUtility;
};
