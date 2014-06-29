#include "T_PuzzleSkill.h"
#include "T_Puzzle.h"

T_PuzzleSkill::T_PuzzleSkill()
{}

T_PuzzleSkill::~T_PuzzleSkill()
{}

void T_PuzzleSkill::SetGameLayer(T_Puzzle* layer)
{
    m_pGameLayer = layer;
}

T_Puzzle* T_PuzzleSkill::GetGameLayer()
{
    return m_pGameLayer;
}

T_Effect* T_PuzzleSkill::GetEffect()
{
    return m_pGameLayer->GetEffect();
}

void T_PuzzleSkill::Init(std::vector<int> num, std::vector<int> prob, std::vector<int> lv)
{
    for (int i = 0 ; i < num.size(); i++)
        CCLog("%d번 스킬 작동할 것임", num[i]);
    
    // init.
    for (int i = 0 ; i < NUMOFSKILL ; i++)
    {
        skillNumber[i] = false;
        skillProb[i] = 0;
        skillLevel[i] = 0;
        skillAppliedCnt[i] = 0;
    }
    
    // 유저가 현재 게임에서 쓸 수 있는 skill 번호들
    for (int i = 0 ; i < num.size() ; i++)
    {
        skillNumber[num[i]] = true; // possible skill 번호
        skillProb[num[i]] = prob[i]; // probability
        skillLevel[num[i]] = lv[i]; // level
    }
    
    // 각 스킬 관련 변수들 초기화
    F8_isActive = false;
}

int T_PuzzleSkill::GetSkillAppliedCount(int skillNum)
{
    return skillAppliedCnt[skillNum];
}

bool T_PuzzleSkill::IsSkillNumberExists(int skillNum)
{
    return skillNumber[skillNum];
}

void T_PuzzleSkill::TrySkills(int pieceColor, int queue_pos)
{
    // Init
    for (int i = 0 ; i < NUMOFSKILL ; i++)
    {
        skillApplied[queue_pos][i] = false;
    }
    
    for (int i = 0 ; i < NUMOFSKILL ; i++)
    {
        if (i >= 24) // 24번부터 궁극 스킬인데 아직 없으니까 무조건 패스하자. 만들면 이거 지워라.
            break;
        
        if (skillNumber[i]) // 스킬 목록에 있어야만 발동함.
        {
            // Cycle (주변부 터뜨리기)
            if (i == 1) {
                if (pieceColor == PIECE_RED && m_pGameLayer->IsCycle(queue_pos))
                    Try(i, queue_pos);
            }
            
            // 기본 1번 스킬
            else if (i == 0) {
                if (pieceColor == PIECE_RED)
                    Try(i, queue_pos);
            }
            
            // 마지막 스킬
            else if (i == 7) {
                if (pieceColor == PIECE_RED) {
                    if (m_pGameLayer->GetPiece8xy(false).size() >= 10)
                        Try(i, queue_pos);
                }
            }
            
            else
                Try(i, queue_pos);
        }
    }
}

void T_PuzzleSkill::Try(int skillNum, int queue_pos)
{
    // 이번 라운드에 시전을 하는지, 그렇지 않은지 결정
    int prob = rand()%100;
    if (prob < skillProb[skillNum])
        skillApplied[queue_pos][skillNum] = true;
    
    if ((skillNum == 5 || skillNum == 13 || skillNum == 21) && skillApplied[queue_pos][skillNum])
    {
        // 6개 이상 제거 시 한 번 더 터뜨리는 스킬. 미리 위치를 저장함.
        result_pos.clear();
        result_pos = m_pGameLayer->GetPiece8xy(false);
    }
    
    // 퍼즐 lock걸어서 스킬 발동이 끝날 때까지 추가적인 한붓그리기를 못하게 한다.
    int i = skillNum;
    if (skillApplied[queue_pos][i])
    {
        if (i == 1 || i == 5 || i == 6 || i == 7 ||
            i == 9 || i == 13 || i == 15 ||
            i == 17 || i == 21 || i == 23)
            // master : i == 25 || i == 26 || i == 28
        {
            m_pGameLayer->SetSkillLock(queue_pos, true);
        }
    }
}

