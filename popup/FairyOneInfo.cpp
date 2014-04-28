#include "FairyOneInfo.h"

CCScene* FairyOneInfo::scene()
{
    CCScene* pScene = CCScene::create();
    FairyOneInfo* pLayer = FairyOneInfo::create();
    pScene->addChild(pLayer);
    
	return pScene;
}

void FairyOneInfo::onEnter()
{
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
    CCLayer::onEnter();
}
void FairyOneInfo::onExit()
{
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
}

void FairyOneInfo::keyBackClicked()
{
    EndScene();
}


bool FairyOneInfo::init()
{
    CCLog("FairyOneInfo :: Init");
	if (!CCLayer::init())
	{
		return false;
	}
    
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    // notification post
    CCString* param = CCString::create("1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification("CocoRoomFairyTown", param);
    
    InitSprites();
    
    return true;
}

void FairyOneInfo::InitSprites()
{
    pBlack = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, winSize.width, winSize.height));
    pBlack->setPosition(ccp(0, 0));
    pBlack->setAnchorPoint(ccp(0, 0));
    pBlack->setColor(ccc3(0, 0, 0));
    pBlack->setOpacity(150);
    this->addChild(pBlack, 0);
    
    spriteClass = new SpriteClass();
    
    // pop-up 배경
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png",
                    ccp(0, 0), ccp(49, 640), CCSize(982, 623), "", "FairyOneInfo", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png",
                    ccp(0, 0), ccp(76, 678), CCSize(929, 562), "", "FairyOneInfo", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_popup_rightup.png",
                    ccp(0, 0), ccp(809, 1039), CCSize(0, 0), "", "FairyOneInfo", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_x_brown.png",
                    ccp(0, 0), ccp(900, 1132), CCSize(0, 0), "", "FairyOneInfo", this, 1) );
    
    // name
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_cocoroom_desc.png1",
                    ccp(0, 0), ccp(404, 1121), CCSize(440, 90), "", "FairyOneInfo", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_grade_a.png",
                    ccp(0, 0), ccp(424, 1136), CCSize(0, 0), "", "FairyOneInfo", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("잠만자양", fontList[0], 52, ccp(0.5, 0.5), spriteClass->FindParentCenterPos("background/bg_cocoroom_desc.png1"), ccc3(255,255,255), "background/bg_cocoroom_desc.png1", "1", NULL, 5) );
    
    // 기본속성
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_cocoroom_desc.png2",
                    ccp(0, 0), ccp(404, 1036), CCSize(440, 58), "", "FairyOneInfo", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("기본속성", fontList[2], 36, ccp(0, 0), ccp(424, 1042), ccc3(121,71,0), "", "FairyOneInfo", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("기본속성", fontList[2], 36, ccp(0, 0), ccp(424, 1042+3), ccc3(255,219,53), "", "FairyOneInfo", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("MP + 100", fontList[0], 36, ccp(0, 0), ccp(574, 1042), ccc3(255,255,255), "", "FairyOneInfo", this, 5) );
    
    // 특수능력
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_cocoroom_desc.png3",
                    ccp(0, 0), ccp(404, 971), CCSize(440, 58), "", "FairyOneInfo", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("특수능력", fontList[2], 36, ccp(0, 0), ccp(424, 977), ccc3(121,71,0), "", "FairyOneInfo", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("특수능력", fontList[2], 36, ccp(0, 0), ccp(424, 977+3), ccc3(255,219,53), "", "FairyOneInfo", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("추가별사탕획득", fontList[0], 36, ccp(0, 0), ccp(574, 977), ccc3(255,255,255), "", "FairyOneInfo", this, 5) );
    
    // 요정 묘사
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabelArea("매일 잠만자는 양이지만 옆에 있는 것만으로도 든든해진다.", fontList[0], 36, ccp(0, 0), ccp(210, 875), ccc3(117,86,47), CCSize(730, 100), kCCTextAlignmentLeft, kCCVerticalTextAlignmentCenter, "", "FairyOneInfo", this, 5) );
    
    // 도전
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_lock_white.png",
                    ccp(0, 0), ccp(141, 822), CCSize(0, 0), "", "FairyOneInfo", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_cocoroom_desc.png4",
                    ccp(0, 0), ccp(210, 822), CCSize(730, 58), "", "FairyOneInfo", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("도전 : 8레벨 달성", fontList[0], 36, ccp(0, 0), ccp(230, 830), ccc3(255,255,255), "", "FairyOneInfo", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("/ 달성시 별사탕으로 구매가능", fontList[2], 30, ccp(0, 0), ccp(505, 835), ccc3(255,255,255), "", "FairyOneInfo", this, 5, 170) );
    
    // 버튼
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_degree_desc.png",
                    ccp(0, 0), ccp(493, 723), CCSize(201, 77), "", "FairyOneInfo", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_starcandy_mini.png",
                    ccp(0, 0), ccp(503, 730), CCSize(0, 0), "", "FairyOneInfo", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("200", fontList[0], 36, ccp(0, 0), ccp(588, 738), ccc3(255,255,255), "", "FairyOneInfo", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_green_mini.png",
                    ccp(0, 0), ccp(717, 711), CCSize(0, 0), "", "FairyOneInfo", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_purchase.png",
        ccp(0.5, 0), ccp(spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->getContentSize().width/2, 25), CCSize(0, 0), "button/btn_green_mini.png", "0", NULL, 5) );
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        spriteClass->AddChild(i);
    }
}


bool FairyOneInfo::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    if (isTouched)
        return false;
    isTouched = true;
    
    CCPoint point = pTouch->getLocation();
    //CCLog("FairyOneInfo : (%d , %d)", (int)point.x, (int)point.y);
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        if (spriteClass->spriteObj[i]->name == "button/btn_x_brown.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
                EndScene();
        }
    }
    
    return true;
}


void FairyOneInfo::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
    //CCPoint point = pTouch->getLocation();
}

void FairyOneInfo::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    isTouched = false;
}


void FairyOneInfo::EndScene()
{
    sound->playClick();
    CCString* param = CCString::create("0");
    CCNotificationCenter::sharedNotificationCenter()->postNotification("CocoRoomFairyTown", param);
    
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    
    this->removeFromParentAndCleanup(true);
}

void FairyOneInfo::EndSceneCallback()
{
}
