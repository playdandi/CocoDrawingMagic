#include "Network.h"
#include <sstream>

void Network::replaceAll(std::string& str, const std::string& from, const std::string& to)
{
    if (from.empty())
        return;
    
    size_t start_pos = 0;
    while ( (start_pos = str.find(from, start_pos)) != std::string::npos )
    {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

void Network::HttpPost(std::string data, std::string url, void* pointer, SEL_HttpResponse hr, std::string tag)
{
    CCLog("HttpPost : url = %s", url.c_str());

    Network::replaceAll(data, "&", "||");
    
    CCLog("param length = %d", (int)data.size());
    CCLog("param = %s", (unsigned char*)(data.c_str()));

    // data => 다음 3단계를 거쳐 암호화한다.
    // 1) RSA_public_encrypt (512 bit씩 쪼개서 암호화)
    // 2) base64_encode
    // 3) replacing '+' to '-'
   
    int unitLength = RSA_BIT/8 - 11; // substring 한 개의 단위길이
    int numOfSubstr = (int)data.size() / unitLength; // substring 개수
    if ((int)data.size() % unitLength > 0)
        numOfSubstr++;
    
    std::string totalEncryptedString = "";
    int totalEncryptedLength = 0;
    
    unsigned char encrypted[1024];
    int result;
    std::string subs;

    // 1) RSA_public_encrypt (512 bit씩 쪼개서 암호화)
    for (int i = 0 ; i < numOfSubstr ; i++)
    {
        subs = data.substr(i*unitLength, unitLength);
        result = RSA_public_encrypt((int)subs.size(), (unsigned char*)(subs.c_str()), encrypted, rsa, RSA_PKCS1_PADDING);
        if (result == -1)
        {
            char* err;
            ERR_load_crypto_strings();
            ERR_error_string(ERR_get_error(), err);
            CCLog("RSA ERROR = %s", err);
            exit(0);
        }
        //CCLog("%d번째 문자열 = %s , %d", i, subs.c_str(), size);
        //CCLog("%d번째 암호화 길이 = %d", i, result);
   
        for (int i = 0 ; i < result ; i++)
            totalEncryptedString.push_back(encrypted[i]);
        totalEncryptedLength += result;
    }
    
    // 2) base64_encode + 3) replacing '+' to '-'
    std::string encoded = Common::base64_encode((unsigned char*)totalEncryptedString.c_str(), totalEncryptedLength);
    for (int i = 0 ; i < encoded.size() ; i++)
        if (encoded[i] == '+')
            encoded[i] = '-';
    
    
    // 이제 파라미터를 만든다. (필요 파라미터 : PS, a)
    std::string postData = "";
    char temp[1024*40];
    
    // PS
    sprintf(temp, "PS=%d|%d|%d|%d&", myInfo->GetKeyValue(), myInfo->GetUserId(), CCUserDefault::sharedUserDefault()->getIntegerForKey("gameVersion"), CCUserDefault::sharedUserDefault()->getIntegerForKey("binaryVersion"));
    postData += temp;
    
    // a (encrypted data)
    sprintf(temp, "a=%s", encoded.c_str());
    postData += temp;
    
    CCLog("%s", postData.c_str());

     
    CCHttpRequest* req = new CCHttpRequest();
    req->setUrl(url.c_str());
    req->setRequestData(postData.c_str(), postData.size());
    req->setRequestType(CCHttpRequest::kHttpPost);
    req->setResponseCallback((CCObject*)pointer, hr);
    if (tag != "")
        req->setTag(tag.c_str());
    CCHttpClient::getInstance()->send(req);
    req->release();
}

int Network::GetHttpResponseData(CCHttpResponse* res, char* data, bool isDeObfuscate)
{
    if (!res || !res->isSucceed())
    {
        CCLog("res failed. error buffer: %s", res->getErrorBuffer());
        return NULL;
    }
    
    // dump data
    std::vector<char> *buffer = res->getResponseData();
    std::string dumpData = "";
    for (unsigned int i = 0 ; i < buffer->size() ; i++)
        dumpData.push_back( (*buffer)[i] );

    int bufferSize = Network::DeObfuscation(dumpData, data);
    
    return bufferSize;
}

int Network::DeObfuscation(std::string obfuscatedStr, char* data)
{
    int obfKey = atoi(obfuscatedStr.substr(0, 2).c_str()) - 10; // 앞 두자리는 key값이므로 분리한다.
    obfuscatedStr = obfuscatedStr.substr(2);
    
    // replacing '-' to '+'
    Network::replaceAll(obfuscatedStr, "-", "+");
    // base64_decode
    obfuscatedStr = Common::base64_decode(obfuscatedStr);
    
    // xor operation with a key
    int keyLen = obfuscationKey[obfKey].size();
    std::string decryptedKey = "";
    for (int i = 0 ; i < obfuscatedStr.size() ; i++)
    {
        decryptedKey += obfuscatedStr[i] ^ obfuscationKey[obfKey][i%keyLen];
    }
    
    sprintf(data, "%s", decryptedKey.c_str());
    
    return (int)decryptedKey.size();
}
