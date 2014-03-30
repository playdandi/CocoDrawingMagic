#include "Puzzle.h"

enum
{
	zBackground = 0,
	zGameObject = 100,
    zGameObjectSP = 101,
    zPieceConn = 3,
};

CCScene* Puzzle::scene()
{
    CCLog("Puzzle : scene");
	CCScene* pScene = CCScene::create();
    
	Puzzle* pLayer = Puzzle::create();
	pScene->addChild(pLayer);
    
	return pScene;
}

void Puzzle::onEnter()
{
    CCLog("Puzzle : onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
    CCLayer::onEnter();
}
void Puzzle::onExit()
{
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
}

void Puzzle::keyBackClicked()
{
    CCDirector::sharedDirector()->end();
}

bool Puzzle::init()
{
    CCLog("puzzle init");
    
	if (CCLayer::init() == false)
	{
		return false;
	}
    
    sound = new SoundGame();
    sound->PreLoadSound();
    
    effect = new Effect();
    effect->Init(this);
    
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("images/game.plist");
    
    spriteClass = new SpriteClass();
    
    puzzleP8set = new PuzzleP8Set();
    puzzleP8set->SetGameLayer(this);
    puzzleP4set = new PuzzleP4Set();
    puzzleP4set->SetGameLayer(this);
    
    
    // skill algorithm
    skill = new PuzzleSkill();
    skill->SetGameLayer(this);
    std::vector<int> skillNum, skillProb, skillLv;
    skillNum.push_back(0);
    skillNum.push_back(8);
    skillNum.push_back(16);
    
    skillNum.push_back(1);
    //skillNum.push_back(9);
    //skillNum.push_back(17);
    
    skillNum.push_back(5);
    skillNum.push_back(13);
    skillNum.push_back(21);
    for (int i = 0 ; i < skillNum.size() ; i++) {
        skillProb.push_back(100);
        skillLv.push_back(4);
    }
    skill->Init(skillNum, skillProb, skillLv);
    
    /* color data 수집
    for (int i = 0; i <= 45; i++) {
        res_allCnt.push_back(0);
        std::vector<int> temp;
        for (int j = 0 ; j < 5; j++)
            temp.push_back(0);
        res_colorCnt.push_back(temp);
    }
    res_cycleCnt = 0;
    */
    
	m_winSize = CCDirector::sharedDirector()->getWinSize();
    
    srand(time(NULL));
    InitSprites();
    InitBoard();
    SetScore();
    SetCombo();
    SetTimer();
    
    this->setKeypadEnabled(true);
    this->setTouchEnabled(true);

    // play background sound
    sound->PlayBackgroundSound();
    
	return true;
}

void Puzzle::InitSprites()
{
    CCLog("Init Sprites");
    // background
    CCTexture2D* bgCache = CCTextureCache::sharedTextureCache()->addImage("images/game_bg.png");
    CCSprite* bg = new CCSprite();
    bg->initWithTexture(bgCache, CCRectMake(0, 0, 1080, 1920));
    bg->setAnchorPoint(ccp(0, 0));
    bg->setPosition(ccp(0, 0));
    this->addChild(bg, 0);
    
    
    // puzzle board
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/board.png", ccp(0, 0), ccp(1, 220), CCSize(0, 0), "", "Puzzle", this, 1) );
    
    // pause button
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/pause.png", ccp(0, 0), ccp(980, 1680), CCSize(0, 0), "", "Puzzle", this, 1) );
    
    // mission, shadow, magicCircle
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/mission.png", ccp(0, 0), ccp(46, 1290), CCSize(0, 0), "", "Puzzle", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/coco_shadow.png", ccp(0, 0), ccp(140, 1400), CCSize(0, 0), "", "Puzzle", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/magic_circle.png", ccp(0, 0), ccp(330, 1380), CCSize(0, 0), "", "Puzzle", this, 1) );
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/timer_outer.png", ccp(0, 0), ccp(28+7, 155+5), CCSize(0, 0), "", "Puzzle", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/timer_inner.png", ccp(0, 0), ccp(28, 155), CCSize(0, 0), "", "Puzzle", this, 1) );
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
    
    
    //CCLog("Init Sprites Done");
    
    /*
    pieceLayer = CCLayer::create();
    pieceLayer->setPosition(ccp(0, 0));
    CCSize visibleSize(1078, 1078);
    
    CCDrawNode* stencil = CCDrawNode::create();
    CCPoint ver[] = {ccp(0, 0), ccp(768, 0), ccp(768, 6*108+6), ccp(768-6-108+32, 6*108+6),
        ccp(768-6-108, 6*108+6+32), ccp(768-6-108, 7*108+6-32), ccp(768-6-108-32, 7*108+6),
        ccp(6+108+32, 7*108+6), ccp(6+108, 7*108+6-32), ccp(6+108, 6*108+6+32),
        ccp(6+108-32, 6*108+6), ccp(6+32, 6*108+6), ccp(6, 6*108+6-32)};
    stencil->drawPolygon(ver, 13, ccc4f(0,0,0,255), 0, ccc4f(0,0,0,255));
    
    CCClippingNode* clip = CCClippingNode::create(stencil);
    clip->addChild(pieceLayer);
    addChild(clip);
    */
    
    /*
    // 오른쪽 상단 (물5: 파도타기)
    CCTexture2D* bg = CCTextureCache::sharedTextureCache()->addImage("images/bg2.png");
    pado = new CCSprite();
    pado->initWithTexture(bg, CCRectMake(0, 0, 90, 90));
    pado->setAnchorPoint(ccp(0, 0));
    pado->setColor(ccc3(80, 80, 80));
    pado->setPosition(ccp(768-90, 768-90));
    this->addChild(pado);
    */
}

