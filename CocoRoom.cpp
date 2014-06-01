#include "CocoRoom.h"
#include "pugixml/pugixml.hpp"

using namespace pugi;

static int tabNumber;
//static int priority;

CCScene* CocoRoom::scene(int tab, int prio)
{
    tabNumber = tab;
    //priority = prio;
    
    CCScene* pScene = CCScene::create();
    CocoRoom* pLayer = CocoRoom::create();
    pScene->addChild(pLayer);
    
	return pScene;
}

void CocoRoom::onEnter()
{
    CCLog("CocoRoom : onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority(), true);
    CCLayer::onEnter();
}
void CocoRoom::onExit()
{
    CCLog("CocoRoom : onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
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

    // make depth tree
    Depth::AddCurDepth("CocoRoom", this);
    
    this->setTouchEnabled(true);
    this->setKeypadEnabled(true);
    this->setTouchPriority(Depth::GetCurPriority());
    CCLog("CocoRoom : touch prio = %d", this->getTouchPriority());
    
    // notification observer
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(CocoRoom::Notification), Depth::GetCurName(), NULL);
    
    // notification
    CCString* param = CCString::create("1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetParentName(), param);
    
    
    winSize = CCDirector::sharedDirector()->getWinSize();

    // scrollView init.
    scrollView = CCScrollView::create();
    //scrollView->retain();
    scrollView->setDirection(kCCScrollViewDirectionHorizontal);
    scrollView->setViewSize(CCSizeMake(782-40, 177-20));
    scrollView->setAnchorPoint(ccp(0, 0));
    scrollView->setPosition(ccp(77+20, 228+12));
    scrollView->setDelegate(this);
    scrollView->setTouchPriority(Depth::GetCurPriority());
    this->addChild(scrollView, 11);

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
    spriteClassFairySlot = new SpriteClass();
    spriteClassCocoSlot = new SpriteClass();
    spriteClassCandyList = new SpriteClass();
    
    par = NULL;
    
    InitSprites();
    curState = -1;
    MakeSprites(tabNumber);
    
    isTodayCandyWorking = false;
    
    return true;
}

void CocoRoom::Notification(CCObject* obj)
{
    CCString* param = (CCString*)obj;
    
    if (param->intValue() == 0)
    {
        // 터치 활성
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority()+1, true);
        this->setTouchPriority(Depth::GetCurPriority());
        isTouched = false;
        CCLog("CocoRoom : 터치 활성 (Priority = %d)", this->getTouchPriority());
        
        // 토파즈, 별사탕, MP 정보 업데이트
        ((CCLabelTTF*)spriteClass->FindLabelByTag(1))->setString(Common::MakeComma(myInfo->GetTopaz()).c_str());
        ((CCLabelTTF*)spriteClass->FindLabelByTag(2))->setString(Common::MakeComma(myInfo->GetStarCandy()).c_str());
        ((CCLabelTTF*)spriteClass->FindLabelByTag(3))->setString(Common::MakeComma(myInfo->GetMPTotal()).c_str());
        
        // 코코의 정보 갱신
        if (curState == 1)
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
            
            // 지팡이 다음 강화에 필요한 cost 및 버튼 그림 갱신
            SetStaffBuildUp();
        }
        // 요정의 정보 갱신
        else if (curState == 2)
        {
            // 요정 레벨, 기본속성, 추가속성 값 갱신
            int fid = myInfo->GetActiveFairyId();
            FairyInfo* f = FairyInfo::GetObj(fid);
            
            char fname[30];
            if (fid > 0)
                sprintf(fname, "%s (%dLv)", f->GetName().c_str(), myInfo->GetActiveFairyLevel());
            else
                sprintf(fname, "요정 없음");
            ((CCLabelTTF*)spriteClassFairy->FindLabelByTag(0))->setString(fname);
            
            if (fid > 0)
                sprintf(fname, "%s", FairyInfo::GetAbilityName(f, myInfo->GetActiveFairyLevel()).c_str());
            else
                sprintf(fname, "없음");
            ((CCLabelTTF*)spriteClassFairy->FindLabelByTag(2))->setString(fname);
            
            // 요정 다음 강화에 필요한 cost 및 버튼 그림 갱신
            SetFairyBuildUp();
        }
        // 오.별 정보 갱신
        else
        {
            SetTodayCandyList();
        }
    }
    else if (param->intValue() == 1)
    {
        // 터치 비활성
        CCLog("CocoRoom : 터치 비활성");
        CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
    }
    else if (param->intValue() == 2)
    {
        // 토파즈, 별사탕, MP 정보 업데이트
        ((CCLabelTTF*)spriteClass->FindLabelByTag(1))->setString(Common::MakeComma(myInfo->GetTopaz()).c_str());
        ((CCLabelTTF*)spriteClass->FindLabelByTag(2))->setString(Common::MakeComma(myInfo->GetStarCandy()).c_str());
        ((CCLabelTTF*)spriteClass->FindLabelByTag(3))->setString(Common::MakeComma(myInfo->GetMPTotal()).c_str());
    }
    else if (param->intValue() == 8)
    {
        // 요정 슬롯 정보 갱신
        spriteClassFairySlot->RemoveAllObjects();
        containerFairy->removeAllChildren();
        containerFairy->removeFromParentAndCleanup(true);
        MakeScrollFairy();
    }
    else if (param->intValue() == 9)
    {
        // 스킬 슬롯 정보 갱신
        spriteClassCocoSlot->RemoveAllObjects();
        containerCoco->removeAllChildren();
        containerCoco->removeFromParentAndCleanup(true);
        MakeScrollCoco();
        char name[7];
        sprintf(name, "%d", (int)myInfo->GetSlot().size());
        ((CCLabelTTF*)spriteClassCoco->FindLabelByTag(100))->setString(name);
    }
}

int offset = -110;
int offset2 = -70;

void CocoRoom::InitSprites()
{
    CCSprite* pBlack = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, winSize.width, winSize.height));
    pBlack->setPosition(ccp(0, 0));
    pBlack->setAnchorPoint(ccp(0, 0));
    pBlack->setColor(ccc3(0, 0, 0));
    pBlack->setOpacity(200);
    this->addChild(pBlack, 0);
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_topinfo.png1", ccp(0, 0), ccp(80, 1666), CCSize(230, 75), "", "CocoRoom", this, 0) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_topinfo.png2", ccp(0, 0), ccp(390, 1666), CCSize(290, 75), "", "CocoRoom", this, 0) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_topinfo.png3", ccp(0, 0), ccp(765, 1666), CCSize(290, 75), "", "CocoRoom", this, 0) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_topaz.png", ccp(0, 0), ccp(15+10, 1656), CCSize(0, 0), "", "CocoRoom", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_starcandy.png", ccp(0, 0), ccp(317, 1660), CCSize(0, 0), "", "CocoRoom", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_magicpoint.png", ccp(0, 0), ccp(696, 1669), CCSize(0, 0), "", "CocoRoom", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_topinfo_plus.png1", ccp(0, 0), ccp(80+230-55, 1679), CCSize(0, 0), "", "CocoRoom", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_topinfo_plus.png2", ccp(0, 0), ccp(390+290-55, 1679), CCSize(0, 0), "", "CocoRoom", this, 5) );
    
    // topaz
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(Common::MakeComma(myInfo->GetTopaz()), fontList[0], 36, ccp(0.5, 0), ccp((80+230+80)/2, 1686), ccc3(255,255,255), "", "CocoRoom", this, 5, 0, 255, 1) );
    // starcandy
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(Common::MakeComma(myInfo->GetStarCandy()), fontList[0], 36, ccp(0.5, 0), ccp((390+290+390)/2, 1686), ccc3(255,255,255), "", "CocoRoom", this, 5, 0, 255, 2) );
    // magic-point
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(Common::MakeComma(myInfo->GetMPTotal()), fontList[0], 36, ccp(0.5, 0), ccp((765+765+290)/2, 1686), ccc3(255,255,255), "", "CocoRoom", this, 5, 0, 255, 3) );
    
    
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
            ccp(0, 0), ccp(896, 312), CCSize(0, 0), "", "CocoRoom", this, 1) );
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
}

