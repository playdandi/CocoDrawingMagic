#ifndef __CocoMagic__Fairy__
#define __CocoMagic__Fairy__

#include "Common.h"
#include "cocos2d.h"
#include "cocos-ext.h"

using namespace cocos2d;

class Fairy
{
public:
    static CCLayer* GetFairy(int id);
    
    // 요정없음
    static CCLayer* MakeEmpty();
    
    // 꽃등신
    static CCLayer* MakeFlower();
    static CCLayer* MakeFlowerNew();
    static void Anim_Flower_Hide(CCLayer* gameLayer, SEL_CallFuncND cf, CCPoint pos);
    static void Anim_Flower_Padac(CCLayer* gameLayer, SEL_CallFuncND cf, CCPoint pos);
    static void Anim_Flower_MoveFace(CCLayer* layer);
    static void Anim_Flower_MoveEyebrow(CCLayer* layer);
    
    // 은근해
    static CCLayer* MakeSun();
    static void Anim_Sun_Sigh(CCLayer* layer, CCLayer* gameLayer, SEL_CallFuncND cf);
    static void Anim_Sun_MoveEye(CCLayer* layer);
    static void Anim_Sun_RotateTails(CCLayer* layer);
    
    // 구르미
    static CCLayer* MakeCloud(bool isUI = false);
    static void Anim_Cloud_Curl(CCLayer* gameLayer, SEL_CallFuncND cf, CCPoint pos);
    
    // 고민형
    static CCLayer* MakeBear(bool isUI = false);
    static void Anim_Bear_Sigh(CCLayer* layer, CCLayer* gameLayer); // 고민형 : 한숨쉬기
    static void Anim_Bear_RotateArm(CCLayer* layer, CCLayer* gameLayer); // 고민형 : 팔흔들기
    static void Anim_Bear_Sweat(CCLayer* layer, CCLayer* gameLayer); // 고민형 : 땀흘리기
    
    // 공손달
    static CCLayer* MakeMoon(bool isUI = false);
    static void Anim_Moon_SayHello(CCLayer* layer, CCLayer* gameLayer); // 공손달 : 인사하기
    static void Anim_Moon_Light(CCLayer* layer, CCLayer* gameLayer); // 공손달 : 밝게빛나기
    static void Anim_Moon_FadeInOut(CCLayer* layer, CCLayer* gameLayer); // 공손달 : 사라지기
    static void Anim_Moon_RotateArm(CCLayer* layer, CCLayer* gameLayer); // 공손달 : 팔흔들기
    
    // 점괘구리
    static CCLayer* MakeFrog(bool isUI = false);
    static void Anim_Frog_Sing(CCLayer* layer, CCLayer* gameLayer); // 점괘구리 : 노래부르기
    static void Anim_Frog_Eye(CCLayer* layer, CCLayer* gameLayer); // 점괘구리 : 깜빡깜빡
    static void Anim_Frog_Card(CCLayer* layer, CCLayer* gameLayer); // 점괘구리 : 카드선택
    
    // 또사자
    static CCLayer* MakeLion(bool isUI = false);
    static void Anim_Lion_Shopping(CCLayer* layer, CCLayer* gameLayer); // 또사자 : 쇼핑
    void Lion_Doridori(CCNode* sender, void* data);
    void Lion_MoveHand(CCNode* sender, void* data);
    void Lion_Clear(CCNode* sender, void* data);
    
    // 그래용
    static CCLayer* MakeDragon(bool isUI = false);
    static void Anim_Dragon_Nod(CCLayer* layer, CCLayer* gameLayer); // 그래용 : 끄덕끄덕
    static void Anim_Dragon_MoveTail(CCLayer* layer, CCLayer* gameLayer); // 그래용 : 살랑살랑
    static void Anim_Dragon_RotateArm(CCLayer* layer, CCLayer* gameLayer); // 그래용 : 팔흔들기
    
    // 놀꺼양
    static CCLayer* MakeSheep(bool isUI = false);
    static void Anim_Sheep_MoveFace(CCLayer* layer, CCLayer* gameLayer); // 놀꺼양 : 도리도리
    static void Anim_Sheep_Grass(CCLayer* layer, CCLayer* gameLayer); // 놀꺼양 : 풀버리기
    void Anim_Sheep_Grass_Schedule(float f);  // 놀꺼양 : 풀버리기 -> 풀 모션
    
    // 아기별
    static CCLayer* MakeBabyStar(bool isUI = false);
    static void Anim_BabyStar_SayHello(CCLayer* layer, CCLayer* gameLayer); // 아기별 : 인사하기
    static void Anim_BabyStar_BlinkStar(CCLayer* layer, CCLayer* gameLayer); // 아기별 : 깜빡거리기
    static void Anim_BabyStar_FadeInOut(CCLayer* layer, CCLayer* gameLayer); // 아기별 : 사라지기
    
    void Anim_Callback(CCNode* sender, void *data);
};

#endif /* defined(__CocoMagic__Fairy__) */
