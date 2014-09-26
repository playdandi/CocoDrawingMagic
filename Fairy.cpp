#include "Fairy.h"

CCLayer* Fairy::GetFairy(int id)
{
    CCLayer* picture = CCLayer::create();
    switch (id)
    {
        case 1: picture = MakeFlower(); picture->setScale(1.0f); break;
        case 2: picture = MakeSun(); picture->setScale(1.0f); break;
        case 3: picture = MakeCloud(true); picture->setScale(0.8f); break;
        case 4: picture = MakeBear(true); picture->setScale(1.0f); break;
        case 5: picture = MakeMoon(true); picture->setScale(1.0f); break;
        case 6: picture = MakeFrog(true); picture->setScale(1.0f); break;
        case 7: picture = MakeLion(true); picture->setScale(1.0f); break;
        case 8: picture = MakeDragon(true); picture->setScale(1.0f); break;
        case 9: picture = MakeSheep(true); picture->setScale(1.0f); break;
        case 10: picture = MakeBabyStar(true); picture->setScale(1.0f); break;
        default: picture = MakeEmpty(); break;
    }
    return picture;
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


////////////////////////////////////////////////////////////////////////////////////////////////
// 꽃등신 (id = 1)
////////////////////////////////////////////////////////////////////////////////////////////////

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


////////////////////////////////////////////////////////////////////////////////////////////////
// 은근해 (id = 2)
////////////////////////////////////////////////////////////////////////////////////////////////

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


////////////////////////////////////////////////////////////////////////////////////////////////
// 구르미 (id = 3)
////////////////////////////////////////////////////////////////////////////////////////////////

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


////////////////////////////////////////////////////////////////////////////////////////////////
// 고민형 (id = 4)
////////////////////////////////////////////////////////////////////////////////////////////////
CCLayer* Fairy::MakeBear(bool isUI)
{
    CCLayer* layer = CCLayer::create();
    
    CCSprite* bear;
    if (isUI)
    {
        bear = CCSprite::createWithSpriteFrameName("bear/bear.png");
        bear->setPosition(ccp(0, 0));
        layer->addChild(bear, 99);
        layer->setContentSize(bear->getContentSize());
    }
    else
    {
        layer->setContentSize(CCSize(0, 0));
        
        CCSprite* body = CCSprite::createWithSpriteFrameName("bear_body.png");
        body->setPosition(ccp(0, 0));
        body->setTag(0);
        layer->addChild(body, 99);
        
        CCSprite* face = CCSprite::createWithSpriteFrameName("bear_face_1.png");
        face->setPosition(ccp(-20, 35));
        face->setTag(1);
        layer->addChild(face, 100);
        
        CCSprite* left = CCSprite::createWithSpriteFrameName("bear_lefthand.png");
        left->setAnchorPoint(ccp(0.5, 1));
        left->setPosition(ccp(-15, -70));
        left->setTag(2);
        layer->addChild(left, 100);
        
        CCSprite* right = CCSprite::createWithSpriteFrameName("bear_righthand.png");
        right->setAnchorPoint(ccp(0.5, 1));
        right->setPosition(ccp(40, -70));
        right->setTag(3);
        layer->addChild(right, 100);
    }
    
    return layer;
}
void Fairy::Anim_Bear_Sigh(CCLayer* layer, CCLayer* gameLayer) // 고민형 : 한숨쉬기
{
    layer->getChildByTag(1)->setVisible(false);
    
    CCSprite* newface = CCSprite::createWithSpriteFrameName("bear_face_3.png");
    newface->setPosition(ccp(-20, 35)); // 원본 표정이랑 똑같은 위치
    newface->setTag(1);
    layer->addChild(newface, 105);
    CCActionInterval* action = CCSequence::create(CCDelayTime::create(1.0f), CCCallFuncND::create(gameLayer, callfuncND_selector(Fairy::Anim_Callback), (void*)layer), NULL);
    newface->runAction(action);
    
    CCSprite* sigh = CCSprite::createWithSpriteFrameName("bear_sigh.png");
    sigh->setPosition(ccp(-55, -25));
    sigh->setOpacity(0);
    sigh->setTag(1);
    layer->addChild(sigh, 105);
    CCActionInterval* action2 = CCSequence::create( CCSpawn::create(CCFadeIn::create(0.5f), CCMoveBy::create(0.5f, ccp(-10, -10)), NULL), CCSpawn::create(CCFadeOut::create(0.5f), CCMoveBy::create(0.5f, ccp(-10, -10)), NULL), CCCallFuncND::create(gameLayer, callfuncND_selector(Fairy::Anim_Callback), (void*)layer), NULL);
    sigh->runAction(action2);
}
void Fairy::Anim_Bear_RotateArm(CCLayer* layer, CCLayer* gameLayer) // 고민형 : 팔흔들기
{
    layer->getChildByTag(3)->setVisible(false);
    
    CCSprite* right = CCSprite::createWithSpriteFrameName("bear_righthand_up.png");
    right->setAnchorPoint(ccp(0, 0));
    right->setPosition(ccp(45, -95));
    right->setTag(3); // tag = 3
    layer->addChild(right, 110);
    
    CCActionInterval* action = CCSequence::create(CCRotateBy::create(0.3f, 30), CCRotateBy::create(0.3f, -30), CCRotateBy::create(0.3f, 30), CCRotateBy::create(0.3f, -30), CCCallFuncND::create(gameLayer, callfuncND_selector(Fairy::Anim_Callback), (void*)layer), NULL);
    right->runAction(action);
}
void Fairy::Anim_Bear_Sweat(CCLayer* layer, CCLayer* gameLayer) // 고민형 : 땀흘리기
{
    layer->getChildByTag(1)->setVisible(false);
    
    CCSprite* newface = CCSprite::createWithSpriteFrameName("bear_face_2.png");
    newface->setPosition(ccp(-20, 35));
    newface->setTag(1); // tag = 1
    layer->addChild(newface, 105);
    CCActionInterval* action = CCSequence::create(CCDelayTime::create(1.0f), CCCallFuncND::create(gameLayer, callfuncND_selector(Fairy::Anim_Callback), (void*)layer), NULL);
    newface->runAction(action);
    
    CCSprite* sweat = CCSprite::createWithSpriteFrameName("bear_sweat.png");
    sweat->setPosition(ccp(80, 90));
    sweat->setTag(1); // tag = 1
    layer->addChild(sweat, 105);
    CCActionInterval* action2 = CCSequence::create(CCSpawn::create(CCMoveBy::create(1.0f, ccp(20, -40)), CCFadeOut::create(1.0f), NULL), CCCallFuncND::create(gameLayer, callfuncND_selector(Fairy::Anim_Callback), (void*)layer), NULL);
    sweat->runAction(action2);
}


////////////////////////////////////////////////////////////////////////////////////////////////
// 공손달 (id = 5)
////////////////////////////////////////////////////////////////////////////////////////////////
CCLayer* Fairy::MakeMoon(bool isUI)
{
    CCLayer* layer = CCLayer::create();
    
    CCSprite* moon;
    if (isUI)
    {
        moon = CCSprite::createWithSpriteFrameName("moon/moon.png");
        moon->setPosition(ccp(0, 0));
        layer->addChild(moon, 99);
        layer->setContentSize(moon->getContentSize());
    }
    else
    {
        layer->setContentSize(CCSize(0, 0));
        
        moon = CCSprite::createWithSpriteFrameName("moon_body.png");
        moon->setPosition(ccp(0, 0));
        moon->setTag(0);
        layer->addChild(moon, 99);
        
        CCSprite* face = CCSprite::createWithSpriteFrameName("moon_face_1.png");
        face->setPosition(ccp(-30, 0));
        face->setTag(1);
        layer->addChild(face, 100);
        
        CCSprite* left = CCSprite::createWithSpriteFrameName("moon_lefthand.png");
        left->setAnchorPoint(ccp(1, 0));
        left->setPosition(ccp(0, -110));
        left->setTag(2);
        layer->addChild(left, 100);
        
        CCSprite* right = CCSprite::createWithSpriteFrameName("moon_righthand.png");
        right->setAnchorPoint(ccp(0, 0));
        right->setPosition(ccp(-7, -110));
        right->setTag(3);
        layer->addChild(right, 100);
    }
    
    return layer;
}
void Fairy::Anim_Moon_SayHello(CCLayer* layer, CCLayer* gameLayer) // 공손달 : 인사하기
{
    layer->getChildByTag(1)->setVisible(false);
    
    CCSprite* newface = CCSprite::createWithSpriteFrameName("moon_face_2.png");
    newface->setPosition(ccp(-30, 0));
    newface->setTag(1); // tag = 1
    layer->addChild(newface, 100);
    
    CCActionInterval* action = CCSequence::create(CCMoveBy::create(0.5f, ccp(0, -20)), CCMoveBy::create(0.5f, ccp(0, 20)), CCCallFuncND::create(gameLayer, callfuncND_selector(Fairy::Anim_Callback), (void*)layer), NULL);
    newface->runAction(action);
    
    CCActionInterval* action2 = CCSequence::create(CCMoveBy::create(0.5f, ccp(0, -15)), CCMoveBy::create(0.5f, ccp(0, 15)), NULL);
    layer->getChildByTag(2)->runAction(action2);
    layer->getChildByTag(3)->runAction((CCActionInterval*)action2->copy());
}
void Fairy::Anim_Moon_Light(CCLayer* layer, CCLayer* gameLayer) // 공손달 : 밝게빛나기
{
    CCSprite* effect = CCSprite::createWithSpriteFrameName("moon_effect.png");
    effect->setPosition(ccp(0, 0));
    effect->setScale(1.0f);
    effect->setTag(-1); // no tag
    layer->addChild(effect, 105);
    CCActionInterval* action = CCSequence::create(CCDelayTime::create(1.25f), CCFadeOut::create(0.01f), CCCallFuncND::create(gameLayer, callfuncND_selector(Fairy::Anim_Callback), (void*)layer), NULL);
    effect->runAction(action);
    
    CCSprite* effect2 = CCSprite::createWithSpriteFrameName("moon_effect.png");
    effect2->setPosition(ccp(0, 0));
    effect2->setScale(1.1f);
    effect2->setTag(-1); // no tag
    effect2->setOpacity(0);
    layer->addChild(effect2, 105);
    CCActionInterval* action2 = CCSequence::create(CCDelayTime::create(0.25f), CCFadeIn::create(0.01f), CCDelayTime::create(0.75f), CCFadeOut::create(0.01f), CCCallFuncND::create(gameLayer, callfuncND_selector(Fairy::Anim_Callback), (void*)layer), NULL);
    effect2->runAction(action2);
    
    CCSprite* effect3 = CCSprite::createWithSpriteFrameName("moon_effect.png");
    effect3->setPosition(ccp(0, 0));
    effect3->setScale(1.2f);
    effect3->setTag(-1); // no tag
    effect3->setOpacity(0);
    layer->addChild(effect3, 105);
    CCActionInterval* action3 = CCSequence::create(CCDelayTime::create(0.5f), CCFadeIn::create(0.01f), CCDelayTime::create(0.25f), CCFadeOut::create(0.01f), CCCallFuncND::create(gameLayer, callfuncND_selector(Fairy::Anim_Callback), (void*)layer), NULL);
    effect3->runAction(action3);
}
void Fairy::Anim_Moon_FadeInOut(CCLayer* layer, CCLayer* gameLayer) // 공손달 : 사라지기
{
    CCActionInterval* action = CCSequence::create(CCFadeOut::create(0.6f), CCFadeIn::create(0.2f), NULL);

    int count = layer->getChildrenCount();
    for (int i = 0 ; i < count ; i++) // body(0), face(1), lefthand(2), righthand(3)
        layer->getChildByTag(i)->runAction((CCActionInterval*)action->copy());
}
void Fairy::Anim_Moon_RotateArm(CCLayer* layer, CCLayer* gameLayer) // 공손달 : 팔흔들기
{
    layer->getChildByTag(3)->setVisible(false);
    
    CCSprite* right = CCSprite::createWithSpriteFrameName("moon_righthand_up.png");
    right->setAnchorPoint(ccp(0, 0));
    right->setPosition(ccp(80, -10));
    right->setTag(3); // tag = 3
    layer->addChild(right, 110);
    
    CCActionInterval* action = CCSequence::create(CCRotateBy::create(0.3f, 30), CCRotateBy::create(0.3f, -30), CCRotateBy::create(0.3f, 30), CCRotateBy::create(0.3f, -30), CCCallFuncND::create(gameLayer, callfuncND_selector(Fairy::Anim_Callback), (void*)layer), NULL);
    right->runAction(action);
}


////////////////////////////////////////////////////////////////////////////////////////////////
// 점괘구리 (id = 6)
////////////////////////////////////////////////////////////////////////////////////////////////
CCLayer* Fairy::MakeFrog(bool isUI)
{
    CCLayer* layer = CCLayer::create();
    
    CCSprite* frog;
    if (isUI)
    {
        frog = CCSprite::createWithSpriteFrameName("frog/frog.png");
        frog->setPosition(ccp(0, 0));
        layer->addChild(frog, 99);
        layer->setContentSize(frog->getContentSize());
    }
    else
    {
        layer->setContentSize(CCSize(0, 0));
        
        frog = CCSprite::createWithSpriteFrameName("frog_head.png");
        frog->setPosition(ccp(0, 0));
        frog->setTag(0);
        layer->addChild(frog, 103);
        
        CCSprite* body = CCSprite::createWithSpriteFrameName("frog_body.png");
        body->setAnchorPoint(ccp(0.5, 1));
        body->setPosition(ccp(0, -60));
        body->setTag(1);
        layer->addChild(body, 100);
        
        CCSprite* face = CCSprite::createWithSpriteFrameName("frog_face.png");
        face->setPosition(ccp(-5, 10));
        face->setTag(2);
        layer->addChild(face, 104);
        
        CCSprite* hand = CCSprite::createWithSpriteFrameName("frog_hand.png");
        hand->setAnchorPoint(ccp(0.5, 1));
        hand->setPosition(ccp(0, -120));
        hand->setTag(3);
        layer->addChild(hand, 104);
    }
    
    return layer;
}
void Fairy::Anim_Frog_Sing(CCLayer* layer, CCLayer* gameLayer) // 점괘구리 : 노래부르기
{
    layer->getChildByTag(2)->setVisible(false);
    
    CCSprite* face = CCSprite::createWithSpriteFrameName("frog_face_sing.png");
    face->setPosition(ccp(25, -5));
    face->setTag(2); // tag = 2
    layer->addChild(face, 104);
    
    CCActionInterval* action = CCSequence::create(CCDelayTime::create(1.0f), CCCallFuncND::create(gameLayer, callfuncND_selector(Fairy::Anim_Callback), (void*)layer), NULL);
    face->runAction(action);
}
void Fairy::Anim_Frog_Eye(CCLayer* layer, CCLayer* gameLayer) // 점괘구리 : 깜빡깜빡
{
    ((CCSprite*)layer->getChildByTag(2))->setOpacity(0);
    
    CCSprite* face = CCSprite::createWithSpriteFrameName("frog_face_smile.png");
    face->setPosition(ccp(-5, 2));
    face->setTag(-1); // no tag
    layer->addChild(face, 104);
    
    CCActionInterval* action = CCSequence::create(CCDelayTime::create(0.3f), CCFadeIn::create(0.01f),
                                                  CCDelayTime::create(0.29f), CCFadeOut::create(0.01f),
                                                  CCDelayTime::create(0.29f), CCFadeIn::create(0.01f), NULL);
    CCActionInterval* action_smile = CCSequence::create(CCDelayTime::create(0.3f), CCFadeOut::create(0.01f),
                                                        CCDelayTime::create(0.29f), CCFadeIn::create(0.01f),
                                                        CCDelayTime::create(0.29f), CCFadeOut::create(0.01f),
                                                        CCDelayTime::create(0.3f),
                                                        CCCallFuncND::create(gameLayer, callfuncND_selector(Fairy::Anim_Callback), (void*)layer), NULL);
    layer->getChildByTag(2)->runAction(action);
    face->runAction(action_smile);
}
void Fairy::Anim_Frog_Card(CCLayer* layer, CCLayer* gameLayer) // 점괘구리 : 카드선택
{
    layer->getChildByTag(2)->setVisible(false);
    layer->getChildByTag(3)->setVisible(false);
    
    CCSprite* face = CCSprite::createWithSpriteFrameName("frog_face_openmouth.png");
    face->setPosition(ccp(-5, 2));
    face->setTag(2); // tag = 2
    layer->addChild(face, 104);
    
    CCSprite* hand = CCSprite::createWithSpriteFrameName("frog_hand_up.png");
    hand->setPosition(ccp(0, -110));
    hand->setTag(3); // tag = 3
    layer->addChild(hand, 105);
    
    CCSprite* cardleft = CCSprite::createWithSpriteFrameName("frog_card_left.png");
    cardleft->setScale(0.95f);
    cardleft->setPosition(ccp(-135, -35));
    cardleft->setTag(-1); // no tag
    layer->addChild(cardleft, 104);
    CCSprite* cardmid = CCSprite::createWithSpriteFrameName("frog_card_middle.png");
    cardmid->setScale(0.95f);
    cardmid->setPosition(ccp(-105, -20));
    cardmid->setTag(-1); // no tag
    layer->addChild(cardmid, 104);
    CCSprite* cardright = CCSprite::createWithSpriteFrameName("frog_card_right.png");
    cardright->setScale(0.95f);
    cardright->setPosition(ccp(-80, -25));
    cardright->setTag(-1); // no tag
    layer->addChild(cardright, 104);
    
    /*CCSprite* cardright2 = CCSprite::createWithSpriteFrameName("frog_card_right_2.png");
    cardright2->setScale(0.95f);
    cardright2->setPosition(ccp(140, -20));
    cardright2->setTag(-1); // no tag
    layer->addChild(cardright2, 104);
     */
    CCSprite* cardopen = CCSprite::createWithSpriteFrameName("frog_card_open.png");
    cardopen->setScale(0.95f);
    cardopen->setPosition(ccp(140, -20));
    cardopen->setTag(-1); // no tag
    cardopen->setOpacity(0);
    layer->addChild(cardopen, 104);
    
    CCActionInterval* action = CCSequence::create(CCDelayTime::create(0.5f),
                                                  CCSpawn::create(CCScaleTo::create(0.6f, 1.2f), CCMoveTo::create(0.6f, ccp(140, -20)), CCSequence::create(CCRotateBy::create(0.15f, 180), CCRotateBy::create(0.15f, 180), CCRotateBy::create(0.15f, 180), CCRotateBy::create(0.15f, 180), NULL), NULL),
                                                  CCDelayTime::create(0.5f),
                                                  CCFadeOut::create(0.6f),
                                                  CCDelayTime::create(0.3f),
                                                  CCCallFuncND::create(gameLayer, callfuncND_selector(Fairy::Anim_Callback), (void*)layer), NULL);
    CCActionInterval* action2 = CCSequence::create(CCDelayTime::create(1.6f), CCFadeIn::create(0.6f), CCDelayTime::create(0.3f), CCCallFuncND::create(gameLayer, callfuncND_selector(Fairy::Anim_Callback), (void*)layer), NULL);
    CCActionInterval* noaction = CCSequence::create(CCDelayTime::create(2.5f), CCCallFuncND::create(gameLayer, callfuncND_selector(Fairy::Anim_Callback), (void*)layer), NULL);
    
    cardright->runAction(action);
    cardopen->runAction(action2);
    face->runAction(noaction);
    hand->runAction((CCActionInterval*)noaction->copy());
    cardleft->runAction((CCActionInterval*)noaction->copy());
    cardmid->runAction((CCActionInterval*)noaction->copy());
}


////////////////////////////////////////////////////////////////////////////////////////////////
// 또사자 (id = 7)
////////////////////////////////////////////////////////////////////////////////////////////////
CCLayer* Fairy::MakeLion(bool isUI)
{
    CCLayer* layer = CCLayer::create();
    
    CCSprite* lion;
    if (isUI)
    {
        lion = CCSprite::createWithSpriteFrameName("lion/lion.png");
        lion->setPosition(ccp(0, 0));
        layer->addChild(lion, 99);
        layer->setContentSize(lion->getContentSize());
    }
    else
    {
        layer->setContentSize(CCSize(0, 0));
        
        CCSprite* body = CCSprite::createWithSpriteFrameName("lion_body.png");
        body->setPosition(ccp(0, 0));
        body->setTag(0);
        layer->addChild(body, 99);
        
        CCSprite* face = CCSprite::createWithSpriteFrameName("lion_face.png");
        face->setPosition(ccp(0, 30));
        face->setTag(1);
        layer->addChild(face, 100);
        CCSprite* face_l = CCSprite::createWithSpriteFrameName("lion_face_left.png");
        face_l->setPosition(ccp(-20, 33));
        face_l->setTag(2);
        face_l->setOpacity(0);
        layer->addChild(face_l, 100);
        CCSprite* face_r = CCSprite::createWithSpriteFrameName("lion_face_right.png");
        face_r->setPosition(ccp(20, 33));
        face_r->setTag(3);
        face_r->setOpacity(0);
        layer->addChild(face_r, 100);
        
        CCSprite* hand = CCSprite::createWithSpriteFrameName("lion_hand.png");
        hand->setAnchorPoint(ccp(0.5, 1));
        hand->setPosition(ccp(0, -50));
        hand->setTag(4);
        layer->addChild(hand, 98);
        
        CCSprite* handup = CCSprite::createWithSpriteFrameName("lion_hand_up.png");
        handup->setAnchorPoint(ccp(0.5, 1));
        handup->setPosition(ccp(0, -60));
        handup->setTag(5);
        handup->setOpacity(0);
        layer->addChild(handup, 98);
        
        CCSprite* bag_red = CCSprite::createWithSpriteFrameName("lion_bag_red.png");
        bag_red->setAnchorPoint(ccp(0.5, 1));
        bag_red->setPosition(ccp(-105, -105)); // -110, -85
        bag_red->setTag(10);
        bag_red->setOpacity(0);
        layer->addChild(bag_red, 101);
        CCSprite* bag_blue = CCSprite::createWithSpriteFrameName("lion_bag_blue.png");
        bag_blue->setAnchorPoint(ccp(0.5, 1));
        bag_blue->setPosition(ccp(-105, -105));
        bag_blue->setTag(11);
        bag_blue->setOpacity(0);
        layer->addChild(bag_blue, 101);
        CCSprite* bag_green = CCSprite::createWithSpriteFrameName("lion_bag_green.png");
        bag_green->setAnchorPoint(ccp(0.5, 1));
        bag_green->setPosition(ccp(-105, -105));
        bag_green->setTag(12);
        bag_green->setOpacity(0);
        layer->addChild(bag_green, 101);
        
        CCSprite* bag_red_2 = CCSprite::createWithSpriteFrameName("lion_bag_red_2.png");
        bag_red_2->setAnchorPoint(ccp(0.5, 1));
        //bag_red_2->setPosition(ccp(88, -85));
        bag_red_2->setPosition(ccp(83, -105));
        bag_red_2->setTag(20);
        bag_red_2->setOpacity(0);
        layer->addChild(bag_red_2, 101);
        
        CCSprite* bag_blue_2 = CCSprite::createWithSpriteFrameName("lion_bag_blue_2.png");
        bag_blue_2->setAnchorPoint(ccp(0.5, 1));
        //bag_blue_2->setPosition(ccp(103, -85));
        bag_blue_2->setPosition(ccp(98, -98));
        bag_blue_2->setTag(21);
        bag_blue_2->setOpacity(0);
        layer->addChild(bag_blue_2, 101);
        
        CCSprite* bag_green_2 = CCSprite::createWithSpriteFrameName("lion_bag_green_2.png");
        bag_green_2->setAnchorPoint(ccp(0.5, 1));
        //bag_green_2->setPosition(ccp(118, -85));
        bag_green_2->setPosition(ccp(113, -106));
        bag_green_2->setTag(22);
        bag_green_2->setOpacity(0);
        layer->addChild(bag_green_2, 101);
    }
    
    return layer;
}
void Fairy::Anim_Lion_Shopping(CCLayer* layer, CCLayer* gameLayer) // 또사자 : 쇼핑
{
    // 도리도리 + 초록가방 fadein + 으쌰으쌰 + 초록가방 오른쪽으로 +
    // 도리도리 + 파란가방 fadein + 으쌰으쌰 + 오른쪽으로 +
    // 도리도리 + 빨간가방 fadein + 으쌰으쌰 + 오른쪽으로
    
    // 도리도리(0.61), 왼가방fadeIn(0.39), 으쌰으쌰(0.61), 왼out오른in(0.01), 대기(0.30 -> 7.30)
    // 1.62 + 7.3 = 8.92

    CCActionInterval* dori = CCSequence::create(
                    CCCallFuncND::create(gameLayer, callfuncND_selector(Fairy::Lion_Doridori), (void*)layer), // 0.61f
                    CCDelayTime::create(8.92f),
                    CCCallFuncND::create(gameLayer, callfuncND_selector(Fairy::Lion_Doridori), (void*)layer), // 0.61f
                    CCDelayTime::create(8.92f),
                    CCCallFuncND::create(gameLayer, callfuncND_selector(Fairy::Lion_Doridori), (void*)layer), // 0.61f
                    CCDelayTime::create(8.92f),
                    CCCallFuncND::create(gameLayer, callfuncND_selector(Fairy::Lion_Clear), (void*)layer), // 원상복귀!
                    NULL);
    
    CCActionInterval* hand = CCSequence::create(
                    CCDelayTime::create(1.0f),
                    CCCallFuncND::create(gameLayer, callfuncND_selector(Fairy::Lion_MoveHand), (void*)layer), // 0.61f
                    CCDelayTime::create(8.92f),
                    CCCallFuncND::create(gameLayer, callfuncND_selector(Fairy::Lion_MoveHand), (void*)layer), // 0.61f
                    CCDelayTime::create(8.92f),
                    CCCallFuncND::create(gameLayer, callfuncND_selector(Fairy::Lion_MoveHand), (void*)layer), // 0.61f
                    NULL);
    
    CCActionInterval* bag_red = CCSequence::create(CCDelayTime::create(0.61f), CCFadeIn::create(0.39f),
                                                   CCPlace::create(ccp(-110, -85)), CCDelayTime::create(0.21f),
                                                   CCPlace::create(ccp(-105, -105)), CCDelayTime::create(0.20f),
                                                   CCPlace::create(ccp(-110, -85)), CCDelayTime::create(0.20f),
                                                   CCPlace::create(ccp(-105, -105)),
                                                   CCFadeOut::create(0.01f), CCDelayTime::create(7.30f),
                                                   NULL);
    CCActionInterval* bag_blue = CCSequence::create(CCDelayTime::create(8.92f+0.61f), CCFadeIn::create(0.39f),
                                                    CCPlace::create(ccp(-110, -85)), CCDelayTime::create(0.21f),
                                                    CCPlace::create(ccp(-105, -105)), CCDelayTime::create(0.20f),
                                                    CCPlace::create(ccp(-110, -85)), CCDelayTime::create(0.20f),
                                                    CCPlace::create(ccp(-105, -105)),
                                                    CCFadeOut::create(0.01f), CCDelayTime::create(7.30f),
                                                    NULL);
    CCActionInterval* bag_green = CCSequence::create(CCDelayTime::create(8.92f+8.92f+0.61f), CCFadeIn::create(0.39f),
                                                     CCPlace::create(ccp(-110, -85)), CCDelayTime::create(0.21f),
                                                     CCPlace::create(ccp(-105, -105)), CCDelayTime::create(0.20f),
                                                     CCPlace::create(ccp(-110, -85)), CCDelayTime::create(0.20f),
                                                     CCPlace::create(ccp(-105, -105)),
                                                     CCFadeOut::create(0.01f), CCDelayTime::create(7.30f),
                                                     NULL);
    CCActionInterval* bag_red_2 = CCSequence::create(CCDelayTime::create(1.61f), CCFadeIn::create(0.01f),
                                                     CCDelayTime::create(7.30f+1.0f),
                                                     CCPlace::create(ccp(88, -85)), CCDelayTime::create(0.21f),
                                                     CCPlace::create(ccp(83, -105)), CCDelayTime::create(0.20f),
                                                     CCPlace::create(ccp(88, -85)), CCDelayTime::create(0.20f),
                                                     CCPlace::create(ccp(83, -105)),
                                                     CCDelayTime::create(7.30f+1.0f),// 원래 7.31+ ...
                                                     CCPlace::create(ccp(88, -85)), CCDelayTime::create(0.21f),
                                                     CCPlace::create(ccp(83, -105)), CCDelayTime::create(0.20f),
                                                     CCPlace::create(ccp(88, -85)), CCDelayTime::create(0.20f),
                                                     CCPlace::create(ccp(83, -105)),
                                                     NULL);
    CCActionInterval* bag_blue_2 = CCSequence::create(CCDelayTime::create(8.92f+1.61f), CCFadeIn::create(0.01f),
                                                     CCDelayTime::create(7.30f+1.0f),
                                                     CCPlace::create(ccp(103, -85)), CCDelayTime::create(0.21f),
                                                     CCPlace::create(ccp(98, -98)), CCDelayTime::create(0.20f),
                                                     CCPlace::create(ccp(103, -85)), CCDelayTime::create(0.20f),
                                                     CCPlace::create(ccp(98, -98)),
                                                     NULL);
    CCActionInterval* bag_green_2 = CCSequence::create(CCDelayTime::create(8.92f+8.92f+1.61f), CCFadeIn::create(0.01f),
                                                       NULL);
    
    layer->runAction(dori); // face
    layer->runAction(hand); // hand
    layer->getChildByTag(10)->runAction(bag_red);
    layer->getChildByTag(11)->runAction(bag_blue);
    layer->getChildByTag(12)->runAction(bag_green);
    layer->getChildByTag(20)->runAction(bag_red_2);
    layer->getChildByTag(21)->runAction(bag_blue_2);
    layer->getChildByTag(22)->runAction(bag_green_2);
}
void Fairy::Lion_Doridori(CCNode* sender, void* data)
{
    CCLayer* layer = (CCLayer*)data;
    
    ((CCSprite*)layer->getChildByTag(1))->setOpacity(0); // middle face
    ((CCSprite*)layer->getChildByTag(2))->setOpacity(255); // left face

    CCActionInterval* action_left = CCSequence::create(CCDelayTime::create(0.20f), CCFadeOut::create(0.01f), NULL);
    CCActionInterval* action_middle = CCSequence::create(CCDelayTime::create(0.20f), CCFadeIn::create(0.01f),
                                                         CCDelayTime::create(0.19f), CCFadeOut::create(0.01f),
                                                         CCDelayTime::create(0.19f), CCFadeIn::create(0.01f), NULL);
    CCActionInterval* action_right = CCSequence::create(CCDelayTime::create(0.40f), CCFadeIn::create(0.01f),
                                                        CCDelayTime::create(0.19f), CCFadeOut::create(0.01f), NULL);
    layer->getChildByTag(1)->runAction(action_middle);
    layer->getChildByTag(2)->runAction(action_left);
    layer->getChildByTag(3)->runAction(action_right);
}
void Fairy::Lion_MoveHand(CCNode* sender, void* data)
{
    CCLayer* layer = (CCLayer*)data;
    
    ((CCSprite*)layer->getChildByTag(4))->setOpacity(0); // hand
    ((CCSprite*)layer->getChildByTag(5))->setOpacity(255); // hand up
    
    CCActionInterval* handup = CCSequence::create(CCDelayTime::create(0.20f), CCFadeOut::create(0.01f),
                                                  CCDelayTime::create(0.19f), CCFadeIn::create(0.01f),
                                                  CCDelayTime::create(0.19f), CCFadeOut::create(0.01f), NULL);
    CCActionInterval* hand = CCSequence::create(CCDelayTime::create(0.20f), CCFadeIn::create(0.01f),
                                                CCDelayTime::create(0.19f), CCFadeOut::create(0.01f),
                                                CCDelayTime::create(0.19f), CCFadeIn::create(0.01f), NULL);
    layer->getChildByTag(4)->runAction(hand);
    layer->getChildByTag(5)->runAction(handup);
}
void Fairy::Lion_Clear(CCNode* sender, void* data)
{
    CCLayer* layer = (CCLayer*)data;
    
    ((CCSprite*)layer->getChildByTag(1))->setOpacity(255); // face
    ((CCSprite*)layer->getChildByTag(4))->setOpacity(255); // hand
    
    int arr[9] = {2,3,5,10,11,12,20,21,22};
    for (int i = 0 ; i < 9 ; i++)
        ((CCSprite*)layer->getChildByTag(arr[i]))->setOpacity(0);
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 그래용 (id = 8)
////////////////////////////////////////////////////////////////////////////////////////////////
CCLayer* Fairy::MakeDragon(bool isUI)
{
    CCLayer* layer = CCLayer::create();
    
    CCSprite* dragon;
    if (isUI)
    {
        dragon = CCSprite::createWithSpriteFrameName("dragon/dragon.png");
        dragon->setPosition(ccp(0, 0));
        layer->addChild(dragon, 99);
        layer->setContentSize(dragon->getContentSize());
    }
    else
    {
        layer->setContentSize(CCSize(0, 0));
        
        CCSprite* head = CCSprite::createWithSpriteFrameName("dragon_head.png");
        head->setPosition(ccp(0, 0));
        head->setTag(0);
        layer->addChild(head, 101);
        
        CCSprite* body = CCSprite::createWithSpriteFrameName("dragon_body.png");
        body->setPosition(ccp(5, -180));
        body->setTag(1);
        layer->addChild(body, 99);
        
        CCSprite* face = CCSprite::createWithSpriteFrameName("dragon_face_1.png");
        face->setPosition(ccp(-3, -30));
        face->setTag(2);
        layer->addChild(face, 102);
        
        CCSprite* left = CCSprite::createWithSpriteFrameName("dragon_hand_left.png");
        left->setAnchorPoint(ccp(0.5, 1));
        left->setPosition(ccp(-50, -110));
        left->setTag(3);
        layer->addChild(left, 100);
        
        CCSprite* right = CCSprite::createWithSpriteFrameName("dragon_hand_right.png");
        right->setAnchorPoint(ccp(0.5, 1));
        right->setPosition(ccp(50, -110));
        right->setTag(4);
        layer->addChild(right, 100);
        
        CCSprite* tail = CCSprite::createWithSpriteFrameName("dragon_tail_1.png");
        tail->setAnchorPoint(ccp(0, 0));
        tail->setPosition(ccp(45, -250));
        tail->setTag(5);
        layer->addChild(tail, 98);
    }
    
    return layer;
}
void Fairy::Anim_Dragon_Nod(CCLayer* layer, CCLayer* gameLayer) // 그래용 : 끄덕끄덕
{
    /*
    CCSprite* newface = CCSprite::createWithSpriteFrameName("dragon_face_2.png");
    newface->setPosition(ccp(-3, -55));
    newface->setTag(-1); // no tag
    layer->addChild(newface, 105);
    CCActionInterval* action = CCSequence::create(CCDelayTime::create(0.25f), CCFadeOut::create(0.01f), CCDelayTime::create(0.24f), CCFadeIn::create(0.01f), CCDelayTime::create(0.24f), CCFadeOut::create(0.01f), CCCallFuncND::create(gameLayer, callfuncND_selector(Fairy::Anim_Callback), (void*)layer), NULL);
    CCActionInterval* action2 = CCSequence::create(CCFadeOut::create(0.01f), CCDelayTime::create(0.24f), CCFadeIn::create(0.01f), CCDelayTime::create(0.24f), CCFadeOut::create(0.01f), CCDelayTime::create(0.24f), CCFadeIn::create(0.01f), NULL);
    layer->getChildByTag(2)->runAction(action2);
    newface->runAction(action);
    */
    
    CCSprite* sigh = CCSprite::createWithSpriteFrameName("dragon_sigh.png");
    sigh->setPosition(ccp(0, -110));
    sigh->setTag(-1); // no tag
    sigh->setOpacity(0);
    layer->addChild(sigh, 105);
    CCActionInterval* action3 = CCSequence::create(CCSpawn::create(CCMoveBy::create(0.5f, ccp(0, -20)), CCFadeIn::create(0.5f), NULL),CCSpawn::create(CCMoveBy::create(0.5f, ccp(0, -20)), CCFadeOut::create(0.5f), NULL), CCCallFuncND::create(gameLayer, callfuncND_selector(Fairy::Anim_Callback), (void*)layer), NULL);
    sigh->runAction(action3);
}
void Fairy::Anim_Dragon_MoveTail(CCLayer* layer, CCLayer* gameLayer) // 그래용 : 살랑살랑
{
    ((CCSprite*)layer->getChildByTag(3))->setOpacity(0);
    ((CCSprite*)layer->getChildByTag(4))->setOpacity(0);
    
    CCSprite* left = CCSprite::createWithSpriteFrameName("dragon_handmove_left.png");
    left->setPosition(ccp(-10, -180));
    left->setTag(-1); // no tag
    layer->addChild(left, 100);
    
    CCSprite* right = CCSprite::createWithSpriteFrameName("dragon_handmove_right.png");
    right->setPosition(ccp(10, -180));
    right->setOpacity(0);
    right->setTag(-1); // no tag
    layer->addChild(right, 100);
    
    CCSprite* tail = CCSprite::createWithSpriteFrameName("dragon_tail_2.png");
    tail->setAnchorPoint(ccp(0, 0));
    tail->setPosition(ccp(45, -250));
    tail->setOpacity(0);
    tail->setTag(-1); // no tag
    layer->addChild(tail, 98);
    
    CCActionInterval* action_l = CCSequence::create( CCDelayTime::create(0.15f),
                                                    CCFadeOut::create(0.01f), CCDelayTime::create(0.44f),
                                                    CCFadeIn::create(0.01f), CCDelayTime::create(0.14f),
                                                    CCFadeOut::create(0.01f), CCCallFuncND::create(gameLayer, callfuncND_selector(Fairy::Anim_Callback), (void*)layer), NULL);
    CCActionInterval* action_m = CCSequence::create(CCDelayTime::create(0.15f), CCFadeIn::create(0.01f),
                                                    CCDelayTime::create(0.14f), CCFadeOut::create(0.01f),
                                                    CCDelayTime::create(0.14f), CCFadeIn::create(0.01f),
                                                    CCDelayTime::create(0.14f), CCFadeOut::create(0.01f),
                                                    CCDelayTime::create(0.14f), CCFadeIn::create(0.01f),
                                                    CCDelayTime::create(0.14f), CCFadeOut::create(0.01f),
                                                    CCDelayTime::create(0.14f), CCFadeIn::create(0.01f),
                                                    NULL);
    CCActionInterval* action_r = CCSequence::create(CCDelayTime::create(0.30f), CCFadeIn::create(0.01f),
                                                    CCDelayTime::create(0.14f), CCFadeOut::create(0.01f),
                                                    CCDelayTime::create(0.44f), CCFadeIn::create(0.01f),
                                                    CCDelayTime::create(0.14f), CCFadeOut::create(0.01f),
                                                    CCCallFuncND::create(gameLayer, callfuncND_selector(Fairy::Anim_Callback), (void*)layer), NULL);
    CCActionInterval* tail_1 = CCSequence::create(CCFadeOut::create(0.01f), CCDelayTime::create(0.14f),
                                                  CCFadeIn::create(0.01f), CCDelayTime::create(0.14f), NULL);
    CCActionInterval* tail_2 = CCSequence::create(CCFadeIn::create(0.01f), CCDelayTime::create(0.14f),
                                                  CCFadeOut::create(0.01f), CCDelayTime::create(0.14f), NULL);
    left->runAction(action_l);
    layer->getChildByTag(3)->runAction(action_m);
    layer->getChildByTag(4)->runAction((CCActionInterval*)action_m->copy());
    right->runAction(action_r);
    layer->getChildByTag(5)->runAction(CCRepeat::create(tail_1, 4));
    tail->runAction(CCSequence::create(CCRepeat::create(tail_2, 4), CCCallFuncND::create(gameLayer, callfuncND_selector(Fairy::Anim_Callback), (void*)layer), NULL));
}
void Fairy::Anim_Dragon_RotateArm(CCLayer* layer, CCLayer* gameLayer) // 그래용 : 팔흔들기
{
    layer->getChildByTag(3)->setVisible(false); // 왼손 없애기
    
    CCSprite* left = CCSprite::createWithSpriteFrameName("dragon_hand_hello.png");
    left->setAnchorPoint(ccp(1, 0.5));
    left->setPosition(ccp(-35, -145));
    left->setTag(3); // tag = 3
    layer->addChild(left, 100); // head보단 뒤에, body보단 앞에.
    
    CCActionInterval* action = CCSequence::create(CCRotateBy::create(0.3f, 30), CCRotateBy::create(0.3f, -30), CCRotateBy::create(0.3f, 30), CCRotateBy::create(0.3f, -30), CCCallFuncND::create(gameLayer, callfuncND_selector(Fairy::Anim_Callback), (void*)layer), NULL);
    left->runAction(action);
}


////////////////////////////////////////////////////////////////////////////////////////////////
// 놀꺼양 (id = 9)
////////////////////////////////////////////////////////////////////////////////////////////////
CCLayer* Fairy::MakeSheep(bool isUI)
{
    CCLayer* layer = CCLayer::create();
    
    CCSprite* sheep;
    if (isUI)
    {
        sheep = CCSprite::createWithSpriteFrameName("sheep/sheep.png");
        sheep->setPosition(ccp(0, 0));
        layer->addChild(sheep, 99);
        layer->setContentSize(sheep->getContentSize());
    }
    else
    {
        layer->setContentSize(CCSize(0, 0));
        
        sheep = CCSprite::createWithSpriteFrameName("sheep_body.png");
        sheep->setPosition(ccp(0, 0));
        sheep->setTag(0);
        layer->addChild(sheep, 99);
        
        CCSprite* face = CCSprite::createWithSpriteFrameName("sheep_face_1.png");
        face->setPosition(ccp(0, 5));
        face->setTag(1);
        layer->addChild(face, 100);
        
        CCSprite* hand = CCSprite::createWithSpriteFrameName("sheep_hand.png");
        hand->setPosition(ccp(10, -120));
        hand->setTag(2);
        layer->addChild(hand, 100);
    }
    
    return layer;
}
void Fairy::Anim_Sheep_MoveFace(CCLayer* layer, CCLayer* gameLayer) // 놀꺼양 : 도리도리
{
    layer->getChildByTag(1)->setVisible(false);
    
    CCSprite* face = CCSprite::createWithSpriteFrameName("sheep_face_2.png");
    face->setPosition(ccp(0, 5));
    face->setTag(1);
    layer->addChild(face, 100);
    
    CCActionInterval* action = CCSequence::create(CCMoveBy::create(0.25f, ccp(-20, 0)), CCMoveBy::create(0.5f, ccp(40, 0)), CCMoveBy::create(0.25f, ccp(-20, 0)), CCCallFuncND::create(gameLayer, callfuncND_selector(Fairy::Anim_Callback), (void*)layer), NULL);
    face->runAction(action);
}
void Fairy::Anim_Sheep_Grass(CCLayer* layer, CCLayer* gameLayer) // 놀꺼양 : 풀버리기
{
    layer->getChildByTag(2)->setVisible(false);
    
    CCSprite* left = CCSprite::createWithSpriteFrameName("sheep_hand_leftup.png");
    left->setPosition(ccp(10, -120));
    left->setTag(2);
    layer->addChild(left, 100);
    CCSprite* right = CCSprite::createWithSpriteFrameName("sheep_hand_rightup.png");
    right->setPosition(ccp(10, -120));
    right->setTag(2);
    right->setOpacity(0);
    layer->addChild(right, 100);
    
    CCActionInterval* action_l = CCSequence::create(CCFadeIn::create(0.01f),
                                                  CCDelayTime::create(0.14f), CCFadeOut::create(0.01f),
                                                  CCDelayTime::create(0.14f), CCFadeIn::create(0.01f),
                                                  CCDelayTime::create(0.14f), CCFadeOut::create(0.01f),
                                                  CCDelayTime::create(0.14f), CCFadeIn::create(0.01f),
                                                  CCDelayTime::create(0.14f), CCFadeOut::create(0.01f),
                                                  CCDelayTime::create(0.14f), CCFadeIn::create(0.01f),
                                                  CCDelayTime::create(0.14f), CCFadeOut::create(0.01f),
                                                  CCDelayTime::create(0.15f),
                                                  CCCallFuncND::create(gameLayer, callfuncND_selector(Fairy::Anim_Callback), (void*)layer), NULL); // 15*7 + 0.01
    CCActionInterval* action_r = CCSequence::create(CCDelayTime::create(0.15f), CCFadeIn::create(0.01f),
                                                  CCDelayTime::create(0.14f), CCFadeOut::create(0.01f),
                                                  CCDelayTime::create(0.14f), CCFadeIn::create(0.01f),
                                                  CCDelayTime::create(0.14f), CCFadeOut::create(0.01f),
                                                  CCDelayTime::create(0.14f), CCFadeIn::create(0.01f),
                                                  CCDelayTime::create(0.14f), CCFadeOut::create(0.01f),
                                                  CCDelayTime::create(0.14f), CCFadeIn::create(0.01f),
                                                  CCDelayTime::create(0.14f), CCFadeOut::create(0.01f),
                                                  CCCallFuncND::create(gameLayer, callfuncND_selector(Fairy::Anim_Callback), (void*)layer), NULL);
    left->runAction(action_l);
    right->runAction(action_r);
    
    for (int i = 0 ; i < 4 ; i++) // 왼쪽 손에서 나오는 풀
    {
        CCSprite* grass = CCSprite::createWithSpriteFrameName("sheep_grass.png");
        grass->setAnchorPoint(ccp(0.5, 0));
        grass->setPosition(ccp(-100, -120));
        grass->setOpacity(0);
        grass->setScale(1.1f);
        grass->setTag(-1);
        layer->addChild(grass, 110);
        
        CCActionInterval* action_grass = CCSequence::create( CCDelayTime::create(0.3f*i), CCFadeIn::create(0.01f), CCSpawn::create(CCMoveBy::create(0.6f, ccp(-20, -50)), CCRotateBy::create(0.6f, -90), CCFadeOut::create(0.6f), NULL), CCCallFuncND::create(gameLayer, callfuncND_selector(Fairy::Anim_Callback), (void*)layer), NULL);
        grass->runAction(action_grass);

        CCSprite* grass2 = CCSprite::createWithSpriteFrameName("sheep_grass.png");
        grass2->setAnchorPoint(ccp(0.5, 0));
        grass2->setPosition(ccp(140, -120));
        grass2->setOpacity(0);
        grass2->setScale(1.1f);
        grass2->setTag(-1);
        layer->addChild(grass2, 110);
        
        CCActionInterval* action_grass2 = CCSequence::create( CCDelayTime::create(0.3f*i+0.15f), CCFadeIn::create(0.01f), CCSpawn::create(CCMoveBy::create(0.6f, ccp(20, -50)), CCRotateBy::create(0.6f, 90), CCFadeOut::create(0.6f), NULL), CCCallFuncND::create(gameLayer, callfuncND_selector(Fairy::Anim_Callback), (void*)layer), NULL);
        grass2->runAction(action_grass2);
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////
// 아기별 (id = 10)
////////////////////////////////////////////////////////////////////////////////////////////////
CCLayer* Fairy::MakeBabyStar(bool isUI)
{
    CCLayer* layer = CCLayer::create();
    
    CCSprite* babystar;
    if (isUI)
    {
        babystar = CCSprite::createWithSpriteFrameName("babystar/babystar.png");
        babystar->setPosition(ccp(0, 0));
        layer->addChild(babystar, 99);
        layer->setContentSize(babystar->getContentSize());
    }
    else
    {
        layer->setContentSize(CCSize(0, 0));
        
        babystar = CCSprite::createWithSpriteFrameName("babystar_body_1.png");
        babystar->setPosition(ccp(0, 0));
        babystar->setTag(0);
        layer->addChild(babystar, 99);
        /*
        CCSprite* face = CCSprite::createWithSpriteFrameName("babystar_face.png");
        face->setPosition(ccp(-10, -20));
        face->setTag(1);
        layer->addChild(face, 100);
        */
    }
    
    return layer;
}
void Fairy::Anim_BabyStar_SayHello(CCLayer* layer, CCLayer* gameLayer) // 아기별 : 인사하기
{
    layer->getChildByTag(0)->setVisible(false);
    
    CCArray* animFrames = CCArray::createWithCapacity(8);
    
    char name[25];
    int number[8] = {1,2,3,2,1,2,3,2};
    for (int j = 0 ; j < 8 ; j++)
    {
        sprintf(name, "babystar_body_%d.png", number[j]);
        CCSpriteFrame* frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(name);
        animFrames->addObject(frame);
    }
    
    CCSprite* first = CCSprite::createWithSpriteFrameName("babystar_body_1.png");
    first->setPosition(ccp(0, 0));
    first->setTag(0); // tag = 0
    layer->addChild(first, 99);
    
    CCAnimation* animation = CCAnimation::createWithSpriteFrames(animFrames, 0.15f);
    CCAnimate* animate = CCAnimate::create(animation);
    CCFiniteTimeAction* action = CCSequence::create(animate, CCCallFuncND::create(gameLayer, callfuncND_selector(Fairy::Anim_Callback), (void*)layer), NULL);
    first->runAction(action);
    
    animFrames->removeAllObjects();
}
void Fairy::Anim_BabyStar_BlinkStar(CCLayer* layer, CCLayer* gameLayer) // 아기별 : 깜빡거리기
{
    CCSprite* star = CCSprite::createWithSpriteFrameName("babystar_effect_star.png");
    star->setPosition(ccp(-10, -30));
    layer->addChild(star, 105);
    
    CCActionInterval* action = CCSequence::create(CCBlink::create(1, 5), CCCallFuncND::create(gameLayer, callfuncND_selector(Fairy::Anim_Callback), (void*)layer), NULL);
    star->runAction(action);
}
void Fairy::Anim_BabyStar_FadeInOut(CCLayer* layer, CCLayer* gameLayer) // 아기별 : 사라지기
{
    CCActionInterval* action = CCSequence::create(CCFadeOut::create(0.6f), CCFadeIn::create(0.2f), NULL);
    
    int count = layer->getChildrenCount();
    for (int i = 0 ; i < count ; i++) // body(0), face(1)
        layer->getChildByTag(i)->runAction((CCActionInterval*)action->copy());
}
////////////////////////////////////////////////////////////////////////////////////////////////



void Fairy::Anim_Callback(CCNode* sender, void *data)
{
    int tag = sender->getTag();
    sender->removeFromParentAndCleanup(true);
    if (tag != -1)
        ((CCLayer*)data)->getChildByTag(tag)->setVisible(true);
}
