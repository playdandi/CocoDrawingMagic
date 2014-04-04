#include "SoundGame.h"
#include "SimpleAudioEngine.h"

using namespace cocos2d;
using namespace CocosDenshion;

void SoundGame::PreLoadSound()
{
    SimpleAudioEngine::sharedEngine()->preloadBackgroundMusic("sounds/game_bgm.mp3");
    
    SetEffectVolume();
    SetBackgroundMusicVolume(0.7f);
}

void SoundGame::SetEffectVolume()
{
    SimpleAudioEngine::sharedEngine()->setEffectsVolume(0);
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
