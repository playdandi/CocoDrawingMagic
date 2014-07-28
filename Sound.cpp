#include "Sound.h"


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
    
    engine = SimpleAudioEngine::sharedEngine();
    engine->preloadBackgroundMusic(ext("sounds/bgm").c_str());
    engine->preloadEffect(ext("sounds/click").c_str());
    engine->preloadEffect(ext("sounds/clickboard").c_str());
    engine->preloadEffect(ext("sounds/lvup_success").c_str());
    engine->preloadEffect(ext("sounds/lvup_fail").c_str());
    engine->preloadEffect(ext("sounds/magiclist").c_str());
    engine->preloadEffect(ext("sounds/warning").c_str());
    engine->preloadEffect(ext("sounds/gamestart").c_str());
    
    SetEffectVolume();
    SetBackgroundMusicVolume(0.7f);
}

void Sound::SetEffectVolume()
{
    if (CCUserDefault::sharedUserDefault()->getBoolForKey("setting_option_0"))
        engine->setEffectsVolume(0.9);
    else
        engine->setEffectsVolume(0);
}
void Sound::SetBackgroundMusicVolume(float vol)
{
    engine->setBackgroundMusicVolume(vol);
}

void Sound::PlayBackgroundSound()
{
    engine->playBackgroundMusic(ext("sounds/bgm").c_str(), true);
}
void Sound::StopBackgroundSound()
{
    engine->stopBackgroundMusic();
}
void Sound::PauseBackgroundSound()
{
    engine->pauseBackgroundMusic();
}
void Sound::ResumeBackgroundSound()
{
    engine->resumeBackgroundMusic();
}

void Sound::StopAllEffects()
{
    engine->stopAllEffects();
}
void Sound::PauseAllEffects()
{
    engine->pauseAllEffects();
}
void Sound::ResumeAllEffects()
{
    engine->resumeAllEffects();
}

void Sound::playClick()
{
    engine->playEffect(ext("sounds/click").c_str());
}
void Sound::playClickboard()
{
    engine->playEffect(ext("sounds/clickboard").c_str());
}

void Sound::playWarning()
{
    engine->playEffect(ext("sounds/warning").c_str());
}

void Sound::playBoardMove()
{
    engine->playEffect(ext("sounds/magiclist").c_str());
}

void Sound::playLvUpSuccess()
{
    engine->playEffect(ext("sounds/lvup_success").c_str());
}
void Sound::playLvUpFail()
{
    engine->playEffect(ext("sounds/lvup_fail").c_str());
}
void Sound::playGameStart()
{
    engine->playEffect(ext("sounds/gamestart").c_str());
}
void Sound::UnLoadSound()
{
    
}


