#ifndef __CocoMagic__T_Puzzle__
#define __CocoMagic__T_Puzzle__

#include "Common.h"
#include "T_PuzzleP8.h"
#include "T_PuzzleP4.h"
#include "T_PuzzleSkill.h"
#include "T_Effect.h"
#include "Sound.h"

#define SKILL_STOPTIME 0
#define SKILL_BASIC 1
#define SKILL_CYCLE 2
#define SKILL_DOUBLESIX 3
#define SKILL_COCOTIME 4
#define SKILL_FINAL 5
#define SKILL_DONE 6

class T_Effect;
class T_PuzzleSkill;
class T_PuzzleP8Set;
class T_PuzzleP4Set;

class T_Puzzle : public CCLayerColor
{
public:
    static CCScene* scene();
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

    void InitTutorial();
    
    void ReverseColor();
    
    void SetScoreAndStarCandy();
    void UpdateScore(int type, int data);
    void UpdateStarCandy(int type, int data);
    void ShowStarCandy(std::vector<CCPoint> pos);
    void ShowStarCandy_Callback(CCNode* sender, void* data);

    void SetTimer();
    
    CCLayer* GetPieceLayer();
    
    CCPoint BoardStartPosition(CCPoint point);
    CCPoint BoardMovePosition(CCPoint point);
    CCPoint SetCorrectPosition(CCPoint point);
    
    void SkillSemaphoreUpdate(int val);
    bool IsSkillSemaphoreFree();
    
    bool IsPaused();
    void PauseGame();
    void SkipGame();
    
    void StopAllActionsAtPieces();
    
    
    void TutorialNextState();
    void TutorialStart(CCNode* sender, void* pointer);
    void TutorialEnd();
    
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
    void Bomb(int queue_pos, std::vector<CCPoint> bomb_pos);
    void BombCallback(CCNode* sender, void *queue_pos);
    
    void FallingProcess();
    void Falling(int queue_pos);
	void FallingCallback(CCNode* sender, void* queue_pos);
    void FallingQueuePushAndFalling(int queue_pos);
    
    void GoNextState(int queue_pos);
    
    void CheckUselessDiaPieces();
    
    
    void SetPiece8xy(int queue_pos, std::vector<CCPoint> pos);
    std::vector<CCPoint> GetPiece8xy(bool afterCast);
    
    int GetGlobalType();
    T_PuzzleP8* GetP8(int x, int y);
    void SetSkillLock(int queue_pos, bool flag);
    bool IsCycle(int queue_pos);
    T_PuzzleSkill* GetSkill();
    
    CCLayer* GetPuzzleLayer();
    CCLayer* GetCocoLayer();
    CCLayer* GetFairyLayer();
    
    //void SetSpiritTouch(bool val);
    
    T_PuzzleP8Set* GetPuzzleP8Set();
    void SetSpriteP8Null(int x, int y);
    CCSprite* GetSpriteP8(int x, int y);
    T_PuzzleP4Set* GetPuzzleP4Set();
    
    void SwapSpriteP8(int x1, int y1, int x2, int y2);
    void SetSpriteP8(int x, int y, CCSprite* sp);
    
    bool IsConnected(int x, int y);
    
    //void PlayEffect(int skillNum, int queue_pos);
    
    bool IsValidInSquare(CCPoint center, CCPoint point);
    int GetDist(CCPoint center, CCPoint point);
    
    float GetBoardSize();
    
	CREATE_FUNC(T_Puzzle);
    
    Sound* GetSound();
    T_Effect* GetEffect();
    
    void EndScene();
    void EndSceneCallback();
    
    float GetPieceWidth();
    
    CCSize m_winSize;
    CCSize vs;
    CCPoint vo;
    CCSize tbSize;
    CCSize boardSize;
    CCSize floorSize;
    
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
    int m_iBombCallbackCnt[QUEUE_CNT];
    int m_iBombCallbackCntMax[QUEUE_CNT];
    int m_iBombCallbackType[QUEUE_CNT];
    int globalType[QUEUE_CNT];
    // skill state도 DROP_QUEUE_CNT만큼 있어야 한다.
    int m_iState[QUEUE_CNT];
    int m_iNextState[QUEUE_CNT];
    bool m_bIsCycle[QUEUE_CNT]; // 사이클 발동 여부
    bool m_bSkillLock[QUEUE_CNT]; // skill 발동 여부에 대한 lock
    
