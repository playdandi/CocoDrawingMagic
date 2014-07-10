#ifndef __poops8__Splash__
#define __poops8__Splash__

#include "Common.h"
#include "Sound.h"
#include "cocos-ext.h"

#define HTTP_JOIN -2
#define HTTP_VERSION 0
#define HTTP_LOGIN 1
#define HTTP_MYINFO 2
#define HTTP_REWARDWEELYRANK 3
#define HTTP_FRIENDS 4
#define HTTP_PROFILE_IMAGE 5
#define HTTP_NONCONSUMED_GET_FRIEND_ID -10
#define HTTP_NONCONSUMEDITEMS -100

using namespace cocos2d;
using namespace cocos2d::extension;

USING_NS_CC_EXT;

class Splash : public CCLayerColor,
                public CCIMEDelegate
{
public:
    ~Splash(void);
	virtual bool init();
    virtual void onEnter();
    virtual void onExit();
    virtual void keyBackClicked();

    void onInitComplete();
    void onInitErrorComplete(const char* status, const char* error);
    void onAuthComplete(bool result);
    void onAuthErrorComplete(char const* status, char const* error);
    void onTokenComplete();
    void onTokenErrorComplete(char const* status, char const* error);
    void onLoginComplete();
    void onLoginErrorComplete(char const* status, char const* error);
    void onLocalUserComplete();
    void onLocalUserErrorComplete(char const* status, char const* error);
    void onFriendsComplete();
    void onFriendsErrorComplete(char const* status, char const* error);
    void onLogoutComplete();
    void onLogoutErrorComplete(char const* status, char const* error);
    void onUnregisterComplete();
    void onUnregisterErrorComplete(char const* status, char const* error);


    virtual bool ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent);
	virtual void ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent);
	virtual void ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent);
    
	static CCScene* scene();
	CREATE_FUNC(Splash);
    
    void Callback_Logo_KakaoGame(CCNode* sender, void* p);
    void Callback_Logo_playDANDi(CCNode* sender, void* p);
    void LogoLoadingCompleted();
    void SoundCallback(CCNode* sender, void* p);
    void Button_Callback();

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
    void CheckFriendList();
    void XmlParseGetFriendKakaoId(xml_document *xmlDoc);
    
    void onHttpRequestCompleted(CCNode *sender, void *data);
    void onHttpRequestCompletedNoEncrypt(CCNode *sender, void *data);
    
    void GetNonConsumedItems(std::string friendKakaoId);
    
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
    bool isKeybackTouched;
    bool isTouched;
    bool waitTouch;
    bool stopHere; // 로그인하면 자동적으로 tokenlistener도 호출하는 것 같다. join관련 (게임시작 버튼) 두번 방지 위해 씀.
    
    int gameVersion; // 새로 받은 게임버전
    bool needToLoginForKakao;
    bool isKakaoLoading;
    
    int mKakaoId;
    int mDeviceType;
    int httpStatus;
    int profileCntTotal;
    int profileCnt;
    
    std::string sessionId;
    char regId[1000]; // 유저의 안드로이드 기기 고유 id
    
private:
    CCSize vs;
    CCPoint vo;
    
    CCSprite* m_pBackground;
    CCSprite* m_pTitle;
    CCSprite* m_pForKakao;
    CCLabelTTF* m_pMsgLabel;
    CCSprite* m_pStartBtn;
    CCSprite* m_pStartLetter;
    CCSprite* m_pKakaoBtn;
    
    CCTextFieldTTF* m_pEditName;
};


#endif /* defined(__poops8__Splash__) */


