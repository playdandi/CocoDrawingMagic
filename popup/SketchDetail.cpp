#include "SketchDetail.h"
#include "Sketchbook.h"

static int skill_common_id;

SketchDetail::~SketchDetail(void)
{
}

CCScene* SketchDetail::scene(int id)
{
    skill_common_id = id;
    
    CCScene* pScene = CCScene::create();
    SketchDetail* pLayer = SketchDetail::create();
    pScene->addChild(pLayer);
	return pScene;
}

void SketchDetail::onEnter()
{
    //CCLog("SketchDetail :: onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority(), true);
    CCLayer::onEnter();
    
    // 전체화면 액션
    CCActionInterval* action = CCSequence::create( CCSpawn::create(CCMoveTo::create(0.3f, ccp(0, 0)), CCScaleTo::create(0.3f, 1.0f), NULL), CCCallFunc::create(this, callfunc_selector(SketchDetail::SceneCallback)), NULL );
    tLayer->runAction(CCEaseExponentialOut::create(action));
}
void SketchDetail::SceneCallback()
{
    isTouched = false;
}
void SketchDetail::onExit()
{
    //CCLog("SketchDetail :: onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
}

void SketchDetail::keyBackClicked()
{
    if (isKeybackTouched || isTouched)
        return;
    isKeybackTouched = true;
    
    sound->playClick();
    EndScene(true);
}


