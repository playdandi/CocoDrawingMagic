#include "BuyPotion.h"

//static int parent_id;

CCScene* BuyPotion::scene(int parent)
{
    //parent_id = parent;
    
    CCScene* pScene = CCScene::create();
    BuyPotion* pLayer = BuyPotion::create();
    pScene->addChild(pLayer);
    
	return pScene;
}

void BuyPotion::onEnter()
{
    CCLog("BuyPotion : onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority(), true);
    CCLayer::onEnter();
}
void BuyPotion::onExit()
{
    CCLog("BuyPotion : onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
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
    
    // make depth tree
    Depth::AddCurDepth("BuyPotion", this);
    
    this->setTouchEnabled(true);
    this->setKeypadEnabled(true);
    this->setTouchPriority(Depth::GetCurPriority());
    CCLog("BuyPotion : touch prio = %d", this->getTouchPriority());
    
    // notification observer
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(BuyPotion::Notification), Depth::GetCurName(), NULL);
    
    // notification post
    CCString* param = CCString::create("1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetParentName(), param);

    
    winSize = CCDirector::sharedDirector()->getWinSize();
    
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
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority()+1, true);
        this->setTouchPriority(Depth::GetCurPriority());
        isTouched = false;
        CCLog("BuyPotion : 터치 활성 (Priority = %d)", this->getTouchPriority());
    }
    else if (param->intValue() == 1)
    {
        // 터치 비활성
        CCLog("BuyPotion 터치 비활성");
        CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
    }
}


void BuyPotion::InitSprites()
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
                    ccp(0, 0), ccp(14, 1343), CCSize(0, 0), "", "BuyPotion", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_x_yellow.png",
                    ccp(0, 0), ccp(875, 1391), CCSize(0, 0), "", "BuyPotion", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_title_purchase_potion.png",
                    ccp(0, 0), ccp(309, 1389), CCSize(0, 0), "", "BuyPotion", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png",
                    ccp(0, 0), ccp(49, 458), CCSize(982, 954), "", "BuyPotion", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png",
                    ccp(0, 0), ccp(75, 492), CCSize(929, 904), "", "BuyPotion", this, 1) );
    CCSprite* bg = CCSprite::create("images/buyPotion_bg.png");
    bg->setAnchorPoint(ccp(0, 0));
    bg->setPosition(ccp(75, 528+132+28));
    this->addChild(bg, 2);
    
    // 내용
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_degree_desc.png",
                    ccp(0, 0), ccp(110, 528), CCSize(857, 132), "", "BuyPotion", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_topaz.png",
                    ccp(0, 0), ccp(50, 16), CCSize(0, 0), "background/bg_degree_desc.png", "1", NULL, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_potion_big.png",
                    ccp(0, 0), ccp(380, 18), CCSize(0, 0), "background/bg_degree_desc.png", "1", NULL, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("토파즈 5개를", fontList[0], 36, ccp(0, 0),
                    ccp(145, 47), ccc3(78,47,8), "background/bg_degree_desc.png", "1", NULL, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("포션 5개로 교환하기", fontList[0], 36,
                ccp(0, 0), ccp(470, 47), ccc3(78,47,8), "background/bg_degree_desc.png", "1", NULL, 2) );
    
    // 코코 그림
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "image/coco_potion.png", ccp(0.5, 0.5), ccp(75+929/2+50, 492+904/2+45), CCSize(0, 0), "", "BuyPotion", this, 5) );
    ((CCSprite*)spriteClass->FindSpriteByName("image/coco_potion.png"))->setScale(1.05f);
    
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
                break;
            }
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_green.png2")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                std::vector<int> data;
                data.push_back(0);
                data.push_back(5);
                Common::ShowPopup(this, "BuyPotion", "NoImage", false, BUYPOTION_1, BTN_2, data);
                break;
            }
        }
    }
    
    return true;
}


void BuyPotion::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
}

void BuyPotion::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    isTouched = false;
}


void BuyPotion::EndScene()
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
    pBlack->removeFromParentAndCleanup(true);
    
    CCTextureCache::sharedTextureCache()->removeTextureForKey("images/buyPotion_bg.png");
    
    this->removeFromParentAndCleanup(true);
}


