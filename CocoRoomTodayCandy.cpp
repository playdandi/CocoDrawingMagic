#include "CocoRoomTodayCandy.h"

//static int priority;

CCScene* CocoRoomTodayCandy::scene(int prio)
{
    //priority = prio;
    
    CCScene* pScene = CCScene::create();
    CocoRoomTodayCandy* pLayer = CocoRoomTodayCandy::create();
    pScene->addChild(pLayer);
    
	return pScene;
}

void CocoRoomTodayCandy::onEnter()
{
    CCLog("CocoRoomTodayCandy :: onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority(), true);
    CCLayer::onEnter();
}
void CocoRoomTodayCandy::onExit()
{
    CCLog("CocoRoomTodayCandy :: onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
}

void CocoRoomTodayCandy::keyBackClicked()
{
    EndScene();
}


bool CocoRoomTodayCandy::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
    // make depth tree
    Depth::AddCurDepth("CocoRoomTodayCandy");
    
    this->setTouchEnabled(true);
    this->setKeypadEnabled(true);
    this->setTouchPriority(Depth::GetCurPriority());
    CCLog("CocoRoomTodayCandy : touch prio = %d", this->getTouchPriority());
    
    // notification
    CCString* param = CCString::create("1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetParentName(), param);
    
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    // scrollView 생성
    scrollView = CCScrollView::create();
    scrollView->retain();
    scrollView->setDirection(kCCScrollViewDirectionVertical);
    scrollView->setViewSize(CCSizeMake(929, 904-80));
    scrollView->setAnchorPoint(ccp(0, 0));
    scrollView->setPosition(ccp(77, 492+40));
    scrollView->setDelegate(this);
    scrollView->setTouchPriority(Depth::GetCurPriority());
    this->addChild(scrollView, 3);
    
    InitSprites();
    MakeScroll();
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
    
    isTouched = false;
    isScrollViewTouched = false;
    isScrolling = false;
    
    return true;
}

void CocoRoomTodayCandy::InitSprites()
{
    pBlack = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, winSize.width, winSize.height));
    pBlack->setPosition(ccp(0, 0));
    pBlack->setAnchorPoint(ccp(0, 0));
    pBlack->setColor(ccc3(0, 0, 0));
    pBlack->setOpacity(150);
    this->addChild(pBlack, 0);
    
    spriteClass = new SpriteClass();

    // make pop-up background
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png1",
                    ccp(0, 0), ccp(49, 458), CCSize(982, 1073), "", "CocoRoomTodayCandy", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png1",
                    ccp(0, 0), ccp(75, 492), CCSize(929, 904), "", "CocoRoomTodayCandy", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_todaystarcandy_groupfriend_select.png", ccp(0, 0), ccp(103, 1429), CCSize(0, 0), "", "CocoRoomTodayCandy", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_x_brown.png",
                    ccp(0, 0), ccp(900, 1420), CCSize(0, 0), "", "CocoRoomTodayCandy", this, 1) );
    
    // bottom background
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png2",
                    ccp(0, 0), ccp(49, 198), CCSize(982, 223), "", "CocoRoom", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png2",
                    ccp(0, 0), ccp(77, 228), CCSize(929, 177), "", "CocoRoom", this, 1) );
    
    // bottom 5 profiles
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_profile.png1",
                    ccp(0, 0), ccp(100, 254), CCSize(0, 0), "", "CocoRoom", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_profile.png2",
                    ccp(0, 0), ccp(229, 254), CCSize(0, 0), "", "CocoRoom", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_profile.png3",
                    ccp(0, 0), ccp(359, 254), CCSize(0, 0), "", "CocoRoom", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_profile.png4",
                    ccp(0, 0), ccp(490, 254), CCSize(0, 0), "", "CocoRoom", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_profile.png5",
                    ccp(0, 0), ccp(620, 254), CCSize(0, 0), "", "CocoRoom", this, 1) );
    
    // bottom button
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_red_mini.png",
                    ccp(0, 0), ccp(750, 262), CCSize(0, 0), "", "CocoRoom", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_confirm_mini.png",
            ccp(0.5, 0), ccp(spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->getContentSize().width/2, 30), CCSize(0, 0), "button/btn_red_mini.png", "0", NULL, 1) );
}

