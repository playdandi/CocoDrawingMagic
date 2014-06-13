#include "PuzzleSkill.h"
#include "Puzzle.h"

PuzzleSkill::PuzzleSkill()
{}

PuzzleSkill::~PuzzleSkill()
{}

void PuzzleSkill::SetGameLayer(Puzzle* layer)
{
    m_pGameLayer = layer;
}

Puzzle* PuzzleSkill::GetGameLayer()
{
    return m_pGameLayer;
}

Effect* PuzzleSkill::GetEffect()
{
    return m_pGameLayer->GetEffect();
}

void PuzzleSkill::Init(std::vector<int> num, std::vector<int> prob, std::vector<int> lv)
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
    
    // 게임 한 판에 시전되는 스킬별 횟수
    
    // 각 스킬 관련 변수들 초기화
    W3_addedScore = 0;
    //W4_addedCandy = 0;
    W7_isTimeSlowed = false;
    W8_isActive = false;
    E7_getPotion = false;
    
    for (int i = 0 ; i < 3; i++)
    {
        spiritShownCnt[i] = 0;
        isSpiritAlive[i] = false;
    }
}

int PuzzleSkill::GetSkillAppliedCount(int skillNum)
{
    return skillAppliedCnt[skillNum];
}

void PuzzleSkill::TrySkills(int pieceColor, int queue_pos)
{
    // Init
    // 10, 11번 스킬은 항상 true
    for (int i = 0 ; i < NUMOFSKILL ; i++)
    {
        skillApplied[queue_pos][i] = false;
        //if (i == 10 || i == 11)
        if (i == 10)
            skillApplied[queue_pos][i] = true;
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
            else if (i == 9) {
                if (pieceColor == PIECE_BLUE && m_pGameLayer->IsCycle(queue_pos))
                    Try(i, queue_pos);
            }
            else if (i == 17) {
                if (pieceColor == PIECE_GREEN && m_pGameLayer->IsCycle(queue_pos))
                    Try(i, queue_pos);
            }
            
            // 기본 1번 스킬
            else if (i == 0) {
                if (pieceColor == PIECE_RED)
                    Try(i, queue_pos);
            }
            else if (i == 8) {
                if (pieceColor == PIECE_BLUE)
                    Try(i, queue_pos);
            }
            else if (i == 16) {
                if (pieceColor == PIECE_GREEN)
                    Try(i, queue_pos);
            }
            
            // 6개이상 한번더 - 4B
            else if (i == 5) {
                if (pieceColor == PIECE_RED && m_pGameLayer->GetPiece8xy(false).size() >= 6)
                    Try(i, queue_pos);
            }
            else if (i == 13) {
                if (pieceColor == PIECE_BLUE && m_pGameLayer->GetPiece8xy(false).size() >= 6)
                    Try(i, queue_pos);
            }
            else if (i == 21) {
                if (pieceColor == PIECE_GREEN && m_pGameLayer->GetPiece8xy(false).size() >= 6)
                    Try(i, queue_pos);
            }
    
            // 10개 이상 제거 시 추가점수/추가별사탕 - F3, E4
            else if (i == 2) {
                if (pieceColor == PIECE_RED && m_pGameLayer->GetPiece8xy(false).size() >= 8)
                    Try(i, queue_pos);
            }
            /*else if (i == 19) {
                if (pieceColor == PIECE_GREEN && m_pGameLayer->GetPiece8xy(false).size() >= 10)
                    Try(i, queue_pos);
            }*/
            
            // 마지막 스킬
            else if (i == 7) {
                if (pieceColor == PIECE_RED && m_pGameLayer->GetPiece8xy(false).size() >= 10)
                    Try(i, queue_pos);
            }
            else if (i == 15) {
                if (pieceColor == PIECE_BLUE && m_pGameLayer->GetPiece8xy(false).size() >= 10)
                    Try(i, queue_pos);
            }
            else if (i == 23) {
                if (pieceColor == PIECE_GREEN && m_pGameLayer->GetPiece8xy(false).size() >= 10)
                    Try(i, queue_pos);
            }
            
            // 코코타임
            else if (i == 6) {
                if (pieceColor == PIECE_RED)
                    Try(i, queue_pos);
            }
            // 시간을 얼리다
            else if (i == 14) {
                if (pieceColor == PIECE_BLUE)
                    Try(i, queue_pos);
            }
            
            else
                Try(i, queue_pos);
        }
    }
}

void PuzzleSkill::Try(int skillNum, int queue_pos)
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

bool PuzzleSkill::IsApplied(int skillNum, int queue_pos)
{
    return skillApplied[queue_pos][skillNum];
}

void PuzzleSkill::SetQueuePos(int queue_pos)
{
    queuePos = queue_pos;
}

void PuzzleSkill::UpdateAppliedSkillCount(int skillNum)
{
    skillAppliedCnt[skillNum]++;
    
    // 미션 내용도 갱신
    m_pGameLayer->UpdateMissionCountBySkill(skillNum);
}

void PuzzleSkill::Invoke(int skillNum, int queue_pos)
{
    // 스킬 목록에 없거나, 있더라도 시전이 되지 않은 경우(확률 실패한 경우), 실행하지 않는다.
    if (!skillNumber[skillNum] || !skillApplied[queue_pos][skillNum])
        return;
    
    // coco animation
    m_pGameLayer->schedule(schedule_selector(Puzzle::CocoAnim), 0.04f);
    
    switch (skillNum)
    {
        case 0:  A1(skillNum, queue_pos); break;
        case 1:  A2(skillNum, queue_pos); break;
        case 2:  F3(skillNum, queue_pos); break;
        //case 3:  F4(skillNum); break;
        case 4:  F5(skillNum); break;
        case 5:  A6(skillNum, queue_pos); break;
        case 6:  F7(skillNum, queue_pos); break;
        case 7:  F8(skillNum, queue_pos); break;
            
        case 8:  A1(skillNum, queue_pos); break;
        case 9:  A2(skillNum, queue_pos); break;
        case 10: W3(skillNum); break;
        //case 11: W4(skillNum); break;
        case 12: W5(skillNum); break;
        case 13: A6(skillNum, queue_pos); break;
        case 14: W7(skillNum); break;
        case 15: W8(skillNum, queue_pos); break;
            
        case 16: A1(skillNum, queue_pos); break;
        case 17: A2(skillNum, queue_pos); break;
        case 18: E3(skillNum); break;
        //case 19: E4(skillNum, queue_pos); break;
        case 20: E5(skillNum); break;
        case 21: A6(skillNum, queue_pos); break;
        case 22: E7(skillNum); break;
        case 23: E8(skillNum, queue_pos); break;
            
        case 24: M1(skillNum); break;
        case 25: M2(skillNum); break;
        case 26: M3(skillNum); break;
        case 27: M4(skillNum); break;
        case 28: M5(skillNum); break;
    }
}

void PuzzleSkill::A1(int num, int queue_pos)
{
    // 마법불꽃, 은은한 달빛, 대지의 숨결 - 각 색깔의 피스 제거 시, 추가점수
  
    UpdateAppliedSkillCount(num);

    // 추가점수 = 스킬레벨 * 한붓그리기 개수 * 30 (점)
    A1_addedScore = 30*skillLevel[num]*m_pGameLayer->GetPiece8xy(true).size();
    m_pGameLayer->UpdateScore(1, A1_addedScore);
    
    // 이펙트
    m_pGameLayer->GetEffect()->PlayEffect_MagicCircle(num);
    m_pGameLayer->GetEffect()->PlayEffect_SkillIcon(num);
    m_pGameLayer->PlayEffect(num, queue_pos);
    
    // 사이클이 발동된 상태면, 사이클 이펙트도 같이 보여준다. (태양열, 파도, 클로버 그림 띄우는 것)
    if (IsApplied(num+1, queue_pos) && m_pGameLayer->IsCycle(queue_pos))
        m_pGameLayer->PlayEffect(-(num+1), queue_pos);
}

void PuzzleSkill::A2(int num, int queue_pos)
{
    // 불꽃송이, 파도타기, 땅울림 - 각 색깔의 피스를 사이클로 제거하면 스킬 레벨에 비례하여 정해진 방식대로 주변부 터뜨리기
    // (여기서는 주변부의 위치만 구한다)
    // 불2 : 스킬레벨 수만큼 사이클의 주변부를 동시에 터뜨린다.
    // 물2 : 스킬레벨 수만큼 사이클이 끝나는 방향으로 파도타듯이 터뜨린다.
    // 땅2 : 스킬레벨 수만큼 완전히 랜덤하게 터뜨린다.
    
    UpdateAppliedSkillCount(num);
    
    // 변수 초기화
    A2_pos.clear();
    int x, y;
    int xy[COLUMN_COUNT][ROW_COUNT];
    
    for (int x = 0 ; x < COLUMN_COUNT ; x++)
        for (int y = 0 ; y < ROW_COUNT ; y++)
            xy[x][y] = 0;
    
    std::vector<CCPoint> pos;
    if (m_pGameLayer->IsRoundInFeverTime(true))
        pos = m_pGameLayer->GetPosForFeverTime(true);
    else
        pos = m_pGameLayer->GetPiece8xy(true);
    for (int i = 0 ; i < pos.size() ; i++)
        xy[(int)pos[i].x][(int)pos[i].y] = -1;
    
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
    
    else if (num == 9) // 물2 (파도타기)
    {
        // 사이클 끝난 방향으로 파도타기 (길이가 모자라면 반대편에서 계속 진행)
        CCPoint last = pos[0];
        CCPoint before = pos[pos.size()-1];
        CCPoint delta = ccp((int)last.x-(int)before.x, (int)last.y-(int)before.y);
        
        CCPoint lastPos = m_pGameLayer->SetPiece8Position((int)pos[0].x, (int)pos[0].y);
        CCPoint beforePos = m_pGameLayer->SetPiece8Position((int)pos[pos.size()-1].x, (int)pos[pos.size()-1].y);
        CCPoint deltaPos = ccp((int)lastPos.x-(int)beforePos.x, (int)lastPos.y-(int)beforePos.y);
        
        // 예외처리 : 시작점, 직전점의 변화량 값을 array의 처음에 넣는다. (파티클 움직임을 위해)
        A2_pos.push_back(deltaPos);
        
        int size = skillLevel[num];
        int cnt = 0;
        int x = (int)pos[0].x + (int)delta.x;
        int y = (int)pos[0].y + (int)delta.y;

        while (cnt < size)
        {
            if ((x == 0 && y == 0) || (x == 0 && y == ROW_COUNT-1) ||
                (x == COLUMN_COUNT-1 && y == 0) || (x == COLUMN_COUNT-1 && y == ROW_COUNT-1))
                break;
            if (x < 0 || y < 0 || x >= COLUMN_COUNT || y >= ROW_COUNT)
                break;
            
            A2_pos.push_back(ccp(x, y));
            x += (int)delta.x;
            y += (int)delta.y;
        }
    }
    
    else // 땅2
    {
        // 완전히 랜덤하게 개수 뽑기
        int size = std::min(skillLevel[num], (int)pos.size());
        int cnt = 0;
        while(cnt < size)
        {
            x = rand() % 7;
            y = rand() % 7;
            if ((x == 0 && y == 0) || (x == 0 && y == ROW_COUNT-1) ||
                (x == COLUMN_COUNT-1 && y == 0) || (x == COLUMN_COUNT-1 && y == ROW_COUNT-1))
                continue;
            if (xy[x][y] == 0)
            {
                xy[x][y] = 1;
                cnt++;
                A2_pos.push_back(ccp(x, y));
            }
        }
    }
    
    // 이펙트 실행 (태양/달/파도 그림)
    m_pGameLayer->GetEffect()->PlayEffect_MagicCircle(num);
    m_pGameLayer->GetEffect()->PlayEffect_SkillIcon(num);
    m_pGameLayer->PlayEffect(num, queue_pos);
    
    // 폭파 실행
    if (num != 9) // 물 사이클 스킬은 이펙트와 함께 발동시켜야 하므로, 여기서 Bomb을 실행하지 않는다.
        m_pGameLayer->Bomb(queue_pos, A2_pos);
}
std::vector<CCPoint> PuzzleSkill::A2GetPos()
{
    return A2_pos;
}
void PuzzleSkill::A2Clear()
{
    A2_pos.clear();
}


