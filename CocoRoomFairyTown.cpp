#include "CocoRoomFairyTown.h"


CocoRoomFairyTown::~CocoRoomFairyTown()
{
    //CCLog("CocoFairyTown 소멸자 실행");
}

CCScene* CocoRoomFairyTown::scene()
{    
    CCScene* pScene = CCScene::create();
    CocoRoomFairyTown* pLayer = CocoRoomFairyTown::create();
    pScene->addChild(pLayer);
    
	return pScene;
}

void CocoRoomFairyTown::onEnter()
{
    CCLog("CocoRoomFairyTown :: onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority(), true);
    CCLayer::onEnter();
}
void CocoRoomFairyTown::onExit()
{
    CCLog("CocoRoomFairyTown :: onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
}

void CocoRoomFairyTown::keyBackClicked()
{
    EndScene();
}


bool CocoRoomFairyTown::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
    // make depth tree
    Depth::AddCurDepth("CocoRoomFairyTown");
    
    this->setTouchEnabled(true);
    this->setKeypadEnabled(true);
    this->setTouchPriority(Depth::GetCurPriority());
    CCLog("CocoRoomFairyTown : touch prio = %d", this->getTouchPriority());
    
    // notification observer
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(CocoRoomFairyTown::Notification), Depth::GetCurName(), NULL);
    
    // notification
    CCString* param = CCString::create("1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetParentName(), param);
    
    
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    // scrollView 생성
    scrollView = CCScrollView::create();
    scrollView->retain();
    scrollView->setDirection(kCCScrollViewDirectionVertical);
    scrollView->setViewSize(CCSizeMake(929, 904-40));
    scrollView->setAnchorPoint(ccp(0, 0));
    scrollView->setPosition(ccp(77, 492+20));
    scrollView->setDelegate(this);
    scrollView->setTouchPriority(Depth::GetCurPriority()); // priority
    this->addChild(scrollView, 3);
    
    InitSprites();
    MakeScroll();
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
    
    return true;
}

void CocoRoomFairyTown::Notification(CCObject* obj)
{
    CCString* param = (CCString*)obj;
    
    if (param->intValue() == 0)
    {
        // 터치 활성
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority()+1, true);
        this->setTouchPriority(Depth::GetCurPriority());
        isTouched = false;
        CCLog("CocoRoomFairyTown : 터치 활성 (Priority = %d)", this->getTouchPriority());
    }
    else if (param->intValue() == 1)
    {
        // 터치 비활성
        CCLog("CocoRoomFairyTown : 터치 비활성");
        CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
    }
}


void CocoRoomFairyTown::InitSprites()
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
                    ccp(0, 0), ccp(49, 458), CCSize(982, 1073), "", "CocoRoomFairyTown", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png1",
                    ccp(0, 0), ccp(75, 492), CCSize(929, 904), "", "CocoRoomFairyTown", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_subtitle_fairytown.png", ccp(0, 0), ccp(103, 1429), CCSize(0, 0), "", "CocoRoomFairyTown", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_x_brown.png",
                    ccp(0, 0), ccp(900, 1420), CCSize(0, 0), "", "CocoRoomFairyTown", this, 1) );
}

