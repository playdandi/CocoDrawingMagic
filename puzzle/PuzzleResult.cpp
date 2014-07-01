#include "PuzzleResult.h"
#include "Puzzle.h"

using namespace cocos2d;

CCScene* PuzzleResult::scene()
{
    CCScene* pScene = CCScene::create();
    PuzzleResult* pLayer = PuzzleResult::create();
    pScene->addChild(pLayer);
    
	return pScene;
}

void PuzzleResult::onEnter()
{
    CCLog("PuzzleResult :: onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority(), true);
    CCLayer::onEnter();
    
    // sound
    //((Puzzle*)Depth::GetParentPointer())->GetSound()->PlayGameResult();
}
void PuzzleResult::onExit()
{
    CCLog("PuzzleResult :: onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
}

void PuzzleResult::keyBackClicked()
{
}


bool PuzzleResult::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
    // make depth tree
    Depth::AddCurDepth("PuzzleResult", this);
    
    this->setKeypadEnabled(true);
    this->setTouchEnabled(true);
    this->setTouchPriority(Depth::GetCurPriority());
    
    // notification observer
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(PuzzleResult::Notification), Depth::GetCurName(), NULL);
    
    // notification
    CCString* param = CCString::create("1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification("Puzzle", param);
    
    
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("images/game_result.plist");
    
    m_winSize = CCDirector::sharedDirector()->getWinSize();
    
    isTouched = false;
    
    spriteClass = new SpriteClass();
    spriteClassSkill = new SpriteClass();
    
    InitSprites();
    InitSkills();
    
    topaz = (float)(myInfo->GetTopaz() - myGameResult->getTopaz);
    starcandy = (float)(myInfo->GetStarCandy() - myGameResult->getStarCandy);
    mp = (float)(myInfo->GetMPTotal() - myGameResult->getMP);
    
    this->schedule(schedule_selector(PuzzleResult::ScoreTimer), 0.05f); // 점수 변동 타이머
    this->schedule(schedule_selector(PuzzleResult::TopazTimer), 0.05f);
    this->schedule(schedule_selector(PuzzleResult::StarCandyTimer), 0.05f);
    this->schedule(schedule_selector(PuzzleResult::MPTimer), 0.05f);
    
    return true;
}

void PuzzleResult::Notification(CCObject* obj)
{
    CCString* param = (CCString*)obj;
    
    if (param->intValue() <= 0)
    {
        // 터치 활성
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority()+1, true);
        this->setTouchPriority(Depth::GetCurPriority());
        isTouched = false;
        CCLog("PuzzleResult : 터치 활성 (Priority = %d)", this->getTouchPriority());
    }
    else if (param->intValue() == 1)
    {
        // 터치 비활성
        CCLog("PuzzleResult : 터치 비활성");
        CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
    }
    else if (param->intValue() == 10)
    {
        // 터치 풀기 (백그라운드에서 돌아올 때)
        isTouched = false;
    }
}

