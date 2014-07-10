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
    CCLog("CocoRoom : onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority(), true);
    CCLayer::onEnter();
    
    // 전체화면 액션
    CCActionInterval* action = CCSequence::create( CCSpawn::create(CCMoveTo::create(0.1f, ccp(0, 0)), CCScaleTo::create(0.1f, 1.0f), NULL), CCCallFunc::create(this, callfunc_selector(CocoRoom::SceneCallback)), NULL );
    tLayer->runAction(CCEaseExponentialOut::create(action));
}
void CocoRoom::SceneCallback()
{
    isTouched = false;
    isSlotTouched = false;
    isScrolling = false;
    isTodayCandyWorking = false;
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
    if (isKeybackTouched || isTouched)
        return;
    isKeybackTouched = true;
    
    sound->playClick();
    EndScene();
}


int offset = -110;
int offset2 = -70;

bool CocoRoom::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
    isTouched = true;
    isSlotTouched = true;
    isScrolling = true;
    isTodayCandyWorking = true;

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
    
    tLayer = CCLayer::create();
    tLayer->setAnchorPoint(ccp(0, 0));
    tLayer->setPosition(ccp(winSize.width/2, winSize.height/2));
    tLayer->setScale(0);
    this->addChild(tLayer, 1);

    // scrollView init.
    scrollView = CCScrollView::create();
    scrollView->setDirection(kCCScrollViewDirectionHorizontal);
    scrollView->setViewSize(CCSizeMake(782-40, 177-20));
    scrollView->setAnchorPoint(ccp(0, 0));
    scrollView->setPosition(ccp(77+20, 228+12));
    scrollView->setDelegate(this);
    scrollView->setTouchPriority(Depth::GetCurPriority());
    tLayer->addChild(scrollView, 11);

    coco = CCLayer::create();
    fairy = CCLayer::create();
    candy = CCLayer::create();
    coco->setPosition(ccp(0, 0));
    fairy->setPosition(ccp(0, 0));
    candy->setPosition(ccp(0, 0));
    tLayer->addChild(coco, 5);
    tLayer->addChild(fairy, 5);
    tLayer->addChild(candy, 5);
    
    spriteClass = new SpriteClass();
    spriteClassCoco = new SpriteClass();
    spriteClassFairy = new SpriteClass();
    spriteClassCandy = new SpriteClass();
    spriteClassFairySlot = new SpriteClass();
    spriteClassCocoSlot = new SpriteClass();
    spriteClassCandyList = new SpriteClass();
    
    par = NULL;
    floor = NULL;
    stencil = CCDrawNode::create();
    CCPoint ver[] = { ccp(83, 487), ccp(83, 487+904+10+offset+10), ccp(83+939, 487+904+10+offset+10), ccp(83+939, 487) };
    stencil->drawPolygon(ver, 4, ccc4f(0,0,0,255), 0, ccc4f(0,0,0,255));
    clip = CCClippingNode::create(stencil);
    tLayer->addChild(clip, 2);
    
    idx = -1;
    InitSprites();
    curState = -1;
    MakeSprites(tabNumber);
    
    return true;
}

