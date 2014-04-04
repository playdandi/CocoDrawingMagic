#ifndef __magician__PuzzleSkill__
#define __magician__PuzzleSkill__

#include "Common.h"

class Puzzle;

class PuzzleSkill
{
public:
	PuzzleSkill();
	~PuzzleSkill();
    
public:
    void SetGameLayer(Puzzle* layer);
    Puzzle* GetGameLayer();
    
    void Init(std::vector<int> num, std::vector<int> prob, std::vector<int> lv);
    
    void TrySkills(int pieceColor);
    void Try(int skillNum);
    
    bool IsApplied(int skillNum);
    
    void Invoke(int skillNum);
    
    void A1(int num);
    void A2A(int num);
    std::vector<CCPoint> A2AGetPos();
    void A2AClear();
    void F2B(int num);
    void W2B(int num);
    void E2B(int num);
    void F3(int num);
    void W3(int num);
    void E3(int num);
    void F4A();
    void W4A();
    void E4A();
    std::vector<CCPoint> A4AGetPos();
    void A4AClear();
    
    void A4B(int num);
    std::vector<CCPoint> A4BGetPos();
    void A4BClear();
    
    void F5();
    void W5();
    void W5Timer(float f);
    bool W5GetVar();
    void E5();
    
    void A8(int num);
    void A8Callback();
    std::vector<CCPoint> A8GetPos();
    void A8Clear();
    
protected:
    Puzzle* m_pGameLayer;

private:
    bool skillNumber[NUMOFSKILL];
    int skillProb[NUMOFSKILL];
    int skillLevel[NUMOFSKILL];

	// result variables
    bool skillApplied[NUMOFSKILL];
    std::vector<CCPoint> A2A_pos;
    
    int A1_addedScore;
    
    int F2B_addedCandy;
    int W2B_addedScore;
    int E2B_addedCandy;
    
    bool F3_isDoubledMT;
    int W3_addedScore;
    int E3_addedCandy;
    
    std::vector<CCPoint> A4A_pos;
    
    std::vector<CCPoint> A4B_pos;
    
    std::vector<std::vector<int> > F5_pos;
    bool W5_isApplied;
    bool E5_getPotion;
    
    int A8_cnt;
    int A8_callbackCnt;
    std::vector<CCPoint> A8_pos;
    
    //PuzzleSkill* sl;
};

#endif /* defined(__magician__PuzzleSkill__) */
