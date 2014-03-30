#include "SoundGame.h"
#include "SimpleAudioEngine.h"

using namespace cocos2d;
using namespace CocosDenshion;

void SoundGame::PreLoadSound()
{
    SimpleAudioEngine::sharedEngine()->preloadBackgroundMusic("sounds/game_bgm.mp3");
    
    SetEffectVolume();
    SetBackgroundMusicVolume();
}

void SoundGame::SetEffectVolume()
{
    SimpleAudioEngine::sharedEngine()->setEffectsVolume(0);
}
void SoundGame::SetBackgroundMusicVolume()
{
    SimpleAudioEngine::sharedEngine()->setBackgroundMusicVolume(0.7);
}


void SoundGame::PlayBackgroundSound()
{
    SimpleAudioEngine::sharedEngine()->playBackgroundMusic("sounds/game_bgm.mp3", true);
}

void SoundGame::StopBackgroundSound()
{
    SimpleAudioEngine::sharedEngine()->stopBackgroundMusic();
}
