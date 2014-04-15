#include "CocoRoom.h"

static int tabNumber;

CCScene* CocoRoom::scene(int tab)
{
    tabNumber = tab;
    
    CCScene* pScene = CCScene::create();
    CocoRoom* pLayer = CocoRoom::create();
    pScene->addChild(pLayer);
    
	return pScene;
}

void CocoRoom::onEnter()
{
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
    CCLayer::onEnter();
}
void CocoRoom::onExit()
{
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
}

void CocoRoom::keyBackClicked()
{
    EndScene();
}


bool CocoRoom::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    // notification observer
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(CocoRoom::Notification), "CocoRoom", NULL);
    
    // notification
    CCString* param = CCString::create("1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification("GameReady", param);
    
    // scrollView init.
    scrollViewCoco = CCScrollView::create();
    scrollViewCoco->retain();
    scrollViewFairy = CCScrollView::create();
    scrollViewFairy->retain();

    coco = CCLayer::create();
    fairy = CCLayer::create();
    candy = CCLayer::create();
    coco->setPosition(ccp(0, 0));
    fairy->setPosition(ccp(0, 0));
    candy->setPosition(ccp(0, 0));
    this->addChild(coco, 5);
    this->addChild(fairy, 5);
    this->addChild(candy, 5);
    
    spriteClass = new SpriteClass();
    spriteClassCoco = new SpriteClass();
    spriteClassFairy = new SpriteClass();
    spriteClassCandy = new SpriteClass();
    
    InitSprites();
    curState = -1;
    MakeSprites(tabNumber);
    
    return true;
}

void CocoRoom::Notification(CCObject* obj)
{
    CCString* param = (CCString*)obj;
    
    if (param->intValue() == 0)
    {
        CCLog("cocoRoom 활성");
        // 터치 활성
        this->setKeypadEnabled(true);
        this->setTouchEnabled(true);
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 0, false);
        isTouched = false;
        
        // 코코의 정보 갱신
        if (curState == 0)
        {
            // 총 mp, 요정 mp, 지팡이퍼센트, 지팡이레벨
            char val[20];
            sprintf(val, "%d", myInfo->GetMPTotal());
            ((CCLabelTTF*)spriteClassCoco->FindLabelByTag(1))->setString(val);
            sprintf(val, "%d", myInfo->GetMP());
            ((CCLabelTTF*)spriteClassCoco->FindLabelByTag(2))->setString(val);
            sprintf(val, "요정 %d", myInfo->GetMPFairy());
            ((CCLabelTTF*)spriteClassCoco->FindLabelByTag(3))->setString(val);
            sprintf(val, "%dLv (+ %d%%)", myInfo->GetStaffLv(), myInfo->GetMPStaffPercent());
            ((CCLabelTTF*)spriteClassCoco->FindLabelByTag(4))->setString(val);
            
            // 지팡이 일반/고급 다음 cost 갱신
            sprintf(val, "x %d", myInfo->GetMPNextCostStarcandy());
            ((CCLabelTTF*)spriteClassCoco->FindLabelByTag(5))->setString(val);
            sprintf(val, "x %d", myInfo->GetMPNextCostTopaz());
            ((CCLabelTTF*)spriteClassCoco->FindLabelByTag(6))->setString(val);
            
            SetFullStaffBtn();
        }
        // 요정의 정보 갱신
        else if (curState == 1)
        {
            
        }
        // 오.별 정보 갱신
        else
        {
            
        }
    }
    else if (param->intValue() == 1)
    {
        CCLog("cocoRoom 비활성");
        // 터치 비활성
        CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
        this->setKeypadEnabled(false);
        this->setTouchEnabled(false);
    }
}

int offset = -110;

