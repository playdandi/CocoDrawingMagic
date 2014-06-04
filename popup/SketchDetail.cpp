#include "SketchDetail.h"
#include "Sketchbook.h"
#include "../pugixml/pugixml.hpp"

using namespace pugi;

static int skill_common_id;
static int priority;

SketchDetail::~SketchDetail(void)
{
    //CCLog("SketchDetail destructor");
}

CCScene* SketchDetail::scene(int id, int prio)
{
    skill_common_id = id;
    priority = prio;
    
    CCScene* pScene = CCScene::create();
    SketchDetail* pLayer = SketchDetail::create();
    pScene->addChild(pLayer);
	return pScene;
}

void SketchDetail::onEnter()
{
    CCLog("SketchDetail :: onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority(), true);
    CCLayer::onEnter();
}
void SketchDetail::onExit()
{
    CCLog("SketchDetail :: onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
}

void SketchDetail::keyBackClicked()
{
    sound->playClick();
    EndScene(true);
}


bool SketchDetail::init()
{
	if (!CCLayer::init())
	{
		return false;
	}

    // make depth tree
    Depth::AddCurDepth("SketchDetail", this);
    
    this->setTouchEnabled(true);
    this->setKeypadEnabled(true);
    this->setTouchPriority(Depth::GetCurPriority());
    CCLog("SketchDetail : touch prio = %d", Depth::GetCurPriority());
    
    // notification observer
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(SketchDetail::Notification), Depth::GetCurName(), NULL);
    
    // notification
    CCString* param = CCString::create("1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetParentName(), param);
    
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    InitSprites();
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);

    return true;
}

void SketchDetail::Notification(CCObject* obj)
{
    CCString* param = (CCString*)obj;
    
    if (param->intValue() == 0 || param->intValue() == -1)
    {
        // 터치 활성
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority()+1, true);
        this->setTouchPriority(Depth::GetCurPriority());
        isTouched = false;
        CCLog("SketchDetail : 터치 활성 (Priority = %d)", this->getTouchPriority());
    }
    else if (param->intValue() == 1)
    {
        // 터치 비활성
        CCLog("SketchDetail : 터치 비활성");
        CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
    }
    else if (param->intValue() == 10)
    {
        // 터치 풀기 (백그라운드에서 돌아올 때)
        isTouched = false;
    }
}


