#include "GameReady.h"
#include "Ranking.h"
#include "MagicList.h"

//static int priority;

GameReady::~GameReady(void)
{
    CCLog("GameReady 소멸자");
}

CCScene* GameReady::scene(int prio)
{
//    priority = prio;
    
    CCScene* pScene = CCScene::create();
    GameReady* pLayer = GameReady::create();
    pScene->addChild(pLayer);
    
	return pScene;
}

void GameReady::onEnter()
{
    CCLog("GameReady :: onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority(), true);
    CCLayer::onEnter();
}
void GameReady::onExit()
{
    CCLog("GameReady :: onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
}

void GameReady::registerWithTouchDispatcher(void)
{
    CCLog("GameReady :: registerWithTouchDispatcher");
    //CCDirector* pDirector = CCDirector::sharedDirector();
    //pDirector->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
}

void GameReady::keyBackClicked()
{
    EndSceneCallback(NULL, this);
}


bool GameReady::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
    // make depth tree
    Depth::AddCurDepth("GameReady", this);
    
    this->setKeypadEnabled(true);
    this->setTouchEnabled(true);
    this->setTouchPriority(Depth::GetCurPriority());
    CCLog("GameReady : touch prio = %d", Depth::GetCurPriority());
    
    // notification observer
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(GameReady::Notification), Depth::GetCurName(), NULL);
    
    // notification
    CCString* param = CCString::create("1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetParentName(), param);
    
    
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    this->schedule(schedule_selector(GameReady::PotionTimer), 0.2f);
    
    // scrollview SLOT init.
    scrollViewSlot = CCScrollView::create();
    //scrollViewSlot->retain();
    scrollViewSlot->setDirection(kCCScrollViewDirectionHorizontal);
    scrollViewSlot->setViewSize(CCSizeMake(782-40, 177-20));
    scrollViewSlot->setAnchorPoint(ccp(0, 0));
    scrollViewSlot->setPosition(ccp(77+20, 488+12));
    scrollViewSlot->setDelegate(this);
    scrollViewSlot->setTouchPriority(Depth::GetCurPriority());
    this->addChild(scrollViewSlot, 5);
    
    // spriteclass init.
    spriteClass = new SpriteClass();
    spriteClassProperty = new SpriteClass();
    spriteClassFairy = new SpriteClass();
    spriteClassSkill = new SpriteClass();
    spriteClassSlot = new SpriteClass();
    
    fairyLayer = CCLayer::create();
    this->addChild(fairyLayer, 10);
    skillLayer = CCLayer::create();
    this->addChild(skillLayer, 4);
    
    // item select init.
    for (int i = 0 ; i < 5 ; i++)
    {
        itemSelected[i] = false;
        //itemNumLayer.push_back(new CCLayer());
    }
    
    InitSprites();
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
    
    InitFairy();
    InitSkill();
    
    MakeScrollSlot();
    
    isTouched = false;
    isScrollViewTouched = false;
    isScrolling = false;
    isStarting = false;
    
    return true;
}

void GameReady::Notification(CCObject* obj)
{
    CCString* param = (CCString*)obj;
    
    if (param->intValue() == 0)
    {
        // 터치 활성
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority()+1, true);
        //this->setKeypadEnabled(true);
        //this->setTouchEnabled(true);
        this->setTouchPriority(Depth::GetCurPriority());
        isTouched = false;
        CCLog("GameReady : 터치 활성 (Priority = %d)", this->getTouchPriority());
        
        // 토파즈, 별사탕, MP, 포션남은시간 정보 업데이트
        ((CCLabelTTF*)spriteClass->FindLabelByTag(1))->setString(Common::MakeComma(myInfo->GetTopaz()).c_str());
        ((CCLabelTTF*)spriteClass->FindLabelByTag(2))->setString(Common::MakeComma(myInfo->GetStarCandy()).c_str());
        ((CCLabelTTF*)spriteClass->FindLabelByTag(3))->setString(Common::MakeComma(myInfo->GetMPTotal()).c_str());
        ((CCLabelTTF*)spriteClass->FindLabelByTag(4))->setString(myInfo->GetRemainPotionTime().c_str());

        // 포션 개수에 따른 포션 아이콘 업데이트
        char name[40];
        for (int i = 0 ; i < 5 ; i++)
        {
            sprintf(name, "icon/icon_potion.png%d", i);
            if (i < myInfo->GetPotion())
                ((CCSprite*)spriteClass->FindSpriteByName(name))->setOpacity(255);
            else
                ((CCSprite*)spriteClass->FindSpriteByName(name))->setOpacity(0);
        }
    }
    else if (param->intValue() == 1)
    {
        // 터치 비활성
        CCLog("GameReady 터치 비활성");
        //this->setKeypadEnabled(false);
        //this->setTouchEnabled(false);
        CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
    }
    else if (param->intValue() == 2)
    {
        // 토파즈, 별사탕, MP, 포션남은시간 정보 업데이트
        ((CCLabelTTF*)spriteClass->FindLabelByTag(1))->setString(Common::MakeComma(myInfo->GetTopaz()).c_str());
        ((CCLabelTTF*)spriteClass->FindLabelByTag(2))->setString(Common::MakeComma(myInfo->GetStarCandy()).c_str());
        ((CCLabelTTF*)spriteClass->FindLabelByTag(3))->setString(Common::MakeComma(myInfo->GetMPTotal()).c_str());
        ((CCLabelTTF*)spriteClass->FindLabelByTag(4))->setString(myInfo->GetRemainPotionTime().c_str());
    }
    else if (param->intValue() == 3)
    {
        // 코코 정보가 바뀌었을 경우 (지팡이 강화 등으로)
    }
    else if (param->intValue() == 4)
    {
        // 요정 정보가 바뀐 경우 (요정 선택변경, 요정 강화 등)
        InitFairy();
    }
    else if (param->intValue() == 5)
    {
        CCLog("게임레디 : 스킬 바꾸자!");
        // 스킬 정보가 바뀐 경우 (게임준비->스케치북->게임준비 돌아올 때)
        InitSkill();
    }
    else if (param->intValue() == 8)
    {
        // 스킬 속성 종류 갱신
        InitProperties();
    }
    else if (param->intValue() == 9)
    {
        // 스킬 슬롯 정보 갱신
        spriteClassSlot->RemoveAllObjects();
        MakeScrollSlot();
        char name[7];
        sprintf(name, "%d", (int)myInfo->GetSlot().size());
        ((CCLabelTTF*)spriteClass->FindLabelByTag(100))->setString(name);
    }
}

