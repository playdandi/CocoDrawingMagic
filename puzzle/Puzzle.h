#ifndef __magician__Puzzle__
#define __magician__Puzzle__

#include "Common.h"
#include "PuzzleP8.h"
#include "PuzzleP4.h"
#include "PuzzleSkill.h"
#include "Effect.h"
#include "Sound.h"
#include "../pugixml/pugixml.hpp"

using namespace pugi;

#define SKILL_STOPTIME 0
#define SKILL_BASIC 1
#define SKILL_CYCLE 2
#define SKILL_DOUBLESIX 3
#define SKILL_COCOTIME 4
#define SKILL_FINAL 5
#define SKILL_DONE 6

class Effect;
class PuzzleSkill;
class PuzzleP8Set;
class PuzzleP4Set;

class Puzzle : public CCLayerColor
{
public:
    static CCScene* scene(int addedPotion, int freezeTime, int cocoTime = 1);
	bool init();
    virtual void onEnter();
    virtual void onExit();
    
    virtual void keyBackClicked();
    
    void Notification(CCObject* obj);
    
    void InitSkills();
    void InitInfoBar();
    void InitSprites();
	void InitBoard();
    void InitCoco();
    void CocoAnim(float f);
    void InitFairy();
    void ChangeAnimCoco(float f);
    void ChangeAnimFairy(float f);
    
    void AnimFairy_Callback(CCNode* sender, void* pointer);
    
    void SetScoreAndStarCandy();
    void UpdateScore(int type, int data);
    void UpdateStarCandy(int type, int data);
    void ShowBasicScore(int score, CCPoint pos, int size);
    void ShowSkillScore(int score, float scale, int queue_pos, int etc = -1, int etc2 = -1, int height = 0);
    void ShowSkillScore_Callback(CCNode* sender, void* data);
    //void ShowSkillAddedScore(int score, int queue_pos, int height, int x = -1, int y = -1);
    float GetScoreBasicScale(int size);
    
    int FakeScore(int score);
    int RealScore();
    int FakeStarCandy(int candy);
    int RealStarCandy();
    
    void ShowStarCandy(std::vector<CCPoint> pos);
    //void ShowStarCandy(bool isCycle, std::vector<CCPoint> pos);
    void ShowStarCandy_Callback(CCNode* sender, void* data);
    void SetCombo();
    int GetCombo();
    void UpdateCombo();
    
    void ComboCallback(CCNode* sender, void* p);
    void ComboTimer(float f);
    void SetTimer();
    void UpdateTimer(float f);
    
    void HintTimer(float f);
    void FeverTimer(float f);
    
    bool IsHintShown();
    
    void AddPiecesByFeverTime(std::vector<CCPoint> &p, int queue_pos);
    
    CCLayer* GetPieceLayer();
    
    CCPoint BoardStartPosition(CCPoint point);
    CCPoint BoardMovePosition(CCPoint point);
    CCPoint SetCorrectPosition(CCPoint point);
    
    void StartMagicTime(float f);
    
    void SkillSemaphoreUpdate(int val);
    bool IsSkillSemaphoreFree();
    
    bool IsPaused();
    void PauseGame();
    
    void StopAllActionsAtPieces();
    