void SketchDetail::InitSprites()
{
    pBlack = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, winSize.width, winSize.height));
    pBlack->setPosition(ccp(0, 0));
    pBlack->setAnchorPoint(ccp(0, 0));
    pBlack->setColor(ccc3(0, 0, 0));
    pBlack->setOpacity(150);
    this->addChild(pBlack, 0);
    
    spriteClass = new SpriteClass();
    
    // make pop-up background
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png1", ccp(0, 0), ccp(49, 615), CCSize(982, 623+50), "", "SketchDetail", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png", ccp(0, 0), ccp(76, 653), CCSize(929, 562+50), "", "SketchDetail", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_popup_rightup.png", ccp(0, 0), ccp(809, 1064), CCSize(0, 0), "", "SketchDetail", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_x_brown.png", ccp(0, 0), ccp(900, 1157), CCSize(0, 0), "", "SketchDetail", this, 1) );
    
    char name[40], name2[40];
    int scid = skill_common_id;
    SkillInfo* sInfo = SkillInfo::GetSkillInfo(scid);
    MySkill* ms = MySkill::GetObj(scid);

    bool isOpened = (ms != NULL);
    
    // 스킬 그림의 배경
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_skill_brown.png", ccp(0, 0), ccp(120, 1090), CCSize(0, 0), "", "SketchDetail", this, 5) );
    
    // 스킬그림 (+ 스킬속성표시, 스킬레벨)
    if (isOpened)
    {
        sprintf(name, "skill_%d.png", scid);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(120+8, 1090+8), CCSize(0, 0), "", "SketchDetail", this, 5) );
        // 스킬속성표시
        if (scid / 10 == 2) sprintf(name, "icon/icon_skill_division_red.png");
        else if (scid / 10 == 1) sprintf(name, "icon/icon_skill_division_blue.png");
        else if (scid / 10 == 3) sprintf(name, "icon/icon_skill_division_green.png");
        else sprintf(name, "icon/icon_skill_division_purple.png");
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(120, 1090+81), CCSize(0, 0), "", "SketchDetail", this, 5) );
        // 스킬레벨
        int offset = 0;
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "number/level_lv.png", ccp(0, 0), ccp(155, 1085), CCSize(0,0), "", "SketchDetail", this, 5) );
        if (ms->GetLevel() >= 10)
        {
            sprintf(name, "number/level_%d.png", ms->GetLevel() / 10);
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(155+43+3, 1085), CCSize(0,0), "",   "SketchDetail", this, 5) );
            offset = spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->getContentSize().width;
        }
        sprintf(name, "number/level_%d.png", ms->GetLevel() % 10);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(155+43+3+offset, 1085), CCSize(0,0), "", "SketchDetail", this, 5) );
        
        // 연습량 프로그레스바 안의 바
        //MySkill* ms = MySkill::GetObj(scid);
        float percentage = ((float)ms->GetExp() / (float)SkillBuildUpInfo::GetMaxExp(ms->GetCommonId(), ms->GetLevel()));
        bar = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, (float)(412-8)*percentage, 31-10));
        bar->setAnchorPoint(ccp(0, 0));
        bar->setPosition(ccp(290+4, 1112+6));
        bar->setColor(ccc3(255,255,255));
        if (ms->GetExp() == SkillBuildUpInfo::GetMaxExp(ms->GetCommonId(), ms->GetLevel()))
            bar->setColor(ccc3(255,219,53));
        this->addChild(bar, 10);
    }
    else
    {
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_question_skill.png", ccp(0, 0), ccp(165, 1118), CCSize(0, 0), "", "SketchDetail", this, 5) );
    }
    
    // 스킬 이름 배경
    //spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_gameready_name.png1", ccp(0, 0), ccp(300, 1177), CCSize(442, 89), "", "SketchDetail", this, 5) );
    // 스킬 이름
    //CCPoint pos = spriteClass->FindParentCenterPos("background/bg_gameready_name.png1");
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(sInfo->GetName(), fontList[0], 60, ccp(0, 0), ccp(300, 1152), ccc3(0,0,0), "", "SketchDetail", this, 5) );
    //spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(sInfo->GetName(), fontList[0], 48, ccp(0.5, 0.5), ccp((int)pos.x, (int)pos.y+2), ccc3(0,0,0), "background/bg_gameready_name.png1", "1", NULL, 5, 1) );
    
    // '패시브' 스킬에 대해 '자동효과' 문구 넣기
    if (isOpened && !sInfo->IsActive())
    {
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_auto_effect.png", ccp(0, 0), ccp(120+89, 1090+112), CCSize(0, 0), "", "SketchDetail", this, 5) );
    }
    
    // 연습량 프로그레스바
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_progress_bar.png", ccp(0, 0), ccp(290, 1112), CCSize(412, 31), "", "SketchDetail", this, 5) );
    
    // 현재 경험치 (연습량) + 레벨업을 위한 max경험치 (연습량)
    if (isOpened)
    {
        sprintf(name, "%d", ms->GetExp());
        sprintf(name2, "/%d", SkillBuildUpInfo::GetMaxExp(ms->GetCommonId(), ms->GetLevel()));
    }
    else // '?'인 경우
    {
        sprintf(name, "0");
        sprintf(name2, "/0");
    }
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(name, fontList[2], 30, ccp(1, 0), ccp(300+412+10+33-3, 1117), ccc3(255,255,255), "", "SketchDetail", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(name2, fontList[2], 30, ccp(0, 0), ccp(300+412+10+33, 1112), ccc3(182,142,142), "", "SketchDetail", this, 5) );
    
    // dotted line
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_dotted_line.png", ccp(0, 0), ccp(105, 1070), CCSize(0, 0), "", "SketchDetail", this, 5) );
    
    
    if (isOpened)
    {
        MakeOpenedSkillSprites();
    }
    else
    {
        MakeClosedSkillSprites();
    }
    
    // 가격표 (스킬을 마스터했을 경우 표시하지 않는다)
    if ( (isOpened && !SkillBuildUpInfo::IsMastered(ms->GetCommonId(), ms->GetLevel())) ||
          !isOpened )
    {
        spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_degree_desc.png", ccp(0, 0), ccp(540, 688), CCSize(201, 77), "", "SketchDetail", this, 5) );
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_starcandy_mini.png", ccp(0, 0), ccp(550, 695), CCSize(0, 0), "", "SketchDetail", this, 5) );
        if (isOpened)
            spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(Common::MakeComma(SkillBuildUpInfo::GetCost(scid, ms->GetLevel()+1)), fontList[0], 36, ccp(0, 0), ccp(617, 708), ccc3(255,255,255), "", "SketchDetail", this, 5) );
        else
            spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(Common::MakeComma(SkillBuildUpInfo::GetCost(scid, 1)), fontList[0], 36, ccp(0, 0), ccp(617, 708), ccc3(255,255,255), "", "SketchDetail", this, 5) );
    }
}


