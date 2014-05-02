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
    
    void TrySkills(int pieceColor, int queue_pos);
    void Try(int skillNum, int queue_pos);
    
    bool IsApplied(int skillNum, int queue_pos);
    
    void SetQueuePos(int queue_pos);
    
    void Invoke(int skillNum, int queue_pos);
    
    void A1(int num, int queue_pos);
    void A2(int num, int queue_pos);
    std::vector<CCPoint> A2GetPos();
    void A2Clear();
    void F3(int num, int queue_pos);
    void F4(int num);
    void F5(int num);
    void F5_Callback(CCNode* sender, void* data);
    void SpiritTry(int type, int queue_pos);
    bool IsSpiritAlive(int type);
    void A6(int num, int queue_pos);

    void F7(int num, int queue_pos);
    void F7_Continue(void* pointer, int queue_pos);
    void F7Recur(int x, int y, int type, std::vector<CCPoint>& v);

    void F8(int num, int queue_pos);
    void F8Check(int x, int y);
    void A8(int num, int queue_pos);
    std::vector<CCPoint> A8GetPos();
    void A8Clear();
    
    void W3(int num);
    int W3GetScore();
    void W4(int num);
    int W4GetCandy();
    void W5(int num);
    void W5_Callback(CCNode* sender, void* data);
    void W7(int num);
    void W7SetVar();
    bool W7GetVar();
    void W7SetTime(int time);
    int W7GetTime();
    
    void E3(int num);
    void E4(int num, int queue_pos);
    void E5(int num);
    void E7();
    
    void M1(int num);
    void M2(int num);
    void M3(int num);
    void M4(int num);
    void M5(int num);
    
    std::vector<CCPoint> GetResult();
    std::vector< std::vector<CCPoint> > GetResultDouble();
    std::vector<CCPoint> GetResultEnd();
    void ResultClear();
    
protected:
    Puzzle* m_pGameLayer;
    
    int queuePos;

private:
    bool skillNumber[NUMOFSKILL];
    int skillProb[NUMOFSKILL];
    int skillLevel[NUMOFSKILL];

	// result variables
    bool skillApplied[QUEUE_CNT][NUMOFSKILL];
    
    int A1_addedScore;
    
    std::vector<CCPoint> A2_pos;
    
    int F3_addedScore;
    bool F4_isDoubledMT;
    
    int spiritShownCnt[3];
    bool isSpiritAlive[3];
    
    std::vector<CCPoint> F5_pos;
    std::vector<CCPoint> F5_pos_end;
    bool F5_check[COLUMN_COUNT][ROW_COUNT];
    int F5_callbackCnt;
    std::vector<CCPoint> F5_i, F5_j;
    int F5_ij_cnt;
    
    bool F7_check[COLUMN_COUNT][ROW_COUNT];
    int F7_callbackCnt;
    int F7_callbackCntMini;
    
    int F8_check[COLUMN_COUNT][ROW_COUNT];
    
    int A8_cnt;
    int A8_callbackCnt;
    std::vector<CCPoint> A8_pos;
    
    //////////////////////////////////////////
    
    int E2B_addedCandy;
    
    int W3_addedScore;
    int W4_addedCandy;
    int W5_callbackCnt;

    int W7_RemainTime;
    bool W7_isTimeSlowed;
    
    //////////////////////////////////////////

    int E4_addedCandy;

    
   // std::vector<CCPoint> A4A_pos;
   // std::vector<CCPoint> A4A_pos_end;
    
   // std::vector<CCPoint> A4B_pos;
    
    std::vector< std::vector<CCPoint> > result_double_pos;
    std::vector<CCPoint> result_pos;
    std::vector<CCPoint> result_pos_end;
    std::vector<CCPoint> result_pos_temp;
    
    bool getPotion;
};

#endif /* defined(__magician__PuzzleSkill__) */
