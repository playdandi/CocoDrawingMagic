#ifndef __CocoMagic__Sound__
#define __CocoMagic__Sound__

#include "Common.h"
#include "cocos2d.h"
#include "SimpleAudioEngine.h"

using namespace cocos2d;
using namespace CocosDenshion;


#define VOICE_TITLE 0
#define VOICE_READY 1
#define VOICE_GO 2
#define VOICE_TIMELIMIT 3
#define VOICE_GAMEOVER 4
#define VOICE_BONUS 5
#define VOICE_GODDESS 6
#define VOICE_STOPTIME 7
#define VOICE_MISSIONSUCCESS 8
#define VOICE_EIT2 9
#define VOICE_EIT3 10


class Sound
{
public:
    void PreLoadSound();
    
    void SetEffectVolume();
    void SetBackgroundMusicVolume(float f);
    
    void PlayBackgroundSound();
    void PauseBackgroundSound();
    void ResumeBackgroundSound();
    void StopBackgroundSound();
    
    void StopAllEffects();
    void ResumeAllEffects();
    
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
    
    void PlayClock();
    void StopClock();
    
    void PlayGameResult();
    
    void PlayPieceClick(int idx);
    void PlayBomb();
    void PlaySkillSound(int skillNum);
    void PlayDesignatedSound(int idx);
    void UnLoadInGameSound();
    
protected:
    int nClockSoundId;
    
private:
    SimpleAudioEngine* engine;
};

#endif /* defined(__CocoMagic__Sound__) */