void CocoRoomTodayCandy::MakeScroll()
{
    int numOfList = friendList.size();
    
    // init select array
    selected.clear();
    for (int i = 0 ; i < numOfList ; i++)
        selected.push_back(false);
    
    // make scroll
    CCLayer* scrollContainer = CCLayer::create();
    scrollContainer->setPosition(ccp(77, 492+904+243));
    scrollContainer->setContentSize(CCSizeMake(862, numOfList*166));
    
    int height = 0;
    char fname[50], fname2[50];
    for (int i = 0 ; i < numOfList ; i++)
    {
        if (friendList[i]->GetKakaoId() == myInfo->GetKakaoId())
            continue;
        
        CCLayer* itemLayer = CCLayer::create();
        itemLayer->setContentSize(CCSizeMake(862, 166));
        itemLayer->setPosition(ccp(34, (numOfList-height-1)*166));
        scrollContainer->addChild(itemLayer, 2);
        spriteClass->layers.push_back(itemLayer);
        height++;
        
        // profile bg
        spriteClass->spriteObj.push_back( SpriteObject::CreateFromSprite(0, friendList[i]->GetProfile(), ccp(0, 0), ccp(45, 35), CCSize(0, 0), "", "Layer", itemLayer, 3) );
        //sprintf(fname, "background/bg_profile.png%d", i);
        //spriteClass->spriteObj.push_back( SpriteObject::Create(0, fname, ccp(0, 0),
        //                ccp(45, 35), CCSize(0, 0), "", "Layer", itemLayer, 3) );
        
        // name (text)
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(friendList[i]->GetNickname(), fontList[0], 48,
                        ccp(0, 0), ccp(196, 71), ccc3(78,47,8), "", "Layer", itemLayer, 3) );
        
        // button
        if (selected[i] == 0)
        {
        sprintf(fname, "button/btn_blue_mini.png%d", i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, fname,
                        ccp(0, 0), ccp(635, 34), CCSize(0, 0), "", "Layer", itemLayer, 3) );
        sprintf(fname2, "letter/letter_select.png%d", i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, fname2,
                        ccp(0.5, 0), ccp(spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->getContentSize().width/2, 24), CCSize(0, 0), fname, "0", NULL, 3) );
        }
        else
        {
            sprintf(fname, "button/btn_skill_master.png%d", i);
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, fname,
                        ccp(0, 0), ccp(635, 34), CCSize(0, 0), "", "Layer", itemLayer, 5) );
            sprintf(fname2, "letter/letter_select_brown.png%d", i);
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, fname2, ccp(0.5, 0),
                        ccp(spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->
                        getContentSize().width/2, 30), CCSize(0, 0), fname, "0", NULL, 5) );
        }
        // dotted line
        if (i < numOfList-1)
        {
            sprintf(fname, "background/bg_dotted_line.png%d", i);
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, fname,
                        ccp(0, 0), ccp(0, 5), CCSize(0, 0), "", "Layer", itemLayer, 3) );
        }
    }
    
    // container 생성 + offset
    scrollView->setContainer(scrollContainer);
    scrollView->setContentSize(scrollContainer->getContentSize());
    scrollView->setContentOffset(ccp(0, 904-80-(numOfList*166)), false);
}


bool CocoRoomTodayCandy::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    if (isTouched)
        return false;
    isTouched = true;
    isScrolling = false;
    isScrollViewTouched = false;
    
    CCPoint point = pTouch->getLocation();
    
    if (scrollView->boundingBox().containsPoint(point))
        isScrollViewTouched = true;
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        if (spriteClass->spriteObj[i]->name == "button/btn_x_brown.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                EndScene();
                break;
            }
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_question.png")
        {
        }
    }
    
    return true;
}


void CocoRoomTodayCandy::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
}

void CocoRoomTodayCandy::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    isTouched = false;
    
    CCPoint point = pTouch->getLocation();
    
    char fname[50];
    for (int i = 0 ; i < friendList.size(); i++)
    {
        if (friendList[i]->GetKakaoId() == myInfo->GetKakaoId())
            continue;
        
        sprintf(fname, "button/btn_blue_mini.png%d", i);
        CCSprite* temp = (CCSprite*)spriteClass->FindSpriteByName(fname);
        CCPoint p = temp->convertToNodeSpace(point);
        
        CCSize size = spriteClass->GetContentSizeByName(fname);
        if (isScrollViewTouched && !isScrolling &&
            (int)p.x >= 0 && (int)p.y >= 0 && (int)p.x <= size.width && (int)p.y <= size.height)
        {
            sound->playClick();
            selected[i] = !selected[i];
        }
    }
}

void CocoRoomTodayCandy::scrollViewDidScroll(CCScrollView* view)
{
    isScrolling = true;
}

void CocoRoomTodayCandy::scrollViewDidZoom(CCScrollView* view)
{
}


void CocoRoomTodayCandy::EndScene()
{
    sound->playClick();
    
    // release depth tree
    Depth::RemoveCurDepth();
    
    // touch 넘겨주기 (GetCurName = 위에서 remove 했기 때문에 결국 여기 입장에서는 부모다)
    CCString* param = CCString::create("0");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetCurName(), param);
    
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    
    // remove all objects
    spriteClass->RemoveAllObjects();
    delete spriteClass;
    scrollView->removeAllChildren();
    scrollView->removeFromParentAndCleanup(true);
    
    this->removeFromParentAndCleanup(true);
}

void CocoRoomTodayCandy::EndSceneCallback()
{
}

