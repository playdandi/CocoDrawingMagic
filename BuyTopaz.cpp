#include "BuyTopaz.h"

CCScene* BuyTopaz::scene(int parent)
{
    CCScene* pScene = CCScene::create();
    BuyTopaz* pLayer = BuyTopaz::create();
    
    pScene->addChild(pLayer);
    
	return pScene;
}

void BuyTopaz::onEnter()
{
    //CCLog("BuyTopaz : onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority(), true);
    CCLayer::onEnter();
    
    // 전체화면 액션
    CCActionInterval* action = CCSequence::create( CCSpawn::create(CCMoveTo::create(0.2f, ccp(0, 0)), CCScaleTo::create(0.2f, 1.0f), NULL), CCCallFunc::create(this, callfunc_selector(BuyTopaz::SceneCallback)), NULL );
    tLayer->runAction(CCEaseExponentialOut::create(action));
}
void BuyTopaz::SceneCallback()
{
    isTouched = false;
}
void BuyTopaz::onExit()
{
    //CCLog("BuyTopaz : onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
}

void BuyTopaz::keyBackClicked()
{
    if (isKeybackTouched || isTouched)
        return;
    isKeybackTouched = true;

    EndScene();
}


bool BuyTopaz::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
    isTouched = true;
    
    // make depth tree
    Depth::AddCurDepth("BuyTopaz", this);
    
    this->setTouchEnabled(true);
    this->setKeypadEnabled(true);
    this->setTouchPriority(Depth::GetCurPriority());
    //CCLog("BuyTopaz : touch prio = %d", this->getTouchPriority());
    
    // notification observer
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(BuyTopaz::Notification), Depth::GetCurName(), NULL);
    
    // notification post
    CCString* param = CCString::create("1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetParentName(), param);

    
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    tLayer = CCLayer::create();
    tLayer->setAnchorPoint(ccp(0, 0));
    tLayer->setPosition(ccp(winSize.width/2, 0));
    tLayer->setScale(0);
    this->addChild(tLayer, 1);
    
    InitSprites();
    MakeScroll();
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
    
    httpStatus = 0;
    
    isTryingPurchase = false;
    
    return true;
}

void BuyTopaz::Notification(CCObject* obj)
{
    CCString* param = (CCString*)obj;
    
    if (param->intValue() == 0 || param->intValue() == -1)
    {
        // 터치 활성
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority()+1, true);
        this->setTouchPriority(Depth::GetCurPriority());
        isTouched = false;
        isKeybackTouched = false;
        //CCLog("BuyTopaz : 터치 활성 (Priority = %d)", this->getTouchPriority());
    }
    else if (param->intValue() == 1)
    {
        // 터치 비활성
        //CCLog("BuyTopaz : 터치 비활성");
        isTouched = true;
        isKeybackTouched = true;
        CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
    }
    else if (param->intValue() == 10)
    {
        // 터치 풀기 (백그라운드에서 돌아올 때)
        isTouched = false;
        isKeybackTouched = false;
        //CCLog("BuyTopaz : noti 10 ( tryingPurchase = %d )", isTryingPurchase);
        
        if (isTryingPurchase)
        {
            // 안드로이드 결제 과정 거친 후 돌아온 경우! (여기 왔다는 것 = 결제에 승인했으나 그 후 에러가 났음을 의미)
            std::vector<int> nullData;
            Common::ShowPopup(Depth::GetCurPointer(), Depth::GetCurName(), "NoImage", false, ERROR_IN_APP_BILLING, BTN_1, nullData);
        }
        isTryingPurchase = false;
    }
    else // "{friendkakaoid}/{priceTopazIdx}" 형태로 왔을 경우
    {
        std::string p = param->getCString();
        std::string friendKakaoId = p.substr(0, p.find("/"));
        int priceTopazIdx = atoi(p.substr(p.find("/")+1).c_str());
        
        //CCLog("%s %d", friendKakaoId.c_str(), priceTopazIdx);
        
        #if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
        char num[10];
        sprintf(num, "%d", priceTopazIdx);
        
        httpStatus = 1;
        verifyStatusScene = this;
        
        std::string param = "";
        char temp[100];
        sprintf(temp, "kakao_id=%s&", myInfo->GetKakaoId().c_str());
        param += temp;
        sprintf(temp, "friend_kakao_id=%s", friendKakaoId.c_str());
        param += temp;
        
        Network::HttpPost(param, URL_GOOGLE_PAYLOAD, this, httpresponse_selector(BuyTopaz::onHttpRequestCompleted), num, friendKakaoId);
        #endif
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
                    ccp(0, 0), ccp(14, 1586), CCSize(0, 0), "", "Layer", tLayer, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_x_yellow.png",
                    ccp(0, 0), ccp(875, 1391+243), CCSize(0, 0), "", "Layer", tLayer, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_title_purchase_topaz.png",
                    ccp(0, 0), ccp(269, 1389+243), CCSize(0, 0), "", "Layer", tLayer, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png",
                    ccp(0, 0), ccp(49, 458-45), CCSize(982, 954+243+45), "", "Layer", tLayer, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png",
                    ccp(0, 0), ccp(75, 492-45), CCSize(929, 904+243+45), "", "Layer", tLayer, 1) );
    
    // button
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_green.png",
                    ccp(0, 0), ccp(319, 191), CCSize(0, 0), "", "Layer", tLayer, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_request.png",
                    ccp(0.5, 0), ccp(spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->
                    getContentSize().width/2, 56), CCSize(0, 0), "button/btn_green.png", "0", NULL, 1, 1) );
}

