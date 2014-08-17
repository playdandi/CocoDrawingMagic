#include "Coupon.h"


CCScene* Coupon::scene()
{
    CCScene* pScene = CCScene::create();
    Coupon* pLayer = Coupon::create();
    pScene->addChild(pLayer);
	return pScene;
}

void Coupon::onEnter()
{
    //CCLog("Coupon :: onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority(), true);
    CCLayer::onEnter();
    
    // 전체화면 액션
    CCActionInterval* action = CCSequence::create( CCSpawn::create(CCMoveTo::create(0.3f, ccp(0, 0)), CCScaleTo::create(0.3f, 1.0f), NULL), CCCallFunc::create(this, callfunc_selector(Coupon::SceneCallback)), NULL );
    tLayer->runAction(CCEaseExponentialOut::create(action));
}
void Coupon::SceneCallback()
{
    isTouched = false;
}
void Coupon::onExit()
{
    //CCLog("Coupon :: onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
}

void Coupon::keyBackClicked()
{
    if (isKeybackTouched || isTouched)
        return;
    isKeybackTouched = true;
    
    sound->playClick();
    EndScene();
}


bool Coupon::init()
{
	if (!CCLayer::init())
		return false;
    
    isTouched = true;
    
    // make depth tree
    Depth::AddCurDepth("Coupon", this);
    
    this->setTouchEnabled(true);
    this->setKeypadEnabled(true);
    this->setTouchPriority(Depth::GetCurPriority());
    //CCLog("Coupon : touch prio = %d", Depth::GetCurPriority());
    
    // notification observer
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(Coupon::Notification), Depth::GetCurName(), NULL);
    
    // notification
    CCString* param = CCString::create("1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetParentName(), param);
    
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    tLayer = CCLayer::create();
    tLayer->setAnchorPoint(ccp(0, 0));
    tLayer->setPosition(ccp(winSize.width/2, winSize.height/2));
    this->addChild(tLayer, 1);
    tLayer->setScale(0);
    
    ofs = 300;
    
    InitSprites();
    
    label_pos = 0;
    
    return true;
}

void Coupon::Notification(CCObject* obj)
{
    CCString* param = (CCString*)obj;
    
    if (param->intValue() <= 0)
    {
        // 터치 활성
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority()+1, true);
        this->setTouchPriority(Depth::GetCurPriority());
        isTouched = false;
        isKeybackTouched = false;
        //CCLog("Coupon : 터치 활성 (Priority = %d)", this->getTouchPriority());
    }
    else if (param->intValue() == 1)
    {
        // 터치 비활성
        //CCLog("Coupon : 터치 비활성");
        isTouched = true;
        isKeybackTouched = true;
        CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
    }
    else if (param->intValue() == 2)
    {
        EndScene();
    }
    else if (param->intValue() == 10)
    {
        // 터치 풀기 (백그라운드에서 돌아올 때)
        isTouched = false;
        isKeybackTouched = false;
    }
}


