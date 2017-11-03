#ifndef TOWERDEFENSE_AGREGARCAMINOBUTTON_H
#define TOWERDEFENSE_AGREGARCAMINOBUTTON_H

#include "Button.h"
#include "Editor.h"

class AgregarCaminoButton : public Button {
private:
    Editor &editor;
public:
    AgregarCaminoButton(int id, SDL_Texture *texture, MousePosition &mousePosition,
                        Renderer &renderer, Editor &editor);
    void click();
};


#endif //TOWERDEFENSE_AGREGARCAMINOBUTTON_H