void Puzzle::InitBoard()
{
    CCLog("Init board");
    
    // 8각형
	for (int x = 0 ; x < COLUMN_COUNT ; x++)
	{
		for (int y = 0 ; y < ROW_COUNT ; y++)
		{
            // board의 모서리에 있는 4개는 취급하지 않는다.
            if ((x == 0 && y == 0) || (x == 0 && y == ROW_COUNT-1) ||
                (x == COLUMN_COUNT-1 && y == 0) || (x == COLUMN_COUNT-1 && y == ROW_COUNT-1))
                continue;

            puzzleP8set->CreatePiece(x, y);
            spriteP8[x][y] = puzzleP8set->GetSprite(x, y);
            puzzleP8set->AddChild(x, y);
		}
	}
    
    // 4각형
    for (int x = 1; x < COLUMN_COUNT ; x++)
    {
        for (int y = 1 ; y < ROW_COUNT ; y++)
        {
            puzzleP4set->CreatePiece(x, y);
            puzzleP4set->AddChild(x, y);
        }
    }
    
    
    // 4각형, 8각형의 lock(semaphore)을 위한 것들
    for (int x = 0 ; x < COLUMN_COUNT ; x++)
    {
        for (int y = 0 ; y < ROW_COUNT ; y++)
        {
            m_bLockP8[x][y] = 0;
            m_bLockP4[x][y] = 0;
        }
    }
    // 여러 동시 drop을 고려하여 drop ready queue를 5칸 할당해 놓는다.
    for (int i = 0 ; i < QUEUE_CNT ; i++)
    {
        std::vector<CCPoint> temp1;
        std::vector<CCPoint> temp2;
        std::vector<CCPoint> temp3;
        std::vector<CCPoint> temp4;
        lock4xy.push_back(temp1);
        lock8xy.push_back(temp2);
        piece8xy.push_back(temp3);
        piece4xy.push_back(temp4);
    }
    drop_order = 0;
    touch_cnt = 0;
    cur_priority = 0;
    max_priority = 0;
    
    m_bTouchStarted = false;
    m_bIsCycle = false;
    m_bSkillLock = false;
    isPadoRunning = false;
}

void Puzzle::SetScore()
{
    iScore = 0;
    pScoreLabel = CCLabelTTF::create("0", fontList[2].c_str(), 50);
    pScoreLabel->setAnchorPoint(ccp(0, 0));
    pScoreLabel->setPosition(ccp(PIECE8_WIDTH-6, 8*PIECE8_HEIGHT-6));
    this->addChild(pScoreLabel);
}

void Puzzle::UpdateScore(int numOfPiece)
{
    iScore += numOfPiece*100;
    char s[10];
    sprintf(s, "%d", iScore);
    pScoreLabel->setString(s);
}

void Puzzle::SetCombo()
{
    iCombo = 0;
    pComboLabel = CCLabelTTF::create("", fontList[2].c_str(), 50);
    pComboLabel->setAnchorPoint(ccp(0, 0));
    pComboLabel->setPosition(ccp(PIECE8_WIDTH-6, 7*PIECE8_HEIGHT+6+10));
    pComboLabel->setOpacity(0);
    this->addChild(pComboLabel);
}

int Puzzle::GetCombo()
{
    return iCombo;
}

void Puzzle::UpdateCombo()
{
    /*
    iCombo++;
    char temp[11];
    sprintf(temp, "%d Combo!", iCombo);
    pComboLabel->setString(temp);
    pComboLabel->setOpacity(255);
    
    iComboTimer = 0;
    this->schedule(schedule_selector(Puzzle::ComboTimer), 0.1f);
    if (iCombo == 1)
    {
        // 처음 콤보가 적용될 때만 타이머 적용을 하면 된다.
        this->schedule(schedule_selector(Puzzle::ComboTimer), 0.1f);
    }
     */
    
    //CCFiniteTimeAction* action =
    //    CCSequence::create(CCFadeIn::create(0.15f), CCFadeOut::create(0.15f), NULL);
    //pComboLabel->runAction(action);
}

void Puzzle::ComboTimer(float f)
{
    iComboTimer += 100;
    if (iComboTimer >= 3000)
    {
        iCombo = 0;
        pComboLabel->setOpacity(0);
        this->unschedule(schedule_selector(Puzzle::ComboTimer));
    }
}

void Puzzle::SetTimer()
{
    iTimer = 60000;
    pTimerLabel = CCLabelTTF::create("60", fontList[2].c_str(), 50);
    pTimerLabel->setAnchorPoint(ccp(0.5, 0.5));
    pTimerLabel->setPosition(ccp(40, 240));
    this->addChild(pTimerLabel);
    
    this->schedule(schedule_selector(Puzzle::UpdateTimer), 0.1f);
}

void Puzzle::UpdateTimer(float f)
{
    /*
    if (skill->W5GetVar()) { // "W5_시간얼리기" 스킬이 적용 중이면 2배속 느리게.
        CCLog("update timer -50");
        iTimer -= 50;
    }
    else
        iTimer -= 100;
     */
    iTimer -= 100;
    if (iTimer % 1000 == 0)
    {
        char temp[3];
        sprintf(temp, "%d", iTimer/1000);
        pTimerLabel->setString(temp);
        
        if (iTimer == 0)
        {
            // game end
            this->unschedule(schedule_selector(Puzzle::UpdateTimer));
            this->setTouchEnabled(false);
            this->setKeypadEnabled(false);
            
            // 게임결과화면 보여줌
            //CCScene* pScene = PuzzleResult::scene();
            //this->addChild(pScene, 2000, 2000);
            
            // go to Ranking Scene
            sound->StopBackgroundSound();
            Common::ShowNextScene(this, "Puzzle", "Ranking", true);
        }
    }
}


CCPoint Puzzle::SetPiece8Position(int x, int y)
{
    // start + (x,y)좌표만큼 이동 + AP로 인해 절반 더 이동 + 보정치
    int posX =   1 + x*PIECE8_WIDTH  + PIECE8_WIDTH/2  + 7;
    int posY = 220 + y*PIECE8_HEIGHT + PIECE8_HEIGHT/2 + 6;
    return ccp(posX, posY);
}
CCPoint Puzzle::SetPiece4Position(int x, int y)
{
    // start + (x,y)좌표만큼 이동 + 보정치
    int posX =   1 + x*PIECE8_WIDTH  + 7;
    int posY = 220 + y*PIECE8_HEIGHT + 6;
    return ccp(posX, posY);
}

/*
bool IsValidInCircle(CCPoint center, CCPoint point)
{
    float radius = 53.0;
    float sq = (center.x-point.x)*(center.x-point.x) + (center.y-point.y)*(center.y-point.y);
    
    return sq <= radius*radius;
}

bool IsValidInOcta(CCPoint center, CCPoint point)
{
    int rect = 54, tri = 32;
    CCPoint lu = ccp(center.x-rect, center.y+rect);
    CCPoint ld = ccp(center.x-rect, center.y-rect);
    CCPoint ru = ccp(center.x+rect, center.y+rect);
    CCPoint rd = ccp(center.x+rect, center.y-rect);
    if (abs(point.x-lu.x)+abs(point.y-lu.y) <= tri || abs(point.x-ld.x)+abs(point.y-ld.y) <= tri ||
        abs(point.x-ru.x)+abs(point.y-ru.y) <= tri || abs(point.x-rd.x)+abs(point.y-rd.y) <= tri) {
        return false;
    }
    if ((center.x-rect <= point.x && point.x <= center.x+rect) &&
        (center.y-rect <= point.y && point.y <= center.y+rect)) {
        return true;
    }
    return false;
}
*/