void CocoRoom::Notification(CCObject* obj)
{
    CCString* param = (CCString*)obj;
    
    if (param->intValue() == -1)
    {
        // 터치 활성
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority()+1, true);
        this->setTouchPriority(Depth::GetCurPriority());
        isTouched = false;
        scrollView->setTouchEnabled(true);
        CCLog("CocoRoom : 터치 활성 (Priority = %d)", this->getTouchPriority());
    }
    else if (param->intValue() == 0)
    {
        // 터치 활성
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority()+1, true);
        this->setTouchPriority(Depth::GetCurPriority());
        isTouched = false;
        isKeybackTouched = false;
        scrollView->setTouchEnabled(true);
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
        isKeybackTouched = true;
        CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
        
        scrollView->setTouchEnabled(false);
    }
    else if (param->intValue() == 2)
    {
        // 토파즈, 별사탕, MP 정보 업데이트
        ((CCLabelTTF*)spriteClass->FindLabelByTag(1))->setString(Common::MakeComma(myInfo->GetTopaz()).c_str());
        ((CCLabelTTF*)spriteClass->FindLabelByTag(2))->setString(Common::MakeComma(myInfo->GetStarCandy()).c_str());
        ((CCLabelTTF*)spriteClass->FindLabelByTag(3))->setString(Common::MakeComma(myInfo->GetMPTotal()).c_str());
    }
    else if (param->intValue() == 7)
    {
        // 요정을 처음 구입했을 경우 -> 그 요정을 바로 자동 착용 -> 정보 갱신해야 함.
        MakeSprites(-curState);
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
    else if (param->intValue() == 10)
    {
        // 터치 풀기 (백그라운드에서 돌아올 때)
        isTouched = false;
        if (idx > -1)
        {
            if (curState == 1) // 코코의방
            {
                spriteClassCoco->spriteObj[idx]->sprite->setColor(ccc3(255,255,255));
                ((CCSprite*)spriteClassCoco->FindSpriteByName("letter/letter_staff.png1"))->setColor(ccc3(255,255,255));
                ((CCSprite*)spriteClassCoco->FindSpriteByName("letter/letter_buildup_normal.png"))->setColor(ccc3(255,255,255));
                ((CCSprite*)spriteClassCoco->FindSpriteByName("icon/icon_lock_white.png1"))->setColor(ccc3(255,255,255));
                ((CCSprite*)spriteClassCoco->FindSpriteByName("button/btn_receive_starcandy.png"))->setColor(ccc3(255,255,255));
                ((CCSprite*)spriteClassCoco->FindSpriteByName("letter/letter_staff.png2"))->setColor(ccc3(255,255,255));
                ((CCSprite*)spriteClassCoco->FindSpriteByName("letter/letter_buildup_fine.png"))->setColor(ccc3(255,255,255));
                ((CCSprite*)spriteClassCoco->FindSpriteByName("icon/icon_lock_white.png2"))->setColor(ccc3(255,255,255));
                ((CCSprite*)spriteClassCoco->FindSpriteByName("button/btn_receive_topaz.png"))->setColor(ccc3(255,255,255));
            }
            else if (curState == 2) // 요정
            {
                spriteClassFairy->spriteObj[idx]->sprite->setColor(ccc3(255,255,255));
                ((CCSprite*)spriteClassFairy->FindSpriteByName("letter/letter_normal.png"))->setColor(ccc3(255,255,255));
                ((CCSprite*)spriteClassFairy->FindSpriteByName("letter/letter_tonic.png1"))->setColor(ccc3(255,255,255));
                ((CCSprite*)spriteClassFairy->FindSpriteByName("icon/icon_lock_white.png1"))->setColor(ccc3(255,255,255));
                ((CCSprite*)spriteClassFairy->FindSpriteByName("button/btn_receive_starcandy.png"))->setColor(ccc3(255,255,255));
                ((CCSprite*)spriteClassFairy->FindSpriteByName("letter/letter_fine.png"))->setColor(ccc3(255,255,255));
                ((CCSprite*)spriteClassFairy->FindSpriteByName("letter/letter_tonic.png2"))->setColor(ccc3(255,255,255));
                ((CCSprite*)spriteClassFairy->FindSpriteByName("icon/icon_lock_white.png2"))->setColor(ccc3(255,255,255));
                ((CCSprite*)spriteClassFairy->FindSpriteByName("button/btn_receive_topaz.png"))->setColor(ccc3(255,255,255));
            }
            else if (curState == 3) // 오.별
            {
                spriteClassCandy->spriteObj[idx]->sprite->setColor(ccc3(255,255,255));
                ((CCSprite*)spriteClassCandy->FindSpriteByName("letter/letter_fairy_select.png"))->setColor(ccc3(255,255,255));
            }
        }
    }
}

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
            ccp(0, 0), ccp(14, 1586+offset), CCSize(0, 0), "", "Layer", tLayer, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_x_yellow.png",
            ccp(0, 0), ccp(875, 1634+offset), CCSize(0, 0), "", "Layer", tLayer, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_title_cocoroom.png",
            ccp(0, 0), ccp(409, 1632+offset), CCSize(0, 0), "", "Layer", tLayer, 2) );
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_sketchbook_select.png1",
            ccp(0, 0), ccp(101, 1417+offset), CCSize(0, 0), "", "Layer", tLayer, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_sketchbook_select.png2",
            ccp(0, 0), ccp(297, 1417+offset), CCSize(0, 0), "", "Layer", tLayer, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_sketchbook_select.png3",
            ccp(0, 0), ccp(495, 1417+offset), CCSize(0, 0), "", "Layer", tLayer, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_menu_coco.png",
            ccp(0, 0), ccp(101+40, 1447+40+offset), CCSize(0, 0), "", "Layer", tLayer, 3) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_menu_fairy.png",
            ccp(0, 0), ccp(297+55, 1447+10+offset), CCSize(0, 0), "", "Layer", tLayer, 3) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_menu_starcandy.png",
            ccp(0, 0), ccp(495+50, 1447+23+offset), CCSize(0, 0), "", "Layer", tLayer, 3) );
    
    // 오늘의 별사탕 띠
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_today_starcandy.png",
                    ccp(0, 0), ccp(630, 1494+offset), CCSize(0, 0), "", "Layer", tLayer, 5) );
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png1",
            ccp(0, 0), ccp(49, 458), CCSize(982, 954+20+offset), "", "Layer", tLayer, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png1",
            ccp(0, 0), ccp(75-5, 492-5), CCSize(929+10, 904+10+offset+10), "", "Layer", tLayer, 1) );
    
    // slot part
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png2",
            ccp(0, 0), ccp(49, 198), CCSize(982, 223), "", "Layer", tLayer, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png2",
            ccp(0, 0), ccp(77, 228), CCSize(782, 177), "", "Layer", tLayer, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_plus_big.png", // slot plus
            ccp(0, 0), ccp(896, 312), CCSize(0, 0), "", "Layer", tLayer, 1) );
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
}