void PuzzleResult::InitSprites()
{
    pBlack = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, m_winSize.width, m_winSize.height));
    pBlack->setPosition(ccp(0, 0));
    pBlack->setAnchorPoint(ccp(0, 0));
    pBlack->setColor(ccc3(0, 0, 0));
    pBlack->setOpacity(150);
    this->addChild(pBlack, 1000);
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_topinfo.png1", ccp(0, 0), ccp(80, 1666), CCSize(230, 75), "", "PuzzleResult", this, 1001) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_topinfo.png2", ccp(0, 0), ccp(390, 1666), CCSize(290, 75), "", "PuzzleResult", this, 1001) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_topinfo.png3", ccp(0, 0), ccp(765, 1666), CCSize(290, 75), "", "PuzzleResult", this, 1001) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/result_topaz.png1", ccp(0, 0), ccp(15+10, 1656), CCSize(0, 0), "", "PuzzleResult", this, 1005) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/result_starcandy.png", ccp(0, 0), ccp(317, 1660), CCSize(0, 0), "", "PuzzleResult", this, 1005) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/result_magicpoint.png", ccp(0, 0), ccp(696, 1669), CCSize(0, 0), "", "PuzzleResult", this, 1005) );
    
    // topaz
    CCLog("%d %d %d", myInfo->GetTopaz(), myInfo->GetStarCandy(), myInfo->GetMPTotal());
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(Common::MakeComma(myInfo->GetTopaz()-myGameResult->getTopaz), fontList[0], 36, ccp(0.5, 0), ccp((80+230+80)/2, 1686), ccc3(255,255,255), "", "PuzzleResult", this, 1005, 0, 255, 1) );
    // starcandy
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(Common::MakeComma(myInfo->GetStarCandy()-myGameResult->getStarCandy), fontList[0], 36, ccp(0.5, 0), ccp((390+290+390)/2, 1686), ccc3(255,255,255), "", "PuzzleResult", this, 1005, 0, 255, 2) );
    // magic-point
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(Common::MakeComma(myInfo->GetMPTotal()-myGameResult->getMP), fontList[0], 36, ccp(0.5, 0), ccp((765+765+290)/2, 1686), ccc3(255,255,255), "", "PuzzleResult", this, 1005, 0, 255, 3) );
    
    int off = 50;
    
    // 배경
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png", ccp(0, 0), ccp(49, 458), CCSize(982, 954+off), "", "PuzzleResult", this, 1001) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_weekly_rank.png", ccp(0, 0), ccp(75, 492), CCSize(929, 904+off), "", "PuzzleResult", this, 1001) );
    
    // 스트랩
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_green.png", ccp(0, 0), ccp(14, 1343+off), CCSize(0, 0), "", "PuzzleResult", this, 1002) );
    //spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_title_gameresult.png", ccp(0, 0), ccp(409, 1389+off), CCSize(0, 0), "", "PuzzleResult", this, 1003) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_title_gameresult.png", ccp(0.5, 0), ccp(m_winSize.width/2, 1389+off), CCSize(0, 0), "", "PuzzleResult", this, 1003) );
    
    // 기록갱신 마크
    if (myGameResult->isNewRecord)
    {
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_newrecord.png", ccp(0.5, 0.5), ccp(320, 1389+90+off), CCSize(0, 0), "", "PuzzleResult", this, 1002) );
    }
    
    // 점수
    varScore = 0;
    
    char number[50];
    sprintf(number, "%d", varScore);
    pScoreLayer = Common::MakeImageNumberLayer(number, 2);
    pScoreLayer->setAnchorPoint(ccp(0, 0));
    pScoreLayer->setPosition(ccp(m_winSize.width/2 - pScoreLayer->getContentSize().width/2, 1230+off));
    this->addChild(pScoreLayer, 1005);
    
    
    // 최고점수
    sprintf(number, "최고점수 : %s", Common::MakeComma(myInfo->GetHighScore()).c_str());
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(number, fontList[0], 36, ccp(0.5, 0.5), ccp(m_winSize.width/2, 1190+off), ccc3(78,47,8), "", "PuzzleResult", this, 1005) );
    // dotted-line
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_dotted_line.png", ccp(0.5, 0.5), ccp(m_winSize.width/2, 1150+off), CCSize(0, 0), "", "PuzzleResult", this, 1005) );
    
    // 기본점수 + 추가점수 + 배경
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_dontknow_1.png", ccp(0.5, 0), ccp(m_winSize.width/2, 975), CCSize(800, 155+off), "", "PuzzleResult", this, 1002) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("기본점수 :", fontList[2], 40, ccp(1, 0.5), ccp(460, 1083+off), ccc3(121,71,0), "", "PuzzleResult", this, 1005) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("기본점수 :", fontList[2], 40, ccp(1, 0.5), ccp(460, 1083+3+off), ccc3(255,219,53), "", "PuzzleResult", this, 1005) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("MP 추가점수 :", fontList[2], 40, ccp(1, 0.5), ccp(460, 1013+off), ccc3(121,71,0), "", "PuzzleResult", this, 1005) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("MP 추가점수 :", fontList[2], 40, ccp(1, 0.5), ccp(460, 1013+3+off), ccc3(255,219,53), "", "PuzzleResult", this, 1005) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("MAX 콤보 :", fontList[2], 36, ccp(1, 0.5), ccp(460, 953+off), ccc3(121,71,0), "", "PuzzleResult", this, 1005) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("MAX 콤보 :", fontList[2], 36, ccp(1, 0.5), ccp(460, 953+3+off), ccc3(255,219,53), "", "PuzzleResult", this, 1005) );
    
    // 기본점수 값
    sprintf(number, "%s", Common::MakeComma(myGameResult->score).c_str());
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(number, fontList[0], 52, ccp(0, 0.5), ccp(500+2, 1088+off-2), ccc3(0,0,0), "", "PuzzleResult", this, 1005) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(number, fontList[0], 52, ccp(0, 0.5), ccp(500, 1088+off), ccc3(255,255,255), "", "PuzzleResult", this, 1005) );
    // MP 추가점수 값
    sprintf(number, "+ %s", Common::MakeComma(myGameResult->totalScore - myGameResult->score).c_str());
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(number, fontList[0], 48, ccp(0, 0.5), ccp(500+2, 1018+off-2), ccc3(0,0,0), "", "PuzzleResult", this, 1005) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(number, fontList[0], 48, ccp(0, 0.5), ccp(500, 1018+off), ccc3(255,255,255), "", "PuzzleResult", this, 1005) );
    // 콤보 값
    sprintf(number, "%d", myGameResult->combo);
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(number, fontList[0], 42, ccp(0, 0.5), ccp(500+2, 958+off-2), ccc3(0,0,0), "", "PuzzleResult", this, 1005) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(number, fontList[0], 42, ccp(0, 0.5), ccp(500, 958+off), ccc3(255,255,255), "", "PuzzleResult", this, 1005) );
 
    
    int offset;
    
    // 별사탕, MP, 포션, 토파즈 (새로 받은) 정보 위치
    // 원래 size (430, 150)
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_degree_desc.png", ccp(0, 0), ccp(140, 800), CCSize(330, 150), "", "PuzzleResult", this, 1005) );
    
    // 별사탕, MP 아이콘
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/result_starcandy_2.png", ccp(0, 0), ccp(160, 870), CCSize(0,0), "", "PuzzleResult", this, 1005) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_magicpoint_mini.png", ccp(0, 0), ccp(163, 810), CCSize(0,0), "", "PuzzleResult", this, 1005) );
    ((CCSprite*)spriteClass->FindSpriteByName("icon/icon_magicpoint_mini.png"))->setScale(1.1f);
    
    // 별사탕 개수
    sprintf(number, "+ %s", Common::MakeComma(myGameResult->getStarCandy).c_str());
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(number, fontList[0], 36, ccp(0, 0.5), ccp(265+2, 907-2), ccc3(0,0,0), "", "PuzzleResult", this, 1005) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(number, fontList[0], 36, ccp(0, 0.5), ccp(265, 907), ccc3(255,255,255), "", "PuzzleResult", this, 1005) );
    // 추가 MP 수
    sprintf(number, "+ %s", Common::MakeComma(myGameResult->getMP).c_str());
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(number, fontList[0], 36, ccp(0, 0.5), ccp(265+2, 840-2), ccc3(0,0,0), "", "PuzzleResult", this, 1005) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(number, fontList[0], 36, ccp(0, 0.5), ccp(265, 840), ccc3(255,255,255), "", "PuzzleResult", this, 1005) );
    
    if (myGameResult->getPotion > 0)
    {
        // 포션 빈칸
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_potion_empty.png", ccp(0, 0), ccp(380, 825), CCSize(0,0), "", "PuzzleResult", this, 1005) );
        // 포션
        CCPoint p = spriteClass->FindParentCenterPos("icon/icon_potion_empty.png");
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_potion.png", ccp(0.5, 0.5), p, CCSize(0,0), "icon/icon_potion_empty.png", "0", NULL, 1005, 1) );
    }
    
     /*
    // 토파즈 빈칸
    // (차후 받아서 쓰자)
    // 토파즈
    if (myGameResult->getTopaz > 0)
    {
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/result_topaz.png2", ccp(0, 0), ccp(465, 825-10), CCSize(0,0), "", "PuzzleResult", this, 1006) );
    }
    */
    
    // 스킬 문양들 쏟아져 나오는 부분
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_degree_desc.png", ccp(0, 0), ccp(590-100, 800), CCSize(350+100, 150), "", "PuzzleResult", this, 1005) );
    
    
    
    // 말풍선
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_speech_balloon.png", ccp(0, 0), ccp(150, 680-5), CCSize(550, 80), "", "PuzzleResult", this, 1005) );
    CCPoint p = spriteClass->FindParentCenterPos("background/bg_speech_balloon.png");
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("와~ 오늘은 연습이 정말 잘 되는구나!", fontList[2], 30, ccp(0.5, 0.5), ccp(p.x, p.y), ccc3(0,0,0), "background/bg_speech_balloon.png", "1", NULL, 1005, 1) );
    // 말풍선 움직이는 액션
    CCActionInterval* action = CCSequence::create(CCMoveBy::create(1.0f, ccp(-5, 0)), CCMoveBy::create(1.0f, ccp(5, 0)), NULL);
    CCActionInterval* rep = CCRepeatForever::create(action);
    ((CCSprite*)spriteClass->FindSpriteByName("background/bg_speech_balloon.png"))->runAction(rep);
    
    
    char name[50];
    // 스킬 문양 + 그 배경
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_skill_brown.png", ccp(0, 0), ccp(125, 525), CCSize(0, 0), "", "PuzzleResult", this, 1005) );
    
    if (myInfo->GetPracticeSkillId() != 0) // 연습 중인 스킬이 있을 때만 노출되는 것들
    {
        sprintf(name, "skill_%d.png", myInfo->GetPracticeSkillId());
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(125+8, 525+8), CCSize(0, 0), "", "PuzzleResult", this, 1005) );
    
        // skill level (ex : Lv.2)
        offset = 0;
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "number/level_lv.png", ccp(0, 0), ccp(125+38, 525-10), CCSize(0,0), "", "PuzzleResult", this, 1005) );
        if (myInfo->GetPracticeSkillLv() >= 10)
        {
            sprintf(name, "number/level_%d.png", myInfo->GetPracticeSkillLv() / 10);
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp((125+38)+43+3, 525-10), CCSize(0,0), "", "PuzzleResult", this, 1005) );
            offset = spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->getContentSize().width;
        }
        sprintf(name, "number/level_%d.png", myInfo->GetPracticeSkillLv() % 10);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp((125+38)+43+3+offset, 525-8), CCSize(0,0), "", "PuzzleResult", this, 1005) );
        
        // 미션 내용
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(Common::GetMissionContent(missionType, missionVal, missionRefVal), fontList[0], 27, ccp(0, 0), ccp(125+146+10, 525+30+36), ccc3(0,0,0), "", "PuzzleResult", this, 1005) );
        
        // 미션 성공/실패 아이콘 노출
        if (myGameResult->isMissionSuccess)
        {
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/mission_success.png", ccp(0.5,0.5), ccp(125+5, 525+146-30+30), CCSize(0,0), "", "PuzzleResult", this, 1005) );
            ((CCSprite*)spriteClass->FindSpriteByName("icon/mission_success.png"))->setRotation(-20);
        }
        else
        {
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/mission_fail.png", ccp(0.5,0.5), ccp(125+40, 525+146-20), CCSize(0,0), "", "PuzzleResult", this, 1010) );
            ((CCSprite*)spriteClass->FindSpriteByName("icon/mission_fail.png"))->setRotation(-10);
        }
        
        // 연습량 프로그레스바 안의 노란 바
        barLayer = CCLayer::create();
        CCSprite* bar = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, 412+10-6, 31-6));
        bar->setAnchorPoint(ccp(1, 0));
        bar->setPosition(ccp(125+146+10 +3, 525+30 +3));
        bar->setColor(ccc3(255,219,53));
        barLayer->addChild(bar, 10);
        
        timerStencil2 = CCDrawNode::create();
        CCPoint ver[] = { ccp(125+146+10+3, 525+30), ccp(125+146+10+3+412+10-6, 525+30), ccp(125+146+10+3+412+10-6, 525+30+31), ccp(125+146+10+3, 525+30+31) };
        timerStencil2->drawPolygon(ver, 4, ccc4f(0,0,0,255), 0, ccc4f(0,0,0,255));
        timerClip2 = CCClippingNode::create(timerStencil2);
        timerClip2->addChild(barLayer);
        this->addChild(timerClip2, 1010);
        
        // bar 크기 계산
        MySkill* ms = MySkill::GetObj(myInfo->GetPracticeSkillId());
        float d = (float)(412+10-6) * ((float)ms->GetExp() / (float)SkillBuildUpInfo::GetMaxExp(ms->GetCommonId(), ms->GetLevel()));
        int isFullExp = (ms->GetExp() == SkillBuildUpInfo::GetMaxExp(ms->GetCommonId(), ms->GetLevel()));
        
        CCActionInterval* action2 = CCSequence::create(CCMoveBy::create(1.5f, ccp(d, 0)), CCCallFuncND::create(this, callfuncND_selector(PuzzleResult::Callback_ProgressBar), (void*)isFullExp), NULL);
        bar->runAction(action2);
    }
    
    // levelup icon (투명하게 해 놓고, 프로그레스바 진행 후, 만렙이 되었다면 나타나게 한다)
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_levelup.png", ccp(1, 0), ccp(125+146+10+412-20+30+30+5, 525+30+31-15), CCSize(0,0), "", "PuzzleResult", this, 1006, 0, 0) );
    
    // 연습량 프로그레스바
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_progress_bar.png", ccp(0, 0), ccp(125+146+10, 525+30), CCSize(412+10, 31), "", "PuzzleResult", this, 1005) );
    
    
    // 현재 경험치 (연습량) + 레벨업을 위한 max경험치 (연습량)
    MySkill* ms = MySkill::GetObj(myInfo->GetPracticeSkillId());
    if (myInfo->GetPracticeSkillId() != 0)
        sprintf(name, "/ %d", SkillBuildUpInfo::GetMaxExp(ms->GetCommonId(), ms->GetLevel()));
    else
        sprintf(name, "/ 0");
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(name, fontList[0], 30, ccp(1, 1), ccp(673+30, 525+30-11), ccc3(182,142,142), "", "PuzzleResult", this, 1005, 0, 255, 100) );
    CCSize s = ((CCLabelTTF*)spriteClass->FindLabelByTag(100))->getContentSize();
    
    if (myInfo->GetPracticeSkillId() != 0)
        sprintf(name, "%d", ms->GetExp());
    else
        sprintf(name, "0");
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(name, fontList[0], 30, ccp(1, 1), ccp(673+30-s.width-5, 525+30-5), ccc3(121,71,0), "", "PuzzleResult", this, 1005) );

    
    // 스킬 이름
    if (myInfo->GetPracticeSkillId() != 0)
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(SkillInfo::GetSkillInfo(myInfo->GetPracticeSkillId())->GetName(), fontList[0], 27, ccp(0, 0), ccp(125+146+10, 525+30-38), ccc3(0,0,0), "", "PuzzleResult", this, 1005) );
    else
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("연습 스킬 없음", fontList[0], 27, ccp(0, 0), ccp(125+146+10, 525+30-38), ccc3(0,0,0), "", "PuzzleResult", this, 1005) );
    
    // 코코
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "image/coco.png", ccp(0.5, 0.5), ccp(850, 645), CCSize(0,0), "", "PuzzleResult", this, 1005) );
    ((CCSprite*)spriteClass->FindSpriteByName("image/coco.png"))->setScale(1.7f);
    ((CCSprite*)spriteClass->FindSpriteByName("image/coco.png"))->setFlipX(true);
    
    // 확인 버튼
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_green.png", ccp(0.5, 0.5), ccp(m_winSize.width/2, 300), CCSize(0, 0), "", "PuzzleResult", this, 1005) );
    CCPoint pos = spriteClass->FindParentCenterPos("button/btn_green.png");
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_confirm_green.png", ccp(0.5, 0.5), ccp(pos.x, pos.y+5), CCSize(0, 0), "button/btn_green.png", "0", this, 1005, 1) );
    

    // 게임시작 버튼 움직이기
    CCSprite* temp = ((CCSprite*)spriteClass->FindSpriteByName("button/btn_green.png"));
    CCActionInterval* action2 = CCSequence::create( CCScaleTo::create(1.0f, 1.02f, 0.97f), CCScaleTo::create(1.0f, 0.98f, 1.03f), NULL );
    temp->runAction(CCRepeatForever::create(action2));
    ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_confirm_green.png"))->runAction(CCRepeatForever::create((CCActionInterval*)action->copy()));
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
}