void Coupon::InitSprites()
{
    pBlack = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, winSize.width, winSize.height));
    pBlack->setPosition(ccp(0, 0));
    pBlack->setAnchorPoint(ccp(0, 0));
    pBlack->setColor(ccc3(0, 0, 0));
    pBlack->setOpacity(150);
    this->addChild(pBlack, 0);
    
    spriteClass = new SpriteClass();
    
    // pop-up 배경
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png", ccp(0, 0), ccp(49, 640+ofs), CCSize(982, 623), "", "Layer", tLayer, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png", ccp(0, 0), ccp(76, 678+ofs), CCSize(929, 562), "", "Layer", tLayer, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_popup_rightup.png", ccp(0, 0), ccp(809, 1039+ofs), CCSize(0, 0), "", "Layer", tLayer, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_x_brown.png", ccp(0, 0), ccp(900, 1132+ofs), CCSize(0, 0), "", "Layer", tLayer, 1) );
    
    std::string text = "알파벳/숫자로 이루어진\n16자리 번호를 입력해 주세요.";
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabelArea(text, fontList[0], 40, ccp(0.5, 0.5), ccp(winSize.width/2, 1000+ofs), ccc3(78,47,8), CCSize(829-20, 280), kCCTextAlignmentCenter, kCCVerticalTextAlignmentCenter, "", "Layer", tLayer, 5) );
    
    
    // title
    std::string title = "쿠폰등록";
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_degree_desc.png", ccp(0, 1), ccp(126+5, 678+562-35+ofs), CCSize(759-126+52, 90), "", "Layer", tLayer, 1) );
    CCPoint pos = spriteClass->FindParentCenterPos("background/bg_degree_desc.png");
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(title, fontList[0], 48, ccp(0.5,0.5), ccp(pos.x+2, pos.y+3-1), ccc3(0,0,0), "background/bg_degree_desc.png", "1", NULL, 2, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(title, fontList[0], 48, ccp(0.5,0.5), ccp(pos.x, pos.y+3), ccc3(242,242,242), "background/bg_degree_desc.png", "1", NULL, 2, 1) );
    
    
    // 닫기 버튼
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_system.png", ccp(0, 0), ccp(126, 711+ofs), CCSize(0, 0), "", "Layer", tLayer, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_cancel.png", ccp(0.5, 0), ccp(spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->getContentSize().width/2, 30), CCSize(0, 0), "button/btn_system.png", "0", NULL, 5, 1) );
    
    // 확인 버튼
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_red_mini.png", ccp(0, 0), ccp(717+5, 711+ofs), CCSize(0, 0), "", "Layer", tLayer, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_confirm_mini.png",ccp(0.5, 0), ccp(spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->getContentSize().width/2, 24), CCSize(0, 0), "button/btn_red_mini.png", "0", NULL, 5, 1) );
    ((CCSprite*)spriteClass->FindSpriteByName("button/btn_red_mini.png"))->setColor(ccc3(170,170,170));
    ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_confirm_mini.png"))->setColor(ccc3(170,170,170));
    
    //spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_degree_desc.png2", ccp(0.5, 0), ccp(winSize.width/2, 880), CCSize(750, 76), "", "Layer", tLayer, 2) );
    
    // 쿠폰 text 레이블
    m_pCouponField = CCTextFieldTTF::textFieldWithPlaceHolder("", CCSize(700, 66), kCCTextAlignmentCenter, fontList[0].c_str(), 56);
    m_pCouponField->setColor(ccc3(0,0,0));
    m_pCouponField->setPosition(ccp(winSize.width/2, 880+5+ofs));
    m_pCouponField->setAnchorPoint(ccp(0.5, 0));
    m_pCouponField->setOpacity(0);
    m_pCouponField->setDelegate(this);
    this->addChild(m_pCouponField, 3);
    
    int h = 860;
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_degree_desc.png2", ccp(0.5, 0), ccp(200-3, h+ofs), CCSize(200, 76), "", "Layer", tLayer, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_degree_desc.png3", ccp(0.5, 0), ccp(430-3, h+ofs), CCSize(200, 76), "", "Layer", tLayer, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_degree_desc.png4", ccp(0.5, 0), ccp(660-3, h+ofs), CCSize(200, 76), "", "Layer", tLayer, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_degree_desc.png5", ccp(0.5, 0), ccp(890-3, h+ofs), CCSize(200, 76), "", "Layer", tLayer, 2) );
    
    // 실제 레이블
    int off = 65;
    int x = 200-3-off;
    int d = 0;
    for (int i = 0 ; i < 16 ; i++)
    {
        if (i == 4) x = 430-3-off;
        else if (i == 8) x = 660-3-off;
        else if (i == 12) x = 890-3-off;
        if (i % 4 == 0) d = 0;
        
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("*", fontList[0], 56, ccp(0.5, 0.5), ccp(x+43*d, h+76/2+ofs), ccc3(0,0,0), "", "Layer", tLayer, 5, 0, 255, i) );
        
        d++;
    }
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
}

