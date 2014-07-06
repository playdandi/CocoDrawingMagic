#include "Loading.h"

static int status;

Loading::~Loading(void)
{
    CCLog("Loading 소멸자");
}

CCScene* Loading::scene(int stat)
{
    status = stat;
    
	CCScene* pScene = CCScene::create();
	Loading* pLayer = Loading::create();
	pScene->addChild(pLayer);
    
	return pScene;
}

void Loading::onEnter()
{
    CCLog("Loading :: onEnter");
    CCLayer::onEnter();
    
    if (status == -1)
    {
        CCActionInterval* action = CCSequence::create(CCDelayTime::create(0.5f), CCCallFuncND::create(this, callfuncND_selector(Loading::AAA), this), NULL);
        pCoco->runAction(action);
    }
}

void Loading::AAA(CCNode* sender, void* p)
{
    Loading* pThis = (Loading*)p;
    
    //Common::ShowNextScene(this, "Loading", "Loading_GameStart", false);
    
    char temp[255];
    std::string param = "";
    sprintf(temp, "kakao_id=%d&", myInfo->GetKakaoId());
    param += temp;
    int a = CCUserDefault::sharedUserDefault()->getIntegerForKey("item_0");
    int b = CCUserDefault::sharedUserDefault()->getIntegerForKey("item_1");
    int c = CCUserDefault::sharedUserDefault()->getIntegerForKey("item_2");
    int d = CCUserDefault::sharedUserDefault()->getIntegerForKey("item_3");
    int e = CCUserDefault::sharedUserDefault()->getIntegerForKey("item_4");
    sprintf(temp, "item_a=%d&item_b=%d&item_c=%d&item_d=%d&item_e=%d", a, b, c, d, e);
    param += temp;
    
    Network::HttpPost(param, URL_GAMESTART, pThis, httpresponse_selector(Loading::onHttpRequestCompleted));
}

void Loading::onExit()
{
    CCLog("Loading :: onExit");
    CCLayer::onExit();
}

bool Loading::init()
{
	if (!CCLayer::init())
		return false;
    
    Depth::DumpDepth();
    
    m_winSize = CCDirector::sharedDirector()->getWinSize();
    loadingSprites = false;
    
    if (status == -1)
    {
        CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("images/game3.plist");
        LoadingSprites();
    }
    else if (status == LOADING_PUZZLEEND)
    {
        // make depth tree
        Depth::AddCurDepth("Loading", this);
        
        LoadingSprites();
    }

    else
    {
        // make depth tree
        Depth::AddCurDepth("Loading", this);
    
        // notification post
        CCString* param = CCString::create("1");
        CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetParentName(), param);

        pLoading = CCSprite::createWithSpriteFrameName("icon/icon_loading_android.png");
        pLoading->setPosition(ccp(m_winSize.width/2, m_winSize.height/2));
        this->addChild(pLoading, 5000);
        pLoading->runAction(CCRepeatForever::create(CCRotateBy::create(0.2f, 72)));
    }
    
	return true;
}

