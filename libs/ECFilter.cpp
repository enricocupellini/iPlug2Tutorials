//
//  ECFilter.cpp
//  a1_Filter-macOS
//
//  Created by Enrico Cupellini on 14/08/2019.
//

#include "ECFilter.hpp"

// By Paul Kellett
// http://www.musicdsp.org/showone.php?id=29
// http://www.musicdsp.org/en/latest/Filters/29-resonant-filter.html
//two first-order lowpass filters in series. First-order means that for every octave above the cutoff frequency, the amplitude is halved (i.e. the volume is reduced by 6dB)

double Filter::process(double inputValue) {
//    buf0 += cutoff * (inputValue - buf0);
    if (inputValue == 0.0) return inputValue;
    double calculatedCutoff = getCalculatedCutoff();
    buf0 += calculatedCutoff * (inputValue - buf0 + feedbackAmount * (buf0 - buf1));
    buf1 += calculatedCutoff * (buf0 - buf1);
    buf2 += calculatedCutoff * (buf1 - buf2);
    buf3 += calculatedCutoff * (buf2 - buf3);
    switch (attenuation) {
        case Db_6:
            return process6(inputValue);
            break;
        case Db_12:
            return process12(inputValue);
            break;
        case Db_18:
            return process18(inputValue);
            break;
        case Db_24:
            return process24(inputValue);
            break;
        default:
            return 0.;
            break;
    }
}

double Filter::process6(double inputValue) {
//    buf0 += cutoff * (inputValue - buf0 + feedbackAmount * (buf0 - buf1));
//    buf1 += cutoff * (buf0 - buf1);
//    buf2 += cutoff * (buf1 - buf2);
//    buf3 += cutoff * (buf2 - buf3);
    
    switch (mode) {
        case FILTER_MODE_LOWPASS:
            return buf0;
        case FILTER_MODE_HIGHPASS:
            return inputValue - buf0;
        case FILTER_MODE_BANDPASS:
            return inputValue - buf0 - buf0;  // NON E' CORRETTO, IMPLEMENTAZIONE SBAGLIATA
        default:
            return 0.0;
    }
}
double Filter::process12(double inputValue) {
//    buf0 += cutoff * (inputValue - buf0 + feedbackAmount * (buf0 - buf1));
//    buf1 += cutoff * (buf0 - buf1);
//    buf2 += cutoff * (buf1 - buf2);
//    buf3 += cutoff * (buf2 - buf3);
    switch (mode) {
        case FILTER_MODE_LOWPASS:
            return buf1;
        case FILTER_MODE_HIGHPASS:
            return inputValue - buf1;
        case FILTER_MODE_BANDPASS:
            return buf0 - buf1;
        default:
            return 0.0;
    }
}
double Filter::process18(double inputValue) {
//    buf0 += cutoff * (inputValue - buf0 + feedbackAmount * (buf0 - buf1));
//    buf1 += cutoff * (buf0 - buf1);
//    buf2 += cutoff * (buf1 - buf2);
//    buf3 += cutoff * (buf2 - buf3);
    switch (mode) {
        case FILTER_MODE_LOWPASS:
            return buf2;
        case FILTER_MODE_HIGHPASS:
            return inputValue - buf2;
        case FILTER_MODE_BANDPASS:
            return buf0 - buf2;
        default:
            return 0.0;
    }
}
double Filter::process24(double inputValue) {
//    buf0 += cutoff * (inputValue - buf0 + feedbackAmount * (buf0 - buf1));
//    buf1 += cutoff * (buf0 - buf1);
//    buf2 += cutoff * (buf1 - buf2);
//    buf3 += cutoff * (buf2 - buf3);
    switch (mode) {
        case FILTER_MODE_LOWPASS:
            return buf3;
        case FILTER_MODE_HIGHPASS:
            return inputValue - buf3;
        case FILTER_MODE_BANDPASS:
            return buf0 - buf3;
        default:
            return 0.0;
    }
}

#pragma mark * * * * * * * * FILTERBUFF * * * * * * * * *

void FilterBufferedStereo::processBuff(unsigned int sigEnum, double* modulation, double **inputs, double **outputs, int nFrames) {
    //    buf0 += cutoff * (inputValue - buf0);
    if (sigEnum == 0.0) return;
    
    switch (attenuation) {
        case Db_6:
            process6(modulation, inputs, outputs, nFrames);
            break;
        case Db_12:
            process12(modulation, inputs, outputs, nFrames);
            break;
        case Db_18:
            process18(modulation, inputs, outputs, nFrames);
            break;
        case Db_24:
            process24(modulation, inputs, outputs, nFrames);
            break;
        default:
            break;
    }
}

