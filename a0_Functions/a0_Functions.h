#pragma once

#include "IPlug_include_in_plug_hdr.h"
#include "ECDspExpr.hpp"

const int kNumPresets = 1;

enum EParams
{
  kSelectorParam = 0,
    kMinParam,
    kMaxParam,
    kVariableParam,
    kExprParam,
  kNumParams
};

enum ECtrlTags
{
    kPlotControl,
    kCtrlTags
};

using namespace iplug;
using namespace igraphics;

class a0_Functions final : public Plugin
{
    ECShapeExprLinear linearFunc = ECShapeExprLinear();
    ECNormalizedCosine normCosFunc = ECNormalizedCosine();
    ECInverseSquareRoot inverseSqrtFunc = ECInverseSquareRoot();
    ECShapeSine sineFunc = ECShapeSine();
    ECExpExprShape exponentialFunc = ECExpExprShape();
public:
    bool initDone = false;
  a0_Functions(const InstanceInfo& info);

#if IPLUG_EDITOR
  void OnParentWindowResize(int width, int height) override;
  bool OnHostRequestingSupportedViewConfiguration(int width, int height) override { return true; }
#endif
  
#if IPLUG_DSP // http://bit.ly/2S64BDd
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
    void OnParamChange(int indx) override;
#endif
};
