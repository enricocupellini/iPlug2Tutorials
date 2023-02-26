//
//  ECFilter.hpp
//  a1_Filter-macOS
//
//  Created by Enrico Cupellini on 14/08/2019.
//

#ifndef ECFilter_hpp
#define ECFilter_hpp

#include <cmath>

class Filter {
public:
    enum FilterMode {
        FILTER_MODE_LOWPASS = 0,
        FILTER_MODE_HIGHPASS,
        FILTER_MODE_BANDPASS,
        kNumFilterModes
    };
    
    enum FilterAttenuation {
        Db_6 = 0,
        Db_12,
        Db_18,
        Db_24,
        kNumAttenuationModes
    };
    
    Filter() :
    cutoff(0.99),
    resonance(0.0),
    cutoffMod(0.0),             // calculated cutoff is the sum of cutoff and cutoffMod
    mode(FILTER_MODE_LOWPASS),
    attenuation(Db_6),
    buf0(0.0),
    buf1(0.0),
    buf2(0.0),
    buf3(0.0)
    {
        calculateFeedbackAmount();
    };
    double process(double inputValue);
    inline void setCutoff(double newCutoff) { cutoff = newCutoff; calculateFeedbackAmount(); };
    inline void setResonance(double newResonance) { resonance = newResonance; calculateFeedbackAmount(); };
    inline void setFilterMode(FilterMode newMode) { mode = newMode; }
    inline void setAttenuation(FilterAttenuation newAttenuation) { attenuation = newAttenuation; }
    inline double getCalculatedCutoff() const {
        return fmax(fmin(cutoff + cutoffMod, 0.99), 0.01);
    };
    inline void setCutoffMod(double newCutoffMod) {
        cutoffMod = newCutoffMod;
        calculateFeedbackAmount();
    }
private:
    double cutoff;
    double cutoffMod;
    double resonance;
    FilterMode mode;
    FilterAttenuation attenuation;
    double feedbackAmount;
//    inline void calculateFeedbackAmount() { feedbackAmount = resonance + resonance/(1.0 - cutoff); }
    inline void calculateFeedbackAmount() {
        feedbackAmount = resonance + resonance/(1.0 - getCalculatedCutoff());
    }
    double buf0;
    double buf1;
    double buf2;
    double buf3;
    
    double process6(double inputValue);
    double process12(double inputValue);
    double process18(double inputValue);
    double process24(double inputValue);
};

#pragma mark * * * * * * * * FILTERBUFF * * * * * * * * *

class FilterBufferedStereo {
public:
    enum FilterMode {
        FILTER_MODE_LOWPASS = 0,
        FILTER_MODE_HIGHPASS,
        FILTER_MODE_BANDPASS,
        kNumFilterModes
    };
    
    enum FilterAttenuation {
        Db_6 = 0,
        Db_12,
        Db_18,
        Db_24,
        kNumAttenuationModes
    };
    
    FilterBufferedStereo() :
    cutoff(0.99),
    resonance(0.0),
    cutoffMod(0.0),             // calculated cutoff is the sum of cutoff and cutoffMod
    mode(FILTER_MODE_LOWPASS),
    attenuation(Db_6)
    {
        buf0[0] = 0.0;
        buf0[1] = 0.0;
        buf1[0] = 0.0;
        buf1[1] = 0.0;
        buf2[0] = 0.0;
        buf2[1] = 0.0;
        buf3[0] = 0.0;
        buf3[1] = 0.0;
        calculateFeedbackAmount();
    };
    
    //sigEnum == 0 -> return;
    void processBuff(unsigned int sigEnum, double* modulation, double** inputs, double** outputs, int nFrames);
    inline void setCutoff(double newCutoff) { cutoff = newCutoff; calculateFeedbackAmount(); };
    inline void setResonance(double newResonance) { resonance = newResonance; calculateFeedbackAmount(); };
    inline void setFilterMode(FilterMode newMode) { mode = newMode; }
    inline void setAttenuation(FilterAttenuation newAttenuation) { attenuation = newAttenuation; }
    inline double getCalculatedCutoff() const {
        return fmax(fmin(cutoff + cutoffMod, 0.99), 0.01);
    };
    inline void setCutoffMod(double newCutoffMod) {
        cutoffMod = newCutoffMod;
        calculateFeedbackAmount();
    }
private:
    double cutoff;
    double cutoffMod;
    double resonance;
    FilterMode mode;
    FilterAttenuation attenuation;
    double feedbackAmount;
    //    inline void calculateFeedbackAmount() { feedbackAmount = resonance + resonance/(1.0 - cutoff); }
    inline void calculateFeedbackAmount() {
        feedbackAmount = resonance + resonance/(1.0 - getCalculatedCutoff());
    }
    inline void calculateBuf(double inputL, double inputR) {
        double calculatedCutoff = getCalculatedCutoff();
        buf0[0] += calculatedCutoff * (inputL - buf0[0] + feedbackAmount * (buf0[0] - buf1[0]));
        buf1[0] += calculatedCutoff * (buf0[0] - buf1[0]);
        buf2[0] += calculatedCutoff * (buf1[0] - buf2[0]);
        buf3[0] += calculatedCutoff * (buf2[0] - buf3[0]);
        
        buf0[1] += calculatedCutoff * (inputL - buf0[1] + feedbackAmount * (buf0[1] - buf1[1]));
        buf1[1] += calculatedCutoff * (buf0[1] - buf1[1]);
        buf2[1] += calculatedCutoff * (buf1[1] - buf2[1]);
        buf3[1] += calculatedCutoff * (buf2[1] - buf3[1]);
    }
    double buf0[2];
    double buf1[2];
    double buf2[2];
    double buf3[2];
    
    void process6(double* modulation, double **inputs, double **outputs, int nFrames);
    void process12(double* modulation, double **inputs, double **outputs, int nFrames);
    void process18(double* modulation, double **inputs, double **outputs, int nFrames);
    void process24(double* modulation, double **inputs, double **outputs, int nFrames);
};
#endif /* ECFilter_hpp */
