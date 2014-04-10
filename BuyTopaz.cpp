#include "BuyTopaz.h"

static int parent_id;

CCScene* BuyTopaz::scene(int parent)
{
    parent_id = parent;
    
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
    EndScene();
}


bool BuyTopaz::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
    // notification observer
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(BuyTopaz::Notification), "BuyTopaz", NULL);
    
    // notification post
    CCString* param = CCString::create("1");
    if (parent_id == 0) // 부모가 'Ranking'
        CCNotificationCenter::sharedNotificationCenter()->postNotification("Ranking", param);
    else if (parent_id == 1) // 부모가 'GameReady'
        CCNotificationCenter::sharedNotificationCenter()->postNotification("GameReady", param);
    else if (parent_id == 2) // 부모가 'BuyPotion'
        CCNotificationCenter::sharedNotificationCenter()->postNotification("BuyPotion", param);
    
    CCLog("BuyTopaz. init");
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    InitSprites();
    MakeScroll();
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
    
    isTouched = false;
    
    return true;
}

void BuyTopaz::Notification(CCObject* obj)
{
    CCString* param = (CCString*)obj;
    
    if (param->intValue() == 0)
    {
        // 터치 활성
        this->setKeypadEnabled(true);
        this->setTouchEnabled(true);
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
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

void BuyTopaz::InitSprites()
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
                    getContentSize().width/2, 56), CCSize(0, 0), "button/btn_green.png", "0", NULL, 1, 1) );
}

void BuyTopaz::MakeScroll()
{    
    // make scroll
    itemContainer = CCLayer::create();
    itemContainer->setPosition(ccp(77, 492));
    this->addChild(itemContainer, 2);
    
    int numOfList = priceTopaz.size();
    
    char name[50], name2[50];
    for (int i = 0 ; i < numOfList ; i++)
    {
        CCLayer* itemLayer = CCLayer::create();
        layers.push_back(itemLayer);
        itemLayer->setContentSize(CCSizeMake(862, 226));
        itemLayer->setPosition(ccp(34, (numOfList-i-1)*226));
        itemContainer->addChild(itemLayer, 2);
        
        // 토파즈 병 이미지
        CCPoint pos;
        if (i == 0) pos = ccp(69, 70);
        else if (i == 1) pos = ccp(35, 21);
        else if (i == 2) pos = ccp(29, 21);
        else if (i == 3) pos = ccp(30, 21);
        else if (i == 4) pos = ccp(24, 21-30);
            
        sprintf(name, "icon/icon_purchase_topaz_%c.png", 'a'+i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), pos, CCSize(0, 0), "", "Layer", itemLayer, 3) );
        
        // 토파즈 개수
        sprintf(name, "%d", priceTopaz[i]->GetCount());
        CCLayer* numberLayer = Common::MakeImageNumberLayer(name, 0);
        numberLayer->setPosition(ccp(214, 126));
        numberLayers.push_back(numberLayer);
        itemLayer->addChild(numberLayer, 3);
        
        // 보너스 숫자
        sprintf(name, "+ %d%%", priceTopaz[i]->GetBonus());
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(name, fontList[0], 36, ccp(0, 0), ccp(348, 138), ccc3(168,122,62), "", "Layer", itemLayer, 3) );

        // 가격
        sprintf(name, "background/bg_degree_desc.png%d", i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(1, name, ccp(0, 0), ccp(226, 18), CCSize(201, 77), "", "Layer", itemLayer, 3) );

        std::string cost = "";
        if (myInfo->GetDeviceType() == 1) // ANDROID
        {
            cost = "₩ " + Common::MakeComma(priceTopaz[i]->GetPrice());
        }
        else if (myInfo->GetDeviceType() == 2) // iPHONE
        {
            cost = "$ " + Common::MakeComma(priceTopaz[i]->GetPrice()/100) + "." + Common::MakeComma(priceTopaz[i]->GetPrice()%100);
        }
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(cost, fontList[0], 36, ccp(0.5, 0.5), ccp(326, 57), ccc3(255,255,255), "", "Layer", itemLayer, 3) );
        
        
        // button (yellow, present, green, letter 순서)
        sprintf(name, "button/btn_yellow_mini2.png%d", i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(472, 62), CCSize(0, 0), "", "Layer", itemLayer, 3) );
        sprintf(name2, "button/btn_present.png%d", i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0.5, 0), ccp(spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->getContentSize().width/2, 24), CCSize(0, 0), name, "0", NULL, 3) );
        sprintf(name, "button/btn_green_mini.png%d", i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(634, 62), CCSize(0, 0), "", "Layer", itemLayer, 3) );
        sprintf(name2, "letter/letter_purchase.png%d", i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0.5, 0), ccp(spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->getContentSize().width/2, 24), CCSize(0, 0), name, "0", NULL, 3) );
        
        // dotted line
        if (i < numOfList-1)
        {
            sprintf(name, "background/bg_dotted_line.png%d", i);
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, name,
                        ccp(0, 0), ccp(0, 0), CCSize(0, 0), "", "Layer", itemLayer, 3) );
        }
    }
}


bool BuyTopaz::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
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
                EndScene();
                break;
            }
        }
        else if (spriteClass->spriteObj[i]->name.substr(0, 27) == "button/btn_yellow_mini2.png")
        {
            CCPoint p = spriteClass->spriteObj[i]->sprite->convertToNodeSpace(point);
            CCSize size = spriteClass->spriteObj[i]->sprite->getContentSize();
            if ((int)p.x >= 0 && (int)p.y >= 0 && (int)p.x <= size.width && (int)p.y <= size.height)
            {
                sound->playClick();
                Common::ShowNextScene(this, "BuyTopaz", "SendTopaz", false);
            }
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_green.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                Common::ShowNextScene(this, "BuyTopaz", "RequestTopaz", false);
            }
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
    sound->playClick();
    
    CCString* param = CCString::create("0");
    if (parent_id == 0) // 부모가 'Ranking'
        CCNotificationCenter::sharedNotificationCenter()->postNotification("Ranking", param);
    else if (parent_id == 1) // 부모가 'GameReady'
        CCNotificationCenter::sharedNotificationCenter()->postNotification("GameReady", param);
    else if (parent_id == 2) // 부모가 'BuyPotion'
        CCNotificationCenter::sharedNotificationCenter()->postNotification("BuyPotion", param);
    
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, "BuyTopaz");
    
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    
    
    // remove children in all layers
    for (int i = 0 ; i < numberLayers.size(); i++)
        numberLayers[i]->removeAllChildren();
    for (int i = 0 ; i < layers.size(); i++)
        layers[i]->removeAllChildren();
    numberLayers.clear();
    layers.clear();
    
    itemContainer->removeAllChildren();
    
    // remove all CCNodes
    spriteClass->RemoveAllObjects();
    delete spriteClass;
    
    // end scene
    this->removeFromParentAndCleanup(true);
    
    CCTextureCache::sharedTextureCache()->removeTextureForKey("images/ranking_scrollbg.png");
}

void BuyTopaz::EndSceneCallback()
{
}

