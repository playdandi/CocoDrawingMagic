#include "PuzzleResult.h"

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
    
    // notification
    CCString* param = CCString::create("1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification("Puzzle", param);
    
    
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("images/game_result.plist");
    
    m_winSize = CCDirector::sharedDirector()->getWinSize();
    
    spriteClass = new SpriteClass();
    spriteClassSkill = new SpriteClass();
    
    InitSprites();
    InitSkills();
    
    this->schedule(schedule_selector(PuzzleResult::ScoreTimer), 0.05f); // 점수 변동 타이머
    this->schedule(schedule_selector(PuzzleResult::TopazTimer), 0.05f);
    this->schedule(schedule_selector(PuzzleResult::StarCandyTimer), 0.05f);
    this->schedule(schedule_selector(PuzzleResult::MPTimer), 0.05f);
    
    return true;
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
    
    topaz = (float)prevTopaz;
    starcandy = (float)prevStarCandy;
    mp = (float)prevMP;
    
    // topaz
    CCLog("%d %d %d", myInfo->GetTopaz(), myInfo->GetStarCandy(), myInfo->GetMPTotal());
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(Common::MakeComma((int)topaz), fontList[0], 36, ccp(0.5, 0), ccp((80+230+80)/2, 1686), ccc3(255,255,255), "", "PuzzleResult", this, 1005, 0, 255, 1) );
    // starcandy
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(Common::MakeComma((int)starcandy), fontList[0], 36, ccp(0.5, 0), ccp((390+290+390)/2, 1686), ccc3(255,255,255), "", "PuzzleResult", this, 1005, 0, 255, 2) );
    // magic-point
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(Common::MakeComma((int)mp), fontList[0], 36, ccp(0.5, 0), ccp((765+765+290)/2, 1686), ccc3(255,255,255), "", "PuzzleResult", this, 1005, 0, 255, 3) );
    
    int off = 50;
    
    // 배경
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png", ccp(0, 0), ccp(49, 458), CCSize(982, 954+off), "", "PuzzleResult", this, 1001) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_weekly_rank.png", ccp(0, 0), ccp(75, 492), CCSize(929, 904+off), "", "PuzzleResult", this, 1001) );
    
    // 스트랩
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_green.png", ccp(0, 0), ccp(14, 1343+off), CCSize(0, 0), "", "PuzzleResult", this, 1002) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_title_gameready.png", ccp(0, 0), ccp(409, 1389+off), CCSize(0, 0), "", "PuzzleResult", this, 1002) );
    
    // 기록갱신 마크
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_up.png", ccp(0.5, 0.5), ccp(220, 1370+off), CCSize(0, 0), "", "PuzzleResult", this, 1002) );
    ((CCSprite*)spriteClass->FindSpriteByName("icon/icon_up.png"))->setScale(1.2f);
    
    // 점수
    score = 13059823;
    //score = 391108;
    varScore = 0;
    
    char number[50];
    sprintf(number, "%d", varScore);
    pScoreLayer = Common::MakeImageNumberLayer(number, 2);
    pScoreLayer->setAnchorPoint(ccp(0, 0));
    pScoreLayer->setPosition(ccp(m_winSize.width/2 - pScoreLayer->getContentSize().width/2, 1230+off));
    this->addChild(pScoreLayer, 1005);
    
    
    // 최고점수
    sprintf(number, "최고점수 : %s", Common::MakeComma(8398504).c_str());
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
    sprintf(number, "%s", Common::MakeComma(7980141).c_str());
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(number, fontList[0], 52, ccp(0, 0.5), ccp(500, 1088+off), ccc3(255,255,255), "", "PuzzleResult", this, 1005) );
    // MP 추가점수 값
    sprintf(number, "+ %d%%", 12);
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(number, fontList[0], 48, ccp(0, 0.5), ccp(500, 1018+off), ccc3(255,255,255), "", "PuzzleResult", this, 1005) );
    // 콤보 값
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("103", fontList[0], 42, ccp(0, 0.5), ccp(500, 958+off), ccc3(255,255,255), "", "PuzzleResult", this, 1005) );
    
    /*
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_cocoroom_desc.png", ccp(0, 0), ccp(524, 800), CCSize(350, 100), "", "PuzzleResult", this, 1002) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_degree_desc.png", ccp(0, 0), ccp(524, 920), CCSize(350, 100), "", "PuzzleResult", this, 1002) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_dontknow_1.png", ccp(0, 0), ccp(524, 1040), CCSize(350, 100), "", "PuzzleResult", this, 1002) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_dontknow_2.png", ccp(0, 0), ccp(100, 500), CCSize(350, 100), "", "PuzzleResult", this, 1002) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_my_profile.png", ccp(0, 0), ccp(100, 620), CCSize(350, 100), "", "PuzzleResult", this, 1002) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_gameready_name.png", ccp(0, 0), ccp(100, 740), CCSize(200, 100), "", "PuzzleResult", this, 1002) );
    */
    
    int offset;
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_degree_desc.png", ccp(0, 0), ccp(140, 800), CCSize(430, 150), "", "PuzzleResult", this, 1005) );
    // 별사탕, MP 아이콘
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/result_starcandy_2.png", ccp(0, 0), ccp(160, 870), CCSize(0,0), "", "PuzzleResult", this, 1005) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_magicpoint_mini.png", ccp(0, 0), ccp(163, 810), CCSize(0,0), "", "PuzzleResult", this, 1005) );
    ((CCSprite*)spriteClass->FindSpriteByName("icon/icon_magicpoint_mini.png"))->setScale(1.1f);
    
    // 별사탕 개수
    sprintf(number, "+ %s", Common::MakeComma(1102).c_str());
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(number, fontList[0], 36, ccp(0, 0.5), ccp(265, 907), ccc3(255,255,255), "", "PuzzleResult", this, 1005) );
    // 추가 MP 수
    sprintf(number, "+ %s", Common::MakeComma(16).c_str());
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(number, fontList[0], 36, ccp(0, 0.5), ccp(265, 840), ccc3(255,255,255), "", "PuzzleResult", this, 1005) );
    
    // 포션 빈칸
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_potion_empty.png", ccp(0, 0), ccp(430, 825+7), CCSize(0,0), "", "PuzzleResult", this, 1005) );
    // 포션
    CCPoint p = spriteClass->FindParentCenterPos("icon/icon_potion_empty.png");
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_potion.png", ccp(0.5, 0.5), p, CCSize(0,0), "icon/icon_potion_empty.png", "0", NULL, 1005, 1) );
    // 토파즈 빈칸
    // (차후 받아서 쓰자)
    // 토파즈
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/result_topaz.png2", ccp(0, 0), ccp(465, 825-10), CCSize(0,0), "", "PuzzleResult", this, 1006) );
    
    
    // 스킬 문양들 쏟아져 나오는 부분
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_degree_desc.png", ccp(0, 0), ccp(140+430+20, 800), CCSize(350, 150), "", "PuzzleResult", this, 1005) );
    
    
    // 말풍선
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_speech_balloon.png", ccp(0, 0), ccp(150, 680-5), CCSize(550, 80), "", "PuzzleResult", this, 1005) );
    p = spriteClass->FindParentCenterPos("background/bg_speech_balloon.png");
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("와~ 오늘은 연습이 정말 잘 되는구나!", fontList[2], 30, ccp(0.5, 0.5), ccp(p.x, p.y), ccc3(0,0,0), "background/bg_speech_balloon.png", "1", NULL, 1005, 1) );
    // 말풍선 움직이는 액션
    CCActionInterval* action = CCSequence::create(CCMoveBy::create(1.0f, ccp(-5, 0)), CCMoveBy::create(1.0f, ccp(5, 0)), NULL);
    CCActionInterval* rep = CCRepeatForever::create(action);
    ((CCSprite*)spriteClass->FindSpriteByName("background/bg_speech_balloon.png"))->runAction(rep);
    
    
    char name[50];
    // 스킬 문양 + 그 배경
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_skill_brown.png", ccp(0, 0), ccp(125, 525), CCSize(0, 0), "", "PuzzleResult", this, 1005) );
    sprintf(name, "skill_%d.png", myInfo->GetPracticeSkillId());
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(125+8, 525+8), CCSize(0, 0), "", "PuzzleResult", this, 1005) );
    
    // 25, 51 base
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
    
    // 스킬 이름
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(SkillInfo::GetSkillInfo(myInfo->GetPracticeSkillId())->GetName(), fontList[0], 30, ccp(0, 0), ccp(125+146+10, 525+30+31+5), ccc3(0,0,0), "", "PuzzleResult", this, 1005) );
    
    // 연습량 프로그레스바
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_progress_bar.png", ccp(0, 0), ccp(125+146+10, 525+30), CCSize(412-20, 31), "", "PuzzleResult", this, 1005) );
    
    
    barLayer = CCLayer::create();
    CCSprite* bar = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, 412-20-6, 31-6));
    bar->setAnchorPoint(ccp(1, 0));
    bar->setPosition(ccp(125+146+10 +3, 525+30 +3));
    bar->setColor(ccc3(255,219,53));
    barLayer->addChild(bar, 10);
    
    timerStencil2 = CCDrawNode::create();
    CCPoint ver[] = { ccp(125+146+10+3, 525+30), ccp(125+146+10+3+412-20-6, 525+30), ccp(125+146+10+3+412-20-6, 525+30+31), ccp(125+146+10+3, 525+30+31) };
    timerStencil2->drawPolygon(ver, 4, ccc4f(0,0,0,255), 0, ccc4f(0,0,0,255));
    timerClip2 = CCClippingNode::create(timerStencil2);
    timerClip2->addChild(barLayer);
    this->addChild(timerClip2, 1010);
    
    // 현재 경험치 (연습량) + 레벨업을 위한 max경험치 (연습량)
    MySkill* ms = MySkill::GetObj(myInfo->GetPracticeSkillId());
    sprintf(name, "/%d", SkillBuildUpInfo::GetMaxExp(ms->GetCommonId(), ms->GetLevel()));
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(name, fontList[0], 30, ccp(1, 1), ccp(673, 525+30-11), ccc3(182,142,142), "", "PuzzleResult", this, 1005, 0, 255, 100) );
    CCSize s = ((CCLabelTTF*)spriteClass->FindLabelByTag(100))->getContentSize();
    
    sprintf(name, "%d", ms->GetExp());
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(name, fontList[0], 30, ccp(1, 1), ccp(673-s.width-5, 525+30-5), ccc3(121,71,0), "", "PuzzleResult", this, 1005) );
    
    float d = (float)(412-20-6) * ((float)(ms->GetExp()+7) / (float)SkillBuildUpInfo::GetMaxExp(ms->GetCommonId(), ms->GetLevel()));
    int isFullExp = (ms->GetExp() == SkillBuildUpInfo::GetMaxExp(ms->GetCommonId(), ms->GetLevel()));
    
    CCActionInterval* action2 = CCSequence::create(CCMoveBy::create(1.5f, ccp(d, 0)), CCCallFuncND::create(this, callfuncND_selector(PuzzleResult::Callback_ProgressBar), (void*)isFullExp), NULL);
    bar->runAction(action2);
    
    // levelup icon
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_levelup.png", ccp(1, 0), ccp(125+146+10+412-20+30+30, 525+30+31-15), CCSize(0,0), "", "PuzzleResult", this, 1006, 0, 0) );
    
    //////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
    // 673
    // 549, 31 --> 582, 25
    
    
    
    // 코코
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "image/coco.png", ccp(0.5, 0.5), ccp(850, 645), CCSize(0,0), "", "PuzzleResult", this, 1005) );
    ((CCSprite*)spriteClass->FindSpriteByName("image/coco.png"))->setScale(1.7f);
    ((CCSprite*)spriteClass->FindSpriteByName("image/coco.png"))->setFlipX(true);
    
    // 확인 버튼
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_red.png", ccp(0.5, 0.5), ccp(m_winSize.width/2, 300), CCSize(0, 0), "", "PuzzleResult", this, 1005) );
    CCPoint pos = spriteClass->FindParentCenterPos("button/btn_red.png");
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_confirm.png", ccp(0.5, 0.5), ccp(pos.x, pos.y+5), CCSize(0, 0), "button/btn_red.png", "0", this, 1005, 1) );
    

    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
}

