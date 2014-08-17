#include "AppDelegate.h"
#include "Splash.h"
#include "puzzle/Puzzle.h"
#include "Ranking.h"
#include "SimpleAudioEngine.h"
USING_NS_CC;

using namespace cocos2d;
using namespace CocosDenshion;

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() 
{
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    CCDirector* pDirector = CCDirector::sharedDirector();
    CCEGLView* pEGLView = CCEGLView::sharedOpenGLView();

    pDirector->setOpenGLView(pEGLView);
    
    // 해상도 : setOpenGLView 이후에 정의되어야 한다.
    pEGLView->setDesignResolutionSize(1080, 1920, kResolutionNoBorder);
	
    // turn on display FPS
    pDirector->setDisplayStats(false);

    // set FPS. the default value is 1.0/60 if you don't call this
    pDirector->setAnimationInterval(1.0 / 60);

    // create a scene. it's an autorelease object
    CCScene *pScene = Splash::scene();
    
    // sound 강제 들고오기
    // if you use SimpleAudioEngine, it must be pause
    SimpleAudioEngine::sharedEngine()->pauseAllEffects();
    SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();

    // run
    pDirector->runWithScene(pScene);

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground()
{
    //CCLog("enter background");
    CCDirector::sharedDirector()->stopAnimation();
    CCDirector::sharedDirector()->pause();
    
    if (isInGameTutorial)
        ;
    
    // (인게임 중일 때 pause된 경우는 실행하면 안 된다)
    // PotionTimer scheduler가 딱 정지된 시점에서 현재 시간을 저장해 둔다.
    // 인게임이 끝나고 다시 UI로 돌아올 때, 벌어진 시간을 갱신해야 하기 떄문이다.
    else if (!isInGame)
    {
        savedTime = savedTime2 = time(0);
        savedMyPotionTime = -1;
    }
    // 인게임 중에는 '일시정지' flag를 세운다.
    else
    {
        // 인게임 중이면 Pause 화면을 띄워준다.
        if (depth.size() > 0 && !isStartGameEnd && Depth::GetCurNameString().substr(0, 6) == "Puzzle")
        {
            if (Depth::GetCurNameString() != "Puzzle_BuyItem")
            {
                void* p = Depth::GetCurPointer();
                ((Puzzle*)p)->GetSound()->ResumeBackgroundInGameSound();
                ((Puzzle*)p)->GetSound()->ResumeAllEffects();
                ((Puzzle*)p)->PauseGame();
            }
        }
    }

    // if you use SimpleAudioEngine, it must be pause
    SimpleAudioEngine::sharedEngine()->pauseAllEffects();
    SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground()
{
    //CCLog("enter foreground");
    CCDirector::sharedDirector()->stopAnimation(); //
    CCDirector::sharedDirector()->resume(); //
    CCDirector::sharedDirector()->startAnimation();
    
    if (isInGameTutorial)
    {
        SimpleAudioEngine::sharedEngine()->resumeAllEffects();
    }
    
    // 시간 갱신 (인게임 중일 때는 할 필요 없다)
    else if (!isInGame)
    {
        if (Depth::GetCurNameString() == "Splash")
            Resume();
        else if (Depth::GetCurNameString() != "Loading" && Depth::GetCurNameString() != "MagicList")
            SessionCheck();
    }
    else
    {
        //CCLog("지금 장면 : %s", Depth::GetCurNameString().c_str());
        if (depth.size() > 0 && (Depth::GetCurNameString() == "RankUp" || Depth::GetCurNameString() == "PuzzleResult"))
        {
            // 게임결과화면, 랭킹변동화면에서는 sessionCheck를 하도록 하자.
            SessionCheck();
            return;
        }
        else
        {
            if (depth.size() > 1 && Depth::GetParentNameString() == "Puzzle")
            {
                //CCLog("foreground : 사운드 1순위로 만들기");
                void* p = Depth::GetParentPointer();
                ((Puzzle*)p)->GetSound()->ResumeBackgroundInGameSound();
                ((Puzzle*)p)->GetSound()->PauseBackgroundInGameSound();
                return;
            }
            else if (!isStartGameEnd && depth.size() == 1 && Depth::GetCurNameString() == "Puzzle")
            {
                //CCLog("게임오버 ~ 게임결과 사이");
                void* p = Depth::GetCurPointer();
                ((Puzzle*)p)->GetSound()->ResumeBackgroundInGameSound();
                ((Puzzle*)p)->GetSound()->PauseBackgroundInGameSound();
                return;
            }
        }
    }
    
    // if you use SimpleAudioEngine, it must resume here
    SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
    SimpleAudioEngine::sharedEngine()->resumeAllEffects();
}


void AppDelegate::SessionCheck()
{
    Common::ShowNextScene(Depth::GetCurPointer(), Depth::GetCurNameString(), "Loading", false, LOADING_MESSAGE);
    
    std::string param = "";
    char temp[40];
    sprintf(temp, "kakao_id=%s", myInfo->GetKakaoId().c_str());
    param += temp;
    
    Network::HttpPost(param, URL_SESSION_CHECK, Depth::GetParentPointer(), httpresponse_selector(AppDelegate::onHttpRequestCompleted));
}


void AppDelegate::onHttpRequestCompleted(CCNode *sender, void *data)
{
    ((Loading*)Depth::GetCurPointer())->EndScene();

    CCHttpResponse* res = (CCHttpResponse*) data;
    
    xml_document xmlDoc;
    Network::GetXMLFromResponseData(res, xmlDoc);
    
    XmlParseSessionCheck(&xmlDoc);
}

void AppDelegate::XmlParseSessionCheck(xml_document *xmlDoc)
{
    xml_node nodeResult = xmlDoc->child("response");
    int code = nodeResult.child("code").text().as_int();
    
    if (code != 0) // ERROR
    {
        if (code <= MAX_COMMON_ERROR_CODE)
            Network::ShowCommonError(code);
    }
    else if (code == 0) // 세션체크 정상
    {
        Resume();
    }
}

void AppDelegate::Resume()
{
    //CCLog("현재위치 : %s", Depth::GetCurName());
    //Depth::DumpDepth();
    
    CCString* param = CCString::create("5");
    CCNotificationCenter::sharedNotificationCenter()->postNotification("Ranking", param);
    
    if (Depth::GetCurNameString() != "Splash")
    {
        // 터치 풀기
        param = CCString::create("10");
        CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetCurName(), param);
    }
    if (Depth::GetCurNameString() != "RankUp" && Depth::GetCurNameString() != "PuzzleResult")
    {
        // if you use SimpleAudioEngine, it must resume here
        SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
        SimpleAudioEngine::sharedEngine()->resumeAllEffects();
    }
}