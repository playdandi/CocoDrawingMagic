#include "Setting.h"

CCScene* Setting::scene()
{
    CCScene* pScene = CCScene::create();
    Setting* pLayer = Setting::create();
    pScene->addChild(pLayer);
    
	return pScene;
}

void Setting::onEnter()
{
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
    CCLayer::onEnter();
}
void Setting::onExit()
{
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
}

void Setting::keyBackClicked()
{
    EndScene();
}


bool Setting::init()
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
    
    selectedBtn = -1;
    
    this->setKeypadEnabled(true);
    this->setTouchEnabled(true);
    
    isTouched = false;
    
    return true;
}

void Setting::InitSprites()
{
    CCSprite* pBlack = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, winSize.width, winSize.height));
    pBlack->setPosition(ccp(0, 0));
    pBlack->setAnchorPoint(ccp(0, 0));
    pBlack->setColor(ccc3(0, 0, 0));
    pBlack->setOpacity(150);
    this->addChild(pBlack, 0);
    
    spriteClass = new SpriteClass();
    
    // strap
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_red.png",
                    ccp(0, 0), ccp(14, 1343), CCSize(0, 0), "", "Setting", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_x_yellow.png",
                    ccp(0, 0), ccp(875, 1391), CCSize(0, 0), "", "Setting", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_title_setting.png",
                    ccp(0, 0), ccp(409, 1389), CCSize(0, 0), "", "Setting", this, 2) );
    
    // background
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png",
                    ccp(0, 0), ccp(49, 147), CCSize(982, 1265), "", "Setting", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png1",
                    ccp(0, 0), ccp(75, 492+270), CCSize(929, 904-270), "", "Setting", this, 1) );
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png2", // ver
                    ccp(0, 0), ccp(77, 640), CCSize(643, 97), "", "Setting", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png3", // id
                    ccp(0, 0), ccp(77, 326), CCSize(926, 97), "", "Setting", this, 1) );
    /*
    //{{1052,281},{312,266}}
    CCScale9Sprite* y1 = CCScale9Sprite::createWithSpriteFrameName("background/bg_board_yellow.png",
                                                                   CCRectMake(100, 80, 100, 10));
    y1->setAnchorPoint(ccp(0, 0));
    y1->setPosition(ccp(77, 640));
    y1->setContentSize(CCSize(643, 97));
    this->addChild(y1, 3);
     */
    
    
    // text (version, kakaoID)
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("게임버전 : 1.0.0 ver", fontList[0], 36, ccp(0, 0), ccp(107, 670), ccc3(78,47,8), "", "Setting", this, 4) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("카카오 ID : 123456789012", fontList[0], 36, ccp(0, 0), ccp(107, 356), ccc3(78,47,8), "", "Setting", this, 4) );
    
    // button : credit, coupon, tutorial, cc, logout, idcopy
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_purple_mini.png1",
                    ccp(0, 0), ccp(737, 633), CCSize(0, 0), "", "Setting", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_maker.png",
                ccp(0.5, 0), ccp(spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->
                getContentSize().width/2, 36), CCSize(0, 0), "button/btn_purple_mini.png1", "0", NULL, 1) );
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_purple_mini.png2",
                ccp(0, 0), ccp(96, 492), CCSize(0, 0), "", "Setting", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_coupon.png",
                ccp(0.5, 0), ccp(spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->
                getContentSize().width/2, 32), CCSize(0, 0), "button/btn_purple_mini.png2", "0", NULL, 1) );
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_purple_mini.png3",
                ccp(0, 0), ccp(423, 492), CCSize(0, 0), "", "Setting", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_tutorial.png",
                ccp(0.5, 0), ccp(spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->
                getContentSize().width/2, 32), CCSize(0, 0), "button/btn_purple_mini.png3", "0", NULL, 1) );
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_purple_mini.png4",
                ccp(0, 0), ccp(737, 492), CCSize(0, 0), "", "Setting", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_service.png",
                ccp(0.5, 0), ccp(spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->
                getContentSize().width/2, 32), CCSize(0, 0), "button/btn_purple_mini.png4", "0", NULL, 1) );
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_system.png1",
                    ccp(0, 0), ccp(82, 192), CCSize(0, 0), "", "Setting", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_logout.png",
                    ccp(0.5, 0), ccp(spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->
                    getContentSize().width/2, 32), CCSize(0, 0), "button/btn_system.png1", "0", NULL, 1) );
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_system.png2",
                    ccp(0, 0), ccp(779, 192), CCSize(0, 0), "", "Setting", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_idcopy.png",
                    ccp(0.5, 0), ccp(spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->
                    getContentSize().width/2, 36), CCSize(0, 0), "button/btn_system.png2", "0", NULL, 1) );
    
    // text
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("효과음", fontList[0], 48, ccp(0, 0), ccp(162, 1277), ccc3(78,47,8), "", "Setting", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("배경음", fontList[0], 48, ccp(0, 0), ccp(162, 1168), ccc3(78,47,8), "", "Setting", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("카톡메시지수신", fontList[0], 48, ccp(0, 0), ccp(162, 1057), ccc3(78,47,8), "", "Setting", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("푸시알림", fontList[0], 48, ccp(0, 0), ccp(162, 942), ccc3(78,47,8), "", "Setting", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("포션,그룹메시지", fontList[0], 48, ccp(0, 0), ccp(162, 829), ccc3(78,47,8), "", "Setting", this, 2) );
    
    
    // on-off button
    char name[50], name2[50], key[20];
    bool status;
    CCPoint pos;
    for (int i = 0; i < 5; i++)
    {
        sprintf(name, "background/bg_degree_desc.png%d", i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(1, name,
                ccp(0, 0), ccp(627, 1261-i*112), CCSize(300, 82), "", "Setting", this, 2) );
        sprintf(name2, "letter/letter_on.png%d", i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, name2,
                ccp(0, 0), ccp(38, 18), CCSize(0, 0), name, "1", NULL, 2) );
        sprintf(name2, "letter/letter_off.png%d", i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, name2,
                ccp(0, 0), ccp(168, 18), CCSize(0, 0), name, "1", NULL, 2) );
      
        sprintf(key, "setting_option_%d", i);
        status = CCUserDefault::sharedUserDefault()->getBoolForKey(key);
        if (status)
            pos = ccp(627, 1261-i*112);
        else
            pos = ccp(627+150, 1261-i*112);
        sprintf(name2, "button/btn_option.png%d", i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, name2,
                ccp(0, 0), pos, CCSize(0, 0), "", "Setting", this, 3) );
    }
    standardBtnPos = ccp(627, 0);
    
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
}



