#ifndef __CocoMagic__Effect__
#define __CocoMagic__Effect__

#include "Common.h"
#include "Puzzle.h"

class Effect
{
public:
    void Init(Effect* effect, Puzzle* layer);
    void InitCheck();
    
    void PlayEffect(int skillNum, int queue_pos, std::vector<CCPoint> pos);
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
    
    void PlayEffect_2(std::vector<CCPoint> pos, int queue_pos); // F3 : 10개이상 제거 시 추가점수
    void Effect2Callback(CCNode* sender, void* pointer);
    void Effect2CallbackNewSprite(CCNode* sender, void* pointer);
    void PlayEffect_19(std::vector<CCPoint> pos, int queue_pos); // E4 : 10개이상 제거 시 추가별사탕
    void Effect19Callback(CCNode* sender, void* pointer);
    void Effect19CallbackNewSprite(CCNode* sender, void* pointer);
    
    void PlayEffect_7_15_23(int num, std::vector<CCPoint> pos, int queue_pos); // 8번 스킬
    void Effect7_15_23_Callback(CCNode* sender, void* pointer);
    void Effect7_15_23_Icon_Callback(CCNode* sender, void* pointer);
    
    void PlayEffect_6(int num); // F7 : 코코 변신 + 코코 주위 링
    void PlayEffect_6_Fire(std::vector< std::vector<CCPoint> > pos, int queue_pos, int cnt); // F7 : 불 이펙트
    void Effect6Callback(CCNode* sender, void* pointer);
    
    void PlayEffect_10();
    void PlayEffect_11();
    
    void PlayEffect_14();
    void Effect14Callback(CCNode* sender, void* data);
    
    void PlayEffect_Spirit();
    
    void ShowStarCandy(std::vector<CCPoint> pos);
    void ShowStarCandy_Callback(CCNode* sender, void* data);
    void ShowStarCandy_Callback_Done(CCNode* sender, void* pointer);
    
private:
    Effect* pThis;
    Puzzle* gameLayer;
    bool isDone[NUMOFSKILL];
    
    std::vector<CCPoint> skillPos;
    CCPoint deltaPos;
    int callbackCnt;
    int callbackAll;
    int queue_pos_now;
    int skillNum;
    
    CCSprite* W7_bg;
    CCSprite* iced_bar;
    CCLayer* timerLayer;
    CCDrawNode* stencil;
    CCClippingNode* clip;
    
    CCSprite* A8_icon;
    
    // 정령
    CCParticleSystemQuad* fire;
    CCParticleSystemQuad* water;
    CCParticleSystemQuad* land;
    
    // 기본 별사탕에 대한 변수
    int starCandyCallbackCnt;
    std::vector<CCPoint> starCandyPos;
    
    int queuePos;
};

#endif /* defined(__CocoMagic__Effect__) */