void CocoRoom::InitSprites()
{
    CCSprite* pBlack = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, winSize.width, winSize.height-250));
    pBlack->setPosition(ccp(0, 0));
    pBlack->setAnchorPoint(ccp(0, 0));
    pBlack->setColor(ccc3(0, 0, 0));
    pBlack->setOpacity(220);
    this->addChild(pBlack, 0);
    CCSprite* pBlack2 = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, winSize.width, 250));
    pBlack2->setPosition(ccp(0, winSize.height-250));
    pBlack2->setAnchorPoint(ccp(0, 0));
    pBlack2->setColor(ccc3(0, 0, 0));
    pBlack2->setOpacity(100);
    this->addChild(pBlack2, 0);
    
    // background
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_purple.png",
            ccp(0, 0), ccp(14, 1586+offset), CCSize(0, 0), "", "CocoRoom", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_x_yellow.png",
            ccp(0, 0), ccp(875, 1634+offset), CCSize(0, 0), "", "CocoRoom", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_title_cocoroom.png",
            ccp(0, 0), ccp(409, 1632+offset), CCSize(0, 0), "", "CocoRoom", this, 2) );
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_sketchbook_select.png1",
            ccp(0, 0), ccp(101, 1417+offset), CCSize(0, 0), "", "CocoRoom", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_sketchbook_select.png2",
            ccp(0, 0), ccp(297, 1417+offset), CCSize(0, 0), "", "CocoRoom", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_sketchbook_select.png3",
            ccp(0, 0), ccp(495, 1417+offset), CCSize(0, 0), "", "CocoRoom", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_menu_coco.png",
            ccp(0, 0), ccp(101+40, 1447+40+offset), CCSize(0, 0), "", "CocoRoom", this, 3) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_menu_fairy.png",
            ccp(0, 0), ccp(297+55, 1447+10+offset), CCSize(0, 0), "", "CocoRoom", this, 3) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_menu_starcandy.png",
            ccp(0, 0), ccp(495+50, 1447+23+offset), CCSize(0, 0), "", "CocoRoom", this, 3) );
    
    // 오늘의 별사탕 띠
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_today_starcandy.png",
                    ccp(0, 0), ccp(630, 1494+offset), CCSize(0, 0), "", "CocoRoom", this, 5) );
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png1",
            ccp(0, 0), ccp(49, 458), CCSize(982, 954+20+offset), "", "CocoRoom", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png1",
            ccp(0, 0), ccp(75-5, 492-5), CCSize(929+10, 904+10+offset+10), "", "CocoRoom", this, 1) );
    
    // slot part
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png2",
            ccp(0, 0), ccp(49, 198), CCSize(982, 223), "", "CocoRoom", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png2",
            ccp(0, 0), ccp(77, 228), CCSize(782, 177), "", "CocoRoom", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_plus_big.png", // slot plus
            ccp(0, 0), ccp(896, 317), CCSize(0, 0), "", "CocoRoom", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_gameready_name.png",
            ccp(0, 0), ccp(867, 242), CCSize(136, 63), "", "CocoRoom", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("4", fontList[0], 48, ccp(0, 0),
            ccp(892, 248), ccc3(255,219,53), "", "CocoRoom", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("/ 7", fontList[0], 36, ccp(0, 0),
            ccp(927, 248), ccc3(182,142,142), "", "CocoRoom", this, 5) );
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
}

void CocoRoom::MakeSprites(int state)
{
    if (curState == state)
        return;
    
    // layer init.
    coco->removeAllChildren();
    spriteClassCoco->spriteObj.clear();
    fairy->removeAllChildren();
    spriteClassFairy->spriteObj.clear();
    candy->removeAllChildren();
    spriteClassCandy->spriteObj.clear();
    
    // slot '+' 밑부분 숫자를, 오늘의별사탕에서는 다른 것으로 대체
    int size = spriteClass->spriteObj.size();
    spriteClass->spriteObj[size-3]->sprite9->setOpacity(255);
    spriteClass->spriteObj[size-2]->label->setOpacity(255);
    spriteClass->spriteObj[size-1]->label->setOpacity(255);
    
    if (state == 0)
    {
        MakeSpritesCoco();
        MakeScrollCoco();
    }
    else if (state == 1)
    {
        MakeSpritesFairy();
        MakeScrollFairy();
    }
    else
    {
        spriteClass->spriteObj[size-3]->sprite9->setOpacity(0);
        spriteClass->spriteObj[size-2]->label->setOpacity(0);
        spriteClass->spriteObj[size-1]->label->setOpacity(0);
        MakeSpritesCandy();
    }
    
    curState = state;
}

