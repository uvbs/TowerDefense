#ifndef TOWERDEFENSE_ELIMINARENEMIGOBUTTON_H
#define TOWERDEFENSE_ELIMINARENEMIGOBUTTON_H

#include "Button.h"
#include "Editor.h"

class EliminarEnemigoButton : public Button {
private:
    int horda;
    std::string enemigo;
    Editor &editor;

    void click();

public:
    EliminarEnemigoButton(int id, int horda, const std::string &enemigo,
                          SDL_Texture *texture, MousePosition &mousePosition,
                          Renderer &renderer, Editor &editor);

};


#endif //TOWERDEFENSE_ELIMINARENEMIGOBUTTON_H