void FilterBufferedStereo::process6(double* modulation, double **inputs, double **outputs, int nFrames) {
    
    switch (mode) {
        case FILTER_MODE_LOWPASS:
            for (int i = 0; i<nFrames; i++) {
                setCutoffMod(modulation[i]);
                calculateBuf(inputs[0][i], inputs[1][i]);
                outputs[0][i] = buf0[0];
                outputs[1][i] = buf0[1];
            }
            break;
        case FILTER_MODE_HIGHPASS:
            for (int i = 0; i<nFrames; i++) {
                setCutoffMod(modulation[i]);
                calculateBuf(inputs[0][i], inputs[1][i]);;
                outputs[0][i] = inputs[0][i] - buf0[0];
                outputs[1][i] = inputs[1][i] - buf0[1];
            }
            break;
        case FILTER_MODE_BANDPASS:
            for (int i = 0; i<nFrames; i++) {
                setCutoffMod(modulation[i]);
                calculateBuf(inputs[0][i], inputs[1][i]);
                outputs[0][i] = inputs[0][i] - buf0[0] - buf0[0];  // NON E' CORRETTO, IMPLEMENTAZIONE SBAGLIATA
                outputs[1][i] = inputs[1][i] - buf0[1] - buf0[1];  // NON E' CORRETTO, IMPLEMENTAZIONE SBAGLIATA
            }
            break;
        default:
            break;
    }
}

void FilterBufferedStereo::process12(double* modulation, double **inputs, double **outputs, int nFrames) {
    switch (mode) {
        case FILTER_MODE_LOWPASS:
            for (int i = 0; i<nFrames; i++) {
                setCutoffMod(modulation[i]);
                calculateBuf(inputs[0][i], inputs[1][i]);
                outputs[0][i] = buf1[0];
                outputs[1][i] = buf1[1];
            }
            break;
        case FILTER_MODE_HIGHPASS:
            for (int i = 0; i<nFrames; i++) {
                setCutoffMod(modulation[i]);
                calculateBuf(inputs[0][i], inputs[1][i]);;
                outputs[0][i] = inputs[0][i] - buf1[0];
                outputs[1][i] = inputs[1][i] - buf1[1];
            }
            break;
        case FILTER_MODE_BANDPASS:
            for (int i = 0; i<nFrames; i++) {
                setCutoffMod(modulation[i]);
                calculateBuf(inputs[0][i], inputs[1][i]);
                outputs[0][i] = buf0[0] - buf1[0];
                outputs[1][i] = buf0[1] - buf1[1];
            }
            break;
        default:
            break;
    }
}
void FilterBufferedStereo::process18(double* modulation, double **inputs, double **outputs, int nFrames) {
    switch (mode){
        case FILTER_MODE_LOWPASS:
            for (int i = 0; i<nFrames; i++) {
                setCutoffMod(modulation[i]);
                calculateBuf(inputs[0][i], inputs[1][i]);
                outputs[0][i] = buf2[0];
                outputs[1][i] = buf2[1];
            }
            break;
        case FILTER_MODE_HIGHPASS:
            for (int i = 0; i<nFrames; i++) {
                setCutoffMod(modulation[i]);
                calculateBuf(inputs[0][i], inputs[1][i]);;
                outputs[0][i] = inputs[0][i] - buf2[0];
                outputs[1][i] = inputs[1][i] - buf2[1];
            }
            break;
        case FILTER_MODE_BANDPASS:
            for (int i = 0; i<nFrames; i++) {
                setCutoffMod(modulation[i]);
                calculateBuf(inputs[0][i], inputs[1][i]);
                outputs[0][i] = buf0[0] - buf2[0];
                outputs[1][i] = buf0[1] - buf2[1];
            }
            break;
        default:
            break;
    }
}
void FilterBufferedStereo::process24(double* modulation, double **inputs, double **outputs, int nFrames) {
    switch (mode){
        case FILTER_MODE_LOWPASS:
            for (int i = 0; i<nFrames; i++) {
                setCutoffMod(modulation[i]);
                calculateBuf(inputs[0][i], inputs[1][i]);
                outputs[0][i] = buf3[0];
                outputs[1][i] = buf3[1];
            }
            break;
        case FILTER_MODE_HIGHPASS:
            for (int i = 0; i<nFrames; i++) {
                setCutoffMod(modulation[i]);
                calculateBuf(inputs[0][i], inputs[1][i]);;
                outputs[0][i] = inputs[0][i] - buf3[0];
                outputs[1][i] = inputs[1][i] - buf3[1];
            }
            break;
        case FILTER_MODE_BANDPASS:
            for (int i = 0; i<nFrames; i++) {
                setCutoffMod(modulation[i]);
                calculateBuf(inputs[0][i], inputs[1][i]);
                outputs[0][i] = buf0[0] - buf3[0];
                outputs[1][i] = buf0[1] - buf3[1];
            }
            break;
        default:
            break;
    }
}