bool IsValidInSquare(CCPoint center, CCPoint point)
{
    // 처음 터치할 때만 보는 것이므로, 완전한 정4각형 범위까지 허용한다.
    //return abs(center.x-point.x)+abs(center.y-point.y) < PIECE8_WIDTH-1;
    if ((center.x-PIECE8_WIDTH/2 < point.x && point.x < center.x+PIECE8_WIDTH/2) &&
        (center.y-PIECE8_WIDTH/2 < point.y && point.y < center.y+PIECE8_WIDTH/2)) {
        return true;
    }
    return false;
}

CCPoint Puzzle::BoardStartPosition(CCPoint point)
{
    for (int x = 0 ; x < COLUMN_COUNT ; x++)
	{
		for (int y = 0 ; y < ROW_COUNT ; y++)
		{
            // board의 모서리에 있는 4개는 취급하지 않는다.
            if ((x == 0 && y == 0) || (x == 0 && y == ROW_COUNT-1) ||
                (x == COLUMN_COUNT-1 && y == 0) || (x == COLUMN_COUNT-1 && y == ROW_COUNT-1))
                continue;
            
            //if (IsValidInOcta(SetPiece8Position(x, y), point))
            if (IsValidInSquare(SetPiece8Position(x, y), point))
                return ccp(x, y);
		}
	}
    return ccp(-1, -1);
}


int GetDist(CCPoint center, CCPoint point)
{
    int dist = (center.x-point.x)*(center.x-point.x) + (center.y-point.y)*(center.y-point.y);
    if (dist > PIECE8_WIDTH*PIECE8_WIDTH + (PIECE8_HEIGHT/2)*(PIECE8_HEIGHT/2))
        return 999999999;
    return dist;
}

CCPoint Puzzle::BoardMovePosition(CCPoint point)
{
    CCPoint resultXY = ccp(-1, -1);
    int dist = 999999999;
    
    for (int x = 0 ; x < COLUMN_COUNT ; x++)
	{
		for (int y = 0 ; y < ROW_COUNT ; y++)
		{
            // board의 모서리에 있는 4개는 취급하지 않는다.
            if ((x == 0 && y == 0) || (x == 0 && y == ROW_COUNT-1) ||
                (x == COLUMN_COUNT-1 && y == 0) || (x == COLUMN_COUNT-1 && y == ROW_COUNT-1))
                continue;
            
            int d = GetDist(SetPiece8Position(x, y), point);
            if (d < dist && globalType[touch_cnt%QUEUE_CNT] == puzzleP8set->GetType(x, y))
            {
                dist = d;
                resultXY = ccp(x, y);
            }
		}
	}
    return resultXY;
}


bool Puzzle::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    if (m_bSkillLock) // skill 발동 중이면 무조건 터치 금지.
        return false;
    
    if (m_bTouchStarted)
        return false;
    m_bTouchStarted = true; // touch lock 걸기
    
    CCPoint point = pTouch->getLocation();
    CCLog("Puzzle :: %d , %d", (int)point.x, (int)point.y);
    /*
    if (pado->boundingBox().containsPoint(point))
    {
        if (isPadoRunning)
            return false;
        isPadoRunning = true;
        
        m_bTouchStarted = true;
        m_bSkillLock = true;
        
        // 물 5 : 파도타기
        //skill->Try(12);
        skill->Invoke(12);
        
        m_pado_callback_cnt = 0;
        
        std::vector<CCPoint> t = skill->A4AGetPos();
        int x, y;
        for (int i = 0 ; i < t.size() ; i++) {
            x = (int)t[i].x;
            y = (int)t[i].y;
            CCFiniteTimeAction* action = CCSequence::create(CCScaleTo::create(0.2f, 0.0f),
                CCCallFuncND::create(this, callfuncND_selector(Puzzle::tCB), NULL), NULL);
            m_pBoard[x][y]->runAction(action);
        }
        
        if (t.size() == 0) {
            m_bSkillLock = false;
            m_bTouchStarted = false;
            isPadoRunning = false;
        }
        
        return false;
    }
    */
    
    // 어느 퍼즐인지 찾기
    CCPoint boardPos = BoardStartPosition(point);
    int x = (int)boardPos.x;
    int y = (int)boardPos.y;
    
    if (x == -1 && y == -1) // 찾지 못했다면 퍼즐 피스를 클릭한 것이 아니므로 그냥 종료.
        return (m_bTouchStarted = false);
    
    if (m_bLockP8[x][y] > 0) // locked면 중지.
        return (m_bTouchStarted = false);
    
    
    m_bIsCycle = false;
    m_bLockP8[x][y]++; // lock 걸기

    //sound->playTouchSound();
    
    // 벡터 초기화
    piece8xy[touch_cnt%QUEUE_CNT].clear();
    piece4xy[touch_cnt%QUEUE_CNT].clear();

    // 8각형 piece를 터뜨릴 목록에 추가
    piece8xy[touch_cnt%QUEUE_CNT].push_back(ccp(x, y));

    // global piece type 지정
    globalType[touch_cnt%QUEUE_CNT] = puzzleP8set->GetType(x, y);

    // 0.9배 축소 action
    spriteP8[x][y]->setScale(0.9f);
    spriteP8[x][y]->setOpacity(100);
    
    return true;
}

void Puzzle::tCB(CCNode* sender, void* pos)
{
    /*
    m_pado_callback_cnt++;
    
    if (m_pado_callback_cnt == skill->A4AGetPos().size())
    {
        int x, y;
        std::vector<CCPoint> t = skill->A4AGetPos();
        for (int i = 0 ; i < t.size() ; i++) {
            x = (int)t[i].x;
            y = (int)t[i].y;
            pieceLayer->removeChild(m_pBoard[x][y], true);
            m_pBoard[x][y] = NULL;
            m_pBoard[x][y] = PuzzleP8::Create(this, 3);
            m_pBoard[x][y]->setPosition(SetPiece8Position(x, y));
            pieceLayer->addChild(m_pBoard[x][y], zGameObject);
        }
        
        m_bSkillLock = false;
        m_bTouchStarted = false;
        isPadoRunning = false;
    }
     */
}


