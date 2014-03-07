#include "BuyTopaz.h"

CCScene* BuyTopaz::scene()
{
    CCScene* pScene = CCScene::create();
    BuyTopaz* pLayer = BuyTopaz::create();
    pScene->addChild(pLayer);
    
	return pScene;
}

void BuyTopaz::onEnter()
{
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
    CCLayer::onEnter();
}
void BuyTopaz::onExit()
{
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
}

void BuyTopaz::keyBackClicked()
{
    CCDirector::sharedDirector()->end();
}


bool BuyTopaz::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
    CCLog("BuyTopaz. init");
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    InitSprites();
    MakeScroll();
    
    return true;
}

void BuyTopaz::InitSprites()
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
                    ccp(0, 0), ccp(14, 1586), CCSize(0, 0), "", "BuyTopaz", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_x_yellow.png",
                    ccp(0, 0), ccp(875, 1391+243), CCSize(0, 0), "", "BuyTopaz", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_title_purchase_topaz.png",
                    ccp(0, 0), ccp(269, 1389+243), CCSize(0, 0), "", "BuyTopaz", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png",
                    ccp(0, 0), ccp(49, 458-45), CCSize(982, 954+243+45), "", "BuyTopaz", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png",
                    ccp(0, 0), ccp(75, 492-45), CCSize(929, 904+243+45), "", "BuyTopaz", this, 1) );
    
    // button
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_green.png",
                    ccp(0, 0), ccp(319, 191), CCSize(0, 0), "", "BuyTopaz", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_request.png",
                    ccp(0.5, 0), ccp(spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->
                    getContentSize().width/2, 56), CCSize(0, 0), "button/btn_green.png", "0", NULL, 1) );
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        spriteClass->AddChild(i);
    }
}