void CocoRoom::MakeSprites(int state)
{
    if (curState == state)
        return;
    
    // scrollview + scrollContainer
    spriteClassCocoSlot->RemoveAllObjects();
    spriteClassFairySlot->RemoveAllObjects();
    scrollView->removeAllChildren();
    
    // layer init.
    spriteClassFairySlot->RemoveAllObjects();
    spriteClassCocoSlot->RemoveAllObjects();
    spriteClassCandyList->RemoveAllObjects();
    
    spriteClassCoco->RemoveAllObjects();
    coco->removeAllChildren();
    spriteClassFairy->RemoveAllObjects();
    fairy->removeAllChildren();
    spriteClassCandy->RemoveAllObjects();
    candy->removeAllChildren();
    
    if (par != NULL)
        par->setVisible(false);
    
    if (abs(state) == 1)
    {
        MakeSpritesCoco();
        MakeScrollCoco();
        par->setVisible(true);
    }
    else if (abs(state) == 2)
    {
        MakeSpritesFairy();
        MakeScrollFairy();
    }
    else
    {
        MakeSpritesCandy();
        // 가짜 container (scrollview touch delegate 에러 방지용)
        scrollView->setContainer(CCLayer::create());
    }
    
    if (state > 0) // 일반 탭 변경일 때만 바꿔야함. (음수 = 요정변경 등)
        curState = state;
}

void CocoRoom::MakeSpritesCoco()
{
    // 슬롯 부분
    spriteClassCoco->spriteObj.push_back( SpriteObject::Create(1, "background/bg_gameready_name.png", ccp(0, 0), ccp(867, 237), CCSize(136, 63), "", "CocoRoom", this, 1) );
    // 구입 개수
    char val[6];
    sprintf(val, "%d", (int)myInfo->GetSlot().size());
    spriteClassCoco->spriteObj.push_back( SpriteObject::CreateLabel(val, fontList[0], 48, ccp(0, 0), ccp(892, 245), ccc3(255,219,53), "", "CocoRoom", this, 5, 0, 255, 100) ); // 현재 개수
    sprintf(val, "/ %d", (int)skillSlotInfo.size());
    spriteClassCoco->spriteObj.push_back( SpriteObject::CreateLabel(val, fontList[0], 36, ccp(0, 0), ccp(927, 245), ccc3(182,142,142), "", "CocoRoom", this, 5) ); // 전체 슬롯 개수
    
    spriteClassCoco->spriteObj.push_back( SpriteObject::Create(1, "background/bg_degree_desc.png",
                        ccp(0, 0), ccp(441, 1290+offset), CCSize(543, 61), "", "Layer", coco, 5) );
    spriteClassCoco->spriteObj.push_back( SpriteObject::CreateLabel("오늘도 곰을 백마리 잡은 마법사", fontList[0], 36, ccp(0.5, 0.5), spriteClassCoco->FindParentCenterPos("background/bg_degree_desc.png"), ccc3(255,255,255), "background/bg_degree_desc.png", "1", NULL, 5, 1) );
    
    // stone floor
    spriteClassCoco->spriteObj.push_back( SpriteObject::Create(0, "background/stone_floor.png",
                        ccp(0, 0), ccp(95, 740+offset), CCSize(0, 0), "", "Layer", coco, 1) );
    
    // coco character
    spriteClassCoco->spriteObj.push_back( SpriteObject::Create(0, "image/coco_room.png", ccp(0.5, 0), ccp(315, 840+offset), CCSize(0, 0), "", "Layer", coco, 5) );
    ((CCSprite*)spriteClassCoco->FindSpriteByName("image/coco_room.png"))->setScale(0.95f);
    
    CCFiniteTimeAction* action = CCSequence::create(CCMoveBy::create(1.0f, ccp(0, +5)), CCMoveBy::create(1.0f, ccp(0, -5)), NULL);
    CCAction* rep = CCRepeatForever::create((CCActionInterval*)action);
    ((CCSprite*)spriteClassCoco->FindSpriteByName("image/coco_room.png"))->runAction(rep);
    
    // 지팡이 particle
    CCSprite* sp = CCSprite::create("particles/effect_10.png");
    par = CCParticleFlower::create();
    //par->retain();
    par->setTexture(sp->getTexture());
    par->setAnchorPoint(ccp(0.5, 0.5));
    par->setPosition(ccp(474, 1094+offset));
    par->setStartSize(65);
    par->setLife(0.4);
    par->setSpeed(120);
    par->setSpeedVar(70);
    ccColor4F startColor = par->getStartColor();
    startColor.r = 0.95f;
    startColor.g = 0.95f;
    startColor.b = 0.95f;
    par->setStartColor(startColor);
    par->setDuration(-1);
    this->addChild(par, 100);
    CCFiniteTimeAction* action2 = CCSequence::create(CCMoveBy::create(1.0f, ccp(0, +5)), CCMoveBy::create(1.0f, ccp(0, -5)), NULL);
    CCAction* rep2 = CCRepeatForever::create((CCActionInterval*)action2);
    par->runAction(rep2);

    
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
    sprintf(val, "%d", myInfo->GetMPTotal());
    spriteClassCoco->spriteObj.push_back( SpriteObject::CreateLabel(val, fontList[0], 60, ccp(0.5, 0.5), ccp(800, 1090+35+offset), ccc3(255,255,255), "", "Layer", coco, 5, 0, 255, 1) );
    sprintf(val, "%d", myInfo->GetMP());
    spriteClassCoco->spriteObj.push_back( SpriteObject::CreateLabel(val, fontList[0], 52, ccp(0.5, 0.5), ccp(800, 999+32+offset), ccc3(255,255,255), "", "Layer", coco, 5, 0, 255, 2) );
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

    // 만렙인 경우
    spriteClassCoco->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_lock_white.png1", ccp(0, 0), ccp(205, 531), CCSize(0, 0), "", "Layer", coco, 5, 0, 0) );
    spriteClassCoco->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_lock_white.png2", ccp(0, 0), ccp(652, 531), CCSize(0, 0), "", "Layer", coco, 5, 0, 0) );
    sprintf(val, "강화 완료");
    spriteClassCoco->spriteObj.push_back( SpriteObject::CreateLabel(val, fontList[0], 36, ccp(0, 0), ccp(270, 541), ccc3(22,56,0), "", "Layer", coco, 5, 0, 0, 7) );
    spriteClassCoco->spriteObj.push_back( SpriteObject::CreateLabel(val, fontList[0], 36, ccp(0, 0), ccp(720, 541), ccc3(22,56,0), "", "Layer", coco, 5, 0, 0, 8) );

    // 일반적인 경우
    spriteClassCoco->spriteObj.push_back( SpriteObject::Create(0, "button/btn_receive_starcandy.png", ccp(0, 0), ccp(224, 531), CCSize(0, 0), "", "Layer", coco, 5) );
    spriteClassCoco->spriteObj.push_back( SpriteObject::Create(0, "button/btn_receive_topaz.png", ccp(0, 0), ccp(671, 531), CCSize(0, 0), "", "Layer", coco, 5) );
    if (myInfo->GetStaffLv() == magicStaffBuildupInfo[magicStaffBuildupInfo.size()-1]->GetLevel())
        sprintf(val, "");
    else
        sprintf(val, "x %d", magicStaffBuildupInfo[myInfo->GetStaffLv()+1-1]->GetCost_StarCandy());
    spriteClassCoco->spriteObj.push_back( SpriteObject::CreateLabel(val, fontList[0], 36, ccp(0, 0), ccp(300, 541), ccc3(22,56,0), "", "Layer", coco, 5, 0, 255, 5) );
    if (myInfo->GetStaffLv() == magicStaffBuildupInfo[magicStaffBuildupInfo.size()-1]->GetLevel())
        sprintf(val, "");
    else
        sprintf(val, "x %d", magicStaffBuildupInfo[myInfo->GetStaffLv()+1-1]->GetCost_Topaz());
    spriteClassCoco->spriteObj.push_back( SpriteObject::CreateLabel(val, fontList[0], 36, ccp(0, 0), ccp(750, 541), ccc3(22,56,0), "", "Layer", coco, 5, 0, 255, 6) );
    
    SetStaffBuildUp();
    
    for (int i = 0 ; i < spriteClassCoco->spriteObj.size() ; i++)
        spriteClassCoco->AddChild(i);
}

