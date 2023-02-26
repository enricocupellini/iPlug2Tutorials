//
//  ECDspExpr.hpp
//  a0_Shapes-macOS
//
//  Created by Enrico Cupellini on 20/08/2019.
//  Copyright © Enrico Cupellini 2019

#ifndef ECDspExpr_hpp
#define ECDspExpr_hpp

#include "IPlugParameter.h"

using namespace iplug;

// These Shapes can be used by an IParam to map the UI controls or used by the dsp process to shape the signal
// use processSignal() method for the latter case

struct ECDspExprShape : public IParam::Shape {
    Shape* Clone() const override { return new ECDspExprShape(*this); };
    void processSignal(double* input, double* output) {}

    IParam::EDisplayType GetDisplayType() const override { return IParam::kDisplayLinear; }
    double NormalizedToValue(double value, const IParam& param) const override {return 0.;};
    double ValueToNormalized(double value, const IParam& param) const override {return 0.;};
};

// a linear shape with slope control
struct ECShapeExprLinear : public ECDspExprShape {
    Shape* Clone() const override { return new ECShapeExprLinear(*this); };
    IParam::EDisplayType GetDisplayType() const override { return IParam::kDisplayLinear; }
    double NormalizedToValue(double value, const IParam& param) const override;
    double ValueToNormalized(double value, const IParam& param) const override;
    void processSignal(double* input, double* output);
    
    double slope = 1.;
};

// normalized cosine.
// Fixed range from 0 to 1
struct ECNormalizedCosine : public ECDspExprShape {
    Shape* Clone() const override { return new ECNormalizedCosine(*this); };
    IParam::EDisplayType GetDisplayType() const override { return IParam::kDisplaySquareRoot; }
    double NormalizedToValue(double value, const IParam& param) const override;
    double ValueToNormalized(double value, const IParam& param) const override;
    void processSignal(double* input, double* output);
};

// sinusoid.
// Fixed range from -1 to 1
struct ECShapeSine : public ECDspExprShape {
    Shape* Clone() const override { return new ECShapeSine(*this); };
    IParam::EDisplayType GetDisplayType() const override { return IParam::kDisplaySquareRoot; }
    double NormalizedToValue(double value, const IParam& param) const override;
    double ValueToNormalized(double value, const IParam& param) const override;
    void processSignal(double* input, double* output);
};

// inverse square root activation func.
// with param to control slope
struct ECInverseSquareRoot : public ECDspExprShape {
    Shape* Clone() const override { return new ECInverseSquareRoot(*this); };
    IParam::EDisplayType GetDisplayType() const override { return IParam::kDisplaySquareRoot; }
    double NormalizedToValue(double value, const IParam& param) const override;
    double ValueToNormalized(double value, const IParam& param) const override;
    void processSignal(double* input, double* output);
    
    double slope = 1;
};

// exponential shape with variable base (2^x is the same increase of frequencies for pitches)

// min & max > 0 !!!
struct ECExpExprShape : public ECDspExprShape {
private:
    double minParam;
    double maxParam;
//
//    double xMinNotNormalized;
//    double xMaxNotNormalized;
//    double xMaxNormalized;
    double mAdd;
    double mMul;
    // base > 0 !
    double base = 2;
    
    void computeValues() {
        mAdd = std::log(minParam)/std::log(base);
        mMul = std::log(maxParam / minParam)/std::log(base);
    }
public:
    Shape* Clone() const override { return new ECExpExprShape(*this); };
    void Init(const IParam& param) override
    {
        minParam = param.GetMin();
        maxParam = param.GetMax();
        if(param.GetMin() < 0.0000001)
            minParam = 0.0000001;
        if(param.GetMax() < 0.0000001)
            maxParam = 0.0000001;
        
        double min = param.GetMin();
        
        if(min <= 0.)
            min = 0.00000001;
        
        computeValues();
    }
    IParam::EDisplayType GetDisplayType() const override { return IParam::kDisplayLog; }
    double NormalizedToValue(double value, const IParam& param) const override;
    double ValueToNormalized(double value, const IParam& param) const override;

    void SetBase(double b) {
        if (b < 0) {
            b = 0;
        }
        base = b;
        computeValues();
    }
};
#endif /* ECDspExpr_hpp */
