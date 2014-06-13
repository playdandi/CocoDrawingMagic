#include "BuyTopaz.h"
#include "pugixml/pugixml.hpp"

using namespace pugi;


CCScene* BuyTopaz::scene(int parent)
{
    CCScene* pScene = CCScene::create();
    BuyTopaz* pLayer = BuyTopaz::create();
    
    pScene->addChild(pLayer);
    
	return pScene;
}

void BuyTopaz::onEnter()
{
    CCLog("BuyTopaz : onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority(), true);
    CCLayer::onEnter();
}
void BuyTopaz::onExit()
{
    CCLog("BuyTopaz : onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
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
    
    // make depth tree
    Depth::AddCurDepth("BuyTopaz", this);
    
    this->setTouchEnabled(true);
    this->setKeypadEnabled(true);
    this->setTouchPriority(Depth::GetCurPriority());
    CCLog("BuyTopaz : touch prio = %d", this->getTouchPriority());
    
    // notification observer
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(BuyTopaz::Notification), Depth::GetCurName(), NULL);
    
    // notification post
    CCString* param = CCString::create("1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetParentName(), param);

    
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    InitSprites();
    MakeScroll();
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
    
    isTouched = false;
    
    httpStatus = 0;
    
    return true;
}

void BuyTopaz::Notification(CCObject* obj)
{
    CCString* param = (CCString*)obj;
    
    if (param->intValue() == 0)
    {
        // 터치 활성
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority()+1, true);
        this->setTouchPriority(Depth::GetCurPriority());
        isTouched = false;
        CCLog("BuyTopaz : 터치 활성 (Priority = %d)", this->getTouchPriority());
    }
    else if (param->intValue() == 1)
    {
        // 터치 비활성
        CCLog("BuyTopaz : 터치 비활성");
        CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
    }
    else if (param->intValue() == 10)
    {
        // 터치 풀기 (백그라운드에서 돌아올 때)
        isTouched = false;
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
                //char productId[15];
                //sprintf(productId, "topaz%d", priceTopaz[number]->GetCount());
                //char topazId[10];
                //sprintf(topazId, "%d", priceTopaz[number]->GetId());
                char num[10];
                sprintf(num, "%d", number);
                
                std::string postData = "kakao_id=";
                char temp[10];
                sprintf(temp, "%d", myInfo->GetKakaoId());
                postData += temp;
                CCLog("%s", postData.c_str());
                
                httpStatus = 0;
                
                CCHttpRequest* req = new CCHttpRequest();
                req->setUrl("http://14.63.225.203/cogma/game/get_payload_google.php");
                req->setRequestData(postData.c_str(), postData.size());
                req->setRequestType(CCHttpRequest::kHttpPost);
                verifyStatusScene = this;
                req->setResponseCallback(this, httpresponse_selector(BuyTopaz::onHttpRequestCompleted));
                //req->setTag(productId);
                req->setTag(num);
                CCHttpClient::getInstance()->send(req);
                req->release();
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
    CCLog("httpStatus = %d", httpStatus);
    
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

    if (httpStatus == 0)
        XmlParseDeveloperPayload(dumpData, buffer->size(), atoi(res->getHttpRequest()->getTag()));
    else if (httpStatus == 1)
        XmlParseVerifyPurchaseResult(dumpData, buffer->size());
}

void BuyTopaz::XmlParseDeveloperPayload(char* data, int size, int priceTopazIdx)
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
        #if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
        char productId[15];
        sprintf(productId, "topaz%d", priceTopaz[priceTopazIdx]->GetCount());
        int topazId = priceTopaz[priceTopazIdx]->GetId();
        
        const char* payload = nodeResult.child("payload").attribute("string").as_string();
        
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                     "com/playDANDi/CocoMagic/CocoMagic",
                                     "StartIAB",
                                     "(IIILjava/lang/String;Ljava/lang/String;)V"))
        {  // 파라미터 (int, String, String), 리턴타입은 Void
            // 함수 호출할 때 Object값을 리턴하는 함수로 받아야함!!!!
            t.env->CallStaticVoidMethod(t.classID, t.methodID, 1, myInfo->GetKakaoId(), topazId, t.env->NewStringUTF(productId), t.env->NewStringUTF(payload));
            // Release
            t.env->DeleteLocalRef(t.classID);
        }
        #endif
    }
    else
    {
        CCLog("failed code = %d", code);
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
    //itemContainer->removeAllChildren();
    pBlack->removeFromParentAndCleanup(true);
    
    this->removeFromParentAndCleanup(true);
}



