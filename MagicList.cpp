#include "MagicList.h"
#include "pugixml/pugixml.hpp"

using namespace pugi;

static int from;
int sid[16] = {22, 25, 26, 28, 12, 15, 16, 18, 32, 35, 36, 38, 42, 43, 45, -1};

static int priority;

CCScene* MagicList::scene(int fromWhere, int prio)
{
    from = fromWhere;
    CCLog("MagicList : from = %d", from);
    
    priority = prio;
    
    CCScene* pScene = CCScene::create();
    MagicList* pLayer = MagicList::create();
    pScene->addChild(pLayer);
    
	return pScene;
}

void MagicList::onEnter()
{
    CCLog("MagicList :: onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, priority, true);
    CCLayer::onEnter();
}
void MagicList::onExit()
{
    CCLog("MagicList :: onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
}

void MagicList::keyBackClicked()
{
    EndScene();
}


bool MagicList::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
    this->setTouchEnabled(true);
    this->setKeypadEnabled(true);
    this->setTouchPriority(priority);
    CCLog("MagicList : touch prio = %d", this->getTouchPriority());
    
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    // notification
    CCLog("MagicList init : from = %d", from);
    CCString* param = CCString::create("1");
    if (from == 0)
        CCNotificationCenter::sharedNotificationCenter()->postNotification("GameReady", param);
    else if (from == 1)
        CCNotificationCenter::sharedNotificationCenter()->postNotification("Sketchbook", param);
    else if (from == 2)
        CCNotificationCenter::sharedNotificationCenter()->postNotification("CocoRoom", param);
    
    InitSprites();
    
    return true;
}

void MagicList::InitSprites()
{
    spriteClass = new SpriteClass();
    
    layer = CCLayer::create();
    layer->setPosition(0, winSize.height);
    this->addChild(layer, 0);
    
    // background
    int offset = 0;
    if (from == 1)
        offset = -230;

    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png",
                        ccp(0, 0), ccp(49, 688+offset), CCSize(982, 1066), "", "Layer", layer, 10) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png1",
                        ccp(0, 0), ccp(77, 952+offset), CCSize(929, 672), "", "Layer", layer, 10) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png2",
                        ccp(0, 0), ccp(77, 726+offset), CCSize(929, 200), "", "Layer", layer, 10) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_x_brown.png",
                        ccp(0, 0), ccp(900, 1638+offset), CCSize(0, 0), "", "Layer", layer, 20) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_subtitle_sketchbookindex.png",
                        ccp(0, 0), ccp(106, 1657+offset), CCSize(0, 0), "", "Layer", layer, 20) );
    
    char name[50], name2[50];
    for (int i = 0 ; i < 4; i++)
    {
        for (int j = 0 ; j < 4; j++)
        {
            sprintf(name, "background/bg_skill_yellow.png%c", (4*i+j)+'a');
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(127+j*229, 1451-i*160+offset), CCSize(0, 0), "", "Layer", layer, 20) );
            //sprintf(name2, "background/bg_skill_select.png%c", (4*i+j)+'a');
            sprintf(name2, "background/bg_skill_select.png%d", sid[i*4+j]);
            CCLog("%s", name2);
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0.5, 0.5), spriteClass->FindParentCenterPos(name), CCSize(0, 0), name, "0", NULL, 21, 1, 0) );
            
            if (i == 0) sprintf(name2, "icon/icon_skill_division_red.png%d", j);
            else if (i == 1) sprintf(name2, "icon/icon_skill_division_blue.png%d", j);
            else if (i == 2) sprintf(name2, "icon/icon_skill_division_green.png%d", j);
            else sprintf(name2, "icon/icon_skill_division_purple.png%d", j);
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0, 0), ccp(127+j*229, 1539-i*160+offset), CCSize(0, 0), "", "Layer", layer, 20) );
            
            // 스킬 문양
            //CCLog("%d", sid[i*4+j]);
            SkillInfo* si = SkillInfo::GetSkillInfo(sid[i*4+j]);
            if (si == NULL)
                continue;
            
            bool flag = false;
            for (int k = 0 ; k < myInfo->GetSkillList().size() ; k++)
            {
                if (myInfo->GetSkillList()[k]->GetCommonId() == si->GetId())
                    flag = true;
            }
            
            if (flag)
            {
                flag = false;
                for (int k = 0 ; k < myInfo->GetSlot().size() ; k++)
                {
                    if (myInfo->GetSlot()[k]->GetCommonId() == sid[i*4+j])
                        flag = true;
                }
                if (flag)
                {
                    //sprintf(name2, "background/bg_skill_select.png%c", (4*i+j)+'a');
                    sprintf(name2, "background/bg_skill_select.png%d", sid[i*4+j]);
                    ((CCSprite*)spriteClass->FindSpriteByName(name2))->setOpacity(255);
                }
                
                sprintf(name2, "skill/skill_%d.png", si->GetId());
                spriteClass->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0, 0), ccp(127+j*229+12, 1451-i*160+offset+12), CCSize(0, 0), "", "Layer", layer, 20, 0, 255, si->GetId()) );
            }
            
            /*
            sprintf(name2, "background/bg_skill_select.png%c", (4*i+j)+'a');
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, name2,
                ccp(0.5, 0.5), spriteClass->FindParentCenterPos(name), CCSize(0, 0), "", "Layer", layer, 20) );
            sprintf(name2, "background/bg_skill_yellow.png%c", (4*i+j)+'a');
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, name2,
                        ccp(0, 0), ccp(1545-i*160, 127+j*229), CCSize(0, 0), "", "Layer", layer, 20) );
             */
        }
    }
    
    // 스킬 선택/비선택할 때 설명 text
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabelArea("", fontList[0],
                36, ccp(0, 0), ccp(77+15, 726+15), ccc3(0,0,0), CCSize(929-15*2, 200-15*2),
                kCCTextAlignmentLeft, kCCVerticalTextAlignmentCenter, "", "Layer", layer, 10) );
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);

    // action
    CCActionInterval* move = CCMoveTo::create(0.5f, ccp(0, 0));
    CCActionInterval* action = CCEaseElasticOut::create((CCActionInterval*)move, 0.5f);
    layer->runAction(action);
}