void CocoRoom::MakeSpritesCoco()
{
    spriteClassCoco->spriteObj.push_back( SpriteObject::Create(1, "background/bg_degree_desc.png",
                        ccp(0, 0), ccp(441, 1290+offset), CCSize(543, 61), "", "Layer", coco, 5) );
    spriteClassCoco->spriteObj.push_back( SpriteObject::CreateLabel("오늘도 곰을 백마리 잡은 마법사", fontList[0], 36, ccp(0.5, 0.5), spriteClassCoco->FindParentCenterPos("background/bg_degree_desc.png"), ccc3(255,255,255), "background/bg_degree_desc.png", "1", NULL, 5) );
    
    // stone floor
    spriteClassCoco->spriteObj.push_back( SpriteObject::Create(0, "background/stone_floor.png",
                        ccp(0, 0), ccp(100, 730+offset), CCSize(0, 0), "", "Layer", coco, 3) );
    
    // coco character
    
    // skill 그림
    CCPoint pos = ccp(608, 1182+offset);
    if (myInfo->IsFire())
    {
        spriteClassCoco->spriteObj.push_back( SpriteObject::Create(0, "background/bg_skill_yellow_mini.png1", ccp(0, 0), pos, CCSize(0, 0), "", "Layer", coco, 5) );
        spriteClassCoco->spriteObj.push_back( SpriteObject::Create(0, "background/bg_property_big.png1", ccp(0.5, 0.5), spriteClassCoco->FindParentCenterPos("background/bg_skill_yellow_mini.png1"), CCSize(0, 0), "background/bg_skill_yellow_mini.png1", "0", NULL, 5, 1) );
        spriteClassCoco->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_property_fire_mini.png", ccp(0.5, 0.5), spriteClassCoco->FindParentCenterPos("background/bg_skill_yellow_mini.png1"), CCSize(0, 0), "background/bg_skill_yellow_mini.png1", "0", NULL, 5, 1) );
        pos = ccp(699, 1182+offset);
    }
    if (myInfo->IsWater())
    {
        spriteClassCoco->spriteObj.push_back( SpriteObject::Create(0, "background/bg_skill_yellow_mini.png2", ccp(0, 0), pos, CCSize(0, 0), "", "Layer", coco, 5) );
        spriteClassCoco->spriteObj.push_back( SpriteObject::Create(0, "background/bg_property_big.png2", ccp(0.5, 0.5), spriteClassCoco->FindParentCenterPos("background/bg_skill_yellow_mini.png2"), CCSize(0, 0), "background/bg_skill_yellow_mini.png2", "0", NULL, 5, 1) );
        spriteClassCoco->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_property_water_mini.png", ccp(0.5, 0.5), spriteClassCoco->FindParentCenterPos("background/bg_skill_yellow_mini.png2"), CCSize(0, 0), "background/bg_skill_yellow_mini.png2", "0", NULL, 5, 1) );
        pos = ccp(790, 1182+offset);
    }
    if (myInfo->IsLand())
    {
        spriteClassCoco->spriteObj.push_back( SpriteObject::Create(0, "background/bg_skill_yellow_mini.png3", ccp(0, 0), pos, CCSize(0, 0), "", "Layer", coco, 5) );
        spriteClassCoco->spriteObj.push_back( SpriteObject::Create(0, "background/bg_property_big.png3", ccp(0.5, 0.5), spriteClassCoco->FindParentCenterPos("background/bg_skill_yellow_mini.png3"), CCSize(0, 0), "background/bg_skill_yellow_mini.png3", "0", NULL, 5, 1) );
        spriteClassCoco->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_property_land_mini.png", ccp(0.5, 0.5), spriteClassCoco->FindParentCenterPos("background/bg_skill_yellow_mini.png3"), CCSize(0, 0), "background/bg_skill_yellow_mini.png3", "0", NULL, 5, 1) );
        pos = ccp(882, 1182+offset);
    }
    if (myInfo->IsMaster())
    {
        spriteClassCoco->spriteObj.push_back( SpriteObject::Create(0, "background/bg_skill_yellow_mini.png4", ccp(0, 0), pos, CCSize(0, 0), "", "Layer", coco, 5) );
        spriteClassCoco->spriteObj.push_back( SpriteObject::Create(0, "background/bg_property_big.png4", ccp(0.5, 0.5), spriteClassCoco->FindParentCenterPos("background/bg_skill_yellow_mini.png4"), CCSize(0, 0), "background/bg_skill_yellow_mini.png4", "0", NULL, 5, 1) );
        spriteClassCoco->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_property_master_mini.png", ccp(0.5, 0.5), spriteClassCoco->FindParentCenterPos("background/bg_skill_yellow_mini.png4"), CCSize(0, 0), "background/bg_skill_yellow_mini.png4", "0", NULL, 5, 1) );
    }
    
    
    // 설명창 배경
    spriteClassCoco->spriteObj.push_back( SpriteObject::Create(1, "background/bg_cocoroom_desc.png1",
                        ccp(0, 0), ccp(524, 1084+offset), CCSize(439, 76), "", "Layer", coco, 5) );
    spriteClassCoco->spriteObj.push_back( SpriteObject::Create(1, "background/bg_cocoroom_desc.png2",
                        ccp(0, 0), ccp(524, 999+offset), CCSize(439, 58), "", "Layer", coco, 5) );
    spriteClassCoco->spriteObj.push_back( SpriteObject::Create(1, "background/bg_cocoroom_desc.png3",
                        ccp(0, 0), ccp(524, 937+offset), CCSize(439, 58), "", "Layer", coco, 5) );
    spriteClassCoco->spriteObj.push_back( SpriteObject::Create(1, "background/bg_cocoroom_desc.png4",
                        ccp(0, 0), ccp(524, 875+offset), CCSize(439, 58), "", "Layer", coco, 5) );
    // 설명
    spriteClassCoco->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_magicpoint.png",
        ccp(0, 0), ccp(549, 1084+offset), CCSize(0, 0), "", "Layer", coco, 5) );
    spriteClassCoco->spriteObj.push_back( SpriteObject::CreateLabel("기본 MP", fontList[2], 36, ccp(0, 0), ccp(549, 1006+offset), ccc3(121,71,0), "", "Layer", coco, 5) );
    spriteClassCoco->spriteObj.push_back( SpriteObject::CreateLabel("기본 MP", fontList[2], 36, ccp(0, 0), ccp(549, 1009+offset), ccc3(255,219,53), "", "Layer", coco, 5) );
    spriteClassCoco->spriteObj.push_back( SpriteObject::CreateLabel("추가속성", fontList[2], 36, ccp(0, 0), ccp(549, 944+offset), ccc3(121,71,0), "", "Layer", coco, 5) );
    spriteClassCoco->spriteObj.push_back( SpriteObject::CreateLabel("추가속성", fontList[2], 36, ccp(0, 0), ccp(549, 947+offset), ccc3(255,219,53), "", "Layer", coco, 5) );
    spriteClassCoco->spriteObj.push_back( SpriteObject::CreateLabel("지팡이", fontList[2], 36, ccp(0, 0), ccp(549, 882+offset), ccc3(121,71,0), "", "Layer", coco, 5) );
    spriteClassCoco->spriteObj.push_back( SpriteObject::CreateLabel("지팡이", fontList[2], 36, ccp(0, 0), ccp(549, 885+offset), ccc3(255,219,53), "", "Layer", coco, 5) );
    
    // 실제 값들 넣기 (MP, 기본mp, 추가속성, 지팡이)
    char val[20];
    sprintf(val, "%d", myInfo->GetMPTotal());
    spriteClassCoco->spriteObj.push_back( SpriteObject::CreateLabel(val, fontList[0], 60, ccp(0.5, 0.5), ccp(800, 1090+35+offset), ccc3(255,255,255), "", "Layer", coco, 5, 0, 255, 1) );
    sprintf(val, "%d", myInfo->GetMP());
    spriteClassCoco->spriteObj.push_back( SpriteObject::CreateLabel(val, fontList[0], 60, ccp(0.5, 0.5), ccp(800, 999+32+offset), ccc3(255,255,255), "", "Layer", coco, 5, 0, 255, 2) );
    sprintf(val, "요정 %d", myInfo->GetMPFairy());
    spriteClassCoco->spriteObj.push_back( SpriteObject::CreateLabel(val, fontList[0], 36, ccp(0.5, 0.5), ccp(800, 946+21+offset), ccc3(0,167,222), "", "Layer", coco, 5, 0, 255, 3) );
    sprintf(val, "%dLv (+ %d%%)", myInfo->GetStaffLv(), myInfo->GetMPStaffPercent());
    spriteClassCoco->spriteObj.push_back( SpriteObject::CreateLabel(val, fontList[0], 36, ccp(0.5, 0.5), ccp(800, 884+20+offset), ccc3(255,219,53), "", "Layer", coco, 5, 0, 255, 4) );
    
    // buttons
    spriteClassCoco->spriteObj.push_back( SpriteObject::Create(0, "button/btn_green.png1",
                        ccp(0, 0), ccp(94, 509), CCSize(0, 0), "", "Layer", coco, 5) );
    spriteClassCoco->spriteObj.push_back( SpriteObject::Create(0, "button/btn_green.png2",
                        ccp(0, 0), ccp(541, 509), CCSize(0, 0), "", "Layer", coco, 5) );
    spriteClassCoco->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_staff.png1",
                        ccp(0, 0), ccp(134, 587), CCSize(0, 0), "", "Layer", coco, 5) );
    spriteClassCoco->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_buildup_normal.png",
                        ccp(0, 0), ccp(299, 587), CCSize(0, 0), "", "Layer", coco, 5) );
    spriteClassCoco->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_staff.png2",
                        ccp(0, 0), ccp(581, 587), CCSize(0, 0), "", "Layer", coco, 5) );
    spriteClassCoco->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_buildup_fine.png",
                        ccp(0, 0), ccp(746, 587), CCSize(0, 0), "", "Layer", coco, 5) );

    ///if (myInfo->GetStaffLvNext() == 0)
    //{
        // 만렙인 경우
        spriteClassCoco->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_lock_white.png1", ccp(0, 0), ccp(205, 531), CCSize(0, 0), "", "Layer", coco, 5, 0, 0) );
        spriteClassCoco->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_lock_white.png2", ccp(0, 0), ccp(652, 531), CCSize(0, 0), "", "Layer", coco, 5, 0, 0) );
        sprintf(val, "강화 완료");
        spriteClassCoco->spriteObj.push_back( SpriteObject::CreateLabel(val, fontList[0], 36, ccp(0, 0), ccp(270, 541), ccc3(22,56,0), "", "Layer", coco, 5, 0, 0, 7) );
        spriteClassCoco->spriteObj.push_back( SpriteObject::CreateLabel(val, fontList[0], 36, ccp(0, 0), ccp(720, 541), ccc3(22,56,0), "", "Layer", coco, 5, 0, 0, 8) );
    //}
    //else
    //{
        // 일반적인 경우
        spriteClassCoco->spriteObj.push_back( SpriteObject::Create(0, "button/btn_receive_starcandy.png",
                        ccp(0, 0), ccp(224, 531), CCSize(0, 0), "", "Layer", coco, 5) );
        spriteClassCoco->spriteObj.push_back( SpriteObject::Create(0, "button/btn_receive_topaz.png",
                        ccp(0, 0), ccp(671, 531), CCSize(0, 0), "", "Layer", coco, 5) );
        sprintf(val, "x %d", myInfo->GetMPNextCostStarcandy());
        spriteClassCoco->spriteObj.push_back( SpriteObject::CreateLabel(val, fontList[0], 36, ccp(0, 0), ccp(300, 541), ccc3(22,56,0), "", "Layer", coco, 5, 0, 255, 5) );
        sprintf(val, "x %d", myInfo->GetMPNextCostTopaz());
        spriteClassCoco->spriteObj.push_back( SpriteObject::CreateLabel(val, fontList[0], 36, ccp(0, 0), ccp(750, 541), ccc3(22,56,0), "", "Layer", coco, 5, 0, 255, 6) );
    //}
    
    SetFullStaffBtn();
    
    for (int i = 0 ; i < spriteClassCoco->spriteObj.size() ; i++)
        spriteClassCoco->AddChild(i);
}