bool Setting::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
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
                EndScene();
        }
        else if (spriteClass->spriteObj[i]->name.substr(0, 21) == "button/btn_option.png") // on-off's
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                selectedSprite = spriteClass->spriteObj[i]->sprite;
                selectedBtn = spriteClass->spriteObj[i]->name[spriteClass->spriteObj[i]->name.size()-1] - '0';
                selectedPos = spriteClass->spriteObj[i]->sprite->getPosition();
                standardBtnPos.y = selectedPos.y;
                selectedTouchPos = point;
            }
        }
    }
    
    return true;
}


void Setting::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
    if (selectedBtn != -1)
    {
        CCPoint point = pTouch->getLocation();
        CCSize size = selectedSprite->getContentSize();
    
        int x = (int)selectedPos.x + ((int)point.x-(int)selectedTouchPos.x);
        if (x < (int)standardBtnPos.x) x = (int)standardBtnPos.x;
        if (x > (int)standardBtnPos.x+(int)size.width) x = (int)standardBtnPos.x+(int)size.width;
        //CCLog("x : %d", x);
    
        selectedSprite->setPosition(ccp((int)x, (int)selectedPos.y));
    }
}

void Setting::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    isTouched = false;
    
    if (selectedBtn != -1)
    {
        char name[20];
        sprintf(name, "setting_option_%d", selectedBtn);
        
        CCPoint pos = selectedSprite->getPosition();
        CCSize size = selectedSprite->getContentSize();
        
        if ((int)pos.x <= (int)standardBtnPos.x + (int)size.width/2)
        {
            // On에 가까우면 on위치로 정확히 옮긴다.
            selectedSprite->setPosition(standardBtnPos);
            
            // 효과음 버튼은 on->on 상황일 때 한번만 소리내자.
            bool playSound = !CCUserDefault::sharedUserDefault()->getBoolForKey(name);
            
            CCUserDefault::sharedUserDefault()->setBoolForKey(name, true);
            if (selectedBtn == 0) {
                sound->SetEffectVolume();
                if (playSound) sound->playClick();
            }
            else if (selectedBtn == 1)
                sound->PlayBackgroundSound();
        }
        else
        {
            // Off에 가까우면 off위치로 정확히 옮긴다.
            selectedSprite->setPosition(ccp((int)standardBtnPos.x+(int)size.width,
                                            (int)standardBtnPos.y));
            
            CCUserDefault::sharedUserDefault()->setBoolForKey(name, false);
            if (selectedBtn == 0)
                sound->SetEffectVolume();
            else if (selectedBtn == 1)
                sound->StopBackgroundSound();
        }
    }
    
    selectedBtn = -1;
}


void Setting::EndScene()
{
    sound->playClick();
    CCString* param = CCString::create("0");
    CCNotificationCenter::sharedNotificationCenter()->postNotification("Ranking", param);
    
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    
    this->removeFromParentAndCleanup(true);
}

void Setting::EndSceneCallback()
{
}

