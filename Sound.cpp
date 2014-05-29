#include "Sound.h"
#include "SimpleAudioEngine.h"

using namespace cocos2d;
using namespace CocosDenshion;

/*void Sound::SetFileExtension()
 {
 int platform = CC_TARGET_PLATFORM;
 switch (platform)
 {
 case CC_PLATFORM_IOS:
 ext = "wav"; break;
 case CC_PLATFORM_ANDROID:
 ext = "ogg"; break;
 }
 }*/


void Sound::PreLoadSound()
{
    SimpleAudioEngine::sharedEngine()->preloadBackgroundMusic("sounds/bgm.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/click.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/clickboard.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/lvup_success.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/lvup_fail.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/magiclist.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/warning.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/gamestart.mp3");
    
    // voices
    SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/voice/title.mp3");
    
    SetEffectVolume();
    SetBackgroundMusicVolume(0.7f);
}

void Sound::SetEffectVolume()
{
    if (CCUserDefault::sharedUserDefault()->getBoolForKey("setting_option_0"))
        SimpleAudioEngine::sharedEngine()->setEffectsVolume(0.9);
    else
        SimpleAudioEngine::sharedEngine()->setEffectsVolume(0);
}
void Sound::SetBackgroundMusicVolume(float vol)
{
    SimpleAudioEngine::sharedEngine()->setBackgroundMusicVolume(vol);
}

void Sound::PlayBackgroundSound()
{
    SimpleAudioEngine::sharedEngine()->playBackgroundMusic("sounds/bgm.mp3", true);
}
void Sound::StopBackgroundSound()
{
    SimpleAudioEngine::sharedEngine()->stopBackgroundMusic();
}
void Sound::ResumeBackgroundSound()
{
    SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
}



void Sound::playClick()
{
    SimpleAudioEngine::sharedEngine()->playEffect("sounds/click.mp3");
}
void Sound::playClickboard()
{
    SimpleAudioEngine::sharedEngine()->playEffect("sounds/clickboard.mp3");
}

void Sound::playWarning()
{
    SimpleAudioEngine::sharedEngine()->playEffect("sounds/warning.mp3");
}

void Sound::playBoardMove()
{
    SimpleAudioEngine::sharedEngine()->playEffect("sounds/magiclist.mp3");
}

void Sound::playLvUpSuccess()
{
    SimpleAudioEngine::sharedEngine()->playEffect("sounds/lvup_success.mp3");
}
void Sound::playLvUpFail()
{
    SimpleAudioEngine::sharedEngine()->playEffect("sounds/lvup_fail.mp3");
}
void Sound::playGameStart()
{
    SimpleAudioEngine::sharedEngine()->playEffect("sounds/gamestart.mp3");
}
void Sound::UnLoadSound()
{
    
}


////////////////////////////////////////////////////////////////////////////////////////////////
void Sound::PreLoadInGameSound()
{
    SimpleAudioEngine::sharedEngine()->preloadBackgroundMusic("sounds/game_bgm.mp3");
    
    char name[22];
    for (int i = 1 ; i <= 18 ; i++)
    {
        sprintf(name, "sounds/pieces/%d.mp3", i);
        SimpleAudioEngine::sharedEngine()->preloadEffect(name);
    }
    
    SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/pieces/bombA.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/pieces/bombB.mp3");
    
    SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/skill/fire_2.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/skill/fire_3_land_4.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/skill/fire_5.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/skill/fire_6.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/skill/fire_7.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/skill/fire_8_dragon.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/skill/fire_8_comet.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/skill/fire_8_bomb_big.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/skill/fire_8_bomb_small.mp3");
    
    SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/skill/water_1.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/skill/water_2.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/skill/water_3_4.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/skill/water_5.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/skill/water_6.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/skill/water_7.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/skill/water_8_goddess_in.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/skill/water_8_goddess_out.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/skill/water_8_make_bluepiece.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/skill/water_8_remove_bluepiece.mp3");
    
    SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/skill/land_2.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/skill/land_5.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/skill/land_6.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/skill/land_8_tree_in.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/skill/land_8_bomb.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/skill/land_8_tree_rooting.mp3");
    
    // voices
    SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/voice/ready.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/voice/go.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/voice/timelimit.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/voice/gameover2.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/voice/bonus.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/voice/eit.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/voice/stoptime.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/voice/missionsuccess.mp3");
    
    SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/clock.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/game_result.mp3");
    
    SetEffectVolume();
    SetBackgroundMusicVolume(0.5f);
}