void Puzzle::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
	if (m_bTouchStarted && !m_bIsCycle)
	{
        CCPoint point = pTouch->getLocation();
        
        // 어느 퍼즐인지 찾기
        CCPoint boardPos = BoardMovePosition(point);
        int x = (int)boardPos.x;
        int y = (int)boardPos.y;
    
        if (x == -1 && y == -1)
            return;
        
        // 바로 직전에 한붓그리기한 piece 위치
        int size = piece8xy[touch_cnt%QUEUE_CNT].size();
        int firstX = piece8xy[touch_cnt%QUEUE_CNT][0].x;
        int firstY = piece8xy[touch_cnt%QUEUE_CNT][0].y;
        int beforeX = piece8xy[touch_cnt%QUEUE_CNT][size-1].x;
        int beforeY = piece8xy[touch_cnt%QUEUE_CNT][size-1].y;
        
        // locked면 중지 (cycle을 생성하는 상황은 예외로 둔다)
        if (m_bLockP8[x][y] > 0)
        {
            if (piece8xy[touch_cnt%QUEUE_CNT].size() < 3) // 일단 3개 미만이면 무조건 끝.
                return;
            else if (!(x == firstX && y == firstY))
                return;
        }
        
        // 선택된 피스와 바로 직전 피스의 type이 같고, 맨하탄distance가 2 이하인 경우
        if ((puzzleP8set->GetType(x, y) == puzzleP8set->GetType(beforeX, beforeY) &&
            abs(x-beforeX)+abs(y-beforeY) <= 2))// ||
            //(x == firstX && y == firstY))
        {
            bool flag = false;
 
            // 대각선으로 연결을 시도한 경우 (4각형 체크를 위해서)
            if (abs(x-beforeX) == 1 && abs(y-beforeY) == 1)
            {
                // BLOCKED면 막힌 것이므로 한붓그리기를 못하게 한다.
                int p4X = (beforeX > x) ? beforeX : x;
                int p4Y = (beforeY > y) ? beforeY : y;
                
                if (puzzleP4set->GetType(p4X, p4Y) == BLOCKED)
                    return;
                
                // 터뜨릴 목록에 지금 대각선으로 지나치는 diamond 좌표 추가
                piece4xy[touch_cnt%QUEUE_CNT].push_back(ccp(p4X, p4Y));
                puzzleP4set->SetOpacity(p4X, p4Y, 100);
                
                flag = true;
                if (x == firstX && y == firstY && size >= 3)
                {
                    CCLog("cycle : %d , %d", x, y);
                    m_bIsCycle = true;
                }
            }
            
            // 가로나 세로로 연결을 시도한 경우
            else if ((x == beforeX && abs(y-beforeY) == 1) || (y == beforeY && abs(x-beforeX) == 1))
            {
                flag = true;
                if (x == firstX && y == firstY && size >= 3)
                {
                    CCLog("cycle : %d , %d", x, y);
                    m_bIsCycle = true;
                }
            }
            
            if (flag)
            {
                if (m_bIsCycle)
                {
                    // cycle을 표시해주는 액션이 들어간다.
                    //CCLog("cycle : %d %d", x, y);
                }
                else
                {
                    // lock
                    m_bLockP8[x][y]++;
                
                    // 8각형 piece를 터뜨릴 목록에 추가
                    piece8xy[touch_cnt%QUEUE_CNT].push_back(ccp(x, y));
            
                    // 0.9배 축소
                    spriteP8[x][y]->setScale(0.9f);
                    spriteP8[x][y]->setOpacity(100);
                    //@m_pBoard[x][y]->setScale(0.9f);
                    //@m_pBoard[x][y]->setOpacity(100);
                }
            }
            //sound->playTouchSound();
        }
	}
}

void Puzzle::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    if (m_bTouchStarted)
    {
        // 3개 이상 한붓그리기가 되었다면, 스킬 적용 여부를 판단하고, 콤보 처리와 함께 bomb를 시작한다.
        if (piece8xy[touch_cnt%QUEUE_CNT].size() >= 3)
        {
            // data log 남기는 부분 (어느 색깔을 몇 번 한붓그리기했나)
            //res_allCnt[piece8xy[touch_cnt%QUEUE_CNT].size()]++;
            //CCLog("global = %d", globalType[touch_cnt%QUEUE_CNT]);
            //CCLog("touch_cnt remain QUEUE_CNT = %d", touch_cnt%QUEUE_CNT);
            //CCLog("hmm? : %d", (int)piece8xy[touch_cnt%QUEUE_CNT].size());
            //res_colorCnt[piece8xy[touch_cnt%QUEUE_CNT].size()][globalType[touch_cnt%QUEUE_CNT]]++;
            //if (m_bIsCycle)
            //  res_cycleCnt++;
            
            // 다시 한붓그린 부분 lock을 푼다
            for (int i = 0 ; i < piece8xy[touch_cnt%QUEUE_CNT].size() ; i++)
            {
                int x = piece8xy[touch_cnt%QUEUE_CNT][i].x;
                int y = piece8xy[touch_cnt%QUEUE_CNT][i].y;
                m_bLockP8[x][y]--;
            }
            
            // 스킬 발동 try
            skill->TrySkills(globalType[touch_cnt%QUEUE_CNT]);
            
            m_iBombCallbackType[touch_cnt%QUEUE_CNT] = 0;
            m_bTouchStarted = false;
            
            touch_cnt++;
            
            // effect 체크 초기화
            effect->InitCheck();
            
            // 스킬 실행 (오토마타 표 참조)
            m_iState = 1;
            InvokeSkills();
            
            m_bIsCycle = false;
        }
        
        // 3개 미만으로 한붓그리기가 되었다면, 원상태로 복구시킨다.
        else
        {
            int x, y;
            for (int i = 0 ; i < piece8xy[touch_cnt%QUEUE_CNT].size() ; i++)
            {
                x = (int)piece8xy[touch_cnt%QUEUE_CNT][i].x;
                y = (int)piece8xy[touch_cnt%QUEUE_CNT][i].y;
                spriteP8[x][y]->setScale(1.0f);
                spriteP8[x][y]->setOpacity(255);
                // lock도 해제
                m_bLockP8[x][y]--;
            }
            
            for (int i = 0 ; i < piece4xy[touch_cnt%QUEUE_CNT].size() ; i++)
            {
                x = (int)piece4xy[touch_cnt%QUEUE_CNT][i].x;
                y = (int)piece4xy[touch_cnt%QUEUE_CNT][i].y;
                puzzleP4set->SetOpacity(x, y, 255);
            }
            
            piece8xy[touch_cnt%QUEUE_CNT].clear();
            piece4xy[touch_cnt%QUEUE_CNT].clear();
            m_bTouchStarted = false;
            m_bIsCycle = false;
        }
    }
}