void CocoRoom::SetFullStaffBtn()
{
    // 만렙인 경우
    if (myInfo->GetStaffLvNext() == 0)
    {
        ((CCSprite*)spriteClassCoco->FindSpriteByName("button/btn_receive_starcandy.png"))->setOpacity(0);
        ((CCSprite*)spriteClassCoco->FindSpriteByName("button/btn_receive_topaz.png"))->setOpacity(0);
        ((CCLabelTTF*)spriteClassCoco->FindLabelByTag(5))->setOpacity(0);
        ((CCLabelTTF*)spriteClassCoco->FindLabelByTag(6))->setOpacity(0);
        
        ((CCSprite*)spriteClassCoco->FindSpriteByName("icon/icon_lock_white.png1"))->setOpacity(255);
        ((CCSprite*)spriteClassCoco->FindSpriteByName("icon/icon_lock_white.png2"))->setOpacity(255);
        ((CCLabelTTF*)spriteClassCoco->FindLabelByTag(7))->setOpacity(255);
        ((CCLabelTTF*)spriteClassCoco->FindLabelByTag(8))->setOpacity(255);
    }
    // 일반적인 경우
    else
    {
        ((CCSprite*)spriteClassCoco->FindSpriteByName("button/btn_receive_starcandy.png"))->setOpacity(255);
        ((CCSprite*)spriteClassCoco->FindSpriteByName("button/btn_receive_topaz.png"))->setOpacity(255);
        ((CCLabelTTF*)spriteClassCoco->FindLabelByTag(5))->setOpacity(255);
        ((CCLabelTTF*)spriteClassCoco->FindLabelByTag(6))->setOpacity(255);
        
        ((CCSprite*)spriteClassCoco->FindSpriteByName("icon/icon_lock_white.png1"))->setOpacity(0);
        ((CCSprite*)spriteClassCoco->FindSpriteByName("icon/icon_lock_white.png2"))->setOpacity(0);
        ((CCLabelTTF*)spriteClassCoco->FindLabelByTag(7))->setOpacity(0);
        ((CCLabelTTF*)spriteClassCoco->FindLabelByTag(8))->setOpacity(0);
    }
}

