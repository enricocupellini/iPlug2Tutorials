//
//  Oscillator.hpp
//  App
//
//  Created by Enrico Cupellini on 07/07/2019.
//

#ifndef ECOscillator_hpp
#define ECOscillator_hpp

#include <stdio.h>

#include <math.h>

class Oscillator {

public:
    enum OscillatorMode {
        OSCILLATOR_MODE_SINE,
        OSCILLATOR_MODE_SAW,
        OSCILLATOR_MODE_SQUARE,
        OSCILLATOR_MODE_TRIANGLE
    };
    void setMode(OscillatorMode mode);
    void setFrequency(double frequency);
    void setSampleRate(double sampleRate);
    void generate(double* buffer, int nFrames);
    inline void setMuted(bool muted) { isMuted = muted; }
    double nextSample();
    void Reset(){mPhase = 0.;}
    Oscillator() :
    mOscillatorMode(OSCILLATOR_MODE_SINE),
    mPI(2*acos(0.0)),
    twoPI(2 * mPI),
    isMuted(false),
    mFrequency(440.0),
    mPhase(0.0),
    mSampleRate(44100.0) { updateIncrement(); };
protected:
    OscillatorMode mOscillatorMode;
    const double mPI;
    const double twoPI;
    bool isMuted;
    double mFrequency;
    double mPhase;
    double mSampleRate;
    double mPhaseIncrement;
    void updateIncrement();
    double naiveWaveformForMode(OscillatorMode mode);
};


class PolyBLEPOscillator: public Oscillator {
public:
    PolyBLEPOscillator() : lastOutput(0.0) { updateIncrement(); };
    double nextSample();
private:
    double poly_blep(double t);
    double lastOutput;
};

class PolyBLAMPOscillator: public Oscillator {
public:
    PolyBLAMPOscillator() : lastOutput(0.0) { updateIncrement(); };
    double nextSample();
private:
    void poly_blamp(double& value);
    double lastOutput;
    double lastOutput_1;
};

#endif /* ECOscillator_hpp */