void SketchDetail::MakeOpenedSkillSprites()
{
    int scid = skill_common_id;
    //SkillInfo* sInfo = SkillInfo::GetSkillInfo(scid);
    MySkill* ms = MySkill::GetObj(scid);
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_degree_desc.png", ccp(0, 0), ccp(120, 680), CCSize(390, 390), "", "SketchDetail", this, 4) );
    
    // 문구
    CCLayer* descLayer = CCLayer::create();
    descLayer->setAnchorPoint(ccp(0, 0));
    descLayer->setPosition(140, 700);
    this->addChild(descLayer, 5);
    spriteClass->layers.push_back(descLayer);
    
    /*
    // 640 1136
    //////////////////////////////////////////////////////////////////////////////////////////////////
    char name[50];
    CCArray* animFrames = CCArray::createWithCapacity(4);
    for (int i = 0 ; i < 4; i++)
    {
        sprintf(name, "a%d.png", i+1);
        CCSpriteFrame* frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(name);
        animFrames->addObject(frame);
    }
    
    CCSprite* first = CCSprite::createWithSpriteFrameName("a1.png");
    first->setAnchorPoint(ccp(0, 0));
    first->setPosition(ccp(120, 680));
    first->setScale((float)390/(float)640);
    this->addChild(first, 10);
    
    CCAnimation* animation = CCAnimation::createWithSpriteFrames(animFrames, 0.1f);
    CCAnimate* animate = CCAnimate::create(animation);
    CCFiniteTimeAction* action = animate;
    
    first->runAction(CCRepeatForever::create((CCActionInterval*)action));
    //////////////////////////////////////////////////////////////////////////////////////////////////
    */
    
    //int p11[25] = {0,0,0,0,0, 0,0,0,0,0, 0,0,1,1,0, 0,0,0,1,0, 0,0,0,0,0};
    //char temp[50];
    /*
    for (int i = 0 ; i < 5 ; i++)
    {
        for (int j = 0 ; j < 5 ; j++)
        {
            if (p11[i*5+j] == 0) sprintf(temp, "icon/3.png%d", i*5+j);
            else if (p11[i*5+j] == 1) sprintf(temp, "icon/0.png%d", i*5+j);
            else if (p11[i*5+j] == 2) sprintf(temp, "icon/1.png%d", i*5+j);
            else if (p11[i*5+j] == 3) sprintf(temp, "icon/2.png%d", i*5+j);
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, temp, ccp(0, 0), ccp(i*65+10*(i-1), j*65+10*(j-1)), CCSize(0,0), "", "Layer", descLayer, 5) );
            ((CCSprite*)spriteClass->FindSpriteByName(temp))->setScale((float)65/(float)138);
        }
    }
    */
    //sprintf(temp, "icon/0.png");
    //spriteClass->spriteObj.push_back( SpriteObject::Create(0, temp, ccp(0, 0), ccp(150, 150), CCSize(0,0), "", "Layer", descLayer, 5) );
    
    // 스킬 설명
    std::string desc = SkillDescription(scid);
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabelArea(desc, fontList[0], 28, ccp(0, 1), ccp(540, 1070), ccc3(0,0,0), CCSize(420, 250), kCCTextAlignmentLeft, kCCVerticalTextAlignmentCenter, "", "SketchDetail", this, 5) );
    
    // 버튼
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_red_mini.png", ccp(0, 0), ccp(760, 673), CCSize(0, 0), "", "SketchDetail", this, 5, 0, 255, scid) );
    CCPoint pos = spriteClass->FindParentCenterPos("button/btn_red_mini.png");

    // 레벨업 해야하는 경우 (강화)
    if (ms->GetExp() == SkillBuildUpInfo::GetMaxExp(scid, ms->GetLevel()) &&
        !SkillBuildUpInfo::IsMastered(ms->GetCommonId(), ms->GetLevel()))
    {
        btnStatus = 1;
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_buildup_red.png", ccp(0.5, 0.5), ccp((int)pos.x, (int)pos.y+2), CCSize(0, 0), "button/btn_red_mini.png", "0", NULL, 5, 1) );
    }
    // 일반적인 경우
    else
    {
        btnStatus = 2;
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_confirm_mini.png", ccp(0.5, 0.5), ccp((int)pos.x, (int)pos.y+2), CCSize(0, 0), "button/btn_red_mini.png", "0", NULL, 5, 1) );
    }
}

