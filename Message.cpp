#include "Message.h"

CCScene* Message::scene()
{
    CCScene* pScene = CCScene::create();
    Message* pLayer = Message::create();
    pScene->addChild(pLayer);
    
	return pScene;
}

void Message::onEnter()
{
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
    CCLayer::onEnter();
}
void Message::onExit()
{
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
}

void Message::keyBackClicked()
{
    EndScene();
}


bool Message::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    // notification post
    CCString* param = CCString::create("1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification("Ranking", param);
    
    InitSprites();
    MakeScroll();
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
    
    this->setKeypadEnabled(true);
    this->setTouchEnabled(true);
    
    isTouched = false;
    isScrolling = false;
    isScrollViewTouched = false;
    
    return true;
}

void Message::InitSprites()
{
    pBlack = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, winSize.width, winSize.height));
    pBlack->setPosition(ccp(0, 0));
    pBlack->setAnchorPoint(ccp(0, 0));
    pBlack->setColor(ccc3(0, 0, 0));
    pBlack->setOpacity(150);
    this->addChild(pBlack, 0);
     
    spriteClass = new SpriteClass();
    
    // background
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_red.png",
                    ccp(0, 0), ccp(14, 1343), CCSize(0, 0), "", "Message", this, 2, 0) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_x_yellow.png",
                    ccp(0, 0), ccp(875, 1391), CCSize(0, 0), "", "Message", this, 2, 0) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_title_message.png",
                    ccp(0, 0), ccp(409, 1389), CCSize(0, 0), "", "Message", this, 2, 0) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png",
                    ccp(0, 0), ccp(49, 458), CCSize(982, 954), "", "Message", this, 1, 0) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png",
                    ccp(0, 0), ccp(75, 492), CCSize(929, 904), "", "Message", this, 1, 0) );

    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_red.png",
            ccp(0, 0), ccp(319, 191), CCSize(929, 904), "", "Message", this, 1, 0) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_potion_all_recieve.png",
            ccp(0.5, 0), ccp(spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->getContentSize().width/2, 40), CCSize(0, 0), "button/btn_red.png", "0", NULL, 1, 1) );
}

void Message::MakeScroll()
{
    // example
    msg_content.push_back("AHRORO님이 포션을 선물하셨어요.");
    msg_content.push_back("오늘의 출석체크 선물을 받았습니다!");
    msg_content.push_back("이벤트 당첨으로 요정 선택권을 받았습니다!");
    msg_content.push_back("아자!");
    msg_content.push_back("오.별~");
    msg_content.push_back("이벤트 당첨으로 요정 선택권을 받았습니다!");
    msg_type.push_back(0); // potion receive
    msg_type.push_back(1); // starcandy receive
    msg_type.push_back(2); // 뽑기
    msg_type.push_back(0); // potion receive
    msg_type.push_back(1); // starcandy receive
    msg_type.push_back(2); // 뽑기
    int numOfList = 6;
    
    
    // make scroll
    scrollContainer = CCLayer::create();
    scrollContainer->setAnchorPoint(ccp(0, 1));
    scrollContainer->setPosition(ccp(77, 492+904));

    char spriteName[35];
    for (int i = 0 ; i < numOfList ; i++)
    {
        CCLayer* itemLayer = CCLayer::create();
        itemLayer->setContentSize(CCSizeMake(862, 166));
        itemLayer->setPosition(ccp(34, (numOfList-i-1)*166));
        scrollContainer->addChild(itemLayer, 2);
        layer.push_back(itemLayer);
        //sprintf(spriteName, "layer%d", i);
        //spriteClass->spriteObj.push_back( SpriteObject::CreateLayer(3, spriteName, itemLayer, "", "Layer", NULL, 2) );
        
        // profile 영역 bg
        sprintf(spriteName, "background/bg_profile.png%d", i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, spriteName,
                        ccp(0, 0), ccp(44, 35), CCSize(0, 0), "", "Layer", itemLayer, 3, 0) );
        // text (fontList[2] = 나눔고딕볼드)
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabelArea(msg_content[i].c_str(), fontList[0],
            36, ccp(0, 0), ccp(194, 45), ccc3(78,47,8), CCSize(400, 105),
            kCCTextAlignmentLeft, kCCVerticalTextAlignmentCenter, "", "Layer", itemLayer, 3, 0) );
        // button
        sprintf(spriteName, "button/btn_green_mini.png%d", i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, spriteName,
                        ccp(0, 0), ccp(634, 35), CCSize(0, 0), "", "Layer", itemLayer, 3, 0) );

        // button 안에 image1개, letter1개 넣는다.
        switch (msg_type[i])
        {
            case 0:
                sprintf(spriteName, "button/btn_receive_potion.png%d", i);
                spriteClass->spriteObj.push_back( SpriteObject::Create(0, spriteName,
                            ccp(0, 0), ccp(667, 55), CCSize(0, 0), "", "Layer", itemLayer, 3, 0) );
                sprintf(spriteName, "letter/letter_receive.png%d", i);
                spriteClass->spriteObj.push_back( SpriteObject::Create(0, spriteName,
                            ccp(0, 0), ccp(725, 62), CCSize(0, 0), "", "Layer", itemLayer, 3, 0) );
                break;
            case 1:
                sprintf(spriteName, "button/btn_receive_starcandy.png%d", i);
                spriteClass->spriteObj.push_back( SpriteObject::Create(0, spriteName,
                            ccp(0, 0), ccp(664, 63), CCSize(0, 0), "", "Layer", itemLayer, 3, 0) );
                sprintf(spriteName, "letter/letter_receive.png%d", i);
                spriteClass->spriteObj.push_back( SpriteObject::Create(0, spriteName,
                            ccp(0, 0), ccp(725, 62), CCSize(0, 0), "", "Layer", itemLayer, 3, 0) );
                break;
            case 2:
                sprintf(spriteName, "letter/letter_vote.png%d", i);
                spriteClass->spriteObj.push_back( SpriteObject::Create(0, spriteName,
                            ccp(0, 0), ccp(676, 60), CCSize(0, 0), "", "Layer", itemLayer, 3, 0) );
                break;
        }
        
        // dotted line
        sprintf(spriteName, "background/bg_dotted_line.png%d", i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, spriteName,
                        ccp(0, 0), ccp(0, 5), CCSize(0, 0), "", "Layer", itemLayer, 3, 0) );
    }
    
    // scrollview 내용 전체크기
    scrollContainer->setContentSize(CCSizeMake(862, numOfList*166));
    // scrollView 생성
    scrollView = CCScrollView::create();
    scrollView->setDirection(kCCScrollViewDirectionVertical);
    scrollView->setViewSize(CCSizeMake(929, 904-80)); // (내용 1개 크기, 노란보드 세로크기)
    scrollView->setContentSize(scrollContainer->getContentSize());
    scrollView->setAnchorPoint(ccp(0, 0));
    scrollView->setPosition(ccp(77, 492+20));
    scrollView->setContainer(scrollContainer);
    scrollView->setDelegate(this);
    scrollView->setContentOffset(ccp(0, 904-80-(numOfList*166)), false);
    this->addChild(scrollView, 3);
    //scrollView->autorelease();
}


