#ifndef __CocoMagic__Effect__
#define __CocoMagic__Effect__

#include "Common.h"
#include "Puzzle.h"

class Effect
{
public:
    void Init(Effect* effect, Puzzle* layer);
    void InitCheck();
    
    void PlayEffect(int skillNum, std::vector<CCPoint> pos, int queue_pos = -1);
    void PlayEffectCallback(CCNode* sender, void* data);
    
    void PlayEffect_Default(std::vector<CCPoint> pos);
    void PlayEffect_CycleOnly(int skillNum, std::vector<CCPoint> pos);
    void SetSpirit(int type);
    CCParticleSystemQuad* GetSpirit(int type);
    void ReleaseSpirit(int type);
    
    void PlayEffect_0(std::vector<CCPoint> pos);
    void PlayEffect_8(std::vector<CCPoint> pos);
    void PlayEffect_16(std::vector<CCPoint> pos);
    
    void PlayEffect_1(std::vector<CCPoint> pos);
    void PlayEffect_9(std::vector<CCPoint> pos, int queue_pos);
    ccBezierConfig GetBezierConfig(Effect* pThis, int mul);
    void Effect9Callback(CCNode* sender, void* pointer);
    void PlayEffect_17(std::vector<CCPoint> pos);
    
    void PlayEffect_5(std::vector<CCPoint> pos); // 6개이상한번더
    void PlayEffect_13(std::vector<CCPoint> pos); // 6개이상한번더
    void PlayEffect_21(std::vector<CCPoint> pos); // 6개이상한번더
    
    void PlayEffect_7(std::vector<CCPoint> pos); // 8번 스킬
    void Effect7Callback(CCNode* sender, void* pointer);
    
    void PlayEffect_Spirit();
    
private:
    Effect* pThis;
    Puzzle* gameLayer;
    bool isDone[NUMOFSKILL];
    
    std::vector<CCPoint> skillPos;
    CCPoint deltaPos;
    int callbackCnt;
    int queue_pos_now;
    
    CCSprite* dragon;
    
    // 정령
    CCParticleSystemQuad* fire;
    CCParticleSystemQuad* water;
    CCParticleSystemQuad* land;
};

#endif /* defined(__CocoMagic__Effect__) */
