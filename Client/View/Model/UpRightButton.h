#ifndef TOWERDEFENSE_UPRIGHTBUTTON_H
#define TOWERDEFENSE_UPRIGHTBUTTON_H


#include "../Common/Button.h"
#include "../Common/Announcement.h"

class UpRightButton : public Button {
private:
    const int type;
    int &command;
    Image transparency;
    Announcement *keySym;

public:
    UpRightButton(Window &w, int element, int &cmd, SDL_Texture *texture,
    SDL_Texture *transp, MousePosition &mousePosition,
    Renderer &renderer, int n, std::string &text);
    void draw(int number, int padding) override;
    void click() override;
};


#endif //TOWERDEFENSE_UPRIGHTBUTTON_H
