#ifndef PuzzleGame_PuzzleP8
#define PuzzleGame_PuzzleP8

#include "Common.h"

//class Puzzle;
/*
class PuzzleP8 : public CCSprite
{
public:
	PuzzleP8();
	~PuzzleP8();

public:
	static PuzzleP8* Create(Puzzle* layer, int pieceType = -1);

	int GetType();
	void SetType(int type);

	void Falling(int targetX, int targetY, int queue_pos);
	void FallingCompleted(CCNode* sender, void* queue_pos);

    void A8Darken();
    void A8DarkenCompleted(CCNode* sender);
    
	void SetGameLayer(Puzzle* layer);

private:
    int m_type;

	Puzzle* m_pGameLayer;
};
*/

class PuzzleP8
{
public:
    static PuzzleP8* CreateP8(CCPoint ap, CCPoint pos, void* parent, int zOrder);
    void SetType(int type);
    int GetType();
    int GetZOrder();
    CCSprite* GetPiece();
    void SetPosition(CCPoint pos);
    
    void SetPiece(CCSprite* sprite);
    void SetZOrder(int zOrder);

private:
    CCSprite* sprite;
    int type;
    void* parent;
    int zOrder;
};

#endif