void Puzzle::InvokeSkills()
{
    if (m_iState == 1)
    {
        CCLog("state 1");
        // W5(시간을얼리다), E5(끈질긴생명력)
        skill->Invoke(14);
        skill->Invoke(22);
        
        // 마지막(8번) 스킬이 적용되었다면,
        if (skill->IsApplied(7) || skill->IsApplied(15) || skill->IsApplied(23))
        {
            // cycle(2A)이 적용되었다면,
            if (skill->IsApplied(1) || skill->IsApplied(9) || skill->IsApplied(17))
            {
                m_iState = 2;
            }
            else // 아니라면,
            {
                m_iState = 3;
            }
        }
        else
        {
            // cycle이 적용됨과 관계없이, 한붓그리기 한 것을 터뜨리기
            m_iState = 4;
        }
        
        InvokeSkills();
    }
    else if (m_iState == 2)
    {
        CCLog("state 2");
        if (skill->IsApplied(7) || skill->IsApplied(15) || skill->IsApplied(23)) // 8번 적용된 경우 8번 '준비'
        {
            m_iNextState = 3;
        }
        else
        {
            m_iNextState = 5;
        }
        
        // 8번, cycle 모두 적용된 경우 : cycle에서 나온 주변부 터뜨리기
        skill->Invoke(1); skill->Invoke(9); skill->Invoke(17);
    }
    else if (m_iState == 3)
    {
        CCLog("state 3");
        m_iNextState = 5;
        
        // 8번 적용된 경우 : 8번 스킬 '준비' & '실행'
        skill->Invoke(7); skill->Invoke(15); skill->Invoke(23);
    }
    else if (m_iState == 4)
    {
        CCLog("state 4");
        if (skill->IsApplied(1) || skill->IsApplied(9) || skill->IsApplied(17))
        {
            m_iNextState = 2;
        }
        else
        {
            m_iNextState = 5;
        }
        
        Lock((touch_cnt-1)%QUEUE_CNT);
        
        // 추가점수, +10 스킬 적용되었다면 실행한 후, 한붓그리기 한 것을 터뜨리기
        skill->Invoke(0); skill->Invoke(8); skill->Invoke(16); // 추가점수
        skill->Invoke(11); skill->Invoke(19); // +10
        
        Bomb(piece8xy[(touch_cnt-1)%QUEUE_CNT]);
    }
    else if (m_iState == 5)
    {
        CCLog("state 5");
        m_iNextState = 6;
        
        // 6개 이상 제거 시, 한 번 더 제거
        skill->Invoke(5); skill->Invoke(13); skill->Invoke(21);

        if (!(skill->IsApplied(5) || skill->IsApplied(13) || skill->IsApplied(21)))
        {
            // 스킬이 발동되지 않았으면 다음으로 넘긴다.
            m_iState = m_iNextState;
            InvokeSkills();
        }
    }
    else if (m_iState == 6)
    {
        CCLog("state 6");
        m_iNextState = 7;
        
        // 불지르기, 물내리기, 땅꺼지기
        skill->Invoke(4); skill->Invoke(20); //skill->Invoke(12); skill->Invoke(20);
        
        if (!(skill->IsApplied(4) || skill->IsApplied(12) || skill->IsApplied(20)))
        {
            // 스킬이 발동되지 않았으면 다음으로 넘긴다.
            m_iState = m_iNextState;
            InvokeSkills();
        }
    }
    else if (m_iState == 7)
    {
        CCLog("state 7");
        m_iNextState = 8;
        
        // 코코타임
        skill->Invoke(6);
        
        if (!skill->IsApplied(6))
        {
            // 스킬이 발동되지 않았으면 다음으로 넘긴다.
            m_iState = m_iNextState;
            InvokeSkills();
        }
    }
    else if (m_iState == 8)
    {
        CCLog("state 8");
        m_iNextState = -1;
        
        // MT*2
        skill->Invoke(3);
        
        SetSkillLock(false);
    }
}

void Puzzle::Lock(int queue_pos)
{
    // 스킬 발동이 된 경우는 이미 전체가 lock이므로 이 함수를 실행할 필요가 없다.
    //if (m_bSkillLock)
    //    return;
    CCLog("Lock() - state : %d", m_iState);
    
    // drop되는 모든 영역에 lock을 건다.
    int x, y;
    int lowest[COLUMN_COUNT];
    for (int i = 0 ; i < COLUMN_COUNT ; i++)
    {
        lowest[i] = ROW_COUNT;
    }
    for (int i = 0 ; i < piece8xy[queue_pos].size() ; i++)
    {
        x = piece8xy[queue_pos][i].x;
        y = piece8xy[queue_pos][i].y;
        if (y < lowest[x])
            lowest[x] = y;
    }
    for (x = 0 ; x < COLUMN_COUNT ; x++)
    {
        for (y = lowest[x] ; y < ROW_COUNT ; y++)
        {
            // lock 거는 부분 (semaphore +1)
            m_bLockP8[x][y]++;
            // 나중에 lock 걸린 위치를 정확히 해제하기 위해, 그 위치를 따로 저장.
            if (m_bLockP8[x][y] > 0)
                lock8xy[queue_pos].push_back(ccp(x, y));
        }
    }
}