void CocoRoom::MakeSpritesFairy()
{
    spriteClassFairy->spriteObj.push_back( SpriteObject::Create(1, "background/bg_cocoroom_desc.png1",
                        ccp(0, 0), ccp(524, 1271), CCSize(439, 89), "", "Layer", fairy, 5) );
    spriteClassFairy->spriteObj.push_back( SpriteObject::Create(1, "background/bg_cocoroom_desc.png2",
                        ccp(0, 0), ccp(524, 1186), CCSize(439, 58), "", "Layer", fairy, 5) );
    spriteClassFairy->spriteObj.push_back( SpriteObject::Create(1, "background/bg_cocoroom_desc.png3",
                        ccp(0, 0), ccp(524, 1124), CCSize(439, 58), "", "Layer", fairy, 5) );
    spriteClassFairy->spriteObj.push_back( SpriteObject::Create(1, "background/bg_cocoroom_desc.png4",
                        ccp(0, 0), ccp(524, 1026), CCSize(439, 58), "", "Layer", fairy, 5) );
    
    // stone floor
    spriteClassFairy->spriteObj.push_back( SpriteObject::Create(0, "background/fairy_floor.png",
                        ccp(0, 0), ccp(100, 750), CCSize(0, 0), "", "Layer", fairy, 5) );
    
    // fairy character
    
    // description
    spriteClassFairy->spriteObj.push_back( SpriteObject::CreateLabel("잠만자양", fontList[0], 52, ccp(0.5, 0.5), spriteClassFairy->FindParentCenterPos("background/bg_cocoroom_desc.png1"), ccc3(255,255,255), "background/bg_cocoroom_desc.png1", "1", NULL, 5) );
    spriteClassFairy->spriteObj.push_back( SpriteObject::CreateLabel("기본속성", fontList[2], 36, ccp(0, 0), ccp(570, 1193), ccc3(121,71,0), "", "Layer", fairy, 5) );
    spriteClassFairy->spriteObj.push_back( SpriteObject::CreateLabel("기본속성", fontList[2], 36, ccp(0, 0), ccp(570, 1196), ccc3(255,219,53), "", "Layer", fairy, 5) );
    spriteClassFairy->spriteObj.push_back( SpriteObject::CreateLabel("추가속성", fontList[2], 36, ccp(0, 0), ccp(570, 1131), ccc3(121,71,0), "", "Layer", fairy, 5) );
    spriteClassFairy->spriteObj.push_back( SpriteObject::CreateLabel("추가속성", fontList[2], 36, ccp(0, 0), ccp(570, 1134), ccc3(255,219,53), "", "Layer", fairy, 5) );
    spriteClassFairy->spriteObj.push_back( SpriteObject::CreateLabel("특수능력", fontList[2], 36, ccp(0, 0), ccp(570, 1033), ccc3(121,71,0), "", "Layer", fairy, 5) );
    spriteClassFairy->spriteObj.push_back( SpriteObject::CreateLabel("특수능력", fontList[2], 36, ccp(0, 0), ccp(570, 1036), ccc3(255,219,53), "", "Layer", fairy, 5) );
    
    // description detail
    spriteClassFairy->spriteObj.push_back( SpriteObject::CreateLabel("MP + 100", fontList[0], 36, ccp(0, 0), ccp(720, 1196), ccc3(255,255,255), "", "Layer", fairy, 5) );
    spriteClassFairy->spriteObj.push_back( SpriteObject::CreateLabel("MP + 50", fontList[0], 36, ccp(0, 0), ccp(720, 1134), ccc3(0,167,222), "", "Layer", fairy, 5) );
    spriteClassFairy->spriteObj.push_back( SpriteObject::CreateLabel("요정공격 확률증가", fontList[0], 36, ccp(0, 0), ccp(720, 1036), ccc3(255,255,255), "", "Layer", fairy, 5) );
    
    // buttons
    spriteClassFairy->spriteObj.push_back( SpriteObject::Create(0, "button/btn_green.png1",
                    ccp(0, 0), ccp(94, 509), CCSize(0, 0), "", "Layer", fairy, 5) );
    spriteClassFairy->spriteObj.push_back( SpriteObject::Create(0, "button/btn_green.png2",
                    ccp(0, 0), ccp(541, 509), CCSize(0, 0), "", "Layer", fairy, 5) );
    spriteClassFairy->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_normal.png",
                    ccp(0, 0), ccp(176, 588), CCSize(0, 0), "", "Layer", fairy, 5) );
    spriteClassFairy->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_tonic.png1",
                    ccp(0, 0), ccp(291, 586), CCSize(0, 0), "", "Layer", fairy, 5) );
    spriteClassFairy->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_fine.png",
                    ccp(0, 0), ccp(623, 588), CCSize(0, 0), "", "Layer", fairy, 5) );
    spriteClassFairy->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_tonic.png2",
                    ccp(0, 0), ccp(738, 586), CCSize(0, 0), "", "Layer", fairy, 5) );
    spriteClassFairy->spriteObj.push_back( SpriteObject::Create(0, "button/btn_receive_starcandy.png",
                    ccp(0, 0), ccp(224, 531), CCSize(0, 0), "", "Layer", fairy, 5) );
    spriteClassFairy->spriteObj.push_back( SpriteObject::Create(0, "button/btn_receive_topaz.png",
                    ccp(0, 0), ccp(681, 531), CCSize(0, 0), "", "Layer", fairy, 5) );
    spriteClassFairy->spriteObj.push_back( SpriteObject::CreateLabel("x 2", fontList[0], 36, ccp(0, 0), ccp(297, 541), ccc3(22,56,0), "", "Layer", fairy, 5) );
    spriteClassFairy->spriteObj.push_back( SpriteObject::CreateLabel("x 2", fontList[0], 36, ccp(0, 0), ccp(741, 541), ccc3(22,56,0), "", "Layer", fairy, 5) );
    
    for (int i = 0 ; i < spriteClassFairy->spriteObj.size() ; i++)
        spriteClassFairy->AddChild(i);
}

