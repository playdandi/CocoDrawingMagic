#include "T_SketchDetail.h"
#include "T_Sketchbook.h"

static int skill_common_id;


CCScene* T_SketchDetail::scene(int id)
{
    skill_common_id = id;
    
    CCScene* pScene = CCScene::create();
    T_SketchDetail* pLayer = T_SketchDetail::create();
    pScene->addChild(pLayer);
	return pScene;
}

void T_SketchDetail::onEnter()
{
    CCLog("SketchDetail :: onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority(), true);
    CCLayer::onEnter();
}
void T_SketchDetail::onExit()
{
    CCLog("SketchDetail :: onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
}

void T_SketchDetail::keyBackClicked()
{
    sound->playClick();
    EndScene(true);
}


bool T_SketchDetail::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
    // make depth tree
    Depth::AddCurDepth("T_SketchDetail", this);
    
    this->setTouchEnabled(true);
    this->setKeypadEnabled(true);
    this->setTouchPriority(Depth::GetCurPriority());
    CCLog("T_SketchDetail : touch prio = %d", Depth::GetCurPriority());
    
    // notification observer
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(T_SketchDetail::Notification), Depth::GetCurName(), NULL);
    
    // notification
    CCString* param = CCString::create("1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetParentName(), param);
    
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    InitSprites();
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
    
    
    ttrArrow = CCSprite::create("images/tutorial_arrow.png");
    ttrPos = CCSprite::create("images/tutorial_position.png");
    
    // 760, 673 (0, 0)
    ttrArrow->setAnchorPoint(ccp(0.5, 0));
    ttrArrow->setPosition(ccp(760+233/2, 673+115+10));
    CCActionInterval* action = CCSequence::create( CCMoveBy::create(0.5f, ccp(0, -5)), CCMoveBy::create(0.5f, ccp(0, 5)), NULL);
    ttrArrow->runAction(CCRepeatForever::create(action));
    this->addChild(ttrArrow, 101);
    
    ttrPos->setAnchorPoint(ccp(0, 0));
    ttrPos->setPosition(ccp(760, 673));
    ttrPos->setScaleX( (float)233 / (float)162 );
    ttrPos->setScaleY( (float)115 / (float)89 );
    this->addChild(ttrPos, 101);
    
    return true;
}

void T_SketchDetail::Notification(CCObject* obj)
{
    CCString* param = (CCString*)obj;
    
    if (param->intValue() == 0 || param->intValue() == -1)
    {
        // 터치 활성
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority()+1, true);
        this->setTouchPriority(Depth::GetCurPriority());
        isTouched = false;
        CCLog("T_SketchDetail : 터치 활성 (Priority = %d)", this->getTouchPriority());
    }
    else if (param->intValue() == 1)
    {
        // 터치 비활성
        CCLog("T_SketchDetail : 터치 비활성");
        CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
    }
    else if (param->intValue() == 10)
    {
        // 터치 풀기 (백그라운드에서 돌아올 때)
        isTouched = false;
    }
}


