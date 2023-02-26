//
//  Oscillator.cpp
//  App
//
//  Created by Enrico Cupellini on 07/07/2019.
//

#include "ECOscillator.h"

void Oscillator::setMode(OscillatorMode mode) {
    mOscillatorMode = mode;
}

void Oscillator::setFrequency(double frequency) {
    mFrequency = frequency;
    updateIncrement();
}

void Oscillator::setSampleRate(double sampleRate) {
    mSampleRate = sampleRate;
    updateIncrement();
}

void Oscillator::updateIncrement() {
    mPhaseIncrement = mFrequency * 2 * mPI / mSampleRate;
}


void Oscillator::generate(double* buffer, int nFrames) {
//    const double twoPI = 2 * mPI;
    switch (mOscillatorMode) {
        case OSCILLATOR_MODE_SINE:
            for (int i = 0; i < nFrames; i++) {
                buffer[i] = sin(mPhase);
                mPhase += mPhaseIncrement;
                while (mPhase >= twoPI) {
                    mPhase -= twoPI;
                }
            }
            break;
        case OSCILLATOR_MODE_SAW:
            for (int i = 0; i < nFrames; i++) {
                buffer[i] = 1.0 - (2.0 * mPhase / twoPI);
                mPhase += mPhaseIncrement;
                while (mPhase >= twoPI) {
                    mPhase -= twoPI;
                }
            }
            break;
        case OSCILLATOR_MODE_SQUARE:
            for (int i = 0; i < nFrames; i++) {
                if (mPhase <= mPI) {
                    buffer[i] = 1.0;
                } else {
                    buffer[i] = -1.0;
                }
                mPhase += mPhaseIncrement;
                while (mPhase >= twoPI) {
                    mPhase -= twoPI;
                }
            }
            break;
        case OSCILLATOR_MODE_TRIANGLE:
            for (int i = 0; i < nFrames; i++) {
                double value = -1.0 + (2.0 * mPhase / twoPI);
                buffer[i] = 2.0 * (fabs(value) - 0.5);
                mPhase += mPhaseIncrement;
                while (mPhase >= twoPI) {
                    mPhase -= twoPI;
                }
            }
            break;
    }
}

double Oscillator::nextSample() {
    double value = naiveWaveformForMode(mOscillatorMode);
    mPhase += mPhaseIncrement;
    while (mPhase >= twoPI) {
        mPhase -= twoPI;
    }
    return value;
}

double Oscillator::naiveWaveformForMode(OscillatorMode mode) {
    double value;
    switch (mode) {
        case OSCILLATOR_MODE_SINE:
            value = sin(mPhase);
            break;
        case OSCILLATOR_MODE_SAW:
            value = (2.0 * mPhase / twoPI) - 1.0;
            break;
        case OSCILLATOR_MODE_SQUARE:
            if (mPhase < mPI) {
                value = 1.0;
            } else {
                value = -1.0;
            }
            break;
        case OSCILLATOR_MODE_TRIANGLE:
            value = -1.0 + (2.0 * mPhase / twoPI);
            value = 2.0 * (fabs(value) - 0.5);
            break;
        default:
            break;
    }
    return value;
}


// * * * * * * * * * * * * * * POLYBLEP Oscillator * * * * * * * * * * * * * *

// PolyBLEP by Tale
// (slightly modified)
// http://www.kvraudio.com/forum/viewtopic.php?t=375517
double PolyBLEPOscillator::poly_blep(double t)
{
    double dt = mPhaseIncrement / twoPI;
    // 0 <= t < 1
    if (t < dt) {
        t /= dt;
        return t+t - t*t - 1.0;
    }
    // -1 < t < 0
    else if (t > 1.0 - dt) {
        t = (t - 1.0) / dt;
        return t*t + t+t + 1.0;
    }
    // 0 otherwise
    else return 0.0;
}

double PolyBLEPOscillator::nextSample() {
    double value = 0.0;
    double t = mPhase / twoPI;
    
    if (mOscillatorMode == OSCILLATOR_MODE_SINE) {
        value = naiveWaveformForMode(OSCILLATOR_MODE_SINE);
    } else if (mOscillatorMode == OSCILLATOR_MODE_SAW) {
        value = naiveWaveformForMode(OSCILLATOR_MODE_SAW);
        value -= poly_blep(t);
    } else {
        value = naiveWaveformForMode(OSCILLATOR_MODE_SQUARE);
        value += poly_blep(t);
        value -= poly_blep(fmod(t + 0.5, 1.0));
        if (mOscillatorMode == OSCILLATOR_MODE_TRIANGLE) {
            // Leaky integrator: y[n] = A * x[n] + (1 - A) * y[n-1]
            value = mPhaseIncrement * value + (1 - mPhaseIncrement) * lastOutput;
            lastOutput = value;
        }
    }
    
    mPhase += mPhaseIncrement;
    while (mPhase >= twoPI) {
        mPhase -= twoPI;
    }
    return value;
}


// * * * * * * * * * * * * * * PolyBLAMPOscillator Oscillator * * * * * * * * * * * * * *

// ispirato da http://www.dafx17.eca.ed.ac.uk/papers/DAFx17_paper_100.pdf
// ma implementato io

void PolyBLAMPOscillator::poly_blamp(double& value)
{
    double diff = (value - lastOutput) - (lastOutput - lastOutput_1);
    if (diff * diff > 0.2) {
        value = (value + lastOutput + lastOutput_1) / 3.;
    }
    lastOutput_1 = lastOutput;
    lastOutput = value;
}

double PolyBLAMPOscillator::nextSample() {
    double value = 0.0;
    
    if (mOscillatorMode == OSCILLATOR_MODE_SINE) {
        value = naiveWaveformForMode(OSCILLATOR_MODE_SINE);
    } else {
        value = naiveWaveformForMode(mOscillatorMode);
        poly_blamp(value);
    }
    
    mPhase += mPhaseIncrement;
    while (mPhase >= twoPI) {
        mPhase -= twoPI;
    }
    return value;
}