void CocoRoom::SetStaffBuildUp()
{
    // 일반적인 경우
    if (myInfo->GetStaffLv() < magicStaffBuildupInfo[magicStaffBuildupInfo.size()-1]->GetLevel())
    {
        // 지팡이 일반/고급 다음 cost 갱신
        char val[20];
        sprintf(val, "x %d", magicStaffBuildupInfo[myInfo->GetStaffLv()+1-1]->GetCost_StarCandy());
        ((CCLabelTTF*)spriteClassCoco->FindLabelByTag(5))->setString(val);
        sprintf(val, "x %d", magicStaffBuildupInfo[myInfo->GetStaffLv()+1-1]->GetCost_Topaz());
        ((CCLabelTTF*)spriteClassCoco->FindLabelByTag(6))->setString(val);
        
        ((CCSprite*)spriteClassCoco->FindSpriteByName("button/btn_receive_starcandy.png"))->setOpacity(255);
        ((CCSprite*)spriteClassCoco->FindSpriteByName("button/btn_receive_topaz.png"))->setOpacity(255);
        ((CCLabelTTF*)spriteClassCoco->FindLabelByTag(5))->setOpacity(255);
        ((CCLabelTTF*)spriteClassCoco->FindLabelByTag(6))->setOpacity(255);
        
        ((CCSprite*)spriteClassCoco->FindSpriteByName("icon/icon_lock_white.png1"))->setOpacity(0);
        ((CCSprite*)spriteClassCoco->FindSpriteByName("icon/icon_lock_white.png2"))->setOpacity(0);
        ((CCLabelTTF*)spriteClassCoco->FindLabelByTag(7))->setOpacity(0);
        ((CCLabelTTF*)spriteClassCoco->FindLabelByTag(8))->setOpacity(0);
    }
    // 만렙인 경우
    else
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
}

void CocoRoom::MakeSpritesFairy()
{
    // init
    spriteClassFairy->RemoveAllObjects();
    
    CCNode* pic = fairy->getChildByTag(99999);
    if (pic != NULL)
        pic->removeAllChildren();
    fairy->removeAllChildren();
    
    // 요정 슬롯 부분
    spriteClassFairy->spriteObj.push_back( SpriteObject::CreateLabel("요정보기", fontList[0], 36, ccp(0, 0), ccp(863, 247), ccc3(255,255,255), "", "CocoRoom", this, 5) );
    
    spriteClassFairy->spriteObj.push_back( SpriteObject::Create(1, "background/bg_cocoroom_desc.png1",
                        ccp(0, 0), ccp(524, 1271+offset), CCSize(439, 89), "", "Layer", fairy, 5) );
    spriteClassFairy->spriteObj.push_back( SpriteObject::Create(1, "background/bg_cocoroom_desc.png2",
                        ccp(0, 0), ccp(524, 1186+offset), CCSize(439, 58), "", "Layer", fairy, 5) );
    spriteClassFairy->spriteObj.push_back( SpriteObject::Create(1, "background/bg_cocoroom_desc.png3",
                        ccp(0, 0), ccp(524, 1124+offset), CCSize(439, 58), "", "Layer", fairy, 5) );
    spriteClassFairy->spriteObj.push_back( SpriteObject::Create(1, "background/bg_cocoroom_desc.png4",
                        ccp(0, 0), ccp(524, 1026+offset), CCSize(439, 58), "", "Layer", fairy, 5) );
    
    // stone floor
    spriteClassFairy->spriteObj.push_back( SpriteObject::Create(0, "background/fairy_floor.png", ccp(0, 0), ccp(100, 760+offset), CCSize(0, 0), "", "Layer", fairy, 5) );
    
    
    int fid = myInfo->GetActiveFairyId();
    int flv = myInfo->GetActiveFairyLevel();
    FairyInfo* f = FairyInfo::GetObj(fid);
    
    // 요정 그림
    CCLayer* picture = Fairy::GetFairy(fid);
    switch (fid)
    {
        case 1: picture->setScale(1.2f); break;
        case 2: picture->setScale(1.2f); break;
        default: picture->setScale(2.0f); break;
    }
    picture->setAnchorPoint(ccp(0, 0));
    picture->setPosition(ccp(300, 1186+offset-50));
    picture->setTag(99999);
    fairy->addChild(picture, 100);
    
    CCActionInterval* action = CCSequence::create(CCMoveBy::create(1.0f, ccp(0, +5)), CCMoveBy::create(1.0f, ccp(0, -5)), NULL);
    picture->runAction(CCRepeatForever::create(action));
    
    char fname[30];
    
    // 요정 이름(레벨)
    if (fid > 0)
        sprintf(fname, "%s (%dLv)", f->GetName().c_str(), flv);
    else
        sprintf(fname, "요정 없음");
    spriteClassFairy->spriteObj.push_back( SpriteObject::CreateLabel(fname, fontList[0], 52, ccp(0.5, 0.5), spriteClassFairy->FindParentCenterPos("background/bg_cocoroom_desc.png1"), ccc3(255,255,255), "background/bg_cocoroom_desc.png1", "1", NULL, 5, 1, 255, 0) );
    
    spriteClassFairy->spriteObj.push_back( SpriteObject::CreateLabel("기본속성", fontList[2], 36, ccp(0, 0), ccp(570, 1193+offset), ccc3(121,71,0), "", "Layer", fairy, 5) );
    spriteClassFairy->spriteObj.push_back( SpriteObject::CreateLabel("기본속성", fontList[2], 36, ccp(0, 0), ccp(570, 1196+offset), ccc3(255,219,53), "", "Layer", fairy, 5) );
    spriteClassFairy->spriteObj.push_back( SpriteObject::CreateLabel("추가속성", fontList[2], 36, ccp(0, 0), ccp(570, 1131+offset), ccc3(121,71,0), "", "Layer", fairy, 5) );
    spriteClassFairy->spriteObj.push_back( SpriteObject::CreateLabel("추가속성", fontList[2], 36, ccp(0, 0), ccp(570, 1134+offset), ccc3(255,219,53), "", "Layer", fairy, 5) );
    spriteClassFairy->spriteObj.push_back( SpriteObject::CreateLabel("특수능력", fontList[2], 36, ccp(0, 0), ccp(570, 1033+offset), ccc3(121,71,0), "", "Layer", fairy, 5) );
    spriteClassFairy->spriteObj.push_back( SpriteObject::CreateLabel("특수능력", fontList[2], 36, ccp(0, 0), ccp(570, 1036+offset), ccc3(255,219,53), "", "Layer", fairy, 5) );
    
    // 기본속성 값
    spriteClassFairy->spriteObj.push_back( SpriteObject::CreateLabel("MP + 100", fontList[0], 36, ccp(0, 0), ccp(720, 1196+offset), ccc3(255,255,255), "", "Layer", fairy, 5, 0, 255, 1) );
    
    // 추가속성 값
    if (fid > 0)
        sprintf(fname, "%s", FairyInfo::GetAbilityName(f, flv).c_str());
    else
        sprintf(fname, "없음");
    spriteClassFairy->spriteObj.push_back( SpriteObject::CreateLabel(fname, fontList[0], 36, ccp(0, 0), ccp(720, 1134+offset), ccc3(0,167,222), "", "Layer", fairy, 5, 0, 255, 2) );
    
    // 특수능력 값
    if (fid > 0)
        sprintf(fname, "%s", FairyInfo::GetAbilityDesc(f->GetType()).c_str());
    else
        sprintf(fname, "없음");
    spriteClassFairy->spriteObj.push_back( SpriteObject::CreateLabel(fname, fontList[0], 36, ccp(0, 0), ccp(720, 1036+offset), ccc3(255,255,255), "", "Layer", fairy, 5) );
    
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
    
    char val[20];
    // 만렙인 경우
    spriteClassFairy->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_lock_white.png1", ccp(0, 0), ccp(205, 531), CCSize(0, 0), "", "Layer", fairy, 5, 0, 0) );
    spriteClassFairy->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_lock_white.png2", ccp(0, 0), ccp(652, 531), CCSize(0, 0), "", "Layer", fairy, 5, 0, 0) );
    if (fid > 0)
        sprintf(val, "강화 완료");
    else
        sprintf(val, "요정 없음");
    spriteClassFairy->spriteObj.push_back( SpriteObject::CreateLabel(val, fontList[0], 36, ccp(0, 0), ccp(270, 541), ccc3(22,56,0), "", "Layer", fairy, 5, 0, 0, 11) );
    spriteClassFairy->spriteObj.push_back( SpriteObject::CreateLabel(val, fontList[0], 36, ccp(0, 0), ccp(720, 541), ccc3(22,56,0), "", "Layer", fairy, 5, 0, 0, 12) );
    
    // 일반적인 경우
    spriteClassFairy->spriteObj.push_back( SpriteObject::Create(0, "button/btn_receive_starcandy.png", ccp(0, 0), ccp(224, 531), CCSize(0, 0), "", "Layer", fairy, 5) );
    spriteClassFairy->spriteObj.push_back( SpriteObject::Create(0, "button/btn_receive_topaz.png", ccp(0, 0), ccp(671, 531), CCSize(0, 0), "", "Layer", fairy, 5) );
    // 일반강화 가격
    if (flv == FairyBuildUpInfo::GetMaxLevel(fid))
        sprintf(val, "");
    else
        sprintf(val, "x %d", FairyBuildUpInfo::GetCostStarCandy(fid, flv+1));
    spriteClassFairy->spriteObj.push_back( SpriteObject::CreateLabel(val, fontList[0], 36, ccp(0, 0), ccp(300, 541), ccc3(22,56,0), "", "Layer", fairy, 5, 0, 255, 9) );
    // 고급강화 가격
    if (flv == FairyBuildUpInfo::GetMaxLevel(fid))
        sprintf(val, "");
    else
        sprintf(val, "x %d", FairyBuildUpInfo::GetCostTopaz(fid, flv+1));
    spriteClassFairy->spriteObj.push_back( SpriteObject::CreateLabel(val, fontList[0], 36, ccp(0, 0), ccp(750, 541), ccc3(22,56,0), "", "Layer", fairy, 5, 0, 255, 10) );
    
    SetFairyBuildUp();
    
    for (int i = 0 ; i < spriteClassFairy->spriteObj.size() ; i++)
        spriteClassFairy->AddChild(i);
}

