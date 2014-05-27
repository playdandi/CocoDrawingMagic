#ifndef __CocoMagic__Sound__
#define __CocoMagic__Sound__

#include "Common.h"
#include "cocos2d.h"

#define VOICE_TITLE 0
#define VOICE_READY 1
#define VOICE_GO 2
#define VOICE_TIMELIMIT 3
#define VOICE_GAMEOVER 4
#define VOICE_BONUS 5
#define VOICE_GODDESS 6


using namespace cocos2d;

class Sound
{
public:
    void PreLoadSound();
    
    void SetEffectVolume();
    void SetBackgroundMusicVolume(float f);
    
    void PlayBackgroundSound();
    void ResumeBackgroundSound();
    void StopBackgroundSound();
    
    void playWarning();
    void playClick();
    void playClickboard();
    void playLvUpSuccess();
    void playLvUpFail();
    void playBoardMove();
    void playGameStart();
    
    void PlayVoice(int type);
    
    void UnLoadSound();
    
    ////////////////////////////////////////////////////////////////
    void PreLoadInGameSound();
    
    void PlayBackgroundInGameSound();
    void PauseBackgroundInGameSound();
    void ResumeBackgroundInGameSound();
    void StopBackgroundInGameSound();
    
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