bool T_PuzzleSkill::IsApplied(int skillNum, int queue_pos)
{
    return skillApplied[queue_pos][skillNum];
}

void T_PuzzleSkill::SetQueuePos(int queue_pos)
{
    queuePos = queue_pos;
}

void T_PuzzleSkill::UpdateAppliedSkillCount(int skillNum)
{
    skillAppliedCnt[skillNum]++;
}

void T_PuzzleSkill::Invoke(int skillNum, int queue_pos)
{
    // 스킬 목록에 없거나, 있더라도 시전이 되지 않은 경우(확률 실패한 경우), 실행하지 않는다.
    //if (!skillNumber[skillNum] || !skillApplied[queue_pos][skillNum])
    //    return;
    
    // coco animation
    if (skillNum != 7)
        m_pGameLayer->schedule(schedule_selector(T_Puzzle::CocoAnim), 0.04f);
    
    switch (skillNum)
    {
        case 0:  A1(skillNum, queue_pos); break;
        case 1:  A2(skillNum, queue_pos); break;
        case 7:  F8(skillNum, queue_pos); break;
    }
}

void T_PuzzleSkill::A1(int num, int queue_pos)
{
    // 마법불꽃, 은은한 달빛, 대지의 숨결 - 각 색깔의 피스 제거 시, 추가점수
    
    // 추가점수 = 스킬레벨 * 한붓그리기 개수 * 30 (점)
    A1_addedScore = 30*skillLevel[num]*m_pGameLayer->GetPiece8xy(true).size();
    m_pGameLayer->UpdateScore(1, A1_addedScore);
    
    // 이펙트
    m_pGameLayer->GetEffect()->PlayEffect_MagicCircle(num);
    m_pGameLayer->GetEffect()->PlayEffect_SkillIcon(num);
    m_pGameLayer->GetEffect()->PlayEffect_0(m_pGameLayer->GetPiece8xy(true));
    
    // 사이클이 발동된 상태면, 사이클 이펙트도 같이 보여준다. (태양열, 파도, 클로버 그림 띄우는 것)
    if (IsApplied(num+1, queue_pos) && m_pGameLayer->IsCycle(queue_pos))
        m_pGameLayer->GetEffect()->PlayEffect_CycleOnly(-1, m_pGameLayer->GetPiece8xy(true));
        //m_pGameLayer->PlayEffect(-(num+1), queue_pos);
}

