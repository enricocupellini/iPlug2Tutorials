//
//  MixerUtilities.hpp
//  a0_Harmonics-macOS
//
//  Created by Enrico Cupellini on 27/08/2019.
//

#ifndef MixerUtilities_hpp
#define MixerUtilities_hpp

#include <stdio.h>

class SoloMute16 {
    unsigned int muted = 0;
    unsigned int solo = 0;
public:
    void SetSolo(const unsigned int& channel, bool& value) {
        if(value){
            solo |= 0x01<<(0xFF & channel);
        }
        else {
            solo &= ~(0x01<<(0xFF & channel));
        }
    }
    inline bool GetSolo(const unsigned int& channel) {return solo & 0x01<<(0xFF & channel);}
    void SetMuted(const unsigned int& channel, bool& value) {
        if (value) {
            muted |= 0x01<<(0xFF & channel);
        }
        else {
            muted &= ~(0x01<<(0xFF & channel));
        }
    }
    bool GetMuted(const unsigned int& channel) {return muted & 0x01<<(0xFF & channel);}
    
    inline bool activeChannel(const unsigned int& channel) {return (solo) ? GetSolo(channel) : !GetMuted(channel);}
    
    inline bool isSoloActive(){return solo > 0;}
};

#endif /* MixerUtilities_hpp */