void CocoRoom::MakeSpritesCandy()
{
    spriteClassCandy->spriteObj.push_back( SpriteObject::Create(0, "todaycandy/bg_group.png",
                        ccp(0, 0), ccp(108, 518), CCSize(0, 0), "", "Layer", candy, 5) );
    
    // 5 profiles
    spriteClassCandy->spriteObj.push_back( SpriteObject::Create(0, "background/bg_profile_noimage.png1",
                        ccp(0, 0), ccp(471, 1200), CCSize(0, 0), "", "Layer", candy, 5) );
    spriteClassCandy->spriteObj.push_back( SpriteObject::Create(0, "background/bg_profile_noimage.png2",
                        ccp(0, 0), ccp(184, 1051), CCSize(0, 0), "", "Layer", candy, 5) );
    spriteClassCandy->spriteObj.push_back( SpriteObject::Create(0, "background/bg_profile_noimage.png3",
                        ccp(0, 0), ccp(267, 671), CCSize(0, 0), "", "Layer", candy, 5) );
    spriteClassCandy->spriteObj.push_back( SpriteObject::Create(0, "background/bg_profile_noimage.png4",
                        ccp(0, 0), ccp(696, 671), CCSize(0, 0), "", "Layer", candy, 5) );
    spriteClassCandy->spriteObj.push_back( SpriteObject::Create(0, "background/bg_profile_noimage.png5",
                        ccp(0, 0), ccp(787, 1051), CCSize(0, 0), "", "Layer", candy, 5) );
    
    // names
    spriteClassCandy->spriteObj.push_back( SpriteObject::Create(1, "background/bg_cocoroom_desc.png1",
                        ccp(0.5, 0), ccp(471+60, 1140), CCSize(150, 58), "", "Layer", candy, 5, 160) );
    spriteClassCandy->spriteObj.push_back( SpriteObject::Create(1, "background/bg_cocoroom_desc.png2",
                        ccp(0.5, 0), ccp(184+60, 987), CCSize(150, 58), "", "Layer", candy, 5, 160) );
    spriteClassCandy->spriteObj.push_back( SpriteObject::Create(1, "background/bg_cocoroom_desc.png3",
                        ccp(0.5, 0), ccp(267+60, 607), CCSize(150, 58), "", "Layer", candy, 5, 160) );
    spriteClassCandy->spriteObj.push_back( SpriteObject::Create(1, "background/bg_cocoroom_desc.png4",
                        ccp(0.5, 0), ccp(696+60, 607), CCSize(150, 58), "", "Layer", candy, 5, 160) );
    spriteClassCandy->spriteObj.push_back( SpriteObject::Create(1, "background/bg_cocoroom_desc.png5",
                        ccp(0.5, 0), ccp(787+60, 987), CCSize(150, 58), "", "Layer", candy, 5, 160) );
    
    // question
    spriteClassCandy->spriteObj.push_back( SpriteObject::Create(0, "button/btn_question.png",
                        ccp(0, 0), ccp(916, 515), CCSize(0, 0), "", "Layer", candy, 5) );
    spriteClassCandy->spriteObj.push_back( SpriteObject::Create(0, "button/btn_red_mini.png",
                        ccp(0, 0), ccp(425, 842), CCSize(0, 0), "", "Layer", candy, 5) );
    spriteClassCandy->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_fairy_select.png",
                ccp(0.5, 0), ccp(spriteClassCandy->spriteObj[spriteClassCandy->spriteObj.size()-1]->sprite->getContentSize().width/2, 30), CCSize(0, 0), "button/btn_red_mini.png", "0", NULL, 5) );
    
    // 보상 설명 부분
    spriteClassCandy->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_starcandy.png",
                        ccp(0, 0), ccp(350, 300), CCSize(0, 0), "", "Layer", candy, 5) );
    spriteClassCandy->spriteObj.push_back( SpriteObject::CreateLabel("오늘의 보상 :", fontList[0], 36, ccp(0, 0), ccp(110, 330), ccc3(78,47,8), "", "Layer", candy, 5) );
    spriteClassCandy->spriteObj.push_back( SpriteObject::CreateLabel("x 500", fontList[0], 36, ccp(0, 0), ccp(450, 320), ccc3(78,47,8), "", "Layer", candy, 5) );
    spriteClassCandy->spriteObj.push_back( SpriteObject::CreateLabel("친구와 함께 보상을 받으세요!", fontList[0], 36, ccp(0, 0), ccp(240, 250), ccc3(255,255,255), "", "Layer", candy, 5) );
    spriteClassCandy->spriteObj.push_back( SpriteObject::CreateLabel("친구목록", fontList[0], 36, ccp(0, 0), ccp(870, 247), ccc3(255,255,255), "", "Layer", candy, 5) );
    
    for (int i = 0 ; i < spriteClassCandy->spriteObj.size() ; i++)
        spriteClassCandy->AddChild(i);
}

