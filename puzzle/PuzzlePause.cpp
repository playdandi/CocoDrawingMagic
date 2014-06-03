#include "PuzzlePause.h"
#include "Puzzle.h"

using namespace cocos2d;

static int height;

CCScene* PuzzlePause::scene(int h)
{
    height = h;
    CCScene* pScene = CCScene::create();
    PuzzlePause* pLayer = PuzzlePause::create();
    pScene->addChild(pLayer);
    
	return pScene;
}

void PuzzlePause::onEnter()
{
    CCLog("PuzzlePause :: onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority(), true);
    CCLayer::onEnter();
}
void PuzzlePause::onExit()
{
    CCLog("PuzzlePause :: onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
}

void PuzzlePause::keyBackClicked()
{
    ResumeGame();
}


bool PuzzlePause::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
    // make depth tree
    Depth::AddCurDepth("PuzzlePause", this);
    
    this->setKeypadEnabled(true);
    this->setTouchEnabled(true);
    this->setTouchPriority(Depth::GetCurPriority());
    
    // notification
    CCString* param = CCString::create("1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification("Puzzle", param);
    
    
    m_winSize = CCDirector::sharedDirector()->getWinSize();
    
    spriteClass = new SpriteClass();
    
    InitSprites();
    //InitSprites2();
    
    return true;
}

void PuzzlePause::InitSprites2()
{
    pBlack = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, m_winSize.width, m_winSize.height));
    pBlack->setPosition(ccp(0, 0));
    pBlack->setAnchorPoint(ccp(0, 0));
    pBlack->setColor(ccc3(0, 0, 0));
    pBlack->setOpacity(130);
    this->addChild(pBlack, 0);

    /*
    char name[50];
    sprintf(name, "background/bg_degree_desc.png");
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, name, ccp(0, 0), ccp(300, height), CCSize(400+40+40, 80+20+10+50+20), "", "PuzzlePause", this, 9) );
    CCPoint p = spriteClass->FindParentCenterPos(name);
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("잠시 쉬고 있는 중이에요.", fontList[0], 36, ccp(0, 0), ccp(p.x-20-200+5, p.y+15), ccc3(255,255,255), name, "1", NULL, 10, 1) );
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_cocoroom_desc.png1", ccp(1, 1), ccp(p.x-10, p.y-20), CCSize(200, 80), name, "1", NULL, 10, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_cocoroom_desc.png2", ccp(0, 1), ccp(p.x+10, p.y-20), CCSize(200, 80), name, "1", NULL, 10, 1) );
    */
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_pause_window.png", ccp(0.5, 0.5), ccp(m_winSize.width/2, m_winSize.height/2), CCSize(0,0), "", "PuzzlePause", this, 9) );
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
}

void PuzzlePause::InitSprites()
{
    pBlack = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, m_winSize.width, m_winSize.height));
    pBlack->setPosition(ccp(0, 0));
    pBlack->setAnchorPoint(ccp(0, 0));
    pBlack->setColor(ccc3(0, 0, 0));
    pBlack->setOpacity(180);
    this->addChild(pBlack, 0);
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_pause_window.png", ccp(0.5, 0.5), ccp(m_winSize.width/2, m_winSize.height/2), CCSize(0,0), "", "PuzzlePause", this, 9) );
    ((CCSprite*)spriteClass->FindSpriteByName("background/bg_pause_window.png"))->setScale(1.05f);
    
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_pause_cont.png", ccp(1, 0.5), ccp(m_winSize.width/2-10, m_winSize.height/2-70), CCSize(0,0), "", "PuzzlePause", this, 10) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_pause_end.png", ccp(0, 0.5), ccp(m_winSize.width/2+10, m_winSize.height/2-70), CCSize(0,0), "", "PuzzlePause", this, 10) );
   
    /*
    // 배경
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow_mini.png", ccp(0.5, 0.5), ccp(m_winSize.width/2, m_winSize.height/2), CCSize(40+30+224*2 +30 +22, 40+30+80+105 +20 +26), "", "PuzzlePause", this, 5) );
    char name[50];
    sprintf(name, "background/bg_potion_time.png");
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, name, ccp(0.5, 0.5), ccp(m_winSize.width/2, m_winSize.height/2), CCSize(40+30+224*2 +30, 40+30+80+105 +20), "", "PuzzlePause", this, 5) );
    
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("잠시 쉬고 있는 중이에요.", fontList[0], 42, ccp(0.5, 0), ccp(m_winSize.width/2, m_winSize.height/2+27.5f - 5 +15), ccc3(255,255,255), "", "PuzzlePause", this, 10) );
    
    // 버튼
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_red_mini.png", ccp(0, 1), ccp(m_winSize.width/2+15, m_winSize.height/2 - 2.5f), CCSize(0, 0), "", "PuzzlePause", this, 10) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_system.png", ccp(1, 1), ccp(m_winSize.width/2-15, m_winSize.height/2 - 2.5f), CCSize(0, 0), "", "PuzzlePause", this, 10) );
    */
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
}


bool PuzzlePause::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    CCPoint point = pTouch->getLocation();
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        if (spriteClass->spriteObj[i]->name == "button/btn_pause_cont.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                ResumeGame();
                break;
            }
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_pause_end.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                EndGame();
                break;
            }
        }
    }
    
    return true;
}


void PuzzlePause::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
}

void PuzzlePause::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    //CCPoint point = pTouch->getLocation();
}

void PuzzlePause::ResumeGame()
{
    EndScene();
    
    CCString* param = CCString::create("0");
    CCNotificationCenter::sharedNotificationCenter()->postNotification("Puzzle", param);
    
    isInGamePause = false;
}

void PuzzlePause::EndGame()
{
    EndScene();
    
    // 이걸 끝내면서, Puzzle에게도 끝내고 Ranking으로 돌아가라고 알려준다.
    CCString* param = CCString::create("2");
    CCNotificationCenter::sharedNotificationCenter()->postNotification("Puzzle", param);
    
    //isInGamePause = false;
}

void PuzzlePause::EndScene()
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