void CocoRoom::SetFairyBuildUp()
{
    int fid = myInfo->GetActiveFairyId();
    int flv = myInfo->GetActiveFairyLevel();
    // 일반적인 경우
    if (flv < FairyBuildUpInfo::GetMaxLevel(fid))
    {
        CCLog("fid flv SSS = %d %d", fid, flv);
        CCLog("%d", FairyBuildUpInfo::GetCostStarCandy(fid, flv+1));
        CCLog("%d", FairyBuildUpInfo::GetCostTopaz(fid, flv+1));
        // 지팡이 일반/고급 다음 cost 갱신
        char val[20];
        sprintf(val, "x %d", FairyBuildUpInfo::GetCostStarCandy(fid, flv+1));
        ((CCLabelTTF*)spriteClassFairy->FindLabelByTag(9))->setString(val);
        sprintf(val, "x %d", FairyBuildUpInfo::GetCostTopaz(fid, flv+1));
        ((CCLabelTTF*)spriteClassFairy->FindLabelByTag(10))->setString(val);
        
        ((CCSprite*)spriteClassFairy->FindSpriteByName("button/btn_receive_starcandy.png"))->setOpacity(255);
        ((CCSprite*)spriteClassFairy->FindSpriteByName("button/btn_receive_topaz.png"))->setOpacity(255);
        ((CCLabelTTF*)spriteClassFairy->FindLabelByTag(9))->setOpacity(255);
        ((CCLabelTTF*)spriteClassFairy->FindLabelByTag(10))->setOpacity(255);
        
        ((CCSprite*)spriteClassFairy->FindSpriteByName("icon/icon_lock_white.png1"))->setOpacity(0);
        ((CCSprite*)spriteClassFairy->FindSpriteByName("icon/icon_lock_white.png2"))->setOpacity(0);
        ((CCLabelTTF*)spriteClassFairy->FindLabelByTag(11))->setOpacity(0);
        ((CCLabelTTF*)spriteClassFairy->FindLabelByTag(12))->setOpacity(0);
    }
    // 만렙인 경우
    else
    {
        ((CCSprite*)spriteClassFairy->FindSpriteByName("button/btn_receive_starcandy.png"))->setOpacity(0);
        ((CCSprite*)spriteClassFairy->FindSpriteByName("button/btn_receive_topaz.png"))->setOpacity(0);
        ((CCLabelTTF*)spriteClassFairy->FindLabelByTag(9))->setOpacity(0);
        ((CCLabelTTF*)spriteClassFairy->FindLabelByTag(10))->setOpacity(0);
        
        ((CCSprite*)spriteClassFairy->FindSpriteByName("icon/icon_lock_white.png1"))->setOpacity(255);
        ((CCSprite*)spriteClassFairy->FindSpriteByName("icon/icon_lock_white.png2"))->setOpacity(255);
        ((CCLabelTTF*)spriteClassFairy->FindLabelByTag(11))->setOpacity(255);
        ((CCLabelTTF*)spriteClassFairy->FindLabelByTag(12))->setOpacity(255);
    }
}


