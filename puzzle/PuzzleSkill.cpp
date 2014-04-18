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
    for (int i = 0 ; i < num.size(); i++) {
        CCLog("%d번 스킬 작동할 것임", num[i]);
    }
    
    // init.
    for (int i = 0 ; i < NUMOFSKILL ; i++)
    {
        skillNumber[i] = false;
        skillProb[i] = 0;
        skillLevel[i] = 0;
    }
    
    // 유저가 현재 게임에서 쓸 수 있는 skill 번호들
    for (int i = 0 ; i < num.size() ; i++)
    {
        skillNumber[num[i]] = true; // possible skill 번호
        skillProb[num[i]] = prob[i]; // probability
        skillLevel[num[i]] = lv[i]; // level
    }
    
    // 각 스킬 관련 변수들 초기화
    isTimeSlowed = false;
    getPotion = false;
    
    for (int i = 0 ; i < 3; i++)
    {
        spiritShownCnt[i] = 0;
        isSpiritAlive[i] = false;
    }
}

void PuzzleSkill::TrySkills(int pieceColor)
{
    // Init
    for (int i = 0 ; i < NUMOFSKILL ; i++)
        skillApplied[i] = false;
    
    for (int i = 0 ; i < NUMOFSKILL ; i++)
    {
        if (i >= 24) // 24번부터 궁극 스킬인데 아직 없으니까 무조건 패스하자. 만들면 이거 지워라.
            break;
        
        if (i == 2 || i == 10 || i == 18) // 2B 시리즈는 게임이 끝나고 발동하는 것이므로 여기서는 제외.
            continue;
        
        if (skillNumber[i]) // 스킬 목록에 있어야만 발동함.
        {
            // Cycle - 2A
            if (i == 1) {
                if (pieceColor == PIECE_RED && m_pGameLayer->IsCycle())
                    Try(i);
            }
            else if (i == 9) {
                if (pieceColor == PIECE_BLUE && m_pGameLayer->IsCycle())
                    Try(i);
            }
            else if (i == 17) {
                if (pieceColor == PIECE_GREEN && m_pGameLayer->IsCycle())
                    Try(i);
            }
            // 자기 색깔 피스가 터져야 발동이 가능한 경우 - 1, 4A, 8
            else if (i == 0 || i == 7) {
                if (pieceColor == PIECE_RED)
                    Try(i);
            }
            else if (i == 8 || i == 15) {
                if (pieceColor == PIECE_BLUE)
                    Try(i);
            }
            else if (i == 16 || i == 23) {
                if (pieceColor == PIECE_GREEN)
                    Try(i);
            }
            
            // 6개이상 한번더 - 4B
            else if (i == 5) {
                if (pieceColor == PIECE_RED && m_pGameLayer->GetPiece8xy(false).size() >= 6)
                    Try(i);
            }
            else if (i == 13) {
                if (pieceColor == PIECE_BLUE && m_pGameLayer->GetPiece8xy(false).size() >= 6)
                    Try(i);
            }
            else if (i == 21) {
                if (pieceColor == PIECE_GREEN && m_pGameLayer->GetPiece8xy(false).size() >= 6)
                    Try(i);
            }
    
            // 10개 이상 제거 시 추가점수/추가사탕 - W3, E3
            else if (i == 11 || i == 19) {
                if (m_pGameLayer->GetPiece8xy(false).size() >= 10)
                    Try(i);
            }
            
            else
                Try(i);
        }
    }
}