void CocoRoomFairyTown::MakeScroll()
{
    int numOfList = fairyInfo.size();
    char fname[50];
    
    // container
    scrollContainer = CCLayer::create();
    scrollContainer->setContentSize(CCSizeMake(870, ((int)((numOfList+10)-1)/3 + 1)*(290+5)));
   
    for (int i = 0 ; i < numOfList+10 ; i++)
    {
        CCLayer* itemLayer = CCLayer::create();
        itemLayer->setContentSize(CCSizeMake(290, 290));
        itemLayer->setPosition(ccp(20+((290+5)*(i%3)), scrollContainer->getContentSize().height-(295)*(i/3+1) ) );
        scrollContainer->addChild(itemLayer, 2);
        spriteClass->layers.push_back(itemLayer);
        
        // bg
        sprintf(fname, "background/bg_board_brown.png_a%d", i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(1, fname, ccp(0, 0), ccp(0, 0), CCSize(290, 290), "", "Layer", itemLayer, 3) );
        // tag에 fairy ID 혹은 이상한 값(터치방지용)을 넣어둔다.
        if (i < numOfList)
            spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite9->setTag(i);
        else
            spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite9->setTag(-1);
        
        // 이름
        sprintf(fname, "background/bg_dontknow_1.png%d", i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(1, fname, ccp(0, 0), ccp(22, 25), CCSize(248, 58), "", "Layer", itemLayer, 100) );
        if (i < numOfList)
            spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(fairyInfo[i]->GetName(), fontList[0], 36, ccp(0.5, 0), ccp(spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite9->getContentSize().width/2, 11), ccc3(255,255,255), fname, "1", NULL, 100) );
        
        if (i >= numOfList)
        {
            // 빈 슬롯 (COMING SOON !)
            sprintf(fname, "icon/icon_fairy_empty.png%d", i);
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, fname, ccp(0.5, 0.5), ccp(290/2, 290/2+17), CCSize(0, 0), "", "Layer", itemLayer, 90) );
            spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->setScale(1.1f);
            continue;
        }
        
        // 그림
        CCLayer* picture = Fairy::GetFairy(fairyInfo[i]->GetId());
        switch (fairyInfo[i]->GetId())
        {
            case 0: picture->setScale(1.0f); break;
            case 1: picture->setScale(0.7f); break;
            case 2: picture->setScale(0.8f); break;
        }
        picture->setAnchorPoint(ccp(0, 0));
        picture->setPosition(ccp(290/2, 290/2+30));
        itemLayer->addChild(picture, 10);

        // grade
        if (fairyInfo[i]->GetGrade() == 1) sprintf(fname, "letter/letter_grade_a.png%d", i);
        else if (fairyInfo[i]->GetGrade() == 2) sprintf(fname, "letter/letter_grade_b.png%d", i);
        else if (fairyInfo[i]->GetGrade() == 3) sprintf(fname, "letter/letter_grade_c.png%d", i);
        else if (fairyInfo[i]->GetGrade() == 4) sprintf(fname, "letter/letter_grade_d.png%d", i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, fname, ccp(0, 0), ccp(25, 219), CCSize(0, 0), "", "Layer", itemLayer, 90) );
        
        // check
        if (myInfo->GetActiveFairyId() == fairyInfo[i]->GetId())
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_fairy_select.png", ccp(0, 0), ccp(172, 173), CCSize(0, 0), "", "Layer", itemLayer, 90) );
    }
    
    // container 설정 + offset
    scrollView->setContainer(scrollContainer);
    scrollView->setContentSize(scrollContainer->getContentSize());
    scrollView->setContentOffset(ccp(0, (904-40)-scrollContainer->getContentSize().height ));
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
            if (spriteClass->spriteObj[i]->sprite9->getTag() == -1)
                continue;
            
            CCPoint p = spriteClass->spriteObj[i]->sprite9->convertToNodeSpace(point);
            CCSize size = spriteClass->spriteObj[i]->sprite9->getContentSize();
            if (scrollViewTouch && !isScrolling &&
                (int)p.x >= 0 && (int)p.y >= 0 && (int)p.x <= size.width && (int)p.y <= size.height)
            {
                sound->playClickboard();
                int idx = spriteClass->spriteObj[i]->sprite9->getTag();
                Common::ShowNextScene(this, "CocoRoomFairyTown", "FairyOneInfo", false, idx);
                //Common::ShowNextScene(this, "CocoRoomFairyTown", "FairyOneInfo", false, idx, priority-1);
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
    
    // remove this notification
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, Depth::GetCurName());
    // release depth tree
    Depth::RemoveCurDepth();
    
    // touch 넘겨주기 (GetCurName = 위에서 remove 했기 때문에 결국 여기 입장에서는 부모다)
    CCString* param = CCString::create("0");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetCurName(), param);
    
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);

    // remove all CCNodes
    spriteClass->RemoveAllObjects();
    delete spriteClass;
    scrollView->getContainer()->removeAllChildren();
    scrollView->removeAllChildren();
    scrollView->release();
    scrollView->removeFromParentAndCleanup(true);
    
    this->removeFromParentAndCleanup(true);
}

void CocoRoomFairyTown::EndSceneCallback()
{
}
