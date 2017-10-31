#include "Renderer.h"

#define HEIGHTFACTOR 80
#define WIDTHFACTOR 80

#define PADDING 400
#define ZOOMINITIAL 1

#define MOVECAMERA 10
#define TOLERANCE 380

Renderer::Renderer(Window &window, int mapWidth, int mapHeight) :
        mapWidth(mapWidth * WIDTHFACTOR * 2),
        mapHeight(mapHeight * HEIGHTFACTOR),
        cameraX(this->mapWidth/4), cameraY(this->mapHeight/4),
        paddingWidth(mapWidth * WIDTHFACTOR + PADDING),
        paddingHeight(PADDING),
        zoom(ZOOMINITIAL),
        renderer(SDL_CreateRenderer(window.getWindow(), -1, 0)),
        windowWidth(window.getWidth()), windowHeight(window.getHeight()) {
    if (!renderer)
        throw ViewError("Create renderer error: %s", SDL_GetError());
}

Renderer::~Renderer() {
    SDL_DestroyRenderer(renderer);
}

void Renderer::copy(SDL_Texture *texture,
                    const SDL_Rect *src, SDL_Rect *dst,
                    int offsetX, int offsetY) {
    int x = dst->x;
    int y = dst->y;
    dst->x = cartesianToIsometricX(x, y) - offsetX * zoom - cameraX;
    dst->y = cartesianToIsometricY(x, y) - offsetY * zoom - cameraY;
    if (!isOnCamera(dst->x, dst->y)) {
        dst->x = x;
        dst->y = y;
        return;
    }
    int dstW = dst->w;
    int dstH = dst->h;
    dst->w *= zoom;
    dst->h *= zoom;
    SDL_RenderCopy(renderer, texture, src, dst);
    dst->x = x;
    dst->y = y;
    dst->w = dstW;
    dst->h = dstH;
}

void Renderer::copyEuclidean(SDL_Texture *texture, SDL_Rect *src, SDL_Rect
*dst) {
    SDL_RenderCopy(renderer, texture, src, dst);
}

void Renderer::copyEuclidean(SDL_Texture *texture, SDL_Rect *dst) {
    SDL_RenderCopy(renderer, texture, NULL, dst);
}

void Renderer::clearRender() {
    SDL_RenderClear(renderer);
}

void Renderer::present() {
    SDL_RenderPresent(renderer);
}

void Renderer::updateCamera(int x, int y) {
    int factor = MOVECAMERA * zoom;

    this->cameraX += (x * factor);
    if (cameraX < 0) cameraX = 0;
    if (cameraX > (mapWidth * zoom - windowWidth + 2 * PADDING * zoom))
        cameraX = mapWidth * zoom - windowWidth + 2 * PADDING * zoom;

    this->cameraY += (y * factor);
    if (cameraY < 0) cameraY = 0;
    if (cameraY > (mapHeight * zoom - windowHeight + 2 * PADDING))
        cameraY = mapHeight * zoom - windowHeight + 2 * PADDING;
}

SDL_Renderer *Renderer::getRenderer() {
    return renderer;
}

int Renderer::cartesianToIsometricX(int x, int y) {
    return (((x  - y) * WIDTHFACTOR) + paddingWidth - WIDTHFACTOR) * zoom;
}

int Renderer::cartesianToIsometricY(int x, int y) {
    return ((x  + y) * HEIGHTFACTOR * zoom / 2) + paddingHeight;
}

void Renderer::zoomIn() {
    if (zoom < 5) {
        zoom += 1;
        cameraX += mapWidth / 2;
        cameraY += windowHeight / 2 + WIDTHFACTOR;
    }
}

void Renderer::zoomOut() {
    if (zoom > 1) {
        zoom -= 1;
        cameraX -= mapWidth / 2;
        cameraY -= windowHeight / 2 + WIDTHFACTOR;
    }
}

bool Renderer::isOnCamera(int x, int y) {
    return ((x >= -(WIDTHFACTOR + TOLERANCE) * zoom) &&
            (y >= -(HEIGHTFACTOR + TOLERANCE) * zoom) &&
            (x <= windowWidth) && (y <= windowHeight));
}