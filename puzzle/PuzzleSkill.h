#ifndef __magician__PuzzleSkill__
#define __magician__PuzzleSkill__

#include "Common.h"

class Puzzle;
class Effect;

class PuzzleSkill
{
public:
	PuzzleSkill();
	~PuzzleSkill();
    
public:
    void SetGameLayer(Puzzle* layer);
    Puzzle* GetGameLayer();
    Effect* GetEffect();
    
    void Init(std::vector<int> num, std::vector<int> prob, std::vector<int> lv);
    
    int GetSkillAppliedCount(int skillNum);
    
    void TrySkills(int pieceColor, int queue_pos);
    void Try(int skillNum, int queue_pos);
    
    bool IsApplied(int skillNum, int queue_pos);
    
    bool IsSkillNumberExists(int skillNum);
    
    void SetQueuePos(int queue_pos);
    
    void UpdateAppliedSkillCount(int skillNum);
    
    void Invoke(int skillNum, int queue_pos);
    
    void A1(int num, int queue_pos);
    void A2(int num, int queue_pos);
    std::vector<CCPoint> A2GetPos();
    void A2Clear();
    void F3(int num, int queue_pos);
    //void F4(int num);
    void F5(int num);
    void F5_Callback(CCNode* sender, void* data);
    void SpiritTry(int type, int queue_pos);
    bool IsSpiritAlive(int type);
    
    void A6(int num, int queue_pos);
    void F6_Callback(CCNode* sender, void *p);

    void F7(int num, int queue_pos);
    

    void F8(int num, int queue_pos);
    void F8Check(int x, int y, int idx);
    void F8_Comet(float f);
    void F8_Timer();
    void F8_BombCallback(CCNode* sender, void* pointer);
    void F8_Bomb(int queue_pos, std::vector<CCPoint> pos, int idx);
    void F8_FinishCountUp();
    bool F8_IsFinished();
    bool F8_IsActive();
    
    void W3(int num);
    int W3GetScore();
    //void W4(int num);
    //int W4GetCandy();
    void W5(int num);
    void W5_Callback(CCNode* sender, void* data);
    void W7(int num);
    void W7SetVar();
    bool W7GetVar();
    void W7SetTime(int time);
    int W7GetTime();
    void W8(int num, int queue_pos);
    void W8_Timer(float f);
    void W8_AccelTimer(float f);
    bool W8_IsActive();
    void W8_Invoke(std::vector<CCPoint> pos, int queue_pos);
    void W8_Callback(CCNode* sender, void* data);
    void W8_LastChange();
    void W8_BombDone();
    
    void E3(int num);
    void E3_Done();
    //void E4(int num, int queue_pos);
    void E5(int num);
    void E5_Callback(CCNode* sender, void* data);
    void E7(int num);
    void E7_Done();
    void E8(int num, int queue_pos);
    void E8_Timer(float f);
    void E8_Bomb(CCNode* sender, void* data);
    void E8_BombCallback(CCNode* sender, void* data);
    void E8_DecideRestart(int x);
    bool E8_IsFinished();
    void E8_Start();
    bool E8_IsActive();

    std::vector<CCPoint> A8GetPos();
    void A8Clear();
    
    
    void GiveHint();
    
    bool IsRenewNeeded();
    void IsRenewNeeded_Recur(int x, int y, int type, int cnt);
    void RenewPuzzle(int queue_pos);
    void RenewPuzzle_Recur(int x, int y, int type, std::vector<CCPoint>& v);
    void RenewPuzzle_End(void* pointer, int queue_pos);
    
    void M1(int num);
    void M2(int num);
    void M3(int num);
    void M4(int num);
    void M5(int num);
    
    void FT_StartEnd(int queue_pos);
    void FT_Bomb(std::vector<CCPoint> p);
    void FT_Callback(CCNode* sender, void* pointer);
    void FT_CreatePiece(int pos);
    void FT_Create_Recur(int x, int y, int cnt, int minx, int maxx);
    bool IsFTBombing();
    
    void ApplyItemPaint(int x, int y, int dx, int dy, int type, int queue_pos);
    void ApplyItemPaint_Callback(CCNode* sender, void* pointer);
    void ApplyItemStaff(int x, int y, int dx, int dy, int queue_pos);
    std::vector<CCPoint> GetPieces(int x, int y, int dx, int dy);
    
    
    std::vector<CCPoint> GetResult();
    std::vector< std::vector<CCPoint> > GetResultDouble();
    std::vector<CCPoint> GetResultEnd();
    void ResultClear();
    
    void RemoveAllObjects();
    
protected:
    Puzzle* m_pGameLayer;
    
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
    
    int F3_addedScore;
    //bool F4_isDoubledMT;
    
    int spiritShownCnt[3];
    bool isSpiritAlive[3];
    
    int F5_callbackCnt;
    int F6_callbackCnt;
    std::vector<CCPoint> F6_order;
    
    bool F7_check[COLUMN_COUNT][ROW_COUNT];
    int F7_callbackCnt;
    int F7_callbackCntMini;
    
    bool F8_isActive;
    int F8_check[COLUMN_COUNT][ROW_COUNT];
    int F8_bombCallbackCnt[20];
    
    int A8_cnt;
    int A8_callbackCnt;
    int F8_finishCnt;
    std::vector<CCPoint> A8_pos;
    
    ////////////////////////////////////////////////////////////////////////////////////
    
    int W3_addedScore;
    //int W4_addedCandy;
    int W5_callbackCnt;

    int W7_RemainTime;
    bool W7_isTimeSlowed;
    
    bool W8_isActive;
    int W8_remainTime;
    int W8_accelTime;
    int W8_accel;
    int W8_callbackCnt;
    bool W8_isLastChangeWaiting;
    int W8_isChanging;
    bool W8_isLastChange;
    bool W8_scheduleDone;
    bool W8_bombFirst;
    
    ////////////////////////////////////////////////////////////////////////////////////

    int E3_addedCandy;
    //int E4_addedCandy;

    std::vector<CCPoint> E5_pos;
    std::vector<CCPoint> E5_pos_end;
    bool E5_check[COLUMN_COUNT][ROW_COUNT];
    int E5_callbackCnt;
    std::vector<CCPoint> E5_i, E5_j;
    int E5_ij_cnt;
    
    bool E7_getPotion;
    
    int E8_prob[5];
    bool E8_isActive;
    //std::queue<int> E8_lineIdx;
    std::vector<int> E8_lineIdx;
    std::vector<int> E8_lineDepth;
    //int E8_activeCnt;
    int E8_endCnt;
    int E8_cnt;
    //bool E8_check[COLUMN_COUNT];
    int E8_bottomY[COLUMN_COUNT];
    int E8_bombCallbackCnt[COLUMN_COUNT];
    int E8_curY[COLUMN_COUNT];
    //int E8_maxScheduleCnt;
    
    ////////////////////////////////////////////////////////////////////////////////////
    
    int FT_pos;
    int FT_callbackCnt;
    int FT_createCnt;
    bool isFTBombing;
    bool FT_check[COLUMN_COUNT][ROW_COUNT];
    
    bool hintCheck[COLUMN_COUNT][ROW_COUNT];
    
    int renewCheck_maxCnt;
    bool renewCheck[COLUMN_COUNT][ROW_COUNT];
    
    int itemPaint_callbackCnt;
    int itemPaint_type;
    
    std::vector< std::vector<CCPoint> > result_double_pos;
    std::vector<CCPoint> result_pos;
    std::vector<CCPoint> result_pos_end;
    std::vector<CCPoint> result_pos_temp;
};

#endif /* defined(__magician__PuzzleSkill__) */
