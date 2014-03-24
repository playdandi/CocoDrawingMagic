#include "ParticleTest.h"


CCScene* ParticleTest::scene()
{
    CCScene* pScene = CCScene::create();
    ParticleTest* pLayer = ParticleTest::create();
    pScene->addChild(pLayer);
    
	return pScene;
}

void ParticleTest::onEnter()
{
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
    CCLayer::onEnter();
}
void ParticleTest::onExit()
{
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
}

void ParticleTest::keyBackClicked()
{
    EndScene();
}


bool ParticleTest::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
    // notification post
    CCString* param = CCString::create("1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification("GameReady", param);
    
    CCLog("ParticleTest. init");
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    InitSprites();
    
    this->setKeypadEnabled(true);
    this->setTouchEnabled(true);
    
    return true;
}

void ParticleTest::InitSprites()
{
    CCSprite* pBlack = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, winSize.width, winSize.height));
    pBlack->setPosition(ccp(0, 0));
    pBlack->setAnchorPoint(ccp(0, 0));
    pBlack->setColor(ccc3(0, 0, 0));
    pBlack->setOpacity(255);
    this->addChild(pBlack, 0);
    
    // x
    sp_x = CCSprite::createWithSpriteFrameName("button/btn_x_yellow.png");
    sp_x->setAnchorPoint(ccp(0, 0));
    //sp_x->setPosition(ccp(winSize.width/2, winSize.height/2));
    sp_x->setPosition(ccp(450, 200));
    sp_x->setOpacity(255);
    this->addChild(sp_x, 700);
    
    btn = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, 200, 100));
    btn->setPosition(ccp(0, 200));
    btn->setAnchorPoint(ccp(0, 0));
    btn->setColor(ccc3(255,255,255));
    btn->setOpacity(255);
    this->addChild(btn, 100);
    label = CCLabelTTF::create("flower", "Arial", 40);
    label->setColor(ccc3(0,0,0));
    label->setAnchorPoint(ccp(0.5,0.5));
    label->setPosition(ccp(btn->getContentSize().width/2, btn->getContentSize().height/2));
    btn->addChild(label, 5);
    
    btn2 = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, 200, 100));
    btn2->setPosition(ccp(200, 200));
    btn2->setAnchorPoint(ccp(0, 0));
    btn2->setColor(ccc3(255,255,255));
    btn2->setOpacity(255);
    this->addChild(btn2, 100);
    label2 = CCLabelTTF::create("s1", "Arial", 40);
    label2->setColor(ccc3(0,0,0));
    label2->setAnchorPoint(ccp(0.5,0.5));
    label2->setPosition(ccp(btn2->getContentSize().width/2, btn2->getContentSize().height/2));
    btn2->addChild(label2, 5);
    
    type = 0;
    stype = 1;
    
    // COLOR PAPER
    //this->schedule(schedule_selector(ParticleTest::MakeColorPaper), 0.02f);
    //this->schedule(schedule_selector(ParticleTest::DeleteColorPaper), 1.0f);
    //this->unschedule(schedule_selector(Puzzle::ComboTimer));
    
    s1 = CCSprite::create("images/1.png");
    s2 = CCSprite::create("images/2.png");
    //CCSprite* s3 = CCSprite::create("images/3.png");
    
    MakeParticles(type, s1);
}

void ParticleTest::MakeParticles(int type, CCSprite* s)
{
    for (int i = 0 ; i < particles.size(); i++) {
        this->removeChild(particles[i]);
        //particles[i]->removeChildren();
      //  particles[i]->removeFromParentAndCleanup(true);
    }
    particles.clear();
    
    // PARTICLE
    if (type == 0) {
        MakeParticle("flower", s, "", 2.0, 70, ccp(0.5,0.5), ccp(500, 700));
        MakeParticle("flower", s, "screen", 2.0, 70, ccp(0.5,0.5), ccp(200, 1200));
        MakeParticle("flower", s, "dodge", 2.0, 70, ccp(0.5,0.5), ccp(winSize.width/2, 1200));
        MakeParticle("flower", s, "blend", 2.0, 70, ccp(0.5,0.5), ccp(1080-200, 1200));
    }
    else if (type == 1) {
        MakeParticle("fire", s, "", 2.0, 70, ccp(0.5,0.5), ccp(500, 700));
        MakeParticle("fire", s, "screen", 2.0, 70, ccp(0.5,0.5), ccp(200, 1200));
        MakeParticle("fire", s, "dodge", 2.0, 70, ccp(0.5,0.5), ccp(winSize.width/2, 1200));
        MakeParticle("fire", s, "blend", 2.0, 70, ccp(0.5,0.5), ccp(1080-200, 1200));
    }
    else {
        MakeParticle("fireworks", s, "", 2.0, 70, ccp(0.5,0.5), ccp(500, 700));
        MakeParticle("fireworks", s, "screen", 2.0, 70, ccp(0.5,0.5), ccp(200, 1200));
        MakeParticle("fireworks", s, "dodge", 2.0, 70, ccp(0.5,0.5), ccp(winSize.width/2, 1200));
        MakeParticle("fireworks", s, "blend", 2.0, 70, ccp(0.5,0.5), ccp(1080-200, 1200));
    }
    //CCActionInterval* action = CCMoveBy::create(0.5f, ccp(0, 50));
    //CCActionInterval* rep = CCRepeat::create(action, 100);
    //par->runAction(rep);
}

