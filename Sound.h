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
    void SetBackgroundMusicVolume(float f);
    
    void PlayBackgroundSound();
    void StopBackgroundSound();
    
    void playWarning();
    void playClick();
    void playClickboard();
    void playLvUpSuccess();
    void playLvUpFail();
    void playBoardMove();
    void playGameStart();
    
    void UnLoadSound();
    
    //////////////////////////////////////////////////////
    void PreLoadInGameSound();
    
    void PlayBackgroundInGameSound();
    
    void PlayPieceClick(int idx);
    void PlayBomb();
    void PlaySkillSound(int skillNum);
    void PlayDesginatedSound(int idx);
    void UnLoadInGameSound();
    
private:
    std::string ext;
    int cnt;
};

#endif /* defined(__CocoMagic__Sound__) */