void PuzzleResult::Callback_ProgressBar(CCNode* sender, void* data)
{
    if ((int)data == 1)
    {
        ((CCSprite*)spriteClass->FindSpriteByName("icon/icon_levelup.png"))->setOpacity(255);
    
        CCActionInterval* action = CCSequence::create(CCMoveBy::create(0.7f, ccp(0, -5)), CCMoveBy::create(0.7f, ccp(0, 5)), NULL);
        CCActionInterval* rep = CCRepeatForever::create(action);
        ((CCSprite*)spriteClass->FindSpriteByName("icon/icon_levelup.png"))->runAction(rep);
    }
}

void PuzzleResult::InitSkills()
{
    int numOfList = myGameResult->skillNum.size();
    CCLayer* container = CCLayer::create();
    
    char name[20], name2[20];
    int width = 0;
    for (int i = 0 ; i < numOfList ; i++)
    {
        sprintf(name, "skill_%d.png", myGameResult->skillNum[i]);
        spriteClassSkill->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(width, 0), CCSize(0,0), "", "Layer", container, 1007, 0, 0, i+10000) );
        //((CCSprite*)spriteClassSkill->FindSpriteByName(name))->setOpacity(0);
        
        sprintf(name2, "X %d", myGameResult->skillCnt[i]);
        spriteClassSkill->spriteObj.push_back( SpriteObject::CreateLabel(name2, fontList[0], 32, ccp(0.5, 0.5), ccp(width+146/2-15, 15), ccc3(255,255,255), "", "Layer", container, 1007, 0, 0, i+20000) );
        
        width = width + ((CCSprite*)spriteClassSkill->FindSpriteByName(name))->getContentSize().width - 20;
    }
    
    for (int i = 0 ; i < spriteClassSkill->spriteObj.size() ; i++)
        spriteClassSkill->AddChild(i);
    
    width += 20;
    container->setContentSize(CCSizeMake(width, 150));
    
    scrollView = CCScrollView::create();
    scrollView->setDirection(kCCScrollViewDirectionHorizontal);
    scrollView->setViewSize(CCSizeMake(100+350-30, 150-20));
    scrollView->setAnchorPoint(ccp(0, 0));
    scrollView->setPosition(ccp(590-100+15, 800+10));
    scrollView->setDelegate(this);
    scrollView->setContainer(container);
    scrollView->setContentSize(container->getContentSize());
    scrollView->setTouchPriority(Depth::GetCurPriority());
    this->addChild(scrollView, 1006);

    skillIdx = 0;
    this->schedule(schedule_selector(PuzzleResult::SkillTimer), 0.33f);
}