bool Message::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
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


void Message::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
    //CCPoint point = pTouch->getLocation();
}

void Message::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    isTouched = false;
    isScrolling = false;
    isScrollViewTouched = false;
}


void Message::scrollViewDidScroll(CCScrollView* view)
{
    isScrolling = true;
}

void Message::scrollViewDidZoom(CCScrollView* view)
{
}


void Message::EndScene()
{
    sound->playClick();
    CCString* param = CCString::create("0");
    CCNotificationCenter::sharedNotificationCenter()->postNotification("Ranking", param);
    
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    
    ReleaseAll();
    
    this->removeFromParentAndCleanup(true);
}

void Message::EndSceneCallback()
{
}

void Message::ReleaseAll()
{
    pBlack->autorelease();
    //pBlack->release();
    //pBlack->removeFromParentAndCleanup(true);
    //CCLog("pBlack : %d", pBlack->retainCount());
    
    /*
    // spriteClass release
    int maxPriority = -1;
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        maxPriority = std::max(maxPriority, spriteClass->spriteObj[i]->priority);
    for (int priority = maxPriority ; priority >= 0 ; priority--)
    {
        for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        {
            if (spriteClass->spriteObj[i]->priority == priority)
            {
                //CCLog("%d : %d", i, spriteClass->spriteObj[i]->type);
                if (spriteClass->spriteObj[i]->type == 0)
                {
                    //spriteClass->spriteObj[i]->sprite->removeAllChildrenWithCleanup(true);
                    //spriteClass->spriteObj[i]->sprite->removeFromParentAndCleanup(true);
                    //spriteClass->spriteObj[i]->sprite->autorelease();
                    //spriteClass->spriteObj[i]->sprite = NULL;
                }
                else if (spriteClass->spriteObj[i]->type == 1)
                {
                    //spriteClass->spriteObj[i]->sprite9->removeAllChildrenWithCleanup(true);
                    //spriteClass->spriteObj[i]->sprite9->removeFromParentAndCleanup(true);
                    //spriteClass->spriteObj[i]->sprite9->autorelease();
                    //spriteClass->spriteObj[i]->sprite9 = NULL;
                }
                else
                {
                    //spriteClass->spriteObj[i]->label->removeAllChildrenWithCleanup(true);
                    //spriteClass->spriteObj[i]->label->removeFromParentAndCleanup(true);
                    //spriteClass->spriteObj[i]->label->autorelease();
                    //spriteClass->spriteObj[i]->label = NULL;
                }
            }
        }
    }
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        if (spriteClass->spriteObj[i]->type == 0)
            CCLog("sprite : %d", spriteClass->spriteObj[i]->sprite->retainCount());
        else if (spriteClass->spriteObj[i]->type == 1)
            CCLog("sprite : %d", spriteClass->spriteObj[i]->sprite9->retainCount());
        else
            CCLog("sprite : %d", spriteClass->spriteObj[i]->label->retainCount());
    }
    */
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        delete spriteClass->spriteObj[i];
    spriteClass->spriteObj.clear();
    delete spriteClass;
    
    // layers
    for (int i = 0 ; i < layer.size() ; i++)
    {
        layer[i]->removeAllChildren();
        //layer[i]->removeFromParentAndCleanup(true);
        //CCLog("layer %d : %d", i, layer[i]->retainCount());
        //layer[i]->release();
        //layer[i]->autorelease();
    }
    layer.clear();
    
    // scrollview
    scrollContainer->removeAllChildren();
    //scrollContainer->removeFromParentAndCleanup(true);
    //scrollContainer->autorelease();
    scrollView->removeAllChildren();
    //scrollView->removeFromParentAndCleanup(true);
    //scrollView->autorelease();

    
 
    // etc
    msg_content.clear();
    msg_type.clear();
    
    this->removeAllChildren();
    
    //CCLog("scrollview : %d", scrollView->retainCount());
    //CCLog("container : %d", scrollContainer->retainCount());
}