void GameReady::InitSprites()
{
    pBlack = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, winSize.width, winSize.height));
    pBlack->setPosition(ccp(0, 0));
    pBlack->setAnchorPoint(ccp(0, 0));
    pBlack->setColor(ccc3(0, 0, 0));
    pBlack->setOpacity(150);
    this->addChild(pBlack, 0);

    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_topinfo.png1", ccp(0, 0), ccp(80, 1666), CCSize(230, 75), "", "GameReady", this, 0) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_topinfo.png2", ccp(0, 0), ccp(390, 1666), CCSize(290, 75), "", "GameReady", this, 0) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_topinfo.png3", ccp(0, 0), ccp(765, 1666), CCSize(290, 75), "", "GameReady", this, 0) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_topaz.png", ccp(0, 0), ccp(15+10, 1656), CCSize(0, 0), "", "GameReady", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_starcandy.png", ccp(0, 0), ccp(317, 1660), CCSize(0, 0), "", "GameReady", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_magicpoint.png", ccp(0, 0), ccp(696, 1669), CCSize(0, 0), "", "GameReady", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_topinfo_plus.png1", ccp(0, 0), ccp(80+230-55, 1679), CCSize(0, 0), "", "GameReady", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_topinfo_plus.png2", ccp(0, 0), ccp(390+290-55, 1679), CCSize(0, 0), "", "GameReady", this, 5) );
    
    
    // topaz
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(Common::MakeComma(myInfo->GetTopaz()), fontList[0], 36, ccp(0.5, 0), ccp((80+230+80)/2, 1686), ccc3(255,255,255), "", "GameReady", this, 5, 0, 255, 1) );
    // starcandy
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(Common::MakeComma(myInfo->GetStarCandy()), fontList[0], 36, ccp(0.5, 0), ccp((390+290+390)/2, 1686), ccc3(255,255,255), "", "GameReady", this, 5, 0, 255, 2) );
    // magic-point
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(Common::MakeComma(myInfo->GetMPTotal()), fontList[0], 36, ccp(0.5, 0), ccp((765+765+290)/2, 1686), ccc3(255,255,255), "", "GameReady", this, 5, 0, 255, 3) );
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png1", ccp(0, 0), ccp(49, 687), CCSize(982, 725), "", "GameReady", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png1", ccp(0, 0), ccp(77, 1126), CCSize(929, 268), "", "GameReady", this, 1) );
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png2", ccp(0, 0), ccp(77, 726), CCSize(929, 200), "", "GameReady", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png2", ccp(0, 0), ccp(92, 1143), CCSize(309, 236), "", "GameReady", this, 1) );
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_gameready_name.png1", ccp(0, 0), ccp(112, 1165), CCSize(274, 53), "", "GameReady", this, 4, 128) );
    
    
    // property 문양 (mp 바로 밑에)
    InitProperties();
    
    // roof
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_roof.png1", ccp(0, 0), ccp(10, 1433), CCSize(0, 0), "", "GameReady", this, 1) );
    CCSize roofSize = spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->getContentSize();
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_roof.png2", ccp(0, 0), ccp(10+roofSize.width, 1433), CCSize(0, 0), "", "GameReady", this, 1) );
    spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->setFlipX(true);
    
    // 버튼들
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_potion_time.png", ccp(0, 0), ccp(506, 1493), CCSize(0, 0), "", "GameReady", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_topinfo_plus.png3", ccp(0, 0), ccp(645, 1498), CCSize(0, 0), "", "GameReady", this, 5) );
    
    // make potion
    char name[40], name2[40];
    for (int i = 0; i < 5; i++)
    {
        sprintf(name, "icon/icon_potion_empty.png%d", i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(89+83*i, 1480), CCSize(0, 0), "", "GameReady", this, 4) );
        
        sprintf(name2, "icon/icon_potion.png%d", i);
        if (i < myInfo->GetPotion())
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0.5, 0.5), spriteClass->FindParentCenterPos(name), CCSize(0, 0), name, "0", NULL, 1000, 1, 255) );
        else
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0.5, 0.5), spriteClass->FindParentCenterPos(name), CCSize(0, 0), name, "0", NULL, 1000, 1, 0) );
    }
    
    // potion-remain-time
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(myInfo->GetRemainPotionTime(), fontList[0], 36, ccp(0, 0), ccp(530, 1508), ccc3(255,255,255), "", "GameReady", this, 5, 0, 255, 4) );

    // strap, 제일 아래 버튼 등
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_blue.png", ccp(0, 0), ccp(14, 1343), CCSize(0, 0), "", "GameReady", this, 11) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_title_gameready.png", ccp(0, 0), ccp(409, 1389), CCSize(0, 0), "", "GameReady", this, 11) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_x_yellow.png", ccp(0, 0), ccp(875, 1391), CCSize(0, 0), "", "GameReady", this, 11) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_grade_a.png", ccp(0, 0), ccp(408+11, 1143+165), CCSize(0, 0), "", "GameReady", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_petlevel.png", ccp(0, 0), ccp(408+55, 1143+187), CCSize(0, 0), "", "GameReady", this, 5) );
    
    // 5 items
    char name3[40];
    int alpha;
    for (int i = 0 ; i < 5; i++)
    {
        sprintf(name3, "item_%d", i);
        itemSelected[i] = CCUserDefault::sharedUserDefault()->getIntegerForKey(name3, 0);
    
        sprintf(name, "background/bg_skill_brown.png%d", i+1);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(84+193*i, 972), CCSize(0, 0), "", "GameReady", this, 5) );

        if (itemSelected[i]) alpha = 255;
        else alpha = 0;
        sprintf(name2, "background/bg_skill_select.png%d", i+1);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0.5, 0.5), spriteClass->FindParentCenterPos(name), CCSize(0, 0), name, "0", NULL, 6, 1, alpha) );
        spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->setTag(i+1);
        
        sprintf(name3, "icon/icon_check.png%d", i+1);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, name3, ccp(0, 0), ccp(100, 110), CCSize(0, 0), name2, "0", NULL, 6, 2, alpha) );
        spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->setTag(i+1);
        
        if (i >= 2) {
            sprintf(name2, "background/bg_skill_yellow.png%d", i+1);
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0.5, 0.5), spriteClass->FindParentCenterPos(name), CCSize(0, 0), name, "0", NULL, 5, 1) );
        }

        // 아이템 아이콘
        if (i == 0)
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_item_clear.png", ccp(0.5, 0.5), spriteClass->FindParentCenterPos(name), CCSize(0, 0), name, "0", NULL, 6, 1) );
        else if (i == 1)
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_item_paint.png", ccp(0.5, 0.5), spriteClass->FindParentCenterPos(name), CCSize(0, 0), name, "0", NULL, 6, 1) );
        //sprintf(name2, "icon/icon_item_%d.png", i+1);
        //spriteClass->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0.5, 0.5), spriteClass->FindParentCenterPos(name), CCSize(0, 0), name, "0", NULL, 6, 1) );
        
        if (myInfo->GetItem(i) > 0)
        {
            /*
            itemNumLayer[i] = Common::MakeItemNumberLayer(Common::MakeComma(myInfo->GetItem(i)));
            itemNumLayer[i]->setPosition(ccp(84+193*i+130, 965));
            this->addChild(itemNumLayer[i], 10);
            */
            CCLayer* layer = Common::MakeItemNumberLayer(Common::MakeComma(myInfo->GetItem(i)));
            layer->setPosition(ccp(84+193*i+130, 965));
            this->addChild(layer, 10);
            spriteClass->layers.push_back(layer);
        }
        else
        {
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_starcandy_mini.png", ccp(0, 0), ccp(84+193*i-15, 925), CCSize(0, 0), "", "GameReady", this, 5) );
            spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("1200", fontList[0], 36, ccp(0, 0), ccp(84+193*i+50, 935), ccc3(255,255,255), "", "GameReady", this, 5) );
        }
    }
    
    // slot part
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png5", ccp(0, 0), ccp(49, 458), CCSize(982, 223), "", "GameReady", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png3", ccp(0, 0), ccp(77, 488), CCSize(782, 177), "", "GameReady", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_plus_big.png", ccp(0, 0), ccp(896, 572), CCSize(0, 0), "", "GameReady", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_gameready_name.png4", ccp(0, 0), ccp(867, 497), CCSize(136, 63), "", "GameReady", this, 1) );
    
    sprintf(name, "%d", (int)myInfo->GetSlot().size());
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(name, fontList[0], 48, ccp(0, 0), ccp(892, 505), ccc3(255,219,53), "", "GameReady", this, 5, 0, 255, 100) ); // 현재 슬롯 개수
    sprintf(name, "/ %d", (int)skillSlotInfo.size());
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(name, fontList[0], 36, ccp(0, 0), ccp(927, 505), ccc3(182,142,142), "", "GameReady", this, 5) ); // 젼체 슬롯 개수
    

    // 게임시작 버튼
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_blue.png", ccp(0, 0), ccp(318, 193), CCSize(0, 0), "", "GameReady", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_gamestart.png", ccp(0.5, 0.5), ccp(319+446/2, 191+160/2+5), CCSize(0, 0), "", "GameReady", this, 5) );
    
    // 오늘의 별사탕 버튼
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_todaycandy.png", ccp(0, 0), ccp(886-50, 1920-1674-44), CCSize(0, 0), "", "GameReady", this, 5) );
    
    // 코코 그림
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "image/coco_ready.png", ccp(0.5, 0.5), ccp(250, 1190+100), CCSize(0, 0), "", "GameReady", this, 3) );
    ((CCSprite*)spriteClass->FindSpriteByName("image/coco_ready.png"))->setScale(0.85f);
    
    // 지팡이 레벨 표시
    sprintf(name, "지팡이 %dLv", myInfo->GetStaffLv());
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(name, fontList[0], 30, ccp(0.5, 0.5), ccp(250, 1190), ccc3(121,71,0), "", "GameReady", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(name, fontList[0], 30, ccp(0.5, 0.5), ccp(250, 1193), ccc3(255,219,53), "", "GameReady", this, 5) );
    
    // 아이템별 설명
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabelArea("행복하지 않을 이유가 하나도 없습니다.", fontList[0], 36, ccp(0, 0), ccp(97, 736), ccc3(0,0,0), CCSize(909, 180), kCCTextAlignmentLeft, kCCVerticalTextAlignmentCenter, "", "GameReady", this, 5) );
}