void PuzzleSkill::Try(int skillNum)
{
    // 이번 라운드에 시전을 하는지, 그렇지 않은지 결정
    int prob = rand()%100;
    if (prob < skillProb[skillNum])
        skillApplied[skillNum] = true;
    
    if ((skillNum == 5 || skillNum == 13 || skillNum == 21) && skillApplied[skillNum])
    {
        // 6개 이상 제거 시 한 번 더 터뜨리는 스킬
        
        // 미리 위치를 저장하고,
        result_pos.clear();
        result_pos = m_pGameLayer->GetPiece8xy(false);
        
        // 그 위치의 8각 틀에 표시하기
    }
    
    // 퍼즐 lock걸어서 스킬 발동이 끝날 때까지 추가적인 한붓그리기를 못하게 한다.
    int i = skillNum;
    if (skillApplied[i])
    {
        // 잠시 i == 12 제외했음 //
        if (i == 1 || i == 9 || i == 17 || i == 3 || i == 4  || i == 20 || i == 5 || i == 13 || i == 21 || i == 6 || i == 7 || i == 15 || i == 23)
        {
            m_pGameLayer->SetSkillLock(true);
        }
    }
}

bool PuzzleSkill::IsApplied(int skillNum)
{
    return skillApplied[skillNum];
}

void PuzzleSkill::Invoke(int skillNum)
{
    // 스킬 목록에 없거나, 있더라도 시전이 되지 않은 경우(확률 실패한 경우), 실행하지 않는다.
    if (!skillNumber[skillNum] || !skillApplied[skillNum])
        return;
    
    // coco animation
    m_pGameLayer->schedule(schedule_selector(Puzzle::CocoAnim), 0.04f);
    
    switch (skillNum) {
        case 0:
            A1(skillNum); break;
        case 1:
            A2(skillNum); break;
        case 2:
            F3(skillNum); break;
        case 3:
            F4(skillNum); break;
        case 4:
            F5(skillNum); break;
        case 5:
            A6(skillNum); break;
        case 6:
            F7(skillNum); break;
        case 7:
            A8(skillNum); break;
            
        case 8:
            A1(skillNum); break;
        case 9:
            A2(skillNum); break;
        case 10:
            W3(skillNum); break;
        case 11:
            W4(skillNum); break;
        case 12:
            W5(skillNum); break;
        case 13:
            A6(skillNum); break;
        case 14:
            W7(); break;
        case 15:
            A8(skillNum); break;
            
        case 16:
            A1(skillNum); break;
        case 17:
            A2(skillNum); break;
        case 18:
            E3(skillNum); break;
        case 19:
            E4(skillNum); break;
        case 20:
            E5(skillNum); break;
        case 21:
            A6(skillNum); break;
        case 22:
            E7(); break;
        case 23:
            A8(skillNum); break;
            
        case 24:
            M1(skillNum); break;
        case 25:
            M2(skillNum); break;
        case 26:
            M3(skillNum); break;
        case 27:
            M4(skillNum); break;
        case 28:
            M5(skillNum); break;
    }
}

void PuzzleSkill::A1(int num)
{
    // 마법불꽃, 은은한 달빛, 대지의 숨결 - 각 색깔의 피스 제거 시, 추가점수
    A1_addedScore = 0;
    A1_addedScore = 300*skillLevel[num]*m_pGameLayer->GetPiece8xy(true).size();
    
    // 한붓그리기가 된 모든 피스의 각 중앙에 '+' 그림을 보여준다.
    m_pGameLayer->PlayEffect(num);
    
    // 사이클이 발동된 상태면, 사이클 이펙트도 같이 보여준다. (태양열, 파도, 클로버 그림 띄우는 것)
    if (m_pGameLayer->IsCycle())
        m_pGameLayer->PlayEffect(-(num+1));
}

