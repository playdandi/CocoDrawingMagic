#include "Sketchbook.h"

static int tabNumber;

CCScene* Sketchbook::scene(int tab)
{
    tabNumber = tab;
    
    CCScene* pScene = CCScene::create();
    Sketchbook* pLayer = Sketchbook::create();
    pScene->addChild(pLayer);
    
	return pScene;
}

void Sketchbook::onEnter()
{
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
    CCLayer::onEnter();
}
void Sketchbook::onExit()
{
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
}

void Sketchbook::keyBackClicked()
{
    EndScene();
}


bool Sketchbook::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    // notification post
    CCString* param = CCString::create("1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification("Ranking", param);
    
    // layer init.
    fire = CCLayer::create();
    water = CCLayer::create();
    land = CCLayer::create();
    master = CCLayer::create();
    fire->setPosition(ccp(0, 0));
    water->setPosition(ccp(0, 0));
    land->setPosition(ccp(0, 0));
    master->setPosition(ccp(0, 0));
    this->addChild(fire, 5);
    this->addChild(water, 5);
    this->addChild(land, 5);
    this->addChild(master, 5);
    
    // scrollview init.
    scrollViewFire = CCScrollView::create();
    scrollViewFire->retain();
    scrollViewWater = CCScrollView::create();
    scrollViewWater->retain();
    scrollViewLand = CCScrollView::create();
    scrollViewLand->retain();
    scrollViewMaster = CCScrollView::create();
    scrollViewMaster->retain();
    scrollViewSlot = CCScrollView::create();
    scrollViewSlot->retain();
    
    spriteClass = new SpriteClass();
    spriteClassFire = new SpriteClass();
    spriteClassWater = new SpriteClass();
    spriteClassLand = new SpriteClass();
    spriteClassMaster = new SpriteClass();
    
    InitSprites();
    MakeScrollSlot();
    
    curState = -1;
    MakeScroll(tabNumber);
    
    isTouched = false;
    isScrolling = false;
    isScrollViewTouched = false;
    
    return true;
}

void Sketchbook::InitSprites()
{
    CCSprite* pBlack = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, winSize.width, winSize.height));
    pBlack->setPosition(ccp(0, 0));
    pBlack->setAnchorPoint(ccp(0, 0));
    pBlack->setColor(ccc3(0, 0, 0));
    pBlack->setOpacity(150);
    this->addChild(pBlack, 0);
    
    // strap
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_purple.png",
                    ccp(0, 0), ccp(14, 1586), CCSize(0, 0), "", "Sketchbook", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_x_yellow.png",
                    ccp(0, 0), ccp(875, 1634), CCSize(0, 0), "", "Sketchbook", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_title_sketchbook.png",
                    ccp(0, 0), ccp(279, 1632), CCSize(0, 0), "", "Sketchbook", this, 2) );
    
    // select menu
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_sketchbook_select.png1",
                    ccp(0, 0), ccp(100, 1416), CCSize(0, 0), "", "Sketchbook", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_sketchbook_select.png2",
                    ccp(0, 0), ccp(296, 1416), CCSize(0, 0), "", "Sketchbook", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_sketchbook_select.png3",
                    ccp(0, 0), ccp(492, 1416), CCSize(0, 0), "", "Sketchbook", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_sketchbook_select.png4",
                    ccp(0, 0), ccp(688, 1416), CCSize(0, 0), "", "Sketchbook", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_property_fire.png",
                    ccp(0, 0), ccp(153, 1460), CCSize(0, 0), "", "Sketchbook", this, 3) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_property_water.png",
                    ccp(0, 0), ccp(350, 1461), CCSize(0, 0), "", "Sketchbook", this, 3) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_property_land.png",
                    ccp(0, 0), ccp(534, 1464), CCSize(0, 0), "", "Sketchbook", this, 3) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_property_master.png",
                    ccp(0, 0), ccp(737, 1463), CCSize(0, 0), "", "Sketchbook", this, 3) );
    
    // background
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png1",
                    ccp(0, 0), ccp(49, 458), CCSize(982, 954+20), "", "Sketchbook", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png1",
                    ccp(0, 0), ccp(77, 492), CCSize(929, 904+10), "", "Sketchbook", this, 1) );
    
    // slot part
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png2",
                    ccp(0, 0), ccp(49, 198), CCSize(982, 223), "", "Sketchbook", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png2",
                    ccp(0, 0), ccp(77, 228), CCSize(782, 177), "", "Sketchbook", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_plus_big.png", // slot plus
                    ccp(0, 0), ccp(896, 317), CCSize(0, 0), "", "Sketchbook", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_gameready_name.png0",
                    ccp(0, 0), ccp(867, 242), CCSize(136, 63), "", "Sketchbook", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("4", fontList[0], 48, ccp(0, 0),
                    ccp(892, 248), ccc3(255,219,53), "", "Sketchbook", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("/ 7", fontList[0], 36, ccp(0, 0),
                    ccp(927, 248), ccc3(182,142,142), "", "Sketchbook", this, 5) );
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
}