void GameReady::InitProperties()
{
    spriteClassProperty->RemoveAllObjects();
    
    CCPoint pos = ccp(848, 1611);
    if (myInfo->IsFire())
    {
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_property.png1", ccp(0, 0), pos, CCSize(0, 0), "", "GameReady", this, 5) );
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_property_fire_mini.png", ccp(0.5, 0.5), spriteClass->FindParentCenterPos("background/bg_property.png1"), CCSize(0, 0), "background/bg_property.png1", "0", NULL, 5, 1) );
        pos = ccp(904, 1611);
    }
    if (myInfo->IsWater())
    {
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_property.png2", ccp(0, 0), pos, CCSize(0, 0), "", "GameReady", this, 5) );
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_property_water_mini.png", ccp(0.5, 0.5), spriteClass->FindParentCenterPos("background/bg_property.png2"), CCSize(0, 0), "background/bg_property.png2", "0", NULL, 5, 1) );
        pos = ccp(959, 1611);
    }
    if (myInfo->IsLand())
    {
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_property.png3", ccp(0, 0), pos, CCSize(0, 0), "", "GameReady", this, 5) );
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_property_land_mini.png", ccp(0.5, 0.5), spriteClass->FindParentCenterPos("background/bg_property.png3"), CCSize(0, 0), "background/bg_property.png3", "0", NULL, 5, 1) );
        pos = ccp(959, 1611);
    }
    
    for (int i = 0 ; i < spriteClassProperty->spriteObj.size() ; i++)
        spriteClassProperty->AddChild(i);
}



