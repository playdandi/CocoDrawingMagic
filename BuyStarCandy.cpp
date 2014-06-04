#include "BuyStarCandy.h"

static int parent_id;

CCScene* BuyStarCandy::scene(int parent)
{
    parent_id = parent;
    
    CCScene* pScene = CCScene::create();
    BuyStarCandy* pLayer = BuyStarCandy::create();
    pScene->addChild(pLayer);
    
	return pScene;
}

void BuyStarCandy::onEnter()
{
    CCLog("BuyStarCandy : onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority(), true);
    CCLayer::onEnter();
}
void BuyStarCandy::onExit()
{
    CCLog("BuyStarCandy : onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
}

void BuyStarCandy::keyBackClicked()
{
    sound->playClick();
    EndScene();
}


bool BuyStarCandy::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
    // make depth tree
    Depth::AddCurDepth("BuyStarCandy", this);
    
    this->setTouchEnabled(true);
    this->setKeypadEnabled(true);
    this->setTouchPriority(Depth::GetCurPriority());
    CCLog("BuyStarCandy : touch prio = %d", this->getTouchPriority());
    
    // notification observer
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(BuyStarCandy::Notification), Depth::GetCurName(), NULL);
    
    // notification post
    CCString* param = CCString::create("1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetParentName(), param);
  
    
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    InitSprites();
    MakeScroll();
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
    
    isTouched = false;
    
    return true;
}

void BuyStarCandy::Notification(CCObject* obj)
{
    CCString* param = (CCString*)obj;
    
    if (param->intValue() == 0)
    {
        // 터치 활성
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority()+1, true);
        this->setTouchPriority(Depth::GetCurPriority());
        isTouched = false;
        CCLog("BuyStarCandy : 터치 활성 (Priority = %d)", this->getTouchPriority());
    }
    else if (param->intValue() == 1)
    {
        // 터치 비활성
        CCLog("BuyStarCandy 터치 비활성");
        CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
    }
    else if (param->intValue() == 10)
    {
        // 터치 풀기 (백그라운드에서 돌아올 때)
        isTouched = false;
    }
}


void BuyStarCandy::InitSprites()
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
                    ccp(0, 0), ccp(14, 1586), CCSize(0, 0), "", "BuyStarCandy", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_x_yellow.png",
                    ccp(0, 0), ccp(875, 1391+243), CCSize(0, 0), "", "BuyStarCandy", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_title_purchase_starcandy.png",
                    ccp(0, 0), ccp(269, 1389+243), CCSize(0, 0), "", "BuyStarCandy", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png",
                    ccp(0, 0), ccp(49, 458-45), CCSize(982, 954+243+45), "", "BuyStarCandy", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png",
                    ccp(0, 0), ccp(75, 492-45), CCSize(929, 904+243+45), "", "BuyStarCandy", this, 1) );
}

void BuyStarCandy::MakeScroll()
{
    // make scroll
    itemContainer = CCLayer::create();
    itemContainer->setPosition(ccp(77, 492));
    this->addChild(itemContainer, 2);
    
    int numOfList = priceStarCandy.size();
    
    char name[50], name2[50];
    for (int i = 0 ; i < numOfList ; i++)
    {
        CCLayer* itemLayer = CCLayer::create();
        itemLayer->setContentSize(CCSizeMake(862, 226));
        itemLayer->setPosition(ccp(34, (numOfList-i-1)*226));
        itemContainer->addChild(itemLayer, 2);
        
        // 별사탕 병 이미지
        CCPoint pos;
        if (i == 0) pos = ccp(69, 70);
        else if (i == 1) pos = ccp(35, 21);
        else if (i == 2) pos = ccp(32, 21);
        else if (i == 3) pos = ccp(30, 21);
        else if (i == 4) pos = ccp(24, 21-30);
        sprintf(name, "icon/icon_purchase_starcandy_%c.png", 'a'+i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), pos, CCSize(0, 0), "", "Layer", itemLayer, 3) );
        
        // 별사탕 개수
        sprintf(name, "%d", priceStarCandy[i]->GetCount());
        CCLayer* numberLayer = Common::MakeImageNumberLayer(name, 0);
        numberLayer->setPosition(ccp(214, 109+15));
        itemLayer->addChild(numberLayer, 3);
        
        spriteClass->layers.push_back(numberLayer);
        spriteClass->layers.push_back(itemLayer);
        
        // 보너스 숫자
        CCPoint bonus_pos;
        if (i == 0) bonus_pos = ccp(428, 130);
        else if (i == 1) bonus_pos = ccp(463, 130);
        else if (i == 2) bonus_pos = ccp(468, 130);
        else if (i == 3) bonus_pos = ccp(488, 130);
        else if (i == 4) bonus_pos = ccp(498, 130);
        sprintf(name, "+ %d%%", priceStarCandy[i]->GetBonus());
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(name, fontList[0], 36, ccp(0, 0), bonus_pos, ccc3(168,122,62), "", "Layer", itemLayer, 3) );
        
        // 가격 (배경, 토파즈아이콘, 숫자)
        sprintf(name, "background/bg_degree_desc.png%d", i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(1, name, ccp(0, 0), ccp(226, 18), CCSize(201, 77), "", "Layer", itemLayer, 3) );
        sprintf(name2, "icon/icon_topaz_mini.png%d", i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0, 0), ccp(16, 5), CCSize(0, 0), name, "1", NULL, 3) );
        sprintf(name, "%d", priceStarCandy[i]->GetPrice());
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(name, fontList[0], 36, ccp(0, 0), ccp(311, 37), ccc3(255,255,255), "", "Layer", itemLayer, 3) );
        
        // button
        sprintf(name, "button/btn_green_mini.png%d", i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(634, 62), CCSize(0, 0), "", "Layer", itemLayer, 3) );
        sprintf(name2, "letter/letter_purchase.png%d", i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0.5, 0), ccp(spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->getContentSize().width/2, 24), CCSize(0, 0), name, "0", NULL, 3) );
        
        // dotted line
        if (i < numOfList-1)
        {
            sprintf(name, "background/bg_dotted_line.png%d", i);
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(0, 0), CCSize(0, 0), "", "Layer", itemLayer, 3) );
        }
    }
}


bool BuyStarCandy::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    if (isTouched)
        return false;
    isTouched = true;
    
    CCPoint point = pTouch->getLocation();
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        if (spriteClass->spriteObj[i]->name == "button/btn_x_yellow.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                EndScene();
                break;
            }
        }
        else if (spriteClass->spriteObj[i]->name.substr(0, 25) == "button/btn_green_mini.png")
        {
            CCPoint p = spriteClass->spriteObj[i]->sprite->convertToNodeSpace(point);
            CCSize size = spriteClass->spriteObj[i]->sprite->getContentSize();
            if ((int)p.x >= 0 && (int)p.y >= 0 && (int)p.x <= size.width && (int)p.y <= size.height)
            {
                sound->playClick();
                
                std::vector<int> data;
                std::string number = spriteClass->spriteObj[i]->name.substr(25);
                int idx = std::atoi(number.c_str());
                data.push_back(idx);
                data.push_back(priceStarCandy[idx]->GetPrice());
                
                Common::ShowPopup(this, "BuyStarCandy", "NoImage", false, BUY_STARCANDY_TRY, BTN_2, data);
                break;
            }
        }
    }
    
    return true;
}


void BuyStarCandy::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
}

void BuyStarCandy::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    isTouched = false;
}


void BuyStarCandy::EndScene()
{
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
    itemContainer->removeAllChildren();
    pBlack->removeFromParentAndCleanup(true);
    
    this->removeFromParentAndCleanup(true);
}