void PuzzleResult::SkillTimer(float f)
{
    /*
    char number[30];
    sprintf(number, "skill_%d.png", test[ppp]);
    CCActionInterval* action = CCSequence::create(CCJumpBy::create(1.5f, ccp(-(350+50), 0), 80, 3), CCCallFuncND::create(this, callfuncND_selector(PuzzleResult::Callback), NULL), NULL);
    ((CCSprite*)spriteClassSkill->FindSpriteByName(number))->runAction(action);
    
    ppp++;
    */
    CCLog("%d %d", skillIdx, (int)myGameResult->skillNum.size());
    if (skillIdx < myGameResult->skillNum.size())
    {
        if (skillIdx == 3) // 4개째 만들어질 때 스크롤링 시작
        {
            CCLog("min offset = %d", (int)scrollView->minContainerOffset().x);
            float d = (float)abs((int)scrollView->minContainerOffset().x) / (float)146 / 2;
            CCLog("time = %f", d);
            scrollView->setContentOffsetInDuration(ccp(scrollView->minContainerOffset().x, 0), d);
        }
        CCActionInterval* action = CCFadeIn::create(0.33f);
        ((CCSprite*)spriteClassSkill->FindSpriteByTag(skillIdx+10000))->runAction(action);
        ((CCLabelTTF*)spriteClassSkill->FindLabelByTag(skillIdx+20000))->runAction((CCActionInterval*)action->copy());
        //spriteClassSkill->spriteObj[skillIdx]->sprite->runAction(action);
        skillIdx++;
    }
    else
    {
        this->unschedule(schedule_selector(PuzzleResult::SkillTimer));
    }
}