bool SketchDetail::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
    idx = -1;
    isTouched = true;
    
    // make depth tree
    Depth::AddCurDepth("SketchDetail", this);
    
    this->setTouchEnabled(true);
    this->setKeypadEnabled(true);
    this->setTouchPriority(Depth::GetCurPriority());
    //CCLog("SketchDetail : touch prio = %d", Depth::GetCurPriority());
    
    // notification observer
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(SketchDetail::Notification), Depth::GetCurName(), NULL);
    
    // notification
    CCString* param = CCString::create("1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetParentName(), param);
    
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    tLayer = CCLayer::create();
    tLayer->setAnchorPoint(ccp(0, 0));
    tLayer->setPosition(ccp(winSize.width/2, winSize.height/2));
    this->addChild(tLayer, 1);
    tLayer->setScale(0);
    
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
        isKeybackTouched = false;
        //CCLog("SketchDetail : 터치 활성 (Priority = %d)", this->getTouchPriority());
    }
    else if (param->intValue() == 1)
    {
        // 터치 비활성
        //CCLog("SketchDetail : 터치 비활성");
        isTouched = true;
        isKeybackTouched = true;
        CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
    }
    else if (param->intValue() == 10)
    {
        // 터치 풀기 (백그라운드에서 돌아올 때)
        isTouched = false;
        isKeybackTouched = false;
        if (idx > -1)
        {
            spriteClass->spriteObj[idx]->sprite->setColor(ccc3(255,255,255));
            if (btnStatus == 1)
                ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_buildup_red.png"))->setColor(ccc3(255,255,255));
            else if (btnStatus == 2)
                ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_confirm_mini.png"))->setColor(ccc3(255,255,255));
            else if (btnStatus == 3)
                ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_get.png"))->setColor(ccc3(255,255,255));
            else if (btnStatus == 4)
            {
                ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_purchase.png"))->setColor(ccc3(255,255,255));
                ((CCSprite*)spriteClass->FindSpriteByName("button/btn_green_mini.png"))->setColor(ccc3(255,255,255));
            }
        }
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
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png1", ccp(0, 0), ccp(49, 615), CCSize(982, 623+50), "", "Layer", tLayer, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png", ccp(0, 0), ccp(76, 653), CCSize(929, 562+50), "", "Layer", tLayer, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_popup_rightup.png", ccp(0, 0), ccp(809, 1064), CCSize(0, 0), "", "Layer", tLayer, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_x_brown.png", ccp(0, 0), ccp(900, 1157), CCSize(0, 0), "", "Layer", tLayer, 1) );
    
    char name[40], name2[40];
    int scid = skill_common_id;
    SkillInfo* sInfo = SkillInfo::GetSkillInfo(scid);
    MySkill* ms = MySkill::GetObj(scid);

    isOpened = (ms != NULL);
    
    // 스킬 그림의 배경
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_skill_brown.png", ccp(0, 0), ccp(120, 1090), CCSize(0, 0), "", "Layer", tLayer, 5) );
    
    // 스킬그림 (+ 스킬속성표시, 스킬레벨)
    if (isOpened)
    {
        sprintf(name, "skill_%d.png", scid);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(120+8, 1090+8), CCSize(0, 0), "", "Layer", tLayer, 5) );
        // 스킬속성표시
        if (scid / 10 == 2) sprintf(name, "icon/icon_skill_division_red.png");
        else if (scid / 10 == 1) sprintf(name, "icon/icon_skill_division_blue.png");
        else if (scid / 10 == 3) sprintf(name, "icon/icon_skill_division_green.png");
        else sprintf(name, "icon/icon_skill_division_purple.png");
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(120, 1090+81), CCSize(0, 0), "", "Layer", tLayer, 5) );
        // 스킬레벨
        int offset = 0;
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "number/level_lv.png", ccp(0, 0), ccp(155, 1085), CCSize(0,0), "", "Layer", tLayer, 5) );
        if (ms->GetLevel() >= 10)
        {
            sprintf(name, "number/level_%d.png", ms->GetLevel() / 10);
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(155+43+3, 1085), CCSize(0,0), "",   "Layer", tLayer, 5) );
            offset = spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->getContentSize().width;
        }
        sprintf(name, "number/level_%d.png", ms->GetLevel() % 10);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(155+43+3+offset, 1085), CCSize(0,0), "", "Layer", tLayer, 5) );
        
        // 연습량 프로그레스바 안의 바
        //MySkill* ms = MySkill::GetObj(scid);
        float percentage = ((float)ms->GetExp() / (float)SkillBuildUpInfo::GetMaxExp(ms->GetCommonId(), ms->GetLevel()));
        bar = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, (float)(412-8)*percentage, 31-10));
        bar->setAnchorPoint(ccp(0, 0));
        bar->setPosition(ccp(290+4, 1112+6));
        bar->setColor(ccc3(255,255,255));
        if (ms->GetExp() == SkillBuildUpInfo::GetMaxExp(ms->GetCommonId(), ms->GetLevel()))
            bar->setColor(ccc3(255,219,53));
        tLayer->addChild(bar, 10);
    }
    else
    {
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_question_skill.png", ccp(0, 0), ccp(165, 1118), CCSize(0, 0), "", "Layer", tLayer, 5) );
    }
    
    // 스킬 이름 배경
    //spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_gameready_name.png1", ccp(0, 0), ccp(300, 1177), CCSize(442, 89), "", "Layer", tLayer, 5) );
    // 스킬 이름
    //CCPoint pos = spriteClass->FindParentCenterPos("background/bg_gameready_name.png1");
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(sInfo->GetName(), fontList[0], 60, ccp(0, 0), ccp(300, 1152), ccc3(0,0,0), "", "Layer", tLayer, 5) );
    //spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(sInfo->GetName(), fontList[0], 48, ccp(0.5, 0.5), ccp((int)pos.x, (int)pos.y+2), ccc3(0,0,0), "background/bg_gameready_name.png1", "1", NULL, 5, 1) );
    
    // '패시브' 스킬에 대해 '자동효과' 문구 넣기
    if (isOpened && !sInfo->IsActive())
    {
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_auto_effect.png", ccp(0, 0), ccp(120+89, 1090+112), CCSize(0, 0), "", "Layer", tLayer, 5) );
    }
    
    // 연습량 프로그레스바
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_progress_bar.png", ccp(0, 0), ccp(290, 1112), CCSize(412, 31), "", "Layer", tLayer, 5) );
    
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
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(name, fontList[2], 30, ccp(1, 0), ccp(300+412+10+33-3, 1117), ccc3(255,255,255), "", "Layer", tLayer, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(name2, fontList[2], 30, ccp(0, 0), ccp(300+412+10+33, 1112), ccc3(182,142,142), "", "Layer", tLayer, 5) );
    
    // dotted line
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_dotted_line.png", ccp(0, 0), ccp(105, 1070), CCSize(0, 0), "", "Layer", tLayer, 5) );
    
    
    if (isOpened)
    {
        MakeOpenedSkillSprites();
    }
    else
    {
        MakeClosedSkillSprites();
    }
}


