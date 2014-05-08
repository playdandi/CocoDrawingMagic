#include "Sketchbook.h"
#include "pugixml/pugixml.hpp"

using namespace pugi;

static int tabNumber;
static int from; // 0 : from Ranking , 1 : from GameReady
static int priority;

CCScene* Sketchbook::scene(int tab, int fromWhere, int prio)
{
    from = fromWhere;
    tabNumber = tab;
    priority = prio;

    CCScene* pScene = CCScene::create();
    Sketchbook* pLayer = Sketchbook::create();
    pScene->addChild(pLayer);
    
	return pScene;
}

int Sketchbook::FromWhere()
{
    return from;
}

void Sketchbook::onEnter()
{
    CCLog("Sketchbook : onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, priority, true);
    CCLayer::onEnter();
}
void Sketchbook::onExit()
{
    CCLog("Sketchbook : onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
}

void Sketchbook::registerWithTouchDispatcher(void)
{
    CCLog("Sketchbook :: registerWithTouchDispatcher");
    //CCDirector* pDirector = CCDirector::sharedDirector();
    //pDirector->getTouchDispatcher()->addTargetedDelegate(this, 0, false);
}

void Sketchbook::keyBackClicked()
{
    sound->playClick();
    EndScene();
}

int offsets = -80;

bool Sketchbook::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
    this->setTouchEnabled(true);
    this->setKeypadEnabled(true);
    this->setTouchPriority(priority);
    CCLog("스케치북 : touch prio = %d", priority);
    
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    // notification observer
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(Sketchbook::Notification), "Sketchbook", NULL);
    
    // notification post
    CCString* param = CCString::create("1");
    if (from == 0)
        CCNotificationCenter::sharedNotificationCenter()->postNotification("Ranking", param);
    else if (from == 1)
        CCNotificationCenter::sharedNotificationCenter()->postNotification("GameReady", param);
    
    // scrollview init.
    scrollViewFire = CCScrollView::create();
    scrollViewFire->retain();
    scrollViewFire->setDirection(kCCScrollViewDirectionVertical);
    scrollViewFire->setViewSize(CCSizeMake(929, 914-40+offsets));
    scrollViewFire->setAnchorPoint(ccp(0, 0));
    scrollViewFire->setPosition(ccp(77, 492+20));
    scrollViewFire->setDelegate(this);
    scrollViewFire->setTouchPriority(priority);
    this->addChild(scrollViewFire, 5);
    
    // scrollview SLOT init.
    scrollViewSlot = CCScrollView::create();
    scrollViewSlot->retain();
    scrollViewSlot->setDirection(kCCScrollViewDirectionHorizontal);
    scrollViewSlot->setViewSize(CCSizeMake(782-40, 177-20));
    scrollViewSlot->setAnchorPoint(ccp(0, 0));
    scrollViewSlot->setPosition(ccp(77+20, 228+12));
    scrollViewSlot->setDelegate(this);
    scrollViewSlot->setTouchPriority(priority);
    this->addChild(scrollViewSlot, 5);
    
    spriteClass = new SpriteClass();
    spriteClassFire = new SpriteClass();
    spriteClassWater = new SpriteClass();
    spriteClassLand = new SpriteClass();
    spriteClassMaster = new SpriteClass();
    spriteClassSlot = new SpriteClass();
    
    InitSprites();
    MakeScrollSlot();
    
    curState = -1;
    MakeScroll(tabNumber);
    
    isTouched = false;
    isScrolling = false;
    isScrollViewTouched = false;
    
    return true;
}

void Sketchbook::Notification(CCObject* obj)
{
    CCString* param = (CCString*)obj;
    
    if (param->intValue() == 0)
    {
        // 터치 활성
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, priority+1, true);
        //this->setKeypadEnabled(true);
        //this->setTouchEnabled(true);
        this->setTouchPriority(priority);
        isTouched = false;
        CCLog("스케치북 : 터치 활성 (Priority = %d)", this->getTouchPriority());
    }
    else if (param->intValue() == 1)
    {
        // 터치 비활성
        CCLog("스케치북 : 터치 비활성");
        //this->setKeypadEnabled(false);
        //this->setTouchEnabled(false);
        CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
    }
    else if (param->intValue() == 2)
    {
        // 스킬 정보 갱신
        MakeScroll(curState, true);
    }
    else if (param->intValue() == 3)
    {
        // 돈+mp 정보 갱신
        ((CCLabelTTF*)spriteClass->FindLabelByTag(1))->setString(Common::MakeComma(myInfo->GetTopaz()).c_str());
        ((CCLabelTTF*)spriteClass->FindLabelByTag(2))->setString(Common::MakeComma(myInfo->GetStarCandy()).c_str());
        ((CCLabelTTF*)spriteClass->FindLabelByTag(3))->setString(Common::MakeComma(myInfo->GetMPTotal()).c_str());
    }
    else if (param->intValue() == 8)
    {
        // 스킬 속성 개/폐 여부 갱신
        CheckProperties();
    }
    else if (param->intValue() == 9)
    {
        // 스킬 슬롯 정보 갱신
        spriteClassSlot->RemoveAllObjects();
        MakeScrollSlot();
        char name[7];
        sprintf(name, "%d", (int)myInfo->GetSlot().size());
        ((CCLabelTTF*)spriteClass->FindLabelByTag(100))->setString(name);
    }
}