void CocoRoom::SetMenuChange(int state)
{
    ((CCSprite*)spriteClass->FindSpriteByName("background/bg_sketchbook_select.png1"))->setColor(ccc3(170,170,170));
    ((CCSprite*)spriteClass->FindSpriteByName("background/bg_sketchbook_select.png1"))->setZOrder(0);
    ((CCSprite*)spriteClass->FindSpriteByName("icon/icon_menu_coco.png"))->setColor(ccc3(170,170,170));
    ((CCSprite*)spriteClass->FindSpriteByName("background/bg_sketchbook_select.png2"))->setColor(ccc3(170,170,170));
    ((CCSprite*)spriteClass->FindSpriteByName("background/bg_sketchbook_select.png2"))->setZOrder(0);
    ((CCSprite*)spriteClass->FindSpriteByName("icon/icon_menu_fairy.png"))->setColor(ccc3(170,170,170));
    ((CCSprite*)spriteClass->FindSpriteByName("background/bg_sketchbook_select.png3"))->setColor(ccc3(170,170,170));
    ((CCSprite*)spriteClass->FindSpriteByName("background/bg_sketchbook_select.png3"))->setZOrder(0);
    ((CCSprite*)spriteClass->FindSpriteByName("icon/icon_menu_starcandy.png"))->setColor(ccc3(170,170,170));
    ((CCSprite*)spriteClass->FindSpriteByName("icon/icon_today_starcandy.png"))->setColor(ccc3(170,170,170));
    
    char name[38];
    sprintf(name, "background/bg_sketchbook_select.png%d", state);
    ((CCSprite*)spriteClass->FindSpriteByName(name))->setColor(ccc3(255,255,255));
    ((CCSprite*)spriteClass->FindSpriteByName(name))->setZOrder(2);
    if (state == 1) ((CCSprite*)spriteClass->FindSpriteByName("icon/icon_menu_coco.png"))->setColor(ccc3(255,255,255));
    else if (state == 2) ((CCSprite*)spriteClass->FindSpriteByName("icon/icon_menu_fairy.png"))->setColor(ccc3(255,255,255));
    else if (state == 3)
    {
        ((CCSprite*)spriteClass->FindSpriteByName("icon/icon_menu_starcandy.png"))->setColor(ccc3(255,255,255));
        ((CCSprite*)spriteClass->FindSpriteByName("icon/icon_today_starcandy.png"))->setColor(ccc3(255,255,255));
    }
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
    
    clip->removeAllChildren();
    
    if (par != NULL)
        par->setVisible(false);
    
    SetMenuChange(abs(state));
    
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
    spriteClassCoco->spriteObj.push_back( SpriteObject::Create(1, "background/bg_gameready_name.png", ccp(0, 0), ccp(867, 237), CCSize(136, 63), "", "Layer", tLayer, 1) );
    // 구입 개수
    char val[6];
    sprintf(val, "%d", (int)myInfo->GetSlot().size());
    spriteClassCoco->spriteObj.push_back( SpriteObject::CreateLabel(val, fontList[0], 48, ccp(0, 0), ccp(892, 245), ccc3(255,219,53), "", "Layer", tLayer, 5, 0, 255, 100) ); // 현재 개수
    sprintf(val, "/ %d", (int)skillSlotInfo.size());
    spriteClassCoco->spriteObj.push_back( SpriteObject::CreateLabel(val, fontList[0], 36, ccp(0, 0), ccp(927, 245), ccc3(182,142,142), "", "Layer", tLayer, 5) ); // 전체 슬롯 개수
    
    spriteClassCoco->spriteObj.push_back( SpriteObject::Create(1, "background/bg_degree_desc.png",
                        ccp(0, 0), ccp(441, 1290+offset), CCSize(543, 61), "", "Layer", coco, 5) );
    spriteClassCoco->spriteObj.push_back( SpriteObject::CreateLabel("오늘도 곰을 백마리 잡은 마법사", fontList[0], 36, ccp(0.5, 0.5), spriteClassCoco->FindParentCenterPos("background/bg_degree_desc.png"), ccc3(255,255,255), "background/bg_degree_desc.png", "1", NULL, 5, 1) );
    
    // stone floor
    floor = CCSprite::createWithSpriteFrameName("background/stone_floor.png");
    floor->setAnchorPoint(ccp(0,0));
    floor->setPosition(ccp(60, 740+offset));
    clip->addChild(floor, 1);
    
    // coco character
    spriteClassCoco->spriteObj.push_back( SpriteObject::Create(0, "image/coco_room.png", ccp(0.5, 0), ccp(315, 840+offset), CCSize(0, 0), "", "Layer", coco, 5) );
    ((CCSprite*)spriteClassCoco->FindSpriteByName("image/coco_room.png"))->setScale(0.95f);
    
    CCFiniteTimeAction* action = CCSequence::create(CCMoveBy::create(1.0f, ccp(0, +5)), CCMoveBy::create(1.0f, ccp(0, -5)), NULL);
    CCAction* rep = CCRepeatForever::create((CCActionInterval*)action);
    ((CCSprite*)spriteClassCoco->FindSpriteByName("image/coco_room.png"))->runAction(rep);
    
    // 지팡이 particle
    CCSprite* sp = CCSprite::create("particles/effect_10.png");
    par = CCParticleFlower::create();
    par->setTexture(sp->getTexture());
    par->setAnchorPoint(ccp(0.5, 0.5));
    par->setPosition(ccp(474, 1094+offset));
    int size;
    if (myInfo->GetStaffLv() <= 1) // 1
        size = 0;
    else if (myInfo->GetStaffLv() > 1 && myInfo->GetStaffLv() < 5) // 2~4
        size = 35;
    else if (myInfo->GetStaffLv() < 10) // 5~9
        size = 65;
    else if (myInfo->GetStaffLv() < 15) // 10~14
        size = 90;
    else if (myInfo->GetStaffLv() < 20) // 15~19
        size = 130;
    else
        size = 175;
    par->setStartSize(size);
    par->setLife(0.4);
    par->setSpeed(120);
    par->setSpeedVar(70);
    ccColor4F startColor = par->getStartColor();
    startColor.r = 0.95f;
    startColor.g = 0.95f;
    startColor.b = 0.95f;
    par->setStartColor(startColor);
    par->setDuration(-1);
    tLayer->addChild(par, 100);
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
    // 버튼 젤리 움직임 (left)
    CCSprite* temp = ((CCSprite*)spriteClassCoco->FindSpriteByName("button/btn_green.png1"));
    CCSize t = temp->getContentSize();
    temp->setAnchorPoint(ccp(0.5, 0.5));
    temp->setPosition(ccp(temp->getPosition().x+t.width/2, temp->getPosition().y+t.height/2));
    CCActionInterval* action3 = CCSequence::create( CCScaleTo::create(1.0f, 1.02f, 0.97f), CCScaleTo::create(1.0f, 0.98f, 1.03f), NULL );
    temp->runAction(CCRepeatForever::create(action3));
    // 버튼 젤리 움직임 (right)
    temp = ((CCSprite*)spriteClassCoco->FindSpriteByName("button/btn_green.png2"));
    t = temp->getContentSize();
    temp->setAnchorPoint(ccp(0.5, 0.5));
    temp->setPosition(ccp(temp->getPosition().x+t.width/2, temp->getPosition().y+t.height/2));
    temp->runAction(CCRepeatForever::create((CCActionInterval*)action3->copy()));
    

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
    // mp 500 미만의 경우
    if (myInfo->GetMPTotal() < 500)
    {
        ((CCSprite*)spriteClassCoco->FindSpriteByName("button/btn_receive_starcandy.png"))->setOpacity(0);
        ((CCSprite*)spriteClassCoco->FindSpriteByName("button/btn_receive_topaz.png"))->setOpacity(0);
        ((CCLabelTTF*)spriteClassCoco->FindLabelByTag(5))->setOpacity(0);
        ((CCLabelTTF*)spriteClassCoco->FindLabelByTag(6))->setOpacity(0);
        
        ((CCSprite*)spriteClassCoco->FindSpriteByName("icon/icon_lock_white.png1"))->setOpacity(255);
        ((CCSprite*)spriteClassCoco->FindSpriteByName("icon/icon_lock_white.png2"))->setOpacity(255);
        ((CCLabelTTF*)spriteClassCoco->FindLabelByTag(7))->setOpacity(255);
        ((CCLabelTTF*)spriteClassCoco->FindLabelByTag(8))->setOpacity(255);
        ((CCLabelTTF*)spriteClassCoco->FindLabelByTag(7))->setString("강화 준비중");
        ((CCLabelTTF*)spriteClassCoco->FindLabelByTag(8))->setString("강화 준비중");
    }
    
    // 일반적인 경우
    else if (myInfo->GetStaffLv() < magicStaffBuildupInfo[magicStaffBuildupInfo.size()-1]->GetLevel())
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
        ((CCLabelTTF*)spriteClassCoco->FindLabelByTag(7))->setString("강화 완료");
        ((CCLabelTTF*)spriteClassCoco->FindLabelByTag(8))->setString("강화 완료");
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
    spriteClassFairy->spriteObj.push_back( SpriteObject::CreateLabel("요정보기", fontList[0], 36, ccp(0, 0), ccp(871+2, 247-2), ccc3(0,0,0), "", "Layer", tLayer, 5) );
    spriteClassFairy->spriteObj.push_back( SpriteObject::CreateLabel("요정보기", fontList[0], 36, ccp(0, 0), ccp(871, 247), ccc3(255,255,255), "", "Layer", tLayer, 5) );
    
    spriteClassFairy->spriteObj.push_back( SpriteObject::Create(1, "background/bg_cocoroom_desc.png1",
                        ccp(0, 0), ccp(524, 1271+offset), CCSize(439, 89), "", "Layer", fairy, 5) );
    spriteClassFairy->spriteObj.push_back( SpriteObject::Create(1, "background/bg_cocoroom_desc.png2",
                        ccp(0, 0), ccp(524, 1186+offset), CCSize(439, 58), "", "Layer", fairy, 5) );
    spriteClassFairy->spriteObj.push_back( SpriteObject::Create(1, "background/bg_cocoroom_desc.png3",
                        ccp(0, 0), ccp(524, 1124+offset), CCSize(439, 58), "", "Layer", fairy, 5) );
    spriteClassFairy->spriteObj.push_back( SpriteObject::Create(1, "background/bg_cocoroom_desc.png4",
                        ccp(0, 0), ccp(524, 1026+offset), CCSize(439, 58), "", "Layer", fairy, 5) );
    
    // stone floor
    //spriteClassFairy->spriteObj.push_back( SpriteObject::Create(0, "background/fairy_floor.png", ccp(0, 0), ccp(100, 760+offset), CCSize(0, 0), "", "Layer", fairy, 5) );
    floor = CCSprite::createWithSpriteFrameName("background/fairy_floor.png");
    floor->setAnchorPoint(ccp(0,0));
    floor->setPosition(ccp(60, 760+40+offset));
    clip->addChild(floor, 1);
    
    
    int fid = myInfo->GetActiveFairyId();
    int flv = myInfo->GetActiveFairyLevel();
    FairyInfo* f = FairyInfo::GetObj(fid);
    
    // 요정 그림
    CCLayer* picture = Fairy::GetFairy(fid);
    switch (fid)
    {
        case 1: picture->setScale(1.2f); break;
        case 2: picture->setScale(1.2f); break;
        case 3: picture->setScale(1.0f); break;
        default: picture->setScale(2.0f); break;
    }
    picture->setAnchorPoint(ccp(0, 0));
    switch (fid)
    {
        case 1: picture->setPosition(ccp(300, 1186+offset-50)); break;
        case 2: picture->setPosition(ccp(310, 1156+offset-50)); break;
        case 3: picture->setPosition(ccp(310, 1136+offset-50)); break;
        default: picture->setPosition(ccp(310, 1136+offset-50)); break;
    }
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
    if (fid > 0)
        spriteClassFairy->spriteObj.push_back( SpriteObject::CreateLabel("MP + 100", fontList[0], 36, ccp(0, 0), ccp(720, 1196+offset), ccc3(255,255,255), "", "Layer", fairy, 5, 0, 255, 1) );
    else
        spriteClassFairy->spriteObj.push_back( SpriteObject::CreateLabel("없음", fontList[0], 36, ccp(0, 0), ccp(720, 1196+offset), ccc3(255,255,255), "", "Layer", fairy, 5, 0, 255, 1) );
    
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
    
    // 버튼 젤리 움직임 (left)
    CCSprite* temp = ((CCSprite*)spriteClassFairy->FindSpriteByName("button/btn_green.png1"));
    CCSize t = temp->getContentSize();
    temp->setAnchorPoint(ccp(0.5, 0.5));
    temp->setPosition(ccp(temp->getPosition().x+t.width/2, temp->getPosition().y+t.height/2));
    CCActionInterval* action3 = CCSequence::create( CCScaleTo::create(1.0f, 1.02f, 0.97f), CCScaleTo::create(1.0f, 0.98f, 1.03f), NULL );
    temp->runAction(CCRepeatForever::create(action3));
    // 버튼 젤리 움직임 (right)
    temp = ((CCSprite*)spriteClassFairy->FindSpriteByName("button/btn_green.png2"));
    t = temp->getContentSize();
    temp->setAnchorPoint(ccp(0.5, 0.5));
    temp->setPosition(ccp(temp->getPosition().x+t.width/2, temp->getPosition().y+t.height/2));
    temp->runAction(CCRepeatForever::create((CCActionInterval*)action3->copy()));
    
    
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

    SetTodayCandyList(); // 5명의 프로필 사진과 이름을 들고온다.
    
    if (!myInfo->IsTodayCandyUsed())
    {
        // button
        spriteClassCandy->spriteObj.push_back( SpriteObject::Create(0, "button/btn_red_mini.png", ccp(0.5, 0.5), ccp(435+223/2, (842+65+offset2)+105/2), CCSize(0, 0), "", "Layer", candy, 105) );
        //spriteClassCandy->spriteObj.push_back( SpriteObject::Create(1, "button/btn_red_mini.png", ccp(0, 0), ccp(435, 842+65+offset2), CCSize(223*1.20f, 105*1.20f), "", "Layer", candy, 105) );
        ((CCSprite*)spriteClassCandy->FindSpriteByName("button/btn_red_mini.png"))->setScale(1.2f);
        // 223 105
        CCPoint p = spriteClassCandy->FindParentCenterPos("button/btn_red_mini.png");
        spriteClassCandy->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_fairy_select.png", ccp(0.5, 0), ccp(435+p.x, 842+65+offset2+30), CCSize(0, 0), "", "Layer", candy, 105) );
        ((CCSprite*)spriteClassCandy->FindSpriteByName("letter/letter_fairy_select.png"))->setScale(1.1f);
        /*
        // 버튼 젤리 움직임
        CCSprite* temp = ((CCSprite*)spriteClassCandy->FindSpriteByName("button/btn_red_mini.png"));
        CCSize t = temp->getContentSize();
        temp->setAnchorPoint(ccp(0.5, 0.5));
        temp->setPosition(ccp(temp->getPosition().x+t.width/2, temp->getPosition().y+t.height/2));
        CCActionInterval* action3 = CCSequence::create( CCScaleTo::create(1.0f, 1.03f, 0.96f), CCScaleTo::create(1.0f, 0.97f, 1.04f), NULL );
        temp->runAction(CCRepeatForever::create(action3));
         */
    }
    
    // '친구목록' 글자
    spriteClassCandy->spriteObj.push_back( SpriteObject::CreateLabel("친구목록", fontList[0], 36, ccp(0, 0), ccp(870+2, 247-2), ccc3(0,0,0), "", "Layer", candy, 5) );
    spriteClassCandy->spriteObj.push_back( SpriteObject::CreateLabel("친구목록", fontList[0], 36, ccp(0, 0), ccp(870, 247), ccc3(255,255,255), "", "Layer", candy, 5) );
    
    for (int i = 0 ; i < spriteClassCandy->spriteObj.size() ; i++)
        spriteClassCandy->AddChild(i);
}