    bool isInGamePause;
    
    bool isCancelling;
    
    bool isFalling;
    std::queue<int> fallingQueue;

    int touch_cnt;
    
    // lock, semaphore 관련 변수들
    bool m_bTouchStarted; // touch began에 대한 lock
    //int m_iSpiritSP; // 정령 터치에 관한 semaphore
    //bool m_bIsSpiritExecuted; // 정령 스킬이 실행될 때 lock
    int m_iSkillSP; // 스킬 lock에 관한 semaphore
    
    // Bomb()에서 연결피스 지울 때 사용되는 변수
    bool P8Bombed[COLUMN_COUNT][ROW_COUNT];
    
    int iTouchRound;
    
    CCSprite* pClock;
    
    int ttrState;
    
private:
    Sound* sound;
    T_PuzzleSkill* skill;
    T_Effect* effect;
    
    CCSprite* pBlackOpen;
    CCSprite* pBlackClose;
    
    int iScore;
    CCLayer* pScoreLayer;
    int iStarCandy;
    CCLabelTTF* pStarCandyLabel;
    
    int iTimer;
    CCLayer* timerLayer;
    CCDrawNode* timerStencil;
    CCClippingNode* timerClip;
    
    CCLayer* gaugeLayer;
    CCDrawNode* gaugeStencil;
    CCClippingNode* gaugeClip;
    
    SpriteClass* spriteClassInfo;
    SpriteClass* spriteClass;
    
    CCLayer* fairyLayer;
    std::vector<CCSprite*> fairy_sp;
    CCLayer* cocoLayer;
    int cocoFrameNumber;
    std::vector<CCSprite*> coco_sp;
    CCLayer* puzzleLayer;
    
    T_PuzzleP8Set* puzzleP8set;
    CCSprite* spriteP8[COLUMN_COUNT][ROW_COUNT];
    T_PuzzleP4Set* puzzleP4set;

    int m_iFallingCallbackCnt;
	int m_numOfFallingObjects;
    
    // 튜토리얼 변수들
    CCLabelTTF* ttrMsg;
    CCScale9Sprite* ttrBg1;
    CCScale9Sprite* ttrBg2;
    //CCSprite* ttrBg;
    //CCSprite* ttrArrow;
    CCSprite* ttrSkip;
    CCSprite* ttrFinger;
    
    bool isSkipPossible;
};

class T_PuzzleP8Set
{
public:
    void SetGameLayer(T_Puzzle* layer);
    void SetPuzzleLayer(CCLayer* layer);
    void CreatePiece(int x, int y, int type = -1);
    CCSprite* GetSprite(int x, int y);
    int GetType(int x, int y);
    void AddChild(int x, int y);
    void RemoveChild(int x, int y);
    T_PuzzleP8* GetObject(int x, int y);
    void MoveObject(int x, int y, int fromX, int fromY);
    void SwapObject(int x1, int y1, int x2, int y2);
    void Falling(int x, int y, int targetX, int targetY, int queue_pos);
    void RemoveAllObjects();
    
private:
    T_PuzzleP8* object[COLUMN_COUNT][ROW_COUNT];
    T_Puzzle* gameLayer;
    CCLayer* puzzleLayer;
};

class T_PuzzleP4Set
{
public:
    void SetGameLayer(T_Puzzle* layer);
    void SetPuzzleLayer(CCLayer* layer);
    void CreatePiece(int x, int y, int type = -100, int designatedType = -1);
    int GetType(int x, int y);
    void SetType(int x, int y, int type);
    void SetOpacity(int x, int y, int alpha);
    void AddChild(int x, int y);
    void RemoveChild(int x, int y);
    void SetAction(int x, int y, CCActionInterval* action);
    void StopAllActions(int x, int y);
    T_PuzzleP4* GetObject(int x, int y);
    void RemoveAllObjects();
    
private:
    T_PuzzleP4* object[COLUMN_COUNT][ROW_COUNT];
    T_Puzzle* gameLayer;
    CCLayer* puzzleLayer;
};


#endif /* defined(__CocoMagic__T_Puzzle__) */
