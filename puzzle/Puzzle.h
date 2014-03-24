#ifndef __magician__Puzzle__
#define __magician__Puzzle__

#include "Common.h"
#include "PuzzleP8.h"
#include "PuzzleP4.h"
#include "PuzzleSkill.h"
//#include "Sound.h"

class PuzzleP4;
class PuzzleP8;
class PuzzleSkill;

class Puzzle : public CCLayer
{
public:
	bool init();
    virtual void onEnter();
    virtual void onExit();
    
    virtual void keyBackClicked();
    
    void InitSprites();
	void InitBoard();
    
    void SetScore();
    void UpdateScore(int numOfPiece);
    void SetCombo();
    int GetCombo();
    void UpdateCombo();
    void ComboTimer(float f);
    void SetTimer();
    void UpdateTimer(float f);
    PuzzleP8* GetBoard(int x, int y);
    CCLayer* GetPieceLayer();
    
    CCPoint BoardStartPosition(CCPoint point);
    CCPoint BoardMovePosition(CCPoint point);
    CCPoint SetCorrectPosition(CCPoint point);
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
	void FallingCallback(int queue_pos);
    
    std::vector<CCPoint> GetPiece8xy(bool afterCast);
    int GetGlobalType();
    int GetP8Type(int x, int y);
    PuzzleP8* GetP8(int x, int y);
    void SetSkillLock(bool flag);
    bool IsCycle();
    PuzzleSkill* GetSkill();
    
    void tCB(CCNode* sender, void* pos);

    static CCScene* scene();//std::vector<int> number, std::vector<int> prob, std::vector<int>level);
	CREATE_FUNC(Puzzle);
    
protected:
    CCSprite* background;
    CCSprite* puzzleFrame;
    
    CCLayer* pieceLayer;
    
	PuzzleP8* m_pBoard[COLUMN_COUNT][ROW_COUNT];
    PuzzleP4* m_pBoardSP[COLUMN_COUNT][ROW_COUNT];
    
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
    int m_iBombCallbackCnt[QUEUE_CNT];
    int m_iBombCallbackCntMax;
    int m_iBombCallbackType[QUEUE_CNT];
    int globalType[QUEUE_CNT];
    
    int touch_cnt;
    
    CCSize m_winSize;

    //int m_iBombCallbackCnt;
    int m_iFallingCallbackCnt;
	int m_numOfFallingObjects;
	//Sound* sound;
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
};

#endif
