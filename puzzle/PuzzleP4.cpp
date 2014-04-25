#include "PuzzleP4.h"
#include "Puzzle.h"

PuzzleP4* PuzzleP4::CreateP4(void* parent, int zOrder, int type)
{
    PuzzleP4* puzzleP4 = new PuzzleP4();
    
    if (type != -100)
        puzzleP4->type = type;
    else
        puzzleP4->type = (rand()%100 < 50) ? BLOCKED : CONNECTED; // 연결 diamond 확률은 50%

    puzzleP4->parent = parent;
    puzzleP4->zOrder = zOrder;
    
    return puzzleP4;
}

void PuzzleP4::InitChild()
{
    leftup = NULL;
    rightup = NULL;
    leftdown = NULL;
    rightdown = NULL;
}

void PuzzleP4::CreateSprites(int x, int y, int lu, int ru, int ld, int rd, CCPoint ap, CCPoint pos)
{
    if (type != BLOCKED)
    {
        InitChild();
        //CCLog("type ; %d", type);
        char name[25];

        if (lu == rd)
        {
            sprintf(name, "pieces/%d_link_lu.png", lu);
            leftup = CCSprite::createWithSpriteFrameName(name);
            sprintf(name, "pieces/%d_link_rd.png", rd);
            rightdown = CCSprite::createWithSpriteFrameName(name);
            
            if (ru != ld)
            {
                sprintf(name, "pieces/%d_link_ru.png", lu);
                rightup = CCSprite::createWithSpriteFrameName(name);
                sprintf(name, "pieces/%d_link_ld.png", rd);
                leftdown = CCSprite::createWithSpriteFrameName(name);
                //m_rightup->initWithTexture(pPuzzlePiece4,
                //CCRectMake(lu*PIECE4_WIDTH+3*lu+1, 0, PIECE4_WIDTH, PIECE4_HEIGHT));
                //m_leftdown->initWithTexture(pPuzzlePiece4,
                //CCRectMake(lu*PIECE4_WIDTH+3*lu+1, 0, PIECE4_WIDTH, PIECE4_HEIGHT));
            }
        }
        if (ru == ld)
        {
            sprintf(name, "pieces/%d_link_ru.png", ru);
            rightup = CCSprite::createWithSpriteFrameName(name);
            sprintf(name, "pieces/%d_link_ld.png", ld);
            leftdown = CCSprite::createWithSpriteFrameName(name);
            if (lu != rd)
            {
                sprintf(name, "pieces/%d_link_lu.png", ru);
                leftup = CCSprite::createWithSpriteFrameName(name);
                sprintf(name, "pieces/%d_link_rd.png", ld);
                rightdown = CCSprite::createWithSpriteFrameName(name);
            }
        }
        
        // set anchorpoint and position
        if (leftup != NULL)
        {
            leftup->setAnchorPoint(ccp(1, 0));
            leftup->setPosition(pos);
        }
        if (rightup != NULL)
        {
            rightup->setAnchorPoint(ccp(0, 0));
            //rightup->setRotation(90);
            rightup->setPosition(ccp((int)pos.x-1, (int)pos.y));
            //rightup->setPosition(pos);
        }
        if (leftdown != NULL)
        {
            leftdown->setAnchorPoint(ccp(1, 1));
            //leftdown->setRotation(-90);
            leftdown->setPosition(ccp((int)pos.x, (int)pos.y+2));
            //leftdown->setPosition(pos);
        }
        if (rightdown != NULL)
        {
            rightdown->setAnchorPoint(ccp(0, 1));
            //rightdown->setRotation(180);
            rightdown->setPosition(ccp((int)pos.x-1, (int)pos.y+2));
            //rightdown->setPosition(pos);
        }
    }
}

CCSprite* PuzzleP4::GetLeftUp()
{
    return leftup;
}
CCSprite* PuzzleP4::GetRightUp()
{
    return rightup;
}
CCSprite* PuzzleP4::GetLeftDown()
{
    return leftdown;
}
CCSprite* PuzzleP4::GetRightDown()
{
    return rightdown;
}

void PuzzleP4::SetType(int type)
{
    this->type = type;
}

int PuzzleP4::GetType()
{
    return type;
}

int PuzzleP4::GetZOrder()
{
    return zOrder;
}

