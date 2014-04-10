#include "RequestPotion.h"

CCScene* RequestPotion::scene()
{
    CCScene* pScene = CCScene::create();
    RequestPotion* pLayer = RequestPotion::create();
    pScene->addChild(pLayer);
    
	return pScene;
}

void RequestPotion::onEnter()
{
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
    CCLayer::onEnter();
}
void RequestPotion::onExit()
{
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
}

void RequestPotion::keyBackClicked()
{
    EndScene();
}


bool RequestPotion::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
    //CCLog("RequestPotion. init");
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    // notification post
    CCString* param = CCString::create("1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification("BuyPotion", param);
    
    InitSprites();
    MakeScroll();
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
    
    isTouched = false;
    isScrolling = false;
    isScrollViewTouched = false;
    
    return true;
}

void RequestPotion::InitSprites()
{
    CCSprite* pBlack = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, winSize.width, winSize.height));
    pBlack->setPosition(ccp(0, 0));
    pBlack->setAnchorPoint(ccp(0, 0));
    pBlack->setColor(ccc3(0, 0, 0));
    pBlack->setOpacity(150);
    this->addChild(pBlack, 0);
    
    spriteClass = new SpriteClass();
    
    // background
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_green.png",
                    ccp(0, 0), ccp(14, 1586), CCSize(0, 0), "", "RequestPotion", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_x_yellow.png",
                    ccp(0, 0), ccp(875, 1391+243), CCSize(0, 0), "", "RequestPotion", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_title_pester.png",
                    ccp(0, 0), ccp(279, 1389+243), CCSize(0, 0), "", "RequestPotion", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_potion_big.png",
                    ccp(0, 0), ccp(192, 1389+243), CCSize(0, 0), "", "RequestTopaz", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png",
                    ccp(0, 0), ccp(49, 458-45), CCSize(982, 954+243+45), "", "RequestPotion", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png",
                    ccp(0, 0), ccp(75, 492-45), CCSize(929, 904+243+45), "", "RequestPotion", this, 1) );
}

void RequestPotion::MakeScroll()
{
    // make scroll
    CCLayer* scrollContainer = CCLayer::create();
    scrollContainer->setPosition(ccp(77, 492+904+243));
    
    int numOfList = friendList.size();
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
        height++;
        
        // profile bg
        spriteClass->spriteObj.push_back( SpriteObject::CreateFromSprite(0, friendList[i]->GetProfile(), ccp(0, 0), ccp(45, 35), CCSize(0, 0), "", "Layer", itemLayer, 3) );
        //sprintf(fname, "background/bg_profile.png%d", i);
        //spriteClass->spriteObj.push_back( SpriteObject::Create(0, fname, ccp(0, 0),
        //                ccp(45, 35), CCSize(0, 0), "", "Layer", itemLayer, 3) );
        
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
    
    // scrollview 내용 전체크기
    scrollContainer->setContentSize(CCSizeMake(862, numOfList*166));
    // scrollView 생성
    scrollView = CCScrollView::create();
    scrollView->retain();
    scrollView->setDirection(kCCScrollViewDirectionVertical);
    scrollView->setViewSize(CCSizeMake(929, 904+243+45-100));
    scrollView->setContentSize(scrollContainer->getContentSize());
    scrollView->setAnchorPoint(ccp(0, 0));
    scrollView->setPosition(ccp(77, 492-45+40));
    scrollView->setContainer(scrollContainer);
    scrollView->setDelegate(this);
    scrollView->setContentOffset(ccp(0, 904+243+45-100-(numOfList*166)), false);
    this->addChild(scrollView, 3);
}


bool RequestPotion::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
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
                EndScene();
        }
        else if (spriteClass->spriteObj[i]->name.substr(0, 26) == "button/btn_yellow_mini.png")
        {
            //if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            //    ;
        }
    }
    
    return true;
}


void RequestPotion::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
    //CCPoint point = pTouch->getLocation();
}

void RequestPotion::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    isTouched = false;
    isScrolling = false;
    isScrollViewTouched = false;
}


void RequestPotion::scrollViewDidScroll(CCScrollView* view)
{
    isScrolling = true;
}

void RequestPotion::scrollViewDidZoom(CCScrollView* view)
{
}


void RequestPotion::EndScene()
{
    sound->playClick();
    
    CCString* param = CCString::create("0");
    CCNotificationCenter::sharedNotificationCenter()->postNotification("BuyPotion", param);
    
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    
    scrollView->removeAllChildren();
    scrollView->removeFromParentAndCleanup(true);
    
    this->removeFromParentAndCleanup(true);
}

void RequestPotion::EndSceneCallback()
{
}
