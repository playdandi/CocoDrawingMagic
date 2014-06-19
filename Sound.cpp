#include "Sound.h"
#include "SimpleAudioEngine.h"

using namespace cocos2d;
using namespace CocosDenshion;


// 안드로이드 및 iOS 확장자만 분류하면 되는 사운드들
std::string ext(std::string fname)
{
    std::string extension;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    extension = "mp3";
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    extension = "ogg";
#endif
    
    std::string res = fname + "." + extension;
    CCLog("sound = %s", res.c_str());
    return res;
}

// 안드로이드 및 iOS 폴더로 분류되는 사운드들
std::string ext(std::string path, std::string fname)
{
    std::string extension;
    std::string os;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    extension = "mp3";
    os = "ios";
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    extension = "ogg";
    os = "android";
#endif
    
    std::string res = path + "/" + os + "/" + fname + "." + extension;
    CCLog("sound = %s", res.c_str());
    return res;
}



void Sound::PreLoadSound()
{
    SimpleAudioEngine::sharedEngine()->preloadBackgroundMusic(ext("sounds/bgm").c_str());
    SimpleAudioEngine::sharedEngine()->preloadEffect(ext("sounds/click").c_str());
    SimpleAudioEngine::sharedEngine()->preloadEffect(ext("sounds/clickboard").c_str());
    SimpleAudioEngine::sharedEngine()->preloadEffect(ext("sounds/lvup_success").c_str());
    SimpleAudioEngine::sharedEngine()->preloadEffect(ext("sounds/lvup_fail").c_str());
    SimpleAudioEngine::sharedEngine()->preloadEffect(ext("sounds/magiclist").c_str());
    SimpleAudioEngine::sharedEngine()->preloadEffect(ext("sounds/warning").c_str());
    SimpleAudioEngine::sharedEngine()->preloadEffect(ext("sounds/gamestart").c_str());
    
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
    SimpleAudioEngine::sharedEngine()->playBackgroundMusic(ext("sounds/bgm").c_str(), true);
}
void Sound::StopBackgroundSound()
{
    SimpleAudioEngine::sharedEngine()->stopBackgroundMusic();
}
void Sound::ResumeBackgroundSound()
{
    SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
}

void Sound::StopAllEffects()
{
    SimpleAudioEngine::sharedEngine()->stopAllEffects();
}
void Sound::ResumeAllEffects()
{
    SimpleAudioEngine::sharedEngine()->resumeAllEffects();
}

void Sound::playClick()
{
    SimpleAudioEngine::sharedEngine()->playEffect(ext("sounds/click").c_str());
}
void Sound::playClickboard()
{
    SimpleAudioEngine::sharedEngine()->playEffect(ext("sounds/clickboard").c_str());
}

void Sound::playWarning()
{
    SimpleAudioEngine::sharedEngine()->playEffect(ext("sounds/warning").c_str());
}

void Sound::playBoardMove()
{
    SimpleAudioEngine::sharedEngine()->playEffect(ext("sounds/magiclist").c_str());
}

void Sound::playLvUpSuccess()
{
    SimpleAudioEngine::sharedEngine()->playEffect(ext("sounds/lvup_success").c_str());
}
void Sound::playLvUpFail()
{
    SimpleAudioEngine::sharedEngine()->playEffect(ext("sounds/lvup_fail").c_str());
}
void Sound::playGameStart()
{
    SimpleAudioEngine::sharedEngine()->playEffect(ext("sounds/gamestart").c_str());
}
void Sound::UnLoadSound()
{
    
}