void T_PuzzleSkill::A2(int num, int queue_pos)
{
    // 불꽃송이, 파도타기, 땅울림 - 각 색깔의 피스를 사이클로 제거하면 스킬 레벨에 비례하여 정해진 방식대로 주변부 터뜨리기
    // (여기서는 주변부의 위치만 구한다)
    // 불2 : 스킬레벨 수만큼 사이클의 주변부를 동시에 터뜨린다.
    // 물2 : 스킬레벨 수만큼 사이클이 끝나는 방향으로 파도타듯이 터뜨린다.
    // 땅2 : 스킬레벨 수만큼 완전히 랜덤하게 터뜨린다.
    
    // 변수 초기화
    A2_pos.clear();
    int x, y;
    int xy[COLUMN_COUNT][ROW_COUNT];
    
    for (int x = 0 ; x < COLUMN_COUNT ; x++)
        for (int y = 0 ; y < ROW_COUNT ; y++)
            xy[x][y] = 0;
    
    std::vector<CCPoint> pos;
    pos = m_pGameLayer->GetPiece8xy(true);
    for (int i = 0 ; i < pos.size() ; i++)
        xy[(int)pos[i].x][(int)pos[i].y] = -1;
    
    int pieceType;
    
    // 주변부 위치 구하기
    if (num == 1) // 불2 (태양열)
    {
        // 사이클에 붙어있는 피스 중 랜덤하게 선택
        std::vector<CCPoint> selectPos;
        for (x = 0 ; x < COLUMN_COUNT ; x++)
        {
            for (y = 0 ; y < ROW_COUNT ; y++)
            {
                if ((x == 0 && y == 0) || (x == 0 && y == ROW_COUNT-1) ||
                    (x == COLUMN_COUNT-1 && y == 0) || (x == COLUMN_COUNT-1 && y == ROW_COUNT-1))
                    continue;
                
                if ((y+1 < ROW_COUNT && xy[x][y+1] == -1) ||
                    (y-1 >= 0 && xy[x][y-1] == -1) ||
                    (x-1 >= 0 && y+1 < ROW_COUNT && xy[x-1][y+1] == -1) ||
                    (x-1 >= 0 && y-1 >= 0 && xy[x-1][y-1] == -1) ||
                    (x-1 >= 0 && xy[x-1][y] == -1) ||
                    (x+1 < COLUMN_COUNT && y+1 < ROW_COUNT && xy[x+1][y+1] == -1) ||
                    (x+1 < COLUMN_COUNT && y-1 >= 0 && xy[x+1][y-1] == -1) ||
                    (x+1 < COLUMN_COUNT && xy[x+1][y] == -1))
                {
                    pieceType = m_pGameLayer->GetPuzzleP8Set()->GetType(x, y);
                    if (pieceType >= PIECE_RED && pieceType <= PIECE_WHITE) // 일반 피스인 경우에만 허용한다.
                        selectPos.push_back(ccp(x, y));
                }
            }
        }
        
        // 후보 위치 중에서 랜덤하게 고른다.
        int size = std::min(skillLevel[num], (int)selectPos.size());
        int cnt = 0, position;
        while (cnt < size)
        {
            position = rand() % (int)selectPos.size();
            x = (int)selectPos[position].x;
            y = (int)selectPos[position].y;
            if (xy[x][y] == 0)
            {
                xy[x][y] = 1;
                cnt++;
                A2_pos.push_back(selectPos[position]);
            }
        }
        selectPos.clear();
    }
    
    // 이펙트 실행 (태양/달/파도 그림)
    m_pGameLayer->GetEffect()->PlayEffect_MagicCircle(num);
    m_pGameLayer->GetEffect()->PlayEffect_SkillIcon(num);
    m_pGameLayer->GetEffect()->PlayEffect_1(A2_pos);
    //m_pGameLayer->PlayEffect(num, queue_pos);
    
    // 폭파 실행
    m_pGameLayer->Bomb(queue_pos, A2_pos);
}
std::vector<CCPoint> T_PuzzleSkill::A2GetPos()
{
    return A2_pos;
}
void T_PuzzleSkill::A2Clear()
{
    A2_pos.clear();
}


static T_PuzzleSkill* psF8;

void T_PuzzleSkill::F8(int num, int queue_pos)
{
    // 붉은 용의 숨결
    F8_isActive = true;
    F8_isFalling = false;
    
    UpdateAppliedSkillCount(num);
    
    psF8 = this;
    
    F8_finishCnt = 0;
    A8_pos.clear();
    
    // 배열 초기화
    int pieceType;
    F8_check[0][0] = F8_check[0][ROW_COUNT-1] = F8_check[COLUMN_COUNT-1][0] = F8_check[COLUMN_COUNT-1][ROW_COUNT-1] = -1;
    for (int x = 0 ; x < COLUMN_COUNT ; x++)
    {
        for (int y = 0 ; y < ROW_COUNT ; y++)
        {
            if ((x == 0 && y == 0) || (x == 0 && y == ROW_COUNT-1) ||
                (x == COLUMN_COUNT-1 && y == 0) || (x == COLUMN_COUNT-1 && y == ROW_COUNT-1))
                continue;
            
            F8_check[x][y] = 0;
            pieceType = m_pGameLayer->GetPuzzleP8Set()->GetType(x, y);
            if (!(pieceType >= PIECE_RED && pieceType <= PIECE_WHITE)) // 아이템은 선택하지 못하도록 한다.
                F8_check[x][y] = -1;
        }
    }
    
    int count = skillLevel[num] + 7;
    
    // 카운트 개수만큼 랜덤한 위치를 구한다.
    int x, y, ch, idx = 0;
    while ((int)A8_pos.size() < count)
    {
        x = rand()%COLUMN_COUNT;
        y = rand()%ROW_COUNT;
        
        if (F8_check[x][y] == 0)
        {
            idx++;
            A8_pos.push_back(ccp(x, y)); // 혜성이 떨어질 위치
            
            if (m_pGameLayer->GetPuzzleP8Set()->GetType(x, y) == PIECE_RED)
                F8Check(x, y, idx);
            else
                F8_check[x][y] = idx;
        }
        
        ch = 0;
        for (int i = 0 ; i < COLUMN_COUNT ; i++)
            for (int j = 0 ; j < ROW_COUNT ; j++)
                if (F8_check[i][j] > 0)
                    ch++;
        if (ch >= 45)
            break;
    }
    
    CCLog("붉은용의숨결 : 원하는 덩어리수(%d) , 실제 덩어리수(%d)", count, (int)A8_pos.size());
    
    // 2차원 vector에 저장
    for (int i = 0 ; i < result_double_pos.size() ; i++)
        result_double_pos[i].clear();
    result_double_pos.clear();
    for (int k = 1; k <= idx; k++)
    {
        std::vector<CCPoint> temp;
        for (int x = 0 ; x < COLUMN_COUNT ; x++)
            for (int y = 0 ; y < ROW_COUNT ; y++)
                if (F8_check[x][y] == k)
                    temp.push_back(ccp(x, y));
        result_double_pos.push_back(temp);
    }
    
    SetQueuePos(queue_pos);
    
    m_pGameLayer->GetSound()->PlaySkillSound(num);
    m_pGameLayer->GetEffect()->PlayEffect_7(result_double_pos, A8_pos, queue_pos);
    
    for (int y = ROW_COUNT-1 ; y >= 0 ; y--)
    {
        CCLog("%d %d %d %d %d %d %d", F8_check[0][y], F8_check[1][y], F8_check[2][y],
              F8_check[3][y], F8_check[4][y], F8_check[5][y], F8_check[6][y]);
    }
}