void ParticleTest::MakeParticle(std::string type, CCSprite* sp, std::string blend, float scale, int speed, CCPoint ap, CCPoint pos)
{
    CCParticleSystem* par;
    if (type == "flower")    par = CCParticleFlower::create();
    else if (type == "fire") par = CCParticleFire::create();
    else if (type == "fireworks") par = CCParticleFireworks::create();
    par->retain();
    par->setTexture(sp->getTexture());
    
    
    if (blend == "blend") {
        ccBlendFunc bf = {GL_DST_ALPHA, GL_ONE_MINUS_SRC_ALPHA}; // blend
        par->setBlendFunc(bf);
    }
    else if (blend == "dodge") {
        ccBlendFunc bf = {GL_ONE, GL_ONE}; // linear dodge
        par->setBlendFunc(bf);
    }
    else if (blend == "screen") {
        ccBlendFunc bf = {GL_ONE_MINUS_DST_COLOR, GL_ONE}; // screen
        par->setBlendFunc(bf);
    }
    
    
    par->setScale(scale);
    par->setSpeed(speed);
    par->setAnchorPoint(ap);
    par->setPosition(pos);
    this->addChild(par, 1000);
    
    particles.push_back(par);
}

/*
 void ParticleTest::MakeColorPaper(float f)
 {
 //for (int i = 0 ; i < 3; i++)
 //{
 int effect = rand()%6 + 1;
 char name[27];
 sprintf(name, "effect/effect_paper_%d.png", effect);
 CCSprite* p = CCSprite::createWithSpriteFrameName(name);
 int w = rand() % (int)winSize.width;
 p->setAnchorPoint(ccp(0.5, 0.5));
 p->setPosition(ccp(w, winSize.height+10));
 
 int s = rand()%3 + 6;
 p->setScale((float)s/(float)10);
 this->addChild(p, 100);
 //paper.push_back(const value_type &__x)
 
 CCFiniteTimeAction* action = CCSpawn::create(CCMoveBy::create(0.2f, ccp(xrand(), -192)),
 CCRotateBy::create(0.2f, -20, 60),
 //CCRotateBy::create(0.02f, 60),
 //CCFlipX::create(true),
 //CCFlipY::create(true),
 NULL);
 
 CCFiniteTimeAction* rep = CCSequence::create(CCRepeat::create(action, 11),
 CCCallFuncN::create(this, callfuncN_selector(ParticleTest::RemoveColorPaper)), NULL);
 p->runAction(rep);
 //}
 }
 void ParticleTest::RemoveColorPaper(CCNode* sender)
 {
 
 sender->removeFromParentAndCleanup(true);
 //sender->removeFromParentAndCleanup(true);
 }
 */


bool ParticleTest::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    CCPoint point = pTouch->getLocation();
    
    if (sp_x->boundingBox().containsPoint(point))
        EndScene();
    
    if (btn->boundingBox().containsPoint(point)) {
        type++;
        if (type > 2) type = 0;
        
        if (type == 0) {
            label->setString("flower");
        }
        else if (type == 1) {
            label->setString("fire");
        }
        else {
            label->setString("fireworks");
        }
        CCLog("%d %d", type, stype);
        if (stype == 1)
            MakeParticles(type, s1);
        else
            MakeParticles(type, s2);
    }
    
    if (btn2->boundingBox().containsPoint(point)) {
        stype++;
        if (stype > 2) stype = 1;
        if (stype == 1)
            label2->setString("s1");
        else
            label2->setString("s2");
        
        if (stype == 1) MakeParticles(type, s1);
        else MakeParticles(type, s2);
    }
    
    return true;
}


void ParticleTest::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
    //CCPoint point = pTouch->getLocation();
}

void ParticleTest::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
}


void ParticleTest::EndScene()
{
    CCString* param = CCString::create("0");
    CCNotificationCenter::sharedNotificationCenter()->postNotification("GameReady", param);
    
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    
    this->removeFromParentAndCleanup(true);
}

void ParticleTest::EndSceneCallback()
{
}