void PuzzleSkill::F3(int num, int queue_pos)
{
    // 뜨거운 것이 좋아 - 10개 이상 피스 제거 시 추가 점수
    
    UpdateAppliedSkillCount(num);
    
    F3_addedScore = skillLevel[num]*7000; // 나중에 보고 밸런스 조정
    m_pGameLayer->UpdateScore(1, F3_addedScore);
    
    // 이펙트 ('+' 그림)
    m_pGameLayer->GetEffect()->PlayEffect_MagicCircle(num);
    m_pGameLayer->GetEffect()->PlayEffect_SkillIcon(num);
    m_pGameLayer->PlayEffect(num, queue_pos);
    
    // sound
    m_pGameLayer->GetSound()->PlaySkillSound(num);
    
    // '빛나는 하얀구체'를 마지막 피스 중앙에 보여주고 score칸으로 움직이는 action 발동 ???
}

/*
void PuzzleSkill::F4(int num)
{
    // 타오르는 마법진 - Magic Time(MT) 때 마법진을 2번 터뜨린다.
    
    UpdateAppliedSkillCount(num);
    
    F4_isDoubledMT = false;
    int prob = rand()%100;
    if (prob < skillProb[num])
    {
        F4_isDoubledMT = true;
    }
}
*/

void PuzzleSkill::SpiritTry(int type, int queue_pos)
{
    // 여기선 queue_pos가 그렇게 의미없다. (무조건 0으로 해 놓았음)
    
    int skillNum = 4+type*8;
    
    // 정령을 생성하는지 결정
    skillApplied[queue_pos][skillNum] = false;
    int prob = rand()%100;
    if (prob < skillProb[skillNum])
        skillApplied[queue_pos][skillNum] = true;
    
    // 스킬 목록에 없거나, 있더라도 시전이 되지 않은 경우(확률 실패한 경우), 실행하지 않는다.
    if (!skillNumber[skillNum] || !skillApplied[queue_pos][skillNum])
        return;
    
    // 이미 출현 회수를 초과했거나 아직 정령이 살아있는 경우도 무시한다.
    if (spiritShownCnt[type] >= skillLevel[skillNum] || isSpiritAlive[type])
        return;
    
    spiritShownCnt[type]++;
    isSpiritAlive[type] = true;
    
    // effect 발생 (정령 생성)
    GetEffect()->SetSpirit(type);
}

bool PuzzleSkill::IsSpiritAlive(int type)
{
    return isSpiritAlive[type];
}

void PuzzleSkill::F5(int num)
{
    // 위험한 불장난 - 모서리에 나타나는 불의 정령을 선택하면 모든 붉은 피스를 정중앙으로 모아준다.
    
    result_pos.clear();
    int type;
    int cnt = 0;
    
    // 모든 붉은 피스 위치를 구한다.
    for (int x = 0 ; x < COLUMN_COUNT ; x++)
    {
        for (int y = 0 ; y < ROW_COUNT ; y++)
        {
            // 네 모서리에 위치한 존재하지 않는 부분
            if ((x == 0 && y == 0) || (x == 0 && y == ROW_COUNT-1) ||
                (x == COLUMN_COUNT-1 && y == 0) || (x == COLUMN_COUNT-1 && y == ROW_COUNT-1))
                continue;
            type = m_pGameLayer->GetPuzzleP8Set()->GetType(x, y);
            if (type == PIECE_RED)
                result_pos.push_back(ccp(x, y));
            if (type == PIECE_RED || type == PIECE_BLUE)
                cnt++;
        }
    }
    
    // 불/물 피스만 존재하는 경우, 정령을 발동시키지 않는다. (즉, 모든 불 피스 주변을 모두 물 피스가 감싸고 있을 때)
    bool flag = false;
    int x, y;
    for (int i = 0 ; i < result_pos.size() ; i++)
    {
        x = (int)result_pos[i].x;
        y = (int)result_pos[i].y;
        for (int xx = x-1; xx <= x+1; xx++)
        {
            for (int yy = y-1; yy <= y+1; yy++)
            {
                // 예외처리
                if (xx < 0 || xx > COLUMN_COUNT-1 || yy < 0 || yy > ROW_COUNT-1)
                    continue;
                if ((xx == 0 && yy == 0) || (xx == 0 && yy == ROW_COUNT-1) ||
                    (xx == COLUMN_COUNT-1 && yy == 0) || (xx == COLUMN_COUNT-1 && yy == ROW_COUNT-1))
                    continue;
                type = m_pGameLayer->GetPuzzleP8Set()->GetType(xx, yy);
                if (type == PIECE_RED || type == PIECE_BLUE)
                    continue;
                
                flag = true;
            }
        }
        if (flag)
            break;
    }
    if (!flag)
    {
        this->m_pGameLayer->SetSpiritTouch(false);
        return;
    }
    
    // 랜덤하게 선택된 붉은 피스 하나의 주변 8개를 불로 바꾼다. (단, 이미 불이거나 물인 경우는 제외한다)
    int p;
    std::vector<CCPoint> pos;
    while (1)
    {
        pos.clear();
        p = rand() % (int)result_pos.size();
        x = (int)result_pos[p].x;
        y = (int)result_pos[p].y;
        
        for (int xx = x-1; xx <= x+1; xx++)
        {
            for (int yy = y-1; yy <= y+1; yy++)
            {
                // 예외처리
                if (xx < 0 || xx > COLUMN_COUNT-1 || yy < 0 || yy > ROW_COUNT-1)
                    continue;
                if ((xx == 0 && yy == 0) || (xx == 0 && yy == ROW_COUNT-1) ||
                    (xx == COLUMN_COUNT-1 && yy == 0) || (xx == COLUMN_COUNT-1 && yy == ROW_COUNT-1))
                    continue;
                type = m_pGameLayer->GetPuzzleP8Set()->GetType(xx, yy);
                if (type == PIECE_RED || type == PIECE_BLUE)
                    continue;
                
                pos.push_back(ccp(xx, yy));
            }
        }
        if ((int)pos.size() > 0)
            break;
    }
    
    int centerX = x;
    int centerY = y;
    
    F5_callbackCnt = 0;
    result_pos = pos;
    pos.clear();
    for (int i = 0 ; i < result_pos.size() ; i++)
    {
        x = (int)result_pos[i].x;
        y = (int)result_pos[i].y;
        
        CCFiniteTimeAction* action = CCSequence::create(CCScaleTo::create(0.15f, 0.0f),
            CCCallFuncND::create(m_pGameLayer, callfuncND_selector(PuzzleSkill::F5_Callback), this), NULL);
        m_pGameLayer->GetPuzzleP8Set()->GetSprite(x, y)->runAction(action);
    }
    
    // sound
    m_pGameLayer->GetSound()->PlaySkillSound(num);
    
    // 이펙트 출현 + 정령을 화면에서 없애기
    GetEffect()->SpiritEffect(0, centerX, centerY);
    GetEffect()->RemoveSpirit(0);
    isSpiritAlive[0] = false;
    
    UpdateAppliedSkillCount(num);
}
void PuzzleSkill::F5_Callback(CCNode* sender, void* data)
{
    PuzzleSkill* ps = (PuzzleSkill*)data;
    
    ps->F5_callbackCnt++;
    
    if (ps->F5_callbackCnt == (int)ps->result_pos.size())
    {
        int x, y;
        for (int i = 0 ; i < ps->result_pos.size() ; i++)
        {
            x = (int)ps->result_pos[i].x;
            y = (int)ps->result_pos[i].y;
            
            // 기존 피스를 없앤다.
            ps->m_pGameLayer->GetPuzzleP8Set()->RemoveChild(x, y);
            ps->m_pGameLayer->SetSpriteP8Null(x, y);
            
            // 그 위치에 새로운 blue 피스를 만든다.
            ps->m_pGameLayer->GetPuzzleP8Set()->CreatePiece(x, y, PIECE_RED);
            ps->m_pGameLayer->GetPuzzleP8Set()->AddChild(x, y);
            ps->m_pGameLayer->GetPuzzleP8Set()->GetSprite(x, y)->setPosition(ps->m_pGameLayer->SetPiece8Position(x, y));
            
            // scale 조정해서 action 준비한다.
            float scale = ps->m_pGameLayer->GetPuzzleP8Set()->GetSprite(x, y)->getScale();
            ps->m_pGameLayer->GetPuzzleP8Set()->GetSprite(x, y)->setScale(0.0f);
            
            // action !
            CCFiniteTimeAction* action = CCSequence::create(CCScaleTo::create(0.15f, scale),
                    CCCallFuncND::create(ps->m_pGameLayer, callfuncND_selector(PuzzleSkill::F5_Callback), ps), NULL);
            ps->m_pGameLayer->GetPuzzleP8Set()->GetSprite(x, y)->runAction(action);
        }
    }
    else if (ps->F5_callbackCnt == 2*(int)ps->result_pos.size())
    {
        //CCLog("F5 callback : dia delete");
        // 4각형 피스들 갱신
        for (int x = 1; x < COLUMN_COUNT ; x++)
        {
            for (int y = 1 ; y < ROW_COUNT ; y++)
            {
                if (ps->m_pGameLayer->GetPuzzleP4Set()->GetObject(x, y) != NULL)
                    ps->m_pGameLayer->GetPuzzleP4Set()->RemoveChild(x, y);
                ps->m_pGameLayer->GetPuzzleP4Set()->CreatePiece(x, y, ps->m_pGameLayer->GetPuzzleP4Set()->GetType(x, y));
                ps->m_pGameLayer->GetPuzzleP4Set()->AddChild(x, y);
            }
        }
        
        ps->m_pGameLayer->SetSpiritTouch(false);
    }
}