std::string SketchDetail::SkillDescription(int scid)
{
    scid = 21;
    switch (scid)
    {
        case 21: return "붉은색 피스를 터뜨리면 추가점수를 얻어요.\n\n스킬 강화를 통해 추가점수의 양을 더 상승시킬 수 있어요."; break;
    }
    return "";
}

void SketchDetail::MakeClosedSkillSprites()
{
    char name[40];
    
    int scid = skill_common_id;
    SkillInfo* sInfo = SkillInfo::GetSkillInfo(scid);
    //MySkill* ms = MySkill::GetObj(scid);
    
    // 문구
    CCLayer* descLayer = CCLayer::create();
    descLayer->setAnchorPoint(ccp(0, 0));
    descLayer->setPosition(180, 950);
    this->addChild(descLayer, 5);
    spriteClass->layers.push_back(descLayer);
    
    
    // '?'스킬의 요구조건을 모두 충족한 경우
    if (myInfo->GetMPTotal() >= sInfo->GetRequiredMP() && myInfo->GetStaffLv() >= sInfo->GetRequiredStaffLv() && MySkill::GetObj(sInfo->GetRequiredSkillId())->GetLevel() >= sInfo->GetRequiredSkillLv())
    {
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabelArea("아래의 요건을 충족하여 새로운 마법을 배울 준비가 다 되었어요. '배움' 버튼을 클릭해 보세요!", fontList[0], 28, ccp(0, 1), ccp(150, 1115), ccc3(0,0,0), CCSize(779, 180), kCCTextAlignmentLeft, kCCVerticalTextAlignmentCenter, "", "SketchDetail", this, 5) );
    }
    else
    {
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabelArea("아래의 요건을 모두 갖추면 이 마법을 배울 수 있어요. 조금만 더 힘내면 더 강해진 코코를 볼 수 있어요!", fontList[0], 28, ccp(0, 1), ccp(150, 1115), ccc3(0,0,0), CCSize(779, 180), kCCTextAlignmentLeft, kCCVerticalTextAlignmentCenter, "", "SketchDetail", this, 5) );
    }
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "number/rank_1.png", ccp(0, 1), ccp(3, 0), CCSize(0, 0), "", "Layer", descLayer, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "number/rank_2.png", ccp(0, 1), ccp(0, -85), CCSize(0, 0), "", "Layer", descLayer, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "number/rank_3.png", ccp(0, 1), ccp(0, -170), CCSize(0, 0), "", "Layer", descLayer, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "number/rank_comma.png1", ccp(0, 1), ccp(26, -30), CCSize(0, 0), "", "Layer", descLayer, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "number/rank_comma.png2", ccp(0, 1), ccp(30, -115), CCSize(0, 0), "", "Layer", descLayer, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "number/rank_comma.png3", ccp(0, 1), ccp(30, -200), CCSize(0, 0), "", "Layer", descLayer, 5) );
    ((CCSprite*)spriteClass->FindSpriteByName("number/rank_1.png"))->setScale(0.8f);
    ((CCSprite*)spriteClass->FindSpriteByName("number/rank_2.png"))->setScale(0.8f);
    ((CCSprite*)spriteClass->FindSpriteByName("number/rank_3.png"))->setScale(0.8f);
    ((CCSprite*)spriteClass->FindSpriteByName("number/rank_comma.png1"))->setScale(0.8f);
    ((CCSprite*)spriteClass->FindSpriteByName("number/rank_comma.png2"))->setScale(0.8f);
    ((CCSprite*)spriteClass->FindSpriteByName("number/rank_comma.png3"))->setScale(0.8f);
    
    // 만족된 조건에 대해 숫자 옆에 체크 표시하기
    if (myInfo->GetMPTotal() >= sInfo->GetRequiredMP())
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_check.png1", ccp(0, 0), ccp(-70, -43), CCSize(0, 0), "", "Layer", descLayer, 5) );
    if (MySkill::GetObj(sInfo->GetRequiredSkillId())->GetLevel() >= sInfo->GetRequiredSkillLv())
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_check.png1", ccp(0, 0), ccp(-70, -128), CCSize(0, 0), "", "Layer", descLayer, 5) );
    if (myInfo->GetStaffLv() >= sInfo->GetRequiredStaffLv())
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_check.png1", ccp(0, 0), ccp(-70, -213), CCSize(0, 0), "", "Layer", descLayer, 5) );
    
    SkillInfo* sInfoReq = SkillInfo::GetSkillInfo(sInfo->GetRequiredSkillId());
    
    // MP아이콘, 스킬아이콘 배치
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_magicpoint.png", ccp(0.5, 0.5), ccp(100, -23), CCSize(0, 0), "", "Layer", descLayer, 5) );
    ((CCSprite*)spriteClass->FindSpriteByName("icon/icon_magicpoint.png"))->setScale(0.8f);
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_skill_brown.png5", ccp(0.5, 0.5), ccp(100, -105), CCSize(0, 0), "", "Layer", descLayer, 5) );
    ((CCSprite*)spriteClass->FindSpriteByName("background/bg_skill_brown.png5"))->setScale(0.6f);
    CCPoint pos = spriteClass->FindParentCenterPos("background/bg_skill_brown.png5");
    sprintf(name, "skill_%d.png", sInfoReq->GetId());
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0.5, 0.5), pos, CCSize(0, 0), "background/bg_skill_brown.png5", "0", spriteClass->FindSpriteByName("background/bg_skill_brown.png5"), 5, 1) );
    
    // 각 조건마다의 문구
    sprintf(name, "%d 이상", sInfo->GetRequiredMP());
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(name, fontList[0], 36, ccp(0, 1), ccp(160, 0), ccc3(0,0,0), "", "Layer", descLayer, 5) );
    sprintf(name, "Lv %d 이상 (%s)", sInfo->GetRequiredSkillLv(), sInfoReq->GetName().c_str());
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(name, fontList[0], 36, ccp(0, 1), ccp(160, -85), ccc3(0,0,0), "", "Layer", descLayer, 5) );
    sprintf(name, "지팡이 Lv %d 이상", sInfo->GetRequiredStaffLv());
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(name, fontList[0], 36, ccp(0, 1), ccp(65, -170), ccc3(0,0,0), "", "Layer", descLayer, 5) );
    
    
    
    // 버튼
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_red_mini.png", ccp(0, 0), ccp(760, 673), CCSize(0, 0), "", "SketchDetail", this, 5, 0, 255, scid) );
    pos = spriteClass->FindParentCenterPos("button/btn_red_mini.png");
    
    // '?'스킬의 요구조건을 모두 충족한 경우
    if (myInfo->GetMPTotal() >= sInfo->GetRequiredMP() && myInfo->GetStaffLv() >= sInfo->GetRequiredStaffLv() && MySkill::GetObj(sInfo->GetRequiredSkillId())->GetLevel() >= sInfo->GetRequiredSkillLv())
    {
        btnStatus = 3;
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_get.png", ccp(0.5, 0.5), ccp((int)pos.x, (int)pos.y+2), CCSize(0, 0), "button/btn_red_mini.png", "0", NULL, 5, 1) );
    }
    // 아닌 경우
    else
    {
        btnStatus = 4;
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_lock_white.png", ccp(0.5, 0.5), ccp((int)pos.x, (int)pos.y+2), CCSize(0, 0), "button/btn_red_mini.png", "0", NULL, 5, 1) );
    }
}


