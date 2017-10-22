#ifndef TOWERDEFENSE_RENDERER_H
#define TOWERDEFENSE_RENDERER_H

#include <SDL2/SDL.h>
#include "Window.h"
#include "ViewError.h"

class Renderer {
private:
    SDL_Renderer *renderer;
    int mapWidth;
    int mapHeight;
    int cameraX; // x superior mostrado
    int cameraY; // y superior mostrado
    int paddingWidth;
    int paddingHeight;
    int zoom;
    int windowWidth;
    int windowHeight;

public:
    Renderer(Window &window, int mapWidth, int mapHeight);
    ~Renderer();
    void copy(SDL_Texture *texture, const SDL_Rect *src, SDL_Rect *dst);
    void clearRender();
    void present();
    void updateCamera(int x, int y);
    int cartesianToIsometricX(int x, int y);
    int cartesianToIsometricY(int x, int y);
    SDL_Renderer* getRenderer();
    void zoomIn();
    void zoomOut();

    bool isOnCamera(int x, int y);
};


#endif //TOWERDEFENSE_RENDERER_H