bool MagicList::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    CCPoint point = pTouch->getLocation();
    //CCLog("MagicList : (%d , %d)", (int)point.x, (int)point.y);
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        if (spriteClass->spriteObj[i]->name == "button/btn_x_brown.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playBoardMove(); // 이 scene만 사운드가 다르다.
                
                // http://14.63.225.203/cogma/game/using_skill.php?kakao_id=1000&slot_id_list[0]=1&user_skill_id_list[0]=1&slot_id_list[1]=2&user_skill_id_list[1]=3
                
                char temp[255];
                std::string url = "http://14.63.225.203/cogma/game/using_skill.php?";
                sprintf(temp, "kakao_id=%d&", myInfo->GetKakaoId());
                url += temp;
                int cnt = 0;
                for (int i = 0 ; i < myInfo->GetSlot().size() ; i++)
                {
                    MySkillSlot* mss = myInfo->GetSlot()[i];
                    if (i == myInfo->GetSlot().size()-1)
                        sprintf(temp, "slot_id_list[%d]=%d&user_skill_id_list[%d]=%d", cnt, cnt+1, cnt, mss->GetUserId());
                    else
                        sprintf(temp, "slot_id_list[%d]=%d&user_skill_id_list[%d]=%d&", cnt, cnt+1, cnt, mss->GetUserId());
                    url += temp;
                    cnt++;
                }
                CCLog("url = %s", url.c_str());
                
                CCHttpRequest* req = new CCHttpRequest();
                req->setUrl(url.c_str());
                req->setRequestType(CCHttpRequest::kHttpPost);
                req->setResponseCallback(this, httpresponse_selector(MagicList::onHttpRequestCompleted));
                CCHttpClient::getInstance()->send(req);
                req->release();
            }
        }
        else if (spriteClass->spriteObj[i]->name.substr(0, 11) == "skill/skill")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                char name[40];
                int sid = spriteClass->spriteObj[i]->sprite->getTag();
                bool isInSlot = false;
                for (int i = 0 ; i < myInfo->GetSlot().size() ; i++)
                {
                    MySkillSlot* mss = myInfo->GetSlot()[i];
                    if (mss->GetCommonId() == sid) // 슬롯 안에 있으니까, 탈착!
                    {
                        isInSlot = true;
                        mss->InsertSkill(0, 0); // scid, suid => to 0
                        
                        sprintf(name, "background/bg_skill_select.png%d", sid);
                        ((CCSprite*)spriteClass->FindSpriteByName(name))->setOpacity(0);
                        
                        SendToParent();
                        sound->playClick();
                        break;
                    }
                }
                if (!isInSlot) // 슬롯에 없으니까, 장착! (하지만 꽉 찼으면 안됨)
                {
                    for (int i = 0; i < myInfo->GetSlot().size() ; i++)
                    {
                        MySkillSlot* mss = myInfo->GetSlot()[i];
                        if (mss->GetCommonId() == 0)
                        {
                            MySkill* ms = MySkill::GetObj(sid);
                            mss->InsertSkill(sid, ms->GetUserId());
                                             
                            sprintf(name, "background/bg_skill_select.png%d", sid);
                            ((CCSprite*)spriteClass->FindSpriteByName(name))->setOpacity(255);
                            
                            SendToParent();
                            sound->playClick();
                            break;
                        }
                    }
                }
                break;
            }
        }
    }
    
    return true;
}


