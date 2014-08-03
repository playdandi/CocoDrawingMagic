#ifndef __CocoMagic__T_PuzzleSkill__
#define __CocoMagic__T_PuzzleSkill__

#include "Common.h"

class T_Puzzle;
class T_Effect;

class T_PuzzleSkill
{
public:
	T_PuzzleSkill();
	~T_PuzzleSkill();
    
public:
    void SetGameLayer(T_Puzzle* layer);
    T_Puzzle* GetGameLayer();
    T_Effect* GetEffect();
    
    void Init(std::vector<int> num, std::vector<int> prob, std::vector<int> lv);
    
    int GetSkillAppliedCount(int skillNum);
    
    void TrySkills(int pieceColor, int queue_pos);
    void Try(int skillNum, int queue_pos);
    
    SkillBuildUpInfo* GetObj(int num);
    int GetBasicSkillScore(int num);
    
    bool IsApplied(int skillNum, int queue_pos);
    
    bool IsSkillNumberExists(int skillNum);
    
    void SetQueuePos(int queue_pos);
    
    void UpdateAppliedSkillCount(int skillNum);
    
    void Invoke(int skillNum, int queue_pos);
    
    void A1(int num, int queue_pos);
    void A2(int num, int queue_pos);
    bool Check_A2(int num, int queue_pos);
    std::vector<CCPoint> A2GetPos();
    void A2Clear();
    
    void F8(int num, int queue_pos);
    void F8Check(int x, int y, int idx);
    void F8_Comet(float f);
    void F8_Timer();
    void F8_BombCallback(CCNode* sender, void* pointer);
    void F8_Bomb(int queue_pos, std::vector<CCPoint> pos, int idx);
    void F8_Bomb_Real();
    void F8_FinishCountUp();
    bool F8_IsFinished();
    bool F8_IsActive();
    
    std::vector<CCPoint> A8GetPos();
    void A8Clear();
    
    
    std::vector<CCPoint> GetResult();
    std::vector< std::vector<CCPoint> > GetResultDouble();
    std::vector<CCPoint> GetResultEnd();
    void ResultClear();
    
    void RemoveAllObjects();
    
protected:
    T_Puzzle* m_pGameLayer;
    
    int queuePos;
    
private:
    bool skillNumber[NUMOFSKILL];
    int skillProb[NUMOFSKILL];
    int skillLevel[NUMOFSKILL];
    
    int skillAppliedCnt[NUMOFSKILL];
    
	// result variables
    bool skillApplied[QUEUE_CNT][NUMOFSKILL];
    
    ////////////////////////////////////////////////////////////////////////////////////
    
    int A1_addedScore;
    
    std::vector<CCPoint> A2_pos;
    

    bool F8_isActive;
    int F8_check[COLUMN_COUNT][ROW_COUNT];
    int F8_bombCallbackCnt[20];
    
    int A8_cnt;
    int A8_callbackCnt;
    int F8_finishCnt;
    bool F8_isFalling;
    std::vector<CCPoint> A8_pos;
    std::queue<int> F8_bombQueueIdx;
    std::queue< std::vector<CCPoint> > F8_bombQueuePos;

    
    std::vector< std::vector<CCPoint> > result_double_pos;
    std::vector<CCPoint> result_pos;
    std::vector<CCPoint> result_pos_end;
    std::vector<CCPoint> result_pos_temp;
};


#endif /* defined(__CocoMagic__T_PuzzleSkill__) */