////////////////////////////////////////////////////////////////////////////////////////////////
void Sound::PreLoadInGameSound()
{
    SimpleAudioEngine::sharedEngine()->preloadBackgroundMusic(ext("sounds/game_bgm").c_str());
    
    char name[22];
    for (int i = 1 ; i <= 18 ; i++)
    {
        sprintf(name, "%d", i);
        SimpleAudioEngine::sharedEngine()->preloadEffect(ext("sounds/pieces", name).c_str());
    }
    
    SimpleAudioEngine::sharedEngine()->preloadEffect(ext("sounds/pieces", "bombA").c_str());
    SimpleAudioEngine::sharedEngine()->preloadEffect(ext("sounds/pieces", "bombB").c_str());
    
    SimpleAudioEngine::sharedEngine()->preloadEffect(ext("sounds/skill", "fire_2").c_str());
    SimpleAudioEngine::sharedEngine()->preloadEffect(ext("sounds/skill", "fire_3_land_4").c_str());
    SimpleAudioEngine::sharedEngine()->preloadEffect(ext("sounds/skill", "fire_5").c_str());
    SimpleAudioEngine::sharedEngine()->preloadEffect(ext("sounds/skill", "fire_6").c_str());
    SimpleAudioEngine::sharedEngine()->preloadEffect(ext("sounds/skill", "fire_7").c_str());
    SimpleAudioEngine::sharedEngine()->preloadEffect(ext("sounds/skill", "fire_8_dragon").c_str());
    SimpleAudioEngine::sharedEngine()->preloadEffect(ext("sounds/skill", "fire_8_comet").c_str());
    SimpleAudioEngine::sharedEngine()->preloadEffect(ext("sounds/skill", "fire_8_bomb_big").c_str());
    SimpleAudioEngine::sharedEngine()->preloadEffect(ext("sounds/skill", "fire_8_bomb_small").c_str());
    
    SimpleAudioEngine::sharedEngine()->preloadEffect(ext("sounds/skill", "water_1").c_str());
    SimpleAudioEngine::sharedEngine()->preloadEffect(ext("sounds/skill", "water_2").c_str());
    SimpleAudioEngine::sharedEngine()->preloadEffect(ext("sounds/skill", "water_3_4").c_str());
    SimpleAudioEngine::sharedEngine()->preloadEffect(ext("sounds/skill", "water_5").c_str());
    SimpleAudioEngine::sharedEngine()->preloadEffect(ext("sounds/skill", "water_6").c_str());
    SimpleAudioEngine::sharedEngine()->preloadEffect(ext("sounds/skill", "water_7").c_str());
    SimpleAudioEngine::sharedEngine()->preloadEffect(ext("sounds/skill", "water_8_goddess_in").c_str());
    SimpleAudioEngine::sharedEngine()->preloadEffect(ext("sounds/skill", "water_8_goddess_out").c_str());
    SimpleAudioEngine::sharedEngine()->preloadEffect(ext("sounds/skill", "water_8_make_bluepiece").c_str());
    SimpleAudioEngine::sharedEngine()->preloadEffect(ext("sounds/skill", "water_8_remove_bluepiece").c_str());
    
    SimpleAudioEngine::sharedEngine()->preloadEffect(ext("sounds/skill", "land_2").c_str());
    SimpleAudioEngine::sharedEngine()->preloadEffect(ext("sounds/skill", "land_5").c_str());
    SimpleAudioEngine::sharedEngine()->preloadEffect(ext("sounds/skill", "land_6").c_str());
    SimpleAudioEngine::sharedEngine()->preloadEffect(ext("sounds/skill", "land_8_tree_in").c_str());
    SimpleAudioEngine::sharedEngine()->preloadEffect(ext("sounds/skill", "land_8_bomb").c_str());
    SimpleAudioEngine::sharedEngine()->preloadEffect(ext("sounds/skill", "land_8_tree_rooting").c_str());
    
    // voices
    SimpleAudioEngine::sharedEngine()->preloadEffect(ext("sounds/voice", "ready").c_str());
    SimpleAudioEngine::sharedEngine()->preloadEffect(ext("sounds/voice", "go").c_str());
    SimpleAudioEngine::sharedEngine()->preloadEffect(ext("sounds/voice", "timelimit").c_str());
    SimpleAudioEngine::sharedEngine()->preloadEffect(ext("sounds/voice", "gameover2").c_str());
    SimpleAudioEngine::sharedEngine()->preloadEffect(ext("sounds/voice", "bonus").c_str());
    SimpleAudioEngine::sharedEngine()->preloadEffect(ext("sounds/voice", "stoptime").c_str());
    SimpleAudioEngine::sharedEngine()->preloadEffect(ext("sounds/voice", "missionsuccess").c_str());
    SimpleAudioEngine::sharedEngine()->preloadEffect(ext("sounds/voice", "eit").c_str());
    SimpleAudioEngine::sharedEngine()->preloadEffect(ext("sounds/voice", "eit2").c_str());
    SimpleAudioEngine::sharedEngine()->preloadEffect(ext("sounds/voice", "eit3").c_str());
    
    SimpleAudioEngine::sharedEngine()->preloadEffect(ext("sounds/clock").c_str());
    SimpleAudioEngine::sharedEngine()->preloadEffect(ext("sounds/game_result").c_str());
    
    SetEffectVolume();
    SetBackgroundMusicVolume(0.5f);
}