void BuyTopaz::verifyPayloadAndProvideItem(const char* data, const char* signature, int topazCount)
{
    CCLog("data = %s", data);
    CCLog("sign = %s", signature);
    
    int topazId;
    for (int i = 0 ; i < priceTopaz.size() ; i++)
    {
        if (priceTopaz[i]->GetCount() == topazCount)
            topazId = priceTopaz[i]->GetId();
    }
    
    CCLog("topaz_id = %d", topazId);

    /*
    const char* data2 = "{\"orderId\":\"12999763169054705758.136161536899983\",\"packageName\":\"com.playDANDi.CocoMagic\",\"productId\":\"test\",\"purchaseTime\":1402512283566,\"purchaseState\":0,\"developerPayload\":\"developerpayload\",\"purchaseToken\":\"lbfdjpphihmngleiknmbecni.AO-J1OzmKhYV99B9Lg1zXbMdKALR2XLd_VISF9UFdkYp7PantN39u-BzT4DvT31pgmZwCGk-Ct1btWiUyY6Zj1kuy1_RskOseVPPPIAiMkU3EmbUDPIOZ24\"}";
    const char* sign2 = "QPlZ/2aHyWhnOqewE3GDx85yhMLL0Cqd0kMWWQWQMW9EVNB6DbE1FzbzSRZw7ruWmCaqQwkHuNjRbw0VXyf7jIVoKtvSu5y5rWXTLtNrNLb8ylnGReNQZro2HIIykEyNE5Gnn3bBlmX8qkgh0wvYUU24bgXvmv4ZyTnufSJlo4l-XfeQEtpgquMZjljtxbANPHhuwHxb7W-du3ji9p5YATfPEHgadotNW/cGNvD49s06k0bQ8zcJ0dcSTbCa3K70Z5XYXqtRFrVqfGacAXmV4XiaMQyHfI/t-dlt38nbfE6DXg77b4wS3jXteMRR57kWLoJ/j4sKe5/kE6zbnsFtXw==";

http://14.63.225.203/cogma/game/purchase_topaz_google.php?kakao_id=1001&topaz_id=120&purchase_data={"orderId":"12999763169054705758.136161536899983","packageName":"com.playDANDi.CocoMagic","productId":"test","purchaseTime":1402512283566,"purchaseState":0,"developerPayload":"developerpayload","purchaseToken":"lbfdjpphihmngleiknmbecni.AO-J1OzmKhYV99B9Lg1zXbMdKALR2XLd_VISF9UFdkYp7PantN39u-BzT4DvT31pgmZwCGk-Ct1btWiUyY6Zj1kuy1_RskOseVPPPIAiMkU3EmbUDPIOZ24"}&signature=QPlZ/2aHyWhnOqewE3GDx85yhMLL0Cqd0kMWWQWQMW9EVNB6DbE1FzbzSRZw7ruWmCaqQwkHuNjRbw0VXyf7jIVoKtvSu5y5rWXTLtNrNLb8ylnGReNQZro2HIIykEyNE5Gnn3bBlmX8qkgh0wvYUU24bgXvmv4ZyTnufSJlo4l-XfeQEtpgquMZjljtxbANPHhuwHxb7W-du3ji9p5YATfPEHgadotNW/cGNvD49s06k0bQ8zcJ0dcSTbCa3K70Z5XYXqtRFrVqfGacAXmV4XiaMQyHfI/t-dlt38nbfE6DXg77b4wS3jXteMRR57kWLoJ/j4sKe5/kE6zbnsFtXw==

  */
    char temp[1024];
    std::string postData = "";
    sprintf(temp, "kakao_id=%d&", myInfo->GetKakaoId());
    postData += temp;
    sprintf(temp, "topaz_id=%d&", topazId);
    postData += temp;
    sprintf(temp, "purchase_data=%s&", data);
    //sprintf(temp, "purchase_data=%s&", data2);
    postData += temp;
    sprintf(temp, "signature=%s", signature);
    //sprintf(temp, "signature=%s", sign2);
    postData += temp;
    CCLog("%s", postData.c_str());
    
    httpStatus = 1;
    
    CCHttpRequest* req = new CCHttpRequest();
    req->setUrl("http://14.63.225.203/cogma/game/purchase_topaz_google.php");
    req->setRequestData(postData.c_str(), postData.size());
    req->setRequestType(CCHttpRequest::kHttpPost);
    req->setResponseCallback(haha, httpresponse_selector(BuyTopaz::onHttpRequestCompleted));
    CCHttpClient::getInstance()->send(req);
    req->release();
}

void BuyTopaz::XmlParseVerifyPurchaseResult(char* data, int size)
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
        CCLog("토파즈 구매 성공!");
        /*
         // 토파즈, 별사탕을 갱신한다.
         int topaz = nodeResult.child("money").attribute("topaz").as_int();
         int starcandy = nodeResult.child("money").attribute("star-candy").as_int();
         myInfo->SetMoney(topaz, starcandy);
         
         // 부모 scene에 갱신
         CCString* param = CCString::create("2");
         CCNotificationCenter::sharedNotificationCenter()->postNotification("Ranking", param);
         CCNotificationCenter::sharedNotificationCenter()->postNotification("GameReady", param);
         CCNotificationCenter::sharedNotificationCenter()->postNotification("CocoRoom", param);
         
         // 성공한 팝업창으로 넘어간다.
         ReplaceScene("NoImage", BUY_TOPAZ_OK, BTN_1);
         */
    }
    else
    {
        CCLog("failed code = %d", code);
        if (code == 10) CCLog("서버인증실패");
        else if (code == 11) CCLog("payload 다름");
        else if (code == 12) CCLog("이미 지급한 토파즈");
        else if (code == 13) CCLog("토파즈 id 이상함");
        
        //ReplaceScene("NoImage", NETWORK_FAIL, BTN_1);
    }
    
    
     #if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
     JniMethodInfo t;
     if (JniHelper::getStaticMethodInfo(t,
                                        "com/playDANDi/CocoMagic/InAppBilling",
                                        "Consume",
                                        "()V"))
     {
     CCLog("hihihi");
     t.env->CallStaticVoidMethod(t.classID, t.methodID);
     // Release
     t.env->DeleteLocalRef(t.classID);
     }
     #endif
    
}


