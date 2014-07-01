#ifndef __poops8__Splash__
#define __poops8__Splash__

#include "Common.h"
#include "Sound.h"
#include "cocos-ext.h"

#define HTTP_VERSION 0
#define HTTP_LOGIN 1
#define HTTP_MYINFO 2
#define HTTP_REWARDWEELYRANK 3
#define HTTP_FRIENDS 4
#define HTTP_PROFILE_IMAGE 5
#define HTTP_NONCONSUMEDITEMS -100

using namespace cocos2d::extension;

USING_NS_CC_EXT;

class Splash : public CCLayer,
                public CCIMEDelegate
{
public:
    ~Splash(void);
	virtual bool init();
    virtual void onEnter();
    virtual void onExit();
    virtual void keyBackClicked();
    
    virtual bool ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent);
	virtual void ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent);
	virtual void ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent);
    
	static CCScene* scene();
	CREATE_FUNC(Splash);
    
    void LogoLoadingCompleted();
    void SoundCallback(CCNode* sender, void* p);
    void Button_Callback();
    
    //virtual void editBoxEditingDidBegin(CCEditBox* editBox);
    //virtual void editBoxEditingDidEnd(CCEditBox* editBox);
    //virtual void editBoxTextChanged(CCEditBox* editBox, const std::string& text);
    //virtual void editBoxReturn(CCEditBox* editBox);
    void keyboardWillShow(CCIMEKeyboardNotificationInfo &info);
    void keyboardWillHide(CCIMEKeyboardNotificationInfo &info);
    
    void TryLogin();
    
    void XMLParseGameData();
    void WriteResFile(char* data, int size);
    
    void XmlParseVersion(xml_document *xmlDoc);
    void XmlParseLogin(xml_document *xmlDoc);
    void XmlParseMyInfo(xml_document *xmlDoc);
    void XmlParseRewardWeeklyRank(xml_document *xmlDoc);
    void XmlParseFriends(xml_document *xmlDoc);
    
    void onHttpRequestCompleted(CCNode *sender, void *data);
    void onHttpRequestCompletedNoEncrypt(CCNode *sender, void *data);
    
    void GetNonConsumedItems();
    
    void GetTodayCandyFriend();
    void LastActionStart();
    void LastActionCallback(CCNode* sender, void *data);
    void LastActionCallback2(CCNode* sender, void *data);
    
    std::string SubstrNickname(std::string nickname);
    void EndScene();
    
    RSA* createRSA(unsigned char * key, int pub);
    
    
protected:
    CCSize winSize;
    bool isLoading;
    bool isStarting;
    bool isKeyBackClicked;
    
    int gameVersion; // 새로 받은 게임버전
    
    int mKakaoId;
    int mDeviceType;
    int httpStatus;
    int profileCntTotal;
    int profileCnt;
    
    std::string sessionId;
    char regId[1000]; // 유저의 안드로이드 기기 고유 id
    
private:
    CCSprite* m_pBackground;
    CCSprite* m_pTitle;
    CCSprite* m_pForKakao;
    CCLabelTTF* m_pMsgLabel;
    CCSprite* m_pStartBtn;
    CCSprite* m_pStartLetter;
    
    CCTextFieldTTF* m_pEditName;
};


#endif /* defined(__poops8__Splash__) */


