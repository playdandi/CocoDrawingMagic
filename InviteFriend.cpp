#include "InviteFriend.h"

CCScene* InviteFriend::scene()
{
    CCScene* pScene = CCScene::create();
    InviteFriend* pLayer = InviteFriend::create();
    pScene->addChild(pLayer);
    
	return pScene;
}

void InviteFriend::onEnter()
{
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
    CCLayer::onEnter();
}
void InviteFriend::onExit()
{
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
}

void InviteFriend::keyBackClicked()
{
    EndScene();
}


bool InviteFriend::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
    //CCLog("invite friend = init()");
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    // notification post
    CCString* param = CCString::create("1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification("Ranking", param);
    
    InitSprites();
    MakeScroll();
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
    
    isTouched = false;
    isScrolling = false;
    isScrollViewTouched = false;
    
    return true;
}

void InviteFriend::InitSprites()
{
    CCSprite* pBlack = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, winSize.width, winSize.height));
    pBlack->setPosition(ccp(0, 0));
    pBlack->setAnchorPoint(ccp(0, 0));
    pBlack->setColor(ccc3(0, 0, 0));
    pBlack->setOpacity(150);
    this->addChild(pBlack, 0);
    
    spriteClass = new SpriteClass();
    
    // strap
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_green.png",
                    ccp(0, 0), ccp(14, 1343), CCSize(0, 0), "", "InviteFriend", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_x_yellow.png",
                    ccp(0, 0), ccp(875, 1391), CCSize(0, 0), "", "InviteFriend", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_title_invite.png",
                    ccp(0, 0), ccp(359, 1389), CCSize(0, 0), "", "InviteFriend", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_addfriend.png",
                    ccp(0, 0), ccp(264, 1389), CCSize(0, 0), "", "InviteFriend", this, 2) );
    
    // background
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png",
                    ccp(0, 0), ccp(49, 147), CCSize(982, 1265), "", "InviteFriend", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png1",
                    ccp(0, 0), ccp(75, 492), CCSize(929, 904), "", "InviteFriend", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png2",
                    ccp(0, 0), ccp(98, 256), CCSize(244, 176), "", "InviteFriend", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png3",
                    ccp(0, 0), ccp(390, 256), CCSize(244, 176), "", "InviteFriend", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png4",
                    ccp(0, 0), ccp(686, 256), CCSize(244, 176), "", "InviteFriend", this, 1) );
    
    // 친구초대 보상이벤트
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_subtitle_friendevent.png",
                    ccp(0, 0), ccp(98, 438), CCSize(0, 0), "", "InviteFriend", this, 2) );
    
    // invite 10,20,30
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_dontknow_1.png1",
                    ccp(0, 0), ccp(228, 378), CCSize(0, 0), "", "InviteFriend", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_dontknow_1.png2",
                    ccp(0, 0), ccp(519, 378), CCSize(0, 0), "", "InviteFriend", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_dontknow_1.png3",
                    ccp(0, 0), ccp(817, 378), CCSize(0, 0), "", "InviteFriend", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_invite_10.png",
                    ccp(0.5, 0.5), spriteClass->FindParentCenterPos("background/bg_dontknow_1.png1"),
                    CCSize(0, 0), "background/bg_dontknow_1.png1", "0", NULL, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_invite_20.png",
                    ccp(0.5, 0.5), spriteClass->FindParentCenterPos("background/bg_dontknow_1.png2"),
                    CCSize(0, 0), "background/bg_dontknow_1.png2", "0", NULL, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_invite_30.png",
                    ccp(0.5, 0.5), spriteClass->FindParentCenterPos("background/bg_dontknow_1.png3"),
                    CCSize(0, 0), "background/bg_dontknow_1.png3", "0", NULL, 2) );
    
    // progressbar
    
    // text
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("27명 초대", fontList[0], 36, ccp(0, 0), ccp(812, 200), ccc3(255,255,255), "", "InviteFriend", this, 2) );
}