void PuzzleSkill::A2(int num)
{
    // 불꽃송이, 파도타기, 땅울림 - 각 색깔의 피스를 사이클로 제거하면 스킬 레벨에 비례하여 정해진 방식대로 주변부 터뜨리기
    // (여기서는 주변부의 위치만 구한다)
    // 불2 : 스킬레벨 수만큼 사이클의 주변부를 동시에 터뜨린다.
    // 물2 : 스킬레벨 수만큼 사이클이 끝나는 방향으로 파도타듯이 터뜨린다.
    // 땅2 : 스킬레벨 수만큼 완전히 랜덤하게 터뜨린다.
    
    // 변수 초기화
    result_pos.clear();
    int x, y;
    int xy[COLUMN_COUNT][ROW_COUNT];
    
    for (int x = 0 ; x < COLUMN_COUNT ; x++)
        for (int y = 0 ; y < ROW_COUNT ; y++)
            xy[x][y] = 0;
    std::vector<CCPoint> pos = m_pGameLayer->GetPiece8xy(true);
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
                result_pos.push_back(selectPos[position]);
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
        result_pos.push_back(deltaPos);
        
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
            
            result_pos.push_back(ccp(x, y));
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
                result_pos.push_back(ccp(x, y));
            }
        }
        
    }
    
    // 이펙트 실행 (태양/달/파도 그림)
    m_pGameLayer->PlayEffect(num);
    
    // 폭파 실행
    if (num != 9) // 물 사이클 스킬은 이펙트와 함께 발동시켜야 하므로, 여기서 Bomb을 실행하지 않는다.
        m_pGameLayer->Bomb(result_pos);
}

/*std::vector<CCPoint> PuzzleSkill::A2GetPos()
{
    return A2_pos;
}
void PuzzleSkill::A2Clear()
{
    A2_pos.clear();
}*/


void PuzzleSkill::F3(int num)
{
    // 뜨거운 것이 좋아 - 10개 이상 피스 제거 시 추가 점수
    F3_addedScore = 0;
    F3_addedScore = skillLevel[num]*7000; // 나중에 보고 밸런스 조정
    
    // '+10' 화면에 보여준다.
    
    // '빛나는 하얀구체'를 마지막 피스 중앙에 보여주고 score칸으로 움직이는 action 발동
}

void PuzzleSkill::F4(int num)
{
    // 타오르는 마법진 - Magic Time(MT) 때 마법진을 2번 터뜨린다.
    F3_isDoubledMT = false;
    int prob = rand()%100;
    if (prob < skillProb[num])
    {
        F3_isDoubledMT = true;
    }
}

//void PuzzleSkill::F5Recur(int x, int y, int k)
//{
    /*
     F4A_check[x][y] = k;
     
     if (x > 0 && y > 0 && !F4A_check[x-1][y-1] && m_pGameLayer->IsConnected(x, y) && m_pGameLayer->GetPuzzleP8Set()->GetType(x-1, y-1) == PIECE_RED)
     { F4A_cnt++; F4ARecur(x-1, y-1, k); }
     if (x > 0 && y+1 < ROW_COUNT && !F4A_check[x-1][y+1] && m_pGameLayer->IsConnected(x, y+1) && m_pGameLayer->GetPuzzleP8Set()->GetType(x-1, y+1) == PIECE_RED)
     { F4A_cnt++; F4ARecur(x-1, y+1, k); }
     if (x+1 < COLUMN_COUNT && y > 0 && !F4A_check[x+1][y-1] && m_pGameLayer->IsConnected(x+1, y) && m_pGameLayer->GetPuzzleP8Set()->GetType(x+1, y-1) == PIECE_RED)
     { F4A_cnt++; F4ARecur(x+1, y-1, k); }
     if (x+1 < COLUMN_COUNT && y+1 < ROW_COUNT && !F4A_check[x+1][y+1] && m_pGameLayer->IsConnected(x+1, y+1) && m_pGameLayer->GetPuzzleP8Set()->GetType(x+1, y+1) == PIECE_RED)
     { F4A_cnt++; F4ARecur(x+1, y+1, k); }
     
     if (y+1 < ROW_COUNT && !F4A_check[x][y+1] && m_pGameLayer->GetPuzzleP8Set()->GetType(x, y+1) == PIECE_RED)
     { F4A_cnt++; F4ARecur(x, y+1, k); }
     if (y > 0 && !F4A_check[x][y-1] && m_pGameLayer->GetPuzzleP8Set()->GetType(x, y-1) == PIECE_RED)
     { F4A_cnt++; F4ARecur(x, y-1, k); }
     if (x > 0 && !F4A_check[x-1][y] && m_pGameLayer->GetPuzzleP8Set()->GetType(x-1, y) == PIECE_RED)
     { F4A_cnt++; F4ARecur(x-1, y, k); }
     if (x+1 < COLUMN_COUNT && !F4A_check[x+1][y] && m_pGameLayer->GetPuzzleP8Set()->GetType(x+1, y) == PIECE_RED)
     { F4A_cnt++; F4ARecur(x+1, y, k); }
     */