void Sketchbook::MakeScroll(int state)
{
    // select button을 눌렀을 때 이 함수를 통해 어떤 layer를 보여줄 지 결정한다.
    if (curState == state)
        return;
    
    // 초기화
    fire->removeAllChildren();
    spriteClassFire->spriteObj.clear();
    water->removeAllChildren();
    spriteClassWater->spriteObj.clear();
    land->removeAllChildren();
    spriteClassLand->spriteObj.clear();
    master->removeAllChildren();
    spriteClassMaster->spriteObj.clear();
    
    if (state == 0)
        MakeScrollFire();
    else if (state == 1)
        MakeScrollWater();
    else if (state == 2)
        MakeScrollLand();
    else
        MakeScrollMaster();
    
    curState = state;
}

void Sketchbook::MakeScrollFire()
{
    int numOfList = 8;
    type.push_back(0);
    type.push_back(1);
    type.push_back(2);
    type.push_back(3);
    type.push_back(3);
    type.push_back(2);
    type.push_back(1);
    type.push_back(0);
    
    containerFire = CCLayer::create();
    containerFire->setPosition(ccp(0, 0));
    
    char name[50], name2[50];
    for (int i = 0 ; i < numOfList; i++)
    {
        CCLayer* itemLayer = CCLayer::create();
        itemLayer->setContentSize(CCSizeMake(872, 196));
        itemLayer->setPosition(ccp(34, (numOfList-i-1)*196));
        containerFire->addChild(itemLayer, 5);
        
        sprintf(name, "background/bg_board_brown.png%d", i);
        spriteClassFire->spriteObj.push_back( SpriteObject::Create(1, name,
                        ccp(0, 0), ccp(0, 0), CCSize(872, 196), "", "Layer", itemLayer, 5) );
        sprintf(name, "icon/icon_skill_division_red.png%d", i);
        spriteClassFire->spriteObj.push_back( SpriteObject::Create(0, name,
                        ccp(0, 0), ccp(25, 122), CCSize(0, 0), "", "Layer", itemLayer, 5) );
        sprintf(name, "background/bg_skill_brown.png%d", i);
        spriteClassFire->spriteObj.push_back( SpriteObject::Create(0, name,
                        ccp(0, 0), ccp(25, 41), CCSize(0, 0), "", "Layer", itemLayer, 5) );
        sprintf(name2, "skill/skill_orange_%d.png", i+1);
        spriteClassFire->spriteObj.push_back( SpriteObject::Create(0, name2,
                        ccp(0, 0), ccp(25+8, 41+8), CCSize(0, 0), "", "Layer", itemLayer, 5) );
        sprintf(name, "icon/icon_auto_effect.png%d", i);
        spriteClassFire->spriteObj.push_back( SpriteObject::Create(0, name,
                        ccp(0, 0), ccp(114, 153), CCSize(0, 0), "", "Layer", itemLayer, 6) );
        sprintf(name, "background/bg_gameready_name.png%d", i);
        spriteClassFire->spriteObj.push_back( SpriteObject::Create(1, name,
                        ccp(0, 0), ccp(169, 106), CCSize(442, 69), "", "Layer", itemLayer, 5) );
        
        spriteClassFire->spriteObj.push_back( SpriteObject::CreateLabel("따뜻한 불꽃그림", fontList[0], 48, ccp(0.5, 0.5), spriteClassFire->FindParentCenterPos(name), ccc3(255,255,255), name, "1", NULL, 5) );
        spriteClassFire->spriteObj.push_back( SpriteObject::CreateLabel("주변 빨간구슬을 터뜨리면 점수상승", fontList[0], 30, ccp(0, 0), ccp(169, 96-26), ccc3(255,255,255), "", "Layer", itemLayer, 5) );
        spriteClassFire->spriteObj.push_back( SpriteObject::CreateLabel("16", fontList[2], 30, ccp(0, 0), ccp(546, 37), ccc3(255,255,255), "", "Layer", itemLayer, 5) );
        spriteClassFire->spriteObj.push_back( SpriteObject::CreateLabel("/20", fontList[2], 30, ccp(0, 0), ccp(582, 25), ccc3(182,142,142), "", "Layer", itemLayer, 5) );
        
        sprintf(name, "background/bg_progress_bar.png%d", i);
        spriteClassFire->spriteObj.push_back( SpriteObject::Create(1, name,
                        ccp(0, 0), ccp(130, 30), CCSize(412, 31), "", "Layer", itemLayer, 5) );
        
        if (type[i] == 0)
        {
            sprintf(name, "button/btn_red_mini.png%d", i);
            spriteClassFire->spriteObj.push_back( SpriteObject::Create(0, name,
                            ccp(0, 0), ccp(633, 46), CCSize(0, 0), "", "Layer", itemLayer, 5) );
            sprintf(name2, "letter/letter_practice.png%d", i);
            spriteClassFire->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0.5, 0),
                            ccp(spriteClassFire->spriteObj[spriteClassFire->spriteObj.size()-1]->sprite->
                            getContentSize().width/2, 27), CCSize(0, 0), name, "0", NULL, 5) );
        }
        else if (type[i] == 1)
        {
            sprintf(name, "button/btn_skill_master.png%d", i);
            spriteClassFire->spriteObj.push_back( SpriteObject::Create(0, name,
                            ccp(0, 0), ccp(633, 61), CCSize(213, 95), "", "Layer", itemLayer, 5) );
            // master 글자는 지금 없음...
            sprintf(name2, "letter/letter_master.png%d", i);
            spriteClassFire->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0.5, 0),
                            ccp(spriteClassFire->spriteObj[spriteClassFire->spriteObj.size()-1]->sprite->
                            getContentSize().width/2, 30), CCSize(0, 0), name, "0", NULL, 5) );
        }
        else if (type[i] == 2)
        {
            sprintf(name, "button/btn_red_mini.png%d", i);
            spriteClassFire->spriteObj.push_back( SpriteObject::Create(0, name,
                            ccp(0, 0), ccp(633, 46), CCSize(0, 0), "", "Layer", itemLayer, 5) );
            sprintf(name2, "icon/icon_lock_white.png%d", i);
            spriteClassFire->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0.5, 0),
                            ccp(spriteClassFire->spriteObj[spriteClassFire->spriteObj.size()-1]->sprite->
                            getContentSize().width/2, 27), CCSize(0, 0), name, "0", NULL, 5) );
            sprintf(name2, "icon/icon_levelup.png%d", i);
            spriteClassFire->spriteObj.push_back( SpriteObject::Create(0, name2,
                            ccp(0, 0), ccp(-4, 73), CCSize(0, 0), name, "0", NULL, 5) );
            
        }
        else if (type[i] == 3)
        {
            sprintf(name, "button/btn_green_mini.png%d", i);
            spriteClassFire->spriteObj.push_back( SpriteObject::Create(0, name,
                            ccp(0, 0), ccp(633, 46), CCSize(0, 0), "", "Layer", itemLayer, 5) );
            sprintf(name2, "button/btn_receive_magicpoint.png%d", i);
            spriteClassFire->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0, 0),
                            ccp(25, 36), CCSize(0, 0), name, "0", NULL, 5) );
            spriteClassFire->spriteObj.push_back( SpriteObject::CreateLabel("30", fontList[0], 36, ccp(0, 0), ccp(115, 40), ccc3(78,47,8), name, "0", NULL, 5) );
        }
    }
    
    for (int i = 0 ; i < spriteClassFire->spriteObj.size() ; i++)
        spriteClassFire->AddChild(i);
    
    // scrollview 내용 전체크기
    containerFire->setContentSize(CCSizeMake(872, numOfList*196));
    // scrollView 생성
    //scrollViewFire = CCScrollView::create();
    scrollViewFire->retain();
    scrollViewFire->setDirection(kCCScrollViewDirectionVertical);
    scrollViewFire->setViewSize(CCSizeMake(929, 904-50)); // (내용 1개 크기, 노란보드 세로크기)
    scrollViewFire->setContentSize(containerFire->getContentSize());
    scrollViewFire->setAnchorPoint(ccp(0, 0));
    scrollViewFire->setPosition(ccp(77, 492+30));
    scrollViewFire->setContainer(containerFire);
    scrollViewFire->setDelegate(this);
    scrollViewFire->setContentOffset(ccp(0, 904-50-(numOfList*196)), false);
    fire->addChild(scrollViewFire, 5);
}