void Puzzle::Bomb(std::vector<CCPoint> bomb_pos)
{
    int queue_pos = (touch_cnt-1)%QUEUE_CNT;
    
    //Lock(queue_pos);
    
    int x, y;
  
    // 8각형들을 터뜨린다.
    m_iBombCallbackCnt[queue_pos] = 0;
    m_iBombCallbackCntMax = bomb_pos.size();
    for (int i = 0 ; i < bomb_pos.size() ; i++)
    {
        x = (int)bomb_pos[i].x;
        y = (int)bomb_pos[i].y;

        // 8각형 주변의 diamond중에 터지지 않은 diamond를 마저 지운다.
        if (x > 0 && y > 0 && x < COLUMN_COUNT && y < ROW_COUNT) // leftdown
            puzzleP4set->RemoveChild(x, y);
        if (x > 0 && y+1 > 0 && x < COLUMN_COUNT && y+1 < ROW_COUNT) // leftup
            puzzleP4set->RemoveChild(x, y+1);
        if (x+1 > 0 && y > 0 && x+1 < COLUMN_COUNT && y < ROW_COUNT) // rightdown
            puzzleP4set->RemoveChild(x+1, y);
        if (x+1 > 0 && y+1 > 0 && x+1 < COLUMN_COUNT && y+1 < ROW_COUNT) // rightup
            puzzleP4set->RemoveChild(x+1, y+1);
        
        float bombTime = 0.15f;
        if (m_iState == 2)
            bombTime = 0.50f;
        
        // 터뜨리는 action
        CCFiniteTimeAction* action = CCSequence::create(
                CCSpawn::create(CCScaleTo::create(bombTime, 1.5f), CCFadeOut::create(bombTime), NULL),
                CCCallFuncND::create(this, callfuncND_selector(Puzzle::BombCallback), (void*)queue_pos),
                NULL);
        spriteP8[x][y]->runAction(action);
    }

    // update score
    UpdateScore(bomb_pos.size());
    // update combo
    UpdateCombo();
    
    //sound->playBombSound();
}

void Puzzle::BombCallback(CCNode* sender, void* queue_pos)
{
    m_iBombCallbackCnt[(int)queue_pos]++;
    
    if (m_iBombCallbackCnt[(int)queue_pos] == m_iBombCallbackCntMax)
    {
        if (m_iState == 2) // cycle 주변부 폭발 완료
        {
            CCLog("bomb callback 2");
            std::vector<CCPoint> temp = skill->A2AGetPos();
            for (int i = 0 ; i < temp.size() ; i++)
            {
                puzzleP8set->RemoveChild((int)temp[i].x, (int)temp[i].y);
                spriteP8[(int)temp[i].x][(int)temp[i].y] = NULL;
            }
            temp.clear();
            skill->A2AClear();
        }
        else if (m_iState == 3) // 8번 스킬 폭발 완료
        {
            CCLog("bomb callback 3");
            std::vector<CCPoint> temp = skill->A8GetPos();
            for (int i = 0 ; i < temp.size() ; i++)
            {
                puzzleP8set->RemoveChild((int)temp[i].x, (int)temp[i].y);
                spriteP8[(int)temp[i].x][(int)temp[i].y] = NULL;
            }
            temp.clear();
            skill->A8Clear();
        }
        else if (m_iState == 4) // 한붓그리기 부분 폭발 완료
        {
            CCLog("bomb callback 4");
            // 추가점수 , +10 은 따로 둔 callback에서 처리하도록 하자
            
            for (int i = 0 ; i < piece8xy[(int)queue_pos].size() ; i++)
            {
                int x = (int)piece8xy[(int)queue_pos][i].x;
                int y = (int)piece8xy[(int)queue_pos][i].y;
                puzzleP8set->RemoveChild(x, y);
                spriteP8[x][y] = NULL;
            }
            
            if (m_iNextState == 2) // 그 다음이 cycle 주변부를 바로 터뜨려야 한다면
            {
                CCLog("bomb callback 4 --> next : cycle boundary");
                m_iState = m_iNextState;
                
                InvokeSkills();
                piece8xy[(int)queue_pos].clear();
                
                return; // drop을 하지 않고 바로 터뜨려야 하므로, Falling()으로 넘어가지 않는다.
            }
            else
            {/*
                if (m_iNextState == 5)
                {
                    // 다음이 '6개 이상 한번더 터뜨리기'면, state(5)가 끝나고 queue를 지우자.
                }
                else
                {
                    piece8xy[(int)queue_pos].clear();
                }
              */
                piece8xy[(int)queue_pos].clear();
            }
        }
        else if (m_iState == 5)
        {
            CCLog("bomb callback 5");
            std::vector<CCPoint> temp = skill->A4BGetPos();
            for (int i = 0 ; i < temp.size() ; i++)
            {
                puzzleP8set->RemoveChild((int)temp[i].x, (int)temp[i].y);
                spriteP8[(int)temp[i].x][(int)temp[i].y] = NULL;
            }
            temp.clear();
            skill->A4BClear(); // 6개 이상 한 번 더 터진 후
        }
        else if (m_iState == 6)
        {
            CCLog("bomb callback 6");
            std::vector<CCPoint> temp = skill->A4AGetPos();
            for (int i = 0 ; i < temp.size() ; i++)
            {
                puzzleP8set->RemoveChild((int)temp[i].x, (int)temp[i].y);
                spriteP8[(int)temp[i].x][(int)temp[i].y] = NULL;
            }
            temp.clear();
            skill->A4AClear();
        }
        else if (m_iState == 7)
        {
            CCLog("bomb callback 7");
        }
        
        // 새로운 8각형 piece들의 drop을 시작한다.
        Falling((int)queue_pos);
    }
}