//}

void PuzzleSkill::SpiritTry(int type)
{
    int skillNum = 4+type*8;
    
    // 정령을 생성하는지 결정
    skillApplied[skillNum] = false;
    int prob = rand()%100;
    if (prob < skillProb[skillNum])
        skillApplied[skillNum] = true;
    
    // 스킬 목록에 없거나, 있더라도 시전이 되지 않은 경우(확률 실패한 경우), 실행하지 않는다.
    if (!skillNumber[skillNum] || !skillApplied[skillNum])
        return;
    
    // 이미 출현 회수를 초과했거나 아직 정령이 살아있는 경우도 무시한다.
    if (spiritShownCnt[type] >= skillLevel[skillNum] || isSpiritAlive[type])
        return;
    
    CCLog("정령 생성 !");
    spiritShownCnt[type]++;
    isSpiritAlive[type] = true;
    
    // effect 발생 (정령 생성)
    GetEffect()->SetSpirit(type);
}
void PuzzleSkill::F5(int num)
{
    // 위험한 불장난 - 모서리에 나타나는 불의 정령을 선택하면 모든 붉은 피스를 정중앙으로 모아준다.
    result_pos.clear();
    result_pos_end.clear();
    
    // 모든 붉은 피스 위치를 구한다.
    for (int x = 0 ; x < COLUMN_COUNT ; x++)
    {
        for (int y = 0 ; y < ROW_COUNT ; y++)
        {
            // 네 모서리에 위치한 존재하지 않는 부분
            if ((x == 0 && y == 0) || (x == 0 && y == ROW_COUNT-1) ||
                (x == COLUMN_COUNT-1 && y == 0) || (x == COLUMN_COUNT-1 && y == ROW_COUNT-1))
                continue;
            
            if (m_pGameLayer->GetPuzzleP8Set()->GetType(x, y) == PIECE_RED)
                result_pos.push_back(ccp(x, y));
        }
    }
    
    // 바꿔치기할 위치를 구한다. (정중앙부터 달팽이 모양으로)
    int dx = 1, dy = 0;
    int curX = 3, curY = 3;
    int cnt = 0, dcnt = 1;
    for (int i = 0 ; i < result_pos.size() ; i++)
    {
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
    
    // 위치가 바뀌지 않는 피스를 찾아서, 액션을 취하지 않도록 한다.
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
    
    for (int i = 0 ; i < result_pos.size() ; i++)
    {
        if (result_pos[i].x == -1 && result_pos[i].y == -1)
            continue;
        for (int j = 0 ; j < result_pos_end.size() ; j++)
        {
            if (result_pos_end[j].x == -1 && result_pos_end[j].y == -1)
                continue;
            
            CCLog ("(%d, %d) , (%d, %d)", (int)result_pos[i].x, (int)result_pos[i].y, (int)result_pos_end[i].x, (int)result_pos_end[i].y);
            
            int x = (int)result_pos[i].x;
            int y = (int)result_pos[i].y;
            int x_end = (int)result_pos_end[j].x;
            int y_end = (int)result_pos_end[j].y;
            
            result_pos_end[j] = ccp(-1, -1);
            
            CCActionInterval* action = CCMoveTo::create(0.35f, m_pGameLayer->SetPiece8Position(x_end, y_end));
            m_pGameLayer->GetSpriteP8(x, y)->runAction(action);
            CCActionInterval* action2 = CCSequence::create(CCMoveTo::create(0.35f, m_pGameLayer->SetPiece8Position(x, y)),
                        CCCallFuncND::create(m_pGameLayer, callfuncND_selector(PuzzleSkill::F5_Callback), NULL), NULL);
            m_pGameLayer->GetSpriteP8(x_end, y_end)->runAction(action2);
            
            break;
        }
    }
    
    GetEffect()->GetSpirit(0)->setDuration(0.5f);
    GetEffect()->GetSpirit(0)->setAutoRemoveOnFinish(true);
    GetEffect()->ReleaseSpirit(0);
    
    isSpiritAlive[0] = false;
}
void PuzzleSkill::F5_Callback(CCNode* sender, void* data)
{
    // 8각형 피스 swap
    m_pGameLayer->GetPuzzleP8Set()->SwapObject(x, y, x_end, y_end);
    
    // 4각형 피스들 갱신
    for (int x = 1; x < COLUMN_COUNT ; x++)
    {
        for (int y = 1 ; y < ROW_COUNT ; y++)
        {
            m_pGameLayer->GetPuzzleP4Set()->SetType(x, y, -m_pGameLayer->GetPuzzleP4Set()->GetType(x, y));
            //if (puzzleP4set->GetType(x, y) != BLOCKED)
            //{
            if (m_pGameLayer->GetPuzzleP4Set()->GetObject(x, y) != NULL)
                m_pGameLayer->GetPuzzleP4Set()->RemoveChild(x, y);
            m_pGameLayer->GetPuzzleP4Set()->CreatePiece(x, y, m_pGameLayer->GetPuzzleP4Set()->GetType(x, y));
            m_pGameLayer->GetPuzzleP4Set()->AddChild(x, y);
            //}
        }
    }
}

void PuzzleSkill::F5Recur(int x, int y, int type, std::vector<CCPoint>& v)
{
    F5_check[x][y] = true;
    v.push_back(ccp(x, y));
    
    if (x > 0 && y > 0 && !F5_check[x-1][y-1] && m_pGameLayer->IsConnected(x, y) && m_pGameLayer->GetPuzzleP8Set()->GetType(x-1, y-1) == type)
        F5Recur(x-1, y-1, type, v);
    if (x > 0 && y+1 < ROW_COUNT && !F5_check[x-1][y+1] && m_pGameLayer->IsConnected(x, y+1) && m_pGameLayer->GetPuzzleP8Set()->GetType(x-1, y+1) == type)
        F5Recur(x-1, y+1, type, v);
    if (x+1 < COLUMN_COUNT && y > 0 && !F5_check[x+1][y-1] && m_pGameLayer->IsConnected(x+1, y) && m_pGameLayer->GetPuzzleP8Set()->GetType(x+1, y-1) == type)
        F5Recur(x+1, y-1, type, v);
    if (x+1 < COLUMN_COUNT && y+1 < ROW_COUNT && !F5_check[x+1][y+1] && m_pGameLayer->IsConnected(x+1, y+1) && m_pGameLayer->GetPuzzleP8Set()->GetType(x+1, y+1) == type)
        F5Recur(x+1, y+1, type, v);
    
    if (y+1 < ROW_COUNT && !F5_check[x][y+1] && m_pGameLayer->GetPuzzleP8Set()->GetType(x, y+1) == type)
        F5Recur(x, y+1, type, v);
    if (y > 0 && !F5_check[x][y-1] && m_pGameLayer->GetPuzzleP8Set()->GetType(x, y-1) == type)
        F5Recur(x, y-1, type, v);
    if (x > 0 && !F5_check[x-1][y] && m_pGameLayer->GetPuzzleP8Set()->GetType(x-1, y) == type)
        F5Recur(x-1, y, type, v);
    if (x+1 < COLUMN_COUNT && !F5_check[x+1][y] && m_pGameLayer->GetPuzzleP8Set()->GetType(x+1, y) == type)
        F5Recur(x+1, y, type, v);
}

void PuzzleSkill::A6(int num)
{
    // 불꽃놀이, 얼음비, 땅의 신비 - 각자의 피스 제거 시 (6개 이상) 일정 확률로 그 위치를 한 번 더 터뜨리기
    
    // 사운드
    m_pGameLayer->GetSound()->PlaySkillSound(num);
    
    // 이펙트 실행
    m_pGameLayer->PlayEffect(num);
    
    // 폭파
    m_pGameLayer->Bomb(result_pos);
    
    // 틀의 색깔 원상복귀
}
/*std::vector<CCPoint> PuzzleSkill::A6GetPos()
{
    return result_pos;
}
void PuzzleSkill::A6Clear()
{
    result_pos.clear();
}*/

void PuzzleSkill::F7(int num)
{
    // 코코 타임 : 3~5 랜덤한 횟수로 한붓그리기 한번에 자동 진행한다. (한붓그리기 위치는 랜덤)
    
    // init
    for (int i = 0 ; i < result_double_pos.size() ; i++)
        result_double_pos[i].clear();
    result_double_pos.clear();
    
    int count = rand()%3 + 3;
    for (int i = 0 ; i < count ; i++)
    {
        std::vector<CCPoint> temp;
        result_double_pos.push_back(temp);
    }
    
    // 변수 init
    for (int x = 0 ; x < COLUMN_COUNT ; x++)
        for (int y = 0 ; y < ROW_COUNT ; y++)
            F5_check[x][y] = false;
    
    // 한붓그리기(3피스 이상)가 가능한 덩어리들을 모은다.
    int i = 0;
    for (int x = 0 ; x < COLUMN_COUNT ; x++)
    {
        for (int y = 0 ; y < ROW_COUNT ; y++)
        {
            // 네 모서리에 위치한 존재하지 않는 부분
            if ((x == 0 && y == 0) || (x == 0 && y == ROW_COUNT-1) ||
                (x == COLUMN_COUNT-1 && y == 0) || (x == COLUMN_COUNT-1 && y == ROW_COUNT-1))
                continue;
            
            if (!F5_check[x][y] && i < count)
            {
                F5Recur(x, y, m_pGameLayer->GetPuzzleP8Set()->GetType(x, y), result_double_pos[i]);
                if ((int)result_double_pos[i].size() < 3)
                {
                    for (int j = 0 ; j < result_double_pos[i].size() ; j++)
                        F5_check[(int)result_double_pos[i][j].x][(int)result_double_pos[i][j].y] = false;
                    result_double_pos[i].clear();
                }
                else
                {
                    i++;
                }
            }
        }
    }
    
    // 하나의 vector로 결과를 만든다. (좌표를 양수/음수로 몇 번째 한붓그리기 덩어리인지 구분하도록 한다)
    result_pos.clear();
    for (int i = 0 ; i < count ; i++)
    {
        for (int j = 0 ; j < result_double_pos[i].size() ; j++)
        {
            if (i % 2 == 0)
                result_pos.push_back(F5_pos[i][j]);
            else
                result_pos.push_back(ccp(-(int)result_double_pos[i][j].x, -(int)result_double_pos[i][j].y));
        }
    }
}

void PuzzleSkill::A8(int num)
{
    // 붉은 용의 숨결, 여신의 은총, 고대나무의 비밀
    // (발동조건은 다름) 모든 각 색깔 피스와 스킬레벨에 비례한 수만큼의 주변 피스를 제거.
    
    // 변수 초기화
    std::vector<CCPoint> temp;
    result_pos.clear();
    A8_callbackCnt = 0;
    A8_cnt = 0;
    int curType = m_pGameLayer->GetGlobalType();
    bool check[COLUMN_COUNT][ROW_COUNT];
    for (int x = 0 ; x < COLUMN_COUNT ; x++)
        for (int y = 0 ; y < ROW_COUNT ; y++)
            check[x][y] = false;
    
    // 한붓그리기 된 것들을 포함하여 모든 현재 색깔 피스를 구한다.
    for (int x = 0 ; x < COLUMN_COUNT ; x++)
    {
        for (int y = 0 ; y < ROW_COUNT ; y++)
        {
            // 네 모서리에 위치한 존재하지 않는 부분
            if ((x == 0 && y == 0) || (x == 0 && y == ROW_COUNT-1) ||
                (x == COLUMN_COUNT-1 && y == 0) || (x == COLUMN_COUNT-1 && y == ROW_COUNT-1))
                continue;
            
            if (m_pGameLayer->GetPuzzleP8Set()->GetType(x, y) == curType)
                check[x][y] = true;
            //A8_cnt++;
        }
    }
    
    // 가능한 주변 위치를 구한다.
    for (int x = 0 ; x < COLUMN_COUNT ; x++)
    {
        for (int y = 0 ; y < ROW_COUNT ; y++)
        {
            // 네 모서리에 위치한 존재하지 않는 부분
            if ((x == 0 && y == 0) || (x == 0 && y == ROW_COUNT-1) ||
                (x == COLUMN_COUNT-1 && y == 0) || (x == COLUMN_COUNT-1 && y == ROW_COUNT-1))
                continue;
            
            // 자기 자신이나 4방향 옆이 현재 색깔이면 이 위치는 폭파될 위치이므로 추가하자.
            if (!check[x][y])
            {
                if ((x > 0 && check[x-1][y]) || (y > 0 && check[x][y-1]) ||
                    (x+1 < COLUMN_COUNT && check[x+1][y]) || (y+1 < ROW_COUNT && check[x][y+1]) ||
                    (x > 0 && y > 0 && check[x-1][y-1]) || (x > 0 && y+1 < ROW_COUNT && check[x-1][y+1]) ||
                    (x+1 < COLUMN_COUNT && y > 0 && check[x+1][y-1]) ||
                    (x+1 < COLUMN_COUNT && y+1 < ROW_COUNT && check[x+1][y+1]) )
                {
                    temp.push_back(ccp(x, y));
                }
            }
            /*
             PuzzleP8Set* puzzleP8set = m_pGameLayer->GetPuzzleP8Set();
             if (m_pGameLayer->GetPuzzleP8Set()->GetType(x, y) == curType ||
             (x > 0 && !(x-1 == 0 && (y == 0 || y == ROW_COUNT-1)) && puzzleP8set->GetType(x-1, y) == curType) ||
             (y > 0 && !((x == 0 || x == COLUMN_COUNT-1) && y-1 == 0) && puzzleP8set->GetType(x, y-1) == curType) ||
             (x < COLUMN_COUNT-1 && !(x+1 == COLUMN_COUNT-1 && (y == 0 || y == ROW_COUNT-1)) && puzzleP8set->GetType(x+1, y) == curType) ||
             (y < ROW_COUNT-1 && !((x == 0 || x == COLUMN_COUNT-1) && y+1 == ROW_COUNT-1) && puzzleP8set->GetType(x, y+1) == curType) ||
             
             (x > 0 && y > 0 && !(x-1 == 0 && y-1 == 0) && m_pGameLayer->IsConnected(x, y) && puzzleP8set->GetType(x-1, y-1) == curType) ||
             (x > 0 && y+1 < ROW_COUNT && !(x-1 == 0 && y+1 == ROW_COUNT) && m_pGameLayer->IsConnected(x, y+1) && puzzleP8set->GetType(x-1, y+1) == curType) ||
             (x+1 < COLUMN_COUNT && y > 0 && !(x+1 == COLUMN_COUNT && y-1 == 0) && m_pGameLayer->IsConnected(x+1, y) && puzzleP8set->GetType(x+1, y-1) == curType) ||
             (x+1 < COLUMN_COUNT && y+1 < ROW_COUNT && !(x+1 == COLUMN_COUNT && y+1 == ROW_COUNT) && m_pGameLayer->IsConnected(x+1, y+1) && puzzleP8set->GetType(x+1, y+1) == curType) )
             {
             temp.push_back(ccp(x, y));
             }
             */
        }
    }
    
    // 랜덤하게 주변 위치 중 터뜨릴 것을 결정한다.
    int cnt = 0, k;
    while (cnt < skillLevel[num] && cnt < (int)temp.size())
    {
        k = rand() % (int)temp.size();
        if ((int)temp[k].x == -1 && (int)temp[k].y == -1)
            continue;
        
        result_pos.push_back(temp[k]);
        temp[k] = ccp(-1, -1);
    }
    
    temp.clear();
    
    //m_pGameLayer->PlayEffect(num);
}
/*
std::vector<CCPoint> PuzzleSkill::A8GetPos()
{
    return A8_pos;
}
void PuzzleSkill::A8Clear()
{
    A8_pos.clear();
}
*/


void PuzzleSkill::W3(int num)
{
    // 한방울 한방울 - 콤보에 비례한 추가 점수
    W2B_addedScore = 0;
    W2B_addedScore = pow(m_pGameLayer->GetCombo(), 0.9f) * (skillLevel[num]*100 + 550);
}

void PuzzleSkill::W4(int num)
{
    // 바다 속 진주 - 콤보에 비례한 추가 별사탕
    W3_addedCandy = 0;
    W3_addedCandy = pow(m_pGameLayer->GetCombo(), 0.8f) * (skillLevel[num]*0.3f + 0.7f);
}

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
}