void Loading::LoadingSprites()
{
    loadingSprites = true;
    
    pCoco = CCSprite::createWithSpriteFrameName("loading/coco_loading.png");
    pCoco->setPosition(ccp(m_winSize.width/2, m_winSize.height/2));
    pCoco->setScale(1.3f);
    this->addChild(pCoco, 5);
    
    // tip
    CCPoint vo = CCDirector::sharedDirector()->getVisibleOrigin();
    pTip = CCSprite::createWithSpriteFrameName("loading/tip.png");
    pTip->setAnchorPoint(ccp(0.5, 0));
    pTip->setPosition(ccp(m_winSize.width/2, vo.y+100));
    this->addChild(pTip, 5);
    
    CCSize t = pTip->getContentSize();
    
    // tip message
    pTipMsg = CCLabelTTF::create(Common::GetTip().c_str(), fontList[0].c_str(), 32, CCSize(774, 126), kCCTextAlignmentLeft, kCCVerticalTextAlignmentCenter);
    pTipMsg->setColor(ccc3(255,255,255));
    pTipMsg->setAnchorPoint(ccp(0, 0.5));
    pTipMsg->setPosition(ccp(m_winSize.width/2-t.width/2+170, vo.y+100+t.height/2-10));
    this->addChild(pTipMsg, 6);
    
    /*
    pLoading = CCSprite::createWithSpriteFrameName("loading/loading1.png");
    pLoading->setPosition(ccp(m_winSize.width/2, m_winSize.height/2-150));
    //pLoading->setTag(1);
    this->addChild(pLoading, 5);
    pLoading2 = CCSprite::createWithSpriteFrameName("loading/loading2.png");
    pLoading2->setPosition(ccp(m_winSize.width/2, m_winSize.height/2-150));
    //pLoading2->setTag(2);
    pLoading2->setOpacity(0);
    this->addChild(pLoading2, 5);
    */
    
    //spriteStatus = 0;
    //timerStop = false;
    //this->schedule(schedule_selector(Loading::LoadingSpriteTimer), 0.1f);
    
    //CCActionInterval* action = CCSequence::create(CCDelayTime::create(0.3f), CCCallFuncND::create(this, callfuncND_selector(Loading::Callback), this), NULL);
    //pLoading->runAction(action);
    
    char name[25];
    CCArray* animFrames = CCArray::createWithCapacity(2);
    for (int j = 0 ; j < 2 ; j++)
    {
        sprintf(name, "loading/loading%d.png", j+1);
        CCSpriteFrame* frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(name);
        animFrames->addObject(frame);
    }
    
    pLoading = CCSprite::createWithSpriteFrameName("loading/loading1.png");
    pLoading->setAnchorPoint(ccp(0.5, 0.5));
    pLoading->setPosition(ccp(m_winSize.width/2, m_winSize.height/2-150-20));
    pLoading->setScale(1.3f);
    this->addChild(pLoading, 5);
    
    CCAnimation* animation = CCAnimation::createWithSpriteFrames(animFrames, 0.1f);
    CCAnimate* animate = CCAnimate::create(animation);
    pLoading->runAction(CCRepeatForever::create(animate));
    
    animFrames->removeAllObjects();
}

void Loading::LoadingSpriteTimer(float f)
{
    CCLog("hahaha = %d", spriteStatus);
    if (spriteStatus % 2 == 0)
    {
        if (timerStop)
            return;
        pLoading->setOpacity(0);
        pLoading2->setOpacity(255);
    }
    else
    {
        if (timerStop)
            return;
        pLoading->setOpacity(255);
        pLoading2->setOpacity(0);
    }
    spriteStatus++;
}

void Loading::onHttpRequestCompleted(CCNode *sender, void *data)
{
    // Loading_GameStart 창 끄기
    //((Loading_GameStart*)Depth::GetCurPointer())->EndScene();
    
    CCHttpResponse* res = (CCHttpResponse*) data;
    
    xml_document xmlDoc;
    Network::GetXMLFromResponseData(res, xmlDoc);
    
    XmlParseGameStart(&xmlDoc);
}