void GameReady::InitFairy()
{
    // init
    spriteClassFairy->RemoveAllObjects();
    CCNode* pic = fairyLayer->getChildByTag(99999);
    if (pic != NULL)
        pic->removeAllChildren();
    fairyLayer->removeAllChildren();
    
    fairyLayer->setAnchorPoint(ccp(0, 0));
    fairyLayer->setPosition(ccp(408, 1143));
    
    spriteClassFairy->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png3", ccp(0, 0), ccp(0, 0), CCSize(309, 236), "", "Layer", fairyLayer, 4) );
    
    fairyLayer->setContentSize(CCSizeMake(309, 236));
    
    int fid = myInfo->GetActiveFairyId();
    int flv = myInfo->GetActiveFairyLevel();
    CCLog("fid flv : %d %d", fid, flv);
    FairyInfo* f = FairyInfo::GetObj(fid);
    
    // 요정 그림
    CCLayer* picture = Fairy::GetFairy(fid);
    picture->setAnchorPoint(ccp(0, 0));
    if (fid == 1)
    {
        picture->setPosition(ccp(309/2+10, 236/2+23));
        picture->setScale(0.63f);
    }
    else if (fid == 2)
    {
        picture->setPosition(ccp(309/2, 236/2+15));
        picture->setScale(0.7f);
    }
    else
    {
        picture->setPosition(ccp(309/2, 236/2+23));
        picture->setScale(0.9f);
    }
    picture->setTag(99999);
    fairyLayer->addChild(picture, 5);
    
    // 요정 등급
    char fname[30];
    if (fid > 0)
    {
        if (f->GetGrade() == 1) sprintf(fname, "letter/letter_grade_a.png");
        else if (f->GetGrade() == 2) sprintf(fname, "letter/letter_grade_b.png");
        else if (f->GetGrade() == 3) sprintf(fname, "letter/letter_grade_c.png");
        else if (f->GetGrade() == 4) sprintf(fname, "letter/letter_grade_d.png");
        spriteClassFairy->spriteObj.push_back( SpriteObject::Create(0, fname, ccp(0, 0), ccp(11, 165), CCSize(0, 0), "", "Layer", fairyLayer, 5) );
    }
    
    if (fid > 0)
    {
        // 요정 레벨 (+그 배경)
        spriteClassFairy->spriteObj.push_back( SpriteObject::Create(0, "background/bg_petlevel.png", ccp(0, 0), ccp(55, 187), CCSize(0, 0), "", "Layer", fairyLayer, 6) );
        sprintf(fname, "%d Lv", flv);
        spriteClassFairy->spriteObj.push_back( SpriteObject::CreateLabel(fname, fontList[0], 24, ccp(0.5, 0.5), ccp(97, 202), ccc3(255,255,255), "", "Layer", fairyLayer, 6) );
    }
    
    if (fid > 0)
    {
        // 요정 이름
        if (fid == 0) sprintf(fname, "요정 없음");
        else sprintf(fname, "%s", f->GetName().c_str());
        spriteClassFairy->spriteObj.push_back( SpriteObject::CreateLabel(fname, fontList[0], 30, ccp(0, 0), ccp(13, 85), ccc3(255,255,255), "", "Layer", fairyLayer, 6) );
    }
    
    // 요정 특수능력 (+그 배경)
    spriteClassFairy->spriteObj.push_back( SpriteObject::Create(1, "background/bg_gameready_name.png1", ccp(0, 0), ccp(19, 22), CCSize(274, 53), "", "Layer", fairyLayer, 6) );
    
    if (fid > 0)
        sprintf(fname, "%s", FairyInfo::GetAbilityDesc(f->GetType()).c_str());
    else
        sprintf(fname, "요정 없음");
    spriteClassFairy->spriteObj.push_back( SpriteObject::CreateLabel(fname, fontList[2], 30, ccp(0.5, 0.5), ccp(19+274/2, 22+53/2), ccc3(121,71,0), "", "Layer", fairyLayer, 6) );
    spriteClassFairy->spriteObj.push_back( SpriteObject::CreateLabel(fname, fontList[2], 30, ccp(0.5, 0.5), ccp(19+274/2, 22+53/2+3), ccc3(255,219,53), "", "Layer", fairyLayer, 6) );
    
    for (int i = 0 ; i < spriteClassFairy->spriteObj.size() ; i++)
        spriteClassFairy->AddChild(i);
}