void Sketchbook::InitSprites()
{
    pBlack = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, winSize.width, winSize.height));
    pBlack->setPosition(ccp(0, 0));
    pBlack->setAnchorPoint(ccp(0, 0));
    pBlack->setColor(ccc3(0, 0, 0));
    pBlack->setOpacity(200);
    this->addChild(pBlack, 0);

    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_topinfo.png1", ccp(0, 0), ccp(80, 1666), CCSize(230, 75), "", "Sketchbook", this, 0) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_topinfo.png2", ccp(0, 0), ccp(390, 1666), CCSize(290, 75), "", "Sketchbook", this, 0) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_topinfo.png3", ccp(0, 0), ccp(765, 1666), CCSize(290, 75), "", "Sketchbook", this, 0) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_topaz.png", ccp(0, 0), ccp(15+10, 1656), CCSize(0, 0), "", "Sketchbook", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_starcandy.png", ccp(0, 0), ccp(317, 1660), CCSize(0, 0), "", "Sketchbook", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_magicpoint.png", ccp(0, 0), ccp(696, 1669), CCSize(0, 0), "", "Sketchbook", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_topinfo_plus.png1", ccp(0, 0), ccp(80+230-55, 1679), CCSize(0, 0), "", "Sketchbook", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_topinfo_plus.png2", ccp(0, 0), ccp(390+290-55, 1679), CCSize(0, 0), "", "Sketchbook", this, 5) );
    
    // topaz
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(Common::MakeComma(myInfo->GetTopaz()), fontList[0], 36, ccp(0.5, 0), ccp((80+230+80)/2, 1686), ccc3(255,255,255), "", "Sketchbook", this, 5, 0, 255, 1) );
    // starcandy
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(Common::MakeComma(myInfo->GetStarCandy()), fontList[0], 36, ccp(0.5, 0), ccp((390+290+390)/2, 1686), ccc3(255,255,255), "", "Sketchbook", this, 5, 0, 255, 2) );
    // magic-point
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(Common::MakeComma(myInfo->GetMPTotal()), fontList[0], 36, ccp(0.5, 0), ccp((765+765+290)/2, 1686), ccc3(255,255,255), "", "Sketchbook", this, 5, 0, 255, 3) );
    
    // strap
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_purple.png",
                    ccp(0, 0), ccp(14, 1586+offsets), CCSize(0, 0), "", "Sketchbook", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_x_yellow.png",
                    ccp(0, 0), ccp(875, 1634+offsets), CCSize(0, 0), "", "Sketchbook", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_title_sketchbook.png",
                    ccp(0, 0), ccp(279, 1632+offsets), CCSize(0, 0), "", "Sketchbook", this, 2) );
    
    // select menu
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_sketchbook_select.png1",
                    ccp(0, 0), ccp(100, 1416+offsets), CCSize(0, 0), "", "Sketchbook", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_sketchbook_select.png2",
                    ccp(0, 0), ccp(296, 1416+offsets), CCSize(0, 0), "", "Sketchbook", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_sketchbook_select.png3",
                    ccp(0, 0), ccp(492, 1416+offsets), CCSize(0, 0), "", "Sketchbook", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_sketchbook_select.png4",
                    ccp(0, 0), ccp(688, 1416+offsets), CCSize(0, 0), "", "Sketchbook", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_property_fire.png",
                    ccp(0, 0), ccp(153, 1460+offsets), CCSize(0, 0), "", "Sketchbook", this, 3) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_property_water.png",
                    ccp(0, 0), ccp(350, 1461+offsets), CCSize(0, 0), "", "Sketchbook", this, 3) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_property_land.png",
                    ccp(0, 0), ccp(534, 1464+offsets), CCSize(0, 0), "", "Sketchbook", this, 3) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_property_master.png",
                    ccp(0, 0), ccp(737, 1463+offsets), CCSize(0, 0), "", "Sketchbook", this, 3) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_lock_white.png1", ccp(0, 0), ccp(100+90, 1416+offsets+40), CCSize(0, 0), "", "Sketchbook", this, 3) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_lock_white.png2", ccp(0, 0), ccp(296+90, 1416+offsets+40), CCSize(0, 0), "", "Sketchbook", this, 3) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_lock_white.png3", ccp(0, 0), ccp(492+90, 1416+offsets+40), CCSize(0, 0), "", "Sketchbook", this, 3) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_lock_white.png4", ccp(0, 0), ccp(688+90, 1416+offsets+40), CCSize(0, 0), "", "Sketchbook", this, 3) );
    
    CheckProperties();
    
    // background
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png1",
                    ccp(0, 0), ccp(49, 458), CCSize(982, 974+offsets), "", "Sketchbook", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png1",
                    ccp(0, 0), ccp(77, 492), CCSize(929, 914+offsets), "", "Sketchbook", this, 1) );
    
    // slot part
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png2",
                    ccp(0, 0), ccp(49, 198), CCSize(982, 223), "", "Sketchbook", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png2",
                    ccp(0, 0), ccp(77, 228), CCSize(782, 177), "", "Sketchbook", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_plus_big.png", // slot plus
                    ccp(0, 0), ccp(896, 317), CCSize(0, 0), "", "Sketchbook", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_gameready_name.png0",
                    ccp(0, 0), ccp(867, 242), CCSize(136, 63), "", "Sketchbook", this, 1) );
    // 슬롯 개수 숫자
    char name[7];
    sprintf(name, "%d", (int)myInfo->GetSlot().size());
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(name, fontList[0], 48, ccp(0, 0), ccp(892, 248), ccc3(255,219,53), "", "Sketchbook", this, 5, 0, 255, 100) ); // 현재 개수
    sprintf(name, "/ %d", (int)skillSlotInfo.size());
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(name, fontList[0], 36, ccp(0, 0), ccp(927, 248), ccc3(182,142,142), "", "Sketchbook", this, 5) ); // 젼체 개수
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
}

