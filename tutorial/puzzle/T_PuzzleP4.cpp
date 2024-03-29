#include "T_PuzzleP4.h"
#include "T_Puzzle.h"

T_PuzzleP4* T_PuzzleP4::CreateP4(void* parent, int zOrder, int type, int designatedType)
{
    T_PuzzleP4* puzzleP4 = new T_PuzzleP4();
    
    if (type != -100)
        puzzleP4->type = type;
    else
    {
        if (designatedType != -1) // 게임중 요정에 의해 새로 만들어졌을 떄 (보통 designatedType == CONNECTED)
            puzzleP4->type = designatedType;
        else
            puzzleP4->type = (rand()%100 < 0) ? BLOCKED : CONNECTED; // 연결 diamond 확률은 50%
    }

    puzzleP4->parent = parent;
    puzzleP4->zOrder = zOrder;
    
    return puzzleP4;
}

void T_PuzzleP4::InitChild()
{
    leftup = NULL;
    rightup = NULL;
    leftdown = NULL;
    rightdown = NULL;
}

void T_PuzzleP4::CreateSprites(int x, int y, int lu, int ru, int ld, int rd, CCPoint ap, CCPoint pos, float half_width)
{
    if (type != BLOCKED)
    {
        InitChild();
        char name[25];

        if (lu == rd && (lu >= PIECE_RED && lu <= PIECE_WHITE))
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
            }
        }
        if (ru == ld && (ru >= PIECE_RED && ru <= PIECE_WHITE))
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
            leftup->setScale((half_width+4)/(float)35);
            leftup->setPosition(ccp(pos.x+1, pos.y-1));
        }
        if (rightup != NULL)
        {
            rightup->setAnchorPoint(ccp(0, 0));
            rightup->setScale((half_width+4)/(float)35);
            rightup->setPosition(ccp(pos.x-1, pos.y-1));
        }
        if (leftdown != NULL)
        {
            leftdown->setAnchorPoint(ccp(1, 1));
            leftdown->setScale((half_width+4)/(float)35);
            leftdown->setPosition(ccp(pos.x+1, pos.y+1));
        }
        if (rightdown != NULL)
        {
            rightdown->setAnchorPoint(ccp(0, 1));
            rightdown->setScale((half_width+4)/(float)35);
            rightdown->setPosition(ccp(pos.x-1, pos.y+1));
        }
    }
}

CCSprite* T_PuzzleP4::GetLeftUp()
{
    return leftup;
}
CCSprite* T_PuzzleP4::GetRightUp()
{
    return rightup;
}
CCSprite* T_PuzzleP4::GetLeftDown()
{
    return leftdown;
}
CCSprite* T_PuzzleP4::GetRightDown()
{
    return rightdown;
}

void T_PuzzleP4::SetType(int type)
{
    this->type = type;
}

int T_PuzzleP4::GetType()
{
    return type;
}

int T_PuzzleP4::GetZOrder()
{
    return zOrder;
}