/*
int test[10] = {11,21,31,15,25,35,23,33,27,28};
int ppp;
void PuzzleResult::InitSkills()
{
    char number[50];
    l = CCLayer::create();
    ppp = 0;
    int k;
    for (int i = 0 ; i < 10 ; i++)
    {
        sprintf(number, "skill_%d.png", test[i]);
        k = (i % 2 == 0) ? 0 : 0;
        spriteClassSkill->spriteObj.push_back( SpriteObject::Create(0, number, ccp(0, 0), ccp(140+430+20+350+50+30*i, 810+k), CCSize(350, 150), "", "Layer", l, 1006) );
        ((CCSprite*)spriteClassSkill->FindSpriteByName(number))->setScale(0.6f);
    }
    for (int i = 0 ; i < spriteClassSkill->spriteObj.size() ; i++)
        spriteClassSkill->AddChild(i);
    
    timerStencil = CCDrawNode::create();
    CCPoint ver[] = { ccp(590, 800), ccp(590, 800+150), ccp(590+350, 800+150), ccp(590+350, 800) };
    timerStencil->drawPolygon(ver, 4, ccc4f(0,0,0,255), 0, ccc4f(0,0,0,255));
    timerClip = CCClippingNode::create(timerStencil);
    timerClip->addChild(l);
    this->addChild(timerClip, 1010);
    
    this->schedule(schedule_selector(PuzzleResult::SkillTimer), 0.2f, 9, 0);
}
void PuzzleResult::Callback(CCNode* sender, void* p)
{
    CCActionInterval* action = CCSequence::create(CCMoveBy::create(0.5f, ccp(0, 50)), CCMoveBy::create(0.5f, ccp(0, -50)), NULL);
    CCActionInterval* rep = CCRepeatForever::create(action);
    sender->runAction(rep);
}
*/