void Sketchbook::CheckProperties()
{
    if (myInfo->IsFire())
        ((CCSprite*)spriteClass->FindSpriteByName("icon/icon_lock_white.png1"))->setOpacity(0);
    if (myInfo->IsWater())
        ((CCSprite*)spriteClass->FindSpriteByName("icon/icon_lock_white.png2"))->setOpacity(0);
    if (myInfo->IsLand())
        ((CCSprite*)spriteClass->FindSpriteByName("icon/icon_lock_white.png3"))->setOpacity(0);
    if (myInfo->IsMaster())
        ((CCSprite*)spriteClass->FindSpriteByName("icon/icon_lock_white.png4"))->setOpacity(0);
}

void Sketchbook::MakeScroll(int state, bool isFromPopup)
{
    CCLog("make scroll");
    // select button을 눌렀을 때 이 함수를 통해 어떤 layer를 보여줄 지 결정한다.
    if (curState == state && !isFromPopup)
        return;
    
    // sprites init
    spriteClassFire->RemoveAllObjects();
    spriteClassWater->RemoveAllObjects();
    spriteClassLand->RemoveAllObjects();
    spriteClassMaster->RemoveAllObjects();
    
    // menu change
    SetMenuChange(state);

    if (state == 0)
        MakeScrollFire(2);
    else if (state == 1)
        MakeScrollFire(1);
    else if (state == 2)
        MakeScrollFire(3);
    else
        MakeScrollMaster();
    
    curState = state;
}

void Sketchbook::SetMenuChange(int state)
{
    ((CCSprite*)spriteClass->FindSpriteByName("background/bg_sketchbook_select.png1"))->setOpacity(170);
    ((CCSprite*)spriteClass->FindSpriteByName("background/bg_sketchbook_select.png1"))->setZOrder(0);
    ((CCSprite*)spriteClass->FindSpriteByName("icon/icon_property_fire.png"))->setOpacity(170);
    ((CCSprite*)spriteClass->FindSpriteByName("background/bg_sketchbook_select.png2"))->setOpacity(170);
    ((CCSprite*)spriteClass->FindSpriteByName("background/bg_sketchbook_select.png2"))->setZOrder(0);
    ((CCSprite*)spriteClass->FindSpriteByName("icon/icon_property_water.png"))->setOpacity(170);
    ((CCSprite*)spriteClass->FindSpriteByName("background/bg_sketchbook_select.png3"))->setOpacity(170);
    ((CCSprite*)spriteClass->FindSpriteByName("background/bg_sketchbook_select.png3"))->setZOrder(0);
    ((CCSprite*)spriteClass->FindSpriteByName("icon/icon_property_land.png"))->setOpacity(170);
    ((CCSprite*)spriteClass->FindSpriteByName("background/bg_sketchbook_select.png4"))->setOpacity(170);
    ((CCSprite*)spriteClass->FindSpriteByName("background/bg_sketchbook_select.png4"))->setZOrder(0);
    ((CCSprite*)spriteClass->FindSpriteByName("icon/icon_property_master.png"))->setOpacity(170);
    
    char name[38];
    sprintf(name, "background/bg_sketchbook_select.png%d", state+1);
    ((CCSprite*)spriteClass->FindSpriteByName(name))->setOpacity(255);
    ((CCSprite*)spriteClass->FindSpriteByName(name))->setZOrder(2);
    if (state+1 == 1) ((CCSprite*)spriteClass->FindSpriteByName("icon/icon_property_fire.png"))->setOpacity(255);
    else if (state+1 == 2) ((CCSprite*)spriteClass->FindSpriteByName("icon/icon_property_water.png"))->setOpacity(255);
    else if (state+1 == 3) ((CCSprite*)spriteClass->FindSpriteByName("icon/icon_property_land.png"))->setOpacity(255);
    else ((CCSprite*)spriteClass->FindSpriteByName("icon/icon_property_master.png"))->setOpacity(255);
}

