#ifndef __magician__Puzzle__
#define __magician__Puzzle__

#include "Common.h"
#include "PuzzleP8.h"
#include "PuzzleP4.h"
#include "PuzzleSkill.h"
#include "Effect.h"
#include "Sound.h"

class Effect;
class PuzzleSkill;
class PuzzleP8Set;
class PuzzleP4Set;

class Puzzle : public CCLayer
{
public:
    static CCScene* scene();
	bool init();
    virtual void onEnter();
    virtual void onExit();
    
    virtual void keyBackClicked();
    
    void InitSprites();
	void InitBoard();
    void InitCoco();
    void CocoAnim(float f);
    void InitFairy();
    void ChangeAnimCoco(float f);
    void ChangeAnimFairy(float f);
    
    void SetScore();
    void UpdateScore(int numOfPiece);
    void SetCombo();
    int GetCombo();
    void UpdateCombo();
    void ComboTimer(float f);
    void SetTimer();
    void UpdateTimer(float f);
    //PuzzleP8* GetBoard(int x, int y);
    CCLayer* GetPieceLayer();
    
    CCPoint BoardStartPosition(CCPoint point);
    CCPoint BoardMovePosition(CCPoint point);
    CCPoint SetCorrectPosition(CCPoint point);
    
    void StartMagicTime(float f);
    
	virtual bool ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent);
	virtual void ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent);
	virtual void ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent);
    
    CCPoint SetPiece8Position(int x, int y);
    CCPoint SetPiece4Position(int x, int y);
    
    void InvokeSkills();
    void Lock(int queue_pos);
    void Bomb(std::vector<CCPoint> bomb_pos);
    void BombCallback(CCNode* sender, void *queue_pos);
    void Falling(int queue_pos);
	void FallingCallback(CCNode* sender, void* queue_pos);
    
    std::vector<CCPoint> GetPiece8xy(bool afterCast);
    int GetGlobalType();
    PuzzleP8* GetP8(int x, int y);
    void SetSkillLock(bool flag);
    bool IsCycle();
    PuzzleSkill* GetSkill();
    
    void tCB(CCNode* sender, void* pos);
    
    
    PuzzleP8Set* GetPuzzleP8Set();
    void SetSpriteP8Null(int x, int y);
    CCSprite* GetSpriteP8(int x, int y);
    
    void PlayEffect(int skillNum);

    
	CREATE_FUNC(Puzzle);
    
    Sound* GetSound();
    
    void EndScene();
    void EndSceneCallback();
    
protected:
    CCSprite* background;
    CCSprite* puzzleFrame;
    
    //CCLayer* pieceLayer;
    
    bool m_bTouchStarted;
    bool m_bIsCycle;
    int m_bLockP8[COLUMN_COUNT][ROW_COUNT];
    int m_bLockP4[COLUMN_COUNT][ROW_COUNT];
    int cur_priority;
    int max_priority;
    
    // queue는 DROP_QUEUE_CNT(5)개.
    int drop_order;
    std::vector< std::vector<CCPoint> > lock8xy;
    std::vector< std::vector<CCPoint> > lock4xy;
    bool m_bSkillLock;
    std::vector< std::vector<CCPoint> > piece8xy;
    std::vector< std::vector<CCPoint> > piece4xy;
    std::vector< std::vector<CCSprite*> > strap;
    int m_iBombCallbackCnt[QUEUE_CNT];
    int m_iBombCallbackCntMax;
    int m_iBombCallbackType[QUEUE_CNT];
    int globalType[QUEUE_CNT];
    
    int touch_cnt;
    
    CCSize m_winSize;
    
	Sound* sound;
    PuzzleSkill* skill;
    
    int m_iState;
    int m_iNextState;
    
    CCSprite* pado;
    int m_pado_callback_cnt;
    int m_pado_total_cnt;
    
    bool isPadoRunning;
    
private:
    int iScore;
    CCLabelTTF* pScoreLabel;
    int iCombo;
    CCLabelTTF* pComboLabel;
    int iComboTimer;
    int iTimer;
    CCLabelTTF* pTimerLabel;
    
    SpriteClass* spriteClass;
    
    CCLayer* fairyLayer;
    CCSprite* fairyShadow;
    CCLayer* cocoLayer;
    int cocoFrameNumber;
    std::vector<CCSprite*> coco_sp;
    CCLayer* puzzleLayer;
    
    PuzzleP8Set* puzzleP8set;
    CCSprite* spriteP8[COLUMN_COUNT][ROW_COUNT];
    PuzzleP4Set* puzzleP4set;
    Effect* effect;

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
    void CreatePiece(int x, int y);
    CCSprite* GetSprite(int x, int y);
    int GetType(int x, int y);
    void AddChild(int x, int y);
    void RemoveChild(int x, int y);
    PuzzleP8* GetObject(int x, int y);
    void MoveObject(int x, int y, int fromX, int fromY);
    void Falling(int x, int y, int targetX, int targetY, int queue_pos);
    void FallingCompleted(CCNode* sender, void* queue_pos);
    
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
    
private:
    PuzzleP4* object[COLUMN_COUNT][ROW_COUNT];
    Puzzle* gameLayer;
    CCLayer* puzzleLayer;
};

#endif
