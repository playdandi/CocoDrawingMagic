#include "Fairy.h"

CCLayer* Fairy::MakeFlowerNew()
{
    CCLayer* layer = CCLayer::create();
    
    CCSprite* leaf = CCSprite::createWithSpriteFrameName("flower_leaf.png");
    leaf->setAnchorPoint(ccp(0, 0));
    leaf->setPosition(ccp(0, 0));
    layer->addChild(leaf, 99);
    
    float p = leaf->getContentSize().width / 2 - 7;
    
    CCLayer* face = CCLayer::create();
    face->setAnchorPoint(ccp(0, 0));
    face->setPosition(ccp(p, 0));
    face->setTag(0);
    layer->addChild(face, 100);
    
    CCSprite* f = CCSprite::createWithSpriteFrameName("flower_face.png");
    f->setAnchorPoint(ccp(0.5, 0));
    f->setPosition(ccp(0, 30));
    face->addChild(f, 101);
    CCSprite* eyebrow = CCSprite::createWithSpriteFrameName("flower_eyebrow.png");
    eyebrow->setPosition(ccp(0, 190));
    eyebrow->setTag(1);
    face->addChild(eyebrow, 102);
    
    layer->setContentSize(CCSize(f->getContentSize().width, f->getContentSize().height+30));
    
    return layer;
}

void Fairy::Anim_Flower_Hide(CCLayer* gameLayer, SEL_CallFuncND cf, CCPoint pos)
{
    CCArray* animFrames = CCArray::createWithCapacity(9);
    
    char name[20];
    int c = -1;
    for (int j = 0 ; j < 9 ; j++)
    {
        if (j >= 5)
        {
            c += 2;
            sprintf(name, "flower_hide_%d.png", j-c);
        }
        else
            sprintf(name, "flower_hide_%d.png", j+1);
        CCSpriteFrame* frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(name);
        animFrames->addObject(frame);
    }

    CCSprite* first = CCSprite::createWithSpriteFrameName("flower_hide_1.png");
    first->setAnchorPoint(ccp(0.50, 0.50));
    first->setPosition(pos);
    gameLayer->addChild(first, 105);
    
    CCAnimation* animation = CCAnimation::createWithSpriteFrames(animFrames, 0.07f);
    CCAnimate* animate = CCAnimate::create(animation);
    CCFiniteTimeAction* action = CCSequence::create(animate, CCCallFuncND::create(gameLayer, cf, NULL), NULL);
    first->runAction(action);

    animFrames->removeAllObjects();
}

void Fairy::Anim_Flower_MoveFace(CCLayer* layer)
{
    CCActionInterval* action = CCSequence::create( CCRotateBy::create(0.15f, 15), CCRotateBy::create(0.3f, -30), CCRotateBy::create(0.15f, 15), NULL);
    layer->getChildByTag(0)->runAction(action);
}

void Fairy::Anim_Flower_MoveEyebrow(CCLayer* layer)
{
    CCActionInterval* action = CCSequence::create( CCMoveBy::create(0.1f, ccp(0, 10)), CCMoveBy::create(0.1f, ccp(0, -10)), CCMoveBy::create(0.1f, ccp(0, 10)), CCMoveBy::create(0.1f, ccp(0, -10)), NULL);
    layer->getChildByTag(0)->getChildByTag(1)->runAction(action);
}

void Fairy::Anim_Flower_Padac(CCLayer* gameLayer, SEL_CallFuncND cf, CCPoint pos)
{
    CCArray* animFrames = CCArray::createWithCapacity(9);
    
    char name[20];
    for (int j = 0 ; j < 5 ; j++)
    {
        if (j % 2 == 0)
            sprintf(name, "flower_hide_1.png");
        else
            sprintf(name, "flower_padac.png");
        CCSpriteFrame* frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(name);
        animFrames->addObject(frame);
    }
    
    CCSprite* first = CCSprite::createWithSpriteFrameName("flower_hide_1.png");
    first->setAnchorPoint(ccp(0.50, 0.50));
    first->setPosition(pos);
    gameLayer->addChild(first, 105);
    
    CCAnimation* animation = CCAnimation::createWithSpriteFrames(animFrames, 0.1f);
    CCAnimate* animate = CCAnimate::create(animation);
    CCFiniteTimeAction* action = CCSequence::create(animate, CCCallFuncND::create(gameLayer, cf, NULL), NULL);
    first->runAction(action);
    
    animFrames->removeAllObjects();
}