void GameReady::InitSkill()
{
    // init
    spriteClassSkill->RemoveAllObjects();
    skillLayer->removeAllChildren();
    
    skillLayer->setAnchorPoint(ccp(0, 0));
    skillLayer->setPosition(ccp(725, 1143));
    
    int sid = myInfo->GetPracticeSkillId();
    int slv = myInfo->GetPracticeSkillLv();
    //CCLog("sid slv = %d %d", sid, slv);
    
    // 배경판
    spriteClassSkill->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png4", ccp(0, 0), ccp(0, 0), CCSize(263, 236), "", "Layer", skillLayer, 1) );
    
    skillLayer->setContentSize(CCSizeMake(263, 236));
    
    // 스킬 배경=
    spriteClassSkill->spriteObj.push_back( SpriteObject::Create(1, "background/bg_gameready_name.png2", ccp(0, 0), ccp(19, 22), CCSize(228, 53), "", "Layer", skillLayer, 5) );
    CCPoint pos = spriteClassSkill->FindParentCenterPos("background/bg_gameready_name.png2");
    if (sid == 0) // 배운 스킬이 없는 경우 '스킬 없음' 표시
    {
        spriteClassSkill->spriteObj.push_back( SpriteObject::CreateLabel("스킬 없음", fontList[2], 30, ccp(0.5, 0.5), ccp(pos.x, pos.y+2), ccc3(255,255,255), "background/bg_gameready_name.png2", "1", NULL, 5, 2) );
    }
    
    // 스킬 문양 (+그 배경판)
    spriteClassSkill->spriteObj.push_back( SpriteObject::Create(0, "background/bg_skill_brown.png", ccp(0, 0), ccp(63, 81), CCSize(0, 0), "", "Layer", skillLayer, 5) );
    if (sid > 0) // 스킬이 있는 경우에만 실제 문양
    {
        char skillName[20];
        // 스킬 옆에 삼각형 아이콘 색깔 (속성별로)
        switch (sid / 10)
        {
            case 2: // fire
                spriteClassSkill->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_skill_division_red.png", ccp(0, 0), ccp(63, 161), CCSize(0, 0), "", "Layer", skillLayer, 5) );
                break;
            case 1: // water
                spriteClassSkill->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_skill_division_blue.png", ccp(0, 0), ccp(63, 161), CCSize(0, 0), "", "Layer", skillLayer, 5) );
                break;
            case 3: // land
                spriteClassSkill->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_skill_division_green.png", ccp(0, 0), ccp(63, 161), CCSize(0, 0), "", "Layer", skillLayer, 5) );
                break;
            default: // master
                spriteClassSkill->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_skill_division_purple.png", ccp(0, 0), ccp(63, 161), CCSize(0, 0), "", "Layer", skillLayer, 5) );
                break;
        }
        // 스킬 문양
        sprintf(skillName, "skill_%d.png", sid);
        spriteClassSkill->spriteObj.push_back( SpriteObject::Create(0, skillName, ccp(0.5, 0.5), spriteClassSkill->FindParentCenterPos("background/bg_skill_brown.png"), CCSize(0, 0), "background/bg_skill_brown.png", "0", NULL, 5, 1) );
        // 스킬 이름
        spriteClassSkill->spriteObj.push_back( SpriteObject::CreateLabel(DataProcess::FindSkillNameById(sid), fontList[2], 30, ccp(0.5, 0.5), ccp(pos.x, pos.y+2), ccc3(255,255,255), "background/bg_gameready_name.png2", "1", NULL, 5, 1) );
        
        // Lv. <- 이 그림
        spriteClassSkill->spriteObj.push_back( SpriteObject::Create(0, "number/level_lv.png", ccp(0, 0), ccp(797-699, 850-772), CCSize(0, 0), "", "Layer", skillLayer, 5) );
        // 레벨 숫자 이미지
        CCSize size = ((CCSprite*)spriteClassSkill->FindSpriteByName("number/level_lv.png"))->getContentSize();
        sprintf(skillName, "number/level_%d.png", slv % 10);
        spriteClassSkill->spriteObj.push_back( SpriteObject::Create(0, skillName, ccp(0, 0), ccp(797-699+size.width, 850-772+3), CCSize(0, 0), "", "Layer", skillLayer, 5) );
    }
    
    for (int i = 0 ; i < spriteClassSkill->spriteObj.size() ; i++)
        spriteClassSkill->AddChild(i);
}