void Coupon::keyboardDidShow(CCIMEKeyboardNotificationInfo& info)
{
    //CCLog("show");
}
void Coupon::keyboardDidHide(CCIMEKeyboardNotificationInfo& info)
{
    //CCLog("hide");
}
bool Coupon::onTextFieldAttachWithIME(CCTextFieldTTF * pSender)
{
    //tLayer->setPosition(ccp(winSize.width/2, winSize.height/2 + 300));
    //CCPoint p = tLayer->getPosition();
    //CCLog("attach : %d %d", (int)p.x, (int)p.y);
    //tLayer->setPosition(ccp(p.x, p.y+300));
    return false;
}
bool Coupon::onTextFieldDetachWithIME(CCTextFieldTTF * pSender)
{
    //CCPoint p = tLayer->getPosition();
    //CCLog("detach : %d %d", (int)p.x, (int)p.y);
    //tLayer->setPosition(ccp(p.x, p.y-300));
    return false;
}
bool Coupon::onTextFieldInsertText(CCTextFieldTTF * pSender, const char * text, int nLen)
{
    if ('\n' == *text)
        return false;
    if (label_pos >= COUPON_LENGTH)
        return true;
    if (!(*text >= 'A' && *text <= 'Z') && !(*text >= 'a' && *text <= 'z') && !(*text >= '0' && *text <= '9'))
        return true;
    
    int sss;
    if (*text >= 'a' && *text <= 'z')
        sss = *text - 32;
    else
        sss = *text;

    char name[3];
    sprintf(name, "%c", sss);
    ////CCLog("pos = %d", label_pos);
    
    ((CCLabelTTF*)spriteClass->FindLabelByTag(label_pos))->setString(name);
    label_pos++;
    
    // '확인 버튼 (비)활성화
    if (label_pos == COUPON_LENGTH)
    {
        ((CCSprite*)spriteClass->FindSpriteByName("button/btn_red_mini.png"))->setColor(ccc3(255,255,255));
        ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_confirm_mini.png"))->setColor(ccc3(255,255,255));
    }
    else
    {
        ((CCSprite*)spriteClass->FindSpriteByName("button/btn_red_mini.png"))->setColor(ccc3(170,170,170));
        ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_confirm_mini.png"))->setColor(ccc3(170,170,170));
    }
    
    return false;
}
bool Coupon::onTextFieldDeleteBackward(CCTextFieldTTF * pSender, const char * delText, int nLen)
{
    if (label_pos <= 0)
        return false;

    label_pos--;
    ((CCLabelTTF*)spriteClass->FindLabelByTag(label_pos))->setString("*");
    
    // '확인 버튼 (비)활성화
    if (label_pos == COUPON_LENGTH)
    {
        ((CCSprite*)spriteClass->FindSpriteByName("button/btn_red_mini.png"))->setColor(ccc3(255,255,255));
        ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_confirm_mini.png"))->setColor(ccc3(255,255,255));
    }
    else
    {
        ((CCSprite*)spriteClass->FindSpriteByName("button/btn_red_mini.png"))->setColor(ccc3(170,170,170));
        ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_confirm_mini.png"))->setColor(ccc3(170,170,170));
    }

    return false;
}
bool Coupon::onDraw(CCTextFieldTTF* pSender)
{
    return false;
}

bool Coupon::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    if (isTouched)
        return false;
    isTouched = true;
    
    CCPoint point = pTouch->getLocation();
    
    int x = point.x;
    int y = point.y;
    if (x >= 98 && x <= 983 && y >= 880+ofs && y <= 880+76+ofs)
    {
        m_pCouponField->attachWithIME();
        //tLayer->runAction(CCMoveBy::create(0.5f, ccp(0, 300)));
        return true;
    }
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        if (spriteClass->spriteObj[i]->name == "button/btn_x_brown.png" ||
            spriteClass->spriteObj[i]->name == "button/btn_system.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                EndScene();
                return true;
            }
        }
        else if (spriteClass->spriteObj[i]->name.substr(0, 24) == "button/btn_red_mini.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                // 알파벳 소문자->대문자 변경
                std::string string = m_pCouponField->getString();
                for (int i = 0 ; i < string.size() ; i++)
                {
                    if (string[i] >= 'a' && string[i] <= 'z')
                        string[i] -= 32;
                }
                
                if ((int)string.size() == COUPON_LENGTH)
                {
                    // Loading 화면으로 MESSAGE request 넘기기
                    Common::ShowNextScene(this, "Coupon", "Loading", false, LOADING_MESSAGE);
                    
                    // 네트워크로 메시지들을 받아온다.
                    char temp[50];
                    std::string params = "";
                    sprintf(temp, "kakao_id=%s&", myInfo->GetKakaoId().c_str());
                    params += temp;
                    sprintf(temp, "coupon_string=%s", string.c_str());
                    params += temp;
                    
                    Network::HttpPost(params, URL_COUPON, this, httpresponse_selector(Coupon::onHttpRequestCompleted));
                }
                return true;
            }
        }
    }
    
    return true;
}


