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
    
    void PlayEffect_SkillIcon_Callback(CCNode* sender, void* p);
    void PlayEffect_SkillIcon(int skillNum);
    void PlayEffect_MagicCircle(int skillNum);
    void PlayEffect_MagicCircle_Callback(CCNode* sender, void* pointer);
    void PlayEffect_Default(std::vector<CCPoint> pos);
    void PlayEffect_CycleOnly(int skillNum, std::vector<CCPoint> pos);
    
    void SetSpirit(int type);
    void SetSpirit_Callback(CCNode* sender, void* pointer);
    CCSprite* GetSpirit(int type);
    void RemoveSpirit(int type);
    void RemoveSpirit_Callback(CCNode* sender, void* pointer);
    //CCParticleSystemQuad* GetSpirit(int type);
    //void ReleaseSpirit(int type);
    
    void PlayEffect_0(std::vector<CCPoint> pos);
    void PlayEffect_8(std::vector<CCPoint> pos);
    void PlayEffect_16(std::vector<CCPoint> pos);
    
    void PlayEffect_1(std::vector<CCPoint> pos);
    void PlayEffect_9(std::vector<CCPoint> pos, int queue_pos);
    ccBezierConfig GetBezierConfig(Effect* pThis, int mul);
    void Effect9Callback(CCNode* sender, void* pointer);
    void PlayEffect_17(std::vector<CCPoint> pos);
    
    //void PlayEffect_5(std::vector<CCPoint> pos); // 6개이상한번더
    void PlayEffect_5(int x, int y);
    void PlayEffect_5_Callback(CCNode* sender, void* pointer);
    void PlayEffect_13(std::vector<CCPoint> pos); // 6개이상한번더
    void PlayEffect_21(std::vector<CCPoint> pos); // 6개이상한번더
    
    void PlayEffect_2(std::vector<CCPoint> pos, int queue_pos); // F3 : 10개이상 제거 시 추가점수
    void Effect2Callback(CCNode* sender, void* pointer);
    void Effect2CallbackNewSprite(CCNode* sender, void* pointer);
    void PlayEffect_19(std::vector<CCPoint> pos, int queue_pos); // E4 : 10개이상 제거 시 추가별사탕
    void Effect19Callback(CCNode* sender, void* pointer);
    void Effect19CallbackNewSprite(CCNode* sender, void* pointer);
    
    void PlayEffect_7(std::vector< std::vector<CCPoint> > pos_d, std::vector<CCPoint> pos, int queue_pos);
    void Effect7_Comet(float f);
    void Effect7_Callback_1(CCNode* sender, void* pointer);
    void Effect7_Callback_2(int idx, void* pointer);
    void Effect7_Callback_3(CCNode* sender, void* pointer);
    void Effect7_Callback_4(cocos2d::CCNode *sender, void *pointer);
    void Effect7_Clear();
    
    void PlayEffect_15(int num, std::vector<CCPoint> pos, int queue_pos); // 8번 스킬 (여신)
    void Effect15_Callback(CCNode* sender, void* pointer);
    void Effect15_Bomb(std::vector<CCPoint> pos, void* pointer);
    void Effect15_Last(std::vector<CCPoint> pos, void* pointer);
    void AddOrbMaxParticle(int v);
    void Effect15_Clear();
    
    void PlayEffect_23(int num, std::vector<CCPoint> pos, int queue_pos); // 8번 스킬 (땅)
    void Effect23_Callback(CCNode* sender, void* pointer);
    void Effect23_Bomb(int x);
    void Effect23_Clear();
    void Effect23_Clear_Callback(CCNode* sender, void* pointer);
    
    void PlayEffect_6(int num); // F7 : 코코 변신 + 코코 주위 링
    void PlayEffect_6_Fire(std::vector< std::vector<CCPoint> > pos, int queue_pos, int cnt); // F7 : 불 이펙트
    void Effect6Callback(CCNode* sender, void* pointer);
    
    void PlayEffect_10();
    void PlayEffect_11();
    
    void PlayEffect_14();
    void Effect14Callback(CCNode* sender, void* data);
    
    //void PlayEffect_Spirit();
    
    std::vector<CCPoint> GetDoublePos(int idx);
    
    void ShowStarCandy(std::vector<CCPoint> pos);
    //void ShowStarCandy_Callback(CCNode* sender, void* data);
    void ShowStarCandy_Callback_Done(CCNode* sender, void* pointer);
    
    void RemoveAllObjects();
    
private:
    CCParticleSystemQuad* circle;
    CCParticleSystemQuad* circle_fire;
    CCParticleSystemQuad* circle_water;
    CCParticleSystemQuad* circle_land;
    
    Effect* pThis;
    Puzzle* gameLayer;
    bool isDone[NUMOFSKILL];
    
    std::vector<CCPoint> skillPos;
    std::vector< std::vector<CCPoint> > skillDoublePos;
    CCPoint deltaPos;
    int callbackCnt;
    int callbackAll;
    int queue_pos_now;
    int skillNum;
    std::vector<int> multi_callbackCnt;
    
    CCSprite* W7_bg;
    CCSprite* iced_bar;
    CCLayer* timerLayer;
    //CCDrawNode* stencil;
    //CCClippingNode* clip;
    
    std::vector<int> cbCnt;
    CCSprite* F8_bg;
    CCParticleSystemQuad* m_F8_fountain;
    std::vector<CCPoint> F8Pos;
    std::vector<CCPoint> F8PosPoint;
    std::vector<int> F8_bomb_cbCnt;
    int F8_finishCnt;
    
    CCSprite* A8_icon;
    
    CCPoint orb_pos;
    CCParticleSystemQuad* m_orb;
    CCParticleSystemQuad* m_W8_bg;
    
    // 정령
    CCSprite* spirit_fire;
    CCSprite* spirit_water;
    CCSprite* spirit_land;
    /*
    CCParticleSystemQuad* fire;
    CCParticleSystemQuad* water;
    CCParticleSystemQuad* land;
    */
    
    // 기본 별사탕에 대한 변수
    int starCandyCallbackCnt;
    std::vector<CCPoint> starCandyPos;
    
    int queuePos;
    
    CCSprite* sp_fire;
};

#endif /* defined(__CocoMagic__Effect__) */