/*CCLayer* Sketchbook::GetLvLayer(int level)
{
    CCLayer* layer = CCLayer::create();
    char name[30];
    int offset = 0;
    
    sprintf(name, "number/level_lv.png");
    spriteClassFire->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(0, 0), CCSize(0,0), "", "Layer", layer, 6) );
    if (level >= 10)
    {
        sprintf(name, "number/level_%d.png", level / 10);
        spriteClassFire->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(43+3, 0), CCSize(0,0), "", "Layer", layer, 6) );
        offset = spriteClassFire->spriteObj[spriteClassFire->spriteObj.size()-1]->sprite9->getContentSize().width + 2;
    }
    sprintf(name, "number/level_%d.png", level % 10);
    spriteClassFire->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(43+3+offset, 0), CCSize(0,0), "", "Layer", layer, 6) );
    
    return layer;
}*/

SkillInfo* Sketchbook::GetNextSkillInfo(int state)
{
    // '?' 스킬 여부
    MySkill* ms;
    for (int i = 0 ; i < skillInfo.size() ; i++)
    {
        if (skillInfo[i]->GetId() / 10 != state)
            continue;
        bool flag = true;
        for (int j = 0 ; j < myInfo->GetSkillList().size() ; j++)
        {
            ms = myInfo->GetSkillList()[j];
            if (skillInfo[i]->GetId() == ms->GetCommonId())
                flag = false;
        }
        if (!flag)
            continue;
        
        CCLog("next = %d", skillInfo[i]->GetId());
        for (int j = 0 ; j < myInfo->GetSkillList().size() ; j++)
        {
            ms = myInfo->GetSkillList()[j];
            if (skillInfo[i]->GetRequiredSkillId() == ms->GetCommonId())
            {
                return skillInfo[i];
            }
        }
    }
    return NULL;
}

