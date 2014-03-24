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
    W5_isApplied = false;
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
            else if (i == 0 || i == 4 || i == 7) {
                if (pieceColor == PIECE_RED)
                    Try(i);
            }
            //else if (i == 8 || i == 12 || i == 15) {
            else if (i == 8 || i == 15) {
                if (pieceColor == PIECE_BLUE)
                    Try(i);
            }
            else if (i == 16 || i == 20 || i == 23) {
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
        CCLog("Try (%d) : more than 6", skillNum);
        
        // 미리 위치를 저장하고,
        A4B_pos.clear();
        A4B_pos = m_pGameLayer->GetPiece8xy(false);
        CCLog("a4b pos size : %d", (int)A4B_pos.size());
        
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
    // 잠시 추가
    if (skillNum != 12) {
    // 스킬 목록에 없거나, 있더라도 시전이 되지 않은 경우(확률 실패한 경우), 실행하지 않는다.
    if (!skillNumber[skillNum] || !skillApplied[skillNum])
        return;
    }
    
    switch (skillNum) {
        case 0:
            A1(skillNum); break;
        case 1:
            A2A(skillNum); break;
        case 2:
            F2B(skillNum); break;
        case 3:
            F3(skillNum); break;
        case 4:
            F4A(); break;
        case 5:
            A4B(); break;
        case 6:
            F5(); break;
        case 7:
            A8(); break;
            
        case 8:
            A1(skillNum); break;
        case 9:
            A2A(skillNum); break;
        case 10:
            W2B(skillNum); break;
        case 11:
            W3(skillNum); break;
        case 12:
            W4A(); break;
        case 13:
            A4B(); break;
        case 14:
            W5(); break;
        case 15:
            A8(); break;
            
        case 16:
            A1(skillNum); break;
        case 17:
            A2A(skillNum); break;
        case 18:
            E2B(skillNum); break;
        case 19:
            E3(skillNum); break;
        case 20:
            E4A(); break;
        case 21:
            A4B(); break;
        case 22:
            E5(); break;
        case 23:
            A8(); break;
    }
}

void PuzzleSkill::A1(int num)
{
    // 마법불꽃, 은은한달빛, 대지의숨결 - 각 색깔의 피스 제거 시, 추가점수
    A1_addedScore = 0;
    A1_addedScore = 300*skillLevel[num]*m_pGameLayer->GetPiece8xy(true).size();
    
    // 한붓그리기가 된 모든 피스의 각 중앙에 '+' 그림을 보여준다.
}

