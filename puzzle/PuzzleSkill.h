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
    
    void TrySkills(int pieceColor);
    void Try(int skillNum);
    
    bool IsApplied(int skillNum);
    
    void SetQueuePos(int queue_pos);
    
    void Invoke(int skillNum);
    
    void A1(int num);
    void A2(int num);
    std::vector<CCPoint> A2GetPos();
    void A2Clear();
    void F3(int num);
    void F4(int num);
    void F5(int num);
    void F5_Callback(CCNode* sender, void* data);
    void SpiritTry(int type);
    void A6(int num);

    void F7(int num);
    void F7_Callback(CCNode* sender, void* data);
    void F7Recur(int x, int y, int type, std::vector<CCPoint>& v);
    void A8(int num);
    //void A8Callback();
    std::vector<CCPoint> A8GetPos();
    void A8Clear();
    
    void W3(int num);
    void W4(int num);
    void W5(int num);
    void W5_Callback(CCNode* sender, void* data);
    void W7();
    bool W7GetVar();
    void W7Timer(float f);
    
    void E3(int num);
    void E4(int num);
    void E5(int num);
    void E7();
    
    void M1(int num);
    void M2(int num);
    void M3(int num);
    void M4(int num);
    void M5(int num);
    
    std::vector<CCPoint> GetResult();
    //std::vector<CCPoint> GetResultDouble();
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
    bool skillApplied[NUMOFSKILL];
    
    int A1_addedScore;
    
    std::vector<CCPoint> A2_pos;
    
    int F3_addedScore;
    
    
    //int W2B_addedScore;
    int E2B_addedCandy;
    
    bool F3_isDoubledMT;
    int W3_addedCandy;
    int E3_addedCandy;
    
    int spiritShownCnt[3];
    bool isSpiritAlive[3];
    
    std::vector<CCPoint> A4A_pos;
    std::vector<CCPoint> A4A_pos_end;
    
    std::vector<CCPoint> A4B_pos;
    
    std::vector<CCPoint> F5_pos;
    std::vector<CCPoint> F5_pos_end;
    bool F5_check[COLUMN_COUNT][ROW_COUNT];
    int F5_callbackCnt;
    std::vector<CCPoint> F5_i, F5_j;
    int F5_ij_cnt;
    
    int W5_callbackCnt;
    
    bool F7_check[COLUMN_COUNT][ROW_COUNT];
    int F7_callbackCnt;
    int F7_callbackCntMini;
    
    int A8_cnt;
    int A8_callbackCnt;
    std::vector<CCPoint> A8_pos;
    
    std::vector< std::vector<CCPoint> > result_double_pos;
    std::vector<CCPoint> result_pos;
    std::vector<CCPoint> result_pos_end;
    std::vector<CCPoint> result_pos_temp;
    
    bool isTimeSlowed;
    bool getPotion;
};

#endif /* defined(__magician__PuzzleSkill__) */