void Sketchbook::MakeScrollFire(int idx)
{
    std::vector<MySkill*> ms;
    int numOfList = myInfo->GetSkillList().size();
    for (int i = 0 ; i < numOfList ; i++)
    {
        if (myInfo->GetSkillList()[i]->GetCommonId() / 10 == idx)
            ms.push_back(myInfo->GetSkillList()[i]);
    }
    numOfList = ms.size();
    //if (numOfList < 8) // 항상 '?' 스킬이 하나 있기 때문에 (8개 다 배우지 않은 이상)
        numOfList++;
    
    containerFire = CCLayer::create();
    if (numOfList > 8)
        containerFire->setContentSize(CCSizeMake(929, (numOfList-1)*206));
    else
        containerFire->setContentSize(CCSizeMake(929, numOfList*206));
    
    SkillInfo* sInfo;
    int id;
    
    char name[50], name2[50];
    for (int i = 0 ; i < numOfList; i++)
    {
        if (i == numOfList-1)
        {
            sInfo = GetNextSkillInfo(idx);
            if (sInfo == NULL)
                continue;
        }
        else
            sInfo = SkillInfo::GetSkillInfo(ms[i]->GetCommonId());
        
        CCLayer* itemLayer = CCLayer::create();
        itemLayer->setPosition(ccp(27, (numOfList-i-1)*206));
        containerFire->addChild(itemLayer, 5);
        spriteClassFire->layers.push_back(itemLayer);
        
        
        if (i == numOfList-1)
            id = sInfo->GetId();
        else
            id = ms[i]->GetCommonId();
        sprintf(name, "background/bg_board_brown.png%d", i+3);
        spriteClassFire->spriteObj.push_back( SpriteObject::Create(1, name, ccp(0, 0), ccp(0, 0), CCSize(872, 206), "", "Layer", itemLayer, 5, 0, 255, id) );
        
        // 스킬 배경
        sprintf(name, "icon/icon_skill_division_red.png%d", i+3);
        spriteClassFire->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(25, 132), CCSize(0, 0), "", "Layer", itemLayer, 5) );
        sprintf(name, "background/bg_skill_brown.png%d", i+3);
        spriteClassFire->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(25, 51), CCSize(0, 0), "", "Layer", itemLayer, 5) );
        // 스킬 문양
        if (i == numOfList-1) // '?' 스킬인 경우
        {
            spriteClassFire->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_question_skill.png", ccp(0, 0), ccp(68, 80), CCSize(0, 0), "", "Layer", itemLayer, 5) );
        }
        else // 그 외
        {
            sprintf(name2, "skill/skill_%d.png", ms[i]->GetCommonId());
            spriteClassFire->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0, 0), ccp(25+8, 51+8), CCSize(0, 0), "", "Layer", itemLayer, 5) );
        }
        
        // 스킬 레벨
        if (i < numOfList-1)
        {
            int offset = 0;
            sprintf(name, "number/level_lv.png%d", i+3);
            spriteClassFire->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(53, 41), CCSize(0,0), "", "Layer", itemLayer, 6) );
            if (ms[i]->GetLevel() >= 10)
            {
                sprintf(name, "number/level_%d.png", ms[i]->GetLevel() / 10);
                spriteClassFire->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(53+43+3, 41), CCSize(0,0), "",   "Layer", itemLayer, 6) );
                offset = spriteClassFire->spriteObj[spriteClassFire->spriteObj.size()-1]->sprite->getContentSize().width;
            }
            sprintf(name, "number/level_%d.png", ms[i]->GetLevel() % 10);
            spriteClassFire->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(53+43+3+offset, 41), CCSize(0,0), "", "Layer", itemLayer, 6) );
        }
        
        // 스킬 이름 배경
        sprintf(name, "background/bg_gameready_name.png%d", i+3);
        spriteClassFire->spriteObj.push_back( SpriteObject::Create(1, name, ccp(0, 0), ccp(169, 116), CCSize(442, 69), "", "Layer", itemLayer, 5) );
        // 스킬 이름
        CCPoint pos = spriteClassFire->FindParentCenterPos(name);
        spriteClassFire->spriteObj.push_back( SpriteObject::CreateLabel(sInfo->GetName(), fontList[0], 48, ccp(0.5, 0.5), ccp((int)pos.x, (int)pos.y+2), ccc3(255,255,255), name, "1", NULL, 5, 1) );
        // 스킬 간략 설명
        spriteClassFire->spriteObj.push_back( SpriteObject::CreateLabel(SkillInfo::GetShortDesc(sInfo->GetId()), fontList[0], 30, ccp(0.5, 0.5), ccp(169+(int)pos.x, 92), ccc3(255,255,255), "", "Layer", itemLayer, 5) );
        
        // '패시브' 스킬에 대해 '자동효과' 문구 넣기
        if (!sInfo->IsActive() && i < numOfList-1)
        {
            sprintf(name2, "icon/icon_auto_effect.png%d", i+3);
            spriteClassFire->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0, 0), ccp(114, 163), CCSize(0, 0), "", "Layer", itemLayer, 6) );
        }
        
        // 연습량 프로그레스바
        sprintf(name, "background/bg_progress_bar.png%d", i+3);
        spriteClassFire->spriteObj.push_back( SpriteObject::Create(1, name, ccp(0, 0), ccp(133, 28), CCSize(412, 31), "", "Layer", itemLayer, 5) );
        
        // 현재 경험치 (연습량) + 레벨업을 위한 max경험치 (연습량)
        if (i < numOfList-1)
        {
            sprintf(name, "%d", ms[i]->GetExp());
            sprintf(name2, "/%d", SkillBuildUpInfo::GetMaxExp(ms[i]->GetCommonId(), ms[i]->GetLevel()));
        }
        else
        {
            sprintf(name, "0");
            sprintf(name2, "/0");
        }
        spriteClassFire->spriteObj.push_back( SpriteObject::CreateLabel(name, fontList[2], 30, ccp(0, 0), ccp(549, 31), ccc3(255,255,255), "", "Layer", itemLayer, 5) );
        spriteClassFire->spriteObj.push_back( SpriteObject::CreateLabel(name2, fontList[2], 30, ccp(0, 0), ccp(582, 25), ccc3(182,142,142), "", "Layer", itemLayer, 5) );
        
        if (i < numOfList-1)
        {
            // 'MASTER' 버튼
            if (SkillBuildUpInfo::IsMastered(ms[i]->GetCommonId(), ms[i]->GetLevel()))
            {
                sprintf(name, "button/btn_skill_master.png%d", i);
                spriteClassFire->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(633, 61), CCSize(213, 95), "", "Layer", itemLayer, 5) );
                sprintf(name2, "letter/letter_master.png%d", i);
                spriteClassFire->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0.5, 0), ccp(spriteClassFire->spriteObj[spriteClassFire->spriteObj.size()-1]->sprite->getContentSize().width/2, 30), CCSize(0, 0), name, "0", NULL, 5, 1) );
            }
            // '레벨업' 버튼
            else if (ms[i]->GetExp() == SkillBuildUpInfo::GetMaxExp(ms[i]->GetCommonId(), ms[i]->GetLevel()))
            {
                sprintf(name, "button/btn_red_mini.png%d", i+3);
                spriteClassFire->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(633, 51), CCSize(0, 0), "", "Layer", itemLayer, 5, 0, 255, ms[i]->GetCommonId()) ); // 태그에 common_id를 둔다.
                sprintf(name2, "letter/letter_practice.png%d", i+3);
                spriteClassFire->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0.5, 0), ccp(spriteClassFire->spriteObj[spriteClassFire->spriteObj.size()-1]->sprite->getContentSize().width/2, 27), CCSize(0, 0), name, "0", NULL, 5) );
                sprintf(name2, "icon/icon_levelup.png%d", i);
                spriteClassFire->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0, 0), ccp(-4, 73), CCSize(0, 0), name, "0", NULL, 5) );
            }
            // '연습' 버튼
            else
            {
                sprintf(name, "button/btn_red_mini.png%d", i+3);
                spriteClassFire->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(633, 51), CCSize(0, 0), "", "Layer", itemLayer, 5, 0, 255, -ms[i]->GetUserId()) ); // 태그에 user_id를 음수로 둔다.
                sprintf(name2, "letter/letter_practice.png%d", i+3);
                spriteClassFire->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0.5, 0), ccp(spriteClassFire->spriteObj[spriteClassFire->spriteObj.size()-1]->sprite->getContentSize().width/2, 27), CCSize(0, 0), name, "0", NULL, 5) );
            }
        }
        else
        {
            // '?'스킬의 요구조건을 모두 충족한 경우
            if (myInfo->GetMPTotal() >= sInfo->GetRequiredMP() && myInfo->GetStaffLv() >= sInfo->GetRequiredStaffLv() &&
                MySkill::GetObj(sInfo->GetRequiredSkillId())->GetLevel() >= sInfo->GetRequiredSkillLv())
            {
                sprintf(name, "button/btn_green_mini.png%d", i+3);
                spriteClassFire->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(633, 51), CCSize(0, 0), "", "Layer", itemLayer, 5, 0, 255, sInfo->GetId()) );
                sprintf(name2, "icon/icon_levelup.png%d", i);
                spriteClassFire->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0, 0), ccp(-4, 73), CCSize(0, 0), name, "0", NULL, 5, 1) );
            }
            // 아닌 경우
            else
            {
                sprintf(name, "button/btn_green_mini.png%d", i+3);
                spriteClassFire->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(633, 51), CCSize(0, 0), "", "Layer", itemLayer, 5, 0, 255, sInfo->GetId()) );
            }
        }
    }

    // sprite들 add child
    for (int i = 0 ; i < spriteClassFire->spriteObj.size() ; i++)
        spriteClassFire->AddChild(i);

    if (curState != -1)
    {
        scrollViewFire->getContainer()->removeAllChildren();
        scrollViewFire->getContainer()->removeFromParent();
    }
    // scrollView에 container를 새 layer로 설정.
    scrollViewFire->setContainer(containerFire);
    scrollViewFire->setContentSize(containerFire->getContentSize());
    // offset 변경
    scrollViewFire->setContentOffset(ccp(0, scrollViewFire->minContainerOffset().y), false);
}