void Sketchbook::MakeScrollWater()
{

}
void Sketchbook::MakeScrollLand()
{
    
}
void Sketchbook::MakeScrollMaster()
{
    
}

void Sketchbook::MakeScrollSlot()
{
    // make scroll
    CCLayer* scrollContainer = CCLayer::create();
    scrollContainer->setPosition(ccp(91, 242));
    int numOfSlots = 7;

    for (int i = 0 ; i < numOfSlots ; i++)
    {
        CCSprite* temp = CCSprite::createWithSpriteFrameName("background/bg_skill_yellow.png");
        temp->setAnchorPoint(ccp(0, 0));
        temp->setPosition(ccp(146*i, 0));
        scrollContainer->addChild(temp, 500);
    }
    
    scrollContainer->setContentSize(CCSizeMake(146*numOfSlots, 146));
    
    //scrollViewSlot = CCScrollView::create();
    scrollViewSlot->retain();
    scrollViewSlot->setDirection(kCCScrollViewDirectionHorizontal);
    //scrollViewSlot->setViewSize(CCSizeMake(782-38, 146));
    scrollViewSlot->setContentSize(scrollContainer->getContentSize());
    
    scrollViewSlot->setViewSize(CCSizeMake(782, 177)); //782-38, 146));
    scrollViewSlot->setAnchorPoint(ccp(0, 0));
    scrollViewSlot->setPosition(ccp(77, 228));
    
    //scrollViewSlot->setAnchorPoint(ccp(0, 0));
    //scrollViewSlot->setPosition(ccp(100, 500));
    scrollViewSlot->setContainer(scrollContainer);
    scrollViewSlot->setDelegate(this);
    scrollViewSlot->setContentOffset(ccp(0, 0), false);
    //scrollView->setContentOffset(ccp(-(numOfSlots-5)*146, 0), false);
    //scrollView->setContentOffset(ccp((numOfSlots)*146, 0), false);
    this->addChild(scrollViewSlot, 50);
}