void BuyTopaz::MakeScroll()
{    
    // make scroll
    itemContainer = CCLayer::create();
    itemContainer->setPosition(ccp(77, 492));
    tLayer->addChild(itemContainer, 2);
    
    int numOfList = priceTopaz.size();
    
    char name[50], name2[50];
    for (int i = 0 ; i < numOfList ; i++)
    {
        CCLayer* itemLayer = CCLayer::create();
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
        
        
        // 'hot' icon
        if (i == 1)
        {
            sprintf(name, "icon/icon_hot.png%d", i);
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0.5, 0), ccp(pos.x+35, pos.y+100), CCSize(0, 0), "", "Layer", itemLayer, 3) );
            ((CCSprite*)spriteClass->FindSpriteByName(name))->setRotation(-20);
            
            CCActionInterval* action = CCSequence::create( CCMoveBy::create(1.0f, ccp(0, -10)), CCMoveBy::create(1.0f, ccp(0, 10)), NULL );
            ((CCSprite*)spriteClass->FindSpriteByName(name))->runAction(CCRepeatForever::create(action));
        }
        
        
        // 토파즈 개수
        sprintf(name, "%d", priceTopaz[i]->GetCount());
        CCLayer* numberLayer = Common::MakeImageNumberLayer(name, 0);
        numberLayer->setPosition(ccp(214, 126));
        itemLayer->addChild(numberLayer, 3);
        
        spriteClass->layers.push_back(numberLayer);
        spriteClass->layers.push_back(itemLayer);
        
        // 보너스 숫자
        sprintf(name, "+ %d%%", priceTopaz[i]->GetBonus());
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(name, fontList[0], 36, ccp(0, 0), ccp(348, 138), ccc3(168,122,62), "", "Layer", itemLayer, 3) );

        // 가격
        sprintf(name, "background/bg_degree_desc.png%d", i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(1, name, ccp(0, 0), ccp(226, 18), CCSize(201, 77), "", "Layer", itemLayer, 3) );

        std::string cost = "";
        if (myInfo->GetDeviceType() == 1) // ANDROID
        {
            cost = "₩ " + Common::MakeComma(priceTopaz[i]->GetPrice(1));
        }
        else if (myInfo->GetDeviceType() == 2) // iPHONE
        {
            cost = "$ " + Common::MakeComma(priceTopaz[i]->GetPrice(2)/100) + "." + Common::MakeComma(priceTopaz[i]->GetPrice(2)%100);
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
    spriteClass->layers.push_back(itemContainer);
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
                int number = atoi(spriteClass->spriteObj[i]->name.substr(27).c_str());
                Common::ShowNextScene(this, "BuyTopaz", "SendTopaz", false, number);
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
                
                int number = atoi(spriteClass->spriteObj[i]->name.substr(25).c_str());
                
                #if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
                char num[10];
                sprintf(num, "%d", number);
                
                std::string param = "";
                char temp[30];
                sprintf(temp, "kakao_id=%s&", myInfo->GetKakaoId().c_str());
                param += temp;
                sprintf(temp, "friend_kakao_id=NULL");
                param += temp;
                
                httpStatus = 0;
                verifyStatusScene = this;
                
                Network::HttpPost(param, URL_GOOGLE_PAYLOAD, this, httpresponse_selector(BuyTopaz::onHttpRequestCompleted), num);
                #endif
                
                #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
                std::vector<int> data;
                data.push_back(number);
                data.push_back(priceTopaz[number]->GetPrice(myInfo->GetDeviceType()));
                Common::ShowPopup(this, "BuyTopaz", "NoImage", false, BUY_TOPAZ_TRY, BTN_2, data);
                return true;
                #endif
            }
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_green.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                Common::ShowNextScene(this, "BuyTopaz", "RequestTopaz", false);
                break;
            }
        }
    }
    
    return true;
}