////////////////////////////////////////////////////////////////////////////////////////////////
void Sound::PreLoadInGameSound()
{
    engine = SimpleAudioEngine::sharedEngine();
    engine->preloadBackgroundMusic(ext("sounds/game_bgm").c_str());
    
    char name[22];
    for (int i = 1 ; i <= 18 ; i++)
    {
        sprintf(name, "%d", i);
        engine->preloadEffect(ext("sounds/pieces", name).c_str());
    }
    engine->preloadEffect(ext("sounds/pieces", "cycle").c_str());
    
    engine->preloadEffect(ext("sounds/pieces", "bombA").c_str());
    engine->preloadEffect(ext("sounds/pieces", "bombB").c_str());
    
    engine->preloadEffect(ext("sounds/skill", "fire_2").c_str());
    engine->preloadEffect(ext("sounds/skill", "fire_3_land_4").c_str());
    engine->preloadEffect(ext("sounds/skill", "fire_5").c_str());
    engine->preloadEffect(ext("sounds/skill", "fire_6").c_str());
    engine->preloadEffect(ext("sounds/skill", "fire_7").c_str());
    engine->preloadEffect(ext("sounds/skill", "fire_8_dragon").c_str());
    engine->preloadEffect(ext("sounds/skill", "fire_8_comet").c_str());
    engine->preloadEffect(ext("sounds/skill", "fire_8_bomb_big").c_str());
    engine->preloadEffect(ext("sounds/skill", "fire_8_bomb_small").c_str());
    
    engine->preloadEffect(ext("sounds/skill", "water_1").c_str());
    engine->preloadEffect(ext("sounds/skill", "water_2").c_str());
    engine->preloadEffect(ext("sounds/skill", "water_3_4").c_str());
    engine->preloadEffect(ext("sounds/skill", "water_5").c_str());
    engine->preloadEffect(ext("sounds/skill", "water_6").c_str());
    engine->preloadEffect(ext("sounds/skill", "water_7").c_str());
    engine->preloadEffect(ext("sounds/skill", "water_8_goddess_in").c_str());
    engine->preloadEffect(ext("sounds/skill", "water_8_goddess_out").c_str());
    engine->preloadEffect(ext("sounds/skill", "water_8_make_bluepiece").c_str());
    engine->preloadEffect(ext("sounds/skill", "water_8_remove_bluepiece").c_str());
    
    engine->preloadEffect(ext("sounds/skill", "land_2").c_str());
    engine->preloadEffect(ext("sounds/skill", "land_5").c_str());
    engine->preloadEffect(ext("sounds/skill", "land_6").c_str());
    engine->preloadEffect(ext("sounds/skill", "land_8_tree_in").c_str());
    engine->preloadEffect(ext("sounds/skill", "land_8_bomb").c_str());
    engine->preloadEffect(ext("sounds/skill", "land_8_tree_rooting").c_str());
    
    // voices
    engine->preloadEffect(ext("sounds/voice", "ready").c_str());
    engine->preloadEffect(ext("sounds/voice", "go").c_str());
    engine->preloadEffect(ext("sounds/voice", "timelimit").c_str());
    engine->preloadEffect(ext("sounds/voice", "gameover2").c_str());
    engine->preloadEffect(ext("sounds/voice", "bonus").c_str());
    engine->preloadEffect(ext("sounds/voice", "stoptime").c_str());
    engine->preloadEffect(ext("sounds/voice", "missionsuccess").c_str());
    engine->preloadEffect(ext("sounds/voice", "eit").c_str());
    engine->preloadEffect(ext("sounds/voice", "eit2").c_str());
    engine->preloadEffect(ext("sounds/voice", "eit3").c_str());
    engine->preloadEffect(ext("sounds/voice", "cocotime").c_str());
    
    engine->preloadEffect(ext("sounds/clock").c_str());
    engine->preloadEffect(ext("sounds/game_result").c_str());
    engine->preloadEffect(ext("sounds/game_result_bg").c_str());
    
    SetEffectVolume();
    SetBackgroundMusicVolume(0.5f);
}

void Sound::ReduceBackgroundMusicVolume(int volCnt)
{
    CCLog("%d", volCnt);
    float vol = 0.8f * ((float)volCnt/(float)10.0f);
    CCLog("reduce Volume = %f", vol);
    engine->setBackgroundMusicVolume(vol);
}
void Sound::GainBackgroundMusicVolume(int volCnt)
{
    //volCnt++;
    //float vol = 0.5f * ((float)volCnt/(float)15.0f);
    //float vol = (float)(volCnt*volCnt) * ((float)1.0f / (float)450.0f); // y = (1/450)x^2 (x = 15일 때 0.5볼륨)
    float vol = (float)(volCnt*volCnt) * ((float)1.0f / (float)200.0f); // y = (1/200)x^2 (x = 10일 때 0.5볼륨)
    engine->setBackgroundMusicVolume(vol);
}
void Sound::PlayBackgroundInGameSound(int type)
{
    // type : 0(처음시작), 1(기본->피버), 2(피버->기본)
    if (type == 0)
    {
        //volCnt = 15;
        engine->playBackgroundMusic(ext("sounds/game_bgm").c_str(), true);
    }
    else if (type == 1)
    {
        engine->stopBackgroundMusic();
        SetBackgroundMusicVolume(0.8f);
        //volCnt = 15;
        engine->playBackgroundMusic(ext("sounds/magictime_bgm").c_str(), true);
    }
    else if (type == 2)
    {
        engine->stopBackgroundMusic();
        //SetBackgroundMusicVolume(0.5f);
        //volCnt = 0;
        engine->playBackgroundMusic(ext("sounds/game_bgm").c_str(), true);
    }
}
int Sound::GetVolumeCnt()
{
    //return volCnt;
    return 0;
}
void Sound::PauseBackgroundInGameSound()
{
    engine->pauseBackgroundMusic();
}
void Sound::ResumeBackgroundInGameSound()
{
    engine->resumeBackgroundMusic();
}
void Sound::StopBackgroundInGameSound()
{
    engine->stopBackgroundMusic();
}

void Sound::PlayPieceClick(int idx)
{
    if (idx % 18 > 0)
        idx = idx % 18;
    char name[22];
    //sprintf(name, "sounds/pieces/%d.mp3", idx);
    sprintf(name, "%d", idx);
    engine->playEffect(ext("sounds/pieces", name).c_str());
}
void Sound::PlayCycle()
{
    engine->playEffect(ext("sounds/pieces", "cycle").c_str());
}