void PuzzleSkill::W7()
{
    // 시간을 얼리다 - 5초 동안 시간을 2배속 늦춘다.
    if (!isTimeSlowed)
    {
        isTimeSlowed = true;
        m_pGameLayer->schedule(schedule_selector(PuzzleSkill::W7Timer), 5.0f);
    }
}
void PuzzleSkill::W7Timer(float f)
{
    // W5 스킬에 대한 timer
    isTimeSlowed = false;
    m_pGameLayer->unschedule(schedule_selector(PuzzleSkill::W7Timer));
}
bool PuzzleSkill::W7GetVar()
{
    return isTimeSlowed;
}







void PuzzleSkill::E3(int num)
{
    // 떡갈나무지팡이 - 지팡이 레벨에 비례한 추가 별사탕
    E2B_addedCandy = 0;
    //E2B_addedCandy = pow((m_pGameLayer->GetStaffLevel())*3+65, 0.8f) * (skillLevel[num]*0.3f + 0.7f);
}

void PuzzleSkill::E4(int num)
{
    // 아낌없이 주는 나무 - 10개 이상 피스 제거 시 추가 별사탕
    E3_addedCandy = 0;
    E3_addedCandy = skillLevel[num]*100; // 나중에 보고 밸런스 조정
    
    // '+10' 화면에 보여준다.
    
    // '별사탕'을 마지막 피스 중앙에 보여주고 score칸으로 움직이는 action 발동
}