void PuzzleSkill::A2A(int num)
{
    // 불꽃송이, 물방울터뜨리기, 뾰족한바위 - 각 색깔의 피스를 사이클로 제거하면 일정 확률로 그 수만큼 주변이 연달아 더 터지기
    // (여기서는 주변부의 위치만 구한다)
    CCLog("A2A skill");
        
    A2A_pos.clear();
        
    bool xy[COLUMN_COUNT][ROW_COUNT];
    for (int x = 0 ; x < COLUMN_COUNT ; x++)
        for (int y = 0 ; y < ROW_COUNT ; y++)
            xy[x][y] = false;

    std::vector<CCPoint> pos = m_pGameLayer->GetPiece8xy(true);
    for (int i = 0 ; i < pos.size() ; i++)
        xy[(int)pos[i].x][(int)pos[i].y] = true;
    
    CCLog("pos size : %d", (int)pos.size());
    int x, y;
    int temp = rand()%4;
    if (temp == 0)
    {
        for (x = 0 ; x < COLUMN_COUNT ; x++) {
            for (y = 0 ; y < ROW_COUNT ; y++) {
                if ((x == 0 && y == 0) || (x == 0 && y == ROW_COUNT-1) ||
                    (x == COLUMN_COUNT-1 && y == 0) || (x == COLUMN_COUNT-1 && y == ROW_COUNT-1))
                    continue;
                if (xy[x][y])
                    continue;
                if ((y+1 < ROW_COUNT && xy[x][y+1]) || (y-1 >= 0 && xy[x][y-1]) ||
                    (x+1 < COLUMN_COUNT && xy[x+1][y]) || (x-1 >= 0 && xy[x-1][y]))
                {
                    if (A2A_pos.size() < pos.size())
                        A2A_pos.push_back(ccp(x, y));
                }
            }
        }
    }
    else if (temp == 1)
    {
        for (x = 0 ; x < COLUMN_COUNT ; x++) {
            for (y = ROW_COUNT-1 ; y >= 0 ; y--) {
                if ((x == 0 && y == 0) || (x == 0 && y == ROW_COUNT-1) ||
                    (x == COLUMN_COUNT-1 && y == 0) || (x == COLUMN_COUNT-1 && y == ROW_COUNT-1))
                    continue;
                if (xy[x][y])
                    continue;
                if ((y+1 < ROW_COUNT && xy[x][y+1]) || (y-1 >= 0 && xy[x][y-1]) ||
                    (x+1 < COLUMN_COUNT && xy[x+1][y]) || (x-1 >= 0 && xy[x-1][y]))
                {
                    if (A2A_pos.size() < pos.size())
                        A2A_pos.push_back(ccp(x, y));
                }
            }
        }
    }
    else if (temp == 2)
    {
        for (y = 0 ; y < ROW_COUNT ; y++) {
            for (x = COLUMN_COUNT-1 ; x >= 0 ; x--) {
                if ((x == 0 && y == 0) || (x == 0 && y == ROW_COUNT-1) ||
                    (x == COLUMN_COUNT-1 && y == 0) || (x == COLUMN_COUNT-1 && y == ROW_COUNT-1))
                    continue;
                if (xy[x][y])
                    continue;
                if ((y+1 < ROW_COUNT && xy[x][y+1]) || (y-1 >= 0 && xy[x][y-1]) ||
                    (x+1 < COLUMN_COUNT && xy[x+1][y]) || (x-1 >= 0 && xy[x-1][y]))
                {
                    if (A2A_pos.size() < pos.size())
                        A2A_pos.push_back(ccp(x, y));
                }
            }
        }
    }
    else
    {
        for (y = 0 ; y < ROW_COUNT ; y++) {
            for (x = 0 ; x < COLUMN_COUNT ; x++) {
                if ((x == 0 && y == 0) || (x == 0 && y == ROW_COUNT-1) ||
                    (x == COLUMN_COUNT-1 && y == 0) || (x == COLUMN_COUNT-1 && y == ROW_COUNT-1))
                    continue;
                if (xy[x][y])
                    continue;
                if ((y+1 < ROW_COUNT && xy[x][y+1]) || (y-1 >= 0 && xy[x][y-1]) ||
                    (x+1 < COLUMN_COUNT && xy[x+1][y]) || (x-1 >= 0 && xy[x-1][y]))
                {
                    if (A2A_pos.size() < pos.size())
                        A2A_pos.push_back(ccp(x, y));
                }
            }
        }
    }
    
    // 사이클 한붓그리기 개수만큼 채우지 못했다면, 랜덤한 위치에 마저 채운다.
    if (A2A_pos.size() < pos.size())
    {
        CCLog("remaining...");
        for (int i = 0 ; i < A2A_pos.size() ; i++)
            xy[(int)A2A_pos[i].x][(int)A2A_pos[i].y] = true;
        
        while(1)
        {
            x = rand()%COLUMN_COUNT;
            y = rand()%ROW_COUNT;
            if (!xy[x][y])
            {
                A2A_pos.push_back(ccp(x, y));
                xy[x][y] = true;
                if (A2A_pos.size() == pos.size())
                    break;
            }
        }
    }
    
    //CCLog("a2a pos size : %d", (int)A2A_pos.size());
    
    // 폭파 실행
    m_pGameLayer->Bomb(A2A_pos);
}
std::vector<CCPoint> PuzzleSkill::A2AGetPos()
{
    return A2A_pos;
}
void PuzzleSkill::A2AClear()
{
    A2A_pos.clear();
}