void CocoRoom::MakeSpritesCandy()
{
    spriteClassCandy->RemoveAllObjects();
    
    // 마법진 배경
    spriteClassCandy->spriteObj.push_back( SpriteObject::Create(0, "background/bg_todaycandy.png", ccp(0.5, 0.5), ccp(188+357, 528+357), CCSize(0, 0), "", "Layer", candy, 79) );

    // 5명의 이름
    spriteClassCandy->spriteObj.push_back( SpriteObject::CreateLabel(MyInfo::GetName(), fontList[0], 36, ccp(0.5, 1), ccp(542, 1210+offset2-65), ccc3(255,255,255), "", "Layer", candy, 110, 0, 255, 0) );
    spriteClassCandy->spriteObj.push_back( SpriteObject::CreateLabel("?", fontList[0], 36, ccp(0.5, 1), ccp(273, 1015+offset2-65), ccc3(255,255,255), "", "Layer", candy, 110, 0, 255, 1) );
    spriteClassCandy->spriteObj.push_back( SpriteObject::CreateLabel("?", fontList[0], 36, ccp(0.5, 1), ccp(334, 752+offset2-65), ccc3(255,255,255), "", "Layer", candy, 110, 0, 255, 2) );
    spriteClassCandy->spriteObj.push_back( SpriteObject::CreateLabel("?", fontList[0], 36, ccp(0.5, 1), ccp(728, 752+offset2-65), ccc3(255,255,255), "", "Layer", candy, 110, 0, 255, 3) );
    spriteClassCandy->spriteObj.push_back( SpriteObject::CreateLabel("?", fontList[0], 36, ccp(0.5, 1), ccp(805, 1015+offset2-65), ccc3(255,255,255), "", "Layer", candy, 110, 0, 255, 4) );
    
    SetTodayCandyList(); // 5명의 프로필 사진과 이름을 들고온다.
    
    CCSize s0 = ((CCLabelTTF*)spriteClassCandy->FindLabelByTag(0))->getContentSize();
    CCSize s1 = ((CCLabelTTF*)spriteClassCandy->FindLabelByTag(1))->getContentSize();
    CCSize s2 = ((CCLabelTTF*)spriteClassCandy->FindLabelByTag(2))->getContentSize();
    CCSize s3 = ((CCLabelTTF*)spriteClassCandy->FindLabelByTag(3))->getContentSize();
    CCSize s4 = ((CCLabelTTF*)spriteClassCandy->FindLabelByTag(4))->getContentSize();
    
    // name의 배경
    spriteClassCandy->spriteObj.push_back( SpriteObject::Create(1, "background/bg_cocoroom_desc.png1", ccp(0.5, 1), ccp(542, 1210+offset2-65), CCSize(std::max(150, (int)s0.width+30), 58), "", "Layer", candy, 105) );
    spriteClassCandy->spriteObj.push_back( SpriteObject::Create(1, "background/bg_cocoroom_desc.png2", ccp(0.5, 1), ccp(273, 1015+offset2-65), CCSize(std::max(150, (int)s1.width+30), 58), "", "Layer", candy, 105) );
    spriteClassCandy->spriteObj.push_back( SpriteObject::Create(1, "background/bg_cocoroom_desc.png3", ccp(0.5, 1), ccp(334, 752+offset2-65), CCSize(std::max(150, (int)s2.width+30), 58), "", "Layer", candy, 105) );
    spriteClassCandy->spriteObj.push_back( SpriteObject::Create(1, "background/bg_cocoroom_desc.png4", ccp(0.5, 1), ccp(728, 752+offset2-65), CCSize(std::max(150, (int)s3.width+30), 58), "", "Layer", candy, 105) );
    spriteClassCandy->spriteObj.push_back( SpriteObject::Create(1, "background/bg_cocoroom_desc.png5", ccp(0.5, 1), ccp(805, 1015+offset2-65), CCSize(std::max(150, (int)s4.width+30), 58), "", "Layer", candy, 105) );
    
    
    // question
    spriteClassCandy->spriteObj.push_back( SpriteObject::Create(0, "button/btn_question.png", ccp(0, 0), ccp(916, 515), CCSize(0, 0), "", "Layer", candy, 5) );
    
    if (!myInfo->IsTodayCandyUsed())
    {
        // button
        spriteClassCandy->spriteObj.push_back( SpriteObject::Create(0, "button/btn_red_mini.png", ccp(0, 0), ccp(435, 842+65+offset2), CCSize(0, 0), "", "Layer", candy, 105) );
        spriteClassCandy->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_fairy_select.png", ccp(0.5, 0), ccp(spriteClassCandy->spriteObj[spriteClassCandy->spriteObj.size()-1]->sprite->getContentSize().width/2, 30), CCSize(0, 0), "button/btn_red_mini.png", "0", NULL, 105, 1) );
    }
    
    // '친구목록' 글자
    spriteClassCandy->spriteObj.push_back( SpriteObject::CreateLabel("친구목록", fontList[0], 36, ccp(0, 0), ccp(870, 247), ccc3(255,255,255), "", "Layer", candy, 5) );
    
    for (int i = 0 ; i < spriteClassCandy->spriteObj.size() ; i++)
        spriteClassCandy->AddChild(i);
}

void CocoRoom::SetTodayCandyList()
{
    for (int i = 0 ; i < 5 ; i++)
        ((CCLabelTTF*)spriteClassCandy->FindLabelByTag(i))->setString("?");
    
    spriteClassCandyList->RemoveAllObjects();
    
    int idx;
    CCPoint pos[] = { ccp(542, 1210+offset2), ccp(273, 1015+offset2), ccp(334, 752+offset2),
                        ccp(728, 752+offset2), ccp(805, 1015+offset2) };
    
    char name[50];
    for (int i = 0 ; i < todayCandyKakaoId.size() ; i++)
    {
        if (todayCandyKakaoId[i] != -1)
        {
            for (int j = 0 ; j < friendList.size() ; j++)
            {
                if (friendList[j]->GetKakaoId() == todayCandyKakaoId[i])
                    idx = j;
            }
            // 프로필 이미지
            CCSprite* profile = ProfileSprite::GetProfile(friendList[idx]->GetImageUrl());
            if (friendList[idx]->GetImageUrl() != "")
            {
                spriteClassCandyList->spriteObj.push_back( SpriteObject::CreateFromSprite(0, profile, ccp(0.5, 0.5), pos[i], CCSize(0,0), "", "Layer", candy, 99, 0, 255, 0.85f) );
                sprintf(name, "background/bg_profile.png%d", i);
                spriteClassCandyList->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0.5, 0.5), pos[i], CCSize(0, 0), "", "Layer", candy, 100) );
            }
            else
            {
                spriteClassCandyList->spriteObj.push_back( SpriteObject::CreateFromSprite(0, profile, ccp(0.5, 0.5), pos[i], CCSize(0,0), "", "Layer", candy, 100) );
            }

            // name 변경
            ((CCLabelTTF*)spriteClassCandy->FindLabelByTag(i))->setString(friendList[idx]->GetNickname().c_str());
        }
    }
    
    
    // 보상 설명 부분
    if (myInfo->IsTodayCandyUsed())
    {
        spriteClassCandyList->spriteObj.push_back( SpriteObject::CreateLabelArea("오늘의 별사탕을 이미 실행하였습니다. 매일 오전 3시에 초기화됩니다.", fontList[0], 36, ccp(0.5, 0.5), ccp(77+782/2, 228+177/2), ccc3(78,47,8), CCSize(592, 147), kCCTextAlignmentCenter, kCCVerticalTextAlignmentCenter, "", "Layer", candy, 5) );
    }
    else
    {
        if (myInfo->GetTodayCandyType() == 1)
            spriteClassCandyList->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_starcandy.png", ccp(0, 0), ccp(350, 300), CCSize(0, 0), "", "Layer", candy, 5) );
        else if (myInfo->GetTodayCandyType() == 2)
            spriteClassCandyList->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_topaz.png", ccp(0, 0), ccp(350, 300), CCSize(0, 0), "", "Layer", candy, 5) );
        else if (myInfo->GetTodayCandyType() == 3)
            spriteClassCandyList->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_potion.png", ccp(0, 0), ccp(350, 300), CCSize(0, 0), "", "Layer", candy, 5) );
        
        spriteClassCandyList->spriteObj.push_back( SpriteObject::CreateLabel("오늘의 보상 :", fontList[0], 36, ccp(0, 0), ccp(110, 330-15), ccc3(78,47,8), "", "Layer", candy, 5) );
        
        char name[40];
        sprintf(name, "x %d  (참가상 = x %d)", myInfo->GetTodayCandyValueChoice(), myInfo->GetTodayCandyValueMiss());
        spriteClassCandyList->spriteObj.push_back( SpriteObject::CreateLabel(name, fontList[0], 36, ccp(0, 0), ccp(440, 320), ccc3(78,47,8), "", "Layer", candy, 5) );
        
        spriteClassCandyList->spriteObj.push_back( SpriteObject::CreateLabel("친구와 함께 보상을 받으세요!", fontList[0], 36, ccp(0, 0), ccp(240, 250), ccc3(255,255,255), "", "Layer", candy, 5) );
    }
    
    for (int i = 0 ; i < spriteClassCandyList->spriteObj.size() ; i++)
        spriteClassCandyList->AddChild(i);
}