void PuzzleSkill::E5(int num)
{
    // 자연의 힘 - 땅의 정령을 터치한 뒤 어떤 피스들이 터지면, 그만큼 더 터진다.
    /*
    result_pos.clear();
    for (int x = 0; x < 3; x++) {
        for (int y = ROW_COUNT-3; y < ROW_COUNT; y++) {
            if (x == 0 && y == ROW_COUNT-1)
                continue;
            result_pos.push_back(ccp(x, y));
        }
    }
    for (int x = COLUMN_COUNT-3; x < COLUMN_COUNT; x++) {
        for (int y = 0; y < 3; y++) {
            if (x == COLUMN_COUNT-1 && y == 0)
                continue;
            result_pos.push_back(ccp(x, y));
        }
    }
     */
}

/*std::vector<CCPoint> PuzzleSkill::A4AGetPos()
{
    return A4A_pos;
}
void PuzzleSkill::A4AClear()
{
    A4A_pos.clear();
}*/

void PuzzleSkill::E7()
{
    // 끈질긴 생명력 - 포션을 1개 얻는다. 한 번 얻으면 더 이상 발동되지 않는다.
    if (!getPotion)
    {
        getPotion = true;
        
        // 화면에 보여주는 애니메이션 구현
    }
}

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
std::vector<CCPoint> PuzzleSkill::GetResultEnd()
{
    return result_pos_end;
}
void PuzzleSkill::ResultClear()
{
    result_pos.clear();
}