void BuyTopaz::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
}

void BuyTopaz::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    isTouched = false;
}


void BuyTopaz::onHttpRequestCompleted(CCNode *sender, void *data)
{
    CCHttpResponse* res = (CCHttpResponse*) data;

    xml_document xmlDoc;
    Network::GetXMLFromResponseData(res, xmlDoc);
  
    std::string tag = res->getHttpRequest()->getTag();
    int priceTopazIdx;
    std::string friendKakaoId = "";
    
    // send_topaz의 경우 "{friendKakaoId}@{priceTopazIdx}" 형태로 tag가 넘어왔기 때문에, 적절히 처리해야 한다.
    if (httpStatus == 1)
    {
        priceTopazIdx = atoi(tag.substr(0, tag.find("@")).c_str());
        friendKakaoId = tag.substr(tag.find("@")+1);
    }
    
    else if (httpStatus == 0)
        priceTopazIdx = atoi(tag.c_str());
    
    XmlParseDeveloperPayload(&xmlDoc, atoi(res->getHttpRequest()->getTag()), friendKakaoId);
}

void BuyTopaz::XmlParseDeveloperPayload(xml_document *xmlDoc, int priceTopazIdx, std::string friendKakaoId)
{
    xml_node nodeResult = xmlDoc->child("response");
    int code = nodeResult.child("code").text().as_int();
    
    // 에러일 경우 code에 따라 적절히 팝업창 띄워줌.
    if (code != 0)
    {
        std::vector<int> nullData;
        if (code <= MAX_COMMON_ERROR_CODE)
            Network::ShowCommonError(code);
        else
            Common::ShowPopup(this, "BuyTopaz", "NoImage", false, NETWORK_FAIL, BTN_1, nullData);
    }
    
    else if (code == 0)
    {
        isTryingPurchase = true; // 안드로이드 끝나고 돌아올 때 오류 체크 위해 필요함. (팝업창 띄우는 용도)
        
        #if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
        int topazId = priceTopaz[priceTopazIdx]->GetId();
        char productId[15];
        if (httpStatus == 0)
            sprintf(productId, "topaz%d", priceTopaz[priceTopazIdx]->GetCount());
        else if (httpStatus == 1)
            sprintf(productId, "topaz%d_p", priceTopaz[priceTopazIdx]->GetCount());
        
        const char* payload = nodeResult.child("payload").attribute("string").as_string();
        
        // type : 일반구매(1) , 선물하기(2)
        int type = (httpStatus == 0) ? 1 : 2;

        //CCLog("type = %d", type);
        //CCLog("topazId = %d", topazId);
        //CCLog("productId = %s", productId);
        //CCLog("kakaoId = %s", myInfo->GetKakaoId().c_str());
        //CCLog("friendKakaoId = %s", friendKakaoId.c_str());
        
        
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                     "com/playDANDi/CocoMagic/CocoMagic",
                                     "StartIAB",
                                     "(IILjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V"))
        {  // 파라미터 (int, String, String), 리턴타입은 Void
            // 함수 호출할 때 Object값을 리턴하는 함수로 받아야함!!!!
            t.env->CallStaticVoidMethod(t.classID, t.methodID,
                                        type,
                                        topazId,
                                        t.env->NewStringUTF(myInfo->GetKakaoId().c_str()),
                                        t.env->NewStringUTF(friendKakaoId.c_str()),
                                        t.env->NewStringUTF(productId),
                                        t.env->NewStringUTF(payload),
                                        t.env->NewStringUTF(gcmKey.c_str())
                                        );
            // Release
            t.env->DeleteLocalRef(t.classID);
        }
        #endif
    }
}


void BuyTopaz::EndScene()
{
    sound->playClick();
    
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
    
    tLayer->removeAllChildren();
    tLayer->removeFromParentAndCleanup(true);
    
    this->removeFromParentAndCleanup(true);
}

void BuyTopaz::SetErrorFlag(bool flag)
{
    ////CCLog("BuyTopaz : error flag to FALSE // flag = %d", flag);
    ((BuyTopaz*)Depth::GetCurPointer())->isTryingPurchase = flag;
}


void BuyTopaz::onSendLinkMessageComplete()
{
    //CCLog("BuyTopaz :: onSendLinkMessageComplete");
}
void BuyTopaz::onSendLinkMessageErrorComplete(char const *status, char const *error)
{
    //CCMessageBox(error, "onSendLinkMessageErrorComplete");
    //CCLog("BuyTopaz :: onSendLinkMessageErrorComplete : %s, %s", status, error);
}