void PuzzleSkill::A6(int num, int queue_pos)
{
    // 불꽃놀이, 얼음비, 땅의 신비 - 각자의 피스 제거 시 (6개 이상) 일정 확률로 그 위치를 한 번 더 터뜨리기
    
    UpdateAppliedSkillCount(num);
    
    // 폭파
    if (num == 5) // 불꽃놀이는 연달아 터지도록 한다.
    {
        // 순서 정하기
        F6_order.clear();
        int k;
        while (result_pos.size() > 0)
        {
            k = rand() % (int)result_pos.size();
            F6_order.push_back(result_pos[k]);
            
            for (int i = k+1; i < result_pos.size() ; i++)
                result_pos[i-1] = result_pos[i];
            result_pos.pop_back();
        }
        result_pos = F6_order;
        
        F6_callbackCnt = 0;
        F6_Callback(NULL, this);
        SetQueuePos(queue_pos);
        
        // 이펙트 실행
        m_pGameLayer->GetEffect()->PlayEffect_MagicCircle(num);
        m_pGameLayer->GetEffect()->PlayEffect_SkillIcon(num);
    }
    else
    {
        m_pGameLayer->Bomb(queue_pos, result_pos);
        
        // 이펙트 실행
        m_pGameLayer->GetEffect()->PlayEffect_MagicCircle(num);
        m_pGameLayer->GetEffect()->PlayEffect_SkillIcon(num);
        m_pGameLayer->PlayEffect(num, queue_pos);
        
        // 사운드
        m_pGameLayer->GetSound()->PlaySkillSound(num);
    }
}
void PuzzleSkill::F6_Callback(CCNode* sender, void *p)
{
    PuzzleSkill* pss = (PuzzleSkill*)p;
    int x, y;

    pss->F6_callbackCnt++;
    
    if (pss->F6_callbackCnt <= (int)pss->result_pos.size())
    {
        x = pss->result_pos[pss->F6_callbackCnt-1].x;
        y = pss->result_pos[pss->F6_callbackCnt-1].y;
        
        // 피스 폭파 개수 갱신
        pss->m_pGameLayer->UpdatePieceBombCnt(pss->m_pGameLayer->GetPuzzleP8Set()->GetType(x, y), 1);
        
        // 이펙트+사운드
        pss->m_pGameLayer->GetEffect()->PlayEffect_5(x, y);
        pss->m_pGameLayer->GetSound()->PlaySkillSound(5);
        
        CCActionInterval* action = CCSequence::create( CCSpawn::create(CCScaleTo::create(0.1f, 1.5f), CCFadeOut::create(0.1f), NULL), CCCallFuncND::create(pss->m_pGameLayer, callfuncND_selector(PuzzleSkill::F6_Callback), pss), NULL);
        pss->m_pGameLayer->GetSpriteP8(x, y)->runAction(action);
    }
    else
    {
        for (int i = 0 ; i < pss->result_pos.size() ; i++)
        {
            x = pss->result_pos[i].x;
            y = pss->result_pos[i].y;
            pss->m_pGameLayer->GetPuzzleP8Set()->RemoveChild(x, y);
            pss->m_pGameLayer->SetSpriteP8Null(x, y);
        }
        pss->result_pos.clear();
        
        pss->m_pGameLayer->Falling(pss->queuePos);
    }
    
}


void PuzzleSkill::F7(int num, int queue_pos)
{
    // 코코 타임 : Fever Time 게이지를 더 빨리 채운다. (10~20% 더 빠르게)
    
    UpdateAppliedSkillCount(num);
}


static PuzzleSkill* psF8;

void PuzzleSkill::F8(int num, int queue_pos)
{
    // 붉은 용의 숨결
    F8_isActive = true;
    
    UpdateAppliedSkillCount(num);
    
    psF8 = this;
    
    F8_finishCnt = 0;
    A8_pos.clear();
    for (int x = 0 ; x < COLUMN_COUNT ; x++)
        for (int y = 0 ; y < ROW_COUNT ; y++)
            F8_check[x][y] = 0;
    F8_check[0][0] = F8_check[0][ROW_COUNT-1] = F8_check[COLUMN_COUNT-1][0] = F8_check[COLUMN_COUNT-1][ROW_COUNT-1] = -1;
    
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
            
            //F8_check[x][y]++;
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
    
    CCLog("용의 숨결 혜성 수 : %d", (int)A8_pos.size());
    m_pGameLayer->GetSound()->PlaySkillSound(num);
    m_pGameLayer->GetEffect()->PlayEffect_7(result_double_pos, A8_pos, queue_pos);
}

void PuzzleSkill::F8Check(int x, int y, int idx)
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

void PuzzleSkill::F8_Timer()
{
    A8_callbackCnt = 0;
    m_pGameLayer->schedule(schedule_selector(PuzzleSkill::F8_Comet), 0.2f);
}
void PuzzleSkill::F8_Comet(float f)
{
    if (psF8->m_pGameLayer->IsPaused())
        return;
    
    psF8->A8_callbackCnt++;
    if (psF8->A8_callbackCnt >= (int)psF8->A8_pos.size())
        psF8->m_pGameLayer->unschedule(schedule_selector(PuzzleSkill::F8_Comet));
    
    //CCLog("timer callback cnt : %d", psF8->A8_callbackCnt);
    Effect* ef = psF8->m_pGameLayer->GetEffect();
    ef->Effect7_Callback_2(psF8->A8_callbackCnt-1, ef);
}

void PuzzleSkill::F8_Bomb(int queue_pos, std::vector<CCPoint> pos, int idx)
{
    SetQueuePos(queue_pos);
    F8_bombCallbackCnt[idx] = 0;
    CCLog("F8 Bomb : (idx = %d)", idx);
    int x, y;
    for (int i = 0 ; i < pos.size() ; i++)
    {
        x = (int)pos[i].x;
        y = (int)pos[i].y;
        
        m_pGameLayer->UpdatePieceBombCnt(m_pGameLayer->GetPuzzleP8Set()->GetType(x, y), 1);
        
        CCActionInterval* action = CCSequence::create( CCSpawn::create(CCScaleTo::create(0.05f, 1.5f), CCFadeOut::create(0.05f), NULL), CCCallFuncND::create(m_pGameLayer, callfuncND_selector(PuzzleSkill::F8_BombCallback), this), NULL);
        m_pGameLayer->GetSpriteP8(x, y)->setTag(idx);
        m_pGameLayer->GetSpriteP8(x, y)->runAction(action);
    }
}

void PuzzleSkill::F8_BombCallback(CCNode* sender, void* pointer)
{
    PuzzleSkill* pss = (PuzzleSkill*)pointer;
    int idx = sender->getTag();
    
    pss->F8_bombCallbackCnt[idx]++;
    if (pss->F8_bombCallbackCnt[idx] >= pss->result_double_pos[idx].size())
    {
        int x, y;
        for (int i = 0 ; i < pss->result_double_pos[idx].size() ; i++)
        {
            x = pss->result_double_pos[idx][i].x;
            y = pss->result_double_pos[idx][i].y;
            pss->m_pGameLayer->GetPuzzleP8Set()->RemoveChild(x, y);
            pss->m_pGameLayer->SetSpriteP8Null(x, y);
        }
        
        pss->m_pGameLayer->FallingQueuePushAndFalling(pss->queuePos);
    }
}