void SketchDetail::MakeOpenedSkillSprites()
{
    int scid = skill_common_id;
    MySkill* ms = MySkill::GetObj(scid);
    
    // 내가 원하는 그림
    char temp[30];
    sprintf(temp, "images/skilldetail/%d.png", scid);
    sp = CCSprite::create(temp);
    sp->setPosition(ccp(120+360/2, 684+360/2));
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_degree_desc.png", ccp(0, 0), ccp(120-3, 684-3), CCSize(360+6, 360+6), "", "Layer", tLayer, 3) );
    
    // mask sprite
    mask = CCScale9Sprite::createWithSpriteFrameName("background/bg_degree_desc.png");
    mask->setPosition(120, 684);
    mask->setAnchorPoint(ccp(0, 0));
    mask->setContentSize(CCSize(360, 360));
    
    clip = CCClippingNode::create();
    clip->setInverted(false);
    clip->setAlphaThreshold(0);
    clip->addChild(sp);
    
    node = CCNode::create();
    node->addChild(mask);
    clip->setStencil(node);
    tLayer->addChild(clip, 5);
    
    
    // 가격표 (스킬을 마스터했을 경우 표시하지 않는다)
    if (!SkillBuildUpInfo::IsMastered(ms->GetCommonId(), ms->GetLevel()))
    {
        spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_degree_desc.png", ccp(0, 0), ccp(540, 688), CCSize(201, 77), "", "Layer", tLayer, 5) );
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_starcandy_mini.png", ccp(0, 0), ccp(550, 695), CCSize(0, 0), "", "Layer", tLayer, 5) );
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(Common::MakeComma(SkillBuildUpInfo::GetCost(scid, ms->GetLevel()+1)), fontList[0], 36, ccp(0, 0), ccp(617, 708), ccc3(255,255,255), "", "Layer", tLayer, 5) );
    }
    
    // 문구
    CCLayer* descLayer = CCLayer::create();
    descLayer->setAnchorPoint(ccp(0, 0));
    descLayer->setPosition(140, 700);
    tLayer->addChild(descLayer, 5);
    spriteClass->layers.push_back(descLayer);
   
    // 스킬 설명
    std::string desc = SkillDescription(scid);
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabelArea(desc, fontList[0], 36, ccp(0, 1), ccp(540, 1020), ccc3(78,47,8), CCSize(430, 600), kCCTextAlignmentLeft, kCCVerticalTextAlignmentTop, "", "Layer", tLayer, 5) );
    
    // 버튼
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_red_mini.png", ccp(0, 0), ccp(760, 673), CCSize(0, 0), "", "Layer", tLayer, 5, 0, 255, scid) );
    CCPoint pos = spriteClass->FindParentCenterPos("button/btn_red_mini.png");

    // 레벨업 해야하는 경우 (강화)
    if (ms->GetExp() == SkillBuildUpInfo::GetMaxExp(scid, ms->GetLevel()) &&
        !SkillBuildUpInfo::IsMastered(ms->GetCommonId(), ms->GetLevel()))
    {
        btnStatus = 1;
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_buildup_red.png", ccp(0.5, 0.5), ccp(760+pos.x, 673+pos.y+2), CCSize(0, 0), "", "Layer", tLayer, 5) );
    }
    // 일반적인 경우
    else
    {
        btnStatus = 2;
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_confirm_mini.png", ccp(0.5, 0.5), ccp(760+pos.x, 673+pos.y+2), CCSize(0, 0), "", "Layer", tLayer, 5) );
    }
    
    // 버튼 젤리 움직임
    CCSprite* temp2 = ((CCSprite*)spriteClass->FindSpriteByName("button/btn_red_mini.png"));
    CCSize t = temp2->getContentSize();
    temp2->setAnchorPoint(ccp(0.5, 0.5));
    temp2->setPosition(ccp(temp2->getPosition().x+t.width/2, temp2->getPosition().y+t.height/2));
    CCActionInterval* action = CCSequence::create( CCScaleTo::create(1.0f, 1.03f, 0.96f), CCScaleTo::create(1.0f, 0.97f, 1.04f), NULL );
    temp2->runAction(CCRepeatForever::create(action));
}