void CocoRoom::MakeScrollCoco()
{
    /*
    containerCoco = CCLayer::create();
    containerCoco->setPosition(ccp(91, 242));
    int numOfSlots = 7;
    
    char name[50];
    for (int i = 0 ; i < numOfSlots ; i++)
    {
        sprintf(name, "background/bg_skill_yellow.png%d", i);
        spriteClassCoco->spriteObj.push_back( SpriteObject::Create(0, name,
                    ccp(0, 0), ccp(148*i, 0), CCSize(0, 0), "", "Layer", containerCoco, 10) );
        spriteClassCoco->AddChild(spriteClassCoco->spriteObj.size()-1);
    }
    
    scrollViewCoco->setDirection(kCCScrollViewDirectionHorizontal);
    scrollViewCoco->setViewSize(CCSizeMake(782, 177)); //782-38, 146));
    scrollViewCoco->setAnchorPoint(ccp(0, 0));
    scrollViewCoco->setPosition(ccp(77, 228));
    containerCoco->setContentSize(CCSizeMake(146*numOfSlots, 146));
    scrollViewCoco->setContentSize(containerCoco->getContentSize());
    scrollViewCoco->setContainer(containerCoco);
    scrollViewCoco->setDelegate(this);
    //this->addChild(scrollView, 5);
    coco->addChild(scrollViewCoco, 5);
    */
    // 안 보임... 수정해야함
}