void Sound::PlayBackgroundInGameSound()
{
    SimpleAudioEngine::sharedEngine()->playBackgroundMusic(ext("sounds/game_bgm").c_str(), true);
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
    //sprintf(name, "sounds/pieces/%d.mp3", idx);
    sprintf(name, "%d", idx);
    SimpleAudioEngine::sharedEngine()->playEffect(ext("sounds/pieces", name).c_str());
}

void Sound::PlayBomb()
{
    SimpleAudioEngine::sharedEngine()->playEffect(ext("sounds/pieces", "bombA").c_str());
    SimpleAudioEngine::sharedEngine()->playEffect(ext("sounds/pieces", "bombB").c_str());
}

void Sound::PlayGameResult()
{
    SimpleAudioEngine::sharedEngine()->playEffect(ext("sounds/game_result").c_str());
}
void Sound::PlayClock()
{
    nClockSoundId = SimpleAudioEngine::sharedEngine()->playEffect(ext("sounds/clock").c_str(), true);
}
void Sound::StopClock()
{
    SimpleAudioEngine::sharedEngine()->stopEffect(nClockSoundId);
}

void Sound::PlaySkillSound(int skillNum)
{
    switch (skillNum)
    {
        case 0: SimpleAudioEngine::sharedEngine()->playEffect(ext("sounds/skill", "fire_2").c_str()); break;
        case 1: SimpleAudioEngine::sharedEngine()->playEffect(ext("sounds/skill", "fire_2").c_str()); break;
        case 2: SimpleAudioEngine::sharedEngine()->playEffect(ext("sounds/skill", "fire_3_land_4").c_str()); break;
        case 4: SimpleAudioEngine::sharedEngine()->playEffect(ext("sounds/skill", "fire_5").c_str()); break;
        case 5: SimpleAudioEngine::sharedEngine()->playEffect(ext("sounds/skill", "fire_6").c_str()); break;
        case 6: SimpleAudioEngine::sharedEngine()->playEffect(ext("sounds/skill", "fire_7").c_str()); break;
        case 7: SimpleAudioEngine::sharedEngine()->playEffect(ext("sounds/skill", "fire_8_dragon").c_str()); break;
        
        case 8: SimpleAudioEngine::sharedEngine()->playEffect(ext("sounds/skill", "water_1").c_str()); break;
        case 9: SimpleAudioEngine::sharedEngine()->playEffect(ext("sounds/skill", "water_2").c_str()); break;
        case 10: SimpleAudioEngine::sharedEngine()->playEffect(ext("sounds/skill", "water_3_4").c_str()); break;
        case 11: SimpleAudioEngine::sharedEngine()->playEffect(ext("sounds/skill", "water_3_4").c_str()); break;
        case 12: SimpleAudioEngine::sharedEngine()->playEffect(ext("sounds/skill", "water_5").c_str()); break;
        case 13: SimpleAudioEngine::sharedEngine()->playEffect(ext("sounds/skill", "water_6").c_str()); break;
        case 14: SimpleAudioEngine::sharedEngine()->playEffect(ext("sounds/skill", "water_7").c_str()); break;
            
        case 17: SimpleAudioEngine::sharedEngine()->playEffect(ext("sounds/skill", "land_2").c_str()); break;
        case 18: SimpleAudioEngine::sharedEngine()->playEffect(ext("sounds/skill", "fire_3_land_4").c_str()); break;
        case 19: SimpleAudioEngine::sharedEngine()->playEffect(ext("sounds/skill", "fire_3_land_4").c_str()); break;
        case 20: SimpleAudioEngine::sharedEngine()->playEffect(ext("sounds/skill", "land_5").c_str()); break;
        case 21: SimpleAudioEngine::sharedEngine()->playEffect(ext("sounds/skill", "land_6").c_str()); break;
    }
}
void Sound::PlayDesignatedSound(int idx)
{
    // 기초 스킬 소리 이외의 것들 ( idx : skillNum*10 하고 0번~9번들 사용가능 (즉, 최대 10개) )
    switch (idx)
    {
        case 70: SimpleAudioEngine::sharedEngine()->playEffect(ext("sounds/skill", "fire_8_comet").c_str()); break;
        case 71: SimpleAudioEngine::sharedEngine()->playEffect(ext("sounds/skill", "fire_8_bomb_small").c_str()); break;
        case 72: SimpleAudioEngine::sharedEngine()->playEffect(ext("sounds/skill", "fire_8_bomb_big").c_str()); break;
            
        case 150: SimpleAudioEngine::sharedEngine()->playEffect(ext("sounds/skill", "water_8_goddess_in").c_str()); break;
        case 151: SimpleAudioEngine::sharedEngine()->playEffect(ext("sounds/skill", "water_8_goddess_out").c_str()); break;
        case 152: SimpleAudioEngine::sharedEngine()->playEffect(ext("sounds/skill", "water_8_make_bluepiece").c_str()); break;
        case 153: SimpleAudioEngine::sharedEngine()->playEffect(ext("sounds/skill", "water_8_remove_bluepiece").c_str()); break;
            
        case 230: SimpleAudioEngine::sharedEngine()->playEffect(ext("sounds/skill", "land_8_tree_in").c_str()); break;
        case 231: SimpleAudioEngine::sharedEngine()->playEffect(ext("sounds/skill", "land_8_bomb").c_str()); break;
        case 232: SimpleAudioEngine::sharedEngine()->playEffect(ext("sounds/skill", "land_8_tree_rooting").c_str()); break;
    }
}

