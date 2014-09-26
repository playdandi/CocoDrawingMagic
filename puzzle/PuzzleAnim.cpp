#include "PuzzleAnim.h"

void PuzzleAnim::Init(PuzzleAnim* pAnim, Puzzle* layer)
{
    thisLayer = pAnim;
    puzzleLayer = layer;
    width = puzzleLayer->m_winSize.width;
    timerH = puzzleLayer->tbSize.height;
    boardH = puzzleLayer->boardSize.height;
    voy = puzzleLayer->vo.y;
    mcW = puzzleLayer->magicCircleWidth;
}


// 요정 기본 이미지 레이어 생성
CCLayer* PuzzleAnim::InitFairy(Puzzle* puzzleLayer)
{
    CCLayer* fairyLayer;
    
    switch (myInfo->GetActiveFairyId())
    {
        case 1: // 꽃등신
            Common::AddSpriteFramesWithFile("fairy_flower");
            fairyLayer = Fairy::MakeFlowerNew();
            fairyLayer->setPosition(ccp(width-280, voy+timerH+boardH+60+30));
            break;
        case 2: // 은근해
            fairyLayer = Fairy::MakeSun();
            fairyLayer->setPosition(ccp(width-280+100, voy+timerH+boardH+60+30+135));
            break;
        case 3: // 구르미
            Common::AddSpriteFramesWithFile("fairy_cloud");
            fairyLayer = Fairy::MakeCloud();
            fairyLayer->setPosition(ccp(width-280-50, voy+timerH+boardH+60+30+30));
            break;
        case 4: // 고민형
            Common::AddSpriteFramesWithFile("fairy_bear");
            fairyLayer = Fairy::MakeBear();
            fairyLayer->setScale(0.7f);
            fairyLayer->setPosition(ccp(width/2+290 +55, voy+timerH+boardH+120 +100));
            break;
        case 5: // 공손달
            Common::AddSpriteFramesWithFile("fairy_moon");
            fairyLayer = Fairy::MakeMoon();
            fairyLayer->setScale(0.7f);
            fairyLayer->setPosition(ccp(width/2+290 +60, voy+timerH+boardH+120 +90));
            break;
            // width-280
        case 6: // 점괘구리
            Common::AddSpriteFramesWithFile("fairy_frog");
            fairyLayer = Fairy::MakeFrog();
            fairyLayer->setScale(0.65f);
            //fairyLayer->setPosition(ccp((width+(width/2+mcW/2))/2, voy+timerH+boardH+120 +110));
            fairyLayer->setPosition(ccp(width/2+290 +55, voy+timerH+boardH+120 +130));
            break;
        case 7: // 또사자
            Common::AddSpriteFramesWithFile("fairy_lion");
            fairyLayer = Fairy::MakeLion();
            fairyLayer->setScale(0.7f);
            fairyLayer->setPosition(ccp(width/2+290 +60, voy+timerH+boardH+120 +115));
            break;
        case 8: // 그래용
            Common::AddSpriteFramesWithFile("fairy_dragon");
            fairyLayer = Fairy::MakeDragon();
            fairyLayer->setScale(0.65f);
            fairyLayer->setPosition(ccp(width/2+290 +60, voy+timerH+boardH+120 +145));
            break;
        case 9: // 놀꺼양
            Common::AddSpriteFramesWithFile("fairy_sheep");
            fairyLayer = Fairy::MakeSheep();
            fairyLayer->setScale(0.65f);
            fairyLayer->setPosition(ccp(width/2+290 +55, voy+timerH+boardH+120 +110));
            break;
        case 10: // 아기별
            Common::AddSpriteFramesWithFile("fairy_babystar");
            fairyLayer = Fairy::MakeBabyStar();
            fairyLayer->setScale(0.7f);
            fairyLayer->setPosition(ccp(width/2+290 +65, voy+timerH+boardH+120 +90));
            break;
        default:
            fairyLayer = CCLayer::create();
            break;
    }

    return fairyLayer;
}