void CocoRoom::MakeScrollCoco()
{
    int numOfList = myInfo->GetSlot().size();
    
    // container 초기화
    containerCoco = CCLayer::create();
    containerCoco->setContentSize(CCSizeMake(numOfList*(146+5), 146));
    
    char fname[50], fname2[20];
    int scid;
    for (int i = 0 ; i < numOfList ; i++)
    {
        sprintf(fname, "background/bg_skill_yellow.png%d", i);
        spriteClassCocoSlot->spriteObj.push_back( SpriteObject::Create(0, fname, ccp(0, 0), ccp(i*(146+5), 0), CCSize(0, 0), "", "Layer", containerCoco, 3) );
        
        scid = myInfo->GetSlot()[i]->GetCommonId();
        if (scid > 0) // 슬롯에 스킬이 있다면 문양을 표시한다.
        {
            sprintf(fname2, "skill_%d.png", scid);
            spriteClassCocoSlot->spriteObj.push_back( SpriteObject::Create(0, fname2, ccp(0.5, 0.5), spriteClassCocoSlot->FindParentCenterPos(fname), CCSize(0, 0), fname, "0", NULL, 4, 1) );
        }
    }
    
    for (int i = 0 ; i < spriteClassCocoSlot->spriteObj.size() ; i++)
        spriteClassCocoSlot->AddChild(i);
    
    // scrollView의 container 재설정
    scrollView->setContainer(containerCoco);
    scrollView->setContentSize(containerCoco->getContentSize());
}

void CocoRoom::MakeScrollFairy()
{
    int numOfList = myInfo->GetFairyList().size();
    
    // container 초기화
    containerFairy = CCLayer::create();
    containerFairy->setContentSize(CCSizeMake(numOfList*(146+5), 146));

    char fname[50];
    int fid, fuid;
    for (int i = 0 ; i < numOfList ; i++)
    {
        fuid = myInfo->GetFairyList()[i]->GetUserId();
        
        sprintf(fname, "background/bg_skill_yellow.png%d", i);
        spriteClassFairySlot->spriteObj.push_back( SpriteObject::Create(0, fname, ccp(0, 0), ccp(i*(146+5), 0), CCSize(0, 0), "", "Layer", containerFairy, 3, 0, 255, fuid) );
        CCLog("f uid = %d", fuid);
        
        fid = myInfo->GetFairyList()[i]->GetId();
        FairyInfo* f = FairyInfo::GetObj(fid);
        spriteClassFairySlot->spriteObj.push_back( SpriteObject::CreateLabel(f->GetName(), fontList[0], 30, ccp(0.5, 0.5), ccp(i*(146+5)+73, 23), ccc3(104,62,51), "", "Layer", containerFairy, 5) );
        spriteClassFairySlot->spriteObj.push_back( SpriteObject::CreateLabel(f->GetName(), fontList[0], 30, ccp(0.5, 0.5), ccp(i*(146+5)+73+3, 18), ccc3(255,255,255), "", "Layer", containerFairy, 5) );
    }
    
    for (int i = 0 ; i < spriteClassFairySlot->spriteObj.size() ; i++)
        spriteClassFairySlot->AddChild(i);
    
    // scrollView 생성
    scrollView->setContainer(containerFairy);
    scrollView->setContentSize(containerFairy->getContentSize());
}



CCPoint po[] = { ccp(542, 1210+offset2), ccp(334, 752+offset2), ccp(805, 1015+offset2), ccp(273, 1015+offset2), ccp(728, 752+offset2)};
int popo = 0;
int candyIdx[] = {0, 2, 4, 1, 3};

void CocoRoom::DecideUser(int selectedKakaoId)
{
    isTodayCandyWorking = true;
    ((CCSprite*)spriteClassCandy->FindSpriteByName("button/btn_red_mini.png"))->setOpacity(0);
    ((CCSprite*)spriteClassCandy->FindSpriteByName("letter/letter_fairy_select.png"))->setOpacity(0);
    
    todaycandy_bg = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0,0,winSize.width,winSize.height));
    todaycandy_bg->setAnchorPoint(ccp(0,0));
    todaycandy_bg->setPosition(ccp(0,0));
    todaycandy_bg->setColor(ccc3(0,0,0));
    todaycandy_bg->setOpacity(160);
    candy->addChild(todaycandy_bg, 80);
    /*
    CCParticleSystemQuad* m_emitter = CCParticleSystemQuad::create("particles/todaycandy_bg.plist");
    m_emitter->setAnchorPoint(ccp(0.5, 0.5));
    m_emitter->setPosition(ccp(188+357, 528+357));
    m_emitter->setScale(4.0f);
    candy->addChild(m_emitter, 91); // 마법진 바로 위
    //m_emitter->setAutoRemoveOnFinish(true);
    */
    
    CCParticleSystemQuad* star = CCParticleSystemQuad::create("particles/todaycandy_star.plist");
    star->setAnchorPoint(ccp(0.5, 0.5));
    star->setPosition(ccp(188+357, 528+357));
    candy->addChild(star, 1001); //91); // 마법진 바로 위
    star->runAction(CCSequence::create( CCMoveTo::create(0.25f, po[popo]), CCCallFuncND::create(this, callfuncND_selector(CocoRoom::Callback), (void*)selectedKakaoId), NULL) );
}

void CocoRoom::Callback(CCNode* sender, void* data)
{
    // 0 2 4 1 3
    popo++;
    if (popo > 15 && todayCandyKakaoId[candyIdx[(popo-1)%5]] == (int)data)
    {
        CCLog("popo = %d", popo);
        ((CCParticleSystemQuad*)sender)->setDuration(0.05f);
        ((CCParticleSystemQuad*)sender)->setAutoRemoveOnFinish(true);
        
        TodayCandy_Result((int)data);
        return;
    }
        
    float time;
    if (popo <= 10)
        time = 0.3f;
    else
        time = 0.3f + (float)(popo-10)*0.1f;
    
    CCActionInterval* action = CCSequence::create( CCMoveTo::create(time, po[popo%5]), CCCallFuncND::create(this, callfuncND_selector(CocoRoom::Callback), data), NULL);
    sender->runAction(action);
}

void CocoRoom::TodayCandy_Result(int selectedKakaoId)
{
    CCParticleSystemQuad* star = CCParticleSystemQuad::create("particles/todaycandy_done.plist");
    star->setAnchorPoint(ccp(0.5, 0.5));
    star->setPosition(po[(popo-1)%5]);
    star->setScale(3.0f);
    ((CocoRoom*)Depth::GetCurPointer())->candy->addChild(star, 92);
    
    CCActionInterval* action = CCSequence::create( CCDelayTime::create(1.0f), CCCallFuncND::create(this, callfuncND_selector(CocoRoom::Callback2), (void*)selectedKakaoId), NULL);
    star->runAction(action);
}
void CocoRoom::Callback2(CCNode* sender, void* kakaoId)
{
    ((CCParticleSystemQuad*)sender)->setDuration(0.01f);
    ((CCParticleSystemQuad*)sender)->setAutoRemoveOnFinish(true);
    
    isTodayCandyWorking = false;
    todaycandy_bg->removeFromParentAndCleanup(true);
    
    // kakaoID랑 친구리스트의 idx를 넘겨준다.
    int idx;
    for (int i = 0 ; i < friendList.size() ; i++)
    {
        if (friendList[i]->GetKakaoId() == (int)kakaoId)
            idx = i;
    }
    std::vector<int> data;
    data.push_back((int)kakaoId);
    data.push_back(idx);
    
    if ((int)kakaoId == myInfo->GetKakaoId())
        Common::ShowPopup(Depth::GetCurPointer(), "CocoRoom", "NoImage", false, TODAYCANDY_RESULT_WIN, BTN_1, data);
    else
        Common::ShowPopup(Depth::GetCurPointer(), "CocoRoom", "NoImage", false, TODAYCANDY_RESULT_LOSE, BTN_1, data);
}