std::string SketchDetail::SkillDescription(int scid)
{
    switch (scid)
    {
        case 21: return "빨간 피스를 그리면\n추가점수를 획득해요.\n(강화시키면 추가점수를\n더 획득해요)"; break;
        case 22: return "빨간 피스를 사이클로 그리면 개수에 비례해, 주변 피스를 더 터뜨릴 수 있어요\n(강화시키면 추가점수를\n더 획득해요)"; break;
        case 23: return "사랑의 불꽃으로\n빨간 피스를 만들어요.\n(강화시키면 좀 더 자주 등장하고 추가점수를 획득해요)"; break;
        case 24: return "빨간 피스를 한번에\n8개 이상 그리면\n추가점수를 획득해요.\n(강화시키면 추가점수를\n더 확득해요)"; break;
        case 25: return "빨간 피스를 한번에\n6개 이상 그리면\n불꽃으로 한번 더 터뜨려요.\n(강화시키면 추가점수가\n증가해요)"; break;
        case 26: return "피버타임으로 즉시\n전환되요.\n(강화시키면 더 자주\n시전되요)"; break;
        case 27: return "빨간 피스를 한번에\n10개 이상 그리면\n드래곤을 소환해요!\n(강화시키면 운석이 더 많이\n떨어져요)"; break;

        case 11: return "파란 피스를 그리면\n추가점수를 획득해요.\n(강화시키면 추가점수를\n더 획득해요)"; break;
        case 12: return "파란 피스를 사이클로 그리면 파도가 피스를 터뜨려요.\n(강화시키면 추가점수를\n더 획득해요)"; break;
        case 13: return "나비가 날아와서\n흰색, 노란색 피스를\n파란 피스로 만들어요.\n(강화시키면 더 많은 피스를\n생성해요)"; break;
        case 14: return "10콤보마다 추가점수를\n획득해요.\n(강화시키면 추가점수를\n더 획득해요)"; break;
        case 15: return "파란 피스를 한번에\n6개 이상 그리면\n얼음비가 한번 더 터뜨려요.\n(강화시키면 추가점수가\n증가해요)"; break;
        case 16: return "코코가 시간을 얼려\n잠깐 시간이 가지 않아요.\n(강화시키면 더 자주\n발동해요)"; break;
        case 17: return "파란 피스를 한번에\n10개 이상 그리면\n여신을 소환해요!\n(강화시키면 지속시간이\n늘어나요)"; break;

        case 31: return "초록 피스를 그리면\n추가점수를 획득해요.\n(강화시키면 추가점수를\n더 획득해요)"; break;
        case 32: return "초록 피스를 사이클로 그리면 개수에 비례해, 무작위로 피스를 터뜨려요.\n(강화시키면 추가점수를\n더 획득해요)"; break;
        case 33: return "마법 잎사귀가 초록피스를\n모두 모아줘요.\n(강화시키면 좀 더 자주 등장하고 추가점수를 획득해요)"; break;
        case 34: return "마법지팡이로 추가별사탕을\n획득해요.\n(강화시키면 추가점수를\n더 획득해요)"; break;
        case 35: return "초록 피스를 한번에\n6개 이상 그리면\n바람으로 한번 더 터뜨려요.\n(강화시키면 추가점수가\n증가해요)"; break;
        case 36: return "가끔씩 포션 1개를 얻을 수\n있어요.\n(강화시키면 더 자주\n발동해요)"; break;
        case 37: return "초록 피스를 한번에\n10개 이상 그리면\n고대나무를 소환해요!\n(강화시키면 더 강력해져요)"; break;
    }
    return "";
}


bool compare_sd(MySkill* ms1, MySkill* ms2)
{
    return ms1->GetLearnTime() < ms2->GetLearnTime();
}
void SketchDetail::SortMySkillByUserId()
{
    std::sort(sList.begin(), sList.end(), compare_sd);
}