void Puzzle::Falling(int queue_pos)
{
    // queue에서 현재 순서(order)인 것을 찾는다. 없으면 대기한다.
    while (queue_pos == drop_order)
    {
        break;
    }
    
    CCLog("Falling : priority (%d) is started", drop_order);
    
    
    // get the number of total falling objects.
    m_iFallingCallbackCnt = 0;
    m_numOfFallingObjects = 0;
    for (int x = 0 ; x < COLUMN_COUNT ; x++)
	{
        for (int y = 0 ; y < ROW_COUNT ; y++)
		{
            // board의 모서리에 있는 4개는 취급하지 않는다.
            if ((x == 0 && y == 0) || (x == 0 && y == ROW_COUNT-1) ||
                (x == COLUMN_COUNT-1 && y == 0) || (x == COLUMN_COUNT-1 && y == ROW_COUNT-1))
                continue;
            
			if (spriteP8[x][y] == NULL)
            {
                m_numOfFallingObjects += (ROW_COUNT - y);
                if (x == 0 || x == COLUMN_COUNT-1)
                    m_numOfFallingObjects--;
                break;
            }
        }
    }
    //CCLog("total falling objects : %d", m_numOfFallingObjects);
    
	for (int x = 0 ; x < COLUMN_COUNT ; x++)
	{
        for (int y = 0 ; y < ROW_COUNT ; y++)
		{
            // board의 모서리에 있는 4개는 취급하지 않는다.
            if ((x == 0 && y == 0) || (x == 0 && y == ROW_COUNT-1) ||
                (x == COLUMN_COUNT-1 && y == 0) || (x == COLUMN_COUNT-1 && y == ROW_COUNT-1))
                continue;
            
			if (spriteP8[x][y] == NULL)
			{
                int pos;
                int end = (x == 0 || x == COLUMN_COUNT-1) ? ROW_COUNT-1 : ROW_COUNT;
                for (pos = y ; pos < end ; pos++)
                {
                    if (spriteP8[x][pos] != NULL) //m_pBoard[x][pos] != NULL)
                        break;
                }
                
                if (pos >= end)
                {
                    // 더 이상 내려야 할 8각형이 없는 경우 (새로 만들어서 drop시킨다)
                    puzzleP8set->CreatePiece(x, y);
                    puzzleP8set->GetObject(x, y)->SetPosition(SetPiece8Position(x, end));
                    puzzleP8set->AddChild(x, y);
                    spriteP8[x][y] = puzzleP8set->GetSprite(x, y);
                    puzzleP8set->Falling(x, y, x, y, queue_pos);
                }
                else
                {
                    // 떨어지는 경우
                    
                    //CCLog("rightbefore fall : %d %d / %d %d", x, y, x, pos);
                    //@m_pBoard[x][y] = m_pBoard[x][pos];
                    //@m_pBoard[x][pos] = NULL;
                    //@m_pBoard[x][y]->Falling(x, y, queue_pos);
                    puzzleP8set->MoveObject(x, y, x, pos);
                    //puzzleP8set->RemoveChild(x, pos);
                    spriteP8[x][y] = puzzleP8set->GetSprite(x, y);
                    spriteP8[x][pos] = NULL;
                    puzzleP8set->Falling(x, y, x, y, queue_pos);
                }
                
                //m_numOfFallingObjects++;
                //CCLog("... %d ", m_numOfFallingObjects);
			}
		}
	}
    
    //CCLog("Falling done : %d", queue_pos);
}

void Puzzle::FallingCallback(CCNode* sender, void* queue_pos)
{
    int queue = (int)queue_pos;
    m_iFallingCallbackCnt++;
    //CCLog("falling callback - %d , %d", m_iFallingCallbackCnt, m_numOfFallingObjects);
	if (m_numOfFallingObjects == m_iFallingCallbackCnt)
	{
        //CCLog("falling callback (queue_pos : %d)", queue);
        // drop이 모두 끝나면, diamond들을 다시 검사해서 적절히 바꿔준다.
		for (int x = 1 ; x < COLUMN_COUNT ; x++)
        {
            for (int y = 1 ; y < ROW_COUNT ; y++)
            {
                if (puzzleP4set->GetType(x, y) != BLOCKED) //m_pBoardSP[x][y]->GetType() != BLOCKED)
                {
                    if (puzzleP4set->GetObject(x, y) != NULL)
                        puzzleP4set->RemoveChild(x, y);
                    puzzleP4set->CreatePiece(x, y, puzzleP4set->GetType(x, y));
                    puzzleP4set->AddChild(x, y);
                    //m_pBoardSP[x][y]->RemoveChildren();
                    //m_pBoardSP[x][y]->CreateSprites(this, x, y);
                    //m_pBoardSP[x][y]->SetPositions(SetPiece4Position(x, y));
                    //m_pBoardSP[x][y]->AddChildren(zGameObjectSP);
                }
            }
        }
        
        // lock을 모두 해제한다. (semaphore를 1씩 감소시킨다)
        int x, y;
        //CCLog("lock size : %d", (int)lock8xy[queue_pos].size());
        for (int i = 0 ; i < lock8xy[queue].size() ; i++)
        {
            x = lock8xy[queue][i].x;
            y = lock8xy[queue][i].y;
            m_bLockP8[x][y]--;
        }
        lock8xy[queue].clear();
        
        /************ LOCK print ****************/
        /*
        for (int y = ROW_COUNT-1 ; y >= 0 ; y--)
        {
            CCLog("%d %d %d %d %d %d %d", m_bLockP8[0][y], m_bLockP8[1][y], m_bLockP8[2][y],
                  m_bLockP8[3][y], m_bLockP8[4][y], m_bLockP8[5][y], m_bLockP8[6][y]);
        }
        */
        /*****************************************/

        // 마지막으로 current priority를 올려, 다음 대기 중인 drop queue가 실행될 수 있도록 한다.
        if (!m_bSkillLock)
            drop_order++;
        
        // 아직 발동할 스킬이 남아있으면 실행한다.
        if (m_iNextState != -1)
        {
            m_iState = m_iNextState;
            InvokeSkills();
        }
        else
        {
            SetSkillLock(false);
        }
    }
}

std::vector<CCPoint> Puzzle::GetPiece8xy(bool afterCast)
{
    if (afterCast)
        return piece8xy[(touch_cnt-1)%QUEUE_CNT];
    return piece8xy[touch_cnt%QUEUE_CNT];
}

int Puzzle::GetGlobalType()
{
    return globalType[(touch_cnt-1)%QUEUE_CNT];
}
/*
int Puzzle::GetP8Type(int x, int y)
{
    return m_pBoard[x][y]->GetType();
}


PuzzleP8* Puzzle::GetP8(int x, int y)
{
    return m_pBoard[x][y];
}
*/
void Puzzle::SetSkillLock(bool flag)
{
    m_bSkillLock = flag;
}

bool Puzzle::IsCycle()
{
    return m_bIsCycle;
}

/*
PuzzleSkill* Puzzle::GetSkill()
{
    return skill;
}
*/
/*
void Puzzle::UpdateScore()
{
    // 3개 = 기본 10점 * 3 = 30점
    // 4개 = 기본 11점 * 4 = 44점
    // 5개 = 기본 12점 * 5 = 60점
    // ...
    iScore += octaPiece.size() * (10+octaPiece.size()-3);
    char score[8];
    sprintf(score, "%d", iScore);
    scoreLabel->setString(score);
}

int Puzzle::GetScore()
{
    return iScore;
}
*/


void Puzzle::EndScene()
{
    //CCString* param = CCString::create("0");
    //CCNotificationCenter::sharedNotificationCenter()->postNotification("Ranking", param);
    
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    
    //CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, "GameReady");
    
    CCTextureCache::sharedTextureCache()->removeTextureForKey("images/game_bg.png");
    
    this->removeAllChildren();
//    scrollView->removeFromParent();
    
    //this->removeFromParentAndCleanup(true);
}

