#include "CocoRoomFairyTown.h"

CCScene* CocoRoomFairyTown::scene()
{
    CCScene* pScene = CCScene::create();
    CocoRoomFairyTown* pLayer = CocoRoomFairyTown::create();
    pScene->addChild(pLayer);
    
	return pScene;
}

void CocoRoomFairyTown::onEnter()
{
    //CCLog("CocoRoomFairyTown :: onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
    CCLayer::onEnter();
}
void CocoRoomFairyTown::onExit()
{
    //CCLog("CocoRoomFairyTown :: onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
}

void CocoRoomFairyTown::keyBackClicked()
{
    EndScene();
}


bool CocoRoomFairyTown::init()
{
    //CCLog("CocoRoomFairyTown :: Init");
	if (!CCLayer::init())
	{
		return false;
	}
    
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    // notification observer
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(CocoRoomFairyTown::Notification), "CocoRoomFairyTown", NULL);
    
    // notification
    CCString* param = CCString::create("1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification("CocoRoom", param);
    
    InitSprites();
    MakeScroll();
    
    return true;
}

void CocoRoomFairyTown::Notification(CCObject* obj)
{
    CCString* param = (CCString*)obj;
    
    if (param->intValue() == 0)
    {
        // 터치 활성
        this->setKeypadEnabled(true);
        this->setTouchEnabled(true);
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 0, false);
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


void CocoRoomFairyTown::InitSprites()
{
    CCSprite* pBlack = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, winSize.width, winSize.height));
    pBlack->setPosition(ccp(0, 0));
    pBlack->setAnchorPoint(ccp(0, 0));
    pBlack->setColor(ccc3(0, 0, 0));
    pBlack->setOpacity(150);
    this->addChild(pBlack, 0);
    
    
    spriteClass = new SpriteClass();
    
    // make pop-up background
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png1",
                    ccp(0, 0), ccp(49, 458), CCSize(982, 1073), "", "CocoRoomFairyTown", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png1",
                    ccp(0, 0), ccp(75, 492), CCSize(929, 904), "", "CocoRoomFairyTown", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_subtitle_fairytown.png", ccp(0, 0), ccp(103, 1429), CCSize(0, 0), "", "CocoRoomFairyTown", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_x_brown.png",
                    ccp(0, 0), ccp(900, 1420), CCSize(0, 0), "", "CocoRoomFairyTown", this, 1) );
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
}