void SketchDetail::MakeClosedSkillSprites()
{
    char name[40];
    
    int scid = skill_common_id;
    SkillInfo* sInfo = SkillInfo::GetSkillInfo(scid);
    
    // 문구
    CCLayer* descLayer = CCLayer::create();
    descLayer->setAnchorPoint(ccp(0, 0));
    descLayer->setPosition(180, 950);
    tLayer->addChild(descLayer, 5);
    spriteClass->layers.push_back(descLayer);
    
    // 지금 이 스킬을 배우는데 요구되는 MP
    sList = myInfo->GetSkillList();
    SortMySkillByUserId();
    int requiredMP = SkillBuildupMPInfo::RequiredMP(sList, sInfo->GetId());
    
    // 스킬을 새로 습득하려면?
    // 조건1) prerequisite 스킬의 레벨이 요구레벨 이상!
    // 조건2) 내 MP가 요구MP 이상! (결제를 통해 얻은 속성의 수에 따라 요구MP 할인이 있을 수 있다)
    
    // '?'스킬의 요구조건을 모두 충족한 경우
    if (MySkill::GetObj(sInfo->GetRequiredSkillId())->GetLevel() >= sInfo->GetRequiredSkillLv() &&
        myInfo->GetMPTotal() >= requiredMP)
    {
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabelArea("새로운 마법을 배울 준비가 다 되었어요.\n'습득' 버튼을 클릭해 보세요!", fontList[0], 34, ccp(0, 1), ccp(150, 1105), ccc3(78,47,8), CCSize(779, 180), kCCTextAlignmentCenter, kCCVerticalTextAlignmentCenter, "", "Layer", tLayer, 5) );
        
        // 가격 표시
        spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_degree_desc.png", ccp(0, 0), ccp(540, 688), CCSize(201, 77), "", "Layer", tLayer, 5) );
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_starcandy_mini.png", ccp(0, 0), ccp(550, 695), CCSize(0, 0), "", "Layer", tLayer, 5) );
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(Common::MakeComma(SkillBuildUpInfo::GetCost(scid, 1)), fontList[0], 36, ccp(0, 0), ccp(617, 708), ccc3(255,255,255), "", "Layer", tLayer, 5) );
    }
    else
    {
        /*
        // 도전 문장의 배경
        spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_cocoroom_desc.png4", ccp(0, 0), ccp(210, 822), CCSize(730, 58), "", "Layer", tLayer, 5) );
        char name[100];
        sprintf(name, "도전 : MP %d 달성", requireMP);
        
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(name, fontList[0], 36, ccp(0, 0), ccp(230, 830), ccc3(255,255,255), "", "Layer", tLayer, 5) );
        CCSize s = spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->label->getContentSize();
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("아래의 조건 달성시 별사탕으로 스킬습득 가능", fontList[2], 30, ccp(0, 0), ccp(230+s.width+10, 835), ccc3(255,255,255), "", "Layer", tLayer, 5, 170) );
        
        */
        
        
        //아래의 요건을 모두 갖추면 별사탕으로 이 마법을 배울 수 있어요.
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabelArea("아래의 조건 달성시 별사탕으로 스킬습득 가능", fontList[0], 36, ccp(0, 1), ccp(150, 1100), ccc3(78,47,8), CCSize(779, 180), kCCTextAlignmentCenter, kCCVerticalTextAlignmentCenter, "", "Layer", tLayer, 5) );
        
        // 가격 표시
        spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_degree_desc.png", ccp(0, 0), ccp(540, 688), CCSize(201, 77), "", "Layer", tLayer, 5) );
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_topaz_mini.png", ccp(0, 0), ccp(550, 695), CCSize(0, 0), "", "Layer", tLayer, 5) );

        int number = SkillBuildupMPInfo::GetRealOrder(sList, skill_common_id);
        //CCLog("scid = %d / number = %d", skill_common_id, number);
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(Common::MakeComma(SkillBuildupMPInfo::GetObj(number-1)->GetTopazCostValue()), fontList[0], 36, ccp(0, 0), ccp(617, 708), ccc3(255,255,255), "", "Layer", tLayer, 5) );
    }
    
    int ofs = 15;
    int lofs = 20;
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "number/rank_1.png", ccp(0, 1), ccp(3+lofs, 0-ofs), CCSize(0, 0), "", "Layer", descLayer, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "number/rank_2.png", ccp(0, 1), ccp(0+lofs, -85-ofs), CCSize(0, 0), "", "Layer", descLayer, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "number/rank_comma.png1", ccp(0, 1), ccp(26+lofs, -30-ofs), CCSize(0, 0), "", "Layer", descLayer, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "number/rank_comma.png2", ccp(0, 1), ccp(30+lofs, -115-ofs), CCSize(0, 0), "", "Layer", descLayer, 5) );

    ((CCSprite*)spriteClass->FindSpriteByName("number/rank_1.png"))->setScale(0.8f);
    ((CCSprite*)spriteClass->FindSpriteByName("number/rank_2.png"))->setScale(0.8f);
    ((CCSprite*)spriteClass->FindSpriteByName("number/rank_comma.png1"))->setScale(0.8f);
    ((CCSprite*)spriteClass->FindSpriteByName("number/rank_comma.png2"))->setScale(0.8f);
    
    // 만족된 조건에 대해 숫자 옆에 체크 표시하기
    if (myInfo->GetMPTotal() >= requiredMP)
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_check.png1", ccp(0, 0), ccp(-70+lofs, -43-ofs), CCSize(0, 0), "", "Layer", descLayer, 5) );
    if (MySkill::GetObj(sInfo->GetRequiredSkillId())->GetLevel() >= sInfo->GetRequiredSkillLv())
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_check.png1", ccp(0, 0), ccp(-70+lofs, -128-ofs), CCSize(0, 0), "", "Layer", descLayer, 5) );
    
    SkillInfo* sInfoReq = SkillInfo::GetSkillInfo(sInfo->GetRequiredSkillId());
    
    // MP아이콘, 스킬아이콘 배치
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_magicpoint.png", ccp(0.5, 0.5), ccp(100+lofs, -23-ofs), CCSize(0, 0), "", "Layer", descLayer, 5) );
    ((CCSprite*)spriteClass->FindSpriteByName("icon/icon_magicpoint.png"))->setScale(0.8f);
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_skill_brown.png5", ccp(0.5, 0.5), ccp(100+lofs, -105-ofs), CCSize(0, 0), "", "Layer", descLayer, 5) );
    ((CCSprite*)spriteClass->FindSpriteByName("background/bg_skill_brown.png5"))->setScale(0.6f);
    CCPoint pos = spriteClass->FindParentCenterPos("background/bg_skill_brown.png5");
    sprintf(name, "skill_%d.png", sInfoReq->GetId());
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0.5, 0.5), pos, CCSize(0, 0), "background/bg_skill_brown.png5", "0", spriteClass->FindSpriteByName("background/bg_skill_brown.png5"), 5, 1) );
    
    // 각 조건마다의 문구
    sprintf(name, "%d 이상", requiredMP);
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(name, fontList[0], 36, ccp(0, 1), ccp(160+lofs, 0-ofs), ccc3(0,0,0), "", "Layer", descLayer, 5) );
    sprintf(name, "Lv %d 이상 (%s)", sInfo->GetRequiredSkillLv(), sInfoReq->GetName().c_str());
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(name, fontList[0], 36, ccp(0, 1), ccp(160+lofs, -85-ofs), ccc3(0,0,0), "", "Layer", descLayer, 5) );
    
    
    
    // 버튼
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_red_mini.png", ccp(0, 0), ccp(760, 673), CCSize(0, 0), "", "Layer", tLayer, 5, 0, 255, scid) );
    pos = spriteClass->FindParentCenterPos("button/btn_red_mini.png");
    
    // '?'스킬의 요구조건을 모두 충족한 경우
    if (MySkill::GetObj(sInfo->GetRequiredSkillId())->GetLevel() >= sInfo->GetRequiredSkillLv() &&
        myInfo->GetMPTotal() >= requiredMP)
    {
        btnStatus = 3;
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_get.png", ccp(0.5, 0.5), ccp(760+pos.x, 673+pos.y+2), CCSize(0, 0), "", "Layer", tLayer, 5) );
    }
    // 아닌 경우
    else
    {
        btnStatus = 4;
        ((CCSprite*)spriteClass->FindSpriteByTag(scid))->setOpacity(0);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_green_mini.png", ccp(0, 0), ccp(760, 673), CCSize(0, 0), "", "Layer", tLayer, 7, 0, 255, scid) );
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_purchase.png", ccp(0.5, 0.5), ccp(760+pos.x, 673+pos.y+2), CCSize(0, 0), "", "Layer", tLayer, 7) );
        
        // 버튼 젤리 움직임
        CCSprite* temp = ((CCSprite*)spriteClass->FindSpriteByName("button/btn_green_mini.png"));
        CCSize t = temp->getContentSize();
        temp->setAnchorPoint(ccp(0.5, 0.5));
        temp->setPosition(ccp(temp->getPosition().x+t.width/2, temp->getPosition().y+t.height/2));
        CCActionInterval* action = CCSequence::create( CCScaleTo::create(1.0f, 1.03f, 0.96f), CCScaleTo::create(1.0f, 0.97f, 1.04f), NULL );
        temp->runAction(CCRepeatForever::create(action));
    }
    
    // 버튼 젤리 움직임
    CCSprite* temp = ((CCSprite*)spriteClass->FindSpriteByName("button/btn_red_mini.png"));
    CCSize t = temp->getContentSize();
    temp->setAnchorPoint(ccp(0.5, 0.5));
    temp->setPosition(ccp(temp->getPosition().x+t.width/2, temp->getPosition().y+t.height/2));
    CCActionInterval* action = CCSequence::create( CCScaleTo::create(1.0f, 1.03f, 0.96f), CCScaleTo::create(1.0f, 0.97f, 1.04f), NULL );
    temp->runAction(CCRepeatForever::create(action));
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
                return true;
            }
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_red_mini.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                spriteClass->spriteObj[i]->sprite->setColor(ccc3(170,170,170));
                if (btnStatus == 1)
                    ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_buildup_red.png"))->setColor(ccc3(170,170,170));
                else if (btnStatus == 2)
                    ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_confirm_mini.png"))->setColor(ccc3(170,170,170));
                else if (btnStatus == 3)
                    ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_get.png"))->setColor(ccc3(170,170,170));
                else if (btnStatus == 4)
                {
                    ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_purchase.png"))->setColor(ccc3(170,170,170));
                    ((CCSprite*)spriteClass->FindSpriteByName("button/btn_green_mini.png"))->setColor(ccc3(170,170,170));
                }
                
                rect = spriteClass->spriteObj[i]->sprite->boundingBox();
                kind = BTN_MENU_CONFIRM;
                idx = i;
                return true;
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
    CCPoint point = pTouch->getLocation();
    
    if (idx > -1)
    {
        spriteClass->spriteObj[idx]->sprite->setColor(ccc3(255,255,255));
        if (btnStatus == 1)
            ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_buildup_red.png"))->setColor(ccc3(255,255,255));
        else if (btnStatus == 2)
            ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_confirm_mini.png"))->setColor(ccc3(255,255,255));
        else if (btnStatus == 3)
            ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_get.png"))->setColor(ccc3(255,255,255));
        else if (btnStatus == 4)
        {
            ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_purchase.png"))->setColor(ccc3(255,255,255));
            ((CCSprite*)spriteClass->FindSpriteByName("button/btn_green_mini.png"))->setColor(ccc3(255,255,255));
        }
    }
    if (rect.containsPoint(point))
    {
        if (kind == BTN_MENU_CONFIRM)
        {
            if (btnStatus == 1) // 레벨업 해야하는 경우 (강화)
            {
                MySkill* ms = MySkill::GetObj(skill_common_id);
                
                if (myInfo->GetStarCandy() < SkillBuildUpInfo::GetCost(skill_common_id, ms->GetLevel()+1))
                {
                    std::vector<int> nullData;
                    Common::ShowPopup(this, "SketchDetail", "NoImage", true, NEED_TO_BUY_STARCANDY, BTN_2, nullData);
                }
                else
                {
                    // Loading 화면으로 MESSAGE request 넘기기
                    Common::ShowNextScene(this, "SketchDetail", "Loading", false, LOADING_MESSAGE);
                    
                    // upgrade skill
                    char temp[255];
                    std::string param = "";
                    sprintf(temp, "kakao_id=%s&", myInfo->GetKakaoId().c_str());
                    param += temp;
                    sprintf(temp, "user_skill_id=%d", ms->GetUserId());
                    param += temp;
                    
                    Network::HttpPost(param, URL_UPGRADE_SKILL, this, httpresponse_selector(SketchDetail::onHttpRequestCompleted), "0");
                }
            }
            else if (btnStatus == 2) // 일반적인 경우
            {
                EndScene(true);
            }
            else if (btnStatus == 3) // '?'스킬의 요구조건을 모두 충족한 경우
            {
                // Loading 화면으로 MESSAGE request 넘기기
                Common::ShowNextScene(this, "SketchDetail", "Loading", false, LOADING_MESSAGE);
                
                char temp[255];
                std::string param = "";
                sprintf(temp, "kakao_id=%s&", myInfo->GetKakaoId().c_str());
                param += temp;
                sprintf(temp, "skill_id=%d&", skill_common_id);
                param += temp;
                sprintf(temp, "cost_type=1"); // 1 : 별사탕, 2 : 토파즈
                param += temp;
                
                Network::HttpPost(param, URL_PURCHASE_SKILL, this, httpresponse_selector(SketchDetail::onHttpRequestCompleted), "1");
            }
            else if (btnStatus == 4) // '?'스킬 요구조건 미충족 -> 토파즈로 살 수 있는 경우
            {
                std::vector<int> data;
                data.push_back(skill_common_id); // 스킬 common id
                int number = SkillBuildupMPInfo::GetRealOrder(sList, skill_common_id);
                data.push_back(SkillBuildupMPInfo::GetObj(number-1)->GetTopazCostValue()); // 토파즈 비용
                data.push_back(number); // 스킬 몇 번째인지.
                Common::ShowPopup(this, "SketchDetail", "NoImage", false, PURCHASE_SKILL_BY_TOPAZ_TRY, BTN_2, data);
            }
        }
    }
    
    isTouched = false;
}

