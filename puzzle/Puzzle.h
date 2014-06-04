#ifndef __magician__Puzzle__
#define __magician__Puzzle__

#include "Common.h"
#include "PuzzleP8.h"
#include "PuzzleP4.h"
#include "PuzzleSkill.h"
#include "Effect.h"
#include "Sound.h"

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
    static CCScene* scene(int addedPotion);
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
    
    void SetScoreAndStarCandy();
    void UpdateScore(int type, int data);
    void UpdateStarCandy(int type, int data);
    void ShowStarCandy(std::vector<CCPoint> pos);
    void ShowStarCandy_Callback(CCNode* sender, void* data);
    void SetCombo();
    int GetCombo();
    void UpdateCombo();
    
    void ComboCallback(CCNode* sender, void* p);
    void ComboTimer(float f);
    void SetTimer();
    void UpdateTimer(float f);
    
    CCLayer* GetPieceLayer();
    
    CCPoint BoardStartPosition(CCPoint point);
    CCPoint BoardMovePosition(CCPoint point);
    CCPoint SetCorrectPosition(CCPoint point);
    
    void StartMagicTime(float f);
    
    void SkillSemaphoreUpdate(int val);
    bool IsSkillSemaphoreFree();
    
    bool IsPaused();
    void PauseGame();
    
	virtual bool ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent);
	virtual void ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent);
	virtual void ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent);
    
    CCPoint SetTouch8Position(int x, int y);
    CCPoint SetTouch4Position(int x, int y);
    CCPoint SetPiece8Position(int x, int y);
    CCPoint SetPiece4Position(int x, int y);
    
    void CancelDrawing();
    void InvokeSkills(int queue_pos);
    void Lock(int queue_pos);
    void LockEach(int x, int y);
    void UnLockEach(int x, int y);
    void Bomb(int queue_pos, std::vector<CCPoint> bomb_pos, int F8_idx = -1);
    void BombCallback(CCNode* sender, void *queue_pos);
    
    void FallingProcess();
    void Falling(int queue_pos, int xx = -1);
	void FallingCallback(CCNode* sender, void* queue_pos);
    void FallingQueuePushAndFalling(int queue_pos);
    
    void GoNextState(int queue_pos);
    
    //void WaitOrder(int queue_pos);
    
    std::vector<CCPoint> GetPiece8xy(bool afterCast);
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
    void Ready(float f);
    void Ready_C(CCNode* sender, void* p);
    void ReadyCallback(CCNode* sender, void* pointer);
    
    void UpdateMissionCountBySkill(int skillNum);
    
    void GameOver_Callback(CCNode* sender, void* pointer);
    void GameEnd(CCNode* sender, void* pointer);
    void onHttpRequestCompleted(CCNode *sender, void *data);
    void XmlParseFriends(char* data, int size);
    void ParseProfileImage(char* data, int size, int idx);
    void XmlParseGameEnd(char* data, int size);
    
protected:
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
    int m_iBombCallbackCnt[QUEUE_CNT];
    int m_iBombCallbackCntMax[QUEUE_CNT];
    int m_iBombCallbackType[QUEUE_CNT];
    int globalType[QUEUE_CNT];
    // skill state도 DROP_QUEUE_CNT만큼 있어야 한다.
    int m_iState[QUEUE_CNT];
    int m_iNextState[QUEUE_CNT];
    bool m_bIsCycle[QUEUE_CNT]; // 사이클 발동 여부
    bool m_bSkillLock[QUEUE_CNT]; // skill 발동 여부에 대한 lock
    
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
    
    SpriteClass* spriteClassInfo;
    SpriteClass* spriteClass;
    
    CCSprite* overBg;
    CCSprite* timelimit;
    
    CCLayer* fairyLayer;
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
    std::vector<CCSprite*> feverSpr;
    
    int m_iFallingCallbackCnt;
	int m_numOfFallingObjects;
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
    void CreatePiece(int x, int y, int type = -100);
    int GetType(int x, int y);
    void SetType(int x, int y, int type);
    void SetOpacity(int x, int y, int alpha);
    void AddChild(int x, int y);
    void RemoveChild(int x, int y);
    PuzzleP4* GetObject(int x, int y);
    void RemoveAllObjects();
    
private:
    PuzzleP4* object[COLUMN_COUNT][ROW_COUNT];
    Puzzle* gameLayer;
    CCLayer* puzzleLayer;
};

#endif

