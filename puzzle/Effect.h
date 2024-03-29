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
    void FadeSpirit(bool fadeIn);
    void SpiritEffect(int type, int centerX = -1, int centerY = -1);
    
    void PlayEffect_0(std::vector<CCPoint> pos);
    void PlayEffect_8(std::vector<CCPoint> pos);
    void PlayEffect_16(std::vector<CCPoint> pos);
    
    void PlayEffect_1(std::vector<CCPoint> pos);
    void PlayEffect_9(std::vector<CCPoint> pos, int queue_pos);
    ccBezierConfig GetBezierConfig(Effect* pThis, int mul);
    void Effect9Callback(CCNode* sender, void* pointer);
    void PlayEffect_17(std::vector<CCPoint> pos, int queue_pos);
    void PlayEffect_17_Callback(CCNode* sender, void* p);
    
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
    
    void PlayEffect_18(CCPoint p); // E3 : 지팡이 레벨에 비례한 추가별사탕
    void Effect18Callback(CCNode* sender, void* pointer);
    void Effect18CallbackNewSprite(CCNode* sender, void* pointer);
    
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
    
    void Effect22Callback(CCNode* sender, void* pointer); // E7 : 끈질긴 생명력
    void PlayEffect_22(CCPoint p);
    
    void PlayEffect_23(int num, std::vector<CCPoint> pos, int queue_pos); // 8번 스킬 (땅)
    void Effect23_Callback(CCNode* sender, void* pointer);
    void Effect23_Bomb(int x);
    void Effect23_Clear();
    void Effect23_Clear_Callback(CCNode* sender, void* pointer);
    
    void PlayEffect_6_Fever(); // F7 : 코코타임
    void PlayEffect_6_Fever_Callback(CCNode *sender, void* pointer);
    
    void PlayEffect_6(int num);
    void PlayEffect_6_Fire(std::vector< std::vector<CCPoint> > pos, int queue_pos, int cnt); // F7 : 불 이펙트
    void Effect6Callback(CCNode* sender, void* pointer);
    
    void PlayEffect_10();
    void PlayEffect_10_Callback(CCNode* sender, void* pointer);
    void PlayEffect_11();
    
    void PlayEffect_14();
    void Effect14Callback(CCNode* sender, void* data);
    
    void PlayEffect_FeverBg();
    void PlayEffect_FeverBg_Off();
    void PlayEffect_FeverCircle(CCPoint p, CCPoint bp, int size);
    void PlayEffect_FeverCircle_Callback(CCNode* sender, void* pointer);
    
    void NewlyMadeConnPiece(int x, int y);
    void FairySkill();
    
    std::vector<CCPoint> GetDoublePos(int idx);
    
    void ShowStarCandy(bool isCycle, std::vector<CCPoint> pos);
    void ShowStarCandyAll(bool isCycle, std::vector<CCPoint> pos);
    void UpdateStarcandy_0(CCNode* sender, void* p);
    void UpdateStarcandy_1(CCNode* sender, void* p);
    void ShowStarCandy_Callback_Done(CCNode* sender, void* pointer);
    
    void ShowItemPaintArrow(int x, int y);
    void ChangeItemPaintArrow(int dx, int dy);
    void RemoveItemPaintArrow();
    
    //void PlayEffect_ItemStaff(int x, int y, int type);
    void PlayEffect_ItemStaff(std::vector<CCPoint> pos, int type);
    
    void BonusBomb(std::vector<CCPoint> pos);
    
    void ShowNewRecordEffect(void* p);
    
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
    int callbackDone;
    int callbackCnt;
    int callbackAll;
    int skillIcon_callbackCnt;
    int queue_pos_now;
    int skillNum;
    int plus_callbackCnt;
    std::vector<int> multi_callbackCnt;
    
    int A2_addedScore;
    
    CCSprite* W7_bg;
    CCSprite* iced_bar;
    CCLayer* timerLayer;
    
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
    
    // 기본 별사탕에 대한 변수
    int starCandyCallbackCnt;
    std::vector<CCPoint> starCandyPos;
    
    int queuePos;
    bool starcandy_0_isCycle;
    bool starcandy_1_isCycle;
    
    CCSprite* sp_fire;
    
    // 피버타임
    CCSprite* fever_black_bg;
    CCParticleSystemQuad* feverBg;
    CCParticleSystemQuad* feverCoco;
    
    std::vector<CCSprite*> itemPaintArrow;
};

#endif /* defined(__CocoMagic__Effect__) */
