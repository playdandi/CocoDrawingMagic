#include "Loading.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
//#include "bridge.h"
//#import "EAGLView.h"
#endif

static int status;

Loading::~Loading(void)
{
    //CCLog("Loading 소멸자");
}

CCScene* Loading::scene(int stat)
{
    status = stat;
    
	CCScene* pScene = CCScene::create();
	Loading* pLayer = Loading::create();
	pScene->addChild(pLayer);
    
	return pScene;
}

void Loading::onEnter()
{
    //CCLog("Loading :: onEnter");
    CCLayer::onEnter();
}

void Loading::onExit()
{
    //CCLog("Loading :: onExit");
    CCLayer::onExit();
    
    //#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    pLoading->removeFromParentAndCleanup(true);
    //#endif
    #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("images/iosloading.plist");
    CCTextureCache::sharedTextureCache()->removeTextureForKey("images/iosloading.png");
    #endif
    
    // release depth tree
    Depth::RemoveCurDepth();
    
    // touch 넘겨주기 (GetCurName = 위에서 remove 했기 때문에 결국 여기 입장에서는 부모다)
    CCString* param = CCString::create("-1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetCurName(), param);
}

bool Loading::init()
{
	if (!CCLayer::init())
		return false;
    
    m_winSize = CCDirector::sharedDirector()->getWinSize();
    loadingSprites = false;
    
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);

    // make depth tree
    Depth::AddCurDepth("Loading", this);
    
    // notification post
    CCString* param = CCString::create("1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetParentName(), param);
    
    #if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    pLoading = CCSprite::createWithSpriteFrameName("icon/icon_loading_android.png");
    pLoading->setPosition(ccp(m_winSize.width/2, m_winSize.height/2));
    this->addChild(pLoading, 5000);
    pLoading->runAction(CCRepeatForever::create(CCRotateBy::create(0.2f, 72)));
    #endif
    
    #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    //CCLog("hi");
    //AddLoadingView((EAGLView*)CCDirector::sharedDirector()->getOpenGLView());
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("images/iosloading.plist");
    iOSLoading();
    #endif
    
	return true;
}

void Loading::iOSLoading()
{
    CCArray* animFrames = CCArray::createWithCapacity(12);
    char name[35];
    for (int j = 0 ; j < 12 ; j++)
    {
        sprintf(name, "ccactivityindicator_%d.png", j+1);
        CCSpriteFrame* frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(name);
        animFrames->addObject(frame);
    }
    
    pLoading = CCSprite::createWithSpriteFrameName("ccactivityindicator_1.png");
    pLoading->setAnchorPoint(ccp(0.5, 0.5));
    pLoading->setPosition(ccp(m_winSize.width/2, m_winSize.height/2));
    this->addChild(pLoading, 5000);
    
    CCAnimation* animation = CCAnimation::createWithSpriteFrames(animFrames, 0.05f);
    CCAnimate* animate = CCAnimate::create(animation);
    //CCFiniteTimeAction* action = CCSequence::create(animate, CCCallFuncND::create(this, callfuncND_selector(Effect::PlayEffectCallback), NULL), NULL);
    pLoading->runAction(CCRepeatForever::create(animate));
    
    CCLog("ok");

    animFrames->removeAllObjects();
}

void Loading::EndScene()
{
    //CCLog("Loading : EndScene");
    //CCLog("%p", Depth::GetCurPointer());
    
    //((Loading*)Depth::GetCurPointer())->removeFromParentAndCleanup(true);
    this->removeFromParentAndCleanup(true);
}
