#include "PuzzleP8.h"
#include "Puzzle.h"

PuzzleP8::PuzzleP8()
: m_pGameLayer(NULL)
{

}

PuzzleP8::~PuzzleP8()
{

}


PuzzleP8* PuzzleP8::Create(Puzzle* layer, int pieceType)
{
    //PuzzleP8* pPuzzleP8 = new PuzzleP8();
    //pPuzzleP8->SetGameLayer(layer);
    
    int type;
    if (pieceType == -1)
        type = rand()%TYPE_COUNT;
    else
        type = pieceType;
    
    char name[15];
    sprintf(name, "pieces/%d.png", type);
    PuzzleP8* pPuzzleP8 = (PuzzleP8*)CCSprite::createWithSpriteFrameName(name);
    pPuzzleP8->SetGameLayer(layer);
	pPuzzleP8->SetType(type);

	return pPuzzleP8;
}

int PuzzleP8::GetType()
{
	return m_type;
}

void PuzzleP8::SetType(int type)
{
	m_type = type;
}

void PuzzleP8::SetGameLayer(Puzzle* layer)
{
	m_pGameLayer = layer;
}


void PuzzleP8::Falling(int targetX, int targetY, int queue_pos)
{
    CCMoveTo* moveTo = CCMoveTo::create(0.1f, m_pGameLayer->SetPiece8Position(targetX, targetY));
    CCFiniteTimeAction* action = CCSequence::create(moveTo,
            CCCallFuncND::create(this, callfuncND_selector(PuzzleP8::FallingCompleted), (void*)queue_pos),
            NULL);
    this->runAction(action);
}

void PuzzleP8::FallingCompleted(CCNode* sender, void* queue_pos)
{
	m_pGameLayer->FallingCallback((int)queue_pos);
}

void PuzzleP8::A8Darken()
{
    CCFiniteTimeAction* action = CCSequence::create(
                CCTintTo::create(0.4f, 50, 50, 50),
                CCCallFunc::create(this, callfunc_selector(PuzzleP8::A8DarkenCompleted)),
                NULL);
    this->runAction(action);
}

void PuzzleP8::A8DarkenCompleted(CCNode* sender)
{
    //m_pGameLayer->GetSkill()->A8Callback();
}