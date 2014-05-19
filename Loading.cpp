#include "Loading.h"
#include "pugixml/pugixml.hpp"

using namespace pugi;

Loading::~Loading(void)
{
    CCLog("Loading 소멸자");
}

CCScene* Loading::scene()
{
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
void Loading::onPause()
{
    //CCLog("Loading :: onPause");
    //CCDirector* pDirector = CCDirector::sharedDirector();
    //pDirector->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
}
void Loading::onExit()
{
    CCLog("Loading :: onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
}

bool Loading::init()
{
	if (CCLayer::init() == false)
	{
		return false;
	}
    
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
    
	return true;
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
        int missionType = gameInfo.child("mission").attribute("type").as_int();
        int missionVal = gameInfo.child("mission").attribute("value").as_int();
        int missionRefVal = gameInfo.child("mission").attribute("reference-value").as_int();
        
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
        
        // 게임 시작!
        Common::ShowNextScene(this, "Loading", "Puzzle", true);
    }
    else
    {
        if (code == 10) CCLog("Loading : 포션 부족함. 재부팅.");
        CCLog("failed code = %d", code);
    }
}

void Loading::EndScene()
{
    CCLog("Loading :: EndScene");
}