CCLayer* Fairy::MakeFlower()
{
    CCLayer* fairyLayer = CCLayer::create();
    
    CCSprite* face = CCSprite::createWithSpriteFrameName("flower/face.png");
    face->setAnchorPoint(ccp(0.5, 0.5));
    face->setPosition(ccp(0, 0));
    fairyLayer->addChild(face, 50);
    
    CCSprite* mouth = CCSprite::createWithSpriteFrameName("flower/mouth.png");
    mouth->setAnchorPoint(ccp(0.5, 0.5));
    mouth->setPosition(ccp(0, -28));
    fairyLayer->addChild(mouth, 50);
    
    CCSprite* eyebg1 = CCSprite::createWithSpriteFrameName("flower/eye_bg.png");
    eyebg1->setAnchorPoint(ccp(0.5, 0.5));
    eyebg1->setPosition(ccp(-32, 15));
    fairyLayer->addChild(eyebg1, 50);
    CCSprite* eyebg2 = CCSprite::createWithSpriteFrameName("flower/eye_bg.png");
    eyebg2->setAnchorPoint(ccp(0.5, 0.5));
    eyebg2->setPosition(ccp(32, 15));
    fairyLayer->addChild(eyebg2, 50);
    
    CCSprite* eye1 = CCSprite::createWithSpriteFrameName("flower/eye.png");
    eye1->setAnchorPoint(ccp(0.5, 0.5));
    eye1->setPosition(ccp(-32, 20));
    fairyLayer->addChild(eye1, 50);
    CCSprite* eye2 = CCSprite::createWithSpriteFrameName("flower/eye.png");
    eye2->setAnchorPoint(ccp(0.5, 0.5));
    eye2->setPosition(ccp(32, 20));
    fairyLayer->addChild(eye2, 50);
    
    CCSprite* eyebrow1 = CCSprite::createWithSpriteFrameName("flower/eyebrow.png");
    eyebrow1->setAnchorPoint(ccp(0.5, 0.5));
    eyebrow1->setPosition(ccp(-32, 40));
    fairyLayer->addChild(eyebrow1, 50);
    CCSprite* eyebrow2 = CCSprite::createWithSpriteFrameName("flower/eyebrow.png");
    eyebrow2->setAnchorPoint(ccp(0.5, 0.5));
    eyebrow2->setPosition(ccp(32, 40));
    fairyLayer->addChild(eyebrow2, 50);
    
    CCSprite* stem = CCSprite::createWithSpriteFrameName("flower/stem_1.png");
    stem->setAnchorPoint(ccp(0.5, 0.5));
    stem->setPosition(ccp(15, -140));
    fairyLayer->addChild(stem, 5);
    
    
    
    CCSprite* leaf1_1 = CCSprite::createWithSpriteFrameName("flower/leaf1.png");
    CCSprite* leaf2_1 = CCSprite::createWithSpriteFrameName("flower/leaf2.png");
    CCSprite* leaf3_1 = CCSprite::createWithSpriteFrameName("flower/leaf3.png");
    CCSprite* leaf4_1 = CCSprite::createWithSpriteFrameName("flower/leaf4.png");
    CCSprite* leaf5_1 = CCSprite::createWithSpriteFrameName("flower/leaf5.png");
    
    leaf1_1->setPosition(ccp(-60, 65));
    fairyLayer->addChild(leaf1_1, 30);
    
    leaf2_1->setPosition(ccp(-80, -37));
    fairyLayer->addChild(leaf2_1, 30);
    
    leaf3_1->setPosition(ccp(5, -88));
    fairyLayer->addChild(leaf3_1, 30);
    
    leaf4_1->setPosition(ccp(90, -25));
    fairyLayer->addChild(leaf4_1, 30);
    
    leaf5_1->setPosition(ccp(54, 75));
    fairyLayer->addChild(leaf5_1, 30);
    
    CCSprite* gleaf1 = CCSprite::createWithSpriteFrameName("flower/leaf_down_1.png");
    CCSprite* gleaf2 = CCSprite::createWithSpriteFrameName("flower/leaf_down_1.png");
    gleaf1->setAnchorPoint(ccp(0, 0));
    gleaf2->setFlipX(true);
    gleaf2->setAnchorPoint(ccp(1, 0));
    gleaf1->setPosition(ccp(8, -190));
    gleaf2->setPosition(ccp(15, -190));
    fairyLayer->addChild(gleaf1, 25);
    fairyLayer->addChild(gleaf2, 25);
    
    // action
    //CCFiniteTimeAction* action = CCSequence::create(CCMoveBy::create(0.5f, ccp(0, -5)), CCMoveBy::create(0.5f, ccp(0, +5)), NULL);
    //CCAction* rep = CCRepeatForever::create((CCActionInterval*)action);
    //fairyLayer->runAction(rep);
    
    return fairyLayer;
}