bool SketchDetail::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    if (isTouched)
        return false;
    isTouched = true;
    
    CCPoint point = pTouch->getLocation();
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        if (spriteClass->spriteObj[i]->name == "button/btn_x_brown.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                EndScene(true);
                break;
            }
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_red_mini.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                
                if (btnStatus == 1) // 레벨업 해야하는 경우 (강화)
                {
                    MySkill* ms = MySkill::GetObj(skill_common_id);
                    
                    if (myInfo->GetStarCandy() < SkillBuildUpInfo::GetCost(skill_common_id, ms->GetLevel()+1))
                    {
                        std::vector<int> nullData;
                        Common::ShowPopup(this, "SketchDetail", "NoImage", false, NEED_TO_BUY_STARCANDY, BTN_2, nullData);
                    }
                    else
                    {
                        // Loading 화면으로 MESSAGE request 넘기기
                        Common::ShowNextScene(this, "SketchDetail", "Loading", false, LOADING_MESSAGE);
                        
                        // upgrade skill
                        char temp[255];
                        std::string url = "http://14.63.225.203/cogma/game/upgrade_skill.php?";
                        sprintf(temp, "kakao_id=%d&", myInfo->GetKakaoId());
                        url += temp;
                        sprintf(temp, "user_skill_id=%d&", ms->GetUserId());
                        url += temp;
                        sprintf(temp, "cost_value=%d", SkillBuildUpInfo::GetCost(skill_common_id, ms->GetLevel()+1)); // 레벨+1
                        url += temp;
                        CCLog("url = %s", url.c_str());
                        
                        CCHttpRequest* req = new CCHttpRequest();
                        req->setUrl(url.c_str());
                        req->setRequestType(CCHttpRequest::kHttpPost);
                        req->setResponseCallback(this, httpresponse_selector(SketchDetail::onHttpRequestCompleted));
                        req->setTag("0");
                        CCHttpClient::getInstance()->send(req);
                        req->release();
                    }
                    break;
                }
                else if (btnStatus == 2) // 일반적인 경우
                {
                    EndScene(true);
                    break;
                }
                else if (btnStatus == 3) // '?'스킬의 요구조건을 모두 충족한 경우
                {
                    // Loading 화면으로 MESSAGE request 넘기기
                    Common::ShowNextScene(this, "SketchDetail", "Loading", false, LOADING_MESSAGE);
                    
                    // purchase skill
                    //http://14.63.225.203/cogma/game/purchase_skill.php?kakao_id=1000&skill_id=22&cost_value=0
                    
                    char temp[255];
                    std::string url = "http://14.63.225.203/cogma/game/purchase_skill.php?";
                    sprintf(temp, "kakao_id=%d&", myInfo->GetKakaoId());
                    url += temp;
                    sprintf(temp, "skill_id=%d&", skill_common_id);
                    url += temp;
                    sprintf(temp, "cost_value=%d", SkillBuildUpInfo::GetCost(skill_common_id, 1)); // 새로 배우니 당연히 레벨 1
                    url += temp;
                    CCLog("url = %s", url.c_str());
                    
                    CCHttpRequest* req = new CCHttpRequest();
                    req->setUrl(url.c_str());
                    req->setRequestType(CCHttpRequest::kHttpPost);
                    req->setResponseCallback(this, httpresponse_selector(SketchDetail::onHttpRequestCompleted));
                    req->setTag("1");
                    CCHttpClient::getInstance()->send(req);
                    req->release();
                    break;
                }
            }
        }
    }
    
    return true;
}