void PuzzleSkill::F8_FinishCountUp()
{
    F8_finishCnt++;
    CCLog("finish(%d), all(%d)", F8_finishCnt, (int)A8_pos.size());
    if (F8_finishCnt >= (int)A8_pos.size())
        F8_isActive = false;
}
bool PuzzleSkill::F8_IsFinished()
{
    return (F8_finishCnt >= (int)A8_pos.size());
}
bool PuzzleSkill::F8_IsActive()
{
    return F8_isActive;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////


void PuzzleSkill::W3(int num)
{
    // 한방울 한방울 - 콤보에 비례한 추가 점수
    
    UpdateAppliedSkillCount(num);
    
    W3_addedScore = pow(m_pGameLayer->GetCombo(), 0.9f) * (skillLevel[num]*100 + 550);
    
    // sound
    m_pGameLayer->GetSound()->PlaySkillSound(num);

    // 이펙트 실행
    m_pGameLayer->GetEffect()->PlayEffect_MagicCircle(num);
    m_pGameLayer->GetEffect()->PlayEffect_SkillIcon(num);
    m_pGameLayer->PlayEffect(num, NULL);
}
int PuzzleSkill::W3GetScore()
{
    return W3_addedScore;
}

/*
void PuzzleSkill::W4(int num)
{
    // 바다 속 진주 - 콤보에 비례한 추가 별사탕
    
    UpdateAppliedSkillCount(num);
    
    W4_addedCandy = pow(m_pGameLayer->GetCombo(), 0.8f) * (skillLevel[num]*0.3f + 0.7f);
    
    // sound
    m_pGameLayer->GetSound()->PlaySkillSound(num);
    
    // 이펙트 실행
    m_pGameLayer->GetEffect()->PlayEffect_MagicCircle(num);
    m_pGameLayer->GetEffect()->PlayEffect_SkillIcon(num);
    m_pGameLayer->PlayEffect(num, NULL);
}
int PuzzleSkill::W4GetCandy()
{
    return W4_addedCandy;
}
*/

void PuzzleSkill::W5(int num)
{
    // 끝없는 물결 - 오른쪽상단 터치 시 일정 개수만큼 white/yellow -> blue로 바꾸기
    
    result_pos.clear();
    
    std::vector<CCPoint> pos;
    for (int x = 0 ; x < COLUMN_COUNT ; x++)
    {
        for (int y = 0 ; y < ROW_COUNT ; y++)
        {
            // 네 모서리에 위치한 존재하지 않는 부분
            if ((x == 0 && y == 0) || (x == 0 && y == ROW_COUNT-1) ||
                (x == COLUMN_COUNT-1 && y == 0) || (x == COLUMN_COUNT-1 && y == ROW_COUNT-1))
                continue;
            
            // 노란색과 하얀색 피스의 위치를 찾는다.
            if (m_pGameLayer->GetPuzzleP8Set()->GetType(x, y) == PIECE_WHITE ||
                m_pGameLayer->GetPuzzleP8Set()->GetType(x, y) == PIECE_YELLOW)
            {
                pos.push_back(ccp(x, y));
            }
        }
    }
    
    // 노란색, 하얀색 피스가 하나도 없으면 실행하지 않는다.
    if (pos.size() == 0)
    {
        m_pGameLayer->SetSpiritTouch(false);
        return;
    }
    
    // 모든 노란색/하얀색 피스 중 랜덤하게 스킬레벨에 비례한 개수만큼 선택한다.
    if (skillLevel[num] >= pos.size())
    {
        result_pos = pos;
    }
    else
    {
        int k;
        while(result_pos.size() < skillLevel[num])
        {
            k = rand()%pos.size();
            if ((int)pos[k].x == -1 && (int)pos[k].y == -1)
                continue;
            
            result_pos.push_back(pos[k]);
            pos[k] = ccp(-1, -1);
        }
    }
    
    // 노란색, 하얀색 피스를 파란색으로 바꾸는 액션 실행한다.
    W5_callbackCnt = 0;
    int x, y;
    for (int i = 0 ; i < result_pos.size() ; i++)
    {
        x = (int)result_pos[i].x;
        y = (int)result_pos[i].y;
        
        CCFiniteTimeAction* action = CCSequence::create(CCScaleTo::create(0.15f, 0.0f),
                            CCCallFuncND::create(m_pGameLayer, callfuncND_selector(PuzzleSkill::W5_Callback), this), NULL);
        m_pGameLayer->GetPuzzleP8Set()->GetSprite(x, y)->runAction(action);
    }
    
    // sound
    m_pGameLayer->GetSound()->PlaySkillSound(num);
    
    // 이펙트 출현 + 정령을 화면에서 없애기
    GetEffect()->SpiritEffect(1);
    GetEffect()->RemoveSpirit(1);
    isSpiritAlive[1] = false;
    
    UpdateAppliedSkillCount(num);
}
void PuzzleSkill::W5_Callback(CCNode* sender, void* data)
{
    PuzzleSkill* ps = (PuzzleSkill*)data;

    ps->W5_callbackCnt++;
    
    if (ps->W5_callbackCnt == (int)ps->result_pos.size())
    {
        int x, y;
        for (int i = 0 ; i < ps->result_pos.size() ; i++)
        {
            x = (int)ps->result_pos[i].x;
            y = (int)ps->result_pos[i].y;
            
            // 기존 피스를 없앤다.
            ps->m_pGameLayer->GetPuzzleP8Set()->RemoveChild(x, y);
            ps->m_pGameLayer->SetSpriteP8Null(x, y);
            
            // 그 위치에 새로운 blue 피스를 만든다.
            ps->m_pGameLayer->GetPuzzleP8Set()->CreatePiece(x, y, PIECE_BLUE);
            ps->m_pGameLayer->GetPuzzleP8Set()->AddChild(x, y);
            ps->m_pGameLayer->GetPuzzleP8Set()->GetSprite(x, y)->setPosition(ps->m_pGameLayer->SetPiece8Position(x, y));

            // scale 조정해서 action 준비한다.
            float scale = ps->m_pGameLayer->GetPuzzleP8Set()->GetSprite(x, y)->getScale();
            ps->m_pGameLayer->GetPuzzleP8Set()->GetSprite(x, y)->setScale(0.0f);

            // action !
            CCFiniteTimeAction* action = CCSequence::create(CCScaleTo::create(0.15f, scale),
                        CCCallFuncND::create(ps->m_pGameLayer, callfuncND_selector(PuzzleSkill::W5_Callback), ps), NULL);
            ps->m_pGameLayer->GetPuzzleP8Set()->GetSprite(x, y)->runAction(action);
        }
    }
    else if (ps->W5_callbackCnt == 2*(int)ps->result_pos.size())
    {
        // 4각형 피스들 갱신
        for (int x = 1; x < COLUMN_COUNT ; x++)
        {
            for (int y = 1 ; y < ROW_COUNT ; y++)
            {
                if (ps->m_pGameLayer->GetPuzzleP4Set()->GetObject(x, y) != NULL)
                    ps->m_pGameLayer->GetPuzzleP4Set()->RemoveChild(x, y);
                ps->m_pGameLayer->GetPuzzleP4Set()->CreatePiece(x, y, ps->m_pGameLayer->GetPuzzleP4Set()->GetType(x, y));
                ps->m_pGameLayer->GetPuzzleP4Set()->AddChild(x, y);
            }
        }
        
        ps->m_pGameLayer->SetSpiritTouch(false);
    }
}

void PuzzleSkill::W7(int num)
{
    // 시간을 얼리다 - 5초 동안 시간을 2배속 늦춘다.
    if (!W7_isTimeSlowed)
    {
        UpdateAppliedSkillCount(num);
        
        W7_isTimeSlowed = true;
        W7_RemainTime = 5000;
        
        // 이펙트 실행
        m_pGameLayer->GetEffect()->PlayEffect_SkillIcon(num);
        m_pGameLayer->PlayEffect(num, NULL);
        
        // iced-bar sound
        m_pGameLayer->GetSound()->PlaySkillSound(num);
        m_pGameLayer->GetSound()->PlayVoice(VOICE_STOPTIME);
    }
}
void PuzzleSkill::W7SetTime(int time)
{
    W7_RemainTime = time;
}
int PuzzleSkill::W7GetTime()
{
    return W7_RemainTime;
}
void PuzzleSkill::W7SetVar()
{
    W7_isTimeSlowed = false;
}
bool PuzzleSkill::W7GetVar()
{
    return W7_isTimeSlowed;
}

static PuzzleSkill* ps;
void PuzzleSkill::W8(int num, int queue_pos)
{
    // 여신의 은총 : 물 10개 터뜨리면 발동
    // 시간이 멈추고, 물 피스 드래그하면 폭발, 그 외에는 물피스로 교체. (물피스 터뜨리면 여신 지속시간 증가)
    if (W8_isActive)
        return;
    
    UpdateAppliedSkillCount(num);
    
    ps = this;
    
    ps->W8_isActive = true;
    ps->W8_remainTime = 10 * 1000;
    ps->W8_accelTime = 0;
    ps->W8_accel = 1;
    ps->W8_isChanging = 0;
    ps->W8_isLastChange = false;
    ps->W8_isLastChangeWaiting = false;
    ps->W8_scheduleDone = false;
    ps->W8_bombFirst = false;
    
    ps->SetQueuePos(queue_pos);
    
    //ps->m_pGameLayer->GetSound()->PlayVoice(VOICE_GODDESS);
    ps->m_pGameLayer->PlayEffect(num, queue_pos);
    
    ps->m_pGameLayer->schedule(schedule_selector(PuzzleSkill::W8_Timer), 0.1f);
    ps->m_pGameLayer->schedule(schedule_selector(PuzzleSkill::W8_AccelTimer), 0.1f);
}
void PuzzleSkill::W8_Invoke(std::vector<CCPoint> pos, int queue_pos)
{
    CCPoint p = pos[0];
    if (m_pGameLayer->GetPuzzleP8Set()->GetType(p.x, p.y) == PIECE_BLUE)
    {
        CCLog("W8_invoke : bomb start");
        if (W8_remainTime <= 0)
            return;
        W8_bombFirst = true;
        //W8_remainTime += 2*1000; // 2초 추가
        //if (m_pGameLayer->IsFeverTime())
        //    m_pGameLayer->AddPiecesByFeverTime(pos, queue_pos);
        W8_remainTime += (int)pos.size() * 500; // 개수*0.5초 만큼 추가
        W8_remainTime = std::min(W8_remainTime, 10*1000);
        
        m_pGameLayer->Lock(queue_pos);
        m_pGameLayer->GetEffect()->Effect15_Bomb(pos, m_pGameLayer->GetEffect());
        m_pGameLayer->Bomb(queue_pos, pos); // 사운드는 이 함수 안에서 처리
        
        m_pGameLayer->GetEffect()->AddOrbMaxParticle((int)pos.size() * 5);
    }
    else
    {
        if (W8_isChanging == 0) // 바꾸는 건 기존에 바꾸는 액션이 끝나야 가능하다.
        {
            CCLog("W8_invoke : other->blue change");
            m_pGameLayer->SkillSemaphoreUpdate(1);
            W8_isChanging++;
            W8_callbackCnt = 0;
            int x, y;
            
            // sound
            m_pGameLayer->GetSound()->PlayDesignatedSound(152);

            // 파란색 piece로 바꾸는 액션 실행한다.
            result_pos = pos;
            for (int i = 0 ; i < result_pos.size() ; i++)
            {
                x = (int)result_pos[i].x;
                y = (int)result_pos[i].y;
                
                // lock을 건다.
                //m_pGameLayer->LockEach(x, y);
                
                // 없애기
                CCFiniteTimeAction* action = CCSequence::create(CCScaleTo::create(0.08f, 0.0f), CCCallFuncND::create(m_pGameLayer, callfuncND_selector(PuzzleSkill::W8_Callback), this), NULL);
                m_pGameLayer->GetPuzzleP8Set()->GetSprite(x, y)->runAction(action);
            }
        }
    }
}
void PuzzleSkill::W8_Callback(CCNode* sender, void* data)
{
    PuzzleSkill* pss = (PuzzleSkill*)data;
    
    pss->W8_callbackCnt++;
    
    if (pss->W8_callbackCnt == (int)pss->result_pos.size())
    {
        int x, y, type;
        for (int i = 0 ; i < pss->result_pos.size() ; i++)
        {
            x = (int)pss->result_pos[i].x;
            y = (int)pss->result_pos[i].y;
            
            // 기존 피스를 없앤다.
            pss->m_pGameLayer->GetPuzzleP8Set()->RemoveChild(x, y);
            pss->m_pGameLayer->SetSpriteP8Null(x, y);
            
            // 그 위치에 새로운 blue 피스를 만든다.
            if (!pss->W8_isLastChange)
            {
                pss->m_pGameLayer->GetPuzzleP8Set()->CreatePiece(x, y, PIECE_BLUE);
            }
            else
            {
                type = rand() % (TYPE_COUNT - pss->m_pGameLayer->IsItemClear());
                if (type == PIECE_BLUE) // yellow piece가 될 확률이 다른 것보다 높지만, 그냥 이렇게 가자.
                    type = PIECE_YELLOW;
                pss->m_pGameLayer->GetPuzzleP8Set()->CreatePiece(x, y, type);
            }
            pss->m_pGameLayer->GetPuzzleP8Set()->AddChild(x, y);
            pss->m_pGameLayer->GetPuzzleP8Set()->GetSprite(x, y)->setPosition( pss->m_pGameLayer->SetPiece8Position(x, y) );
            
            // scale 조정해서 action 준비한다.
            float scale = pss->m_pGameLayer->GetBoardSize() / (float)1076;
            pss->m_pGameLayer->GetPuzzleP8Set()->GetSprite(x, y)->setScale(0.0f);
            
            // action !
            CCFiniteTimeAction* action = CCSequence::create(CCScaleTo::create(0.08f, scale), CCCallFuncND::create(pss->m_pGameLayer, callfuncND_selector(PuzzleSkill::W8_Callback), pss), NULL);
            pss->m_pGameLayer->GetPuzzleP8Set()->GetSprite(x, y)->runAction(action);
        }
    }
    else if (pss->W8_callbackCnt == 2*(int)pss->result_pos.size())
    {
        // 4각형 피스들 갱신
        for (int x = 1; x < COLUMN_COUNT ; x++)
        {
            for (int y = 1 ; y < ROW_COUNT ; y++)
            {
                if (pss->m_pGameLayer->GetPuzzleP4Set()->GetObject(x, y) != NULL)
                    pss->m_pGameLayer->GetPuzzleP4Set()->RemoveChild(x, y);
                pss->m_pGameLayer->GetPuzzleP4Set()->CreatePiece(x, y, pss->m_pGameLayer->GetPuzzleP4Set()->GetType(x, y));
                pss->m_pGameLayer->GetPuzzleP4Set()->AddChild(x, y);
            }
        }
        /*
        // lock 풀기
        int x, y;
        for (int i = 0 ; i < ps->result_pos.size() ; i++)
        {
            x = (int)ps->result_pos[i].x;
            y = (int)ps->result_pos[i].y;
            ps->m_pGameLayer->UnLockEach(x, y);
        }
        */
        
        pss->m_pGameLayer->SkillSemaphoreUpdate(-1);
        
        pss->W8_isChanging--;
        if (pss->W8_isLastChangeWaiting)
            pss->W8_LastChange();
    }
}
void PuzzleSkill::W8_LastChange()
{
    CCLog("W8_lastChange");
    ps->W8_isLastChangeWaiting = false;
    ps->W8_isLastChange = true;
    
    // 한붓그리던 그림 모두 취소하고 없애기
    ps->m_pGameLayer->SkillSemaphoreUpdate(1);
    ps->m_pGameLayer->CancelDrawing();
    
    ps->W8_callbackCnt = 0;
    
    // 파란색 piece 찾기
    ps->result_pos.clear();
    int x, y;
    for (x = 0 ; x < COLUMN_COUNT ; x++)
    {
        for (y = 0 ; y < ROW_COUNT ; y++)
        {
            // 네 모서리에 위치한 존재하지 않는 부분
            if ((x == 0 && y == 0) || (x == 0 && y == ROW_COUNT-1) ||
                (x == COLUMN_COUNT-1 && y == 0) || (x == COLUMN_COUNT-1 && y == ROW_COUNT-1))
                continue;
            
            // 파란색을 찾는다.
            if (ps->m_pGameLayer->GetPuzzleP8Set()->GetType(x, y) == PIECE_BLUE)
                ps->result_pos.push_back(ccp(x, y));
        }
    }
    
    // effect
    ps->m_pGameLayer->GetEffect()->Effect15_Last(ps->result_pos, ps->m_pGameLayer->GetEffect());
    
    // 없앤다. (callback에서 새로 생성한다)
    for (int i = 0 ; i < ps->result_pos.size() ; i++)
    {
        x = (int)ps->result_pos[i].x;
        y = (int)ps->result_pos[i].y;
        
        CCLog("last : %d %d", x, y);
        
        // lock을 건다.
        //ps->m_pGameLayer->LockEach(x, y);
        
        // 없애기
        CCFiniteTimeAction* action = CCSequence::create(CCScaleTo::create(0.08f, 0.0f), CCCallFuncND::create(ps->m_pGameLayer, callfuncND_selector(PuzzleSkill::W8_Callback), ps), NULL);
        ps->m_pGameLayer->GetPuzzleP8Set()->GetSprite(x, y)->runAction(action);
    }
}
void PuzzleSkill::W8_Timer(float f) // 여신 지속시간 timer
{
    // 피스 renew 중, Pause 상태 -> 시간을 정지시킨다.
    if (ps->m_pGameLayer->IsPaused() || ps->m_pGameLayer->IsRenewing())
        return;
    if (ps->W8_scheduleDone || ps->W8_bombFirst)
        return;
    
    if (ps->W8_remainTime <= 0)
    {
        CCLog("Timer remainTime = 0 ; LastChange will be started.");
        if (ps->W8_bombFirst)
            return;
        ps->W8_scheduleDone = true;
        ps->m_pGameLayer->unschedule(schedule_selector(PuzzleSkill::W8_Timer));
        ps->m_pGameLayer->unschedule(schedule_selector(PuzzleSkill::W8_AccelTimer));
        ps->W8_isActive = false;
        ps->m_pGameLayer->GetEffect()->Effect15_Clear();
        
        // 마지막 변화 (모든 blue piece를 다른 색으로 바꾼다)
        ps->W8_isLastChangeWaiting = false;
        if (ps->W8_isChanging == 0)
            ps->W8_LastChange();
        else
            ps->W8_isLastChangeWaiting = true;
    }
    ps->W8_remainTime -= ps->W8_accel*100;
    
    CCLog("W8_timer = %d %d %d", ps->W8_remainTime, ps->W8_accelTime, ps->W8_accel);
    
    ps->m_pGameLayer->GetEffect()->AddOrbMaxParticle(-ps->W8_accel);
}
void PuzzleSkill::W8_AccelTimer(float f) // 여신 지속시간 감소에 대한 가속도 timer
{
    // 피스 renew 중, Pause 상태 -> 시간을 정지시킨다.
    if (ps->m_pGameLayer->IsPaused() || ps->m_pGameLayer->IsRenewing())
        return;
    
    ps->W8_accelTime += 100;
    if (ps->W8_accelTime % 3000 == 0)
        ps->W8_accel++;
}
void PuzzleSkill::W8_BombDone()
{
    W8_bombFirst = false;
}
bool PuzzleSkill::W8_IsActive()
{
    return W8_isActive;
}


////////////////////////////////////////////////////////////////////////////////////////////////////


void PuzzleSkill::E3(int num)
{
    // 떡갈나무지팡이 - 지팡이 레벨에 비례한 추가 별사탕
    
    UpdateAppliedSkillCount(num);
    
    E3_addedCandy = 0;
    //E3_addedCandy = pow((m_pGameLayer->GetStaffLevel())*3+65, 0.8f) * (skillLevel[num]*0.3f + 0.7f);
}

/*
void PuzzleSkill::E4(int num, int queue_pos)
{
    // 아낌없이 주는 나무 - 10개 이상 피스 제거 시 추가 별사탕
    
    UpdateAppliedSkillCount(num);
    
    E4_addedCandy = skillLevel[num]*30; // 나중에 보고 밸런스 조정
    
    // 이펙트 실행
    m_pGameLayer->GetEffect()->PlayEffect_MagicCircle(num);
    m_pGameLayer->GetEffect()->PlayEffect_SkillIcon(num);
    m_pGameLayer->PlayEffect(num, queue_pos);
    
    // sound
    m_pGameLayer->GetSound()->PlaySkillSound(num);
}
*/


void PuzzleSkill::E5(int num)
{
    // 자연의 힘 - 땅의 정령을 선택하면 모든 붉은 피스를 정중앙으로 모아준다.
    
    result_pos.clear();
    result_pos_end.clear();
    
    // 모든 green 피스 위치를 구한다.
    for (int x = 0 ; x < COLUMN_COUNT ; x++)
    {
        for (int y = 0 ; y < ROW_COUNT ; y++)
        {
            // 네 모서리에 위치한 존재하지 않는 부분
            if ((x == 0 && y == 0) || (x == 0 && y == ROW_COUNT-1) ||
                (x == COLUMN_COUNT-1 && y == 0) || (x == COLUMN_COUNT-1 && y == ROW_COUNT-1))
                continue;
            
            if (m_pGameLayer->GetPuzzleP8Set()->GetType(x, y) == PIECE_GREEN)
                result_pos.push_back(ccp(x, y));
        }
    }
    
    // 만약 보드판에 green piece가 하나도 없다면 아예 실행하지 않는다.
    if ((int)result_pos.size() == 0)
    {
        this->m_pGameLayer->SetSpiritTouch(false);
        return;
    }
    
    // 바꿔치기할 위치를 구한다. (정중앙부터 달팽이 모양으로)
    int dx = 1, dy = 0;
    int curX = 3, curY = 3;
    int cnt = 0, dcnt = 1;
    for (int i = 0 ; i < result_pos.size() ; i++)
    {
        if (!((curX == 0 && curY == 0) || (curX == 0 && curY == ROW_COUNT-1) || (curX == COLUMN_COUNT-1 && curY == 0) ||
              (curX == COLUMN_COUNT-1 && curY == ROW_COUNT-1)))
            result_pos_end.push_back(ccp(curX, curY));
        
        if (cnt++ >= std::abs(dcnt))
        {
            cnt = 1;
            if (dcnt > 0)
                dcnt *= -1;
            else
                dcnt = (-dcnt)+1;
            
            if (dx == 1 && dy == 0) { dx = 0; dy = 1; }
            else if (dx == 0 && dy == 1) { dx = -1; dy = 0; }
            else if (dx == -1 && dy == 0) { dx = 0; dy = -1; }
            else { dx = 1; dy = 0; }
        }
        
        curX += dx;
        curY += dy;
    }
    
    // 위치가 바뀌지 않는 피스를 찾는다. (-1로 값 대체)
    CCPoint temp;
    for (int i = 0 ; i < result_pos_end.size() ; i++)
    {
        for (int j = 0; j < result_pos.size() ; j++)
        {
            if (result_pos_end[i].x == result_pos[j].x && result_pos_end[i].y == result_pos[j].y)
            {
                result_pos_end[i] = ccp(-1, -1);
                result_pos[j] = ccp(-1, -1);
            }
        }
    }
    
    assert(result_pos.size() == result_pos_end.size());
    E5_i.clear();
    E5_j.clear();
    E5_ij_cnt = 0;
    E5_callbackCnt = 0;
    
    // 남은 것들을 서로 매칭시킨다.
    for (int i = 0 ; i < result_pos.size() ; i++)
    {
        if (result_pos[i].x == -1 && result_pos[i].y == -1)
            continue;
        for (int j = 0 ; j < result_pos_end.size() ; j++)
        {
            if (result_pos_end[j].x == -1 && result_pos_end[j].y == -1)
                continue;
            
            int x = (int)result_pos[i].x;
            int y = (int)result_pos[i].y;
            int x_end = (int)result_pos_end[j].x;
            int y_end = (int)result_pos_end[j].y;
            E5_i.push_back(result_pos[i]);
            E5_j.push_back(result_pos_end[j]);
            
            E5_callbackCnt++;
            
            result_pos_end[j] = ccp(-1, -1);
            
            CCActionInterval* action = CCMoveTo::create(0.35f, m_pGameLayer->SetPiece8Position(x_end, y_end));
            m_pGameLayer->GetSpriteP8(x, y)->runAction( CCEaseOut::create(action, 0.35f) );
            
            CCActionInterval* action2 = CCSequence::create(CCMoveTo::create(0.35f, m_pGameLayer->SetPiece8Position(x, y)), CCCallFuncND::create(m_pGameLayer, callfuncND_selector(PuzzleSkill::E5_Callback), this), NULL);
            m_pGameLayer->GetSpriteP8(x_end, y_end)->runAction( CCEaseOut::create(action2, 0.35f) );
            
            break;
        }
    }
    
    // green 피스는 있으나 모두 움직이지 않는다면 이 또한 실행하지 않도록 한다.
    if (E5_callbackCnt == 0)
    {
        this->m_pGameLayer->SetSpiritTouch(false);
        return;
    }
    
    // sound
    m_pGameLayer->GetSound()->PlaySkillSound(num);
    
    // 이펙트 출현 + 정령을 화면에서 없애기
    GetEffect()->SpiritEffect(2);
    GetEffect()->RemoveSpirit(2);
    isSpiritAlive[2] = false;
    
    UpdateAppliedSkillCount(num);
}
void PuzzleSkill::E5_Callback(CCNode* sender, void* data)
{
    PuzzleSkill* ps = (PuzzleSkill*)data;
    int x = (int)ps->E5_i[ps->E5_ij_cnt].x;
    int y = (int)ps->E5_i[ps->E5_ij_cnt].y;
    int x_end = (int)ps->E5_j[ps->E5_ij_cnt].x;
    int y_end = (int)ps->E5_j[ps->E5_ij_cnt].y;
    
    // 8각형 피스 swap (data)
    ps->m_pGameLayer->GetPuzzleP8Set()->SwapObject(x, y, x_end, y_end);
    
    ps->E5_ij_cnt++;
    if (ps->E5_callbackCnt == (int)ps->E5_ij_cnt)
    {
        // 4각형 피스들 갱신
        for (int x = 1; x < COLUMN_COUNT ; x++)
        {
            for (int y = 1 ; y < ROW_COUNT ; y++)
            {
                if (ps->m_pGameLayer->GetPuzzleP4Set()->GetObject(x, y) != NULL)
                    ps->m_pGameLayer->GetPuzzleP4Set()->RemoveChild(x, y);
                ps->m_pGameLayer->GetPuzzleP4Set()->CreatePiece(x, y, ps->m_pGameLayer->GetPuzzleP4Set()->GetType(x, y));
                ps->m_pGameLayer->GetPuzzleP4Set()->AddChild(x, y);
            }
        }
        
        ps->m_pGameLayer->SetSpiritTouch(false);
    }
}


void PuzzleSkill::E7(int num)
{
    // 끈질긴 생명력 - 포션을 1개 얻는다. 한 번 얻으면 더 이상 발동되지 않는다.
    if (!E7_getPotion)
    {
        UpdateAppliedSkillCount(num);
        
        E7_getPotion = true;
        
        //m_pGameLayer->GetEffect()->PlayEffect_MagicCircle(num);
    }
}



void PuzzleSkill::E8_Timer(float f) // 라인 흔들기를 시작한다.
{
    if (ps->m_pGameLayer->IsPaused())
        return;
    
    //if (ps->E8_lineIdx.empty())
    if (ps->E8_cnt == (int)ps->E8_lineIdx.size()) // 끝났음 : 스케쥴링 중지
    {
        ps->m_pGameLayer->unschedule(schedule_selector(PuzzleSkill::E8_Timer));
    }
    else
    {
        int x = ps->E8_lineIdx[ps->E8_cnt];
        ps->E8_bottomY[x] = ROW_COUNT-1-ps->E8_lineDepth[ps->E8_cnt]+1;
        ps->E8_curY[x] = ROW_COUNT-1;
        if (x == 0 || x == COLUMN_COUNT-1)
        {
            ps->E8_curY[x]--;
            if (ps->E8_bottomY[x] == 0)
                ps->E8_bottomY[x]++;
        }
        
        // 흔드는 소리
        ps->m_pGameLayer->GetSound()->PlayDesignatedSound(232);
        // 흔들기+폭파 시작
        ps->E8_Bomb(NULL, (void*)x);
        
        ps->E8_cnt++;
    }
}
void PuzzleSkill::E8_Bomb(CCNode* sender, void* data)
{
    int x = (int)data;
    if (ps->E8_curY[x] < ps->E8_bottomY[x]) // 다 흔든 경우 : 폭파시킨다.
    {
        // bomb effect
        ps->m_pGameLayer->GetEffect()->Effect23_Bomb(x);
        
        // bomb sound
        //ps->m_pGameLayer->GetSound()->PlayDesignatedSound(231);
        ps->m_pGameLayer->GetSound()->PlayBomb();
        
        // 폭파
        ps->E8_bombCallbackCnt[x] = 0;
        for (int y = ROW_COUNT-1 ; y >= ps->E8_bottomY[x] ; y--)
        {
            if (y == ROW_COUNT-1 && (x == 0 || x == COLUMN_COUNT-1))
                continue;
            
            // 폭파 개수 갱신
            ps->m_pGameLayer->UpdatePieceBombCnt(ps->m_pGameLayer->GetPuzzleP8Set()->GetType(x, y), 1);
            
            CCFiniteTimeAction* action = CCSequence::create(CCSpawn::create(CCScaleTo::create(0.15f, 1.5f), CCFadeOut::create(0.15f), NULL), CCCallFuncND::create(ps->m_pGameLayer, callfuncND_selector(PuzzleSkill::E8_BombCallback), (void*)x), NULL);
            ps->m_pGameLayer->GetSpriteP8(x, y)->runAction(action);
        }
    }
    else // 다음 피스를 흔든다.
    {
        //CCLog("E8_Bomb : (x, curY, bottomY) = (%d, %d, %d)", x, ps->E8_curY[x], ps->E8_bottomY[x]);
        CCActionInterval* action = CCSequence::create(CCMoveBy::create(0.02f, ccp(-30, 0)), CCMoveBy::create(0.04f, ccp(60, 0)), CCMoveBy::create(0.04f, ccp(-60, 0)), CCMoveBy::create(0.04f, ccp(60, 0)), CCMoveBy::create(0.02f, ccp(-30, 0)), CCCallFuncND::create(ps->m_pGameLayer, callfuncND_selector(PuzzleSkill::E8_Bomb), (void*)x), NULL);
        ps->m_pGameLayer->GetSpriteP8(x, ps->E8_curY[x])->runAction(action);
        ps->E8_curY[x]--;
    }
}
void PuzzleSkill::E8_BombCallback(CCNode* sender, void* data)
{
    int x = (int)data;
    ps->E8_bombCallbackCnt[x]++;
    if (ps->E8_bombCallbackCnt[x] >= ROW_COUNT-1-ps->E8_bottomY[x]+1 - (x == 0 || x == COLUMN_COUNT-1))
    {
        for (int y = ROW_COUNT-1 ; y >= ps->E8_bottomY[x] ; y--)
        {
            if (y == ROW_COUNT-1 && (x == 0 || x == COLUMN_COUNT-1))
                continue;
            ps->m_pGameLayer->GetPuzzleP8Set()->RemoveChild(x, y);
            ps->m_pGameLayer->SetSpriteP8Null(x, y);
        }
        ps->m_pGameLayer->Falling(ps->queuePos, x);
    }
}
void PuzzleSkill::E8_DecideRestart(int x)
{
    ps->E8_endCnt++;
    
    if (ps->E8_endCnt == (int)ps->E8_lineIdx.size())
    {
        ps->E8_isActive = false;
        ps->m_pGameLayer->GetEffect()->Effect23_Clear();
    }
}

bool PuzzleSkill::E8_IsFinished()
{
    return !E8_isActive;
}

void PuzzleSkill::E8(int num, int queue_pos)
{
    // 고대나무의 비밀
    // 정해진 회수만큼 라인을 잡아서, 터뜨린다. (초록 피스가 있는 라인)
    
    UpdateAppliedSkillCount(num);
    
    // 변수 초기화
    E8_isActive = false;
    E8_cnt = 0;
    E8_endCnt = 0;
    E8_lineIdx.clear();
    E8_lineDepth.clear();
  
    SetQueuePos(queue_pos);
    ps = this;
    
    // line별 depth 확률
    int E8_prob_sum = 0;
    E8_prob[0] = 300 - (skillLevel[23]-1)*50;
    E8_prob_sum += E8_prob[0];
    E8_prob[1] = 250 - (skillLevel[23]-1)*25 + E8_prob_sum;
    E8_prob_sum += 250 - (skillLevel[23]-1)*25;
    E8_prob[2] = 200 + E8_prob_sum;
    E8_prob_sum += 200;
    E8_prob[3] = 150 + (skillLevel[23]-1)*25 + E8_prob_sum;
    E8_prob_sum += 150 + (skillLevel[23]-1)*25;
    E8_prob[4] = 100 + (skillLevel[23]-1)*50 + E8_prob_sum;
    E8_prob_sum += 100 + (skillLevel[23]-1)*50;
    
    // 처음 7번에 대한 순서를 정한다.
    int check[7] = {0,};
    int cnt = 0, k, depth;
    while (cnt < 7)
    {
        k = rand()%7;
        if (check[k] == 0)
        {
            cnt++;
            
            check[k] = 1;
            E8_lineIdx.push_back(k);
            
            // 그 라인의 depth를 구한다.
            k = rand()%1000;
            for (int j = 0 ; j < 5 ; j++)
            {
                if (k < E8_prob[j])
                {
                    depth = j+3;
                    break;
                }
            }
            E8_lineDepth.push_back(depth);
            //CCLog("%d번째 라인 : %d (depth = %d)", (int)E8_lineIdx.size(), E8_lineIdx[E8_lineIdx.size()-1], E8_lineDepth[E8_lineDepth.size()-1]);
        }
    }
    for (int i = cnt ; i < skillLevel[23]+7 ; i++) // 남은 개수 마저 구한다 (처음 7개의 순서로 반복된다)
    {
        E8_lineIdx.push_back(E8_lineIdx[i-7]);
        k = rand()%1000;
        for (int j = 0 ; j < 5 ; j++)
        {
            if (k < E8_prob[j])
            {
                depth = j+3;
                break;
            }
        }
        E8_lineDepth.push_back(depth);
        //CCLog("%d번째 라인 : %d (depth = %d)", (int)E8_lineIdx.size(), E8_lineIdx[E8_lineIdx.size()-1], E8_lineDepth[E8_lineDepth.size()-1]);
    }
    
    E8_isActive = true;
    ps->m_pGameLayer->PlayEffect(num, queue_pos);
    
    /*
    E8_FindLine(-1);

    if (E8_cnt > 0)
    {
        E8_isActive = true;
        E8_activeCnt = E8_cnt; // 실제 진행중인 line 수
        ps->m_pGameLayer->PlayEffect(num, queue_pos);
    }
    */
}
void PuzzleSkill::E8_Start()
{
    //CCLog("초기 시작 라인 수 = %d", ps->E8_activeCnt);
    CCLog("실행 라인 수 = %d", ps->skillLevel[23]+7);
    ps->m_pGameLayer->schedule(schedule_selector(PuzzleSkill::E8_Timer), 0.3f);
}
bool PuzzleSkill::E8_IsActive()
{
    return E8_isActive;
}

std::vector<CCPoint> PuzzleSkill::A8GetPos()
{
    return A8_pos;
}
void PuzzleSkill::A8Clear()
{
    A8_pos.clear();
}




////////////////////////////////////////////////////////////////////////////////////////////////////////

void PuzzleSkill::M1(int num)
{
}
void PuzzleSkill::M2(int num)
{
}
void PuzzleSkill::M3(int num)
{
}
void PuzzleSkill::M4(int num)
{
}
void PuzzleSkill::M5(int num)
{
}

std::vector<CCPoint> PuzzleSkill::GetResult()
{
    return result_pos;
}
std::vector< std::vector<CCPoint> > PuzzleSkill::GetResultDouble()
{
    return result_double_pos;
}
std::vector<CCPoint> PuzzleSkill::GetResultEnd()
{
    return result_pos_end;
}
void PuzzleSkill::ResultClear()
{
    result_pos.clear();
}


void PuzzleSkill::RemoveAllObjects()
{
    for (int i = 0 ; i < result_double_pos.size() ; i++)
        result_double_pos[i].clear();
    result_double_pos.clear();
    result_pos.clear();
    result_pos_end.clear();
    result_pos_temp.clear();

    E5_pos.clear();
    E5_pos_end.clear();
    E5_i.clear();
    E5_j.clear();
    //while (!E8_lineIdx.empty())
    //    E8_lineIdx.pop();
    E8_lineIdx.clear();
    E8_lineDepth.clear();
    A8_pos.clear();
    A2_pos.clear();
}




void PuzzleSkill::GiveHint()
{
    int x, y, sx, sy, type;
    std::vector<CCPoint> pos8, pos4;
    std::queue<CCPoint> qpos;
    CCPoint p;
    
    int cnt = 10; // test로 최대 10번만 검사함
    while (cnt-- > 0)
    {
        sx = rand() % COLUMN_COUNT;
        sy = rand() % ROW_COUNT;
        // 네 모서리에 위치한 존재하지 않는 부분
        if ((sx == 0 && sy == 0) || (sx == 0 && sy == ROW_COUNT-1) ||
            (sx == COLUMN_COUNT-1 && sy == 0) || (sx == COLUMN_COUNT-1 && sy == ROW_COUNT-1))
            continue;
        
        // check 초기화
        for (x = 0 ; x < COLUMN_COUNT ; x++)
            for (y = 0 ; y < ROW_COUNT ; y++)
                hintCheck[x][y] = false;
        hintCheck[0][0] = hintCheck[0][ROW_COUNT-1] = hintCheck[COLUMN_COUNT-1][0] = hintCheck[COLUMN_COUNT-1][ROW_COUNT-1] = true;
        
        // 큐에 시작점 넣기
        pos8.clear();
        pos4.clear();
        qpos.push(ccp(sx, sy));
        type = m_pGameLayer->GetPuzzleP8Set()->GetType(sx, sy);
        hintCheck[sx][sy] = true;
        
        // BFS로, 연결된 같은색 피스 찾기
        while (!qpos.empty())
        {
            p = qpos.front();
            qpos.pop();
            
            pos8.push_back(p);
            
            x = p.x;
            y = p.y;
            //CCLog("hint = %d %d", x, y);
            
            if (x > 0 && y > 0 && !hintCheck[x-1][y-1] && m_pGameLayer->IsConnected(x, y) && m_pGameLayer->GetPuzzleP8Set()->GetType(x-1, y-1) == type)
            {   qpos.push(ccp(x-1, y-1)); hintCheck[x-1][y-1] = true; pos4.push_back(ccp(x, y)); }
            if (x > 0 && y+1 < ROW_COUNT && !hintCheck[x-1][y+1] && m_pGameLayer->IsConnected(x, y+1) && m_pGameLayer->GetPuzzleP8Set()->GetType(x-1, y+1) == type)
            {   qpos.push(ccp(x-1, y+1)); hintCheck[x-1][y+1] = true; pos4.push_back(ccp(x, y+1)); }
            if (x+1 < COLUMN_COUNT && y > 0 && !hintCheck[x+1][y-1] && m_pGameLayer->IsConnected(x+1, y) && m_pGameLayer->GetPuzzleP8Set()->GetType(x+1, y-1) == type)
            {   qpos.push(ccp(x+1, y-1)); hintCheck[x+1][y-1] = true; pos4.push_back(ccp(x+1, y)); }
            if (x+1 < COLUMN_COUNT && y+1 < ROW_COUNT && !hintCheck[x+1][y+1] && m_pGameLayer->IsConnected(x+1, y+1) && m_pGameLayer->GetPuzzleP8Set()->GetType(x+1, y+1) == type)
            {   qpos.push(ccp(x+1, y+1)); hintCheck[x+1][y+1] = true; pos4.push_back(ccp(x+1, y+1)); }
            
            if (y+1 < ROW_COUNT && !hintCheck[x][y+1] && m_pGameLayer->GetPuzzleP8Set()->GetType(x, y+1) == type)
            {   qpos.push(ccp(x, y+1)); hintCheck[x][y+1] = true; }
            if (y > 0 && !hintCheck[x][y-1] && m_pGameLayer->GetPuzzleP8Set()->GetType(x, y-1) == type)
            {   qpos.push(ccp(x, y-1)); hintCheck[x][y-1] = true; }
            if (x > 0 && !hintCheck[x-1][y] && m_pGameLayer->GetPuzzleP8Set()->GetType(x-1, y) == type)
            {   qpos.push(ccp(x-1, y)); hintCheck[x-1][y] = true; }
            if (x+1 < COLUMN_COUNT && !hintCheck[x+1][y] && m_pGameLayer->GetPuzzleP8Set()->GetType(x+1, y) == type)
            {   qpos.push(ccp(x+1, y)); hintCheck[x+1][y] = true; }
        }
        
        // 피스 3개 이상인 덩어리를 찾으면 알려주기
        if (pos8.size() >= 3)
            break;
    }
    
    CCLog("find hint? %d", cnt);
    
    // hint action
    for (int i = 0 ; i < pos8.size() ; i++)
    {
        x = pos8[i].x;
        y = pos8[i].y;
        
        CCActionInterval* action = CCSequence::create(CCTintTo::create(0.5f, 100, 100, 100), CCTintTo::create(0.5f, 255, 255, 255), CCDelayTime::create(0.5f), NULL);
        m_pGameLayer->GetSpriteP8(x, y)->runAction(CCRepeatForever::create(action));
    }
    for (int i = 0 ; i < pos4.size() ; i++)
    {
        x = pos4[i].x;
        y = pos4[i].y;
        
        CCActionInterval* action = CCSequence::create(CCTintTo::create(0.5f, 100, 100, 100), CCTintTo::create(0.5f, 255, 255, 255), CCDelayTime::create(0.5f), NULL);
        m_pGameLayer->GetPuzzleP4Set()->SetAction(x, y, CCRepeatForever::create(action));
    }
}



// 피스들의 드랍이 끝나고 (혹은 특정 스킬이 끝나고), 한붓그리기할 부분이 없어 갱신해야 하는지 검사하는 함수
bool PuzzleSkill::IsRenewNeeded()
{
    m_pGameLayer->SetRenewFlag(true);
    
    // 변수 init
    for (int x = 0 ; x < COLUMN_COUNT ; x++)
        for (int y = 0 ; y < ROW_COUNT ; y++)
            renewCheck[x][y] = false;
    renewCheck[0][0] = renewCheck[0][ROW_COUNT-1] = renewCheck[COLUMN_COUNT-1][0] = renewCheck[COLUMN_COUNT-1][ROW_COUNT-1] = true;
    
    for (int x = 0 ; x < COLUMN_COUNT ; x++)
    {
        for (int y = 0 ; y < ROW_COUNT ; y++)
        {
            if (!renewCheck[x][y])
            {
                renewCheck_maxCnt = 0;
                IsRenewNeeded_Recur(x, y, m_pGameLayer->GetPuzzleP8Set()->GetType(x, y), 0);
                if (renewCheck_maxCnt >= 3)
                {
                    m_pGameLayer->SetRenewFlag(false);
                    return false;
                }
            }
        }
    }
    return true;
}
void PuzzleSkill::IsRenewNeeded_Recur(int x, int y, int type, int cnt)
{
    renewCheck[x][y] = true;
    renewCheck_maxCnt = std::max(renewCheck_maxCnt, cnt);
    if (renewCheck_maxCnt >= 3)
        return;
    
    if (x > 0 && y > 0 && !renewCheck[x-1][y-1] && m_pGameLayer->IsConnected(x, y) && m_pGameLayer->GetPuzzleP8Set()->GetType(x-1, y-1) == type)
        IsRenewNeeded_Recur(x-1, y-1, type, cnt+1);
    if (x > 0 && y+1 < ROW_COUNT && !renewCheck[x-1][y+1] && m_pGameLayer->IsConnected(x, y+1) && m_pGameLayer->GetPuzzleP8Set()->GetType(x-1, y+1) == type)
        IsRenewNeeded_Recur(x-1, y+1, type, cnt+1);
    if (x+1 < COLUMN_COUNT && y > 0 && !renewCheck[x+1][y-1] && m_pGameLayer->IsConnected(x+1, y) && m_pGameLayer->GetPuzzleP8Set()->GetType(x+1, y-1) == type)
        IsRenewNeeded_Recur(x+1, y-1, type, cnt+1);
    if (x+1 < COLUMN_COUNT && y+1 < ROW_COUNT && !renewCheck[x+1][y+1] && m_pGameLayer->IsConnected(x+1, y+1) && m_pGameLayer->GetPuzzleP8Set()->GetType(x+1, y+1) == type)
        IsRenewNeeded_Recur(x+1, y+1, type, cnt+1);
    
    if (y+1 < ROW_COUNT && !renewCheck[x][y+1] && m_pGameLayer->GetPuzzleP8Set()->GetType(x, y+1) == type)
        IsRenewNeeded_Recur(x, y+1, type, cnt+1);
    if (y > 0 && !renewCheck[x][y-1] && m_pGameLayer->GetPuzzleP8Set()->GetType(x, y-1) == type)
        IsRenewNeeded_Recur(x, y-1, type, cnt+1);
    if (x > 0 && !renewCheck[x-1][y] && m_pGameLayer->GetPuzzleP8Set()->GetType(x-1, y) == type)
        IsRenewNeeded_Recur(x-1, y, type, cnt+1);
    if (x+1 < COLUMN_COUNT && !renewCheck[x+1][y] && m_pGameLayer->GetPuzzleP8Set()->GetType(x+1, y) == type)
        IsRenewNeeded_Recur(x+1, y, type, cnt+1);
}

// 피스 갱신 함수
void PuzzleSkill::RenewPuzzle(int queue_pos)
{
    // init
    for (int i = 0 ; i < result_double_pos.size() ; i++)
        result_double_pos[i].clear();
    result_double_pos.clear();
    
    //int count = rand()%3 + 3;
    int count = 3;
    for (int i = 0 ; i < count ; i++)
    {
        std::vector<CCPoint> temp;
        result_double_pos.push_back(temp);
    }
    
    // 변수 init
    for (int x = 0 ; x < COLUMN_COUNT ; x++)
        for (int y = 0 ; y < ROW_COUNT ; y++)
            F7_check[x][y] = false;
    F7_check[0][0] = F7_check[0][ROW_COUNT-1] = F7_check[COLUMN_COUNT-1][0] = F7_check[COLUMN_COUNT-1][ROW_COUNT-1] = true;
    
    // 덩어리를 랜덤하게 모은다
    int cur = 0;
    int x, y;
    while (cur < count)
    {
        x = rand() % COLUMN_COUNT;
        y = rand() % ROW_COUNT;
        
        // 네 모서리에 위치한 존재하지 않는 부분
        if ((x == 0 && y == 0) || (x == 0 && y == ROW_COUNT-1) ||
            (x == COLUMN_COUNT-1 && y == 0) || (x == COLUMN_COUNT-1 && y == ROW_COUNT-1))
            continue;
        if (F7_check[x][y])
            continue;
        
        RenewPuzzle_Recur(x, y, m_pGameLayer->GetPuzzleP8Set()->GetType(x, y), result_double_pos[cur]);
        cur++;
    }
    
    // 코코 사운드
    m_pGameLayer->GetSound()->PlaySkillSound(6);
    // 코코 이펙트
    m_pGameLayer->GetEffect()->PlayEffect_MagicCircle(6);
    m_pGameLayer->GetEffect()->PlayEffect_SkillIcon(6);
    m_pGameLayer->PlayEffect(6, NULL); // 양수 : 코코 주위에 링 생김
    
    //F7_callbackCnt = 0;
    //if (F7_callbackCnt < count && (int)result_double_pos[F7_callbackCnt].size() > 0)
    //{
        m_pGameLayer->GetEffect()->PlayEffect_6_Fire(result_double_pos, queue_pos, 0);
    //}
}
void PuzzleSkill::RenewPuzzle_Recur(int x, int y, int type, std::vector<CCPoint>& v)
{
    F7_check[x][y] = true;
    v.push_back(ccp(x, y));
    
    if (x > 0 && y > 0 && !F7_check[x-1][y-1] && m_pGameLayer->IsConnected(x, y) && m_pGameLayer->GetPuzzleP8Set()->GetType(x-1, y-1) == type)
        RenewPuzzle_Recur(x-1, y-1, type, v);
    if (x > 0 && y+1 < ROW_COUNT && !F7_check[x-1][y+1] && m_pGameLayer->IsConnected(x, y+1) && m_pGameLayer->GetPuzzleP8Set()->GetType(x-1, y+1) == type)
        RenewPuzzle_Recur(x-1, y+1, type, v);
    if (x+1 < COLUMN_COUNT && y > 0 && !F7_check[x+1][y-1] && m_pGameLayer->IsConnected(x+1, y) && m_pGameLayer->GetPuzzleP8Set()->GetType(x+1, y-1) == type)
        RenewPuzzle_Recur(x+1, y-1, type, v);
    if (x+1 < COLUMN_COUNT && y+1 < ROW_COUNT && !F7_check[x+1][y+1] && m_pGameLayer->IsConnected(x+1, y+1) && m_pGameLayer->GetPuzzleP8Set()->GetType(x+1, y+1) == type)
        RenewPuzzle_Recur(x+1, y+1, type, v);
    
    if (y+1 < ROW_COUNT && !F7_check[x][y+1] && m_pGameLayer->GetPuzzleP8Set()->GetType(x, y+1) == type)
        RenewPuzzle_Recur(x, y+1, type, v);
    if (y > 0 && !F7_check[x][y-1] && m_pGameLayer->GetPuzzleP8Set()->GetType(x, y-1) == type)
        RenewPuzzle_Recur(x, y-1, type, v);
    if (x > 0 && !F7_check[x-1][y] && m_pGameLayer->GetPuzzleP8Set()->GetType(x-1, y) == type)
        RenewPuzzle_Recur(x-1, y, type, v);
    if (x+1 < COLUMN_COUNT && !F7_check[x+1][y] && m_pGameLayer->GetPuzzleP8Set()->GetType(x+1, y) == type)
        RenewPuzzle_Recur(x+1, y, type, v);
}

void PuzzleSkill::RenewPuzzle_End(void* pointer, int queue_pos)
{
    CCLog("F7 콜백 끝 : Falling 시작");
    PuzzleSkill* pss = (PuzzleSkill*)pointer;
    int x, y;
    for (int i = 0 ; i < pss->result_double_pos.size() ; i++)
    {
        for (int j = 0 ; j < pss->result_double_pos[i].size() ; j++)
        {
            // 실제로 제거
            x = (int)pss->result_double_pos[i][j].x;
            y = (int)pss->result_double_pos[i][j].y;
            pss->m_pGameLayer->GetPuzzleP8Set()->RemoveChild(abs(x), abs(y));
            pss->m_pGameLayer->SetSpriteP8Null(abs(x), abs(y));
        }
    }
    
    // falling queue insertion
    pss->m_pGameLayer->FallingQueuePushAndFalling(queue_pos);
    
   // m_pGameLayer->SetRenewFlag(false);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void PuzzleSkill::FT_StartEnd(int queue_pos)
{
    SetQueuePos(queue_pos);
    result_pos.clear();
    
    FT_pos = 2;
    
    for (int x = 0 ; x < COLUMN_COUNT ; x++)
    {
        for (int y = 0 ; y < ROW_COUNT ; y++)
        {
            // 네 모서리에 위치한 존재하지 않는 부분
            if ((x == 0 && y == 0) || (x == 0 && y == ROW_COUNT-1) ||
                (x == COLUMN_COUNT-1 && y == 0) || (x == COLUMN_COUNT-1 && y == ROW_COUNT-1))
                continue;
            
            m_pGameLayer->LockEach(x, y);
            result_pos.push_back(ccp(x, y));
        }
    }
    
    FT_callbackCnt = 0;
    for (int i = 0 ; i < result_pos.size() ; i++)
    {
        int x = result_pos[i].x;
        int y = result_pos[i].y;
        
        CCActionInterval* action = CCSequence::create( CCSpawn::create(CCScaleTo::create(0.15f, 1.5f), CCFadeOut::create(0.15f), NULL), CCCallFuncND::create(m_pGameLayer, callfuncND_selector(PuzzleSkill::FT_Callback), this), NULL);
        m_pGameLayer->GetSpriteP8(x, y)->runAction(action);
    }
}
bool PuzzleSkill::IsFTBombing()
{
    return isFTBombing;
}

void PuzzleSkill::FT_Bomb(std::vector<CCPoint> p)
{
    isFTBombing = true;
    
    int x, y;
    int minx, maxx;
    
    x = p[0].x;
    if (0 <= x && x <= 2)
    {
        minx = 0;
        maxx = 2;
        FT_pos = 0;
    }
    else
    {
        minx = 4;
        maxx = 6;
        FT_pos = 1;
    }
    
    sound->PlayBomb();
    
    FT_callbackCnt = 0;
    result_pos.clear();
    for (int x = minx ; x <= maxx ; x++)
    {
        for (int y = 0 ; y < ROW_COUNT ; y++)
        {
            // 네 모서리에 위치한 존재하지 않는 부분
            if ((x == 0 && y == 0) || (x == 0 && y == ROW_COUNT-1) ||
                (x == COLUMN_COUNT-1 && y == 0) || (x == COLUMN_COUNT-1 && y == ROW_COUNT-1))
                continue;
            
            result_pos.push_back(ccp(x, y));
        }
    }
    
    // 피버타임 끝났으면 하지마!
//    if (!m_pGameLayer->IsFeverTime())
//        return;
    
    for (int i = 0 ; i < result_pos.size() ; i++)
    {
        x = result_pos[i].x;
        y = result_pos[i].y;
        
        CCActionInterval* action = CCSequence::create( CCSpawn::create(CCScaleTo::create(0.15f, 1.5f), CCFadeOut::create(0.15f), NULL), CCCallFuncND::create(m_pGameLayer, callfuncND_selector(PuzzleSkill::FT_Callback), this), NULL);
        m_pGameLayer->GetSpriteP8(x, y)->runAction(action);
    }
}

void PuzzleSkill::FT_Callback(CCNode* sender, void* pointer)
{
    PuzzleSkill* pss = (PuzzleSkill*)pointer;
    
    pss->FT_callbackCnt++;
    if (pss->FT_callbackCnt == pss->result_pos.size())
    {
        // 피버타임 처음/끝 폭발 시
        int x, y;
        for (int i = 0 ; i < pss->result_pos.size() ; i++)
        {
            x = pss->result_pos[i].x;
            y = pss->result_pos[i].y;
            pss->m_pGameLayer->GetPuzzleP8Set()->RemoveChild(x, y);
            pss->m_pGameLayer->SetSpriteP8Null(x, y);
        }
        pss->result_pos.clear();
        
        if (!pss->m_pGameLayer->IsFeverTime())
            pss->FT_pos = -1;
        CCLog("%d", pss->FT_pos);
        pss->m_pGameLayer->Falling(pss->queuePos, pss->FT_pos);
    }
}

void PuzzleSkill::FT_CreatePiece(int pos)
{
    if (m_pGameLayer->IsFeverTime() && m_pGameLayer->GetFeverRemainTime() <= 0)
    {
        isFTBombing = false;
        m_pGameLayer->EndFeverTime();
    }
    
    int minx = 0, maxx = COLUMN_COUNT-1;
    if (pos == 0)
        maxx = 2;
    else if (pos == 1)
        minx = 4;
    
    // 안전을 위한 lock 걸기
    for (int x = minx ; x <= maxx ; x++)
    {
        for (int y = 0 ; y < ROW_COUNT ; y++)
        {
            // 네 모서리에 위치한 존재하지 않는 부분
            if ((x == 0 && y == 0) || (x == 0 && y == ROW_COUNT-1) ||
                (x == COLUMN_COUNT-1 && y == 0) || (x == COLUMN_COUNT-1 && y == ROW_COUNT-1))
                continue;
            
            m_pGameLayer->LockEach(x, y);
        }
    }
    
    for (int x = 0 ; x < COLUMN_COUNT ; x++)
        for (int y = 0 ; y < ROW_COUNT ; y++)
            FT_check[x][y] = false;
    FT_check[0][0] = FT_check[0][ROW_COUNT-1] = FT_check[COLUMN_COUNT-1][0] = FT_check[COLUMN_COUNT-1][ROW_COUNT-1] = true;
 
    result_pos.clear();
    
    if (pos == 0 || pos == 2) // 보드판의 왼쪽에 생성
    {
        FT_createCnt = rand()%13 + 3; // 3 ~ 15
        int x = rand() % 3; // 0 ~ 2
        int y = rand() % ROW_COUNT;
        if (x == 0 && (y == 0 || y == ROW_COUNT-1))
            x++;
        
        FT_Create_Recur(x, y, 0, 0, 2);
    }
    if (pos == 1 || pos == 2) // 보드판의 오른쪽에 생성
    {
        FT_createCnt = rand()%13 + 3; // 3 ~ 15
        int x = rand() % 3 + 4; // 4 ~ 6
        int y = rand() % ROW_COUNT;
        if (x == COLUMN_COUNT-1 && (y == 0 || y == ROW_COUNT-1))
            x--;
        
        FT_Create_Recur(x, y, 0, 4, 6);
    }
    
    // 실제 생성
    int type = rand() % 3; // red, blue, green 중 하나
    for (int i = 0 ; i < result_pos.size() ; i++)
    {
        int x = result_pos[i].x;
        int y = result_pos[i].y;
        m_pGameLayer->GetPuzzleP8Set()->RemoveChild(x, y);
        m_pGameLayer->SetSpriteP8Null(x, y);
        
        m_pGameLayer->GetPuzzleP8Set()->CreatePiece(x, y, type);
        m_pGameLayer->GetPuzzleP8Set()->AddChild(x, y);
        m_pGameLayer->GetPuzzleP8Set()->GetSprite(x, y)->setPosition( m_pGameLayer->SetPiece8Position(x, y) );
        
        m_pGameLayer->UnLockEach(x, y);
    }
    
    // diamond들을 다시 검사해서 적절히 바꿔준다.
    for (int x = 1 ; x < COLUMN_COUNT ; x++)
    {
        for (int y = 1 ; y < ROW_COUNT ; y++)
        {
            if (m_pGameLayer->GetPuzzleP4Set()->GetType(x, y) != BLOCKED)
            {
                if (m_pGameLayer->GetPuzzleP4Set()->GetObject(x, y) != NULL)
                    m_pGameLayer->GetPuzzleP4Set()->RemoveChild(x, y);
                m_pGameLayer->GetPuzzleP4Set()->CreatePiece(x, y, m_pGameLayer->GetPuzzleP4Set()->GetType(x, y));
                m_pGameLayer->GetPuzzleP4Set()->AddChild(x, y);
            }
        }
    }
    
    isFTBombing = false;
}

void PuzzleSkill::FT_Create_Recur(int x, int y, int cnt, int minx, int maxx)
{
    FT_check[x][y] = true;
    if (result_pos.size() >= FT_createCnt)
        return;
    result_pos.push_back(ccp(x, y));
        
    if ((x-1 >= minx && x-1 <= maxx) && y > 0 && !FT_check[x-1][y-1] && m_pGameLayer->IsConnected(x, y))
        FT_Create_Recur(x-1, y-1, cnt+1, minx, maxx);
    if ((x-1 >= minx && x-1 <= maxx) && y+1 < ROW_COUNT && !FT_check[x-1][y+1] && m_pGameLayer->IsConnected(x, y+1))
        FT_Create_Recur(x-1, y+1, cnt+1, minx, maxx);
    if ((x+1 >= minx && x+1 <= maxx) && y > 0 && !FT_check[x+1][y-1] && m_pGameLayer->IsConnected(x+1, y))
        FT_Create_Recur(x+1, y-1, cnt+1, minx, maxx);
    if ((x+1 >= minx && x+1 <= maxx) && y+1 < ROW_COUNT && !FT_check[x+1][y+1] && m_pGameLayer->IsConnected(x+1, y+1))
        FT_Create_Recur(x+1, y+1, cnt+1, minx, maxx);
    
    if (y+1 < ROW_COUNT && !FT_check[x][y+1])
        FT_Create_Recur(x, y+1, cnt+1, minx, maxx);
    if (y > 0 && !FT_check[x][y-1])
        FT_Create_Recur(x, y-1, cnt+1, minx, maxx);
    if ((x-1 >= minx && x-1 <= maxx) && !FT_check[x-1][y])
        FT_Create_Recur(x-1, y, cnt+1, minx, maxx);
    if ((x+1 >= minx && x+1 <= maxx) && !FT_check[x+1][y])
        FT_Create_Recur(x+1, y, cnt+1, minx, maxx);
}