void Sound::PlayItemPaint()
{
    engine->playEffect(ext("sounds/item", "paint").c_str());
}
void Sound::PlayItemStaff()
{
    engine->playEffect(ext("sounds/item", "staff").c_str());
}
void Sound::PlayBomb()
{
    engine->playEffect(ext("sounds/pieces", "bombA").c_str());
    engine->playEffect(ext("sounds/pieces", "bombB").c_str());
}

void Sound::PlayGameResult()
{
    engine->playEffect(ext("sounds/game_result_bg").c_str()); // 배경효과음
    nGameResultId = engine->playEffect(ext("sounds/game_result").c_str(), true); // 또또또또 효과음
}
void Sound::StopGameResult()
{
    engine->stopEffect(nGameResultId);
}
void Sound::PlayClock()
{
    nClockSoundId = engine->playEffect(ext("sounds/clock").c_str(), true);
}
void Sound::StopClock()
{
    engine->stopAllEffects();
}

void Sound::PlaySkillSound(int skillNum)
{
    switch (skillNum)
    {
        case 0: engine->playEffect(ext("sounds/skill", "fire_2").c_str()); break;
        case 1: engine->playEffect(ext("sounds/skill", "fire_2").c_str()); break;
        case 2: engine->playEffect(ext("sounds/skill", "fire_3_land_4").c_str()); break;
        case 4: engine->playEffect(ext("sounds/skill", "fire_5").c_str()); break;
        case 5: engine->playEffect(ext("sounds/skill", "fire_6").c_str()); break;
        case 6: engine->playEffect(ext("sounds/skill", "fire_7").c_str()); break;
        case 7: engine->playEffect(ext("sounds/skill", "fire_8_dragon").c_str()); break;
        
        case 8: engine->playEffect(ext("sounds/skill", "water_1").c_str()); break;
        case 9: engine->playEffect(ext("sounds/skill", "water_2").c_str()); break;
        case 10: engine->playEffect(ext("sounds/skill", "water_3_4").c_str()); break;
        case 11: engine->playEffect(ext("sounds/skill", "water_3_4").c_str()); break;
        case 12: engine->playEffect(ext("sounds/skill", "water_5").c_str()); break;
        case 13: engine->playEffect(ext("sounds/skill", "water_6").c_str()); break;
        case 14: engine->playEffect(ext("sounds/skill", "water_7").c_str()); break;
            
        case 17: engine->playEffect(ext("sounds/skill", "land_2").c_str()); break;
        case 18: engine->playEffect(ext("sounds/skill", "fire_3_land_4").c_str()); break;
        case 19: engine->playEffect(ext("sounds/skill", "fire_3_land_4").c_str()); break;
        case 20: engine->playEffect(ext("sounds/skill", "land_5").c_str()); break;
        case 21: engine->playEffect(ext("sounds/skill", "land_6").c_str()); break;
    }
}
void Sound::PlayDesignatedSound(int idx)
{
    // 기초 스킬 소리 이외의 것들 ( idx : skillNum*10 하고 0번~9번들 사용가능 (즉, 최대 10개) )
    switch (idx)
    {
        case 70: engine->playEffect(ext("sounds/skill", "fire_8_comet").c_str()); break;
        case 71: engine->playEffect(ext("sounds/skill", "fire_8_bomb_small").c_str()); break;
        case 72: engine->playEffect(ext("sounds/skill", "fire_8_bomb_big").c_str()); break;
            
        case 150: engine->playEffect(ext("sounds/skill", "water_8_goddess_in").c_str()); break;
        case 151: engine->playEffect(ext("sounds/skill", "water_8_goddess_out").c_str()); break;
        case 152: engine->playEffect(ext("sounds/skill", "water_8_make_bluepiece").c_str()); break;
        case 153: engine->playEffect(ext("sounds/skill", "water_8_remove_bluepiece").c_str()); break;
            
        case 230: engine->playEffect(ext("sounds/skill", "land_8_tree_in").c_str()); break;
        case 231: engine->playEffect(ext("sounds/skill", "land_8_bomb").c_str()); break;
        case 232: engine->playEffect(ext("sounds/skill", "land_8_tree_rooting").c_str()); break;
    }
}

