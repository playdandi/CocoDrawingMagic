#ifndef __CocoMagic__T_Effect__
#define __CocoMagic__T_Effect__

#include "Common.h"
#include "T_Puzzle.h"

class T_Effect
{
public:
    void Init(T_Effect* effect, T_Puzzle* layer);
    void InitCheck();
    
    //void PlayEffect(int skillNum, int queue_pos, std::vector<CCPoint> pos);
    void PlayEffectCallback(CCNode* sender, void* data);
    
    void PlayEffect_SkillIcon_Callback(CCNode* sender, void* p);
    void PlayEffect_SkillIcon(int skillNum);
    void PlayEffect_MagicCircle(int skillNum);
    void PlayEffect_MagicCircle_Callback(CCNode* sender, void* pointer);
    void PlayEffect_Default(std::vector<CCPoint> pos);
    void PlayEffect_CycleOnly(int skillNum, std::vector<CCPoint> pos);
    
    /*
    void SetSpirit(int type);
    void SetSpirit_Callback(CCNode* sender, void* pointer);
    CCSprite* GetSpirit(int type);
    void RemoveSpirit(int type);
    void RemoveSpirit_Callback(CCNode* sender, void* pointer);
    void SpiritEffect(int type, int centerX = -1, int centerY = -1);
    */
    
    void PlayEffect_1(std::vector<CCPoint> pos);
    void PlayEffect_9(std::vector<CCPoint> pos, int queue_pos);
    ccBezierConfig GetBezierConfig(T_Effect* pThis, int mul);
    void PlayEffect_17(std::vector<CCPoint> pos, int queue_pos);
    void PlayEffect_17_Callback(CCNode* sender, void* p);
    
    void PlayEffect_0(std::vector<CCPoint> pos);
    void PlayEffect_8(std::vector<CCPoint> pos);
    void PlayEffect_16(std::vector<CCPoint> pos);
    
    void PlayEffect_7(std::vector< std::vector<CCPoint> > pos_d, std::vector<CCPoint> pos, int queue_pos);
    void Effect7_Comet(float f);
    void Effect7_Callback_1(CCNode* sender, void* pointer);
    void Effect7_Callback_2(int idx, void* pointer);
    void Effect7_Callback_3(CCNode* sender, void* pointer);
    void Effect7_Callback_4(cocos2d::CCNode *sender, void *pointer);
    void Effect7_Clear();

    void Effect9Callback(CCNode* sender, void* pointer);
    
    std::vector<CCPoint> GetDoublePos(int idx);
    
    void ShowStarCandy(std::vector<CCPoint> pos);
    void ShowStarCandy_Callback_Done(CCNode* sender, void* pointer);
    
    void RemoveAllObjects();
    
private:
    CCParticleSystemQuad* circle;
    CCParticleSystemQuad* circle_fire;
    CCParticleSystemQuad* circle_water;
    CCParticleSystemQuad* circle_land;
    
    T_Effect* pThis;
    T_Puzzle* gameLayer;
    bool isDone[NUMOFSKILL];
    
    std::vector<CCPoint> skillPos;
    std::vector< std::vector<CCPoint> > skillDoublePos;
    CCPoint deltaPos;
    int callbackCnt;
    int callbackDone;
    int callbackAll;
    int queue_pos_now;
    int skillNum;
    std::vector<int> multi_callbackCnt;
    
    CCLayer* timerLayer;
    
    std::vector<int> cbCnt;
    CCSprite* F8_bg;
    CCParticleSystemQuad* m_F8_fountain;
    std::vector<CCPoint> F8Pos;
    std::vector<CCPoint> F8PosPoint;
    std::vector<int> F8_bomb_cbCnt;
    int F8_finishCnt;
    
    CCSprite* A8_icon;
    
    // 정령
    //CCSprite* spirit_fire;
    //CCSprite* spirit_water;
    //CCSprite* spirit_land;
    
    // 기본 별사탕에 대한 변수
    int starCandyCallbackCnt;
    std::vector<CCPoint> starCandyPos;
    
    int queuePos;
    
    CCSprite* sp_fire;
};

#endif /* defined(__CocoMagic__T_Effect__) */