void T_PuzzleSkill::F8Check(int x, int y, int idx)
{
    for (int xx = x-1; xx <= x+1; xx++)
    {
        for (int yy = y-1; yy <= y+1; yy++)
        {
            if (xx < 0 || xx > COLUMN_COUNT-1 || yy < 0 || yy > ROW_COUNT-1)
                continue;
            if (F8_check[xx][yy] == 0)
            {
                F8_check[xx][yy] = idx;
                if (m_pGameLayer->GetPuzzleP8Set()->GetType(xx, yy) == PIECE_RED && !(xx == x && yy == y))
                    F8Check(xx, yy, idx);
            }
        }
    }
}

void T_PuzzleSkill::F8_Timer()
{
    A8_callbackCnt = 0;
    m_pGameLayer->schedule(schedule_selector(T_PuzzleSkill::F8_Comet), 0.2f);
}
void T_PuzzleSkill::F8_Comet(float f)
{
    if (psF8->m_pGameLayer->IsPaused())
        return;
    
    psF8->A8_callbackCnt++;
    if (psF8->A8_callbackCnt >= (int)psF8->A8_pos.size())
        psF8->m_pGameLayer->unschedule(schedule_selector(T_PuzzleSkill::F8_Comet));
    
    T_Effect* ef = psF8->m_pGameLayer->GetEffect();
    ef->Effect7_Callback_2(psF8->A8_callbackCnt-1, ef);
}