// 요정 애니메이션 발동 (puzzle layer에서 timer가 맞을 때 실행 (여기서는 2초마다 1회))
void PuzzleAnim::StartAnim(CCLayer* fairyLayer, int fairyTimer)
{
    int n;
    CCPoint pos;
    
    switch (myInfo->GetActiveFairyId())
    {
        case 1: // 꽃등신
            n = rand() % 4;
            if (n == 0 || n == 1)
            {
                // 기존 요정 잠시 안 보이게 한다.
                fairyLayer->setVisible(false);
                
                // anim action
                CCSize s = fairyLayer->getContentSize();
                float w = s.width * 0.5f;
                float h = s.height * 0.5f;
                pos = ccp(width-280 + w-18, voy+timerH+boardH+60+30 + h-5);
                
                if (n == 0) // 꽃 접었다 펴기
                    Fairy::Anim_Flower_Hide((CCLayer*)puzzleLayer, callfuncND_selector(Puzzle::AnimFairy_Callback), pos);
                else // 초록잎 파닥파닥
                    Fairy::Anim_Flower_Padac((CCLayer*)puzzleLayer, callfuncND_selector(Puzzle::AnimFairy_Callback), pos);
            }
            else if (n == 2) // 얼굴 왔다갔다하기
                Fairy::Anim_Flower_MoveFace(fairyLayer);
            else // 눈썹 씰룩씰룩
                Fairy::Anim_Flower_MoveEyebrow(fairyLayer);
            break;
            
        case 2: // 은근해
            n = rand() % 3;
            if (n == 0) // 한숨
                Fairy::Anim_Sun_Sigh(fairyLayer, (CCLayer*)puzzleLayer, callfuncND_selector(Puzzle::AnimFairy_Callback));
            else if (n == 1) // 눈 움직이기
                Fairy::Anim_Sun_MoveEye(fairyLayer);
            else // 꼬리 회전
                Fairy::Anim_Sun_RotateTails(fairyLayer);
            break;
            
        case 3: // 구르미
            if (fairyTimer % 10000 == 0) // 이 요정은 10초에 한번 액션 발동.
            {
                fairyLayer->setVisible(false);
                pos = ccp(width-280-50, voy+timerH+boardH+60+30+30);
                Fairy::Anim_Cloud_Curl((CCLayer*)puzzleLayer, callfuncND_selector(Puzzle::AnimFairy_Callback), pos);
            }
            break;
            
        case 4: // 고민형
            n = rand() % 3;
            if (n == 0) // 한숨쉬기
                Fairy::Anim_Bear_Sigh(fairyLayer, (CCLayer*)puzzleLayer);
            else if (n == 1) // 팔흔들기
                Fairy::Anim_Bear_RotateArm(fairyLayer, (CCLayer*)puzzleLayer);
            else // 땀흘리기
                Fairy::Anim_Bear_Sweat(fairyLayer, (CCLayer*)puzzleLayer);
            break;
            
        case 5: // 공손달
            n = rand() % 4;
            if (n == 0) // 인사하기
            {
                //pos = ccp(width-280-50, voy+timerH+boardH+60+30+30);
                //Fairy::Anim_Moon_SayHello(fairyLayer, (CCLayer*)puzzleLayer, callfuncND_selector(Puzzle::AnimFairy_Callback), pos);
                Fairy::Anim_Moon_SayHello(fairyLayer, (CCLayer*)puzzleLayer);
            }
            else if (n == 1) // 밝게빛나기
            {
                //pos = ccp(width-280-50 +165, voy+timerH+boardH+60+30+30 +165);
                //Fairy::Anim_Moon_Light((CCLayer*)puzzleLayer, callfuncND_selector(Puzzle::AnimFairy_Callback), pos);
                Fairy::Anim_Moon_Light(fairyLayer, (CCLayer*)puzzleLayer);
            }
            else if (n == 2) // 사라지기
            {
                //Fairy::Anim_Moon_FadeInOut(fairyLayer);
                Fairy::Anim_Moon_FadeInOut(fairyLayer, (CCLayer*)puzzleLayer);
            }
            else // 팔흔들기
            {
                //pos = ccp(width-280-50 + 225, voy+timerH+boardH+60+30+30 +145);
                //Fairy::Anim_Moon_RotateArm(fairyLayer, (CCLayer*)puzzleLayer, callfuncND_selector(Puzzle::AnimFairy_Callback), pos);
                Fairy::Anim_Moon_RotateArm(fairyLayer, (CCLayer*)puzzleLayer);
            }
            break;
            
        case 6: // 점괘구리
            n = rand() % 3;
            if (n == 0) // 노래부르기
                Fairy::Anim_Frog_Sing(fairyLayer, (CCLayer*)puzzleLayer);
            else if (n == 1) // 깜빡깜빡
                Fairy::Anim_Frog_Eye(fairyLayer, (CCLayer*)puzzleLayer);
            else // 카드선택
                Fairy::Anim_Frog_Card(fairyLayer, (CCLayer*)puzzleLayer);
            break;
            
        case 7: // 또사자
            if (fairyTimer == 2000 || fairyTimer == 32000) // 시작하고 2초, 32초 뒤에 실행.
                Fairy::Anim_Lion_Shopping(fairyLayer, (CCLayer*)puzzleLayer);
            break;
            
        case 8: // 그래용
            n = rand() % 3;
            if (n == 0) // 끄덕끄덕
                Fairy::Anim_Dragon_Nod(fairyLayer, (CCLayer*)puzzleLayer);
            else if (n == 1) // 살랑살랑
                Fairy::Anim_Dragon_MoveTail(fairyLayer, (CCLayer*)puzzleLayer);
            else // 팔흔들기
                Fairy::Anim_Dragon_RotateArm(fairyLayer, (CCLayer*)puzzleLayer);
            break;
            
        case 9: // 놀꺼양
            n = rand() % 2;
            if (n == 0) // 도리도리
                Fairy::Anim_Sheep_MoveFace(fairyLayer, (CCLayer*)puzzleLayer);
            else // 풀버리기
                Fairy::Anim_Sheep_Grass(fairyLayer, (CCLayer*)puzzleLayer);
            break;
        
        case 10: // 아기별
            n = rand() % 3;
            if (n == 0) // 인사하기
            {
                //fairyLayer->setVisible(false);
                //pos = ccp(width-280-50 +175, voy+timerH+boardH+60+30+30 +170);
                //Fairy::Anim_BabyStar_SayHello((CCLayer*)puzzleLayer, callfuncND_selector(Puzzle::AnimFairy_Callback), pos);
                Fairy::Anim_BabyStar_SayHello(fairyLayer, (CCLayer*)puzzleLayer);
            }
            else if (n == 1) // 깜빡깜빡
            {
                //pos = ccp(width-280-50 +2, voy+timerH+boardH+60+30+30 -5);
                //Fairy::Anim_BabyStar_BlinkStar((CCLayer*)puzzleLayer, callfuncND_selector(Puzzle::AnimFairy_Callback), pos);
                Fairy::Anim_BabyStar_BlinkStar(fairyLayer, (CCLayer*)puzzleLayer);
            }
            else // 사라지기
                //Fairy::Anim_BabyStar_FadeInOut(fairyLayer);
                Fairy::Anim_BabyStar_FadeInOut(fairyLayer, (CCLayer*)puzzleLayer);
            break;
        default:
            break;
    }
}