void GameReady::MakeScrollSlot()
{
    scrollViewSlot->removeAllChildren();
    
    int numOfList = myInfo->GetSlot().size();
    
    // container 초기화
    containerSlot = CCLayer::create();
    containerSlot->setContentSize(CCSizeMake(numOfList*(146+5), 146));
    
    char fname[50], fname2[20];
    int scid;
    for (int i = 0 ; i < numOfList ; i++)
    {
        sprintf(fname, "background/bg_skill_yellow.png%d", i);
        spriteClassSlot->spriteObj.push_back( SpriteObject::Create(0, fname, ccp(0, 0), ccp(i*(146+5), 0), CCSize(0, 0), "", "Layer", containerSlot, 3) );
        
        scid = myInfo->GetSlot()[i]->GetCommonId();
        //CCLog("slot common id = %d", scid);
        if (scid > 0) // 슬롯에 스킬이 있다면 문양을 표시한다.
        {
            sprintf(fname2, "skill_%d.png", scid);
            spriteClassSlot->spriteObj.push_back( SpriteObject::Create(0, fname2, ccp(0.5, 0.5), spriteClassSlot->FindParentCenterPos(fname), CCSize(0, 0), fname, "0", NULL, 4, 1) );
        }
    }
    
    for (int i = 0 ; i < spriteClassSlot->spriteObj.size() ; i++)
        spriteClassSlot->AddChild(i);
    
    // scrollView의 container 재설정
    scrollViewSlot->setContainer(containerSlot);
    scrollViewSlot->setContentSize(containerSlot->getContentSize());
}


void GameReady::PotionTimer(float f)
{
    int remainTime, potion;
    
    potion = myInfo->GetPotion();
    remainTime = myInfo->GetRemainPotionTimeNumber();
    
    if (potion <= 5)
    {
        if (remainTime >= 719) // 719초나 720초 일 때
        {
            CCLog("GameReady: PotionTimer -> potion = %d", potion);
            char name[25];
            sprintf(name, "icon/icon_potion.png%d", potion-1);
            ((CCSprite*)spriteClass->FindSpriteByName(name))->setOpacity(255);
        }
        ((CCLabelTTF*)spriteClass->FindLabelByTag(4))->setString(myInfo->GetRemainPotionTime().c_str());
    }
}

