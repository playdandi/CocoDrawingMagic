#ifndef __CocoMagic__Sound__
#define __CocoMagic__Sound__

#include "Common.h"

class Sound
{
public:
    void PreLoadSound();
    void playBackgroundSound();
    void StopBackgroundSound();
    
    void playMenuComeup();
    void playClickX();
    void playClickBtn();
    void playWarning();
    
    /*
    void SetFileExtension();
    void PreLoadSound();
    void playTouchSound();
    void playBombSound();
    void playBackgroundSound(bool isCrushing);
    void StopBackgroundSound();
    void playComboSound(int combo);
    void playItemSound();
    void playMassSound();
    */
private:
    std::string ext;
};

#endif /* defined(__CocoMagic__Sound__) */