void SketchDetail::EndScene(bool isNoti)
{
    // remove this notification
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, Depth::GetCurName());
    // release depth tree
    Depth::RemoveCurDepth();
    
    if (isNoti)
    {        
        // touch 넘겨주기 (GetCurName = 위에서 remove를 했기 때문에 결국 여기 입장에서는 부모다)
        CCString* param = CCString::create("0");
        CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetCurName(), param);
    }
    
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    
    // remove all CCNodes
    if (isOpened)
    {
        sp->removeFromParentAndCleanup(true);
        mask->removeFromParentAndCleanup(true);
        node->removeFromParentAndCleanup(true);
        clip->removeFromParentAndCleanup(true);
    }

    spriteClass->RemoveAllObjects();
    delete spriteClass;
    pBlack->removeFromParentAndCleanup(true);
    
    tLayer->removeAllChildren();
    tLayer->removeFromParentAndCleanup(true);
    
    this->removeFromParentAndCleanup(true);
}



void SketchDetail::onHttpRequestCompleted(CCNode *sender, void *data)
{
    // Loading 창 끄기
    ((Loading*)Depth::GetCurPointer())->EndScene();
    
    CCHttpResponse* res = (CCHttpResponse*) data;

    xml_document xmlDoc;
    Network::GetXMLFromResponseData(res, xmlDoc);
    
    int tag = atoi(res->getHttpRequest()->getTag());
    if (tag == 0 || tag == 1)
        XmlParseUpgradeOrPurchaseSkill(&xmlDoc, tag);
}

