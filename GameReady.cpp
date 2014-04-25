#include "GameReady.h"
#include "Ranking.h"
#include "MagicList.h"

GameReady::~GameReady(void)
{
    // 게임준비 소멸자
//    CCLog("GameReady 소멸자");
}

CCScene* GameReady::scene()
{
    CCScene* pScene = CCScene::create();
    GameReady* pLayer = GameReady::create();
    pScene->addChild(pLayer);
    
	return pScene;
}

void GameReady::onEnter()
{
    //CCLog("GameReady :: onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
    CCLayer::onEnter();
}
void GameReady::onExit()
{
    //CCLog("GameReady :: onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
}


void GameReady::keyBackClicked()
{
    EndSceneCallback(this, this);
}


bool GameReady::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    // notification observer
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(GameReady::Notification), "GameReady", NULL);
    
    // notification
    CCString* param = CCString::create("1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification("Ranking", param);
    
    spriteClass = new SpriteClass();
    
    // item select init.
    for (int i = 0 ; i < 5 ; i++)
    {
        itemSelected[i] = false;
        itemNumLayer.push_back(new CCLayer());
    }
    
    InitSprites();
    MakeScroll();
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
    
    this->setKeypadEnabled(true);
    this->setTouchEnabled(true);
    
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
        this->setKeypadEnabled(true);
        this->setTouchEnabled(true);
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
        isTouched = false;
        
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
        CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
        this->setKeypadEnabled(false);
        this->setTouchEnabled(false);
    }
    else if (param->intValue() == 2)
    {
        // 토파즈, 별사탕, MP, 포션남은시간 정보 업데이트
        ((CCLabelTTF*)spriteClass->FindLabelByTag(1))->setString(Common::MakeComma(myInfo->GetTopaz()).c_str());
        ((CCLabelTTF*)spriteClass->FindLabelByTag(2))->setString(Common::MakeComma(myInfo->GetStarCandy()).c_str());
        ((CCLabelTTF*)spriteClass->FindLabelByTag(3))->setString(Common::MakeComma(myInfo->GetMPTotal()).c_str());
        ((CCLabelTTF*)spriteClass->FindLabelByTag(4))->setString(myInfo->GetRemainPotionTime().c_str());
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
    
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_topinfo.png1", ccp(0, 0), ccp(80, 1666), CCSize(230, 75), "", "GameReady", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_topinfo.png2", ccp(0, 0), ccp(390, 1666), CCSize(290, 75), "", "GameReady", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_topinfo.png3", ccp(0, 0), ccp(765, 1666), CCSize(290, 75), "", "GameReady", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png1", ccp(0, 0), ccp(49, 687), CCSize(982, 725), "", "GameReady", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png1", ccp(0, 0), ccp(77, 1126), CCSize(929, 268), "", "GameReady", this, 1) );
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png2", ccp(0, 0), ccp(77, 726), CCSize(929, 200), "", "GameReady", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png2", ccp(0, 0), ccp(92, 1143), CCSize(309, 236), "", "GameReady", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png3", ccp(0, 0), ccp(408, 1143), CCSize(309, 236), "", "GameReady", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png4", ccp(0, 0), ccp(725, 1143), CCSize(263, 236), "", "GameReady", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png5", ccp(0, 0), ccp(49, 458), CCSize(982, 223), "", "GameReady", this, 1) );
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png3", ccp(0, 0), ccp(77, 488), CCSize(782, 177), "", "GameReady", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_gameready_name.png1", ccp(0, 0), ccp(112, 1165), CCSize(274, 53), "", "GameReady", this, 1, 128) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_gameready_name.png2", ccp(0, 0), ccp(425, 1165), CCSize(274, 53), "", "GameReady", this, 1, 128) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_gameready_name.png3", ccp(0, 0), ccp(744, 1165), CCSize(228, 53), "", "GameReady", this, 1, 128) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_gameready_name.png4", ccp(0, 0), ccp(867, 497), CCSize(136, 63), "", "GameReady", this, 1) );
    
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_topaz.png", ccp(0, 0), ccp(15, 1656), CCSize(0, 0), "", "GameReady", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_starcandy.png", ccp(0, 0), ccp(317, 1660), CCSize(0, 0), "", "GameReady", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_magicpoint.png", ccp(0, 0), ccp(696, 1669), CCSize(0, 0), "", "GameReady", this, 5) );
    
    // property 문양 (mp 바로 밑에)
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
    
    // roof
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_roof.png1", ccp(0, 0), ccp(10, 1433), CCSize(0, 0), "", "GameReady", this, 1) );
    CCSize roofSize = spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->getContentSize();
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_roof.png2", ccp(0, 0), ccp(10+roofSize.width, 1433), CCSize(0, 0), "", "GameReady", this, 1) );
    spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->setFlipX(true);
    
    // 버튼들
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_topinfo_plus.png1", ccp(0, 0), ccp(80+230-55, 1679), CCSize(0, 0), "", "GameReady", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_topinfo_plus.png2", ccp(0, 0), ccp(390+290-55, 1679), CCSize(0, 0), "", "GameReady", this, 5) );
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
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0.5, 0.5), spriteClass->FindParentCenterPos(name), CCSize(0, 0), name, "0", NULL, 4, 1, 255) );
        else
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0.5, 0.5), spriteClass->FindParentCenterPos(name), CCSize(0, 0), name, "0", NULL, 4, 1, 0) );
    }
    
    // topaz
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(Common::MakeComma(myInfo->GetTopaz()), fontList[0], 36, ccp(0, 0), ccp(110, 1686), ccc3(255,255,255), "", "GameReady", this, 5, 0, 255, 1) );
    // starcandy
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(Common::MakeComma(myInfo->GetStarCandy()), fontList[0], 36, ccp(0, 0), ccp(420, 1686), ccc3(255,255,255), "", "GameReady", this, 5, 0, 255, 2) );
    // magic-point
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(Common::MakeComma(myInfo->GetMPTotal()), fontList[0], 36, ccp(0, 0), ccp(800, 1686), ccc3(255,255,255), "", "GameReady", this, 5, 0, 255, 3) );
    // potion-remain-time
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(myInfo->GetRemainPotionTime(), fontList[0], 36, ccp(0, 0), ccp(530, 1508), ccc3(255,255,255), "", "GameReady", this, 5, 0, 255, 4) );

    // strap, 제일 아래 버튼 등
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_blue.png", ccp(0, 0), ccp(14, 1343), CCSize(0, 0), "", "GameReady", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_title_gameready.png", ccp(0, 0), ccp(409, 1389), CCSize(0, 0), "", "GameReady", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_x_yellow.png", ccp(0, 0), ccp(875, 1391), CCSize(0, 0), "", "GameReady", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_grade_a.png", ccp(0, 0), ccp(408+11, 1143+165), CCSize(0, 0), "", "GameReady", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_petlevel.png", ccp(0, 0), ccp(408+55, 1143+187), CCSize(0, 0), "", "GameReady", this, 5) );
    
    // 5 items
    char name3[40];
    for (int i = 0 ; i < 5; i++)
    {
        sprintf(name, "background/bg_skill_brown.png%d", i+1);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(84+193*i, 972), CCSize(0, 0), "", "GameReady", this, 5) );

        sprintf(name2, "background/bg_skill_select.png%d", i+1);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0.5, 0.5), spriteClass->FindParentCenterPos(name), CCSize(0, 0), name, "0", NULL, 6, 1, 0) );
        spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->setTag(i+1);
        
        sprintf(name3, "icon/icon_check.png%d", i+1);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, name3, ccp(0, 0), ccp(100, 110), CCSize(0, 0), name2, "0", NULL, 6, 2, 0) );
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
            itemNumLayer[i] = Common::MakeItemNumberLayer(Common::MakeComma(myInfo->GetItem(i)));
            itemNumLayer[i]->setPosition(ccp(84+193*i+130, 965));
            this->addChild(itemNumLayer[i], 10);
        }
        else
        {
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_starcandy_mini.png", ccp(0, 0), ccp(84+193*i-15, 925), CCSize(0, 0), "", "GameReady", this, 5) );
            spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("1200", fontList[0], 36, ccp(0, 0), ccp(84+193*i+50, 935), ccc3(255,255,255), "", "GameReady", this, 5) );
        }
    }
    
    // 슬롯 추가 버튼
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_plus_big.png", ccp(0, 0), ccp(896, 572), CCSize(0, 0), "", "GameReady", this, 5) );
    
    // 게임시작 버튼
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_blue.png", ccp(0, 0), ccp(318, 193), CCSize(0, 0), "", "GameReady", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_gamestart.png", ccp(0.5, 0.5), ccp(319+446/2, 191+160/2+5), CCSize(0, 0), "", "GameReady", this, 5) );
    
    // 오늘의 별사탕 버튼
    //
    //ccp(912, 1920-1582-86)
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_todaycandy.png", ccp(0, 0), ccp(886-50, 1920-1674-44), CCSize(0, 0), "", "GameReady", this, 5) );
    
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("지팡이 24Lv", fontList[0], 30, ccp(0.5, 0.5), ccp(250, 1190), ccc3(121,71,0), "", "GameReady", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("지팡이 24Lv", fontList[0], 30, ccp(0.5, 0.5), ccp(250, 1193), ccc3(255,219,53), "", "GameReady", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("특수능력없음", fontList[0], 30, ccp(0.5, 0.5), ccp(550, 1190), ccc3(121,71,0), "", "GameReady", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("특수능력없음", fontList[0], 30, ccp(0.5, 0.5), ccp(550, 1193), ccc3(255,219,53), "", "GameReady", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("따뜻한 불꽃그림", fontList[0], 30, ccp(0.5, 0.5), ccp(860, 1190), ccc3(121,71,0), "", "GameReady", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("따뜻한 불꽃그림", fontList[0], 30, ccp(0.5, 0.5), ccp(860, 1193), ccc3(255,255,255), "", "GameReady", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabelArea("행복하지 않을 이유가 하나도 없습니다.", fontList[0], 36, ccp(0, 0), ccp(97, 736), ccc3(0,0,0), CCSize(909, 180), kCCTextAlignmentLeft, kCCVerticalTextAlignmentCenter, "", "GameReady", this, 5) );
}

void GameReady::MakeScroll()
{
    // make scroll
    CCLayer* scrollContainer = CCLayer::create();
    scrollContainer->setAnchorPoint(ccp(0, 0));
    scrollContainer->setPosition(ccp(100, 500));
    int numOfSlots = 7;
    
    for (int i = 0 ; i < numOfSlots ; i++)
    {
        CCSprite* temp = CCSprite::createWithSpriteFrameName("background/bg_skill_yellow.png");
        temp->setAnchorPoint(ccp(0, 0));
        temp->setPosition(ccp(146*i, 0));
        scrollContainer->addChild(temp, 5);
    }
    
    scrollContainer->setContentSize(CCSizeMake(146*numOfSlots, 146));

    scrollView = CCScrollView::create();
    scrollView->retain();
    scrollView->setDirection(kCCScrollViewDirectionHorizontal);
    scrollView->setViewSize(CCSizeMake(782-38, 146));
    scrollView->setContentSize(scrollContainer->getContentSize());
    scrollView->setAnchorPoint(ccp(0, 0));
    scrollView->setPosition(ccp(100, 500));
    scrollView->setContainer(scrollContainer);
    scrollView->setDelegate(this);
    //scrollView->setContentOffset(ccp(-(numOfSlots-5)*146, 0), false);
    //scrollView->setContentOffset(ccp((numOfSlots)*146, 0), false);
    this->addChild(scrollView, 4);
}


bool GameReady::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    if (isTouched || isStarting)
        return false;
    isTouched = true;
    isScrolling = false;
    
    CCPoint point = pTouch->getLocation();
    CCLog("GameReady : (%d , %d)", (int)point.x, (int)point.y);
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        if (spriteClass->spriteObj[i]->name == "button/btn_x_yellow.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                
                callbackType = 0;
                EndSceneCallback(this, this);
                return true;
            }
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_topinfo_plus.png1")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClickboard();
                Common::ShowNextScene(this, "GameReady", "BuyTopaz", false, 1);
            }
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_topinfo_plus.png2")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClickboard();
                Common::ShowNextScene(this, "GameReady", "BuyStarCandy", false, 1);
            }
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_topinfo_plus.png3")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClickboard();
                Common::ShowNextScene(this, "GameReady", "BuyPotion", false, 1);
            }
        }

        else if (spriteClass->spriteObj[i]->name.substr(0, 29) == "background/bg_skill_brown.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                char n1[35], n2[35];
                std::string name = spriteClass->spriteObj[i]->name;
                sprintf(n1, "background/bg_skill_select.png%c", name[name.size()-1]);
                sprintf(n2, "icon/icon_check.png%c", name[name.size()-1]);
                
                int tag = name[name.size()-1] - '0';
                itemSelected[tag-1] = !itemSelected[tag-1];
                
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
                isStarting = true;
                
                sound->playClick();

                pBlackClose = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, winSize.width, winSize.height));
                pBlackClose->setPosition(ccp(0, 0));
                pBlackClose->setAnchorPoint(ccp(0, 0));
                pBlackClose->setColor(ccc3(0, 0, 0));
                pBlackClose->setOpacity(0);
                this->addChild(pBlackClose, 1000);
                
                callbackType = 1;
                CCActionInterval* action = CCSequence::create( CCFadeIn::create(1.0f),
                        CCCallFuncND::create(this, callfuncND_selector(GameReady::EndSceneCallback), this), NULL);
                pBlackClose->runAction(action);
                
                return true;
                //Common::ShowNextScene(this, "GameReady", "Puzzle", true);
            }
        }
        else if (spriteClass->spriteObj[i]->name == "background/bg_board_brown.png2")
        {
            if (spriteClass->spriteObj[i]->sprite9->boundingBox().containsPoint(point))
            {
                sound->playClickboard();
                Common::ShowNextScene(this, "GameReady", "CocoRoom", false, 0);
            }
        }
        else if (spriteClass->spriteObj[i]->name == "background/bg_board_brown.png3")
        {
            if (spriteClass->spriteObj[i]->sprite9->boundingBox().containsPoint(point))
            {
                sound->playClickboard();
                Common::ShowNextScene(this, "GameReady", "CocoRoom", false, 1);
            }
        }
        else if (spriteClass->spriteObj[i]->name == "background/bg_board_brown.png4")
        {
            if (spriteClass->spriteObj[i]->sprite9->boundingBox().containsPoint(point))
            {
                sound->playClickboard();
                Common::ShowNextScene(this, "GameReady", "Sketchbook", false, 0);
            }
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_todaycandy.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                Common::ShowNextScene(this, "GameReady", "CocoRoom", false, 2);
            }
        }
    }
 
    return true;
}