void Puzzle::EndSceneCallback()
{
}

PuzzleP8Set* Puzzle::GetPuzzleP8Set()
{
    return puzzleP8set;
}


void Puzzle::PlayEffect(int skillNum)
{
    CCLog("Puzzle :: PlayEffect - %d", skillNum);
    if (skillNum == 1 || skillNum == 9 || skillNum == 17)
        effect->PlayEffect(skillNum, skill->A2AGetPos());
    else if (skillNum == 5 || skillNum == 13 || skillNum == 21)
        effect->PlayEffect(skillNum, skill->A4BGetPos());
    else
        effect->PlayEffect(skillNum, piece8xy[(touch_cnt-1)%QUEUE_CNT]);
}


///////////////////////////////////////////////////////////////////////
void PuzzleP8Set::SetGameLayer(Puzzle* layer)
{
    gameLayer = layer;
}

void PuzzleP8Set::CreatePiece(int x, int y)
{
    object[x][y] = PuzzleP8::CreateP8(ccp(0.5, 0.5), gameLayer->SetPiece8Position(x, y), gameLayer, zGameObject);
}

CCSprite* PuzzleP8Set::GetSprite(int x, int y)
{
    return object[x][y]->GetPiece();
}

void PuzzleP8Set::AddChild(int x, int y)
{
    gameLayer->addChild(object[x][y]->GetPiece(), object[x][y]->GetZOrder());
}

void PuzzleP8Set::RemoveChild(int x, int y)
{
    gameLayer->removeChild(object[x][y]->GetPiece(), true);
    delete object[x][y];
}

int PuzzleP8Set::GetType(int x, int y)
{
    return object[x][y]->GetType();
}

PuzzleP8* PuzzleP8Set::GetObject(int x, int y)
{
    return object[x][y];
}

void PuzzleP8Set::MoveObject(int x, int y, int fromX, int fromY)
{
    object[x][y] = object[fromX][fromY];
}

void PuzzleP8Set::Falling(int x, int y, int targetX, int targetY, int queue_pos)
{
    //CCLog("falling : %d %d  /  %d %d / %d", x, y, targetX, targetY, queue_pos);
    CCMoveTo* moveTo = CCMoveTo::create(0.1f, gameLayer->SetPiece8Position(targetX, targetY));
    CCFiniteTimeAction* action = CCSequence::create(
            moveTo,
            CCCallFuncND::create(gameLayer, callfuncND_selector(Puzzle::FallingCallback), (void*)queue_pos),
            NULL);
    object[x][y]->GetPiece()->runAction(action);
}

void PuzzleP8Set::FallingCompleted(CCNode* sender, void* queue_pos)
{
	//gameLayer->FallingCallback((int)queue_pos);
}




////////////////////////////////////////////////////////////////////////
void PuzzleP4Set::SetGameLayer(Puzzle* layer)
{
    gameLayer = layer;
}
void PuzzleP4Set::CreatePiece(int x, int y, int type)
{
    // 처음에만 object를 생성한다.
    if (type == -1)
        object[x][y] = PuzzleP4::CreateP4(gameLayer, zGameObjectSP, type);

    // 4각형의 4방향에 있는 8각형들의 type을 구해서 삼각형을 어떻게 만들지 결정한다.
    // 가장자리는 그 주위의 P8 type이 존재하지 않을 수도 있으므로 조건문을 잘 쓰자.
    int lu = (x == 1 && y == ROW_COUNT-1) ? -1 : gameLayer->GetPuzzleP8Set()->GetType(x-1, y);
    int ru = (x == COLUMN_COUNT-1 && y == ROW_COUNT-1) ? -2 : gameLayer->GetPuzzleP8Set()->GetType(x, y);
    int ld = (x == 1 && y == 1) ? -3 : gameLayer->GetPuzzleP8Set()->GetType(x-1, y-1);
    int rd = (x == COLUMN_COUNT-1 && y == 1) ? -4 : gameLayer->GetPuzzleP8Set()->GetType(x, y-1);
    
    //float offsetX = (float)39.7/(float)40;
    //float offsetY = (float)0.2 /(float)40;
    int offsetX = 1;
    int offsetY = 0;
    object[x][y]->CreateSprites(x, y, lu, ru, ld, rd, ccp(offsetX, offsetY), gameLayer->SetPiece4Position(x, y));
}
int PuzzleP4Set::GetType(int x, int y)
{
    return object[x][y]->GetType();
}
void PuzzleP4Set::SetOpacity(int x, int y, int alpha)
{
    object[x][y]->GetLeftUp()->setOpacity(alpha);
    object[x][y]->GetRightUp()->setOpacity(alpha);
    object[x][y]->GetLeftDown()->setOpacity(alpha);
    object[x][y]->GetRightDown()->setOpacity(alpha);
}
void PuzzleP4Set::AddChild(int x, int y)
{
    int zOrder = object[x][y]->GetZOrder();
    if (object[x][y]->GetLeftUp() != NULL)
        gameLayer->addChild(object[x][y]->GetLeftUp(), zOrder);
    if (object[x][y]->GetRightUp() != NULL)
        gameLayer->addChild(object[x][y]->GetRightUp(), zOrder);
    if (object[x][y]->GetLeftDown() != NULL)
        gameLayer->addChild(object[x][y]->GetLeftDown(), zOrder);
    if (object[x][y]->GetRightDown() != NULL)
        gameLayer->addChild(object[x][y]->GetRightDown(), zOrder);
}
void PuzzleP4Set::RemoveChild(int x, int y)
{
    if (object[x][y]->GetLeftUp() != NULL)
        gameLayer->removeChild(object[x][y]->GetLeftUp(), true);
    if (object[x][y]->GetRightUp() != NULL)
        gameLayer->removeChild(object[x][y]->GetRightUp(), true);
    if (object[x][y]->GetLeftDown() != NULL)
        gameLayer->removeChild(object[x][y]->GetLeftDown(), true);
    if (object[x][y]->GetRightDown() != NULL)
        gameLayer->removeChild(object[x][y]->GetRightDown(), true);
    
    object[x][y]->InitChild();
}
PuzzleP4* PuzzleP4Set::GetObject(int x, int y)
{
    return object[x][y];
}
