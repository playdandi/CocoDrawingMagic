#ifndef __CocoMagic__Sound__
#define __CocoMagic__Sound__

#include "Common.h"
#include "cocos2d.h"

using namespace cocos2d;

class Sound
{
public:
    void PreLoadSound();
    void SetEffectVolume();
    void SetBackgroundMusicVolume();
    
    void PlayBackgroundSound();
    void StopBackgroundSound();
    
    void playWarning();
    void playClick();
    void playClickboard();
    void playLvUpSuccess();
    void playLvUpFail();
    void playBoardMove();
    
private:
    std::string ext;
    int cnt;
    
};

#endif /* defined(__CocoMagic__Sound__) */