void Sound::PlayVoice(int type)
{
    switch (type)
    {
        case VOICE_TITLE: engine->playEffect(ext("sounds/voice", "title").c_str()); break;
        case VOICE_READY: engine->playEffect(ext("sounds/voice", "ready").c_str()); break;
        case VOICE_GO: engine->playEffect(ext("sounds/voice", "go").c_str()); break;
        case VOICE_TIMELIMIT: engine->playEffect(ext("sounds/voice", "timelimit").c_str()); break;
        case VOICE_GAMEOVER: engine->playEffect(ext("sounds/voice", "gameover2").c_str()); break;
        case VOICE_BONUS: engine->playEffect(ext("sounds/voice", "bonus").c_str()); break;
        case VOICE_STOPTIME: engine->playEffect(ext("sounds/voice", "stoptime").c_str()); break;
        case VOICE_MISSIONSUCCESS: engine->playEffect(ext("sounds/voice", "missionsuccess").c_str()); break;
        case VOICE_EIT2: engine->playEffect(ext("sounds/voice", "eit2").c_str()); break;
        case VOICE_EIT3: engine->playEffect(ext("sounds/voice", "eit3").c_str()); break;
        case VOICE_COCOTIME : engine->playEffect(ext("sounds/voice", "cocotime").c_str()); break;
    }
}

void Sound::UnLoadInGameSound()
{
    engine->stopBackgroundMusic(ext("sounds/game_bgm").c_str());
    
    char name[22];
    for (int i = 1 ; i <= 18 ; i++)
    {
        //sprintf(name, "sounds/pieces/%d.mp3", i);
        sprintf(name, "%d", i);
        engine->unloadEffect(ext("sounds/pieces", name).c_str());
    }
    engine->unloadEffect(ext("sounds/pieces", "cycle").c_str());
    
    engine->unloadEffect(ext("sounds/pieces", "bombA").c_str());
    engine->unloadEffect(ext("sounds/pieces", "bombB").c_str());
    
    engine->unloadEffect(ext("sounds/skill", "fire_2").c_str());
    engine->unloadEffect(ext("sounds/skill", "fire_3_land_4").c_str());
    engine->unloadEffect(ext("sounds/skill", "fire_5").c_str());
    engine->unloadEffect(ext("sounds/skill", "fire_6").c_str());
    engine->unloadEffect(ext("sounds/skill", "fire_7").c_str());
    engine->unloadEffect(ext("sounds/skill", "fire_8_dragon").c_str());
    engine->unloadEffect(ext("sounds/skill", "fire_8_comet").c_str());
    engine->unloadEffect(ext("sounds/skill", "fire_8_bomb_big").c_str());
    engine->unloadEffect(ext("sounds/skill", "fire_8_bomb_small").c_str());
    
    engine->unloadEffect(ext("sounds/skill", "water_1").c_str());
    engine->unloadEffect(ext("sounds/skill", "water_2").c_str());
    engine->unloadEffect(ext("sounds/skill", "water_3_4").c_str());
    engine->unloadEffect(ext("sounds/skill", "water_5").c_str());
    engine->unloadEffect(ext("sounds/skill", "water_6").c_str());
    engine->unloadEffect(ext("sounds/skill", "water_7").c_str());
    engine->unloadEffect(ext("sounds/skill", "water_8_goddess_in").c_str());
    engine->unloadEffect(ext("sounds/skill", "water_8_goddess_out").c_str());
    engine->unloadEffect(ext("sounds/skill", "water_8_make_bluepiece").c_str());
    engine->unloadEffect(ext("sounds/skill", "water_8_remove_bluepiece").c_str());
    
    engine->unloadEffect(ext("sounds/skill", "land_2").c_str());
    engine->unloadEffect(ext("sounds/skill", "land_5").c_str());
    engine->unloadEffect(ext("sounds/skill", "land_6").c_str());
    engine->unloadEffect(ext("sounds/skill", "land_8_tree_in").c_str());
    engine->unloadEffect(ext("sounds/skill", "land_8_bomb").c_str());
    engine->unloadEffect(ext("sounds/skill", "land_8_tree_rooting").c_str());
    
    // voices
    engine->unloadEffect(ext("sounds/voice", "ready").c_str());
    engine->unloadEffect(ext("sounds/voice", "go").c_str());
    engine->unloadEffect(ext("sounds/voice", "timelimit").c_str());
    engine->unloadEffect(ext("sounds/voice", "gameover2").c_str());
    engine->unloadEffect(ext("sounds/voice", "bonus").c_str());
    engine->unloadEffect(ext("sounds/voice", "stoptime").c_str());
    engine->unloadEffect(ext("sounds/voice", "missionsuccess").c_str());
    engine->unloadEffect(ext("sounds/voice", "eit").c_str());
    engine->unloadEffect(ext("sounds/voice", "eit2").c_str());
    engine->unloadEffect(ext("sounds/voice", "eit3").c_str());
    engine->unloadEffect(ext("sounds/voice", "cocotime").c_str());
    
    engine->unloadEffect(ext("sounds/clock").c_str());
    engine->unloadEffect(ext("sounds/game_result").c_str());
    engine->unloadEffect(ext("sounds/game_result_bg").c_str());
}


