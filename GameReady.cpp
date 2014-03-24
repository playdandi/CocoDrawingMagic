#include "GameReady.h"
#include "Ranking.h"
#include "MagicList.h"

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
    EndScene();
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
    
    for (int i = 0 ; i < 5 ; i++)
        itemSelected[i] = false;
    
    InitSprites();
    MakeScroll();
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
    
    this->setKeypadEnabled(true);
    this->setTouchEnabled(true);
    
    isTouched = false;
    isScrollViewTouched = false;
    isScrolling = false;
    
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
    }
    else if (param->intValue() == 1)
    {
        // 터치 비활성
        CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
        this->setKeypadEnabled(false);
        this->setTouchEnabled(false);
    }
}


void GameReady::InitSprites()
{
    CCSprite* pBlack = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, winSize.width, winSize.height));
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
    
    
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_topaz.png", ccp(0, 0), ccp(15, 1656), CCSize(0, 0), "", "Ranking", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_starcandy.png", ccp(0, 0), ccp(317, 1660), CCSize(0, 0), "", "Ranking", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_magicpoint.png", ccp(0, 0), ccp(696, 1669), CCSize(0, 0), "", "Ranking", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_property.png1", ccp(0, 0), ccp(848, 1611), CCSize(0, 0), "", "Ranking", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_property.png2", ccp(0, 0), ccp(904, 1611), CCSize(0, 0), "", "Ranking", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_property.png3", ccp(0, 0), ccp(959, 1611), CCSize(0, 0), "", "Ranking", this, 5) );
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_roof.png1", ccp(0, 0), ccp(10, 1433), CCSize(0, 0), "", "Ranking", this, 1) );
    CCSize roofSize = spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->getContentSize();
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_roof.png2", ccp(0, 0), ccp(10+roofSize.width, 1433), CCSize(0, 0), "", "Ranking", this, 1) );
    spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->setFlipX(true);
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_topinfo_plus.png1", ccp(0, 0), ccp(80+230-55, 1679), CCSize(0, 0), "", "Ranking", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_topinfo_plus.png2", ccp(0, 0), ccp(390+290-55, 1679), CCSize(0, 0), "", "Ranking", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_potion_time.png", ccp(0, 0), ccp(506, 1493), CCSize(0, 0), "", "Ranking", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_topinfo_plus.png3", ccp(0, 0), ccp(645, 1498), CCSize(0, 0), "", "Ranking", this, 5) );
    
    // make potion
    char name[40], name2[40];
    int numOfPotion = 3;
    for (int i = 0; i < 5; i++)
    {
        sprintf(name, "icon/icon_potion_empty.png%d", i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(89+83*i, 1480), CCSize(0, 0), "", "Ranking", this, 4) );
        if (i < numOfPotion)
        {
            sprintf(name2, "icon/icon_potion.png%d", i);
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0.5, 0.5), spriteClass->FindParentCenterPos(name), CCSize(0, 0), name, "0", NULL, 4) );
        }
    }
    
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
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0.5, 0.5), spriteClass->FindParentCenterPos(name), CCSize(0, 0), name, "0", NULL, 6, 0) );
        spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->setTag(i+1);
        
        sprintf(name3, "icon/icon_check.png%d", i+1);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, name3, ccp(0, 0), ccp(100, 110), CCSize(0, 0), name2, "0", NULL, 6, 0) );
        spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->setTag(i+1);
    }
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_plus_big.png", ccp(0, 0), ccp(896, 572), CCSize(0, 0), "", "GameReady", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_blue.png", ccp(0, 0), ccp(318, 193), CCSize(0, 0), "", "GameReady", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_gamestart.png", ccp(0.5, 0.5), ccp(319+446/2, 191+160/2+5), CCSize(0, 0), "", "GameReady", this, 5) );
    
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
    if (isTouched)
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
                EndScene();
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
                sound->playClick();
                Common::ShowNextScene(this, "GameReady", "Puzzle", true);
                //Common::ShowNextScene(this, "GameReady", "ParticleTest", false);
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
    }
    
    
    /*
    if (903 <= (int)point.x && (int)point.x <= 1004 &&
        1504 <= (int)point.y && (int)point.y <= 1591)
    {
        EndLayer();
    }
    else if (54 <= (int)point.x && (int)point.x <= 879 &&
        454 <= (int)point.y && (int)point.y <= 642)
    {
        // slot part
        ShowMagicSlot();
    }
    
    else if (pItemNormal->boundingBox().containsPoint(point))
    {
        // item 부분 클릭
        bItem = !bItem;
        if (bItem)
        {
            pItemClicked->setOpacity(255);
            desc->setString("[마법 폭탄] 양방향 대각선으로 4개씩 총 8칸이 터집니다. 히힛~");
        }
        else
        {
            pItemClicked->setOpacity(0);
            desc->setString("[마법 폭탄] 양방향 대각선으로 4개씩 총 8칸이 터집니다. 히힛~");
        }
    }
    */
    return true;
}


void GameReady::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
    //CCPoint point = pTouch->getLocation();
}

void GameReady::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
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
    sound->playClick();
    
    CCString* param = CCString::create("0");
    CCNotificationCenter::sharedNotificationCenter()->postNotification("Ranking", param);
    
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, "GameReady");
    
    scrollView->removeAllChildren();
    scrollView->removeFromParent();
    
    this->removeFromParentAndCleanup(true);
    
    /*
    CCFiniteTimeAction* action =
    CCSequence::create(CCFadeOut::create(0.1f),
                       CCCallFunc::create(this, callfunc_selector(GameReady::EndLayerCallback)), NULL);
    pBackground->runAction((CCActionInterval*)action->copy()->autorelease());
    pItemNormal->runAction((CCActionInterval*)action->copy()->autorelease());
    if (bItem)
        pItemClicked->runAction((CCActionInterval*)action->copy()->autorelease());
     */
}

void GameReady::EndSceneCallback()
{
    /*
    this->removeChild(pItemNormal);
    this->removeChild(pItemClicked);
    this->removeChild(pBackground);
    this->removeChild(pDescription);
    this->removeChild(pBlack);
    pItemNormal->autorelease();
    pItemClicked->autorelease();
    pBackground->autorelease();
    pDescription->autorelease();
    
    CCTextureCache::sharedTextureCache()->removeTextureForKey("images/gameready.png");
    CCTextureCache::sharedTextureCache()->removeTextureForKey("images/gameready_description.png");
    CCTextureCache::sharedTextureCache()->removeTextureForKey("images/gameready_item_normal.png");
    CCTextureCache::sharedTextureCache()->removeTextureForKey("images/gameready_item_clicked.png");
    
    this->removeFromParentAndCleanup(true);
     */
}

void GameReady::scrollViewDidScroll(CCScrollView* view)
{
    isScrolling = true;
}

void GameReady::scrollViewDidZoom(CCScrollView* view)
{
}