void SketchDetail::XmlParseUpgradeOrPurchaseSkill(xml_document *xmlDoc, int tag)
{
    xml_node nodeResult = xmlDoc->child("response");
    int code = nodeResult.child("code").text().as_int();
    
    // 에러일 경우 code에 따라 적절히 팝업창 띄워줌.
    if (code != 0)
    {
        std::vector<int> nullData;
        if (code <= MAX_COMMON_ERROR_CODE && code != 3) // '돈 모자람'의 경우는 따로 처리한다.
            Network::ShowCommonError(code);
        else if (tag == 0) // UPGRADE SKILL 실패한 경우
        {
            if (code == 3) // 돈 모자람.
                Common::ShowPopup(this, "SketchDetail", "NoImage", true, NEED_TO_BUY_STARCANDY, BTN_2, nullData);
            else if (code >= 10 && code <= 12)
            {
                // code 10 : 가지고 있는 스킬이 아님.
                // code 11 : 스킬이 이미 만렙임.
                // code 12 : 연습량 미달.
                Common::ShowPopup(this, "SketchDetail", "NoImage", true, UPGRADE_SKILL_FAIL, BTN_1, nullData, -1);
            }
            else
                Common::ShowPopup(this, "SketchDetail", "NoImage", true, NETWORK_FAIL, BTN_1, nullData);
        }
        else if (tag == 1) // PURCHASE SKILL 실패한 경우
        {
            if (code == 3) // 돈 모자람.
                Common::ShowPopup(this, "SketchDetail", "NoImage", true, NEED_TO_BUY_STARCANDY, BTN_2, nullData);
            else if (code >= 10 && code <= 15)
            {
                // code 10 : 존재하지 않는 스킬 ID.
                // code 11 : 해당 스킬 속성을 지닌 마법사가 아님.
                // code 12 : 요구 MP 미달.
                // code 13 : 요구 지팡이의 레벨 미달.
                // code 14 : 요구 스킬의 소유or레벨 미달.
                // code 15 : 이미 배운 스킬임.
                Common::ShowPopup(this, "SketchDetail", "NoImage", true, PURCHASE_SKILL_FAIL, BTN_1, nullData, -1);
            }
            else
                Common::ShowPopup(this, "SketchDetail", "NoImage", true, NETWORK_FAIL, BTN_1, nullData);
        }
        else
            Common::ShowPopup(this, "SketchDetail", "NoImage", true, NETWORK_FAIL, BTN_1, nullData);
    }
    
    else if (code == 0)
    {
        // 돈 갱신
        int topaz = nodeResult.child("money").attribute("topaz").as_int();
        int starcandy = nodeResult.child("money").attribute("star-candy").as_int();
        myInfo->SetMoney(topaz, starcandy);
        
        // 나의 스킬 리스트 갱신
        myInfo->ClearSkillList();
        xml_object_range<xml_named_node_iterator> its = nodeResult.child("skill-list").children("skill");
        int csi, usi, level, exp, learntime;
        for (xml_named_node_iterator it = its.begin() ; it != its.end() ; ++it)
        {
            for (xml_attribute_iterator ait = it->attributes_begin() ; ait != it->attributes_end() ; ++ait)
            {
                std::string name = ait->name();
                if (name == "common-skill-id") csi = ait->as_int();
                else if (name == "user-skill-id") usi = ait->as_int();
                else if (name == "level") level = ait->as_int();
                else if (name == "exp") exp = ait->as_int();
                else if (name == "learn-time") learntime = ait->as_int();
            }
            myInfo->AddSkill(csi, usi, level, exp, learntime);
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
        
        // 게임준비 연습스킬 정보 갱신
        for (int i = 0 ; i < myInfo->GetSkillList().size() ; i++)
        {
            if (myInfo->GetSkillList()[i]->GetCommonId() == myInfo->GetPracticeSkillId())
            {
                myInfo->SetPracticeSkill(myInfo->GetPracticeSkillId(), myInfo->GetSkillList()[i]->GetLevel());
                break;
            }
        }
        param = CCString::create("5");
        CCNotificationCenter::sharedNotificationCenter()->postNotification("GameReady", param);

        // 성공/실패 팝업창 띄우기
        std::vector<int> data;
        data.push_back(skill_common_id); // 스킬 common id
        data.push_back(MySkill::GetObj(skill_common_id)->GetLevel()); // 증가한 스킬 레벨
        if (tag == 0) // 스킬을 레벨업한 경우
            Common::ShowPopup(this, "SketchDetail", "NoImage", true, UPGRADE_SKILL_OK, BTN_1, data, -1);
        else if (tag == 1) // 스킬을 새로 배운 경우
            Common::ShowPopup(this, "SketchDetail", "NoImage", true, PURCHASE_SKILL_OK, BTN_1, data, -1);
    }
}

