#ifndef TOWERDEFENSE_NUEVAHORDABUTTON_H
#define TOWERDEFENSE_NUEVAHORDABUTTON_H

#include "Button.h"
#include "Editor.h"

class NuevaHordaButton : public Button {
private:
    Editor &editor;

    void click();

public:
    NuevaHordaButton(int id, SDL_Texture *texture, MousePosition &mousePosition,
                     Renderer &renderer, Editor &editor);

};


#endif //TOWERDEFENSE_NUEVAHORDABUTTON_H
