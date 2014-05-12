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

    SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/pieces/fire_2.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/pieces/fire_3_land_4.mp3");
    //SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/pieces/fire_5.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/pieces/fire_6.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/pieces/fire_7.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/pieces/fire_8_dragon.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/pieces/fire_8_comet.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/pieces/water_2.mp3");
    //SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/pieces/water_5.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/pieces/water_6.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/pieces/water_7.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/pieces/land_6.mp3");
    
    
    SetEffectVolume();
    SetBackgroundMusicVolume(0.7f);
}


void Sound::PlayBackgroundInGameSound()
{
    SimpleAudioEngine::sharedEngine()->playBackgroundMusic("sounds/game_bgm.mp3", true);
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

void Sound::PlaySkillSound(int skillNum)
{
    switch (skillNum)
    {
        case 1: SimpleAudioEngine::sharedEngine()->playEffect("sounds/pieces/fire_2.mp3"); break;
        case 2: SimpleAudioEngine::sharedEngine()->playEffect("sounds/pieces/fire_3_land_4.mp3"); break;
        //case 4: SimpleAudioEngine::sharedEngine()->playEffect("sounds/pieces/fire_5.mp3"); break;
        case 5: SimpleAudioEngine::sharedEngine()->playEffect("sounds/pieces/fire_6.mp3"); break;
        case 6: SimpleAudioEngine::sharedEngine()->playEffect("sounds/pieces/fire_7.mp3"); break;
        case 7: SimpleAudioEngine::sharedEngine()->playEffect("sounds/pieces/fire_8_dragon.mp3"); break;
        case 9: SimpleAudioEngine::sharedEngine()->playEffect("sounds/pieces/water_2.mp3"); break;
        //case 12: SimpleAudioEngine::sharedEngine()->playEffect("sounds/pieces/water_5.mp3"); break;
        case 13: SimpleAudioEngine::sharedEngine()->playEffect("sounds/pieces/water_6.mp3"); break;
        case 14: SimpleAudioEngine::sharedEngine()->playEffect("sounds/pieces/water_7.mp3"); break;
        case 19: SimpleAudioEngine::sharedEngine()->playEffect("sounds/pieces/fire_3_land_4.mp3"); break;
        case 21: SimpleAudioEngine::sharedEngine()->playEffect("sounds/pieces/land_6.mp3"); break;
    }
}
void Sound::PlayDesginatedSound(int idx)
{
    switch (idx)
    {
        case 70: SimpleAudioEngine::sharedEngine()->playEffect("sounds/pieces/fire_8_comet.mp3"); break;
    }
}