void Sketchbook::MakeScrollWater()
{

}
void Sketchbook::MakeScrollLand()
{
    
}
void Sketchbook::MakeScrollMaster()
{
    
}

void Sketchbook::MakeScrollSlot()
{
    int numOfList = myInfo->GetSlot().size();
    
    // container 초기화
    containerSlot = CCLayer::create();
    containerSlot->setContentSize(CCSizeMake(numOfList*(146+5), 146));
    
    char fname[50], fname2[20];
    int scid;
    for (int i = 0 ; i < numOfList ; i++)
    {
        sprintf(fname, "background/bg_skill_yellow.png%d", i);
        spriteClassSlot->spriteObj.push_back( SpriteObject::Create(0, fname, ccp(0, 0), ccp(i*(146+5), 0), CCSize(0, 0), "", "Layer", containerSlot, 3) );
        
        scid = myInfo->GetSlot()[i]->GetCommonId();
        CCLog("slot common id = %d", scid);
        if (scid > 0) // 슬롯에 스킬이 있다면 문양을 표시한다.
        {
            sprintf(fname2, "skill/skill_%d.png", scid);
            spriteClassSlot->spriteObj.push_back( SpriteObject::Create(0, fname2, ccp(0.5, 0.5), spriteClassSlot->FindParentCenterPos(fname), CCSize(0, 0), fname, "0", NULL, 4) );
        }
    }
    
    for (int i = 0 ; i < spriteClassSlot->spriteObj.size() ; i++)
        spriteClassSlot->AddChild(i);
    
    // scrollView의 container 재설정
    scrollViewSlot->setContainer(containerSlot);
    scrollViewSlot->setContentSize(containerSlot->getContentSize());
}