bool CocoRoom::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    if (isTouched || isTodayCandyWorking)
        return false;
    isTouched = true;
    isScrolling = false;
    isSlotTouched = false;
    
    CCPoint point = pTouch->getLocation();
    CCLog("%d %d", (int)point.x, (int)point.y);
    
    // 슬롯 터치 시작
    if (curState != 3 && scrollView->boundingBox().containsPoint(point))
        isSlotTouched = true;

    // 공통 sprite 관련
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        if (spriteClass->spriteObj[i]->name.substr(0, 35) == "background/bg_sketchbook_select.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                int number = atoi(spriteClass->spriteObj[i]->name.substr(35, 36).c_str());
                sound->playClickboard();
                MakeSprites(number);
                return true;
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
            if (curState == 1 && spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                // 슬롯 구매
                sound->playClickboard();
                std::vector<int> data;
                if ((int)myInfo->GetSlot().size() >= (int)skillSlotInfo.size())
                    Common::ShowPopup(this, "CocoRoom", "NoImage", false, BUY_SKILLSLOT_FULL, BTN_1, data);
                else
                {
                    data.push_back((int)myInfo->GetSlot().size()+1);
                    data.push_back(SkillSlotInfo::GetCost((int)myInfo->GetSlot().size()+1));
                    if (SkillSlotInfo::GetCostType((int)myInfo->GetSlot().size()+1) == 1)
                        Common::ShowPopup(this, "CocoRoom", "NoImage", false, BUY_SKILLSLOT_BY_STARCANDY_TRY, BTN_2, data, 1);
                    else
                        Common::ShowPopup(this, "CocoRoom", "NoImage", false, BUY_SKILLSLOT_BY_TOPAZ_TRY, BTN_2, data, 1);
                }
                return true;
            }
            else if (curState == 2 && spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClickboard();
                Common::ShowNextScene(this, "CocoRoom", "CocoRoomFairyTown", false); // 요정의 마을
                return true;
            }
            else if (curState == 3 && spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClickboard();
                Common::ShowNextScene(this, "CocoRoom", "CocoRoomTodayCandy", false); // 오.별 친구고르기
                return true;
            }
        }
        else if (spriteClass->spriteObj[i]->name == "background/bg_topinfo.png1")
        {
            if (spriteClass->spriteObj[i]->sprite9->boundingBox().containsPoint(point))
            {
                sound->playClickboard();
                Common::ShowNextScene(this, "Ranking", "BuyTopaz", false, 0);
                break;
            }
        }
        else if (spriteClass->spriteObj[i]->name == "background/bg_topinfo.png2")
        {
            if (spriteClass->spriteObj[i]->sprite9->boundingBox().containsPoint(point))
            {
                sound->playClickboard();
                Common::ShowNextScene(this, "Ranking", "BuyStarCandy", false, 0);
                break;
            }
        }
    }
    
    // 코코 관련
    for (int i = 0 ; i < spriteClassCoco->spriteObj.size() ; i++)
    {
        if (spriteClassCoco->spriteObj[i]->name == "button/btn_green.png1")
        {
            if (spriteClassCoco->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                if (myInfo->GetStaffLv() < magicStaffBuildupInfo[magicStaffBuildupInfo.size()-1]->GetLevel())
                {
                    sound->playClick();
                    std::vector<int> data;
                    data.push_back(0);
                    data.push_back(magicStaffBuildupInfo[myInfo->GetStaffLv()-1+1]->GetCost_StarCandy());
                    Common::ShowPopup(this, "CocoRoom", "NoImage", false, UPGRADE_STAFF_BY_STARCANDY_TRY, BTN_2, data);
                    return true;
                }
            }
        }
        else if (spriteClassCoco->spriteObj[i]->name == "button/btn_green.png2")
        {
            if (spriteClassCoco->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                if (myInfo->GetStaffLv() < magicStaffBuildupInfo[magicStaffBuildupInfo.size()-1]->GetLevel())
                {
                    sound->playClick();
                    std::vector<int> data;
                    data.push_back(0);
                    data.push_back(magicStaffBuildupInfo[myInfo->GetStaffLv()-1+1]->GetCost_Topaz());
                    Common::ShowPopup(this, "CocoRoom", "NoImage", false, UPGRADE_STAFF_BY_TOPAZ_TRY, BTN_2, data);
                    return true;
                }
            }
        }
    }
    
    // 요정 관련
    for (int i = 0 ; i < spriteClassFairy->spriteObj.size() ; i++)
    {
        if (spriteClassFairy->spriteObj[i]->name == "button/btn_green.png1")
        {
            if (spriteClassFairy->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                if (myInfo->GetActiveFairyLevel() < FairyBuildUpInfo::GetMaxLevel(myInfo->GetActiveFairyId()))
                {
                    
                    if (myInfo->GetActiveFairyId() <= 0)
                        continue;
                
                    sound->playClick();
                    std::vector<int> data;
                    data.push_back(0);
                    data.push_back(FairyBuildUpInfo::GetCostStarCandy(myInfo->GetActiveFairyId(), myInfo->GetActiveFairyLevel()+1));
                    Common::ShowPopup(this, "CocoRoom", "NoImage", false, UPGRADE_FAIRY_BY_STARCANDY_TRY, BTN_2, data);
                    return true;
                }
            }
        }
        else if (spriteClassFairy->spriteObj[i]->name == "button/btn_green.png2")
        {
            if (spriteClassFairy->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                if (myInfo->GetActiveFairyLevel() < FairyBuildUpInfo::GetMaxLevel(myInfo->GetActiveFairyId()))
                {
                    if (myInfo->GetActiveFairyId() <= 0)
                        continue;
                
                    sound->playClick();
                    std::vector<int> data;
                    data.push_back(0);
                    data.push_back(FairyBuildUpInfo::GetCostTopaz(myInfo->GetActiveFairyId(), myInfo->GetActiveFairyLevel()+1));
                    Common::ShowPopup(this, "CocoRoom", "NoImage", false, UPGRADE_FAIRY_BY_TOPAZ_TRY, BTN_2, data);
                    return true;
                }
            }
        }
    }
    
    // 오늘의 별사탕 관련
    for (int i = 0 ; i < spriteClassCandy->spriteObj.size() ; i++)
    {
        if (spriteClassCandy->spriteObj[i]->name == "button/btn_red_mini.png")
        {
            if (spriteClassCandy->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                
                // 친구가 4명보다 적을 경우 띄운다.
                if (friendList.size() < 4+1)
                {
                    std::vector<int> nullData;
                    Common::ShowPopup(this, "CocoRoom", "NoImage", false, TODAYCANDY_NOTENOUGH_FRIENDS, BTN_1, nullData);
                    return true;
                }
                
                bool flag = true;
                for (int i = 0 ; i < todayCandyKakaoId.size() ; i++)
                {
                    if (todayCandyKakaoId[i] == -1)
                        flag = false;
                }
                
                if (flag)
                {
                    if (myInfo->IsTodayCandyUsed())
                    {
                        // 오늘 오.별을 이미 1번 행한 경우
                        std::vector<int> nullData;
                        Common::ShowPopup(this, "CocoRoom", "NoImage", false, TODAYCANDY_ALREADY_DONE, BTN_1, nullData);
                    }
                    else
                    {
                        // 오늘의 별사탕 결과 받기
                        char temp[150];
                        //http://14.63.225.203/cogma/game/today_starcandy.php?kakao_id=1000&friend_kakao_id_1=1001&friend_kakao_id_2=1002&friend_kakao_id_3=1003&friend_kakao_id_4=1004
                        std::string url = "http://14.63.225.203/cogma/game/today_starcandy.php?";
                        sprintf(temp, "kakao_id=%d&", myInfo->GetKakaoId());
                        url += temp;
                        for (int i = 1 ; i < 5 ; i++)
                        {
                            sprintf(temp, "friend_kakao_id_%d=%d&", i, todayCandyKakaoId[i]);
                            url += temp;
                        }
                        url = url.substr(0, url.size()-1);
                        CCLog("url = %s", url.c_str());
                        
                        CCHttpRequest* req = new CCHttpRequest();
                        req->setUrl(url.c_str());
                        req->setRequestType(CCHttpRequest::kHttpPost);
                        req->setResponseCallback(this, httpresponse_selector(CocoRoom::onHttpRequestCompleted));
                        CCHttpClient::getInstance()->send(req);
                        req->release();
                    }
                }
                else
                    Common::ShowNextScene(this, "CocoRoom", "CocoRoomTodayCandy", false); // 오.별 친구고르기
                
                return true;
            }
        }
    }
    
    return true;
}