CCLayer* Fairy::MakeSun()
{
    CCLayer* layer = CCLayer::create();
    
    CCSprite* face = CCSprite::createWithSpriteFrameName("sun/sun_face.png");
    face->setAnchorPoint(ccp(0.5, 0.5));
    face->setPosition(ccp(0, 0));
    layer->addChild(face, 50);
    CCSprite* mouth = CCSprite::createWithSpriteFrameName("sun/sun_mouth.png");
    mouth->setAnchorPoint(ccp(0.5, 0.5));
    mouth->setPosition(ccp(0, -30));
    mouth->setTag(0); // tag
    layer->addChild(mouth, 100);
    
    CCLayer* eyeLayer = CCLayer::create();
    CCSprite* eye1 = CCSprite::createWithSpriteFrameName("sun/sun_eye.png");
    eye1->setAnchorPoint(ccp(0.5, 0.5));
    eye1->setPosition(ccp(0, 0));
    eyeLayer->addChild(eye1, 50);
    CCSprite* eye2 = CCSprite::createWithSpriteFrameName("sun/sun_eye.png");
    eye2->setAnchorPoint(ccp(0.5, 0.5));
    eye2->setPosition(ccp(70, 0));
    eyeLayer->addChild(eye2, 50);
    eyeLayer->setPosition(ccp(-50, 0));
    eyeLayer->setTag(1); // tag
    layer->addChild(eyeLayer, 50);
    
    CCLayer* tails = CCLayer::create(); // 태양 주위 8개에 대한 layer
    tails->setAnchorPoint(ccp(0, 0));
    tails->setPosition(ccp(0, 0));
    tails->setTag(2); // tag
    layer->addChild(tails, 50);
    
    float r = 110.0;
    float x, y;
    for (int i = 0 ; i < 8 ; i++)
    {
        CCSprite* temp = CCSprite::createWithSpriteFrameName("sun/sun_head.png");
        temp->setAnchorPoint(ccp(0.5, 0.5));
        if (i == 0)      { x = 0; y = r; }
        else if (i == 1) { x = r/sqrt(2); y = r/sqrt(2); }
        else if (i == 2) { x = r; y = 0; }
        else if (i == 3) { x = r/sqrt(2); y = -r/sqrt(2); }
        else if (i == 4) { x = 0; y = -r; }
        else if (i == 5) { x = -r/sqrt(2); y = -r/sqrt(2); }
        else if (i == 6) { x = -r; y = 0; }
        else if (i == 7) { x = -r/sqrt(2); y = r/sqrt(2); }
        temp->setPosition(ccp(x, y));
        temp->setRotation(45*i);
        tails->addChild(temp, 50);
    }
    
    return layer;
}

void Fairy::Anim_Sun_Sigh(CCLayer* layer, CCLayer* gameLayer, SEL_CallFuncND cf)
{
    CCSprite* sigh = CCSprite::createWithSpriteFrameName("sun/sun_sigh.png");
    sigh->setPosition(ccp(30, 0));
    sigh->setOpacity(0);
    layer->getChildByTag(0)->addChild(sigh, 3005);
    
    CCActionInterval* action = CCSequence::create( CCSpawn::create(CCFadeIn::create(0.4f), CCMoveBy::create(0.4f, ccp(20, -20)), NULL), CCSpawn::create(CCFadeOut::create(0.4f), CCMoveBy::create(0.4f, ccp(20, -20)), NULL), CCCallFuncND::create(gameLayer, cf, NULL), NULL);
    sigh->runAction(action);
}