void PuzzleSkill::F2B(int num)
{
    // 뜨거운 것이 좋아 - 콤보에 비례한 추가 별사탕
    F2B_addedCandy = 0;
    F2B_addedCandy = pow(m_pGameLayer->GetCombo(), 0.8f) * (skillLevel[num]*0.3f + 0.7f);
}

void PuzzleSkill::W2B(int num)
{
    // 끝없는 물결 - 콤보에 비례한 추가 점수
    W2B_addedScore = 0;
    W2B_addedScore = pow(m_pGameLayer->GetCombo(), 0.9f) * (skillLevel[num]*100 + 550);
}

void PuzzleSkill::E2B(int num)
{
    // 떡갈나무지팡이 - 지팡이 레벨에 비례한 추가 별사탕
    E2B_addedCandy = 0;
    //E2B_addedCandy = pow((m_pGameLayer->GetStaffLevel())*3+65, 0.8f) * (skillLevel[num]*0.3f + 0.7f);
}

void PuzzleSkill::F3(int num)
{
    // 타오르는 열정 - Magic Time(MT) 때 마법진을 2번 터뜨린다.
    F3_isDoubledMT = false;
    int prob = rand()%100;
    if (prob < skillProb[num])
    {
        F3_isDoubledMT = true;
    }
}

void PuzzleSkill::W3(int num)
{
    // 바다 속 진주 - 10개 이상 피스 제거 시 추가 점수
    W3_addedScore = 0;
    W3_addedScore = skillLevel[num]*7000; // 나중에 보고 밸런스 조정
    
    // '+10' 화면에 보여준다.
    
    // '빛나는 하얀구체'를 마지막 피스 중앙에 보여주고 score칸으로 움직이는 action 발동
}

void PuzzleSkill::E3(int num)
{
    // 아낌없이 주는 나무 - 10개 이상 피스 제거 시 추가 별사탕
    E3_addedCandy = 0;
    E3_addedCandy = skillLevel[num]*100; // 나중에 보고 밸런스 조정
    
    // '+10' 화면에 보여준다.
    
    // '별사탕'을 마지막 피스 중앙에 보여주고 score칸으로 움직이는 action 발동
}

void PuzzleSkill::F4A()
{
    // 위험한 불장난 - red피스 제거 시 일정 확률로 불지르기
    A4A_pos.clear();
    int x, y;
    
    x = 0; y = 5;
    while (y >= 0)
    {
        A4A_pos.push_back(ccp(x, y));
        x++; y--;
    }
    x = 1; y = 5;
    while (y > 0)
    {
        A4A_pos.push_back(ccp(x, y));
        x++; y--;
    }
    x = 1; y = 6;
    while (y > 0)
    {
        A4A_pos.push_back(ccp(x, y));
        x++; y--;
    }
}

void PuzzleSkill::W4A()
{
    // 파도타기 - 오른쪽상단 터치 시 일정 개수만큼 노/회 -> 파 로 바꾸기
    /*
    int r = rand()%2;
    if (r == 0) r = 1;
    else if (r == 1) r = 4;
    */
    
    /*
    A4A_pos.clear();
    int cnt = 0;
    
    std::vector<CCPoint> pos;
    for (int x = 0 ; x < COLUMN_COUNT ; x++)
    {
        for (int y = 0 ; y < ROW_COUNT ; y++)
        {
            // 네 모서리에 위치한 존재하지 않는 부분
            if ((x == 0 && y == 0) || (x == 0 && y == ROW_COUNT-1) ||
                (x == COLUMN_COUNT-1 && y == 0) || (x == COLUMN_COUNT-1 && y == ROW_COUNT-1))
                continue;
            
            //int curType = m_pGameLayer->GetGlobalType();
            
            // 노란색이나 회색 피스면, 정해진 개수까지 이를 파란색으로 바꾼다.
            if (m_pGameLayer->GetP8Type(x, y) == 1 || m_pGameLayer->GetP8Type(x, y) == 4) {
                pos.push_back(ccp(x, y));
            }
        }
    }
    
    if (pado_value >= pos.size())
        A4A_pos = pos;
    else {
        int k;
        while(A4A_pos.size() < pado_value) {
            k = rand()%pos.size();
            if ((int)pos[k].x == -1 && (int)pos[k].y == -1)
                continue;
            
            A4A_pos.push_back(pos[k]);
            pos[k] = ccp(-1, -1);
            
        }
    }
     */
    
    /*
    // 파도타기 - blue피스 제거 시 일정 확률로 물내리기
    A4A_pos.clear();
    for (int y = 1; y < ROW_COUNT-1; y++)
        A4A_pos.push_back(ccp(0, y));
    for (int y = 0; y < ROW_COUNT; y++)
        A4A_pos.push_back(ccp(1, y));
    for (int x = 1; x < COLUMN_COUNT-1; x++)
        A4A_pos.push_back(ccp(x, 0));
    for (int x = 0; x < COLUMN_COUNT; x++)
        A4A_pos.push_back(ccp(x, 1));
     */
}

