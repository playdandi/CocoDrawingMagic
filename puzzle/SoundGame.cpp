/*
#include "SoundGame.h"
#include "SimpleAudioEngine.h"

using namespace cocos2d;
using namespace CocosDenshion;

void SoundGame::PreLoadSound()
{
    SimpleAudioEngine::sharedEngine()->preloadBackgroundMusic("sounds/game_bgm.mp3");

    char name[22];
    for (int i = 1 ; i <= 18 ; i++)
    {
        sprintf(name, "sounds/pieces/%d.mp3", i);
        SimpleAudioEngine::sharedEngine()->preloadEffect(name);
    }
    SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/pieces/bomb.mp3");
//    SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/pieces/bomb2.mp3");
    
    SetEffectVolume();
    SetBackgroundMusicVolume(0.6f);
}

void SoundGame::SetEffectVolume()
{
    SimpleAudioEngine::sharedEngine()->setEffectsVolume(0.7f);
}
void SoundGame::SetBackgroundMusicVolume(float vol)
{
    SimpleAudioEngine::sharedEngine()->setBackgroundMusicVolume(vol);
}


void SoundGame::PlayBackgroundSound()
{
    SimpleAudioEngine::sharedEngine()->playBackgroundMusic("sounds/game_bgm.mp3", true);
}

void SoundGame::StopBackgroundSound()
{
    SimpleAudioEngine::sharedEngine()->stopBackgroundMusic();
}


void SoundGame::PlayPieceClick(int idx)
{
    if (idx % 18 > 0)
        idx = idx % 18;
    char name[22];
    sprintf(name, "sounds/pieces/%d.mp3", idx);
    SimpleAudioEngine::sharedEngine()->playEffect(name);
}

void SoundGame::PlayBomb()
{
    SimpleAudioEngine::sharedEngine()->playEffect("sounds/pieces/bomb.mp3");
}

void SoundGame::PlaySkillSound(int skillNum)
{
    if (skillNum == 5)
        SimpleAudioEngine::sharedEngine()->playEffect("sounds/pieces/fire_6.mp3");
}
*/