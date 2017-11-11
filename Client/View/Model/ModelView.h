#ifndef TOWERDEFENSE_MODELVIEW_H
#define TOWERDEFENSE_MODELVIEW_H


#include "ShotView.h"
#include "MapView.h"
#include "DepthLevel.h"
#include <mutex>

#include <map>

class ModelView {
private:
    Renderer &renderer;
    TextureLoader &textureLoader;
    MapView map;
    std::mutex m;
    std::vector<ShotView *> shots;
    std::vector<DepthLevel *> depthLevels;
    std::map<int, int> idDepthLevelsTowers;
    std::map<int, int> idDepthLevelsUnits;

public:
    ModelView(Renderer &renderer, TextureLoader &textureLoader);
    ~ModelView();
    void draw(Uint32 time);
    bool isValidTile(int x, int y);
    void createPathTile(int x, int y);
    void createStructureTile(int x, int y);
    void setMapWidthHeight(int w, int h);
    void createPortalEntrada(int x, int y);
    void createPortalSalida(int x, int y);
    void createUnit(int id, int key,
                    int x, int y, int toX, int toY, Uint32 t);
    void createTower(int id, int key, int x, int y);
    void createSpell(int key, int x, int y, Uint32 t);
    void createShot(int key, int x, int y, int toX, int toY, Uint32 t);
    void moveUnit(int id, int x, int y, int toX, int toY, Uint32 t);
    void killUnit(int id);
};


#endif //TOWERDEFENSE_MODELVIEW_H