void BuyTopaz::MakeScroll()
{
    spriteName.push_back("icon/icon_purchase_topaz_a.png");
    spriteName.push_back("icon/icon_purchase_topaz_b.png");
    spriteName.push_back("icon/icon_purchase_topaz_c.png");
    spriteName.push_back("icon/icon_purchase_topaz_d.png");
    spriteName.push_back("icon/icon_purchase_topaz_e.png");
    cost.push_back("20");
    cost.push_back("65");
    cost.push_back("300");
    cost.push_back("500");
    cost.push_back("800");
    
    int spriteClassSize = spriteClass->spriteObj.size();
    
    // make scroll
    CCLayer* scrollContainer = CCLayer::create();
    //scrollContainer->setAnchorPoint(ccp(0, 0));
    //scrollContainer->setPosition(ccp(77, 492+904+243));
    scrollContainer->setPosition(ccp(77, 492));
    ////
    this->addChild(scrollContainer, 2);
    ////
    
    int numOfList = 5;
    char name[50], name2[50];
    for (int i = 0 ; i < numOfList ; i++)
    {
        CCLayer* itemLayer = CCLayer::create();
        itemLayer->setContentSize(CCSizeMake(862, 226));
        itemLayer->setPosition(ccp(34, (numOfList-i-1)*226));
        if (i == numOfList-1)
            itemLayer->setPosition(ccp(34, (numOfList-i-1)*226-30));
        scrollContainer->addChild(itemLayer, 2);
        
        // image
        CCPoint pos = ccp(35, 21);
        if (i == 0)
            pos = ccp(69, 70);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, spriteName[i], ccp(0, 0),
                            pos, CCSize(0, 0), "", "Layer", itemLayer, 3) );
        
        // number + salenumber
        CCLayer* numberLayer = Common::MakeImageNumberLayer(cost[i]);
        numberLayer->setPosition(ccp(214, 126));
        itemLayer->addChild(numberLayer, 3);
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("+ 10%", fontList[0], 36,
                        ccp(0, 0), ccp(348, 138), ccc3(168,122,62), "", "Layer", itemLayer, 3) );

        // 가격
        sprintf(name, "background/bg_degree_desc.png%d", i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(1, name, ccp(0, 0), ccp(226, 18),
                        CCSize(201, 77), "", "Layer", itemLayer, 3) );
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("₩ 35,700", fontList[0], 36,
            ccp(0.5, 0.5), ccp(326, 57), ccc3(255,255,255), "", "Layer", itemLayer, 3) );
        
        // button (yellow, present, green, letter 순)
        sprintf(name, "button/btn_yellow_mini2.png%d", i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, name,
                        ccp(0, 0), ccp(472, 62), CCSize(0, 0), "", "Layer", itemLayer, 3) );
        sprintf(name2, "button/btn_present.png%d", i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, name2,
            ccp(0.5, 0), ccp(spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->getContentSize().width/2, 24), CCSize(0, 0), name, "0", NULL, 3) );
        sprintf(name, "button/btn_green_mini.png%d", i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, name,
                        ccp(0, 0), ccp(634, 62), CCSize(0, 0), "", "Layer", itemLayer, 3) );
        sprintf(name2, "letter/letter_purchase.png%d", i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, name2,
            ccp(0.5, 0), ccp(spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->getContentSize().width/2, 24), CCSize(0, 0), name, "0", NULL, 3) );
        
        // dotted line
        if (i < numOfList-1)
        {
            sprintf(name, "background/bg_dotted_line.png%d", i);
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, name,
                        ccp(0, 0), ccp(0, 0), CCSize(0, 0), "", "Layer", itemLayer, 3) );
        }
    }
    
    // addchild
    for (int i = spriteClassSize ; i < spriteClass->spriteObj.size() ; i++)
    {
        spriteClass->AddChild(i);
    }
    
    /*
    // scrollview 내용 전체크기
    scrollContainer->setContentSize(CCSizeMake(862, numOfList*226));
    // scrollView 생성
    scrollView = CCScrollView::create();
    scrollView->retain();
    scrollView->setDirection(kCCScrollViewDirectionVertical);
    scrollView->setViewSize(CCSizeMake(929, 904-40)); // (내용 1개 크기, 노란보드 세로크기)
    scrollView->setContentSize(scrollContainer->getContentSize());
    scrollView->setAnchorPoint(ccp(0, 0));
    scrollView->setPosition(ccp(77, 492+20));
    scrollView->setContainer(scrollContainer);
    scrollView->setDelegate(this);
    scrollView->setContentOffset(ccp(0, 904-40-(numOfList*226)), false);
    this->addChild(scrollView, 3);
     */
}


bool BuyTopaz::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
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
        else if (spriteClass->spriteObj[i]->name.substr(0, 27) == "button/btn_yellow_mini2.png")
        {
            CCSprite* sp = spriteClass->spriteObj[i]->sprite;
            CCPoint p = spriteClass->spriteObj[i]->sprite->convertToNodeSpace(point);
            //CCLog("%d, %d", (int)p.x, (int)p.y);
            // convertToNodeSpace 설명
            // ex) sprite1->convertToNodeSpace(sprite2->getPosition());
            // sprite1 좌측하단부터 sprite2의 anchorPoint까지의 거리
            if (p.x >= 0 && p.y >= 0 && p.x < sp->getContentSize().width && p.y < sp->getContentSize().height)
                Common::ShowNextScene(this, "BuyTopaz", "SendTopaz", false);
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_green.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
                Common::ShowNextScene(this, "BuyTopaz", "RequestTopaz", false);
        }
    }
    
    return true;
}


void BuyTopaz::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
    //CCPoint point = pTouch->getLocation();
}

void BuyTopaz::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    isTouched = false;
}

void BuyTopaz::EndScene()
{
    this->removeFromParentAndCleanup(true);
}


void BuyTopaz::scrollViewDidScroll(CCScrollView* view)
{
    isScrolling = true;
}

void BuyTopaz::scrollViewDidZoom(CCScrollView* view)
{
}