void Loading::XmlParseGameStart(xml_document *xmlDoc)
{
    xml_node nodeResult = xmlDoc->child("response");
    int code = nodeResult.child("code").text().as_int();
    
    // 에러일 경우 code에 따라 적절히 팝업창 띄워줌.
    if (code != 0)
    {
        std::vector<int> nullData;
        if (code <= MAX_COMMON_ERROR_CODE)
            Network::ShowCommonError(code);
        else if (code == 10) // 포션 부족함.
            Common::ShowPopup(this, "Loading", "NoImage", false, YOU_WERE_BLOCKED, BTN_1, nullData);
        else
            Common::ShowPopup(this, "Loading", "NoImage", false, NETWORK_FAIL, BTN_1, nullData);
    }

    else if (code == 0)
    {        
        xml_node gameInfo = nodeResult.child("game-info");
        
        // item 사용/미사용 결과 받아서 client에 저장해 두기
        int item;
        char name[10];
        for (int i = 0 ; i < 5; i++)
        {
            sprintf(name, "item%d", i+1);
            item = gameInfo.child("using-item").attribute(name).as_int();
            sprintf(name, "item_%d", i);
            CCUserDefault::sharedUserDefault()->setIntegerForKey(name, item);
        }
        
        // 미션
        /*
        type : 1-한개피스, 2-특정마법, 3-모든마법, 4-모든피스
        value : 피스(터트려야하는 수), 마법(시전마법수)
        reference-value : type1일때-1:물피스, 2:불피스, 3:땅피스
        (type 2일때 : 시전해야하는 스킬 ID)
         */
        missionType = gameInfo.child("mission").attribute("type").as_int();
        missionVal = gameInfo.child("mission").attribute("value").as_int();
        missionRefVal = gameInfo.child("mission").attribute("reference-value").as_int();
        
        // 사용할 active+passive 스킬 목록 (active의 경우, 슬롯 정보도 같이 갱신한다)
        int slotSize = myInfo->GetSlot().size();
        inGameSkill.clear();
        myInfo->ClearSkillSlot();
        xml_object_range<xml_named_node_iterator> its = gameInfo.child("using-skill").children("skill");
        int id, csi, usi, isActive;
        for (xml_named_node_iterator it = its.begin() ; it != its.end() ; ++it)
        {
            for (xml_attribute_iterator ait = it->attributes_begin() ; ait != it->attributes_end() ; ++ait)
            {
                std::string name = ait->name();
                if (name == "slot-id") id = ait->as_int();
                else if (name == "common-skill-id") csi = ait->as_int();
                else if (name == "user-skill-id") usi = ait->as_int();
                else if (name == "is-active") isActive = ait->as_int();
            }
            if (isActive == 1)
                myInfo->AddSkillSlot(id, csi, usi);
            inGameSkill.push_back(csi);
        }
        // 빈 슬롯 처리
        for (int i = myInfo->GetSlot().size()+1 ; i <= slotSize ; i++)
            myInfo->AddSkillSlot(i, 0, 0);
        
        // 돈 갱신
        int topaz = nodeResult.child("money").attribute("topaz").as_int();
        int starcandy = nodeResult.child("money").attribute("star-candy").as_int();
        myInfo->SetMoney(topaz, starcandy);
        
        // potion 갱신
        int potion = nodeResult.child("potion").attribute("potion-count").as_int();
        int remainTime = nodeResult.child("potion").attribute("remain-time").as_int();
        myInfo->SetPotion(potion, remainTime);
        
        // item 개수 갱신
        std::vector<int> items;
        for (int i = 0; i < 5; i++)
        {
            sprintf(name, "count-%d", i+1);
            items.push_back( nodeResult.child("item").attribute(name).as_int() );
        }
        myInfo->SetItem(items);
        items.clear();
        
        // 연습 스킬 갱신
        int practice_usi = nodeResult.child("coco").attribute("practice-user-skill-id").as_int();
        myInfo->SetPracticeSkill(0, 0);
        for (int i = 0 ; i < myInfo->GetSkillList().size() ; i++)
        {
            MySkill* ms = myInfo->GetSkillList()[i];
            if (ms->GetUserId() == practice_usi)
                myInfo->SetPracticeSkill(ms->GetCommonId(), ms->GetLevel());
        }
        
        // '끈질긴 생명력' 스킬 발동 결과
        int addedPotion = gameInfo.child("add-potion").attribute("add-number").as_int();
        // '시간을 얼리다' 발동 횟수
        int numOfFreezetime = gameInfo.child("freeze-time").attribute("add-number").as_int();
        
        //return;
        Common::ShowNextScene(this, "Loading", "Puzzle", true, addedPotion, numOfFreezetime);
    }
}

void Loading::EndScene()
{
    CCLog("%s", Depth::GetCurName());
    CCLog("%p", this);
    // release depth tree
    Depth::RemoveCurDepth();
    
    // touch 넘겨주기 (GetCurName = 위에서 remove 했기 때문에 결국 여기 입장에서는 부모다)
    CCString* param = CCString::create("-1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetCurName(), param);
    
    CCLog("Loading : EndScene");
    if (loadingSprites)
    {
        pCoco->removeFromParentAndCleanup(true);
        pTip->removeFromParentAndCleanup(true);
        pTipMsg->removeFromParentAndCleanup(true);
    }
    pLoading->removeFromParentAndCleanup(true);
    
    this->removeFromParentAndCleanup(true);
}