void PuzzleSkill::E4A()
{
    // 대지의 울림 - green피스 제거 시 일정 확률로 땅꺼지기
    A4A_pos.clear();
    for (int x = 0; x < 3; x++) {
        for (int y = ROW_COUNT-3; y < ROW_COUNT; y++) {
            if (x == 0 && y == ROW_COUNT-1)
                continue;
            A4A_pos.push_back(ccp(x, y));
        }
    }
    for (int x = COLUMN_COUNT-3; x < COLUMN_COUNT; x++) {
        for (int y = 0; y < 3; y++) {
            if (x == COLUMN_COUNT-1 && y == 0)
                continue;
            A4A_pos.push_back(ccp(x, y));
        }
    }
}
std::vector<CCPoint> PuzzleSkill::A4AGetPos()
{
    return A4A_pos;
}
void PuzzleSkill::A4AClear()
{
    A4A_pos.clear();
}

void PuzzleSkill::A4B()
{
    // 불꽃놀이, 얼음비, 땅의 신비 - 각자의 피스 제거 시 (6개 이상) 일정 확률로 그 위치를 한 번 더 터뜨리기
    CCLog("A4B()");
    m_pGameLayer->Bomb(A4B_pos);
    
    // 틀의 색깔 원상복귀
}
std::vector<CCPoint> PuzzleSkill::A4BGetPos()
{
    return A4B_pos;
}
void PuzzleSkill::A4BClear()
{
    A4B_pos.clear();
}

void PuzzleSkill::F5()
{
    // init
    for (int i = 0 ; i < F5_pos.size() ; i++)
        F5_pos[i].clear();
    F5_pos.clear();
    
    int count = rand()%3 + 3;
    for (int i = 0 ; i < count ; i++)
    {
        // 한붓그리기는 한번에 몇 개? 어떻게 그을까?
    }
}

void PuzzleSkill::W5()
{
    CCLog("W5W5W5W5W5W5W5W5W5W5W5W5");
    // 시간을 얼리다 - 5초 동안 시간을 2배속 늦춘다.
    if (!W5_isApplied)
    {
        CCLog("W5 (time *2) applied");
        W5_isApplied = true;
        m_pGameLayer->schedule(schedule_selector(PuzzleSkill::W5Timer), 5.0f);
    }
}
void PuzzleSkill::W5Timer(float f)
{
    // W5 스킬에 대한 timer
    W5_isApplied = false;
    m_pGameLayer->unschedule(schedule_selector(PuzzleSkill::W5Timer));
}
bool PuzzleSkill::W5GetVar()
{
    return W5_isApplied;
}

void PuzzleSkill::E5()
{
    // 끈질긴 생명력 - 포션을 1개 얻는다. 한 번 얻으면 더 이상 발동되지 않는다.
    if (!E5_getPotion)
    {
        E5_getPotion = true;
        
        // 화면에 보여주는 애니메이션 구현
    }
}