void CocoRoom::SetTodayCandyList()
{
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
            spriteClassCandyList->spriteObj.push_back( SpriteObject::CreateLabel(friendList[idx]->GetNickname(), fontList[0], 36, ccp(0.5, 1), ccp(pos[i].x, pos[i].y-65-5), ccc3(255,255,255), "", "Layer", candy, 110, 0, 255, i+1) );
            CCSize s0 = ((CCLabelTTF*)spriteClassCandyList->FindLabelByTag(i+1))->getContentSize();
            
            // name의 배경
            spriteClassCandyList->spriteObj.push_back( SpriteObject::Create(1, "background/bg_cocoroom_desc.png1", ccp(0.5, 1), ccp(pos[i].x, pos[i].y-65), CCSize(std::max(150, (int)s0.width+30), 58), "", "Layer", candy, 105) );
        }
        else
        {
            // 빈 프로필 공간
            spriteClassCandyList->spriteObj.push_back( SpriteObject::Create(1, "background/bg_cocoroom_desc.png100", ccp(0.5, 0.5), pos[i], CCSize(120, 120), "", "Layer", candy, 105) );
            
            spriteClassCandyList->spriteObj.push_back( SpriteObject::CreateLabel("?", fontList[0], 36, ccp(0.5, 1), ccp(pos[i].x, pos[i].y-65-5), ccc3(255,255,255), "", "Layer", candy, 110, 0, 255, i+1) );
            CCSize s0 = ((CCLabelTTF*)spriteClassCandyList->FindLabelByTag(i+1))->getContentSize();
            
            // name의 배경
            spriteClassCandyList->spriteObj.push_back( SpriteObject::Create(1, "background/bg_cocoroom_desc.png1", ccp(0.5, 1), ccp(pos[i].x, pos[i].y-65), CCSize(std::max(150, (int)s0.width+30), 58), "", "Layer", candy, 105) );
        }
    }
    
    
    // 보상 설명 부분
    if (myInfo->IsTodayCandyUsed())
    {
        spriteClassCandyList->spriteObj.push_back( SpriteObject::CreateLabelArea("오늘의 별사탕을 이미 실행하였습니다. 매일 오전 3시에 초기화됩니다.", fontList[0], 36, ccp(0.5, 0.5), ccp(77+782/2, 228+177/2), ccc3(78,47,8), CCSize(592, 147), kCCTextAlignmentCenter, kCCVerticalTextAlignmentCenter, "", "Layer", candy, 5) );
    }
    else
    {
        int h = 228 + 177/2;
        //int dh = 20;
        //spriteClassCandyList->spriteObj.push_back( SpriteObject::CreateLabel("오늘의 보상 :", fontList[0], 36, ccp(0, 0.5), ccp(110, 330-15), ccc3(78,47,8), "", "Layer", candy, 5) );
        spriteClassCandyList->spriteObj.push_back( SpriteObject::CreateLabel("오늘의 보상 :", fontList[0], 36, ccp(0, 0.5), ccp(110, h), ccc3(78,47,8), "", "Layer", candy, 5) );
        
        CCSize s;
        
        char sname[40], sname2[40];
        if (myInfo->GetTodayCandyType() == 1)
        {
            sprintf(sname, "icon/icon_starcandy.png1");
            sprintf(sname2, "icon/icon_starcandy.png2");
        }
        else if (myInfo->GetTodayCandyType() == 2)
        {
            sprintf(sname, "icon/icon_topaz.png1");
            sprintf(sname2, "icon/icon_topaz.png2");
        }
        else if (myInfo->GetTodayCandyType() == 3)
        {
            sprintf(sname, "icon/icon_potion.png1");
            sprintf(sname2, "icon/icon_potion.png2");
        }
        spriteClassCandyList->spriteObj.push_back( SpriteObject::Create(0, sname, ccp(0, 0.5), ccp(340, h), CCSize(0, 0), "", "Layer", candy, 5) );
        ((CCSprite*)spriteClassCandyList->FindSpriteByName(sname))->setScale(1.15f);
        s = ((CCSprite*)spriteClassCandyList->FindSpriteByName(sname))->getContentSize();
        s.width *= 1.150f;
        s.height *= 1.150f;
        
        
        char name[40];
        sprintf(name, "%d개", myInfo->GetTodayCandyValueChoice());
        spriteClassCandyList->spriteObj.push_back( SpriteObject::CreateLabel(name, fontList[0], 40, ccp(0, 0.5), ccp(340+s.width+10, h), ccc3(78,47,8), "", "Layer", candy, 5, 0, 255, 123456) );
        CCSize s2 = ((CCLabelTTF*)spriteClassCandyList->FindLabelByTag(123456))->getContentSize();
        int curPosX = 350+s.width+10;
        
        spriteClassCandyList->spriteObj.push_back( SpriteObject::CreateLabel("<참가상>", fontList[0], 30, ccp(0, 0.5), ccp(curPosX+s2.width+30, h+40), ccc3(78,47,8), "", "Layer", candy, 5, 0, 255, 123457) );
        //curPosX = curPosX+s2.width+10;
        //s2 = ((CCLabelTTF*)spriteClassCandyList->FindLabelByTag(123457))->getContentSize();
        
        spriteClassCandyList->spriteObj.push_back( SpriteObject::CreateLabel("(", fontList[0], 32, ccp(0, 0.5), ccp(curPosX+s2.width+20, h-20), ccc3(78,47,8), "", "Layer", candy, 5) );
        
        spriteClassCandyList->spriteObj.push_back( SpriteObject::Create(0, sname2, ccp(0, 0.5), ccp(curPosX+s2.width+40, h-20), CCSize(0, 0), "", "Layer", candy, 5) );
        ((CCSprite*)spriteClassCandyList->FindSpriteByName(sname2))->setScale(0.7f);
        curPosX = curPosX+s2.width+40;
        s2 = ((CCSprite*)spriteClassCandyList->FindSpriteByName(sname2))->getContentSize();
        s2.width *= 0.70f;
        s2.height *= 0.70f;
        
        sprintf(name, "%d개 )", myInfo->GetTodayCandyValueMiss());
        spriteClassCandyList->spriteObj.push_back( SpriteObject::CreateLabel(name, fontList[0], 32, ccp(0, 0.5), ccp(curPosX+s2.width+10, h-20), ccc3(78,47,8), "", "Layer", candy, 5) );
        
        //sprintf(name, "x %d  (참가상 = x %d)", myInfo->GetTodayCandyValueChoice(), myInfo->GetTodayCandyValueMiss());
        //spriteClassCandyList->spriteObj.push_back( SpriteObject::CreateLabel(name, fontList[0], 36, ccp(0, 0), ccp(440, 320), ccc3(78,47,8), "", "Layer", candy, 5) );
        
        /*
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
        
        spriteClassCandyList->spriteObj.push_back( SpriteObject::CreateLabel("친구와 함께 보상을 받으세요!", fontList[0], 36, ccp(0, 0), ccp(240+2, 250-2), ccc3(0,0,0), "", "Layer", candy, 5) );
        spriteClassCandyList->spriteObj.push_back( SpriteObject::CreateLabel("친구와 함께 보상을 받으세요!", fontList[0], 36, ccp(0, 0), ccp(240, 250), ccc3(255,255,255), "", "Layer", candy, 5) );
         */
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
        fid = myInfo->GetFairyList()[i]->GetId();
        
        switch (fid)
        {
            case 1: sprintf(fname, "icon/icon_fairyslot_flower.png"); break;
            case 2: sprintf(fname, "icon/icon_fairyslot_sun.png"); break;
            case 3: sprintf(fname, "icon/icon_fairyslot_cloud.png"); break;
        }
        //sprintf(fname, "background/bg_skill_yellow.png%d", i);
        spriteClassFairySlot->spriteObj.push_back( SpriteObject::Create(0, fname, ccp(0, 0), ccp(i*(146+5), 0), CCSize(0, 0), "", "Layer", containerFairy, 3, 0, 255, fuid) );
        
        FairyInfo* f = FairyInfo::GetObj(fid);
        spriteClassFairySlot->spriteObj.push_back( SpriteObject::CreateLabel(f->GetName(), fontList[0], 30, ccp(0.5, 0.5), ccp(i*(146+5)+73+3, 18-3), ccc3(104,62,51), "", "Layer", containerFairy, 5) );
        spriteClassFairySlot->spriteObj.push_back( SpriteObject::CreateLabel(f->GetName(), fontList[0], 30, ccp(0.5, 0.5), ccp(i*(146+5)+73, 18), ccc3(255,255,255), "", "Layer", containerFairy, 5) );
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
    
    rect = CCRectZero;
    kind = -1;
    idx = -1;
    
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
                if (myInfo->IsTodayCandyUsed())
                {
                    // 오늘 오.별을 이미 1번 행한 경우
                    std::vector<int> nullData;
                    Common::ShowPopup(this, "CocoRoom", "NoImage", false, TODAYCANDY_ALREADY_DONE, BTN_1, nullData);
                }
                else
                {
                    // 오.별 친구고르기
                    Common::ShowNextScene(this, "CocoRoom", "CocoRoomTodayCandy", false);
                }
                return true;
            }
        }
        else if (spriteClass->spriteObj[i]->name == "background/bg_topinfo.png1")
        {
            if (spriteClass->spriteObj[i]->sprite9->boundingBox().containsPoint(point))
            {
                sound->playClickboard();
                Common::ShowNextScene(this, "Ranking", "BuyTopaz", false, 0);
                return true;
            }
        }
        else if (spriteClass->spriteObj[i]->name == "background/bg_topinfo.png2")
        {
            if (spriteClass->spriteObj[i]->sprite9->boundingBox().containsPoint(point))
            {
                sound->playClickboard();
                Common::ShowNextScene(this, "Ranking", "BuyStarCandy", false, 0);
                return true;
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
                sound->playClick();
                spriteClassCoco->spriteObj[i]->sprite->setColor(ccc3(170,170,170));
                ((CCSprite*)spriteClassCoco->FindSpriteByName("letter/letter_staff.png1"))->setColor(ccc3(170,170,170));
                ((CCSprite*)spriteClassCoco->FindSpriteByName("letter/letter_buildup_normal.png"))->setColor(ccc3(170,170,170));
                ((CCSprite*)spriteClassCoco->FindSpriteByName("icon/icon_lock_white.png1"))->setColor(ccc3(170,170,170));
                ((CCSprite*)spriteClassCoco->FindSpriteByName("button/btn_receive_starcandy.png"))->setColor(ccc3(170,170,170));
                
                rect = spriteClassCoco->spriteObj[i]->sprite->boundingBox();
                kind = BTN_MENU_UPGRADE_1;
                idx = i;
                return true;
            }
        }
        else if (spriteClassCoco->spriteObj[i]->name == "button/btn_green.png2")
        {
            if (spriteClassCoco->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                spriteClassCoco->spriteObj[i]->sprite->setColor(ccc3(170,170,170));
                ((CCSprite*)spriteClassCoco->FindSpriteByName("letter/letter_staff.png2"))->setColor(ccc3(170,170,170));
                ((CCSprite*)spriteClassCoco->FindSpriteByName("letter/letter_buildup_fine.png"))->setColor(ccc3(170,170,170));
                ((CCSprite*)spriteClassCoco->FindSpriteByName("icon/icon_lock_white.png2"))->setColor(ccc3(170,170,170));
                ((CCSprite*)spriteClassCoco->FindSpriteByName("button/btn_receive_topaz.png"))->setColor(ccc3(170,170,170));
                
                rect = spriteClassCoco->spriteObj[i]->sprite->boundingBox();
                kind = BTN_MENU_UPGRADE_2;
                idx = i;
                return true;
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
                sound->playClick();
                spriteClassFairy->spriteObj[i]->sprite->setColor(ccc3(170,170,170));
                ((CCSprite*)spriteClassFairy->FindSpriteByName("letter/letter_normal.png"))->setColor(ccc3(170,170,170));
                ((CCSprite*)spriteClassFairy->FindSpriteByName("letter/letter_tonic.png1"))->setColor(ccc3(170,170,170));
                ((CCSprite*)spriteClassFairy->FindSpriteByName("icon/icon_lock_white.png1"))->setColor(ccc3(170,170,170));
                ((CCSprite*)spriteClassFairy->FindSpriteByName("button/btn_receive_starcandy.png"))->setColor(ccc3(170,170,170));
                
                rect = spriteClassFairy->spriteObj[i]->sprite->boundingBox();
                kind = BTN_MENU_UPGRADE_1;
                idx = i;
                return true;
            }
        }
        else if (spriteClassFairy->spriteObj[i]->name == "button/btn_green.png2")
        {
            if (spriteClassFairy->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                spriteClassFairy->spriteObj[i]->sprite->setColor(ccc3(170,170,170));
                ((CCSprite*)spriteClassFairy->FindSpriteByName("letter/letter_fine.png"))->setColor(ccc3(170,170,170));
                ((CCSprite*)spriteClassFairy->FindSpriteByName("letter/letter_tonic.png2"))->setColor(ccc3(170,170,170));
                ((CCSprite*)spriteClassFairy->FindSpriteByName("icon/icon_lock_white.png2"))->setColor(ccc3(170,170,170));
                ((CCSprite*)spriteClassFairy->FindSpriteByName("button/btn_receive_topaz.png"))->setColor(ccc3(170,170,170));
                
                rect = spriteClassFairy->spriteObj[i]->sprite->boundingBox();
                kind = BTN_MENU_UPGRADE_2;
                idx = i;
                return true;
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
                if (!myInfo->IsTodayCandyUsed())
                {
                    sound->playClick();
                    spriteClassCandy->spriteObj[i]->sprite->setColor(ccc3(170,170,170));
                    ((CCSprite*)spriteClassCandy->FindSpriteByName("letter/letter_fairy_select.png"))->setColor(ccc3(170,170,170));
                    rect = spriteClassCandy->spriteObj[i]->sprite->boundingBox();
                    kind = BTN_MENU_CONFIRM;
                    idx = i;
                    return true;
                }
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
                    // Loading 화면으로 MESSAGE request 넘기기
                    Common::ShowNextScene(this, "SketchDetail", "Loading", false, LOADING_MESSAGE);
                    
                    char temp[150];
                    std::string param = "";
                    sprintf(temp, "kakao_id=%d&", myInfo->GetKakaoId());
                    param += temp;
                    sprintf(temp, "user_fairy_id=%d", tag);
                    param += temp;
                    
                    Network::HttpPost(param, URL_USING_FAIRY, this, httpresponse_selector(CocoRoom::onHttpRequestCompleted));
                    break;
                }
            }
        }
    }
    
    if (idx > -1)
    {
        if (curState == 1) // 코코의방
        {
            spriteClassCoco->spriteObj[idx]->sprite->setColor(ccc3(255,255,255));
            ((CCSprite*)spriteClassCoco->FindSpriteByName("letter/letter_staff.png1"))->setColor(ccc3(255,255,255));
            ((CCSprite*)spriteClassCoco->FindSpriteByName("letter/letter_buildup_normal.png"))->setColor(ccc3(255,255,255));
            ((CCSprite*)spriteClassCoco->FindSpriteByName("icon/icon_lock_white.png1"))->setColor(ccc3(255,255,255));
            ((CCSprite*)spriteClassCoco->FindSpriteByName("button/btn_receive_starcandy.png"))->setColor(ccc3(255,255,255));
            ((CCSprite*)spriteClassCoco->FindSpriteByName("letter/letter_staff.png2"))->setColor(ccc3(255,255,255));
            ((CCSprite*)spriteClassCoco->FindSpriteByName("letter/letter_buildup_fine.png"))->setColor(ccc3(255,255,255));
            ((CCSprite*)spriteClassCoco->FindSpriteByName("icon/icon_lock_white.png2"))->setColor(ccc3(255,255,255));
            ((CCSprite*)spriteClassCoco->FindSpriteByName("button/btn_receive_topaz.png"))->setColor(ccc3(255,255,255));
        }
        else if (curState == 2) // 요정
        {
            spriteClassFairy->spriteObj[idx]->sprite->setColor(ccc3(255,255,255));
            ((CCSprite*)spriteClassFairy->FindSpriteByName("letter/letter_normal.png"))->setColor(ccc3(255,255,255));
            ((CCSprite*)spriteClassFairy->FindSpriteByName("letter/letter_tonic.png1"))->setColor(ccc3(255,255,255));
            ((CCSprite*)spriteClassFairy->FindSpriteByName("icon/icon_lock_white.png1"))->setColor(ccc3(255,255,255));
            ((CCSprite*)spriteClassFairy->FindSpriteByName("button/btn_receive_starcandy.png"))->setColor(ccc3(255,255,255));
            ((CCSprite*)spriteClassFairy->FindSpriteByName("letter/letter_fine.png"))->setColor(ccc3(255,255,255));
            ((CCSprite*)spriteClassFairy->FindSpriteByName("letter/letter_tonic.png2"))->setColor(ccc3(255,255,255));
            ((CCSprite*)spriteClassFairy->FindSpriteByName("icon/icon_lock_white.png2"))->setColor(ccc3(255,255,255));
            ((CCSprite*)spriteClassFairy->FindSpriteByName("button/btn_receive_topaz.png"))->setColor(ccc3(255,255,255));
        }
        else if (curState == 3) // 오.별
        {
            spriteClassCandy->spriteObj[idx]->sprite->setColor(ccc3(255,255,255));
            ((CCSprite*)spriteClassCandy->FindSpriteByName("letter/letter_fairy_select.png"))->setColor(ccc3(255,255,255));
        }
    }
    if (rect.containsPoint(point))
    {
        if (kind == BTN_MENU_UPGRADE_1 || kind == BTN_MENU_UPGRADE_2)
        {
            if (curState == 1) // 코코(지팡이) 강화
            {
                if (myInfo->GetMPTotal() < 500)
                {
                    sound->playClick();
                    std::vector<int> nullData;
                    Common::ShowPopup(this, "CocoRoom", "NoImage", false, UPGRADE_STAFF_INSUFFICIENT_MP, BTN_1, nullData);
                }
                else if (kind == BTN_MENU_UPGRADE_1 && myInfo->GetStaffLv() < magicStaffBuildupInfo[magicStaffBuildupInfo.size()-1]->GetLevel())
                {
                    std::vector<int> data;
                    data.push_back(0);
                    data.push_back(magicStaffBuildupInfo[myInfo->GetStaffLv()-1+1]->GetCost_StarCandy());
                    Common::ShowPopup(this, "CocoRoom", "NoImage", false, UPGRADE_STAFF_BY_STARCANDY_TRY, BTN_2, data);
                }
                else if (kind == BTN_MENU_UPGRADE_2 && myInfo->GetStaffLv() < magicStaffBuildupInfo[magicStaffBuildupInfo.size()-1]->GetLevel())
                {
                    std::vector<int> data;
                    data.push_back(0);
                    data.push_back(magicStaffBuildupInfo[myInfo->GetStaffLv()-1+1]->GetCost_Topaz());
                    Common::ShowPopup(this, "CocoRoom", "NoImage", false, UPGRADE_STAFF_BY_TOPAZ_TRY, BTN_2, data);
                }
            }
            else if (curState == 2) // 요정 강화
            {
                if (kind == BTN_MENU_UPGRADE_1 && myInfo->GetActiveFairyLevel() < FairyBuildUpInfo::GetMaxLevel(myInfo->GetActiveFairyId()))
                {
                    if (myInfo->GetActiveFairyId() > 0)
                    {
                        sound->playClick();
                        std::vector<int> data;
                        data.push_back(0);
                        data.push_back(FairyBuildUpInfo::GetCostStarCandy(myInfo->GetActiveFairyId(), myInfo->GetActiveFairyLevel()+1));
                        Common::ShowPopup(this, "CocoRoom", "NoImage", false, UPGRADE_FAIRY_BY_STARCANDY_TRY, BTN_2, data);
                    }
                }
                else if (kind == BTN_MENU_UPGRADE_2 && myInfo->GetActiveFairyLevel() < FairyBuildUpInfo::GetMaxLevel(myInfo->GetActiveFairyId()))
                {
                    if (myInfo->GetActiveFairyId() > 0)
                    {
                        sound->playClick();
                        std::vector<int> data;
                        data.push_back(0);
                        data.push_back(FairyBuildUpInfo::GetCostTopaz(myInfo->GetActiveFairyId(), myInfo->GetActiveFairyLevel()+1));
                        Common::ShowPopup(this, "CocoRoom", "NoImage", false, UPGRADE_FAIRY_BY_TOPAZ_TRY, BTN_2, data);
                    }
                }
            }
        }
        else if (kind == BTN_MENU_CONFIRM) // 오.별
        {
            // 친구가 4명보다 적을 경우 띄운다.
            if (friendList.size() < 4+1)
            {
                std::vector<int> nullData;
                Common::ShowPopup(this, "CocoRoom", "NoImage", false, TODAYCANDY_NOTENOUGH_FRIENDS, BTN_1, nullData);
            }
            else
            {
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
                        // Loading 화면으로 MESSAGE request 넘기기
                        Common::ShowNextScene(this, "CocoRoom", "Loading", false, LOADING_MESSAGE);
                        
                        // 오늘의 별사탕 결과 받기
                        char temp[150];
                        std::string param = "";
                        sprintf(temp, "kakao_id=%d&", myInfo->GetKakaoId());
                        param += temp;
                        for (int i = 1 ; i < 5 ; i++)
                        {
                            sprintf(temp, "friend_kakao_id_%d=%d&", i, todayCandyKakaoId[i]);
                            param += temp;
                        }
                        param = param.substr(0, param.size()-1);
                        
                        Network::HttpPost(param, URL_TODAY_STARCANDY, this, httpresponse_selector(CocoRoom::onHttpRequestCompleted));
                    }
                }
                else
                    Common::ShowNextScene(this, "CocoRoom", "CocoRoomTodayCandy", false); // 오.별 친구고르기
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
    
    stencil->removeFromParentAndCleanup(true);
    clip->removeAllChildren();
    clip->removeFromParentAndCleanup(true);
    
    tLayer->removeAllChildren();
    tLayer->removeFromParentAndCleanup(true);
    
    this->removeFromParentAndCleanup(true);
}