void PuzzleResult::ScoreTimer(float f)
{
    varScore += (myGameResult->totalScore / 20);
    if (varScore > myGameResult->totalScore)
    {
        this->unschedule(schedule_selector(PuzzleResult::ScoreTimer));
        varScore = myGameResult->totalScore;
    }
    
    pScoreLayer->removeAllChildren();
    pScoreLayer->removeFromParentAndCleanup(true);
    
    char number[50];
    sprintf(number, "%d", varScore);
    pScoreLayer = Common::MakeImageNumberLayer(number, 2);
    pScoreLayer->setAnchorPoint(ccp(0, 0));
    pScoreLayer->setPosition(ccp(m_winSize.width/2 - pScoreLayer->getContentSize().width/2, 1230+50));
    this->addChild(pScoreLayer, 1005);
}
void PuzzleResult::TopazTimer(float f)
{
    topaz += ((float)(myGameResult->getTopaz) / 20.0f);
    if ((int)topaz >= myInfo->GetTopaz())
    {
        this->unschedule(schedule_selector(PuzzleResult::TopazTimer));
        topaz = (float)myInfo->GetTopaz();
    }
    
    ((CCLabelTTF*)spriteClass->FindLabelByTag(1))->setString(Common::MakeComma((int)topaz).c_str());
}
void PuzzleResult::StarCandyTimer(float f)
{
    starcandy += ((float)(myGameResult->getStarCandy) / 20.0f);
    if ((int)starcandy >= myInfo->GetStarCandy())
    {
        this->unschedule(schedule_selector(PuzzleResult::StarCandyTimer));
        starcandy = (float)myInfo->GetStarCandy();
    }
    
    ((CCLabelTTF*)spriteClass->FindLabelByTag(2))->setString(Common::MakeComma((int)starcandy).c_str());
}
void PuzzleResult::MPTimer(float f)
{
    mp += ((float)(myGameResult->getMP) / 20.0f);
    if ((int)mp >= myInfo->GetMPTotal())
    {
        this->unschedule(schedule_selector(PuzzleResult::MPTimer));
        mp = (float)myInfo->GetMPTotal();
    }
    
    ((CCLabelTTF*)spriteClass->FindLabelByTag(3))->setString(Common::MakeComma((int)mp).c_str());
}