void T_PuzzleSkill::F8_Bomb(int queue_pos, std::vector<CCPoint> pos, int idx)
{
    SetQueuePos(queue_pos);
    F8_bombQueueIdx.push(idx);
    F8_bombQueuePos.push(pos);
    CCLog("F8 Bomb : (idx = %d)", idx);
    
    if (!F8_isFalling)
    {
        F8_isFalling = true;
        F8_Bomb_Real();
    }
}
void T_PuzzleSkill::F8_Bomb_Real()
{
    if (F8_bombQueueIdx.empty()) // 폭발할 게 아직 없다면 정지.
    {
        F8_isFalling = false;
        return;
    }
    
    int bombIdx = F8_bombQueueIdx.front();
    std::vector<CCPoint> pos = F8_bombQueuePos.front();
    F8_bombQueueIdx.pop();
    F8_bombQueuePos.pop();
    
    CCLog("F8 Bomb Real : (idx = %d)", bombIdx);
    F8_bombCallbackCnt[bombIdx] = 0;
    
    int itemPieceCnt = 0;
    int x, y, pieceType;
    for (int i = 0 ; i < pos.size() ; i++)
    {
        x = (int)pos[i].x;
        y = (int)pos[i].y;
        
        pieceType = m_pGameLayer->GetPuzzleP8Set()->GetType(x, y);
        if (!(pieceType >= PIECE_RED && pieceType <= PIECE_WHITE)) // 일반 피스가 아니면 폭발하지 않도록 한다.
        {
            F8_bombCallbackCnt[bombIdx]++;
            itemPieceCnt++;
            continue;
        }

        CCActionInterval* action = CCSequence::create( CCSpawn::create(CCScaleTo::create(0.05f, 1.5f), CCFadeOut::create(0.05f), NULL), CCCallFuncND::create(m_pGameLayer, callfuncND_selector(T_PuzzleSkill::F8_BombCallback), this), NULL);
        m_pGameLayer->GetSpriteP8(x, y)->setTag(bombIdx);
        m_pGameLayer->GetSpriteP8(x, y)->runAction(action);
    }
    
    // 폭발 예상 피스가 모두 일반 피스가 아니라면, callback을 호출하지 못하므로 즉시 턴을 종료한다.
    CCLog("%d %d", itemPieceCnt, (int)result_double_pos[bombIdx].size());
    if (itemPieceCnt == (int)result_double_pos[bombIdx].size())
    {
        F8_isFalling = false;
        
        F8_FinishCountUp();
        if (F8_IsFinished())
        {
            m_pGameLayer->GetEffect()->Effect7_Clear();
            m_pGameLayer->GoNextState(queuePos);
        }
    }
}

void T_PuzzleSkill::F8_BombCallback(CCNode* sender, void* pointer)
{
    T_PuzzleSkill* pss = (T_PuzzleSkill*)pointer;
    int idx = sender->getTag();
    
    pss->F8_bombCallbackCnt[idx]++;
    if (pss->F8_bombCallbackCnt[idx] >= pss->result_double_pos[idx].size())
    {
        int x, y, pieceType;
        for (int i = 0 ; i < pss->result_double_pos[idx].size() ; i++)
        {
            x = pss->result_double_pos[idx][i].x;
            y = pss->result_double_pos[idx][i].y;
            
            pieceType = pss->m_pGameLayer->GetPuzzleP8Set()->GetType(x, y);
            if (!(pieceType >= PIECE_RED && pieceType <= PIECE_WHITE)) // 일반 피스가 아니면 폭발하지 않도록 한다.
                continue;
            
            pss->m_pGameLayer->GetPuzzleP8Set()->RemoveChild(x, y);
            pss->m_pGameLayer->SetSpriteP8Null(x, y);
        }
        
        pss->m_pGameLayer->Falling(pss->queuePos);
    }
}

void T_PuzzleSkill::F8_FinishCountUp()
{
    F8_finishCnt++;
    CCLog("finish(%d), all(%d)", F8_finishCnt, (int)A8_pos.size());
    if (F8_finishCnt >= (int)A8_pos.size())
        F8_isActive = false;
}
bool T_PuzzleSkill::F8_IsFinished()
{
    return (F8_finishCnt >= (int)A8_pos.size());
}
/*void T_PuzzleSkill::F8_SetIsFalling(bool flag)
 {
 F8_isFalling = flag;
 }*/
bool T_PuzzleSkill::F8_IsActive()
{
    return F8_isActive;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////


std::vector<CCPoint> T_PuzzleSkill::GetResult()
{
    return result_pos;
}
std::vector< std::vector<CCPoint> > T_PuzzleSkill::GetResultDouble()
{
    return result_double_pos;
}
std::vector<CCPoint> T_PuzzleSkill::GetResultEnd()
{
    return result_pos_end;
}
void T_PuzzleSkill::ResultClear()
{
    result_pos.clear();
}


void T_PuzzleSkill::RemoveAllObjects()
{
    for (int i = 0 ; i < result_double_pos.size() ; i++)
        result_double_pos[i].clear();
    result_double_pos.clear();
    result_pos.clear();
    result_pos_end.clear();
    result_pos_temp.clear();
    
    A8_pos.clear();
    A2_pos.clear();
}

