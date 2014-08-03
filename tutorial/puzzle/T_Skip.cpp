#include "T_Skip.h"
#include "T_Puzzle.h"

using namespace cocos2d;

static int height;
static int skipFromWhere;

CCScene* T_Skip::scene(int h, int from)
{
    height = h;
    skipFromWhere = from; // 0 : 실제 인게임 튜토리얼(게임 첫 시작) , 1 : 환경설정->튜토리얼
    
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
    
    this->setKeypadEnabled(false);
    this->setTouchEnabled(true);
    this->setTouchPriority(Depth::GetCurPriority());
    
    // notification
    CCString* param = CCString::create("1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification("T_Puzzle", param);
    
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("images/popup.plist");
    
    m_winSize = CCDirector::sharedDirector()->getWinSize();
    
    InitSprites();
    
    return true;
}

void T_Skip::InitSprites()
{
    spriteClass = new SpriteClass();
    
    pBlack = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, m_winSize.width, m_winSize.height));
    pBlack->setPosition(ccp(0, 0));
    pBlack->setAnchorPoint(ccp(0, 0));
    pBlack->setColor(ccc3(0, 0, 0));
    pBlack->setOpacity(180);
    this->addChild(pBlack, 0);
    
    /*
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_pause_window.png", ccp(0.5, 0.5), ccp(m_winSize.width/2, m_winSize.height/2), CCSize(0,0), "", "T_Skip", this, 9) );
    ((CCSprite*)spriteClass->FindSpriteByName("background/bg_pause_window.png"))->setScale(1.05f);
    
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_pause_cont.png", ccp(1, 0.5), ccp(m_winSize.width/2-10, m_winSize.height/2-70), CCSize(0,0), "", "T_Skip", this, 10) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_pause_end.png", ccp(0, 0.5), ccp(m_winSize.width/2+10, m_winSize.height/2-70), CCSize(0,0), "", "T_Skip", this, 10) );
     */

    // pop-up 배경
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png", ccp(0, 0), ccp(49, 640), CCSize(982, 623), "", "T_Skip", this, 10001) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png", ccp(0, 0), ccp(76, 678), CCSize(929, 562), "", "T_Skip", this, 10001) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_popup_rightup.png", ccp(0, 0), ccp(809, 1039), CCSize(0, 0), "", "T_Skip", this, 10001) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_x_brown.png", ccp(0, 0), ccp(900, 1132), CCSize(0, 0), "", "T_Skip", this, 10001) );

    // 텍스트
    std::string text = "튜토리얼을 넘기시겠습니까?";
    if (skipFromWhere == 1)
        text = "튜토리얼을 그만두시겠습니까?";
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabelArea(text, fontList[0], 40, ccp(0.5, 0.5), ccp(m_winSize.width/2, m_winSize.height/2), ccc3(78,47,8), CCSize(829-20, 250), kCCTextAlignmentCenter, kCCVerticalTextAlignmentCenter, "", "T_Skip", this, 10005) );

    // 확인 버튼
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_red_mini.png", ccp(0, 0), ccp(717+5, 711), CCSize(0, 0), "", "T_Skip", this, 10005) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_confirm_mini.png",ccp(0.5, 0), ccp(spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->getContentSize().width/2, 24), CCSize(0, 0), "button/btn_red_mini.png", "0", NULL, 10005, 1) );

    // 취소 버튼
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_system.png", ccp(0, 0), ccp(126, 711), CCSize(0, 0), "", "T_Skip", this, 10005) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_cancel.png", ccp(0.5, 0), ccp(spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->getContentSize().width/2, 30), CCSize(0, 0), "button/btn_system.png", "0", NULL, 10005, 1) );

    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
}


bool T_Skip::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    CCPoint point = pTouch->getLocation();
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        if (spriteClass->spriteObj[i]->name == "button/btn_system.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                ResumeGame();
                return true;
            }
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_red_mini.png")
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
    EndScene();
    
    CCString* param = CCString::create("0");
    CCNotificationCenter::sharedNotificationCenter()->postNotification("T_Puzzle", param);
}

void T_Skip::EndGame()
{
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
    CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("images/popup.plist");
    CCTextureCache::sharedTextureCache()->removeTextureForKey("images/popup.png");
    
    // remove all objects
    spriteClass->RemoveAllObjects();
    delete spriteClass;
    pBlack->removeFromParentAndCleanup(true);
    
    this->removeFromParentAndCleanup(true);
}