bool PuzzleResult::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    CCPoint point = pTouch->getLocation();
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        if (spriteClass->spriteObj[i]->name == "button/btn_green.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                spriteClass->spriteObj[i]->sprite->setColor(ccc3(170,170,170));
                ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_confirm_green.png"))->setColor(ccc3(170,170,170));
                return true;
            }
        }
    }
    return true;
}


void PuzzleResult::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
}

void PuzzleResult::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    CCPoint point = pTouch->getLocation();
    
    ((CCSprite*)spriteClass->FindSpriteByName("button/btn_green.png"))->setColor(ccc3(255,255,255));
    ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_confirm_green.png"))->setColor(ccc3(255,255,255));
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        if (spriteClass->spriteObj[i]->name == "button/btn_green.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                // 화면 어둡게 하고, PuzzleResult 팝업창 끄고, Puzzle->Ranking으로 돌아가자.
                pBlackClose = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, m_winSize.width, m_winSize.height));
                pBlackClose->setPosition(ccp(0, 0));
                pBlackClose->setAnchorPoint(ccp(0, 0));
                pBlackClose->setColor(ccc3(0, 0, 0));
                pBlackClose->setOpacity(0);
                this->addChild(pBlackClose, 7000);
                
                CCActionInterval* action = CCSequence::create( CCFadeIn::create(1.5f), CCCallFuncND::create(this, callfuncND_selector(PuzzleResult::EndSceneCallback), this), NULL);
                pBlackClose->runAction(action);
                break;
            }
        }
    }
}