void Sound::PlayVoice(int type)
{
    switch (type)
    {
        case VOICE_TITLE: SimpleAudioEngine::sharedEngine()->playEffect(ext("sounds/voice", "title").c_str()); break;
        case VOICE_READY: SimpleAudioEngine::sharedEngine()->playEffect(ext("sounds/voice", "ready").c_str()); break;
        case VOICE_GO: SimpleAudioEngine::sharedEngine()->playEffect(ext("sounds/voice", "go").c_str()); break;
        case VOICE_TIMELIMIT: SimpleAudioEngine::sharedEngine()->playEffect(ext("sounds/voice", "timelimit").c_str()); break;
        case VOICE_GAMEOVER: SimpleAudioEngine::sharedEngine()->playEffect(ext("sounds/voice", "gameover2").c_str()); break;
        case VOICE_BONUS: SimpleAudioEngine::sharedEngine()->playEffect(ext("sounds/voice", "bonus").c_str()); break;
        case VOICE_STOPTIME: SimpleAudioEngine::sharedEngine()->playEffect(ext("sounds/voice", "stoptime").c_str()); break;
        case VOICE_MISSIONSUCCESS: SimpleAudioEngine::sharedEngine()->playEffect(ext("sounds/voice", "missionsuccess").c_str()); break;
        case VOICE_EIT2: SimpleAudioEngine::sharedEngine()->playEffect(ext("sounds/voice", "eit2").c_str()); break;
        case VOICE_EIT3: SimpleAudioEngine::sharedEngine()->playEffect(ext("sounds/voice", "eit3").c_str()); break;
    }
}