bool Sketchbook::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    if (isTouched)
        return false;
    isTouched = true;
    isScrolling = false;
    isScrollViewTouched = false;
    
    CCPoint point = pTouch->getLocation();
    
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        if (spriteClass->spriteObj[i]->name == "background/bg_sketchbook_select.png1")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClickboard();
                MakeScroll(0);
            }
        }
        else if (spriteClass->spriteObj[i]->name == "background/bg_sketchbook_select.png2")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClickboard();
                MakeScroll(1);
            }
        }
        else if (spriteClass->spriteObj[i]->name == "background/bg_sketchbook_select.png3")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClickboard();
                MakeScroll(2);
            }
        }
        else if (spriteClass->spriteObj[i]->name == "background/bg_sketchbook_select.png4")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClickboard();
                MakeScroll(3);
            }
        }
        
        else if (spriteClass->spriteObj[i]->name == "button/btn_x_yellow.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
                EndScene();
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_plus_big.png")
        {
            if (curState == 0 && spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
                ; // 슬롯 구매
            else if (curState == 1 && spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
                ; // 요정의 마을
        }
    }
    
    return true;
}


void Sketchbook::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
    //CCPoint point = pTouch->getLocation();
}

void Sketchbook::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    //CCPoint point = pTouch->getLocation();
    isTouched = false;
    isScrolling = false;
    isScrollViewTouched = false;
}


void Sketchbook::scrollViewDidScroll(CCScrollView* view)
{
    isScrolling = true;
    //CCLog("Sketchbook - scrolling~");
}

void Sketchbook::scrollViewDidZoom(CCScrollView* view)
{
}


void Sketchbook::EndScene()
{
    sound->playClick();
    CCString* param = CCString::create("0");
    CCNotificationCenter::sharedNotificationCenter()->postNotification("Ranking", param);
    
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    
    scrollViewFire->removeAllChildren();
    scrollViewFire->removeFromParent();
    scrollViewWater->removeAllChildren();
    scrollViewWater->removeFromParent();
    scrollViewLand->removeAllChildren();
    scrollViewLand->removeFromParent();
    scrollViewMaster->removeAllChildren();
    scrollViewMaster->removeFromParent();
    scrollViewSlot->removeAllChildren();
    scrollViewSlot->removeFromParent();
    
    this->removeFromParentAndCleanup(true);
}

void Sketchbook::EndSceneCallback()
{
}