void PuzzleSkill::A8()
{
    // 8번 스킬 준비 (자기 색깔 변화주기)
    A8_pos.clear();
    A8_callbackCnt = 0;
    A8_cnt = 0;
    
    for (int x = 0 ; x < COLUMN_COUNT ; x++)
    {
        for (int y = 0 ; y < ROW_COUNT ; y++)
        {
            // 네 모서리에 위치한 존재하지 않는 부분
            if ((x == 0 && y == 0) || (x == 0 && y == ROW_COUNT-1) ||
                (x == COLUMN_COUNT-1 && y == 0) || (x == COLUMN_COUNT-1 && y == ROW_COUNT-1))
                continue;

            int curType = m_pGameLayer->GetGlobalType();
            
            // 현재 색깔 피스의 개수를 센다. 음영처리 후의 callback에 사용된다.
            if (m_pGameLayer->GetP8Type(x, y) == curType)
                A8_cnt++;
            
            // 자기 자신이나 4방향 옆이 현재 색깔이면 이 위치는 폭파될 위치이므로 추가하자.
            if (m_pGameLayer->GetP8Type(x, y) == curType ||
                (x > 0 && !(x-1 == 0 && (y == 0 || y == ROW_COUNT-1)) && m_pGameLayer->GetP8Type(x-1, y) == curType) ||
                (y > 0 && !((x == 0 || x == COLUMN_COUNT-1) && y-1 == 0) && m_pGameLayer->GetP8Type(x, y-1) == curType) ||
                (x < COLUMN_COUNT-1 && !(x+1 == COLUMN_COUNT-1 && (y == 0 || y == ROW_COUNT-1)) && m_pGameLayer->GetP8Type(x+1, y) == curType) ||
                (y < ROW_COUNT-1 && !((x == 0 || x == COLUMN_COUNT-1) && y+1 == ROW_COUNT-1) && m_pGameLayer->GetP8Type(x, y+1) == curType))
            {
                A8_pos.push_back(ccp(x, y));
            }
        }
    }

    for (int x = 0 ; x < COLUMN_COUNT ; x++)
    {
        for (int y = 0 ; y < ROW_COUNT ; y++)
        {
            // 네 모서리에 위치한 존재하지 않는 부분
            if ((x == 0 && y == 0) || (x == 0 && y == ROW_COUNT-1) ||
                (x == COLUMN_COUNT-1 && y == 0) || (x == COLUMN_COUNT-1 && y == ROW_COUNT-1))
                continue;
             /*
            // 자기 자신이나 4방향 옆이 현재 색깔이면 이 위치는 폭파될 위치이므로 추가하자.
            if ((x > 0 && m_pGameLayer->GetP8Type(x-1, y) == m_pGameLayer->GetGlobalType()) ||
                (x < COLUMN_COUNT-1 && m_pGameLayer->GetP8Type(x+1, y) == m_pGameLayer->GetGlobalType()) ||
                (y > 0 && m_pGameLayer->GetP8Type(x, y-1) == m_pGameLayer->GetGlobalType()) ||
                (y < ROW_COUNT-1 && m_pGameLayer->GetP8Type(x, y+1) == m_pGameLayer->GetGlobalType()) ||
                m_pGameLayer->GetP8Type(x, y) == m_pGameLayer->GetGlobalType())
            {
                A8_pos.push_back(ccp(x, y));
            }
            */
            // 음영 바꾸는 action
            if (m_pGameLayer->GetP8Type(x, y) == m_pGameLayer->GetGlobalType())
            {
                /*
                CCFiniteTimeAction* action = CCSequence::create(
                    CCTintTo::create(0.4f, 50, 50, 50),
                    CCCallFunc::create(m_pGameLayer, callfunc_selector(PuzzleSkill::A8Callback)),
                    NULL);
                m_pGameLayer->GetP8(x, y)->runAction(action);
                 */
                m_pGameLayer->GetP8(x, y)->A8Darken();
            }
        }
    }
}
void PuzzleSkill::A8Callback()
{
    // 8번 스킬 실제 실행 (터뜨리기)
    A8_callbackCnt++;
    if (A8_callbackCnt == A8_cnt)
    {
        CCLog("A8 callback executed");
        m_pGameLayer->Bomb(A8_pos);
    }
}
std::vector<CCPoint> PuzzleSkill::A8GetPos()
{
    return A8_pos;
}
void PuzzleSkill::A8Clear()
{
    A8_pos.clear();
}
