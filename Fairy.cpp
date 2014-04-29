#include "Fairy.h"

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
    CCLayer* fairyLayer = CCLayer::create();
    
    CCSprite* face = CCSprite::createWithSpriteFrameName("sun/sun_face.png");
    face->setAnchorPoint(ccp(0.5, 0.5));
    face->setPosition(ccp(0, 0));
    fairyLayer->addChild(face, 50);
    CCSprite* mouth = CCSprite::createWithSpriteFrameName("sun/sun_mouth.png");
    mouth->setAnchorPoint(ccp(0.5, 0.5));
    mouth->setPosition(ccp(0, -30));
    fairyLayer->addChild(mouth, 50);
    
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
    eyeLayer->setTag(0);
    fairyLayer->addChild(eyeLayer, 50);
    
    CCLayer* tails = CCLayer::create(); // 태양 주위 8개에 대한 layer
    tails->setPosition(ccp(0, 0));
    fairyLayer->addChild(tails, 50);
    
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
    
    //fairyLayer->setScale(0.8f);
    //fairyLayer->setPosition(ccp(800, vs.height+vo.y-530));
    //fairyLayer->setPosition(ccp(m_winSize.width-280, vo.y+tbSize.height+boardSize.height+60));
    //this->addChild(fairyLayer, 1000);
    
    // action
    //CCFiniteTimeAction* action = CCSequence::create(CCMoveBy::create(0.5f, ccp(0, -5)), CCMoveBy::create(0.5f, ccp(0, +5)), NULL);
    //CCAction* rep = CCRepeatForever::create((CCActionInterval*)action);
    //fairyLayer->runAction(rep);
    
    return fairyLayer;
}


