#include "PuzzleP8.h"
#include "Puzzle.h"

PuzzleP8* PuzzleP8::CreateP8(CCPoint ap, CCPoint pos, void* parent, int zOrder)
{
    PuzzleP8* puzzleP8 = new PuzzleP8();
  
    puzzleP8->type = rand() % TYPE_COUNT;
    char name[15];
    sprintf(name, "pieces/%d.png", puzzleP8->type);
    
    puzzleP8->sprite = CCSprite::createWithSpriteFrameName(name);
    puzzleP8->sprite->setAnchorPoint(ap);
    puzzleP8->sprite->setPosition(pos);
    
    //puzzleP8->sprite->setOpacity(215);
    
    puzzleP8->parent = parent;
    puzzleP8->zOrder = zOrder;
    
    return puzzleP8;
}

void PuzzleP8::SetType(int type)
{
    this->type = type;
}

int PuzzleP8::GetType()
{
    return type;
}

int PuzzleP8:: GetZOrder()
{
    return zOrder;
}

CCSprite* PuzzleP8::GetPiece()
{
    return sprite;
}

void PuzzleP8::SetPosition(CCPoint pos)
{
    sprite->setPosition(pos);
}

void PuzzleP8::SetPiece(CCSprite* sprite)
{
    this->sprite = sprite;
}

void PuzzleP8::SetZOrder(int zOrder)
{
    this->zOrder = zOrder;
}