void GameReady::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
    //CCPoint point = pTouch->getLocation();
}

void GameReady::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    if (isStarting)
        return;
    
    CCPoint point = pTouch->getLocation();
    
    if (!isScrolling && scrollView->boundingBox().containsPoint(point))
    {
        sound->playBoardMove(); // 이 scene만 사운드가 다르다.
        Common::ShowNextScene(this, "GameReady", "MagicList", false);
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
    
    CCString* param;
    if (pThis->callbackType == 1) // 게임시작 버튼을 누른 경우
        param = CCString::create("3");
    else // x 버튼을 눌러 GameReady만 끈 경우
        param = CCString::create("0");
    CCNotificationCenter::sharedNotificationCenter()->postNotification("Ranking", param);
    
    pThis->setKeypadEnabled(false);
    pThis->setTouchEnabled(false);
    
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(pThis, "GameReady");
    
    // remove all CCNodes
    spriteClass->RemoveAllObjects();
    delete spriteClass;
    
    // itemLayer
    for (int i = 0 ; i < itemNumLayer.size() ; i++)
    {
        itemNumLayer[i]->removeAllChildren();
        CCLog("ii : %d", itemNumLayer[i]->retainCount());
    }
    itemNumLayer.clear();
    
    // background layer
    CCTextureCache::sharedTextureCache()->removeTextureForKey("images/ranking_scrollbg.png");
    CCLog("pBlack %d", pBlack->retainCount());
    //CCLog("pBlackClose %d", pBlackClose->retainCount());
    
    scrollView->getContainer()->removeAllChildren();
    scrollView->removeAllChildren();
    scrollView->removeFromParent();
    //CCLog("%d", scrollView->retainCount());
    
    //CCNode* parent = pThis->getParent();
    pThis->removeFromParentAndCleanup(true);
    
    if (pThis->callbackType == 1)
    {
        // Ranking도 끝낸다.
        param = CCString::create("4");
        CCNotificationCenter::sharedNotificationCenter()->postNotification("Ranking", param);
    }
}

void GameReady::scrollViewDidScroll(CCScrollView* view)
{
    isScrolling = true;
}

void GameReady::scrollViewDidZoom(CCScrollView* view)
{
}