void CocoRoom::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
}

void CocoRoom::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    if (!isTouched || isTodayCandyWorking)
        return;
    
    CCPoint point = pTouch->getLocation();
    CCPoint p;
    
    if (curState == 1)
    {
        if (isSlotTouched && !isScrolling && scrollView->boundingBox().containsPoint(point))
        {
            sound->playBoardMove(); // 이 scene만 사운드가 다르다.
            Common::ShowNextScene(this, "CocoRoom", "MagicList", false, 1);
        }
    }
    else if (curState == 2)
    {
        for (int i = 0 ; i < spriteClassFairySlot->spriteObj.size() ; i++)
        {
            if (spriteClassFairySlot->spriteObj[i]->sprite == NULL)
                continue;
            
            p = spriteClassFairySlot->spriteObj[i]->sprite->convertToNodeSpace(point);
            CCSize size = spriteClassFairySlot->spriteObj[i]->sprite->getContentSize();
            if (isSlotTouched && !isScrolling &&
                (int)p.x >= 0 && (int)p.y >= 0 && (int)p.x <= size.width && (int)p.y <= size.height)
            {
                sound->playClick();
                // 요정 변경
                int tag = spriteClassFairySlot->spriteObj[i]->sprite->getTag();
                if (tag != myInfo->GetActiveFairyUserId())
                {
                    char temp[150];
                    std::string url = "http://14.63.225.203/cogma/game/using_fairy.php?";
                    sprintf(temp, "kakao_id=%d&", myInfo->GetKakaoId());
                    url += temp;
                    sprintf(temp, "user_fairy_id=%d", tag);
                    url += temp;
                    CCLog("url = %s", url.c_str());
                    
                    CCHttpRequest* req = new CCHttpRequest();
                    req->setUrl(url.c_str());
                    req->setRequestType(CCHttpRequest::kHttpPost);
                    req->setResponseCallback(this, httpresponse_selector(CocoRoom::onHttpRequestCompleted));
                    CCHttpClient::getInstance()->send(req);
                    req->release();
                    break;
                }
            }
        }
    }
    
    isTouched = false;
    isSlotTouched = false;
}


void CocoRoom::scrollViewDidScroll(CCScrollView* view)
{
    isScrolling = true;
}

void CocoRoom::scrollViewDidZoom(CCScrollView* view)
{
}


void CocoRoom::EndScene()
{
    sound->playClick();
    
    // remove this notification
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, Depth::GetCurName());
    // release depth tree
    Depth::RemoveCurDepth();
    
    // touch 넘겨주기 (GetCurName = 위에서 remove를 했기 때문에 결국 여기 입장에서는 부모다)
    CCString* param = CCString::create("0");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetCurName(), param);

    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);

    // remove all objects
    spriteClass->RemoveAllObjects();
    delete spriteClass;
    spriteClassCoco->RemoveAllObjects();
    delete spriteClassCoco;
    spriteClassFairy->RemoveAllObjects();
    delete spriteClassFairy;
    spriteClassCocoSlot->RemoveAllObjects();
    delete spriteClassCocoSlot;
    spriteClassFairySlot->RemoveAllObjects();
    delete spriteClassFairySlot;
    spriteClassCandy->RemoveAllObjects();
    delete spriteClassCandy;
    spriteClassCandyList->RemoveAllObjects();
    delete spriteClassCandyList;
    
    scrollView->getContainer()->removeAllChildren();
    scrollView->removeAllChildren();
    scrollView->removeFromParentAndCleanup(true);
    
    if (par != NULL)
        par->removeFromParentAndCleanup(true);
    
    coco->removeAllChildren();
    coco->removeFromParentAndCleanup(true);
    fairy->removeAllChildren();
    fairy->removeFromParentAndCleanup(true);
    candy->removeAllChildren();
    candy->removeFromParentAndCleanup(true);
    
    this->removeFromParentAndCleanup(true);
}

void CocoRoom::EndSceneCallback()
{
}


void CocoRoom::onHttpRequestCompleted(CCNode *sender, void *data)
{
    CCHttpResponse* res = (CCHttpResponse*) data;
    CCLog("on http request");
    
    if (!res || !res->isSucceed())
    {
        CCLog("res failed. error buffer: %s", res->getErrorBuffer());
        return;
    }
    
    // dump data
    std::vector<char> *buffer = res->getResponseData();
    char dumpData[BUFFER_SIZE];
    for (unsigned int i = 0 ; i < buffer->size() ; i++)
        dumpData[i] = (*buffer)[i];
    dumpData[buffer->size()] = NULL;
    
    if (curState == 2)
        XmlParseFairyList(dumpData, (int)buffer->size());
    else if (curState == 3)
        XmlParseTodayCandy(dumpData, (int)buffer->size());
}

void CocoRoom::XmlParseFairyList(char* data, int size)
{
    // xml parsing
    xml_document xmlDoc;
    xml_parse_result result = xmlDoc.load_buffer(data, size);
    
    if (!result)
    {
        CCLog("error description: %s", result.description());
        CCLog("error offset: %d", result.offset);
        return;
    }
    
    // get data
    xml_node nodeResult = xmlDoc.child("response");
    int code = nodeResult.child("code").text().as_int();
    if (code == 0)
    {
        // init
        myInfo->ClearFairyList();
        
        xml_object_range<xml_named_node_iterator> its = nodeResult.child("fairy-list").children("fairy");
        int cfi, ufi, level, isUse;
        for (xml_named_node_iterator it = its.begin() ; it != its.end() ; ++it)
        {
            for (xml_attribute_iterator ait = it->attributes_begin() ; ait != it->attributes_end() ; ++ait)
            {
                std::string name = ait->name();
                if (name == "common-fairy-id") cfi = ait->as_int();
                else if (name == "user-fairy-id") ufi = ait->as_int();
                else if (name == "level") level = ait->as_int();
                else if (name == "is-use") isUse = ait->as_int();
            }
            myInfo->AddFairy(cfi, ufi, level, isUse);
        }
        
        // 정보 갱신 (게임준비, 코코방_요정, 친구리스트의 내정보)
        CCString* param = CCString::create("4");
        CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetParentName(), param);
        Friend::ChangeMyFairyInfo();
        MakeSprites(-curState);
    }
    else
    {
        CCLog("XmlParseFairyList : failed code = %d", code);
    }
}

void CocoRoom::XmlParseTodayCandy(char* data, int size)
{
    // xml parsing
    xml_document xmlDoc;
    xml_parse_result result = xmlDoc.load_buffer(data, size);
    
    if (!result)
    {
        CCLog("error description: %s", result.description());
        CCLog("error offset: %d", result.offset);
        return;
    }
    
    // get data
    xml_node nodeResult = xmlDoc.child("response");
    int code = nodeResult.child("code").text().as_int();
    if (code == 0)
    {
        // 오늘의 별사탕 액션 시작
        int selectedKakaoId = nodeResult.child("today-starcandy").attribute("selected-user").as_int();
        myInfo->SetTodayCandy(1);
        DecideUser(selectedKakaoId);
    }
    else
    {
        CCLog("XmlParseTodayCandy : failed code = %d", code);
        std::vector<int> nullData;
        Common::ShowPopup(this, "CocoRoom", "NoImage", false, TODAYCANDY_ALREADY_DONE, BTN_1, nullData);
        // code = 10 : 오늘 이미 오.별 이용했음. (팝업창 띄우기)
    }
}
