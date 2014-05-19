#include "AppDelegate.h"
#include "Splash.h"
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

    // run
    pDirector->runWithScene(pScene);

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground()
{
    CCLog("enter background");
    CCDirector::sharedDirector()->stopAnimation();
    CCDirector::sharedDirector()->pause();
    
    // (인게임 중일 때 pause된 경우는 실행하면 안 된다)
    // PotionTimer scheduler가 딱 정지된 시점에서 현재 시간을 저장해 둔다.
    // 인게임이 끝나고 다시 UI로 돌아올 때, 벌어진 시간을 갱신해야 하기 떄문이다.
    if (!isInGame)
        savedTime = time(0);

    // if you use SimpleAudioEngine, it must be pause
    SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
    //SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    CCLog("enter foreground");
    CCDirector::sharedDirector()->stopAnimation(); //
    CCDirector::sharedDirector()->resume(); //
    CCDirector::sharedDirector()->startAnimation();
    
    // 시간 갱신 (인게임 중일 때는 할 필요 없다)
    if (!isInGame)
    {
        CCString* param = CCString::create("5");
        CCNotificationCenter::sharedNotificationCenter()->postNotification("Ranking", param);
    }

    // if you use SimpleAudioEngine, it must resume here
    SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
    //SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
}