void PuzzleResult::EndScene()
{
}

void PuzzleResult::EndSceneCallback(CCNode* sender, void* pointer)
{
    PuzzleResult* pThis = (PuzzleResult*)pointer;
    
    CCTextureCache::sharedTextureCache()->removeTextureForKey("images/ranking_scrollbg.png");
    CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("images/game_result.plist");
    CCTextureCache::sharedTextureCache()->removeTextureForKey("images/game_result.png");
    
    // remove this notification
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, Depth::GetCurName());
    // release depth tree
    Depth::RemoveCurDepth();
    
    // 이걸 끝내면서, Puzzle에게도 끝내고 Ranking으로 돌아가라고 알려준다.
    CCString* param = CCString::create("2");
    CCNotificationCenter::sharedNotificationCenter()->postNotification("Puzzle", param);
    
    // remove all objects
    pThis->spriteClass->RemoveAllObjects();
    delete pThis->spriteClass;
    pThis->spriteClassSkill->RemoveAllObjects();
    delete pThis->spriteClassSkill;
    
    //pThis->timerStencil->removeFromParentAndCleanup(true);
    //pThis->timerClip->removeFromParentAndCleanup(true);
    if (myInfo->GetPracticeSkillId() != 0) // 연습 중인 스킬이 있을 때만 노출되는 것들
    {
        pThis->barLayer->removeAllChildren();
        pThis->barLayer->removeFromParentAndCleanup(true);
        pThis->timerStencil2->removeFromParentAndCleanup(true);
        pThis->timerClip2->removeFromParentAndCleanup(true);
    }
    
    pThis->pBlack->removeFromParentAndCleanup(true);
    pThis->pBlackClose->removeFromParentAndCleanup(true);
    
    pThis->scrollView->getContainer()->removeAllChildren();
    pThis->scrollView->removeAllChildren();
    pThis->scrollView->removeFromParentAndCleanup(true);
    
    pThis->removeFromParentAndCleanup(true);
}

void PuzzleResult::scrollViewDidScroll(CCScrollView* view)
{
    isScrolling = true;
}

void PuzzleResult::scrollViewDidZoom(CCScrollView* view)
{
}