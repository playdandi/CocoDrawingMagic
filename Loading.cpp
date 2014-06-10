#include "Loading.h"
#include "pugixml/pugixml.hpp"

using namespace pugi;

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
}
void Loading::onExit()
{
    CCLog("Loading :: onExit");
    CCLayer::onExit();
}

bool Loading::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
    m_winSize = CCDirector::sharedDirector()->getWinSize();
    
    if (status == -1)
    {
        CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("images/game3.plist");
        
        LoadingSprites();
        
        // gameStart protocol
        // http://14.63.225.203/cogma/game/game_start.php?kakao_id=1000&item_a=0&item_b=0&item_c=0&item_d=0&item_e=0
        char temp[255];
        std::string url = "http://14.63.225.203/cogma/game/game_start.php?";
        sprintf(temp, "kakao_id=%d&", myInfo->GetKakaoId());
        url += temp;
        int a = CCUserDefault::sharedUserDefault()->getIntegerForKey("item_0");
        int b = CCUserDefault::sharedUserDefault()->getIntegerForKey("item_1");
        int c = CCUserDefault::sharedUserDefault()->getIntegerForKey("item_2");
        int d = CCUserDefault::sharedUserDefault()->getIntegerForKey("item_3");
        int e = CCUserDefault::sharedUserDefault()->getIntegerForKey("item_4");
        sprintf(temp, "item_a=%d&item_b=%d&item_c=%d&item_d=%d&item_e=%d", a, b, c, d, e);
        url += temp;
        CCLog("url = %s", url.c_str());
        
        CCHttpRequest* req = new CCHttpRequest();
        req->setUrl(url.c_str());
        req->setRequestType(CCHttpRequest::kHttpPost);
        req->setResponseCallback(this, httpresponse_selector(Loading::onHttpRequestCompleted));
        CCHttpClient::getInstance()->send(req);
        req->release();
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
    pCoco = CCSprite::createWithSpriteFrameName("loading/coco_loading.png");
    pCoco->setPosition(ccp(m_winSize.width/2, m_winSize.height/2));
    pCoco->setScale(1.0f);
    this->addChild(pCoco, 5);
    
    pLoading = CCSprite::createWithSpriteFrameName("loading/loading1.png");
    pLoading->setPosition(ccp(m_winSize.width/2, m_winSize.height/2-150));
    pLoading->setTag(1);
    this->addChild(pLoading, 5);
    pLoading2 = CCSprite::createWithSpriteFrameName("loading/loading2.png");
    pLoading2->setPosition(ccp(m_winSize.width/2, m_winSize.height/2-150));
    pLoading2->setTag(2);
    pLoading2->setOpacity(0);
    this->addChild(pLoading2, 5);
    
    CCActionInterval* action = CCSequence::create(CCDelayTime::create(0.3f), CCCallFuncND::create(this, callfuncND_selector(Loading::Callback), this), NULL);
    pLoading->runAction(action);
}

void Loading::Callback(CCNode* sender, void* pointer)
{
    CCActionInterval* action = CCSequence::create(CCDelayTime::create(0.3f), CCCallFuncND::create(this, callfuncND_selector(Loading::Callback), this), NULL);
    if (sender->getTag() == 1)
    {
        pLoading->setOpacity(255);
        pLoading2->setOpacity(0);
        pLoading->runAction(action);
    }
    else
    {
        pLoading->setOpacity(0);
        pLoading2->setOpacity(255);
        pLoading2->runAction(action);
    }
}

void Loading::onHttpRequestCompleted(CCNode *sender, void *data)
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
    
    XmlParseGameStart(dumpData, (int)buffer->size());
}

void Loading::XmlParseGameStart(char* data, int size)
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
        
        // image memory 해제
        pCoco->removeFromParentAndCleanup(true);
        pLoading->removeFromParentAndCleanup(true);
        pLoading2->removeFromParentAndCleanup(true);
        
        // 게임 시작!
        Common::ShowNextScene(this, "Loading", "Puzzle", true, addedPotion);
    }
    else
    {
        if (code == 10) CCLog("Loading : 포션 부족함. 재부팅.");
        CCLog("failed code = %d", code);
    }
}

void Loading::EndScene()
{
    // release depth tree
    Depth::RemoveCurDepth();
    
    // touch 넘겨주기 (GetCurName = 위에서 remove 했기 때문에 결국 여기 입장에서는 부모다)
    CCString* param = CCString::create("-1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetCurName(), param);
    
    pLoading->removeFromParentAndCleanup(true);
    if (status == LOADING_PUZZLEEND)
    {
        pCoco->removeFromParentAndCleanup(true);
        pLoading2->removeFromParentAndCleanup(true);
    }
    
    this->removeFromParentAndCleanup(true);
}