void CocoRoomFairyTown::MakeScroll()
{
    name.push_back("똥싼바지");
    name.push_back("몬시로");
    name.push_back("은근해");
    name.push_back("파워플라이");
    name.push_back("잠만자양");
    name.push_back("꽃등신");
    name.push_back("흑미랑");
    name.push_back("빙신");
    name.push_back("메케메케");
    name.push_back("니케");
    name.push_back("쿠쿠리");
    grade.push_back(0); // 0,1,2,3 -> A,B,C,D
    grade.push_back(2);
    grade.push_back(2);
    grade.push_back(3);
    grade.push_back(1);
    grade.push_back(3);
    grade.push_back(2);
    grade.push_back(1);
    grade.push_back(0);
    grade.push_back(1);
    grade.push_back(2);
    
    int spriteClassSize = spriteClass->spriteObj.size();
    
    // make scroll
    CCLayer* scrollContainer = CCLayer::create();
    //scrollContainer->setPosition(ccp(77, 492));
    
    int numOfList = 11;
    char fname[50];
    for (int i = 0 ; i < numOfList ; i++)
    {
        CCLog("y : %d", 492+904-(290+5)*(i/3+1));
        CCLayer* itemLayer = CCLayer::create();
        itemLayer->setContentSize(CCSizeMake(290, 290));
        itemLayer->setPosition(ccp(20+((290+5)*(i%3)), -220+492+904-(290+5)*(i/3+1)));//492+904
        scrollContainer->addChild(itemLayer, 2);
        
        // bg
        sprintf(fname, "background/bg_board_brown.png_a%d", i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(1, fname, ccp(0, 0),
                        ccp(0, 0), CCSize(290, 290), "", "Layer", itemLayer, 3) );
        // grade
        if (grade[i] == 0) sprintf(fname, "letter/letter_grade_a.png%d", i);
        else if (grade[i] == 1) sprintf(fname, "letter/letter_grade_b.png%d", i);
        else if (grade[i] == 2) sprintf(fname, "letter/letter_grade_c.png%d", i);
        else if (grade[i] == 3) sprintf(fname, "letter/letter_grade_d.png%d", i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, fname, ccp(0, 0),
                        ccp(25, 219), CCSize(0, 0), "", "Layer", itemLayer, 3) );
        
        // 이름
        sprintf(fname, "background/bg_dontknow_1.png%d", i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(1, fname, ccp(0, 0),
                        ccp(22, 25), CCSize(248, 58), "", "Layer", itemLayer, 3) );
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(name[i], fontList[0], 36,
                        ccp(0.5, 0), ccp(spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite9->getContentSize().width/2, 13), ccc3(255,255,255), fname, "1", NULL, 3) );
        
        // check
        if (i == 4) {
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_fairy_select.png",
                        ccp(0, 0), ccp(172, 173), CCSize(0, 0), "", "Layer", itemLayer, 3) );
        }
    }
    
    // addchild
    for (int i = spriteClassSize ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
    
    // scrollview 내용 전체크기
    scrollContainer->setContentSize(CCSizeMake(862, ((int)(numOfList-1)/3 + 1)*(290+5)));
    // scrollView 생성
    scrollView = CCScrollView::create();
    scrollView->retain();
    scrollView->setDirection(kCCScrollViewDirectionVertical);
    scrollView->setViewSize(CCSizeMake(929, 904-40));
    scrollView->setContentSize(scrollContainer->getContentSize());
    scrollView->setAnchorPoint(ccp(0, 0));
    scrollView->setPosition(ccp(77, 492+20));
    scrollView->setContainer(scrollContainer);
    scrollView->setDelegate(this);
    //scrollView->setContentOffset(ccp(0, 904-40-), false);
    scrollView->setContentOffset(ccp(0, 904-40-(((int)(numOfList-1)/3 + 1)*(290+5))), false);
    this->addChild(scrollView, 3);
}


bool CocoRoomFairyTown::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    if (isTouched)
        return false;
    isTouched = true;
    isScrolling = false;
    scrollViewTouch = false;
    
    CCPoint point = pTouch->getLocation();

    if (scrollView->boundingBox().containsPoint(point))
        scrollViewTouch = true;
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        if (spriteClass->spriteObj[i]->name == "button/btn_x_brown.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
                EndScene();
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_question.png")
        {
        }
    }
    
    return true;
}


void CocoRoomFairyTown::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
}

void CocoRoomFairyTown::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    CCPoint point = pTouch->getLocation();
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        if (spriteClass->spriteObj[i]->name.substr(0, 31) == "background/bg_board_brown.png_a")
        {
            CCPoint p = spriteClass->spriteObj[i]->sprite9->convertToNodeSpace(point);
            CCSize size = spriteClass->spriteObj[i]->sprite9->getContentSize();
            if (scrollViewTouch && !isScrolling &&
                (int)p.x >= 0 && (int)p.y >= 0 && (int)p.x <= size.width && (int)p.y <= size.height)
            {
                sound->playClickboard();
                Common::ShowNextScene(this, "CocoRoomFairyTown", "FairyOneInfo", false);
            }
        }
    }
    
    isTouched = false;
}

void CocoRoomFairyTown::scrollViewDidScroll(CCScrollView* view)
{
    isScrolling = true;
}

void CocoRoomFairyTown::scrollViewDidZoom(CCScrollView* view)
{
}


void CocoRoomFairyTown::EndScene()
{
    sound->playClick();
    
    CCString* param = CCString::create("0");
    CCNotificationCenter::sharedNotificationCenter()->postNotification("CocoRoom", param);
    
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    
    scrollView->removeAllChildren();
    scrollView->removeFromParent();

    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, "CocoRoomFairyTown");
    
    this->removeFromParentAndCleanup(true);
}

void CocoRoomFairyTown::EndSceneCallback()
{
}