	virtual bool ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent);
	virtual void ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent);
	virtual void ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent);
    
    CCPoint SetTouch8Position(int x, int y);
    CCPoint SetTouch4Position(int x, int y);
    CCPoint SetPiece8Position(int x, int y);
    CCPoint SetPiece4Position(int x, int y);
    
    void Cycle_Callback(CCNode* sender, void* p);
    
    void CancelDrawing();
    void InvokeSkills(int queue_pos);
    void Lock(int queue_pos);
    void LockEach(int x, int y);
    void UnLockEach(int x, int y);
    bool IsLocked(int x, int y);
    void Bomb(int queue_pos, std::vector<CCPoint> bomb_pos);//, int F8_idx = -1);
    void BombCallback(CCNode* sender, void *queue_pos);
    
    void RemoveConnectPieces(std::vector<CCPoint> pos);
    void CreateConnectPieces();
    void RemoveConnectPiecesXY(int x, int y);
    //void ReplaceConnectPieces();
    
    
    void FallingProcess();
    void Falling(int queue_pos, int xx = -1);
	void FallingCallback(CCNode* sender, void* queue_pos);
    void FallingQueuePushAndFalling(int queue_pos);
    
    void GoNextState(int queue_pos);
    
    void CheckUselessDiaPieces();
    
    
    void SetPiece8xy(int queue_pos, std::vector<CCPoint> pos);
    std::vector<CCPoint> GetPiece8xy(bool afterCast);
    std::vector<CCPoint> GetPosForFeverTime(bool afterCast);
    
    int GetGlobalType();
    PuzzleP8* GetP8(int x, int y);
    void SetSkillLock(int queue_pos, bool flag);
    bool IsCycle(int queue_pos);
    PuzzleSkill* GetSkill();
    
    CCLayer* GetPuzzleLayer();
    CCLayer* GetCocoLayer();
    CCLayer* GetFairyLayer();
    
    void SetSpiritTouch(bool val);
    
    void UpdatePieceBombCnt(int type, int cnt);
    void MissionComplete();
    
    PuzzleP8Set* GetPuzzleP8Set();
    void SetSpriteP8Null(int x, int y);
    CCSprite* GetSpriteP8(int x, int y);
    PuzzleP4Set* GetPuzzleP4Set();
    
    void SwapSpriteP8(int x1, int y1, int x2, int y2);
    void SetSpriteP8(int x, int y, CCSprite* sp);
    
    bool IsConnected(int x, int y);
    
    void PlayEffect(int skillNum, int queue_pos);

    bool IsValidInSquare(CCPoint center, CCPoint point);
    int GetDist(CCPoint center, CCPoint point);
    
    float GetBoardSize();
    
	CREATE_FUNC(Puzzle);
    
    Sound* GetSound();
    Effect* GetEffect();
    
    void EndScene();
    void EndSceneCallback();

    float GetPieceWidth();
    
    CCSize m_winSize;
    CCSize vs;
    CCPoint vo;
    CCSize tbSize;
    CCSize boardSize;
    CCSize floorSize;
    
    void ReadyAndStart(CCNode* sender, void* pointer);
    //void Ready(float f);
    void Ready_C(CCNode* sender, void* p);
    void ReadyCallback(CCNode* sender, void* pointer);
    
    void UpdateMissionCountBySkill(int skillNum);
    
    void SetRenewFlag(bool flag);
    bool IsRenewing();
    
    void SetGlobalType(int queue_pos, int pieceType);
    
    void StartFeverTime();
    void EndFeverTime();
    bool IsFeverTime();
    int GetFeverRemainTime();
    bool IsRoundInFeverTime(bool afterCast);
    
    void BonusTime(CCNode* sender, void* pointer);
    
    void FairySkillAction();
    
    bool IsItemClear();
    bool IsItemTime();
    bool IsItemPaint();
    bool IsItemStaff();
    void SetItemPossible(bool flag);
    
    int Time100(int denom);
    
    void GameOver_Callback(CCNode* sender, void* pointer);
    void GameEnd(CCNode* sender, void* pointer);
    
    void onHttpRequestCompleted(CCNode *sender, void *data);
    //void onHttpRequestCompletedNoEncrypt(CCNode *sender, void *data);
    //void XmlParseGameStart(xml_document *xmlDoc);
    void XmlParseFriends(xml_document *xmlDoc);
    void XmlParseGameEnd(xml_document *xmlDoc);
    std::string SubstrNickname(std::string nickname);
    //void ParseProfileImage(char* data, int size, int idx);
    
    CCClippingNode* GetBoardClip();
    
    bool IsGameOver();
    
    // 요정능력 관련 변수
    int CONN_PIECE_PROB;        // 연결피스 확률
    int FEVER_TIME_MAX;         // feverTime 시간
    int ADD_STARCANDY_PERCENT;  // 추가별사탕 %
    
    // 한붓그리기 기본 점수
    int dragScore;
    
