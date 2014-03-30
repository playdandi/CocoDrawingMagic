#ifndef __CocoMagic__Effect__
#define __CocoMagic__Effect__

#include "Common.h"
#include "Puzzle.h"

class Effect
{
public:
    void Init(Puzzle* layer);
    void InitCheck();
    
    void PlayEffect(int skillNum, std::vector<CCPoint> pos);
    void PlayEffectCallback(CCNode* sender, void* data);
    
    void PlayEffect_CycleOnly(int skillNum, std::vector<CCPoint> pos);
    void PlayEffect_0(std::vector<CCPoint> pos);
    void PlayEffect_1(std::vector<CCPoint> pos);
    void PlayEffect_8(std::vector<CCPoint> pos);
    void PlayEffect_9(std::vector<CCPoint> pos);
    void PlayEffect_16(std::vector<CCPoint> pos);
    
    void PlayEffect_5(std::vector<CCPoint> pos); // 6개이상한번더
    void PlayEffect_13(std::vector<CCPoint> pos); // 6개이상한번더
    void PlayEffect_21(std::vector<CCPoint> pos); // 6개이상한번더
    
private:
    Puzzle* gameLayer;
    bool isDone[NUMOFSKILL];
};

#endif /* defined(__CocoMagic__Effect__) */
