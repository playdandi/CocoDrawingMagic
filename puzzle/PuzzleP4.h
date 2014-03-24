#ifndef __poops8__PuzzleP4__
#define __poops8__PuzzleP4__

#include "Common.h"

#define BLOCKED 0
#define CONNECTED 1
#define SPECIAL 2

class Puzzle;

class PuzzleP4
{
public:
	PuzzleP4();
	~PuzzleP4();
    
public:
	static PuzzleP4* Create(Puzzle* layer, int x, int y);
	int GetType();
	void SetType(int type);
    int GetTypeSP();
    void SetTypeSP(int type_sp);
    
	void SetGameLayer(Puzzle* layer);
    Puzzle* GetGameLayer();
    
    void InitSprites();
    void CreateSprites(Puzzle* layer, int x, int y);
    
    void SetScales(float scale);
    void SetOpacities(int alpha);
    void SetPositions(CCPoint pos);
    void AddChildren(int zOrder);
    void RemoveChildren();
    
private:
    int m_type; // 0 : blocked, 1 : connected , 2 : special
    //int m_type_sp; // special type -> 0 : 4*4 bomb , ......
    
    //CCSprite* m_basic;
    CCSprite* m_leftup;
    CCSprite* m_rightup;
    CCSprite* m_leftdown;
    CCSprite* m_rightdown;
    CCSprite* m_special;

	Puzzle* m_pGameLayer;
};

#endif