protected:
    bool isKeybackTouched;
    
    float PIECE8_WIDTH;
    float PIECE8_HEIGHT;
    float PIECE4_WIDTH;
    float PIECE4_HEIGHT;
    float PIECE8_FRAME_WIDTH;
    float PIECE8_FRAME_HEIGHT;
    
    int iPieceBombCnt[5];
    
    int m_bLockP8[COLUMN_COUNT][ROW_COUNT];
    int m_bLockP4[COLUMN_COUNT][ROW_COUNT];
    
    // queue는 DROP_QUEUE_CNT(5)개.
    std::vector< std::vector<CCPoint> > lock8xy;
    std::vector< std::vector<CCPoint> > lock4xy;
    std::vector< std::vector<CCPoint> > piece8xy;
    std::vector< std::vector<CCPoint> > piece4xy;
    std::vector< std::vector<CCSprite*> > strap;
    std::vector<CCPoint> lastPosition;
    int m_iBombCallbackCnt[QUEUE_CNT];
    int m_iBombCallbackCntMax[QUEUE_CNT];
    int m_iBombCallbackType[QUEUE_CNT];
    int globalType[QUEUE_CNT];
    // skill state도 DROP_QUEUE_CNT만큼 있어야 한다.
    int m_iState[QUEUE_CNT];
    int m_iNextState[QUEUE_CNT];
    bool m_bIsCycle[QUEUE_CNT]; // 사이클 발동 여부
    bool m_bSkillLock[QUEUE_CNT]; // skill 발동 여부에 대한 lock
    bool isFeverRound[QUEUE_CNT]; // 이 queue_cnt에 대한 폭발이 feverTime용인가?
    
    int feverBombOrderCnt[QUEUE_CNT];
    int feverBombOrderMax[QUEUE_CNT];
    int feverBombOrder[QUEUE_CNT][COLUMN_COUNT][ROW_COUNT]; // 피버타임 때 폭발하는 순서
    
    std::vector< std::vector<CCPoint> > posForFeverTime;
    
    bool isCancelling;
    
    // falling queue
    bool isFalling;
    std::queue<int> fallingQueue;
    
    // E8 스킬에 대한 falling cnt
    int m_iFallingCallbackCnt_E8[COLUMN_COUNT];
    int m_numOfFallingObjects_E8[COLUMN_COUNT];
    
    int touch_cnt;
    
    int W3_total;
    int W4_total;
    
    // lock, semaphore 관련 변수들
    bool m_bTouchStarted; // touch began에 대한 lock
    int m_iSpiritSP; // 정령 터치에 관한 semaphore
    bool m_bIsSpiritExecuted; // 정령 스킬이 실행될 때 lock
    int m_iSkillSP; // 스킬 lock에 관한 semaphore
    
    // Bomb()에서 연결피스 지울 때 사용되는 변수
    bool P8Bombed[COLUMN_COUNT][ROW_COUNT];
    
    bool isGameStarted;
    bool isGameOver;
    
    int iTouchRound;
    
    //bool isMissionSuccess;
    //int isNewRecord;
    int iMissionCnt;
    bool isMissionDone;
    CCLabelTTF* pMissionLabel;
    CCSprite* pMissionSprite;
    CCSprite* pMissionSpriteDetail;
    CCSprite* pMissionSpriteDetailIcon;
    CCLabelTTF* pMissionSpriteDetailContent;
    
    CCSprite* pClock;
    
    int XMLStatus;
    int profileCnt;

    int curMyRank;
    bool isRankUp;
    
    int readyCnt;
    bool isRenewing;
    
    int feverStartCnt; // 피버타임에 들어가기 위해 필요한 마법발동횟수
    int magicCnt; // 마법발동 횟수 (피버타임 끝나면 0으로 초기화)
    int iFeverTime; // 피버타임 발동중 남은 시간
    int iFeverCombo; // 피버용 콤보 수
    
    int iNumOfFairySkillStart; // 요정 스킬 발동해야 하는 횟수 (falling callback에서 수행하자)
    
    int bonusTimeState; // 보너스타임 순서 상태
    
    bool m_bIsItemPossible;
    bool item_clear;
    bool item_time;
    bool item_paint;
    bool item_staff;
    int totalCnt_paint; // 나와야 할 전체 paint item 수
    int totalCnt_staff; // 나와야 할 전체 staff item 수
    int iNumOfPaintItemRemained; // 지금까지 나타난 paint item 수
    int iNumOfStaffItemRemained; // 지금까지 나타난 staff item 수
    int iNumOfPaintItemNeeded; // paint item 발동 횟수
    int iNumOfStaffItemNeeded; // staff item 발동 횟수
    bool m_bIsItemTouched;
    int item_dx;
    int item_dy;
    
    CCArray* animFairyFrames;
    int fairyTimer; // 요정 액션 타이머
    
    CCSprite* readySprite; // '레디' sprite (pause후 인게임 끝낼 때 필요해서)
    
    int fairyId;
    int fairyLv;
    
    int volumeCnt;
    
    int bonusBombCnt; // 보너스타임 때 터지는 덩어리 개수
    
    int spirit_scid; // 정령스킬 튜토리얼에 쓰이는 정령 스킬 common id
    CCSprite* spirit_sp;
    CCScale9Sprite* balloon;
    CCLabelTTF* ball;
    