bool GameReady::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    if (isTouched || isStarting)
        return false;
    isTouched = true;
    isScrollViewTouched = false;
    isScrolling = false;
    
    CCPoint point = pTouch->getLocation();
    CCLog("GameReady : (%d , %d)", (int)point.x, (int)point.y);
    
    if (scrollViewSlot->boundingBox().containsPoint(point))
        isScrollViewTouched = true;
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        if (spriteClass->spriteObj[i]->name == "button/btn_x_yellow.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                
                callbackType = 0;
                EndSceneCallback(NULL, this);
                return true;
            }
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_topinfo_plus.png1")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClickboard();
                Common::ShowNextScene(this, "GameReady", "BuyTopaz", false, 1);
                break;
            }
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_topinfo_plus.png2")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClickboard();
                Common::ShowNextScene(this, "GameReady", "BuyStarCandy", false, 1);
                break;
            }
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_topinfo_plus.png3")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClickboard();
                Common::ShowNextScene(this, "GameReady", "BuyPotion", false, 1);
                break;
            }
        }

        else if (spriteClass->spriteObj[i]->name.substr(0, 29) == "background/bg_skill_brown.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                char n1[35], n2[35], itemName[8];
                std::string name = spriteClass->spriteObj[i]->name;
                sprintf(n1, "background/bg_skill_select.png%c", name[name.size()-1]);
                sprintf(n2, "icon/icon_check.png%c", name[name.size()-1]);
                
                int tag = name[name.size()-1] - '0';
                itemSelected[tag-1] = !itemSelected[tag-1];
                // user default에도 바로바로 저장.
                sprintf(itemName, "item_%d", tag-1);
                CCUserDefault::sharedUserDefault()->setIntegerForKey(itemName, itemSelected[tag-1]);
                
                if (tag <= 2)
                    sound->playClick();
                else
                    sound->playClickboard();
                
                CCSprite* border = (CCSprite*)spriteClass->FindSpriteByName(n1);
                CCSprite* check = (CCSprite*)spriteClass->FindSpriteByName(n2);
                border->setOpacity(255-border->getOpacity());
                check->setOpacity(255-check->getOpacity());
            }
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_blue.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                if (myInfo->GetPotion() > 0)
                {
                    isStarting = true;
                    
                    // 포션 1개 없애기
                    /*
                    if (myInfo->GetPotion() <= 5) // 5개보다 많을 때는 액션 없어도 될 듯.
                    {
                        CCPoint p = spriteClass->FindParentCenterPos("icon/icon_potion_empty.png0");
                        CCPoint realPos = ccp(89+83*(myInfo->GetPotion()-1)+p.x, 1480+p.y);
                        
                        char name[25];
                        sprintf(name, "icon/icon_potion.png%d", myInfo->GetPotion()-1);
                        CCPoint bluePos = spriteClass->FindParentCenterPos("button/btn_blue.png");
                        CCActionInterval* action = CCSequence::create(
                                            CCMoveBy::create(1.0f, ccp(318+bluePos.x-realPos.x, 193+bluePos.y-realPos.y)), CCFadeOut::create(0.3f), NULL);
                        ((CCSprite*)spriteClass->FindSpriteByName(name))->runAction(action);
                    }
                    */
                    
                    sound->playGameStart();
                    
                    CCPoint p = ccp(530+50, 1508+15);
                    if (myInfo->GetPotion() <= 5)
                        p = ccp(89+83*(myInfo->GetPotion()-1)+60/2, 1480+82/2);
                    CCParticleSystemQuad* m_emitter = CCParticleSystemQuad::create("particles/gamestart.plist");
                    //m_emitter->retain();
                    m_emitter->setAnchorPoint(ccp(0.5, 0.5));
                    m_emitter->setPosition(p);
                    this->addChild(m_emitter, 2000);
                    m_emitter->setAutoRemoveOnFinish(true);
                    
                    if (myInfo->GetPotion() > 5)
                        myInfo->SetPotion(myInfo->GetPotion()-1, 0);
                    else if (myInfo->GetPotion() == 5)
                        myInfo->SetPotion(myInfo->GetPotion()-1, 720);
                    else
                        myInfo->SetPotion(myInfo->GetPotion()-1, -1);
                    
                    CCString* param = CCString::create("2");
                    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetCurName(), param);
                    
                    if (myInfo->GetPotion() < 5)
                    {
                        char name[25];
                        sprintf(name, "icon/icon_potion.png%d", myInfo->GetPotion());
                        CCActionInterval* action = CCFadeOut::create(0.2f);
                        ((CCSprite*)spriteClass->FindSpriteByName(name))->runAction(action);
                    }

                    
                    pBlackClose = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, winSize.width, winSize.height));
                    pBlackClose->setPosition(ccp(0, 0));
                    pBlackClose->setAnchorPoint(ccp(0, 0));
                    pBlackClose->setColor(ccc3(0, 0, 0));
                    pBlackClose->setOpacity(0);
                    this->addChild(pBlackClose, 1000);
                    
                    callbackType = 1;
                    CCActionInterval* action = CCSequence::create( CCFadeIn::create(1.8f),
                            CCCallFuncND::create(this, callfuncND_selector(GameReady::EndSceneCallback), this), NULL);
                    pBlackClose->runAction(action);
                    
                    return true;
                }
                else
                {
                    // 포션 부족 - 구매창 유도 팝업
                    std::vector<int> nullData;
                    Common::ShowPopup(this, "GameReady", "NoImage", false, NEED_TO_BUY_POTION, BTN_2, nullData);
                    break;
                }
            }
        }
        else if (spriteClass->spriteObj[i]->name == "background/bg_board_brown.png2")
        {
            if (spriteClass->spriteObj[i]->sprite9->boundingBox().containsPoint(point))
            {
                sound->playClickboard();
                Common::ShowNextScene(this, "GameReady", "CocoRoom", false, 1);
                break;
            }
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_todaycandy.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                Common::ShowNextScene(this, "GameReady", "CocoRoom", false, 3);
                break;
            }
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_plus_big.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClickboard();
                // 슬롯 구매
                std::vector<int> data;
                if ((int)myInfo->GetSlot().size() >= (int)skillSlotInfo.size())
                    Common::ShowPopup(this, "Sketchbook", "NoImage", false, BUY_SKILLSLOT_FULL, BTN_1, data);
                else
                {
                    data.push_back((int)myInfo->GetSlot().size()+1);
                    data.push_back(SkillSlotInfo::GetCost((int)myInfo->GetSlot().size()+1));
                    if (SkillSlotInfo::GetCostType((int)myInfo->GetSlot().size()+1) == 1)
                        Common::ShowPopup(this, "Sketchbook", "NoImage", false, BUY_SKILLSLOT_BY_STARCANDY_TRY, BTN_2, data, 2);
                    else
                        Common::ShowPopup(this, "Sketchbook", "NoImage", false, BUY_SKILLSLOT_BY_TOPAZ_TRY, BTN_2, data, 2);
                }
                break;
            }
        }

    }
    
    if (fairyLayer->boundingBox().containsPoint(point))
    {
        sound->playClickboard();
        Common::ShowNextScene(this, "GameReady", "CocoRoom", false, 2);
    }
    else if (skillLayer->boundingBox().containsPoint(point))
    {
        sound->playClickboard();
        Common::ShowNextScene(this, "GameReady", "Sketchbook", false, 0);
    }
 
    return true;
}