void T_SketchDetail::InitSprites()
{
    pBlack = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, winSize.width, winSize.height));
    pBlack->setPosition(ccp(0, 0));
    pBlack->setAnchorPoint(ccp(0, 0));
    pBlack->setColor(ccc3(0, 0, 0));
    pBlack->setOpacity(150);
    this->addChild(pBlack, 0);
    
    spriteClass = new SpriteClass();
    
    // make pop-up background
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png1", ccp(0, 0), ccp(49, 615), CCSize(982, 623+50), "", "T_SketchDetail", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png", ccp(0, 0), ccp(76, 653), CCSize(929, 562+50), "", "T_SketchDetail", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_popup_rightup.png", ccp(0, 0), ccp(809, 1064), CCSize(0, 0), "", "T_SketchDetail", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_x_brown.png", ccp(0, 0), ccp(900, 1157), CCSize(0, 0), "", "T_SketchDetail", this, 1) );
    
    char name[40], name2[40];
    int scid = skill_common_id;
    SkillInfo* sInfo = SkillInfo::GetSkillInfo(scid);
    MySkill* ms = MySkill::GetObj(scid);
    
    bool isOpened = (ms != NULL);
    
    // 스킬 그림의 배경
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_skill_brown.png", ccp(0, 0), ccp(120, 1090), CCSize(0, 0), "", "T_SketchDetail", this, 5) );
    
    // 스킬그림 (+ 스킬속성표시, 스킬레벨)
    /*
    if (isOpened)
    {
        sprintf(name, "skill_%d.png", scid);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(120+8, 1090+8), CCSize(0, 0), "", "T_SketchDetail", this, 5) );
        // 스킬속성표시
        if (scid / 10 == 2) sprintf(name, "icon/icon_skill_division_red.png");
        else if (scid / 10 == 1) sprintf(name, "icon/icon_skill_division_blue.png");
        else if (scid / 10 == 3) sprintf(name, "icon/icon_skill_division_green.png");
        else sprintf(name, "icon/icon_skill_division_purple.png");
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(120, 1090+81), CCSize(0, 0), "", "T_SketchDetail", this, 5) );
        // 스킬레벨
        int offset = 0;
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "number/level_lv.png", ccp(0, 0), ccp(155, 1085), CCSize(0,0), "", "T_SketchDetail", this, 5) );
        if (ms->GetLevel() >= 10)
        {
            sprintf(name, "number/level_%d.png", ms->GetLevel() / 10);
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(155+43+3, 1085), CCSize(0,0), "",   "T_SketchDetail", this, 5) );
            offset = spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->getContentSize().width;
        }
        sprintf(name, "number/level_%d.png", ms->GetLevel() % 10);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(155+43+3+offset, 1085), CCSize(0,0), "", "T_SketchDetail", this, 5) );
        
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
     */
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_question_skill.png", ccp(0, 0), ccp(165, 1118), CCSize(0, 0), "", "T_SketchDetail", this, 5) );
    //}
    
    
    // 스킬 이름
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(sInfo->GetName(), fontList[0], 60, ccp(0, 0), ccp(300, 1152), ccc3(0,0,0), "", "T_SketchDetail", this, 5) );
    
    /*
    // '패시브' 스킬에 대해 '자동효과' 문구 넣기
    if (isOpened && !sInfo->IsActive())
    {
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_auto_effect.png", ccp(0, 0), ccp(120+89, 1090+112), CCSize(0, 0), "", "T_SketchDetail", this, 5) );
    }
    */
    
    // 연습량 프로그레스바
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_progress_bar.png", ccp(0, 0), ccp(290, 1112), CCSize(412, 31), "", "T_SketchDetail", this, 5) );
    
    /*
    // 현재 경험치 (연습량) + 레벨업을 위한 max경험치 (연습량)
    if (isOpened)
    {
        sprintf(name, "%d", ms->GetExp());
        sprintf(name2, "/%d", SkillBuildUpInfo::GetMaxExp(ms->GetCommonId(), ms->GetLevel()));
    }
    else // '?'인 경우
    {
        */
        sprintf(name, "0");
        sprintf(name2, "/0");
    //}
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(name, fontList[2], 30, ccp(1, 0), ccp(300+412+10+33-3, 1117), ccc3(255,255,255), "", "T_SketchDetail", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(name2, fontList[2], 30, ccp(0, 0), ccp(300+412+10+33, 1112), ccc3(182,142,142), "", "T_SketchDetail", this, 5) );
    
    // dotted line
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_dotted_line.png", ccp(0, 0), ccp(105, 1070), CCSize(0, 0), "", "T_SketchDetail", this, 5) );
    
    /*
    if (isOpened)
    {
        MakeOpenedSkillSprites();
    }
    else
    {
        */
        MakeClosedSkillSprites();
    //}
    
    // 가격표 (스킬을 마스터했을 경우 표시하지 않는다)
    //if ( (isOpened && !SkillBuildUpInfo::IsMastered(ms->GetCommonId(), ms->GetLevel())) ||
    //    !isOpened )
    //{
        spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_degree_desc.png", ccp(0, 0), ccp(540, 688), CCSize(201, 77), "", "T_SketchDetail", this, 5) );
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_starcandy_mini.png", ccp(0, 0), ccp(550, 695), CCSize(0, 0), "", "T_SketchDetail", this, 5) );
        //if (isOpened)
        //    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(Common::MakeComma(SkillBuildUpInfo::GetCost(scid, ms->GetLevel()+1)), fontList[0], 36, ccp(0, 0), ccp(617, 708), ccc3(255,255,255), "", "T_SketchDetail", this, 5) );
        //else
        //    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(Common::MakeComma(SkillBuildUpInfo::GetCost(scid, 1)), fontList[0], 36, ccp(0, 0), ccp(617, 708), ccc3(255,255,255), "", "T_SketchDetail", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("0", fontList[0], 36, ccp(0, 0), ccp(617, 708), ccc3(255,255,255), "", "T_SketchDetail", this, 5) );
    //}
}

/*
void T_SketchDetail::MakeOpenedSkillSprites()
{
    int scid = skill_common_id;
    //SkillInfo* sInfo = SkillInfo::GetSkillInfo(scid);
    MySkill* ms = MySkill::GetObj(scid);
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_degree_desc.png", ccp(0, 0), ccp(120, 680), CCSize(390, 390), "", "T_SketchDetail", this, 4) );
    
    // 문구
    CCLayer* descLayer = CCLayer::create();
    descLayer->setAnchorPoint(ccp(0, 0));
    descLayer->setPosition(140, 700);
    this->addChild(descLayer, 5);
    spriteClass->layers.push_back(descLayer);
    
    
    // 스킬 설명
    std::string desc = SkillDescription(scid);
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabelArea(desc, fontList[0], 28, ccp(0, 1), ccp(540, 1070), ccc3(0,0,0), CCSize(420, 250), kCCTextAlignmentLeft, kCCVerticalTextAlignmentCenter, "", "T_SketchDetail", this, 5) );
    
    // 버튼
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_red_mini.png", ccp(0, 0), ccp(760, 673), CCSize(0, 0), "", "T_SketchDetail", this, 5, 0, 255, scid) );
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

std::string T_SketchDetail::SkillDescription(int scid)
{
    scid = 21;
    switch (scid)
    {
        case 21: return "붉은색 피스를 터뜨리면 추가점수를 얻어요.\n\n스킬 강화를 통해 추가점수의 양을 더 상승시킬 수 있어요."; break;
    }
    return "";
}
*/