void SketchDetail::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
}

void SketchDetail::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    isTouched = false;
}

void SketchDetail::EndScene(bool isNoti)
{
    // remove this notification
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, Depth::GetCurName());
    // release depth tree
    Depth::RemoveCurDepth();
    
    Depth::DumpDepth();
    
    if (isNoti)
    {        
        // touch 넘겨주기 (GetCurName = 위에서 remove를 했기 때문에 결국 여기 입장에서는 부모다)
        CCString* param = CCString::create("0");
        CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetCurName(), param);
    }
    
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    
    // remove all CCNodes
    spriteClass->RemoveAllObjects();
    delete spriteClass;
    pBlack->removeFromParentAndCleanup(true);
    
    this->removeFromParentAndCleanup(true);
}



void SketchDetail::onHttpRequestCompleted(CCNode *sender, void *data)
{
    CCHttpResponse* res = (CCHttpResponse*) data;
    if (!res || !res->isSucceed())
    {
        CCLog("res failed. error buffer: %s", res->getErrorBuffer());
        return;
    }
    
    // Loading 창 끄기
    ((Loading*)Depth::GetCurPointer())->EndScene();
    
    // dump data
    std::vector<char> *buffer = res->getResponseData();
    char dumpData[BUFFER_SIZE];
    for (unsigned int i = 0 ; i < buffer->size() ; i++)
        dumpData[i] = (*buffer)[i];
    dumpData[buffer->size()] = NULL;
    
    int tag = atoi(res->getHttpRequest()->getTag());
    if (tag == 0 || tag == 1)
        XmlParseUpgradeOrPurchaseSkill(dumpData, (int)buffer->size(), tag);
}
void SketchDetail::XmlParseUpgradeOrPurchaseSkill(char* data, int size, int tag)
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
        // 돈 갱신
        int topaz = nodeResult.child("money").attribute("topaz").as_int();
        int starcandy = nodeResult.child("money").attribute("star-candy").as_int();
        myInfo->SetMoney(topaz, starcandy);
        
        // 나의 스킬 리스트 갱신
        myInfo->ClearSkillList();
        xml_object_range<xml_named_node_iterator> its = nodeResult.child("skill-list").children("skill");
        int csi, usi, level, exp;
        for (xml_named_node_iterator it = its.begin() ; it != its.end() ; ++it)
        {
            for (xml_attribute_iterator ait = it->attributes_begin() ; ait != it->attributes_end() ; ++ait)
            {
                std::string name = ait->name();
                if (name == "common-skill-id") csi = ait->as_int();
                else if (name == "user-skill-id") usi = ait->as_int();
                else if (name == "level") level = ait->as_int();
                else if (name == "exp") exp = ait->as_int();
            }
            myInfo->AddSkill(csi, usi, level, exp);
        }
        myInfo->SortMySkillByCommonId(); // common-skill-id 오름차순 정렬
        
        // 돈 정보 화면 갱신
        CCString* param = CCString::create("2");
        int from = ((Sketchbook*)this->getParent())->FromWhere();
        if (from == 0) // Ranking의 돈 정보 갱신
            CCNotificationCenter::sharedNotificationCenter()->postNotification("Ranking", param);
        else if (from == 1) // GameReady의 돈 정보 갱신
            CCNotificationCenter::sharedNotificationCenter()->postNotification("GameReady", param);
        param = CCString::create("3");
        CCNotificationCenter::sharedNotificationCenter()->postNotification("Sketchbook", param);
        
        // 스케치북 스킬리스트 화면 정보 갱신
        param = CCString::create("2");
        CCNotificationCenter::sharedNotificationCenter()->postNotification("Sketchbook", param);
        
        
        
        // 성공/실패 팝업창 띄우기
        std::vector<int> data;
        data.push_back(skill_common_id); // 스킬 common id
        data.push_back(MySkill::GetObj(skill_common_id)->GetLevel()); // 증가한 스킬 레벨
        if (tag == 0) // 스킬을 레벨업한 경우
            Common::ShowPopup(this, "SketchDetail", "NoImage", true, UPGRADE_SKILL_OK, BTN_1, data, -1, priority-1);
        else if (tag == 1) // 스킬을 새로 배운 경우
            Common::ShowPopup(this, "SketchDetail", "NoImage", true, PURCHASE_SKILL_OK, BTN_1, data, -1, priority-1);
    }
    else
    {
        std::vector<int> nullData;
        if (tag == 0) // UPGRADE SKILL 실패한 경우
        {
            if (code == 3) {
                std::vector<int> nullData;
                Common::ShowPopup(this, "SketchDetail", "NoImage", false, NEED_TO_BUY_STARCANDY, BTN_2, nullData);
            }
            else if (code == 10) CCLog("SketchDetail : 가지고 있는 스킬 아님");
            else if (code == 11) CCLog("SketchDetail : 스킬이 만렙임");
            else if (code == 12) CCLog("SketchDetail : 연습량 미달");
            else if (code == 4) CCLog("SketchDetail : 가격 잘못되었음. 재부팅.");
            Common::ShowPopup(this, "SketchDetail", "NoImage", true, UPGRADE_SKILL_FAIL, BTN_1, nullData, -1, priority-1);
        }
        else if (tag == 1) // PURCHASE SKILL 실패한 경우
        {
            if (code == 3) {
                std::vector<int> nullData;
                Common::ShowPopup(this, "SketchDetail", "NoImage", false, NEED_TO_BUY_STARCANDY, BTN_2, nullData);
            }
            else if (code == 10) CCLog("SketchDetail : 존재하지 않는 스킬 ID");
            else if (code == 11) CCLog("SketchDetail : 해당 스킬 속성을 지닌 마법사가 아님");
            else if (code == 12) CCLog("SketchDetail : 요구 MP 미달");
            else if (code == 13) CCLog("SketchDetail : 요구 지팡이 Lv 미달");
            else if (code == 14) CCLog("SketchDetail : 요구 스킬 소유/Lv 미달");
            else if (code == 15) CCLog("SketchDetail : 이미 배운 스킬");
            else if (code == 4) CCLog("SketchDetail : 가격 잘못되었음. 재부팅.");
            Common::ShowPopup(this, "SketchDetail", "NoImage", true, PURCHASE_SKILL_FAIL, BTN_1, nullData, -1, priority-1);
        }
    }
}