void Sound::PlayBackgroundInGameSound()
{
    SimpleAudioEngine::sharedEngine()->playBackgroundMusic("sounds/game_bgm.mp3", true);
}
void Sound::PauseBackgroundInGameSound()
{
    SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
}
void Sound::ResumeBackgroundInGameSound()
{
    SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
}
void Sound::StopBackgroundInGameSound()
{
    SimpleAudioEngine::sharedEngine()->stopBackgroundMusic();
}

void Sound::PlayPieceClick(int idx)
{
    if (idx % 18 > 0)
        idx = idx % 18;
    char name[22];
    sprintf(name, "sounds/pieces/%d.mp3", idx);
    SimpleAudioEngine::sharedEngine()->playEffect(name);
}

void Sound::PlayBomb()
{
    SimpleAudioEngine::sharedEngine()->playEffect("sounds/pieces/bombA.mp3");
    SimpleAudioEngine::sharedEngine()->playEffect("sounds/pieces/bombB.mp3");
}

void Sound::PlayGameResult()
{
    SimpleAudioEngine::sharedEngine()->playEffect("sounds/game_result.mp3");
}
void Sound::PlayClock()
{
    SimpleAudioEngine::sharedEngine()->playEffect("sounds/clock.mp3");
}

void Sound::PlaySkillSound(int skillNum)
{
    switch (skillNum)
    {
        case 0: SimpleAudioEngine::sharedEngine()->playEffect("sounds/skill/fire_2.mp3"); break;
        case 1: SimpleAudioEngine::sharedEngine()->playEffect("sounds/skill/fire_2.mp3"); break;
        case 2: SimpleAudioEngine::sharedEngine()->playEffect("sounds/skill/fire_3_land_4.mp3"); break;
        case 4: SimpleAudioEngine::sharedEngine()->playEffect("sounds/skill/fire_5.mp3"); break;
        case 5: SimpleAudioEngine::sharedEngine()->playEffect("sounds/skill/fire_6.mp3"); break;
        case 6: SimpleAudioEngine::sharedEngine()->playEffect("sounds/skill/fire_7.mp3"); break;
        case 7: SimpleAudioEngine::sharedEngine()->playEffect("sounds/skill/fire_8_dragon.mp3"); break;
        
        case 8: SimpleAudioEngine::sharedEngine()->playEffect("sounds/skill/water_1.mp3"); break;
        case 9: SimpleAudioEngine::sharedEngine()->playEffect("sounds/skill/water_2.mp3"); break;
        case 10: SimpleAudioEngine::sharedEngine()->playEffect("sounds/skill/water_3_4.mp3"); break;
        case 11: SimpleAudioEngine::sharedEngine()->playEffect("sounds/skill/water_3_4.mp3"); break;
        case 12: SimpleAudioEngine::sharedEngine()->playEffect("sounds/skill/water_5.mp3"); break;
        case 13: SimpleAudioEngine::sharedEngine()->playEffect("sounds/skill/water_6.mp3"); break;
        case 14: SimpleAudioEngine::sharedEngine()->playEffect("sounds/skill/water_7.mp3"); break;
            
        case 17: SimpleAudioEngine::sharedEngine()->playEffect("sounds/skill/land_2.mp3"); break;
        case 19: SimpleAudioEngine::sharedEngine()->playEffect("sounds/skill/fire_3_land_4.mp3"); break;
        case 20: SimpleAudioEngine::sharedEngine()->playEffect("sounds/skill/land_5.mp3"); break;
        case 21: SimpleAudioEngine::sharedEngine()->playEffect("sounds/skill/land_6.mp3"); break;
    }
}
void Sound::PlayDesignatedSound(int idx)
{
    // 기초 스킬 소리 이외의 것들 ( idx : skillNum*10 하고 0번~9번들 사용가능 (즉, 최대 10개) )
    switch (idx)
    {
        case 70: SimpleAudioEngine::sharedEngine()->playEffect("sounds/skill/fire_8_comet.mp3"); break;
        case 71: SimpleAudioEngine::sharedEngine()->playEffect("sounds/skill/fire_8_bomb_small.mp3"); break;
        case 72: SimpleAudioEngine::sharedEngine()->playEffect("sounds/skill/fire_8_bomb_big.mp3"); break;
            
        case 150: SimpleAudioEngine::sharedEngine()->playEffect("sounds/skill/water_8_goddess_in.mp3"); break;
        case 151: SimpleAudioEngine::sharedEngine()->playEffect("sounds/skill/water_8_goddess_out.mp3"); break;
        case 152: SimpleAudioEngine::sharedEngine()->playEffect("sounds/skill/water_8_make_bluepiece.mp3"); break;
        case 153: SimpleAudioEngine::sharedEngine()->playEffect("sounds/skill/water_8_remove_bluepiece.mp3"); break;
            
        case 230: SimpleAudioEngine::sharedEngine()->playEffect("sounds/skill/land_8_tree_in.mp3"); break;
        case 231: SimpleAudioEngine::sharedEngine()->playEffect("sounds/skill/land_8_bomb.mp3"); break;
        case 232: SimpleAudioEngine::sharedEngine()->playEffect("sounds/skill/land_8_tree_rooting.mp3"); break;
    }
}

