#include "Setting.h"
#include "pugixml/pugixml.hpp"

using namespace pugi;

CCScene* Setting::scene()
{
    CCScene* pScene = CCScene::create();
    Setting* pLayer = Setting::create();
    pScene->addChild(pLayer);
    
	return pScene;
}

void Setting::onEnter()
{
    CCLog("Setting : onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority(), true);
    CCLayer::onEnter();
}
void Setting::onExit()
{
    CCLog("Setting : onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
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
    
    // make depth tree
    Depth::AddCurDepth("Setting");
    
    this->setTouchEnabled(true);
    this->setKeypadEnabled(true);
    this->setTouchPriority(Depth::GetCurPriority());
    CCLog("Setting : touch prio = %d", this->getTouchPriority());
    
    // notification post
    CCString* param = CCString::create("1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetParentName(), param);
    //CCNotificationCenter::sharedNotificationCenter()->postNotification("Ranking", param);
    
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    spriteClass = new SpriteClass();
    InitSprites();
    
    // 팝업창을 나갈 때 서버 저장용 예비 변수들
    kakaoMsgReserved = myInfo->GetKakaoMsg();
    pushNotiReserved = myInfo->GetPushNotification();
    potionMsgReserved = myInfo->GetPotionMsg();
    
    selectedBtn = -1;
    isTouched = false;
    
    return true;
}

void Setting::InitSprites()
{
    pBlack = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, winSize.width, winSize.height));
    pBlack->setPosition(ccp(0, 0));
    pBlack->setAnchorPoint(ccp(0, 0));
    pBlack->setColor(ccc3(0, 0, 0));
    pBlack->setOpacity(150);
    this->addChild(pBlack, 0);
    
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
      
        if (i < 2) // 클라이언트에서 들고오는 정보 (효과음, 배경음)
        {
            sprintf(key, "setting_option_%d", i);
            status = CCUserDefault::sharedUserDefault()->getBoolForKey(key);
        }
        else // 서버에서 들고오는 정보 (카카오, 푸시, 포션)
        {
            if (i == 2)      status = myInfo->GetKakaoMsg();
            else if (i == 3) status = myInfo->GetPushNotification();
            else if (i == 4) status = myInfo->GetPotionMsg();
        }
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
            {
                if (kakaoMsgReserved)
                    kakaoMsgReserved = true;
                else if (selectedBtn == 3)
                    pushNotiReserved = true;
                else if (selectedBtn == 4)
                    potionMsgReserved = true;
                
                // 카카오메시지, 푸시메시지, 포션수신 중 하나라도 바뀐 게 있다면 서버에 전송.
                if (kakaoMsgReserved != myInfo->GetKakaoMsg() || pushNotiReserved != myInfo->GetPushNotification() || potionMsgReserved != myInfo->GetPotionMsg())
                {
                    char temp[50];
                    std::string url = "http://14.63.225.203/cogma/game/setting.php?";
                    sprintf(temp, "kakao_id=%d&", myInfo->GetKakaoId());
                    url += temp;
                    sprintf(temp, "kakao_message=%d&", kakaoMsgReserved);
                    url += temp;
                    sprintf(temp, "push_notification=%d&", pushNotiReserved);
                    url += temp;
                    sprintf(temp, "potion_message=%d", potionMsgReserved);
                    url += temp;
                    CCLog("url : %s", url.c_str());
                    
                    CCHttpRequest* req = new CCHttpRequest();
                    req->setUrl(url.c_str());
                    req->setRequestType(CCHttpRequest::kHttpPost);
                    req->setResponseCallback(this, httpresponse_selector(Setting::onHttpRequestCompleted));
                    CCHttpClient::getInstance()->send(req);
                    req->release();
                }
                else
                {
                    // 바뀐 게 없으면 그냥 끈다.
                    EndScene();
                }
                break;
            }
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
                break;
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
            
            if (selectedBtn == 0)
            {
                CCUserDefault::sharedUserDefault()->setBoolForKey(name, true);
                sound->SetEffectVolume();
                if (playSound) sound->playClick();
            }
            else if (selectedBtn == 1)
            {
                CCUserDefault::sharedUserDefault()->setBoolForKey(name, true);
                sound->PlayBackgroundSound();
            }
            else if (selectedBtn == 2)
                kakaoMsgReserved = true;
            else if (selectedBtn == 3)
                pushNotiReserved = true;
            else if (selectedBtn == 4)
                potionMsgReserved = true;
        }
        else
        {
            // Off에 가까우면 off위치로 정확히 옮긴다.
            selectedSprite->setPosition(ccp((int)standardBtnPos.x+(int)size.width,
                                            (int)standardBtnPos.y));
            
            if (selectedBtn == 0)
            {
                CCUserDefault::sharedUserDefault()->setBoolForKey(name, false);
                sound->SetEffectVolume();
            }
            else if (selectedBtn == 1)
            {
                CCUserDefault::sharedUserDefault()->setBoolForKey(name, false);
                sound->StopBackgroundSound();
            }
            else if (selectedBtn == 2)
                kakaoMsgReserved = false;
            else if (selectedBtn == 3)
                pushNotiReserved = false;
            else if (selectedBtn == 4)
                potionMsgReserved = false;
        }
    }
    
    selectedBtn = -1;
}

void Setting::onHttpRequestCompleted(CCNode *sender, void *data)
{
    CCHttpResponse* res = (CCHttpResponse*) data;
    
    if (!res || !res->isSucceed())
    {
        CCLog("res failed. error buffer: %s", res->getErrorBuffer());
        return;
    }
    
    // dump data
    std::vector<char> *buffer = res->getResponseData();
    char dumpData[BUFFER_SIZE];
    for (unsigned int i = 0 ; i < buffer->size() ; i++)
        dumpData[i] = (*buffer)[i];
    dumpData[buffer->size()] = NULL;
    
    XmlParseResult(dumpData, buffer->size());
}

void Setting::XmlParseResult(char* data, int size)
{
    // xml parsing
    xml_document xmlDoc;
    xml_parse_result result = xmlDoc.load_buffer(data, size);
    
    if (!result)
    {
        CCLog("error description: %s", result.description());
        CCLog("error offset: %d", result.offset);
        return;
    }
    
    // get data
    xml_node nodeResult = xmlDoc.child("response");
    int code = nodeResult.child("code").text().as_int();
    if (code == 0)
    {
        CCLog("setting code 0 SUCCESS");
        myInfo->SetSettingVariables(kakaoMsgReserved, pushNotiReserved, potionMsgReserved);
        EndScene();
    }
    else
    {
        // failed msg
        CCLog("failed code = %d", code);
    }
}


void Setting::EndScene()
{
    sound->playClick();
    
    // release depth tree
    Depth::RemoveCurDepth();
    
    // touch 넘겨주기 (GetCurName = 위에서 remove 했기 때문에 결국 여기 입장에서는 부모다)
    CCString* param = CCString::create("0");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetCurName(), param);
    
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    
    // remove all objects
    spriteClass->RemoveAllObjects();
    delete spriteClass;
    pBlack->removeFromParentAndCleanup(true);
    
    this->removeFromParentAndCleanup(true);
}




