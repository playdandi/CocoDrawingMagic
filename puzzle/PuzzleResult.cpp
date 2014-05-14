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
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, -1, true);
    CCLayer::onEnter();
}
void PuzzleResult::onExit()
{
    CCLog("PuzzleResult :: onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
}

void PuzzleResult::keyBackClicked()
{
}


bool PuzzleResult::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
    this->setKeypadEnabled(true);
    this->setTouchEnabled(true);
    this->setTouchPriority(-1);
    
    // notification
    CCString* param = CCString::create("1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification("Puzzle", param);
    
    m_winSize = CCDirector::sharedDirector()->getWinSize();
    
    spriteClass = new SpriteClass();
    
    InitSprites();
    
    return true;
}

void PuzzleResult::InitSprites()
{
    pBlack = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, m_winSize.width, m_winSize.height));
    pBlack->setPosition(ccp(0, 0));
    pBlack->setAnchorPoint(ccp(0, 0));
    pBlack->setColor(ccc3(0, 0, 0));
    pBlack->setOpacity(150);
    this->addChild(pBlack, 1000);
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_topinfo.png1", ccp(0, 0), ccp(80, 1666), CCSize(230, 75), "", "PuzzleResult", this, 1001) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_topinfo.png2", ccp(0, 0), ccp(390, 1666), CCSize(290, 75), "", "PuzzleResult", this, 1001) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_topinfo.png3", ccp(0, 0), ccp(765, 1666), CCSize(290, 75), "", "PuzzleResult", this, 1001) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/result_topaz.png", ccp(0, 0), ccp(15+10, 1656), CCSize(0, 0), "", "PuzzleResult", this, 1005) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/result_starcandy.png", ccp(0, 0), ccp(317, 1660), CCSize(0, 0), "", "PuzzleResult", this, 1005) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/result_magicpoint.png", ccp(0, 0), ccp(696, 1669), CCSize(0, 0), "", "PuzzleResult", this, 1005) );
    
    // topaz
    CCLog("%d %d %d", myInfo->GetTopaz(), myInfo->GetStarCandy(), myInfo->GetMPTotal());
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(Common::MakeComma(myInfo->GetTopaz()), fontList[0], 36, ccp(0.5, 0), ccp((80+230+80)/2, 1686), ccc3(255,255,255), "", "PuzzleResult", this, 1005, 0, 255, 1) );
    // starcandy
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(Common::MakeComma(myInfo->GetStarCandy()), fontList[0], 36, ccp(0.5, 0), ccp((390+290+390)/2, 1686), ccc3(255,255,255), "", "PuzzleResult", this, 1005, 0, 255, 2) );
    // magic-point
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(Common::MakeComma(myInfo->GetMPTotal()), fontList[0], 36, ccp(0.5, 0), ccp((765+765+290)/2, 1686), ccc3(255,255,255), "", "PuzzleResult", this, 1005, 0, 255, 3) );
    
    // 배경
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png", ccp(0, 0), ccp(49, 458), CCSize(982, 954+100), "", "PuzzleResult", this, 1001) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png", ccp(0, 0), ccp(75, 492), CCSize(929, 904+100), "", "PuzzleResult", this, 1001) );
    
    // 확인 버튼
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_red_mini.png", ccp(0.5, 0.5), ccp(m_winSize.width/2, 300), CCSize(0, 0), "", "PuzzleResult", this, 1005) );
    CCPoint pos = spriteClass->FindParentCenterPos("button/btn_red_mini.png");
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_confirm_mini.png", ccp(0.5, 0.5), ccp(pos.x, pos.y+3), CCSize(0, 0), "button/btn_red_mini.png", "0", this, 1005, 1) );

    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
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
    CCPoint point = pTouch->getLocation();
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        if (spriteClass->spriteObj[i]->name == "button/btn_red_mini.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                // 화면 어둡게 하고, PuzzleResult 팝업창 끄고, Puzzle->Ranking으로 돌아가자.
                pBlackClose = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, m_winSize.width, m_winSize.height));
                pBlackClose->setPosition(ccp(0, 0));
                pBlackClose->setAnchorPoint(ccp(0, 0));
                pBlackClose->setColor(ccc3(0, 0, 0));
                pBlackClose->setOpacity(0);
                this->addChild(pBlackClose, 1000);
                
                CCActionInterval* action = CCSequence::create( CCFadeIn::create(1.5f), CCCallFuncND::create(this, callfuncND_selector(PuzzleResult::EndSceneCallback), this), NULL);
                pBlackClose->runAction(action);
                break;
            }
        }
    }
}

void PuzzleResult::EndScene()
{
}

void PuzzleResult::EndSceneCallback()
{
    spriteClass->RemoveAllObjects();
    delete spriteClass;
    
    this->removeAllChildren();
    
    // 이걸 끝내면서, Puzzle에게도 끝내고 Ranking으로 돌아가라고 알려준다.
    CCString* param = CCString::create("0");
    CCNotificationCenter::sharedNotificationCenter()->postNotification("Puzzle", param);
    
    this->removeFromParentAndCleanup(true);
}