void MagicList::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
    //CCPoint point = pTouch->getLocation();
}

void MagicList::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
}

void MagicList::SendToParent() // 슬롯 정보 갱신
{
    CCString* param = CCString::create("9");
    if (from == 0)
        CCNotificationCenter::sharedNotificationCenter()->postNotification("GameReady", param);
    else if (from == 1)
        CCNotificationCenter::sharedNotificationCenter()->postNotification("Sketchbook", param);
    else if (from == 2)
        CCNotificationCenter::sharedNotificationCenter()->postNotification("CocoRoom", param);
}

void MagicList::EndScene()
{
    CCString* param = CCString::create("0");
    if (from == 0)
        CCNotificationCenter::sharedNotificationCenter()->postNotification("GameReady", param);
    else if (from == 1)
        CCNotificationCenter::sharedNotificationCenter()->postNotification("Sketchbook", param);
    else if (from == 2)
        CCNotificationCenter::sharedNotificationCenter()->postNotification("CocoRoom", param);
    
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);

    CCFiniteTimeAction* action =
    CCSequence::create(CCMoveTo::create(0.2f, ccp(0, winSize.height)),
                       CCCallFunc::create(this, callfunc_selector(MagicList::EndSceneCallback)), NULL);
    layer->runAction(action);
}

void MagicList::EndSceneCallback()
{
    //this->removeChild(pBlack);
    //this->removeChild(pBackground);
    //pBackground->autorelease();
    //tBackground->autorelease();
    //CCTextureCache::sharedTextureCache()->removeTextureForKey("images/gameready_MagicList_detail.png");
    //CCLog("MagicList :: ELC - tBackground retain : %d", tBackground->retainCount());
    //CCLog("MagicList :: init - pBackground retain : %d", pBackground->retainCount());
    this->removeFromParentAndCleanup(true);
    //CCLog("MagicList :: ELC - tBackground retain : %d", tBackground->retainCount());
    //CCLog("MagicList :: init - pBackground retain : %d", pBackground->retainCount());
}

void MagicList::onHttpRequestCompleted(CCNode *sender, void *data)
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
    
    XmlParseSkillSlot(dumpData, (int)buffer->size());
}

void MagicList::XmlParseSkillSlot(char* data, int size)
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
        // skill-slot 갱신
        myInfo->ClearSkillSlot();
        xml_object_range<xml_named_node_iterator> its = nodeResult.child("skill-slot").children("slot");
        int id, csi, usi;
        for (xml_named_node_iterator it = its.begin() ; it != its.end() ; ++it)
        {
            for (xml_attribute_iterator ait = it->attributes_begin() ; ait != it->attributes_end() ; ++ait)
            {
                std::string name = ait->name();
                if (name == "id") id = ait->as_int();
                else if (name == "common-skill-id") csi = ait->as_int();
                else if (name == "user-skill-id") usi = ait->as_int();
            }
            myInfo->AddSkillSlot(id, csi, usi);
        }
        
        SendToParent();
        EndScene();
    }
    else
    {
        // failed msg
        CCLog("failed code = %d", code);
    }
}
