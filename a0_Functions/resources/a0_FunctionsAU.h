
#include <TargetConditionals.h>
#if TARGET_OS_IOS == 1
#import <UIKit/UIKit.h>
#else
#import <Cocoa/Cocoa.h>
#endif

#define IPLUG_AUVIEWCONTROLLER IPlugAUViewController_va0_Functions
#define IPLUG_AUAUDIOUNIT IPlugAUAudioUnit_va0_Functions
#import <a0_FunctionsAU/IPlugAUViewController.h>
#import <a0_FunctionsAU/IPlugAUAudioUnit.h>

//! Project version number for a0_FunctionsAU.
FOUNDATION_EXPORT double a0_FunctionsAUVersionNumber;

//! Project version string for a0_FunctionsAU.
FOUNDATION_EXPORT const unsigned char a0_FunctionsAUVersionString[];

@class IPlugAUViewController_va0_Functions;
