#include "PuzzleResult.h"

using namespace cocos2d;

CCScene* PuzzleResult::scene()
{
    CCScene* pScene = CCScene::create();
    PuzzleResult* pLayer = PuzzleResult::create();
    pScene->addChild(pLayer);
    
	return pScene;
}

void PuzzleResult::onEnter()
{
    CCLog("PuzzleResult :: onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
    CCLayer::onEnter();
}
void PuzzleResult::onExit()
{
    CCLog("PuzzleResult :: onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
}

void PuzzleResult::keyBackClicked()
{
    CCDirector::sharedDirector()->end();
}


bool PuzzleResult::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
    //
    //res_allCnt
    
    //winSize = CCDirector::sharedDirector()->getWinSize();
    //CCLog("%d %d", winSize.width, winSize.height);
    winSize = CCSize(768, 1024);
    
    // load images
    tBackground = CCTextureCache::sharedTextureCache()->addImage("images/bg2.png");
    
    InitSprites();
    
    return true;
}

void PuzzleResult::InitSprites()
{
    /*
    pBlack = new CCSprite();
    pBlack->initWithTexture(tBackground, CCRectMake(0, 0, winSize.width, winSize.height));
    pBlack->setPosition(ccp(0, 0));
    pBlack->setAnchorPoint(ccp(0, 0));
    pBlack->setColor(ccc3(0, 0, 0));
    pBlack->setOpacity(120);
    this->addChild(pBlack, 0);
    
    // popup
    CCSprite* pBackground = new CCSprite();
    pBackground->initWithTexture(tBackground, CCRectMake(0, 0, 500, 800));
    pBackground->setAnchorPoint(ccp(0.5, 0.5));
    pBackground->setPosition(ccp(winSize.width/2, winSize.height/2));
    this->addChild(pBackground);
    
    // labels
    int allCnt = 0;
    char cont[50];
    for (int i = 3; i <= 10; i++) {
        CCLabelTTF* temp = CCLabelTTF::create("s", "Arial", 30);
        sprintf(cont, "%d개 한붓그리기 : %d 회", i, res_allCnt[i]);
        temp->setString(cont);
        temp->setAnchorPoint(ccp(0.5, 0.5));
        temp->setPosition(ccp(pBackground->getContentSize().width/2 , 200+(10-i)*60));
        temp->setColor(ccc3(0, 0, 0));
        pBackground->addChild(temp);
        
        // 초 노 빨 파 회
        CCLabelTTF* temptemp = CCLabelTTF::create("s", "Arial", 22);
        char color[100];
        sprintf(color, "초(%d), 노(%d), 빨(%d), 파(%d), 회(%d)", res_colorCnt[i][0], res_colorCnt[i][1],
                res_colorCnt[i][2], res_colorCnt[i][3], res_colorCnt[i][4]);
        temptemp->setString(color);
        temptemp->setAnchorPoint(ccp(0.5, 1));
        temptemp->setColor(ccc3(0, 0, 0));
        temptemp->setPosition(ccp(temp->getContentSize().width/2, 0));
        temp->addChild(temptemp);
        
        allCnt += res_allCnt[i];
    }
    
    CCLabelTTF* all = CCLabelTTF::create("s", "Arial", 50);
    sprintf(cont, "총 %d 회 한붓그리기", allCnt);
    all->setString(cont);
    all->setAnchorPoint(ccp(0.5, 0.5));
    all->setPosition(ccp(pBackground->getContentSize().width/2, 750));
    all->setColor(ccc3(0, 0, 0));
    pBackground->addChild(all);
    
    CCLabelTTF* cycle = CCLabelTTF::create("s", "Arial", 40);
    sprintf(cont, "CYCLE 회수 : %d 회", res_cycleCnt);
    cycle->setString(cont);
    cycle->setAnchorPoint(ccp(0.5, 0.5));
    cycle->setPosition(ccp(pBackground->getContentSize().width/2, 50));
    cycle->setColor(ccc3(0, 0, 0));
    pBackground->addChild(cycle);
    */
    
    //CCActionInterval* action = CCMoveTo::create(0.2f, ccp(0, 0));
    //pBackground->runAction(action);
    //this->addChild(pBackground, 0);
}


bool PuzzleResult::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    //CCPoint point = pTouch->getLocation();
    return true;
}


void PuzzleResult::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
}

void PuzzleResult::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
}

void PuzzleResult::EndLayer()
{
    CCFiniteTimeAction* action =
    CCSequence::create(CCMoveTo::create(0.2f, ccp(0, -winSize.height)),
                       CCCallFunc::create(this, callfunc_selector(PuzzleResult::EndLayerCallback)), NULL);
    
    pBackground->runAction(action);
}

void PuzzleResult::EndLayerCallback()
{
    //this->removeChild(pBackground);
    //this->removeChild(pBlack);
    this->removeAllChildren();
    //this->removeAllComponents();
    pBackground->autorelease();
    pBlack->autorelease();
    pBackground = NULL;
    pBlack = NULL;
    
    this->removeFromParentAndCleanup(true);
    
    //CCTextureCache::sharedTextureCache()->dumpCachedTextureInfo();
}