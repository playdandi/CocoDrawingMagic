#include "Loading_GameStart.h"

CCScene* Loading_GameStart::scene()
{
	CCScene* pScene = CCScene::create();
	Loading_GameStart* pLayer = Loading_GameStart::create();
	pScene->addChild(pLayer);
    
	return pScene;
}

void Loading_GameStart::onEnter()
{
    CCLog("Loading_GameStart :: onEnter");
    CCLayer::onEnter();
}

void Loading_GameStart::onExit()
{
    CCLog("Loading_GameStart :: onExit");
    CCLayer::onExit();
}

bool Loading_GameStart::init()
{
	if (!CCLayer::init())
		return false;
    
    // make depth tree
    Depth::AddCurDepth("Loading_GameStart", this);

    m_winSize = CCDirector::sharedDirector()->getWinSize();
    
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("images/game3.plist");
    LoadingSprites();
    
	return true;
}

void Loading_GameStart::LoadingSprites()
{   
    pCoco = CCSprite::createWithSpriteFrameName("loading/coco_loading.png");
    pCoco->setPosition(ccp(m_winSize.width/2, m_winSize.height/2));
    pCoco->setScale(1.3f);
    this->addChild(pCoco, 5);
    
    // tip
    CCPoint vo = CCDirector::sharedDirector()->getVisibleOrigin();
    pTip = CCSprite::createWithSpriteFrameName("loading/tip.png");
    pTip->setAnchorPoint(ccp(0.5, 0));
    pTip->setPosition(ccp(m_winSize.width/2, vo.y+100));
    this->addChild(pTip, 5);
    
    CCSize t = pTip->getContentSize();
    
    // tip message
    pTipMsg = CCLabelTTF::create(Common::GetTip().c_str(), fontList[0].c_str(), 32, CCSize(774, 126), kCCTextAlignmentLeft, kCCVerticalTextAlignmentCenter);
    pTipMsg->setColor(ccc3(255,255,255));
    pTipMsg->setAnchorPoint(ccp(0, 0.5));
    pTipMsg->setPosition(ccp(m_winSize.width/2-t.width/2+170, vo.y+100+t.height/2-10));
    this->addChild(pTipMsg, 6);
    
    
    char name[25];
    CCArray* animFrames = CCArray::createWithCapacity(2);
    for (int j = 0 ; j < 2 ; j++)
    {
        sprintf(name, "loading/loading%d.png", j+1);
        CCSpriteFrame* frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(name);
        animFrames->addObject(frame);
    }
    
    pLoading = CCSprite::createWithSpriteFrameName("loading/loading1.png");
    pLoading->setAnchorPoint(ccp(0.5, 0.5));
    pLoading->setPosition(ccp(m_winSize.width/2, m_winSize.height/2-150-20));
    pLoading->setScale(1.3f);
    this->addChild(pLoading, 5);
    
    CCAnimation* animation = CCAnimation::createWithSpriteFrames(animFrames, 0.1f);
    CCAnimate* animate = CCAnimate::create(animation);
    pLoading->runAction(CCRepeatForever::create(animate));
    
    animFrames->removeAllObjects();
}


void Loading_GameStart::EndScene()
{
    // release depth tree
    Depth::RemoveCurDepth();
    
    CCLog("Loading_GameStart : EndScene");
    pCoco->removeFromParentAndCleanup(true);
    pLoading->removeFromParentAndCleanup(true);
    pTip->removeFromParentAndCleanup(true);
    pTipMsg->removeFromParentAndCleanup(true);
    
    this->removeFromParentAndCleanup(true);
}
