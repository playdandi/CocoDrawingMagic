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
    CCDirector::sharedDirector()->end();
}


bool Message::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    InitSprites();
    MakeScroll();
    
    return true;
}

void Message::InitSprites()
{
    CCSprite* pBlack = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, winSize.width, winSize.height));
    pBlack->setPosition(ccp(0, 0));
    pBlack->setAnchorPoint(ccp(0, 0));
    pBlack->setColor(ccc3(0, 0, 0));
    pBlack->setOpacity(150);
    this->addChild(pBlack, 0);
     
    spriteClass = new SpriteClass();
    
    // background
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_red.png",
                    ccp(0, 0), ccp(14, 1343), CCSize(0, 0), "", "Message", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_x_yellow.png",
                    ccp(0, 0), ccp(875, 1391), CCSize(0, 0), "", "Message", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_title_message.png",
                    ccp(0, 0), ccp(409, 1389), CCSize(0, 0), "", "Message", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png",
                    ccp(0, 0), ccp(49, 458), CCSize(982, 954), "", "Message", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png",
                    ccp(0, 0), ccp(75, 492), CCSize(929, 904), "", "Message", this, 1) );

    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_red.png",
            ccp(0, 0), ccp(319, 191), CCSize(929, 904), "", "Message", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_potion_all_recieve.png",
            ccp(0.5, 0), ccp(spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->getContentSize().width/2, 40), CCSize(0, 0), "button/btn_red.png", "0", NULL, 1) );
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        spriteClass->AddChild(i);
    }
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
    
    int curSize = spriteClass->spriteObj.size();
    
    // make scroll
    CCLayer* scrollContainer = CCLayer::create();
    scrollContainer->setAnchorPoint(ccp(0, 1));
    scrollContainer->setPosition(ccp(77, 492+904));

    char spriteName[35];
    for (int i = 0 ; i < numOfList ; i++)
    {
        CCLayer* itemLayer = CCLayer::create();
        itemLayer->setContentSize(CCSizeMake(862, 166));
        itemLayer->setPosition(ccp(34, (numOfList-i-1)*166));
        scrollContainer->addChild(itemLayer, 2);
        
        // profile 영역 bg
        sprintf(spriteName, "background/bg_profile.png%d", i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, spriteName,
                        ccp(0, 0), ccp(44, 35), CCSize(0, 0), "", "Layer", itemLayer, 3) );
        // text (fontList[2] = 나눔고딕볼드)
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabelArea(msg_content[i].c_str(), fontList[0],
            36, ccp(0, 0), ccp(194, 45), ccc3(78,47,8), CCSize(400, 105),
            kCCTextAlignmentLeft, kCCVerticalTextAlignmentCenter, "", "Layer", itemLayer, 3) );
        // button
        sprintf(spriteName, "button/btn_green_mini.png%d", i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, spriteName,
                        ccp(0, 0), ccp(634, 35), CCSize(0, 0), "", "Layer", itemLayer, 3) );

        // button 안에 image1개, letter1개 넣는다.
        switch (msg_type[i])
        {
            case 0:
                sprintf(spriteName, "button/btn_receive_potion.png%d", i);
                spriteClass->spriteObj.push_back( SpriteObject::Create(0, spriteName,
                            ccp(0, 0), ccp(667, 55), CCSize(0, 0), "", "Layer", itemLayer, 3) );
                sprintf(spriteName, "letter/letter_receive.png%d", i);
                spriteClass->spriteObj.push_back( SpriteObject::Create(0, spriteName,
                            ccp(0, 0), ccp(725, 62), CCSize(0, 0), "", "Layer", itemLayer, 3) );
                break;
            case 1:
                sprintf(spriteName, "button/btn_receive_starcandy.png%d", i);
                spriteClass->spriteObj.push_back( SpriteObject::Create(0, spriteName,
                            ccp(0, 0), ccp(664, 63), CCSize(0, 0), "", "Layer", itemLayer, 3) );
                sprintf(spriteName, "letter/letter_receive.png%d", i);
                spriteClass->spriteObj.push_back( SpriteObject::Create(0, spriteName,
                            ccp(0, 0), ccp(725, 62), CCSize(0, 0), "", "Layer", itemLayer, 3) );
                break;
            case 2:
                sprintf(spriteName, "letter/letter_vote.png%d", i);
                spriteClass->spriteObj.push_back( SpriteObject::Create(0, spriteName,
                            ccp(0, 0), ccp(676, 60), CCSize(0, 0), "", "Layer", itemLayer, 3) );
                break;
        }
        
        // dotted line
        sprintf(spriteName, "background/bg_dotted_line.png%d", i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, spriteName,
                        ccp(0, 0), ccp(0, 5), CCSize(0, 0), "", "Layer", itemLayer, 3) );
    }
    
    for (int i = curSize ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
    
    
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


bool Message::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    if (isTouched)
        return false;
    isTouched = true;
    isScrolling = false;
    
    CCPoint point = pTouch->getLocation();
    //CCLog("DegreeInfo : (%d , %d)", (int)point.x, (int)point.y);
    
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


void Message::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
    //CCPoint point = pTouch->getLocation();
}

void Message::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    isTouched = false;
}

void Message::EndScene()
{
    this->removeFromParentAndCleanup(true);
}


void Message::scrollViewDidScroll(CCScrollView* view)
{
    isScrolling = true;
}

void Message::scrollViewDidZoom(CCScrollView* view)
{
}