void Coupon::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
}

void Coupon::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    isTouched = false;
}

void Coupon::EndScene()
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
    
    // remove all CCNodes
    spriteClass->RemoveAllObjects();
    delete spriteClass;
    pBlack->removeFromParentAndCleanup(true);
    m_pCouponField->detachWithIME();
    m_pCouponField->removeFromParentAndCleanup(true);
    
    tLayer->removeAllChildren();
    tLayer->removeFromParentAndCleanup(true);
    
    this->removeFromParentAndCleanup(true);
}



void Coupon::onHttpRequestCompleted(CCNode *sender, void *data)
{
    // Loading 창 끄기
    ((Loading*)Depth::GetCurPointer())->EndScene();
    
    CCHttpResponse* res = (CCHttpResponse*) data;
    
    xml_document xmlDoc;
    Network::GetXMLFromResponseData(res, xmlDoc);
    
    XmlParseCouponResult(&xmlDoc);
}

void Coupon::XmlParseCouponResult(xml_document *xmlDoc)
{
    xml_node nodeResult = xmlDoc->child("response");
    int code = nodeResult.child("code").text().as_int();
    
    // 에러일 경우 code에 따라 적절히 팝업창 띄워줌.
    if (code != 0)
    {
        std::vector<int> nullData;
        if (code <= MAX_COMMON_ERROR_CODE)
            Network::ShowCommonError(code);
        else if (code == 10) // 존재하지 않는 쿠폰
            Common::ShowPopup(this, "Coupon", "NoImage", false, COUPON_NOT_EXIST, BTN_1, nullData);
        else if (code == 11) // 유효기간 지난 쿠폰
            Common::ShowPopup(this, "Coupon", "NoImage", false, COUPON_EXPIRED, BTN_1, nullData);
        else if (code == 12) // 이미 사용한 쿠폰
            Common::ShowPopup(this, "Coupon", "NoImage", false, COUPON_ALREADY_USED, BTN_1, nullData);
        else if (code == 13) // 같은 유형의 쿠폰
            Common::ShowPopup(this, "Coupon", "NoImage", false, COUPON_SAME_TYPE, BTN_1, nullData);
        else
            Common::ShowPopup(this, "Coupon", "NoImage", false, NETWORK_FAIL, BTN_1, nullData);
    }
    
    else if (code == 0)
    {
        // 쿠폰의 타입과 보상 값
        // type { 1 : 별사탕, 2 : 토파즈 , 3 : 포션 }
        int type = nodeResult.child("coupon").attribute("type").as_int();
        int value = nodeResult.child("coupon").attribute("value").as_int();
        
        // 토파즈, 별사탕을 갱신.
        int topaz = nodeResult.child("money").attribute("topaz").as_int();
        int starcandy = nodeResult.child("money").attribute("star-candy").as_int();
        myInfo->SetMoney(topaz, starcandy);
        
        // 포션 갱신.
        int potion = nodeResult.child("potion").attribute("potion-count").as_int();
        int remainTime = nodeResult.child("potion").attribute("remain-time").as_int();
        myInfo->SetPotion(potion, remainTime);
        
        // 성공 팝업창 띄우기
        std::vector<int> data;
        data.push_back(type);
        data.push_back(value);
        Common::ShowPopup(this, "Coupon", "NoImage", false, COUPON_OK, BTN_1, data);
        
        // 화면 갱신 (Ranking)
        CCString* param = CCString::create("2");
        CCNotificationCenter::sharedNotificationCenter()->postNotification("Ranking", param);
    }
}
