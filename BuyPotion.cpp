#include "BuyPotion.h"

static int parent_id;

CCScene* BuyPotion::scene(int parent)
{
    parent_id = parent;
    
    CCScene* pScene = CCScene::create();
    BuyPotion* pLayer = BuyPotion::create();
    pScene->addChild(pLayer);
    
	return pScene;
}

void BuyPotion::onEnter()
{
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
    CCLayer::onEnter();
}
void BuyPotion::onExit()
{
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
}

void BuyPotion::keyBackClicked()
{
    sound->playClick();
    EndScene();
}


bool BuyPotion::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
    //CCLog("BuyPotion. init");
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    // notification observer
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(BuyPotion::Notification), "BuyPotion", NULL);
    
    // notification post
    CCString* param = CCString::create("1");
    if (parent_id == 0) // 부모가 'Ranking'
        CCNotificationCenter::sharedNotificationCenter()->postNotification("Ranking", param);
    else if (parent_id == 1) // 부모가 'GameReady'
        CCNotificationCenter::sharedNotificationCenter()->postNotification("GameReady", param);
    
    InitSprites();
    
    isTouched = false;
    
    return true;
}

void BuyPotion::Notification(CCObject* obj)
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


void BuyPotion::InitSprites()
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
                    ccp(0, 0), ccp(14, 1343), CCSize(0, 0), "", "BuyPotion", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_x_yellow.png",
                    ccp(0, 0), ccp(875, 1391), CCSize(0, 0), "", "BuyPotion", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_title_purchase_potion.png",
                    ccp(0, 0), ccp(309, 1389), CCSize(0, 0), "", "BuyPotion", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png",
                    ccp(0, 0), ccp(49, 458), CCSize(982, 954), "", "BuyPotion", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png",
                    ccp(0, 0), ccp(75, 492), CCSize(929, 904), "", "BuyPotion", this, 1) );
    
    // 내용
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_degree_desc.png",
                    ccp(0, 0), ccp(106, 528), CCSize(857, 132), "", "BuyPotion", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_topaz.png",
                    ccp(0, 0), ccp(50, 16), CCSize(0, 0), "background/bg_degree_desc.png", "1", NULL, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_potion_big.png",
                    ccp(0, 0), ccp(380, 18), CCSize(0, 0), "background/bg_degree_desc.png", "1", NULL, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("토파즈 5개를", fontList[0], 36, ccp(0, 0),
                    ccp(145, 47), ccc3(78,47,8), "background/bg_degree_desc.png", "1", NULL, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("포션 5개로 교환하기", fontList[0], 36,
                ccp(0, 0), ccp(470, 47), ccc3(78,47,8), "background/bg_degree_desc.png", "1", NULL, 2) );
    
    // 코코 그림
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "image/coco_potion.png", ccp(0.5, 0.5), ccp(75+929/2, 492+904/2+70), CCSize(0, 0), "", "BuyPotion", this, 5) );
    ((CCSprite*)spriteClass->FindSpriteByName("image/coco_potion.png"))->setScale(1.15f);
    
    // button
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_green.png1",
                    ccp(0, 0), ccp(71, 193), CCSize(0, 0), "", "BuyPotion", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_request.png",
                    ccp(0.5, 0), ccp(spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->
                    getContentSize().width/2, 56), CCSize(0, 0), "button/btn_green.png1", "0", NULL, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_green.png2",
                    ccp(0, 0), ccp(568, 193), CCSize(0, 0), "", "BuyPotion", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_exchange.png",
                    ccp(0.5, 0), ccp(spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->
                    getContentSize().width/2, 40), CCSize(0, 0), "button/btn_green.png2", "0", NULL, 1) );
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
}


bool BuyPotion::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
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
        else if (spriteClass->spriteObj[i]->name == "button/btn_green.png1")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                Common::ShowNextScene(this, "BuyPotion", "RequestPotion", false);
            }
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_green.png2")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                std::vector<int> nullData;
                Common::ShowPopup(this, "BuyPotion", "NoImage", false, BUYPOTION_1, BTN_2, nullData);
            }
        }
    }
    
    return true;
}


void BuyPotion::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
    //CCPoint point = pTouch->getLocation();
}

void BuyPotion::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    isTouched = false;
}


void BuyPotion::EndScene()
{
    CCString* param = CCString::create("0");
    if (parent_id == 0) // 부모가 'Ranking'
        CCNotificationCenter::sharedNotificationCenter()->postNotification("Ranking", param);
    else if (parent_id == 1) // 부모가 'GameReady'
        CCNotificationCenter::sharedNotificationCenter()->postNotification("GameReady", param);
    
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, "BuyPotion");
    
    this->removeFromParentAndCleanup(true);
}

void BuyPotion::EndSceneCallback()
{
}