void PuzzleResult::Callback_ProgressBar(CCNode* sender, void* data)
{
    //if ((int)data == 1)
    //{
        ((CCSprite*)spriteClass->FindSpriteByName("icon/icon_levelup.png"))->setOpacity(255);
    
        CCActionInterval* action = CCSequence::create(CCMoveBy::create(0.7f, ccp(0, -5)), CCMoveBy::create(0.7f, ccp(0, 5)), NULL);
        CCActionInterval* rep = CCRepeatForever::create(action);
        ((CCSprite*)spriteClass->FindSpriteByName("icon/icon_levelup.png"))->runAction(rep);
    //}
}



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


void PuzzleResult::ScoreTimer(float f)
{
    varScore += (score / 20);
    if (varScore > score)
    {
        this->unschedule(schedule_selector(PuzzleResult::ScoreTimer));
        varScore = score;
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
void PuzzleResult::SkillTimer(float f)
{
    char number[30];
    sprintf(number, "skill_%d.png", test[ppp]);
    
    CCActionInterval* action = CCSequence::create(CCJumpBy::create(1.5f, ccp(-(350+50), 0), 80, 3), CCCallFuncND::create(this, callfuncND_selector(PuzzleResult::Callback), NULL), NULL);
    ((CCSprite*)spriteClassSkill->FindSpriteByName(number))->runAction(action);
    
    ppp++;
}
void PuzzleResult::TopazTimer(float f)
{
    topaz += ((float)(myInfo->GetTopaz() - prevTopaz) / 20.0f);
    if ((int)topaz >= myInfo->GetTopaz())
    {
        this->unschedule(schedule_selector(PuzzleResult::TopazTimer));
        topaz = (float)myInfo->GetTopaz();
    }
    
    ((CCLabelTTF*)spriteClass->FindLabelByTag(1))->setString(Common::MakeComma((int)topaz).c_str());
}
void PuzzleResult::StarCandyTimer(float f)
{
    starcandy += ((float)(myInfo->GetStarCandy() - prevStarCandy) / 20.0f);
    if ((int)starcandy >= myInfo->GetStarCandy())
    {
        this->unschedule(schedule_selector(PuzzleResult::StarCandyTimer));
        starcandy = (float)myInfo->GetStarCandy();
    }
    
    ((CCLabelTTF*)spriteClass->FindLabelByTag(2))->setString(Common::MakeComma((int)starcandy).c_str());
}
void PuzzleResult::MPTimer(float f)
{
    mp += ((float)(myInfo->GetMPTotal() - prevMP) / 20.0f);
    if ((int)mp >= myInfo->GetMPTotal())
    {
        this->unschedule(schedule_selector(PuzzleResult::MPTimer));
        mp = (float)myInfo->GetMPTotal();
    }
    
    ((CCLabelTTF*)spriteClass->FindLabelByTag(3))->setString(Common::MakeComma((int)mp).c_str());
}




bool PuzzleResult::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    //CCPoint point = pTouch->getLocation();
    return true;
}


void PuzzleResult::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
}

void PuzzleResult::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    CCPoint point = pTouch->getLocation();
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        if (spriteClass->spriteObj[i]->name == "button/btn_red.png")
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
    pThis->pBlack->removeFromParentAndCleanup(true);
    pThis->pBlackClose->removeFromParentAndCleanup(true);
    
    pThis->removeFromParentAndCleanup(true);
}

