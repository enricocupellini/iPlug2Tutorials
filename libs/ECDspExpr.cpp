//
//  ECDspExpr.cpp
//  a0_Shapes-macOS
//
//  Created by Enrico Cupellini on 20/08/2019.
//  Copyright © Enrico Cupellini 2019

#include "ECDspExpr.hpp"

#pragma mark ECShapeExprLinear

double ECShapeExprLinear::NormalizedToValue(double value, const IParam& param) const
{
    return  slope * (param.GetMin() + (value *(param.GetMax() - param.GetMin())));
}

double ECShapeExprLinear::ValueToNormalized(double value, const IParam& param) const
{
    return ((value - param.GetMin()) /  (param.GetMax() - param.GetMin()))/slope;
}

void ECShapeExprLinear::processSignal(double *input, double* output) {
    *output = *input * slope;
}

#pragma mark ECNormalizedCosine

double ECNormalizedCosine::NormalizedToValue(double value, const IParam& param) const
{
    return - 0.5 * cos(PI*value) + 0.5 * (param.GetMin() + (value * (param.GetMax() - param.GetMin())));
}

double ECNormalizedCosine::ValueToNormalized(double value, const IParam& param) const
{
    return (asin(((value - param.GetMin()) / (param.GetMax() - param.GetMin())) * 2. - 1.)/PI + 0.5);
}

void ECNormalizedCosine::processSignal(double *input, double* output) {
    *output = (*input >0)? - 0.5 * cos(*input * PI) + 0.5: 0.5 * cos(*input * PI) - 0.5;
}

#pragma mark ECShapeSine

double ECShapeSine::NormalizedToValue(double value, const IParam& param) const
{
    return - 0.5 * cos(PI*value) + 0.5;
}

double ECShapeSine::ValueToNormalized(double value, const IParam& param) const
{
    double a = asin(value * 2. - 1.);
    return a/PI + 0.5;
}

void ECShapeSine::processSignal(double *input, double* output) {
    *output = sin(M_PI_2 * *input);
}

#pragma mark ECInverseSquareRoot

double ECInverseSquareRoot::NormalizedToValue(double value, const IParam& param) const
{
    return slope*value/sqrt(pow(slope, 2)* pow(value, 2)+ 1.);
}

double ECInverseSquareRoot::ValueToNormalized(double value, const IParam& param) const
{
    return (value - param.GetMin()) / (param.GetMax() - param.GetMin());
}

void ECInverseSquareRoot::processSignal(double *input, double* output) {
    *output = slope* *input/sqrt(pow(slope, 2)* pow(*input, 2)+ 1.);
}

#pragma mark ECExp2xShape

double ECExpExprShape::NormalizedToValue(double value, const IParam& param) const
{
//    return param.GetMin() + std::pow(base, value * (param.GetMax() - param.GetMin())) -1.;
    return pow(base, mAdd + value * mMul);
}

double ECExpExprShape::ValueToNormalized(double value, const IParam& param) const
{
//    double paramMin 
//    double x = value - param.GetMin() + 1;
//    return std::log(((value - param.GetMin()) / (param.GetMax() - param.GetMin())) + 1)/std::log(base) / x ;
    return ((std::log(value)/std::log(base)) - mAdd) / mMul;
}

//void ECExpExprShape::computeValues(){
//    xMinNotNormalized = std::log(minParam) / std::log(base); //parameter mapping to get xMinNotNormalized^y = param.GetMin()
//    xMaxNotNormalized = std::log(maxParam) / std::log(base); //parameter mapping to get xMaxNotNormalized^y = param.GetMax()
//    xMaxNormalized = 10 / std::log(base); // log(10) = 1
//}