void CocoRoom::MakeScrollFairy()
{
    containerFairy = CCLayer::create();
    containerFairy->setPosition(ccp(91, 242));
    
    // ...
}


bool CocoRoom::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    if (isTouched)
        return false;
    isTouched = true;
    isScrolling = false;
    
    CCPoint point = pTouch->getLocation();
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        if (spriteClass->spriteObj[i]->name == "background/bg_sketchbook_select.png1")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClickboard();
                MakeSprites(0);
            }
        }
        else if (spriteClass->spriteObj[i]->name == "background/bg_sketchbook_select.png2")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClickboard();
                MakeSprites(1);
            }
        }
        else if (spriteClass->spriteObj[i]->name == "background/bg_sketchbook_select.png3")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClickboard();
                MakeSprites(2);
            }
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_x_yellow.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                EndScene();
                return true;
            }
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_plus_big.png")
        {
            if (curState == 0 && spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClickboard();
                // 슬롯 구매
            }
            else if (curState == 1 && spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClickboard();
                Common::ShowNextScene(this, "CocoRoom", "CocoRoomFairyTown", false); // 요정의 마을
            }
            else if (curState == 2 && spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClickboard();
                Common::ShowNextScene(this, "CocoRoom", "CocoRoomTodayCandy", false); // 오.별 친구고르기
            }
        }
    }
    
    for (int i = 0 ; i < spriteClassCoco->spriteObj.size() ; i++)
    {
        if (spriteClassCoco->spriteObj[i]->name == "button/btn_green.png1")
        {
            if (spriteClassCoco->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                
                
                if (myInfo->GetStaffLvNext() == 0)
                {
                    // 이미 지팡이 만렙
                    //std::vector<int> nullData;
                    //Common::ShowPopup(this, "CocoRoom", "NoImage", false, UPGRADE_STAFF_FULL_LEVEL, BTN_1, nullData);
                }
                else
                {
                    sound->playClick();
                    std::vector<int> data;
                    data.push_back(0);
                    data.push_back(myInfo->GetMPNextCostStarcandy());
                    Common::ShowPopup(this, "CocoRoom", "NoImage", false, UPGRADE_STAFF_BY_STARCANDY_TRY, BTN_2, data);
                }
            }
        }
        
        else if (spriteClassCoco->spriteObj[i]->name == "button/btn_green.png2")
        {
            if (spriteClassCoco->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                
                
                if (myInfo->GetStaffLvNext() == 0)
                {
                    // 이미 지팡이 만렙
                    //std::vector<int> nullData;
                    //Common::ShowPopup(this, "CocoRoom", "NoImage", false, UPGRADE_STAFF_FULL_LEVEL, BTN_1, nullData);
                }
                else
                {
                    sound->playClick();
                    std::vector<int> data;
                    data.push_back(0);
                    data.push_back(myInfo->GetMPNextCostTopaz());
                    Common::ShowPopup(this, "CocoRoom", "NoImage", false, UPGRADE_STAFF_BY_TOPAZ_TRY, BTN_2, data);
                }
            }
        }
    }
    
    for (int i = 0 ; i < spriteClassFairy->spriteObj.size() ; i++)
    {
        if (spriteClassFairy->spriteObj[i]->name == "button/btn_green.png1")
        {
            if (spriteClassFairy->spriteObj[i]->sprite->boundingBox().containsPoint(point))
                ;//sound->playTest();
        }
    }
    
    return true;
}


void CocoRoom::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
    //CCPoint point = pTouch->getLocation();
}

void CocoRoom::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    //CCPoint point = pTouch->getLocation();
    isTouched = false;
}


void CocoRoom::scrollViewDidScroll(CCScrollView* view)
{
    isScrolling = true;
    //CCLog("cocoroom - scrolling~");
}

void CocoRoom::scrollViewDidZoom(CCScrollView* view)
{
}


void CocoRoom::EndScene()
{
    sound->playClick();
    
    CCString* param = CCString::create("0");
    CCNotificationCenter::sharedNotificationCenter()->postNotification("GameReady", param);

    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    
    scrollViewCoco->removeAllChildren();
    scrollViewCoco->removeFromParentAndCleanup(true);
    scrollViewFairy->removeAllChildren();
    scrollViewFairy->removeFromParentAndCleanup(true);

    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, "CocoRoom");
    this->removeFromParentAndCleanup(true);
}

void CocoRoom::EndSceneCallback()
{
}