void CocoRoom::EndSceneCallback()
{
}


void CocoRoom::onHttpRequestCompleted(CCNode *sender, void *data)
{
    // Loading 창 끄기
    ((Loading*)Depth::GetCurPointer())->EndScene();
    
    CCHttpResponse* res = (CCHttpResponse*) data;
    
    xml_document xmlDoc;
    Network::GetXMLFromResponseData(res, xmlDoc);

    if (curState == 2)
        XmlParseFairyList(&xmlDoc);
    else if (curState == 3)
        XmlParseTodayCandy(&xmlDoc);
}

void CocoRoom::XmlParseFairyList(xml_document *xmlDoc)
{
    xml_node nodeResult = xmlDoc->child("response");
    int code = nodeResult.child("code").text().as_int();
    
    // 에러일 경우 code에 따라 적절히 팝업창 띄워줌.
    if (code != 0)
    {
        std::vector<int> nullData;
        if (code <= MAX_COMMON_ERROR_CODE)
            Network::ShowCommonError(code);
        else
            Common::ShowPopup(this, "CocoRoom", "NoImage", false, NETWORK_FAIL, BTN_1, nullData);
    }
    
    else if (code == 0)
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
}

void CocoRoom::XmlParseTodayCandy(xml_document *xmlDoc)
{
    xml_node nodeResult = xmlDoc->child("response");
    int code = nodeResult.child("code").text().as_int();
    
    // 에러일 경우 code에 따라 적절히 팝업창 띄워줌.
    if (code != 0)
    {
        std::vector<int> nullData;
        if (code <= MAX_COMMON_ERROR_CODE)
            Network::ShowCommonError(code);
        else if (code == 10) // 오늘 이미 오.별 이용했음.
            Common::ShowPopup(this, "CocoRoom", "NoImage", false, TODAYCANDY_ALREADY_DONE, BTN_1, nullData);
        else
            Common::ShowPopup(this, "CocoRoom", "NoImage", false, NETWORK_FAIL, BTN_1, nullData);
    }
    
    else if (code == 0)
    {
        // 오늘의 별사탕 액션 시작
        int selectedKakaoId = nodeResult.child("today-starcandy").attribute("selected-user").as_int();
        myInfo->SetTodayCandy(1);
        DecideUser(selectedKakaoId);
    }
}