void Sketchbook::SetTouchLock(bool val)
{
    isTouched = val;
}

bool Sketchbook::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    if (isTouched)
        return false;
    isTouched = true;
    isSlotTouched = false;
    isScrolling = false;
    isScrollViewTouched = false;
    
    CCPoint point = pTouch->getLocation();
    CCLog("Sketchbook touch began");
    
    if (scrollViewSlot->boundingBox().containsPoint(point))
        isSlotTouched = true;
    if (scrollViewFire->boundingBox().containsPoint(point))
        isScrollViewTouched = true;
    
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        if (spriteClass->spriteObj[i]->name.substr(0, 35) == "background/bg_sketchbook_select.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClickboard();
                int number = atoi(spriteClass->spriteObj[i]->name.substr(35,36).c_str());
                if (number == 4)
                    continue;
                
                if ( (number == 1 && myInfo->IsFire()) || (number == 2 && myInfo->IsWater()) ||
                    (number == 3 && myInfo->IsLand()) || (number == 4 && myInfo->IsMaster()) )
                {
                    MakeScroll(number-1);
                }
                else
                {
                    std::vector<int> data;
                    data.push_back(number); // 불(1), 물(2), 땅(3), 마스터(4)
                    data.push_back(SkillPropertyInfo::GetCost(number)); // 가격
                    Common::ShowPopup(this, "Sketchbook", "NoImage", false, BUY_PROPERTY_TRY, BTN_2, data, -1, priority-1);
                }
                break;
            }
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_x_yellow.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                EndScene();
                break;
            }
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_plus_big.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                // 슬롯 구매
                std::vector<int> data;
                if ((int)myInfo->GetSlot().size() >= (int)skillSlotInfo.size())
                    Common::ShowPopup(this, "Sketchbook", "NoImage", false, BUY_SKILLSLOT_FULL, BTN_1, data, -1, priority-1);
                else
                {
                    data.push_back((int)myInfo->GetSlot().size()+1);
                    data.push_back(SkillSlotInfo::GetCost((int)myInfo->GetSlot().size()+1));
                    if (SkillSlotInfo::GetCostType((int)myInfo->GetSlot().size()+1) == 1)
                        Common::ShowPopup(this, "Sketchbook", "NoImage", false, BUY_SKILLSLOT_BY_STARCANDY_TRY, BTN_2, data, 1, priority-1);
                    else
                        Common::ShowPopup(this, "Sketchbook", "NoImage", false, BUY_SKILLSLOT_BY_TOPAZ_TRY, BTN_2, data, 1, priority-1);
                }
                break;
            }
        }
    }
    
    return true;
}


void Sketchbook::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
    if (!isTouched)
        return;
}