void Sound::UnLoadInGameSound()
{
    SimpleAudioEngine::sharedEngine()->stopBackgroundMusic(ext("sounds/game_bgm").c_str());
    
    char name[22];
    for (int i = 1 ; i <= 18 ; i++)
    {
        //sprintf(name, "sounds/pieces/%d.mp3", i);
        sprintf(name, "%d", i);
        SimpleAudioEngine::sharedEngine()->unloadEffect(ext("sounds/pieces", name).c_str());
    }
    
    SimpleAudioEngine::sharedEngine()->unloadEffect(ext("sounds/pieces", "bombA").c_str());
    SimpleAudioEngine::sharedEngine()->unloadEffect(ext("sounds/pieces", "bombB").c_str());
    
    SimpleAudioEngine::sharedEngine()->unloadEffect(ext("sounds/skill", "fire_2").c_str());
    SimpleAudioEngine::sharedEngine()->unloadEffect(ext("sounds/skill", "fire_3_land_4").c_str());
    SimpleAudioEngine::sharedEngine()->unloadEffect(ext("sounds/skill", "fire_5").c_str());
    SimpleAudioEngine::sharedEngine()->unloadEffect(ext("sounds/skill", "fire_6").c_str());
    SimpleAudioEngine::sharedEngine()->unloadEffect(ext("sounds/skill", "fire_7").c_str());
    SimpleAudioEngine::sharedEngine()->unloadEffect(ext("sounds/skill", "fire_8_dragon").c_str());
    SimpleAudioEngine::sharedEngine()->unloadEffect(ext("sounds/skill", "fire_8_comet").c_str());
    SimpleAudioEngine::sharedEngine()->unloadEffect(ext("sounds/skill", "fire_8_bomb_big").c_str());
    SimpleAudioEngine::sharedEngine()->unloadEffect(ext("sounds/skill", "fire_8_bomb_small").c_str());
    
    SimpleAudioEngine::sharedEngine()->unloadEffect(ext("sounds/skill", "water_1").c_str());
    SimpleAudioEngine::sharedEngine()->unloadEffect(ext("sounds/skill", "water_2").c_str());
    SimpleAudioEngine::sharedEngine()->unloadEffect(ext("sounds/skill", "water_3_4").c_str());
    SimpleAudioEngine::sharedEngine()->unloadEffect(ext("sounds/skill", "water_5").c_str());
    SimpleAudioEngine::sharedEngine()->unloadEffect(ext("sounds/skill", "water_6").c_str());
    SimpleAudioEngine::sharedEngine()->unloadEffect(ext("sounds/skill", "water_7").c_str());
    SimpleAudioEngine::sharedEngine()->unloadEffect(ext("sounds/skill", "water_8_goddess_in").c_str());
    SimpleAudioEngine::sharedEngine()->unloadEffect(ext("sounds/skill", "water_8_goddess_out").c_str());
    SimpleAudioEngine::sharedEngine()->unloadEffect(ext("sounds/skill", "water_8_make_bluepiece").c_str());
    SimpleAudioEngine::sharedEngine()->unloadEffect(ext("sounds/skill", "water_8_remove_bluepiece").c_str());
    
    SimpleAudioEngine::sharedEngine()->unloadEffect(ext("sounds/skill", "land_2").c_str());
    SimpleAudioEngine::sharedEngine()->unloadEffect(ext("sounds/skill", "land_5").c_str());
    SimpleAudioEngine::sharedEngine()->unloadEffect(ext("sounds/skill", "land_6").c_str());
    SimpleAudioEngine::sharedEngine()->unloadEffect(ext("sounds/skill", "land_8_tree_in").c_str());
    SimpleAudioEngine::sharedEngine()->unloadEffect(ext("sounds/skill", "land_8_bomb").c_str());
    SimpleAudioEngine::sharedEngine()->unloadEffect(ext("sounds/skill", "land_8_tree_rooting").c_str());
    
    // voices
    SimpleAudioEngine::sharedEngine()->unloadEffect(ext("sounds/voice", "ready").c_str());
    SimpleAudioEngine::sharedEngine()->unloadEffect(ext("sounds/voice", "go").c_str());
    SimpleAudioEngine::sharedEngine()->unloadEffect(ext("sounds/voice", "timelimit").c_str());
    SimpleAudioEngine::sharedEngine()->unloadEffect(ext("sounds/voice", "gameover2").c_str());
    SimpleAudioEngine::sharedEngine()->unloadEffect(ext("sounds/voice", "bonus").c_str());
    SimpleAudioEngine::sharedEngine()->unloadEffect(ext("sounds/voice", "stoptime").c_str());
    SimpleAudioEngine::sharedEngine()->unloadEffect(ext("sounds/voice", "missionsuccess").c_str());
    SimpleAudioEngine::sharedEngine()->unloadEffect(ext("sounds/voice", "eit").c_str());
    SimpleAudioEngine::sharedEngine()->unloadEffect(ext("sounds/voice", "eit2").c_str());
    SimpleAudioEngine::sharedEngine()->unloadEffect(ext("sounds/voice", "eit3").c_str());
    
    SimpleAudioEngine::sharedEngine()->unloadEffect(ext("sounds/clock").c_str());
    SimpleAudioEngine::sharedEngine()->unloadEffect(ext("sounds/game_result").c_str());
}