void Sound::PlayVoice(int type)
{
    switch (type)
    {
        case VOICE_TITLE: SimpleAudioEngine::sharedEngine()->playEffect("sounds/voice/title.mp3"); break;
        case VOICE_READY: SimpleAudioEngine::sharedEngine()->playEffect("sounds/voice/ready.mp3"); break;
        case VOICE_GO: SimpleAudioEngine::sharedEngine()->playEffect("sounds/voice/go.mp3"); break;
        case VOICE_TIMELIMIT: SimpleAudioEngine::sharedEngine()->playEffect("sounds/voice/timelimit.mp3"); break;
        case VOICE_GAMEOVER: SimpleAudioEngine::sharedEngine()->playEffect("sounds/voice/gameover2.mp3"); break;
        case VOICE_BONUS: SimpleAudioEngine::sharedEngine()->playEffect("sounds/voice/bonus.mp3"); break;
        case VOICE_STOPTIME: SimpleAudioEngine::sharedEngine()->playEffect("sounds/voice/stoptime.mp3"); break;
        case VOICE_MISSIONSUCCESS: SimpleAudioEngine::sharedEngine()->playEffect("sounds/voice/missionsuccess.mp3"); break;
    }
}

void Sound::UnLoadInGameSound()
{
    SimpleAudioEngine::sharedEngine()->stopBackgroundMusic("sounds/game_bgm.mp3");
    
    char name[22];
    for (int i = 1 ; i <= 18 ; i++)
    {
        sprintf(name, "sounds/pieces/%d.mp3", i);
        SimpleAudioEngine::sharedEngine()->unloadEffect(name);
    }
    
    SimpleAudioEngine::sharedEngine()->unloadEffect("sounds/pieces/bombA.mp3");
    SimpleAudioEngine::sharedEngine()->unloadEffect("sounds/pieces/bombB.mp3");
    
    SimpleAudioEngine::sharedEngine()->unloadEffect("sounds/skill/fire_2.mp3");
    SimpleAudioEngine::sharedEngine()->unloadEffect("sounds/skill/fire_3_land_4.mp3");
    SimpleAudioEngine::sharedEngine()->unloadEffect("sounds/skill/fire_5.mp3");
    SimpleAudioEngine::sharedEngine()->unloadEffect("sounds/skill/fire_6.mp3");
    SimpleAudioEngine::sharedEngine()->unloadEffect("sounds/skill/fire_7.mp3");
    SimpleAudioEngine::sharedEngine()->unloadEffect("sounds/skill/fire_8_dragon.mp3");
    SimpleAudioEngine::sharedEngine()->unloadEffect("sounds/skill/fire_8_comet.mp3");
    SimpleAudioEngine::sharedEngine()->unloadEffect("sounds/skill/fire_8_bomb_big.mp3");
    SimpleAudioEngine::sharedEngine()->unloadEffect("sounds/skill/fire_8_bomb_small.mp3");
    
    SimpleAudioEngine::sharedEngine()->unloadEffect("sounds/skill/water_1.mp3");
    SimpleAudioEngine::sharedEngine()->unloadEffect("sounds/skill/water_2.mp3");
    SimpleAudioEngine::sharedEngine()->unloadEffect("sounds/skill/water_3_4.mp3");
    SimpleAudioEngine::sharedEngine()->unloadEffect("sounds/skill/water_5.mp3");
    SimpleAudioEngine::sharedEngine()->unloadEffect("sounds/skill/water_6.mp3");
    SimpleAudioEngine::sharedEngine()->unloadEffect("sounds/skill/water_7.mp3");
    SimpleAudioEngine::sharedEngine()->unloadEffect("sounds/skill/water_8_goddess_in.mp3");
    SimpleAudioEngine::sharedEngine()->unloadEffect("sounds/skill/water_8_goddess_out.mp3");
    SimpleAudioEngine::sharedEngine()->unloadEffect("sounds/skill/water_8_make_bluepiece.mp3");
    SimpleAudioEngine::sharedEngine()->unloadEffect("sounds/skill/water_8_remove_bluepiece.mp3");
    
    SimpleAudioEngine::sharedEngine()->unloadEffect("sounds/skill/land_2.mp3");
    SimpleAudioEngine::sharedEngine()->unloadEffect("sounds/skill/land_5.mp3");
    SimpleAudioEngine::sharedEngine()->unloadEffect("sounds/skill/land_6.mp3");
    SimpleAudioEngine::sharedEngine()->unloadEffect("sounds/skill/land_8_tree_in.mp3");
    SimpleAudioEngine::sharedEngine()->unloadEffect("sounds/skill/land_8_bomb.mp3");
    SimpleAudioEngine::sharedEngine()->unloadEffect("sounds/skill/land_8_tree_rooting.mp3");
    
    // voices
    SimpleAudioEngine::sharedEngine()->unloadEffect("sounds/voice/ready.mp3");
    SimpleAudioEngine::sharedEngine()->unloadEffect("sounds/voice/go.mp3");
    SimpleAudioEngine::sharedEngine()->unloadEffect("sounds/voice/timelimit.np3");
    SimpleAudioEngine::sharedEngine()->unloadEffect("sounds/voice/gameover2.mp3");
    SimpleAudioEngine::sharedEngine()->unloadEffect("sounds/voice/bonus.mp3");
    SimpleAudioEngine::sharedEngine()->unloadEffect("sounds/voice/eit.mp3");
    SimpleAudioEngine::sharedEngine()->unloadEffect("sounds/voice/stoptime.mp3");
    SimpleAudioEngine::sharedEngine()->unloadEffect("sounds/voice/missionsuccess.mp3");
    
    SimpleAudioEngine::sharedEngine()->unloadEffect("sounds/clock.mp3");
    SimpleAudioEngine::sharedEngine()->unloadEffect("sounds/game_result.mp3");
}


