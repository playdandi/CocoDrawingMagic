#include "Loading.h"

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
    
    pLoading->removeFromParentAndCleanup(true);
    
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
    pLoading = CCSprite::createWithSpriteFrameName("icon/icon_loading_android.png");
    pLoading->setPosition(ccp(m_winSize.width/2, m_winSize.height/2));
    this->addChild(pLoading, 5000);
    pLoading->runAction(CCRepeatForever::create(CCRotateBy::create(0.2f, 72)));
    
	return true;
}

void Loading::EndScene()
{
    //CCLog("Loading : EndScene");
    //CCLog("%p", Depth::GetCurPointer());
    
    //((Loading*)Depth::GetCurPointer())->removeFromParentAndCleanup(true);
    this->removeFromParentAndCleanup(true);
}
