#include "T_Skip.h"
#include "T_Puzzle.h"

using namespace cocos2d;

static int height;

CCScene* T_Skip::scene(int h)
{
    height = h;
    CCScene* pScene = CCScene::create();
    T_Skip* pLayer = T_Skip::create();
    pScene->addChild(pLayer);
    
	return pScene;
}

void T_Skip::onEnter()
{
    CCLog("T_Skip :: onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority(), true);
    CCLayer::onEnter();
}
void T_Skip::onExit()
{
    CCLog("T_Skip :: onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
}

void T_Skip::keyBackClicked()
{
}


bool T_Skip::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
    // make depth tree
    Depth::AddCurDepth("T_Skip", this);
    
    this->setKeypadEnabled(true);
    this->setTouchEnabled(true);
    this->setTouchPriority(Depth::GetCurPriority());
    
    // notification
    CCString* param = CCString::create("1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification("T_Puzzle", param);
    
    
    m_winSize = CCDirector::sharedDirector()->getWinSize();
    
    spriteClass = new SpriteClass();
    
    InitSprites();

    return true;
}

void T_Skip::InitSprites()
{
    pBlack = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, m_winSize.width, m_winSize.height));
    pBlack->setPosition(ccp(0, 0));
    pBlack->setAnchorPoint(ccp(0, 0));
    pBlack->setColor(ccc3(0, 0, 0));
    pBlack->setOpacity(180);
    this->addChild(pBlack, 0);
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_pause_window.png", ccp(0.5, 0.5), ccp(m_winSize.width/2, m_winSize.height/2), CCSize(0,0), "", "T_Skip", this, 9) );
    ((CCSprite*)spriteClass->FindSpriteByName("background/bg_pause_window.png"))->setScale(1.05f);
    
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_pause_cont.png", ccp(1, 0.5), ccp(m_winSize.width/2-10, m_winSize.height/2-70), CCSize(0,0), "", "T_Skip", this, 10) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_pause_end.png", ccp(0, 0.5), ccp(m_winSize.width/2+10, m_winSize.height/2-70), CCSize(0,0), "", "T_Skip", this, 10) );
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
}


bool T_Skip::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    CCPoint point = pTouch->getLocation();
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        if (spriteClass->spriteObj[i]->name == "button/btn_pause_cont.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                ResumeGame();
                return true;
            }
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_pause_end.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                EndGame();
                return true;
            }
        }
    }
    
    return true;
}


void T_Skip::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
}

void T_Skip::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
}

void T_Skip::ResumeGame()
{
    CCLog("T_Skip : Resume game");
    EndScene();
    
    CCString* param = CCString::create("0");
    CCNotificationCenter::sharedNotificationCenter()->postNotification("T_Puzzle", param);
}

void T_Skip::EndGame()
{
    CCLog("T_Skip : End game");
    EndScene();
    
    // 이걸 끝내면서, Puzzle에게도 끝내고 Ranking으로 돌아가라고 알려준다.
    CCString* param = CCString::create("2");
    CCNotificationCenter::sharedNotificationCenter()->postNotification("T_Puzzle", param);
}

void T_Skip::EndScene()
{
    // release depth tree
    Depth::RemoveCurDepth();
    
    CCTextureCache::sharedTextureCache()->removeTextureForKey("images/ranking_scrollbg.png");
    
    // remove all objects
    spriteClass->RemoveAllObjects();
    delete spriteClass;
    pBlack->removeFromParentAndCleanup(true);
    
    this->removeFromParentAndCleanup(true);
}


