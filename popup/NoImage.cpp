#include "NoImage.h"

static int type;
static int btn;
static std::vector<int> d;

CCScene* NoImage::scene(int popupType, int btnType, std::vector<int> data)
{
    // data
    type = popupType;
    btn = btnType;
    d = data;
    
    CCScene* pScene = CCScene::create();
    NoImage* pLayer = NoImage::create();
    pScene->addChild(pLayer);
    
	return pScene;
}

void NoImage::onEnter()
{
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
    CCLayer::onEnter();
}
void NoImage::onExit()
{
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
}

void NoImage::keyBackClicked()
{
    EndScene();
}


bool NoImage::init()
{
    CCLog("NoImage :: Init");
	if (!CCLayer::init())
	{
		return false;
	}
    
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    // notification post
    CCString* param = CCString::create("1");
    if (type == POPUP_STARCANDY_0)
        CCNotificationCenter::sharedNotificationCenter()->postNotification("BuyStarCandy", param);
    else if (type == POPUP_EXIT)
        CCNotificationCenter::sharedNotificationCenter()->postNotification("Ranking", param);
    
    InitSprites();
    
    return true;
}

void NoImage::InitSprites()
{
    pBlack = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, winSize.width, winSize.height));
    pBlack->setPosition(ccp(0, 0));
    pBlack->setAnchorPoint(ccp(0, 0));
    pBlack->setColor(ccc3(0, 0, 0));
    pBlack->setOpacity(150);
    this->addChild(pBlack, 0);
    
    spriteClass = new SpriteClass();
    
    // pop-up 배경
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png",
                    ccp(0, 0), ccp(49, 640), CCSize(982, 623), "", "NoImage", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png",
                    ccp(0, 0), ccp(76, 678), CCSize(929, 562), "", "NoImage", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_popup_rightup.png",
                    ccp(0, 0), ccp(809, 1039), CCSize(0, 0), "", "NoImage", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_x_brown.png",
                    ccp(0, 0), ccp(900, 1132), CCSize(0, 0), "", "NoImage", this, 1) );
    
    // text
    char text[150], cost[10];
    if (type == POPUP_STARCANDY_0)
        sprintf(text, "토파즈를 사용하여 별사탕 %d개를 구입하시겠습니까?", d[0]);
    else if (type == POPUP_EXIT)
        sprintf(text, "정말 그만둘꼬얌?");
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabelArea(text, fontList[0], 52, ccp(0.5, 0.5), ccp(49+982/2, 640+623/2+50), ccc3(78,47,8), CCSize(982-200, 300), kCCTextAlignmentLeft, kCCVerticalTextAlignmentCenter, "", "NoImage", this, 5) );
    
    // 가격표
    if (type == POPUP_STARCANDY_0)
    {
        spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_degree_desc.png",
                    ccp(0, 0), ccp(493, 723), CCSize(201, 77), "", "NoImage", this, 5) );
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_topaz_mini.png",
                    ccp(0, 0), ccp(513, 730), CCSize(0, 0), "", "NoImage", this, 5) );
        //spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_topaz_mini.png",
        //            ccp(0, 0), ccp(503, 730), CCSize(0, 0), "", "NoImage", this, 5) );
        sprintf(cost, "x %d", 13);
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(cost, fontList[0], 36, ccp(0, 0), ccp(588, 743), ccc3(255,255,255), "", "NoImage", this, 5) );
    }
    
    
    ////// 
    if (btn == BTN_2) // 취소 버튼까지 넣는다.
    {
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_system.png",
                    ccp(0, 0), ccp(126, 711), CCSize(0, 0), "", "NoImage", this, 5) );
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_cancel.png",
            ccp(0.5, 0), ccp(spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->getContentSize().width/2, 30), CCSize(0, 0), "button/btn_system.png", "0", NULL, 5) );
    }
    
    // 확인 버튼
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_red_mini.png",
                    ccp(0, 0), ccp(717, 711), CCSize(0, 0), "", "NoImage", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_confirm_mini.png",
            ccp(0.5, 0), ccp(spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->getContentSize().width/2, 24), CCSize(0, 0), "button/btn_red_mini.png", "0", NULL, 5) );
    
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
}


bool NoImage::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    if (isTouched)
        return false;
    isTouched = true;
    
    CCPoint point = pTouch->getLocation();
    //CCLog("NoImage : (%d , %d)", (int)point.x, (int)point.y);
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        if (spriteClass->spriteObj[i]->name == "button/btn_x_brown.png" ||
            spriteClass->spriteObj[i]->name == "button/btn_system.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
                EndScene();
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_red_mini.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                
                if (type == POPUP_STARCANDY_0)
                    ReplaceScene("NoImage", POPUP_STARCANDY_1, BTN_2);
                else if (type == POPUP_EXIT)
                {
                    CCDirector::sharedDirector()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
                    exit(0);
#endif
                }
            }
        }
    }
    
    return true;
}


void NoImage::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
    //CCPoint point = pTouch->getLocation();
}

void NoImage::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    isTouched = false;
}


void NoImage::ReplaceScene(std::string to, int type, int btnType)
{
    Common::ShowPopup(this, "NoImage", to, true, type, btnType, d);
    this->removeFromParentAndCleanup(true);
}

void NoImage::EndScene()
{
    sound->playClick();
    
    CCString* param = CCString::create("0");
    if (type == POPUP_STARCANDY_0 || type == POPUP_STARCANDY_1 || type == POPUP_STARCANDY_2)
        CCNotificationCenter::sharedNotificationCenter()->postNotification("BuyStarCandy", param);
    else if (type == POPUP_EXIT)
        CCNotificationCenter::sharedNotificationCenter()->postNotification("Ranking", param);
    
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    
    this->removeFromParentAndCleanup(true);
}

void NoImage::EndSceneCallback()
{
}