void GameReady::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
}

void GameReady::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    if (isStarting)
        return;
    
    CCPoint point = pTouch->getLocation();
    
    if (!isScrolling && isScrollViewTouched && scrollViewSlot->boundingBox().containsPoint(point))
    {
        sound->playBoardMove(); // 이 scene만 사운드가 다르다.
        Common::ShowNextScene(this, "GameReady", "MagicList", false, 0);
    }
    
    isScrolling = false;
    isScrollViewTouched = false;
    isTouched = false;
}

void GameReady::EndScene()
{
}

void GameReady::EndSceneCallback(CCNode* sender, void* pointer)
{
    GameReady* pThis = (GameReady*)pointer;
    if (sender != NULL)
        sender->removeFromParentAndCleanup(true);
    
    pThis->unschedule(schedule_selector(GameReady::PotionTimer));
    
    // remove this notification
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(pThis, Depth::GetCurName());
    // release depth tree
    Depth::RemoveCurDepth();
    
    CCString* param;
    if (pThis->callbackType == 1) // 게임시작 버튼을 누른 경우
    {
        param = CCString::create("3");
        CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetCurName(), param);
    }
    
    // touch 넘겨주기 (GetCurName = 위에서 remove를 했기 때문에 결국 여기 입장에서는 부모다)
    param = CCString::create("0");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetCurName(), param);
    
    pThis->setKeypadEnabled(false);
    pThis->setTouchEnabled(false);
    
    // remove all CCNodes
    pThis->spriteClass->RemoveAllObjects();
    delete pThis->spriteClass;
    pThis->spriteClassProperty->RemoveAllObjects();
    delete pThis->spriteClassProperty;
    pThis->spriteClassFairy->RemoveAllObjects();
    delete pThis->spriteClassFairy;
    pThis->spriteClassSkill->RemoveAllObjects();
    delete pThis->spriteClassSkill;
    pThis->spriteClassSlot->RemoveAllObjects();
    delete pThis->spriteClassSlot;
    
    scrollViewSlot->getContainer()->removeAllChildren();
    scrollViewSlot->removeAllChildren();
    scrollViewSlot->removeFromParent();
    
    pThis->pBlack->removeFromParentAndCleanup(true);
    CCNode* pic = pThis->fairyLayer->getChildByTag(99999);
    if (pic != NULL)
        pic->removeAllChildren();
    pThis->fairyLayer->removeAllChildren();
    pThis->fairyLayer->removeFromParentAndCleanup(true);
    pThis->skillLayer->removeAllChildren();
    pThis->skillLayer->removeFromParentAndCleanup(true);
    
    
    CCTextureCache::sharedTextureCache()->removeTextureForKey("images/ranking_scrollbg.png");
    
    pThis->removeFromParentAndCleanup(true);
    
    if (pThis->callbackType == 1)
    {
        // Ranking도 끝낸다.
        param = CCString::create("4");
        CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetCurName(), param);
    }
}

void GameReady::scrollViewDidScroll(CCScrollView* view)
{
    isScrolling = true;
}

void GameReady::scrollViewDidZoom(CCScrollView* view)
{
}