void T_SketchDetail::MakeClosedSkillSprites()
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
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabelArea("아래의 요건을 충족하여 새로운 마법을 배울 준비가 다 되었어요. '배움' 버튼을 클릭해 보세요!", fontList[0], 28, ccp(0, 1), ccp(150, 1115), ccc3(0,0,0), CCSize(779, 180), kCCTextAlignmentLeft, kCCVerticalTextAlignmentCenter, "", "T_SketchDetail", this, 5) );
    }
    else
    {
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabelArea("아래의 요건을 모두 갖추면 이 마법을 배울 수 있어요. 조금만 더 힘내면 더 강해진 코코를 볼 수 있어요!", fontList[0], 28, ccp(0, 1), ccp(150, 1115), ccc3(0,0,0), CCSize(779, 180), kCCTextAlignmentLeft, kCCVerticalTextAlignmentCenter, "", "T_SketchDetail", this, 5) );
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
    //if (myInfo->GetMPTotal() >= sInfo->GetRequiredMP())
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_check.png1", ccp(0, 0), ccp(-70, -43), CCSize(0, 0), "", "Layer", descLayer, 5) );
    //if (MySkill::GetObj(sInfo->GetRequiredSkillId())->GetLevel() >= sInfo->GetRequiredSkillLv())
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_check.png1", ccp(0, 0), ccp(-70, -128), CCSize(0, 0), "", "Layer", descLayer, 5) );
    //if (myInfo->GetStaffLv() >= sInfo->GetRequiredStaffLv())
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_check.png1", ccp(0, 0), ccp(-70, -213), CCSize(0, 0), "", "Layer", descLayer, 5) );
    
    
    //SkillInfo* sInfoReq = SkillInfo::GetSkillInfo(sInfo->GetRequiredSkillId());
    SkillInfo* sInfoReq = SkillInfo::GetSkillInfo(skill_common_id-1);
    
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
    //sprintf(name, "0 이상");
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(name, fontList[0], 36, ccp(0, 1), ccp(160, 0), ccc3(0,0,0), "", "Layer", descLayer, 5) );
    sprintf(name, "Lv %d 이상 (%s)", sInfo->GetRequiredSkillLv(), sInfoReq->GetName().c_str());
    //sprintf(name, "Lv 1 이상 (%s)", sInfoReq->GetName().c_str());
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(name, fontList[0], 36, ccp(0, 1), ccp(160, -85), ccc3(0,0,0), "", "Layer", descLayer, 5) );
    sprintf(name, "지팡이 Lv %d 이상", sInfo->GetRequiredStaffLv());
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(name, fontList[0], 36, ccp(0, 1), ccp(65, -170), ccc3(0,0,0), "", "Layer", descLayer, 5) );
    
    
    
    // 버튼
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_red_mini.png", ccp(0, 0), ccp(760, 673), CCSize(0, 0), "", "T_SketchDetail", this, 5, 0, 255, scid) );
    pos = spriteClass->FindParentCenterPos("button/btn_red_mini.png");
    
    btnStatus = 3;
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_get.png", ccp(0.5, 0.5), ccp((int)pos.x, (int)pos.y+2), CCSize(0, 0), "button/btn_red_mini.png", "0", NULL, 5, 1) );
}


bool T_SketchDetail::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    if (isTouched)
        return false;
    isTouched = true;
    
    CCPoint point = pTouch->getLocation();
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        if (spriteClass->spriteObj[i]->name == "button/btn_red_mini.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                
                // 튜토리얼 상태 하나 넘기기
                CCString* param = CCString::create("2");
                CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetParentName(), param);
                

                std::vector<int> data;
                data.push_back(skill_common_id);
                Common::ShowPopup(this, "T_SketchDetail", "T_NoImage", true, NULL, BTN_1, data);
                
                break;
            }
        }
    }
    
    return true;
}


void T_SketchDetail::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
}

void T_SketchDetail::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    isTouched = false;
}

void T_SketchDetail::EndScene(bool isNoti)
{
    // remove this notification
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, Depth::GetCurName());
    // release depth tree
    Depth::RemoveCurDepth();
    
    ttrArrow->removeFromParentAndCleanup(true);
    ttrPos->removeFromParentAndCleanup(true);
    
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

