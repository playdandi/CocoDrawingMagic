#include "RequestTopaz.h"

CCScene* RequestTopaz::scene()
{
    CCScene* pScene = CCScene::create();
    RequestTopaz* pLayer = RequestTopaz::create();
    pScene->addChild(pLayer);
    
	return pScene;
}

void RequestTopaz::onEnter()
{
    CCLog("RequestTopaz : onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority(), true);
    CCLayer::onEnter();
}
void RequestTopaz::onExit()
{
    CCLog("RequestTopaz : onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
}

void RequestTopaz::keyBackClicked()
{
    EndScene();
}


bool RequestTopaz::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
    // make depth tree
    Depth::AddCurDepth("RequestTopaz");
    
    this->setTouchEnabled(true);
    this->setKeypadEnabled(true);
    this->setTouchPriority(Depth::GetCurPriority());
    CCLog("RequestTopaz : touch prio = %d", this->getTouchPriority());
    
    // notification observer
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(RequestTopaz::Notification), Depth::GetCurName(), NULL);
    
    // notification post
    CCString* param = CCString::create("1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetParentName(), param);
    

    winSize = CCDirector::sharedDirector()->getWinSize();
    
    scrollView = CCScrollView::create();
    //scrollView->retain();
    scrollView->setDirection(kCCScrollViewDirectionVertical);
    scrollView->setViewSize(CCSizeMake(929, 904+243+45-100));
    scrollView->setAnchorPoint(ccp(0, 0));
    scrollView->setPosition(ccp(77, 492-45+40));
    scrollView->setDelegate(this);
    scrollView->setTouchPriority(Depth::GetCurPriority());
    this->addChild(scrollView, 3);
    
    InitSprites();
    MakeScroll();
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
    
    isTouched = false;
    isScrolling= false;
    isScrollViewTouched = false;
    
    return true;
}

void RequestTopaz::Notification(CCObject* obj)
{
    CCString* param = (CCString*)obj;
    
    if (param->intValue() == 0)
    {
        // 터치 활성
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority()+1, true);
        this->setTouchPriority(Depth::GetCurPriority());
        isTouched = false;
        CCLog("RequestTopaz : 터치 활성 (Priority = %d)", this->getTouchPriority());
    }
    else if (param->intValue() == 1)
    {
        // 터치 비활성
        CCLog("RequestTopaz 터치 비활성");
        CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
    }
}


void RequestTopaz::InitSprites()
{
    pBlack = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, winSize.width, winSize.height));
    pBlack->setPosition(ccp(0, 0));
    pBlack->setAnchorPoint(ccp(0, 0));
    pBlack->setColor(ccc3(0, 0, 0));
    pBlack->setOpacity(150);
    this->addChild(pBlack, 0);
    
    spriteClass = new SpriteClass();
    
    // background
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_green.png",
                    ccp(0, 0), ccp(14, 1586), CCSize(0, 0), "", "RequestTopaz", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_x_yellow.png",
                    ccp(0, 0), ccp(875, 1391+243), CCSize(0, 0), "", "RequestTopaz", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_title_pester.png",
                    ccp(0, 0), ccp(279, 1389+243), CCSize(0, 0), "", "RequestTopaz", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_present_box.png",
                    ccp(0, 0), ccp(179, 1389+243+5), CCSize(0, 0), "", "RequestTopaz", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png",
                    ccp(0, 0), ccp(49, 458-45), CCSize(982, 954+243+45), "", "RequestTopaz", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png",
                    ccp(0, 0), ccp(75, 492-45), CCSize(929, 904+243+45), "", "RequestTopaz", this, 1) );
}

void RequestTopaz::MakeScroll()
{
    int numOfList = friendList.size();
    
    scrollContainer = CCLayer::create();
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
        //               ccp(45, 35), CCSize(0, 0), "", "Layer", itemLayer, 3) );
        
        // name (text)
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(friendList[i]->GetNickname(), fontList[0], 48, ccp(0, 0), ccp(196, 71), ccc3(78,47,8), "", "Layer", itemLayer, 3) );
        
        // button
        sprintf(fname, "button/btn_yellow_mini.png%d", i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, fname,
                        ccp(0, 0), ccp(635, 34), CCSize(0, 0), "", "Layer", itemLayer, 3) );
        sprintf(fname2, "letter/letter_request2.png%d", i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, fname2,
                        ccp(0.5, 0), ccp(spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->getContentSize().width/2, 24), CCSize(0, 0), fname, "0", NULL, 3) );
        // dotted line
        if (i < numOfList-1)
        {
            sprintf(fname, "background/bg_dotted_line.png%d", i);
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, fname,
                        ccp(0, 0), ccp(0, 5), CCSize(0, 0), "", "Layer", itemLayer, 3) );
        }
    }
    
    // container 생성
    scrollView->setContainer(scrollContainer);
    scrollView->setContentSize(scrollContainer->getContentSize());
    scrollView->setContentOffset(ccp(0, 904+243+45-100-(numOfList*166)), false);
}


bool RequestTopaz::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
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
        if (spriteClass->spriteObj[i]->name == "button/btn_x_yellow.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                EndScene();
                break;
            }
        }
    }
    
    return true;
}


void RequestTopaz::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
}

void RequestTopaz::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    CCPoint point = pTouch->getLocation();
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        if (spriteClass->spriteObj[i]->name.substr(0, 26) == "button/btn_yellow_mini.png")
        {
            CCPoint p = spriteClass->spriteObj[i]->sprite->convertToNodeSpace(point);
            CCSize size = spriteClass->spriteObj[i]->sprite->getContentSize();
            if (isScrollViewTouched && !isScrolling &&
                (int)p.x >= 0 && (int)p.y >= 0 && (int)p.x <= size.width && (int)p.y <= size.height)
            {
                // protocol 요청
                
                break;
            }
        }
    }
    
    isTouched = false;
    isScrolling = false;
    isScrollViewTouched = false;
}


void RequestTopaz::scrollViewDidScroll(CCScrollView* view)
{
    isScrolling = true;
}

void RequestTopaz::scrollViewDidZoom(CCScrollView* view)
{
}


void RequestTopaz::EndScene()
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
    
    scrollView->getContainer()->removeAllChildren();
    scrollView->removeAllChildren();
    scrollView->removeFromParentAndCleanup(true);
    
    pBlack->removeFromParentAndCleanup(true);
    
    this->removeFromParentAndCleanup(true);
}