void Fairy::Anim_Sun_MoveEye(CCLayer* layer)
{
    CCActionInterval* action = CCSequence::create( CCMoveBy::create(0.4f, ccp(40, 0)), CCMoveBy::create(0.4f, ccp(-40, 0)), NULL);
    layer->getChildByTag(1)->runAction(action);
}

void Fairy::Anim_Sun_RotateTails(CCLayer* layer)
{
    CCActionInterval* action = CCSequence::create( CCRotateBy::create(0.2f, 90), CCRotateBy::create(0.2f, 90), CCRotateBy::create(0.2f, 90), CCRotateBy::create(0.2f, 90), NULL);
    layer->getChildByTag(2)->runAction(action);
}

CCLayer* Fairy::MakeCloud(bool isUI)
{
    CCLayer* layer = CCLayer::create();
    
    CCSprite* cloud;
    if (isUI)
    {
        cloud = CCSprite::createWithSpriteFrameName("cloud/cloud.png");
        cloud->setAnchorPoint(ccp(0.5, 0.5));
        cloud->setPosition(ccp(0, 0));
        cloud->setScale(0.9f);
        layer->addChild(cloud, 99);
    }
    else // in game
    {
        cloud = CCSprite::createWithSpriteFrameName("cloud_1.png");
        cloud->setAnchorPoint(ccp(0, 0));
        cloud->setPosition(ccp(0, 0));
        cloud->setScale(0.9f);
        layer->addChild(cloud, 99);
        
        CCSize c = cloud->getContentSize();
        CCSprite* face = CCSprite::createWithSpriteFrameName("cloud_face.png");
        face->setAnchorPoint(ccp(0.5, 0.5));
        face->setPosition(ccp(c.width/2-20, 100));
        face->setScale(0.9f);
        layer->addChild(face, 100);
    }
    
    layer->setContentSize(cloud->getContentSize());
    
    return layer;
}

void Fairy::Anim_Cloud_Curl(CCLayer* gameLayer, SEL_CallFuncND cf, CCPoint pos)
{
    CCArray* animFrames = CCArray::createWithCapacity(8);
    
    char name[20];
    for (int i = 0 ; i < 2 ; i++)
    {
        for (int j = 0 ; j < 4 ; j++)
        {
            sprintf(name, "cloud_%d.png", j+1);
            CCSpriteFrame* frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(name);
            animFrames->addObject(frame);
        }
    }
    
    CCSprite* first = CCSprite::createWithSpriteFrameName("cloud_1.png");
    first->setAnchorPoint(ccp(0, 0));
    first->setScale(0.9f);
    first->setPosition(pos);
    gameLayer->addChild(first, 105);
    
    CCAnimation* animation = CCAnimation::createWithSpriteFrames(animFrames, 0.07f);
    CCAnimate* animate = CCAnimate::create(animation);
    CCFiniteTimeAction* action = CCSequence::create(animate, CCCallFuncND::create(gameLayer, cf, NULL), NULL);
    first->runAction(action);
    
    animFrames->removeAllObjects();
}


CCLayer* Fairy::MakeEmpty()
{
    CCLayer* fairyLayer = CCLayer::create();
    
    CCSprite* sp = CCSprite::createWithSpriteFrameName("icon/icon_fairy_empty.png");
    sp->setAnchorPoint(ccp(0.5, 0.5));
    sp->setPosition(ccp(0, 0));
    fairyLayer->addChild(sp, 50);
    
    return fairyLayer;
}

CCLayer* Fairy::GetFairy(int id)
{
    CCLayer* picture = CCLayer::create();
    switch (id)
    {
        case 1: picture = MakeFlower(); picture->setScale(1.0f); break;
        case 2: picture = MakeSun(); picture->setScale(1.0f); break;
        case 3: picture = MakeCloud(true); picture->setScale(0.8f); break;
        default: picture = MakeEmpty(); break;
    }
    return picture;
}