private:
    Sound* sound;
    PuzzleSkill* skill;
    Effect* effect;
    
    CCSprite* pBlackOpen;
    CCLabelTTF* readyTimeLabel;
    int iReadyTime;
    
    int iScore;
    CCLayer* pScoreLayer;
    int iStarCandy;
    CCLabelTTF* pStarCandyLabel;
    int iCombo;
    int maxCombo;

    int iComboTimer;
    int iTimer;
    CCLabelTTF* pTimerLabel;
    
    CCLayer* timerLayer;
    CCDrawNode* timerStencil;
    CCClippingNode* timerClip;
    
    CCLayer* gaugeLayer;
    CCDrawNode* gaugeStencil;
    CCClippingNode* gaugeClip;
    
    //int cancelledComboTime;
    bool isHintShown;
    int iHintTime;
    
    SpriteClass* spriteClassInfo;
    SpriteClass* spriteClass;
    
    CCSprite* overBg;
    CCSprite* timelimit;
    
    CCLayer* fairyLayer;
    CCSprite* fairyShadow;
    std::vector<CCSprite*> fairy_sp;
    CCLayer* cocoLayer;
    int cocoFrameNumber;
    std::vector<CCSprite*> coco_sp;
    CCLayer* puzzleLayer;
    
    PuzzleP8Set* puzzleP8set;
    CCSprite* spriteP8[COLUMN_COUNT][ROW_COUNT];
    PuzzleP4Set* puzzleP4set;

    CCSprite* magicTimeBg;
    int m_iStartMagicTimeStatus;
    bool isMagicTime;
    
    CCSprite* fever;
    bool isFeverTime;
    int feverRemainTime;
    int forFeverTimeCnt;
    std::vector<CCSprite*> feverSpr;
    
    int m_iFallingCallbackCnt;
	int m_numOfFallingObjects;
    
    CCSprite* boardSP;
    CCClippingNode* boardClip;
    
    CCSet* runningActions;
};

class PuzzleP8Set
{
public:
    void SetGameLayer(Puzzle* layer);
    void SetPuzzleLayer(CCLayer* layer);
    void CreatePiece(int x, int y, int type = -1);
    CCSprite* GetSprite(int x, int y);
    int GetType(int x, int y);
    void AddChild(int x, int y);
    void RemoveChild(int x, int y);
    PuzzleP8* GetObject(int x, int y);
    void MoveObject(int x, int y, int fromX, int fromY);
    void SwapObject(int x1, int y1, int x2, int y2);
    void Falling(int x, int y, int targetX, int targetY, int queue_pos, int vertical_idx);
    void RemoveAllObjects();
    
private:
    PuzzleP8* object[COLUMN_COUNT][ROW_COUNT];
    Puzzle* gameLayer;
    CCLayer* puzzleLayer;
};

class PuzzleP4Set
{
public:
    void SetGameLayer(Puzzle* layer);
    void SetPuzzleLayer(CCLayer* layer);
    void CreateObject(int x, int y);
    //void CreatePiece(int x, int y, int type = -100, int designatedType = -1);
    bool CreatePiece(int x, int y, int type = -1);
    int GetType(int x, int y);
    void SetType(int x, int y, int type);
    void SetOpacity(int x, int y, int alpha);
    void AddChild(int x, int y);
    void RemoveChild(int x, int y);
    void SetAction(int x, int y, CCActionInterval* action);
    void StopAllActions(int x, int y);
    PuzzleP4* GetObject(int x, int y);
    void RemoveAllObjects();
    bool IsRemoved(int x, int y);
    
private:
    PuzzleP4* object[COLUMN_COUNT][ROW_COUNT];
    Puzzle* gameLayer;
    CCLayer* puzzleLayer;
};

#endif

