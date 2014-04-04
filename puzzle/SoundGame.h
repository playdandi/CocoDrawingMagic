#ifndef __CocoMagic__SoundGame__
#define __CocoMagic__SoundGame__

#include "Common.h"
#include "cocos2d.h"

using namespace cocos2d;

class SoundGame
{
public:
    void PreLoadSound();
    
    void SetEffectVolume();
    void SetBackgroundMusicVolume(float vol);
    
    void PlayBackgroundSound();
    void StopBackgroundSound();

private:
    std::string ext;
    int cnt;
};


#endif /* defined(__CocoMagic__SoundGame__) */
