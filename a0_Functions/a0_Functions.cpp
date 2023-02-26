#include "a0_Functions.h"
#include "IPlug_include_in_plug_src.h"

#if IPLUG_EDITOR
#include "IControls.h"
#endif

a0_Functions::a0_Functions(const InstanceInfo& info)
: Plugin(info, MakeConfig(kNumParams, kNumPresets))
{
    GetParam(kMinParam)->InitDouble("", -1., -2, 2., 0.01);
    GetParam(kMaxParam)->InitDouble("", 1., -2, 2., 0.01);
    GetParam(kVariableParam)->InitDouble("", 0., -2, 2., 0.01);
    GetParam(kExprParam)->InitDouble("", 0., -1, 1., 0.01);
    GetParam(kSelectorParam)->InitEnum("Shapes", 0, 8, "", IParam::kFlagsNone, "", "Linear", "Square", "e^x", "Linear expression", "normalized sin function", "inverse square root", "sin (bipolar)", "exponential x^y");

#if IPLUG_EDITOR // http://bit.ly/2S64BDd
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS);
  };
  
    mLayoutFunc = [&](IGraphics* pGraphics) {
      pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
      pGraphics->AttachPanelBackground(COLOR_GRAY);
      pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
      const IRECT b = pGraphics->GetBounds();
        pGraphics->AttachControl(new IVTabSwitchControl(b.GetGridCell(0, 2, 2), kSelectorParam, {}, "", DEFAULT_STYLE, EVShape::Rectangle, EDirection::Vertical));
        pGraphics->AttachControl(new IVKnobControl(b.GetGridCell(7, 0, 10, 6), kVariableParam));
        pGraphics->AttachControl(new IVKnobControl(b.GetGridCell(7, 1, 10, 6), kMinParam, "min"));
        pGraphics->AttachControl(new IVKnobControl(b.GetGridCell(7, 2, 10, 6), kMaxParam, "max"));
        
        pGraphics->AttachControl(new IVPlotControl(b.GetGridCell(1, 1, 2),
        {{COLOR_BLACK,
            [&](double x){  //from likely signal
                x = 2. * x - 1.;
                double *i = &x;
                double o = 0;
                if (GetParam(kSelectorParam)->Int() == 3) {
                    linearFunc.processSignal(i, &o);
                }
                else if (GetParam(kSelectorParam)->Int() == 4) {
                    normCosFunc.processSignal(i, &o);
                }
                else if (GetParam(kSelectorParam)->Int() == 5) {
                    inverseSqrtFunc.processSignal(i, &o);
                }
                else if (GetParam(kSelectorParam)->Int() == 6) {
                    sineFunc.processSignal(i, &o);
                }
                return o;
        }}, {COLOR_RED,
            [&](double x){
                return GetParam(kExprParam)->FromNormalized(x);//from UI control
            }}, {COLOR_BLUE,
                [&](double x){
                    return GetParam(kExprParam)->ToNormalized(2. * x - 1.); // from param to UI control
                }}}, 800, "", DEFAULT_STYLE, -1.2, 1.2), kPlotControl, "");
        initDone = true;
    };
#endif
}

#if IPLUG_EDITOR
void a0_Functions::OnParentWindowResize(int width, int height)
{
  if(GetUI())
    GetUI()->Resize(width, height, 1.f, false);
}
#endif

#if IPLUG_DSP
void a0_Functions::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{ 
//  const int nChans = NOutChansConnected();
//  const double gain = GetParam(kParamGain)->Value() / 100.;
//  
//  for (int s = 0; s < nFrames; s++) {
//    for (int c = 0; c < nChans; c++) {
//      outputs[c][s] = inputs[c][s] * gain;
//    }
//  }
}

void a0_Functions::OnParamChange(int idx) {
    int selectorEnum = GetParam(kSelectorParam)->Int();
    double val = GetParam(kVariableParam)->Value();
    double min = GetParam(kMinParam)->Value();
    double max = GetParam(kMaxParam)->Value();
    if (selectorEnum == 0) {
        GetParam(kExprParam)->InitDouble("", 0., min, max, 0.01);
    }
    else if (selectorEnum == 1) {
        GetParam(kExprParam)->InitDouble("", 0., min, max, 0.01, "", 0, "", IParam::ShapePowCurve(2));
    }
    else if (selectorEnum == 2) {
        GetParam(kExprParam)->InitDouble("", 0., min, max, 0.01, "", 0, "", IParam::ShapeExp());
    }
    else if (selectorEnum == 3) {
        linearFunc.slope = val;
        GetParam(kExprParam)->InitDouble("function", 0, min, max, 0.01, "", 0, "", linearFunc);
    }
    else if (selectorEnum == 4) {
        GetParam(kExprParam)->InitDouble("function", 0, min, max, 0.01, "", 0, "", normCosFunc);
    }
    else if (selectorEnum == 5) {
        inverseSqrtFunc.slope = val;
        GetParam(kExprParam)->InitDouble("function", 0, min, max, 0.01, "", 0, "", inverseSqrtFunc);
    }
    else if (selectorEnum == 6) {
        GetParam(kExprParam)->InitDouble("function", 0, min, max, 0.01, "", 0, "", sineFunc);
    }
    else if (selectorEnum == 7) {
        exponentialFunc.SetBase(val*5);
        GetParam(kExprParam)->InitDouble("function", 0, min, max, 0.01, "", 0, "", exponentialFunc);
    }
    if (initDone) {
        GetUI()-> GetControlWithTag(kPlotControl)->SetDirty();
    }
}
#endif