void InviteFriend::MakeScroll()
{
    // example
    name.push_back("AHRORO");
    name.push_back("견음");
    name.push_back("막나가는딸내미");
    name.push_back("나눔고딕48");
    name.push_back("진하게");
    name.push_back("세로는뭔가");
    name.push_back("견음");
    state.push_back(NOTADDED);
    state.push_back(NOTADDED);
    state.push_back(ADDED);
    state.push_back(ADDED);
    state.push_back(NOTADDED);
    state.push_back(ADDED);
    state.push_back(NOTADDED);
    int numOfList = 7;
    
    
    // make scroll
    CCLayer* scrollContainer = CCLayer::create();
    scrollContainer->setAnchorPoint(ccp(0, 1));
    scrollContainer->setPosition(ccp(77, 492+904));
    
    char spriteName[50], spriteName2[50];
    for (int i = 0 ; i < numOfList ; i++)
    {
        CCLayer* itemLayer = CCLayer::create();
        itemLayer->setContentSize(CCSizeMake(862, 166));
        itemLayer->setPosition(ccp(34, (numOfList-i-1)*166));
        scrollContainer->addChild(itemLayer, 2);
        
        // profile bg
        sprintf(spriteName, "background/bg_profile.png%d", i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, spriteName,
                        ccp(0, 0), ccp(45, 35), CCSize(0, 0), "", "Layer", itemLayer, 3) );
        
        // name
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(name[i], fontList[0], 48,
                        ccp(0, 0), ccp(196, 118-10), ccc3(78,47,8), "", "Layer", itemLayer, 3) );
        
        // starcandy bg + starcandy + text(*1000)
        sprintf(spriteName, "background/bg_degree_desc.png%d", i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(1, spriteName,
                        ccp(0, 0), ccp(269, 25), CCSize(223, 76), "", "Layer", itemLayer, 3) );
        sprintf(spriteName2, "icon/icon_starcandy_mini.png%d", i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, spriteName2,
                        ccp(0, 0), ccp(13, 6), CCSize(0, 0), spriteName, "1", NULL, 3) );
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("X 1000", fontList[0], 36,
                        ccp(0, 0), ccp(83, 19), ccc3(78,47,8), spriteName, "1", NULL, 3) );
        
        // button
        if (state[i] == NOTADDED)
        {
            sprintf(spriteName, "button/btn_blue_mini.png%d", i);
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, spriteName,
                        ccp(0, 0), ccp(635, 34+10), CCSize(0, 0), "", "Layer", itemLayer, 3) );
            sprintf(spriteName2, "letter/letter_invite.png%d", i);
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, spriteName2,
                        ccp(0, 0), ccp(45, 25), CCSize(0, 0), spriteName, "0", NULL, 3) );
        }
        else
        {
            sprintf(spriteName, "button/btn_invite.png%d", i);
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, spriteName,
                        ccp(0, 0), ccp(635, 34+10), CCSize(0, 0), "", "Layer", itemLayer, 3) );
            sprintf(spriteName2, "letter/letter_invite_brown.png%d", i);
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, spriteName2,
                        ccp(0, 0), ccp(45, 25), CCSize(0, 0), spriteName, "0", NULL, 3) );
        }
        
        // dotted line
        sprintf(spriteName, "background/bg_dotted_line.png%d", i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, spriteName,
                        ccp(0, 0), ccp(0, 10), CCSize(0, 0), "", "Layer", itemLayer, 3) );
    }
    
    // scrollview 내용 전체크기
    scrollContainer->setContentSize(CCSizeMake(862, numOfList*166));
    // scrollView 생성
    scrollView = CCScrollView::create();
    scrollView->retain();
    scrollView->setDirection(kCCScrollViewDirectionVertical);
    scrollView->setViewSize(CCSizeMake(929, 904-80)); // (내용 1개 크기, 노란보드 세로크기)
    scrollView->setContentSize(scrollContainer->getContentSize());
    scrollView->setAnchorPoint(ccp(0, 0));
    scrollView->setPosition(ccp(77, 492+20));
    scrollView->setContainer(scrollContainer);
    scrollView->setDelegate(this);
    scrollView->setContentOffset(ccp(0, 904-80-(numOfList*166)), false);
    this->addChild(scrollView, 3);
}


bool InviteFriend::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    if (isTouched)
        return false;
    isTouched = true;
    isScrolling = false;
    isScrollViewTouched = false;
    
    CCPoint point = pTouch->getLocation();
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        if (spriteClass->spriteObj[i]->name == "button/btn_x_yellow.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
                EndScene();
        }
    }
    
    return true;
}


void InviteFriend::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
    //CCPoint point = pTouch->getLocation();
}

void InviteFriend::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    isTouched = false;
    isScrolling = false;
    isScrollViewTouched = false;
}

void InviteFriend::scrollViewDidScroll(CCScrollView* view)
{
    isScrolling = true;
    CCLog("invitefriend scrolling");
}

void InviteFriend::scrollViewDidZoom(CCScrollView* view)
{
}


void InviteFriend::EndScene()
{
    sound->playClick();
    CCString* param = CCString::create("0");
    CCNotificationCenter::sharedNotificationCenter()->postNotification("Ranking", param);
    
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    
    scrollView->removeAllChildren();
    scrollView->removeFromParentAndCleanup(true);
    
    this->removeFromParentAndCleanup(true);
}

void InviteFriend::EndSceneCallback()
{
}

