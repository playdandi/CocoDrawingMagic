#include "FairyOneInfo.h"

int fairy_idx;
static int priority;

CCScene* FairyOneInfo::scene(int idx, int prio)
{
    fairy_idx = idx;
    priority = prio;
    
    CCScene* pScene = CCScene::create();
    FairyOneInfo* pLayer = FairyOneInfo::create();
    pScene->addChild(pLayer);
    
	return pScene;
}

void FairyOneInfo::onEnter()
{
    CCLog("FairyOneInfo : onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, priority, true);
    CCLayer::onEnter();
}
void FairyOneInfo::onExit()
{
    CCLog("FairyOneInfo : onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
}

void FairyOneInfo::keyBackClicked()
{
    EndScene();
}


bool FairyOneInfo::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
    this->setTouchEnabled(true);
    this->setKeypadEnabled(true);
    this->setTouchPriority(priority);
    CCLog("FairyOneInfo : touch prio = %d", priority);
    
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    // notification observer
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(FairyOneInfo::Notification), "FairyOneInfo", NULL);
    
    // notification post
    CCString* param = CCString::create("1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification("CocoRoomFairyTown", param);
    
    InitSprites();
    
    return true;
}

void FairyOneInfo::Notification(CCObject* obj)
{
    CCString* param = (CCString*)obj;
    
    if (param->intValue() == 0)
    {
        // 터치 활성
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, priority+1, true);
        //this->setKeypadEnabled(true);
        //this->setTouchEnabled(true);
        this->setTouchPriority(priority);
        isTouched = false;
        CCLog("FairyOneInfo : 터치 활성 (Priority = %d)", this->getTouchPriority());
    }
    else if (param->intValue() == 1)
    {
        // 터치 비활성
        CCLog("FairyOneInfo : 터치 비활성");
        //this->setKeypadEnabled(false);
        //this->setTouchEnabled(false);
        CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
    }
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
    
    // 요정 이미지
    CCLayer* picture;
    switch (fairyInfo[fairy_idx]->GetId())
    {
        case 1: picture = Fairy::MakeFlower(); picture->setScale(0.7f); break;
        case 2: picture = Fairy::MakeSun(); picture->setScale(0.8f); break;
    }
    picture->setAnchorPoint(ccp(0, 0));
    picture->setPosition(ccp(250, 1100));
    this->addChild(picture, 10);
    

    // name + grade background
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_cocoroom_desc.png1", ccp(0, 0), ccp(404, 1121), CCSize(440, 90), "", "FairyOneInfo", this, 5) );
    // grade
    char fname[30];
    if (fairyInfo[fairy_idx]->GetGrade() == 1) sprintf(fname, "letter/letter_grade_a.png");
    else if (fairyInfo[fairy_idx]->GetGrade() == 2) sprintf(fname, "letter/letter_grade_b.png");
    else if (fairyInfo[fairy_idx]->GetGrade() == 3) sprintf(fname, "letter/letter_grade_c.png");
    else if (fairyInfo[fairy_idx]->GetGrade() == 4) sprintf(fname, "letter/letter_grade_d.png");
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, fname, ccp(0, 0), ccp(424, 1136), CCSize(0, 0), "", "FairyOneInfo", this, 5) );
    // name
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(fairyInfo[fairy_idx]->GetName(), fontList[0], 52, ccp(0.5, 0.5), spriteClass->FindParentCenterPos("background/bg_cocoroom_desc.png1"), ccc3(255,255,255), "background/bg_cocoroom_desc.png1", "1", NULL, 5) );
    
    
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
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(FairyInfo::GetAbilityDesc(fairyInfo[fairy_idx]->GetType()), fontList[0], 36, ccp(0, 0), ccp(574, 977), ccc3(255,255,255), "", "FairyOneInfo", this, 5) );
    
    // 요정 묘사
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabelArea(fairyInfo[fairy_idx]->GetDescription(), fontList[0], 36, ccp(0, 0), ccp(210, 875), ccc3(117,86,47), CCSize(730, 100), kCCTextAlignmentLeft, kCCVerticalTextAlignmentCenter, "", "FairyOneInfo", this, 5) );
    
    // 도전
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_lock_white.png",
                    ccp(0, 0), ccp(141, 822), CCSize(0, 0), "", "FairyOneInfo", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_cocoroom_desc.png4",
                    ccp(0, 0), ccp(210, 822), CCSize(730, 58), "", "FairyOneInfo", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("도전 : 8레벨 달성", fontList[0], 36, ccp(0, 0), ccp(230, 830), ccc3(255,255,255), "", "FairyOneInfo", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("/ 달성시 별사탕으로 구매가능", fontList[2], 30, ccp(0, 0), ccp(505, 835), ccc3(255,255,255), "", "FairyOneInfo", this, 5, 170) );
    
    bool flag = true;
    for (int i = 0 ; i < myInfo->GetFairyList().size() ; i++)
    {
        if (myInfo->GetFairyList()[i]->GetId() == fairyInfo[fairy_idx]->GetId())
        {
            flag = false;
            break;
        }
    }
    
    if (flag)
    {
        isByTopaz = true; // 도전 내용 만족하면 false로 바꿔야한다.
        
        // 가격 표시
        spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_degree_desc.png", ccp(0, 0), ccp(493, 723), CCSize(201, 77), "", "FairyOneInfo", this, 5) );

        if (isByTopaz)
        {
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_topaz_mini.png", ccp(0, 0), ccp(503, 730), CCSize(0, 0), "", "FairyOneInfo", this, 5) );
            
            sprintf(fname, "x %d", fairyInfo[fairy_idx]->GetCostTopaz());
            spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(fname, fontList[0], 36, ccp(0, 0), ccp(588, 738), ccc3(255,255,255), "", "FairyOneInfo", this, 5) );
        }
        else
        {
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_starcandy_mini.png", ccp(0, 0), ccp(503, 730), CCSize(0, 0), "", "FairyOneInfo", this, 5) );
            
            sprintf(fname, "x %d", fairyInfo[fairy_idx]->GetCostStarCandy());
            spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(fname, fontList[0], 36, ccp(0, 0), ccp(588, 738), ccc3(255,255,255), "", "FairyOneInfo", this, 5) );
        }
    
        // 버튼
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_green_mini.png", ccp(0, 0), ccp(717, 711), CCSize(0, 0), "", "FairyOneInfo", this, 5) );
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_purchase.png", ccp(0.5, 0), ccp(spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->getContentSize().width/2, 25), CCSize(0, 0), "button/btn_green_mini.png", "0", NULL, 5) );
    }
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
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
            {
                sound->playClick();
                EndScene();
                break;
            }
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_green_mini.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                std::vector<int> data;
                data.push_back(fairy_idx);
                if (isByTopaz)
                {
                    data.push_back(fairyInfo[fairy_idx]->GetCostTopaz());
                    Common::ShowPopup(this, "FairyOneInfo", "NoImage", false, BUY_FAIRY_BY_TOPAZ_TRY, BTN_2, data);
                }
                else
                {
                    data.push_back(fairyInfo[fairy_idx]->GetCostStarCandy());
                    Common::ShowPopup(this, "FairyOneInfo", "NoImage", false, BUY_FAIRY_BY_STARCANDY_TRY, BTN_2, data);
                }
            }
        }
    }
    
    return true;
}


void FairyOneInfo::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
}

void FairyOneInfo::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    isTouched = false;
}


void FairyOneInfo::EndScene()
{
    CCString* param = CCString::create("0");
    CCNotificationCenter::sharedNotificationCenter()->postNotification("CocoRoomFairyTown", param);
    
    // remove all CCNodes
    spriteClass->RemoveAllObjects();
    delete spriteClass;
    
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, "FairyOneInfo");
    
    this->removeFromParentAndCleanup(true);
}

void FairyOneInfo::EndSceneCallback()
{
}