void Sketchbook::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    CCLog("sketchbook touch ended (isTouched = %d)", isTouched);
    if (!isTouched)
        return;
    
    CCPoint point = pTouch->getLocation();
    
    CCLog("%d %d", isScrolling, isSlotTouched);
    if (!isScrolling && isSlotTouched)
    {
        Common::ShowNextScene(this, "Sketchbook", "MagicList", false, 1, priority-1);
    }
    
    CCPoint p;
    for (int i = 0 ; i < spriteClassFire->spriteObj.size() ; i++)
    {
        if (spriteClassFire->spriteObj[i]->name.substr(0, 29) == "background/bg_board_brown.png")
        {
            // 스케치북에서 버튼 외의 영역을 눌렀을 때 스킬상세 팝업창이 뜨게 하자.
            p = spriteClassFire->spriteObj[i]->sprite9->convertToNodeSpace(point);
            CCSize size = spriteClassFire->spriteObj[i]->sprite9->getContentSize();
            if (isScrollViewTouched && !isScrolling &&
                (int)p.x >= 0 && (int)p.y >= 0 && (int)p.x <= 600 && (int)p.y <= size.height)
            {
                sound->playClick();
                int id = spriteClassFire->spriteObj[i]->sprite9->getTag();
                Common::ShowNextScene(this, "Sketchbook", "SketchDetail", false, id, priority-1);
            }
        }
        else if (spriteClassFire->spriteObj[i]->name.substr(0, 23) == "button/btn_red_mini.png")
        {
            p = spriteClassFire->spriteObj[i]->sprite->convertToNodeSpace(point);
            CCSize size = spriteClassFire->spriteObj[i]->sprite->getContentSize();
            if (isScrollViewTouched && !isScrolling &&
                (int)p.x >= 0 && (int)p.y >= 0 && (int)p.x <= size.width && (int)p.y <= size.height)
            {
                sound->playClick();
                int id = spriteClassFire->spriteObj[i]->sprite->getTag();
                
                // '연습' 누를 때
                if (id < 0) // 이때 id는 user_id
                {
                    char temp[255];
                    std::string url = "http://14.63.225.203/cogma/game/practice_skill.php?";
                    sprintf(temp, "kakao_id=%d&", myInfo->GetKakaoId());
                    url += temp;
                    sprintf(temp, "user_skill_id=%d", -id); // 양수로 고치자.
                    url += temp;
                    CCLog("url = %s", url.c_str());
                    
                    CCHttpRequest* req = new CCHttpRequest();
                    req->setUrl(url.c_str());
                    req->setRequestType(CCHttpRequest::kHttpPost);
                    req->setResponseCallback(this, httpresponse_selector(Sketchbook::onHttpRequestCompleted));
                    CCHttpClient::getInstance()->send(req);
                    req->release();
                }
                // 연습량이 다 차서 레벨업을 해야 하는 경우 (강화)
                else // 이때 id는 common_id
                {
                    Common::ShowNextScene(this, "Sketchbook", "SketchDetail", false, id, priority-1);
                }
            }
        }
        else if (spriteClassFire->spriteObj[i]->name.substr(0, 25) == "button/btn_green_mini.png")
        {
            p = spriteClassFire->spriteObj[i]->sprite->convertToNodeSpace(point);
            CCSize size = spriteClassFire->spriteObj[i]->sprite->getContentSize();
            if (isScrollViewTouched && !isScrolling &&
                (int)p.x >= 0 && (int)p.y >= 0 && (int)p.x <= size.width && (int)p.y <= size.height)
            {
                sound->playClick();
                
                // 요구조건을 만족한 경우, 만족하지 않은 경우 둘 다 팝업창을 띄운다.
                int id = spriteClassFire->spriteObj[i]->sprite->getTag();
                Common::ShowNextScene(this, "Sketchbook", "SketchDetail", false, id, priority-1);
            }
        }
    }
    
    isTouched = false;
    isScrolling = false;
    isSlotTouched = false;
    isScrollViewTouched = false;
}

void Sketchbook::scrollViewDidScroll(CCScrollView* view)
{
    isScrolling = true;
}

void Sketchbook::scrollViewDidZoom(CCScrollView* view)
{
}


void Sketchbook::EndScene()
{
    //sound->playClick();
    CCString* param = CCString::create("0");
    if (from == 0)
        CCNotificationCenter::sharedNotificationCenter()->postNotification("Ranking", param);
    else if (from == 1)
    {
        CCNotificationCenter::sharedNotificationCenter()->postNotification("GameReady", param);
        param = CCString::create("9");
        CCNotificationCenter::sharedNotificationCenter()->postNotification("GameReady", param);
    }
    
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, "Sketchbook");
    
    scrollViewFire->removeAllChildren();
    scrollViewFire->removeFromParent();
    scrollViewSlot->removeAllChildren();
    scrollViewSlot->removeFromParent();
    
    this->removeFromParentAndCleanup(true);
}

void Sketchbook::EndSceneCallback()
{
}

void Sketchbook::onHttpRequestCompleted(CCNode *sender, void *data)
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
    
    XmlParsePracticeSkill(dumpData, (int)buffer->size());
}
void Sketchbook::XmlParsePracticeSkill(char* data, int size)
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
        int puid = nodeResult.child("coco").attribute("practice-user-skill-id").as_int();
        
        // myInfo에 현재 연습 스킬 정보 갱신
        for (int i = 0 ; i < myInfo->GetSkillList().size() ; i++)
        {
            MySkill* ms = myInfo->GetSkillList()[i];
            if (ms->GetUserId() == puid)
                myInfo->SetPracticeSkill(ms->GetCommonId(), ms->GetLevel());
        }
        
        // 장면 전환
        if (from == 0) // Sketchbook없애고, 그 부모인 Ranking에 GameReady를 입힌다.
        {
            CCNode* parent = this->getParent();
            EndScene();
            Common::ShowNextScene(parent, "Ranking", "GameReady", false, priority);
        }
        else if (from == 1) // Sketchbook 없앤다.
        {
            // GameReady에 연습 스킬 정보 갱신하기
            CCString* param = CCString::create("5");
            CCNotificationCenter::sharedNotificationCenter()->postNotification("GameReady", param);
            EndScene();
        }
    }
    else
    {
        CCLog("failed code = %d", code);
    }
}