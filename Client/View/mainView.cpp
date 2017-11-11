#include "Common/Window.h"
#include "Common/Renderer.h"
#include "Common/TextureLoader.h"
#include "Model/MapView.h"
#include "Common/SpriteNamesConfig.h"
#include "Model/PortalEntradaView.h"
#include "Model/PortalSalidaView.h"
#include "Model/TowerView.h"
#include "Model/UnitView.h"
#include "Model/ShotView.h"
#include "Model/SpellView.h"

#define TITLE "Tower Defense"

#define WINDOWWIDTH 640
#define WINDOWHEIGHT 480
#define FPS 40

#define MAPSIZE 7

int main(int argc, char** argv) {
    bool quit = false;
    SDL_Event event{};
    SDL_Init(SDL_INIT_VIDEO);
    Window window(TITLE, WINDOWWIDTH, WINDOWHEIGHT);
    Renderer renderer(window, MAPSIZE, MAPSIZE);
    TextureLoader textureLoader(renderer.getRenderer(), 0);

    MapView mapView(MAPSIZE, MAPSIZE, DESIERTO, renderer, textureLoader);

    //Armo un camino
    mapView.addPathTile(0, 0);
    mapView.addPathTile(1, 0);
    mapView.addPathTile(1, 1);
    mapView.addPathTile(1, 2);
    mapView.addPathTile(1, 3);
    mapView.addPathTile(2, 3);
    mapView.addPathTile(2, 4);
    mapView.addPathTile(3, 4);
    mapView.addPathTile(3, 5);
    mapView.addPathTile(4, 5);
    mapView.addPathTile(5, 5);
    mapView.addPathTile(6, 5);
    mapView.addPathTile(6, 6);

    //Pongo tierra firme
    mapView.addStructureTile(1, 4);
    mapView.addStructureTile(2, 0);
    mapView.addStructureTile(2, 2);
    mapView.addStructureTile(4, 4);
    mapView.addStructureTile(4, 6);

    //Pongo un portal de entrada y uno de salida
    PortalEntradaView portalEntrada = PortalEntradaView(textureLoader, renderer);
    portalEntrada.setXY(0, 0);
    PortalSalidaView portalSalida = PortalSalidaView(textureLoader, renderer);
    portalSalida.setXY(6, 6);

    //Agrego una torre de aire en el 2,0
    TowerView tower1(1, TORRE_TIERRA, textureLoader, renderer);
    tower1.setXY(2, 0);

    //Agrego una torre de agua en el 4,4
    TowerView tower2(2, TORRE_AGUA, textureLoader, renderer);
    tower2.setXY(4, 4);

    //Agrego una torre de fuego en el 1,4
    TowerView tower3(3, TORRE_FUEGO, textureLoader, renderer);
    tower3.setXY(1, 4);

    //Agrego una torre de tierra en el 5,5
    TowerView tower4(4, TORRE_TIERRA, textureLoader, renderer);
    tower4.setXY(4, 6);

    //Agrego un disparo
    ShotView shot = ShotView(DISPARO_TIERRA, textureLoader, renderer);

    //Agrego un muro de fuego
    SpellView fireWall = SpellView(METEORITO, textureLoader, renderer);

    //Creo una unidad en el 0,0
    UnitView unit(1, ABOMINABLE, textureLoader, renderer);
    unit.move(0, 0, 1, 0, 3000);

    Uint32 t1;
    Uint32 t2;
    Uint32 s = 1000 / FPS;
    Uint32 delta = 0;
    Uint32 elapsedTime = 0;
    Uint32 delayTime = 0;

    while (!quit) {
        t1 = SDL_GetTicks();

        while(SDL_PollEvent(&event)) {

            switch (event.type) {
                case SDL_QUIT:
                    quit = true; break;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            quit = true; break;
                        case SDLK_i:
                            renderer.zoomIn(); break;
                        case SDLK_o:
                            renderer.zoomOut(); break;
                        case SDLK_d:
                            unit.enableDying(); break;
                        case SDLK_s:
                            shot.shoot(2, 0, 0, 0, 500); break;
                        case SDLK_a:
                            fireWall.cast(1, 1, 5000); break;
                        case SDLK_z:
                            unit.move(1, 0, 1, 1, 3000);; break;
                        case SDLK_x:
                            unit.move(1, 1, 0, 1, 3000);; break;
                        case SDLK_c:
                            unit.move(0, 1, 0, 0, 3000);; break;
                        case SDLK_v:
                            unit.move(0, 0, 1, 0, 3000);; break;
                        case SDLK_LEFT:
                            renderer.updateCamera(-1, 0); break;
                        case SDLK_RIGHT:
                            renderer.updateCamera(1, 0); break;
                        case SDLK_UP:
                            renderer.updateCamera(0, -1); break;
                        case SDLK_DOWN:
                            renderer.updateCamera(0, 1); break;
                    }
            }
        }
        renderer.clearRender();

        Uint32 ticks = SDL_GetTicks();
        mapView.draw(ticks);
        portalEntrada.draw(ticks);
        portalSalida.draw(ticks);

        //if ((ticks % 100) == 0)
            //unit.move(1, 0, 1, 1, 5000);
        unit.draw(ticks);
        tower1.draw(ticks);
        tower2.draw(ticks);
        tower3.draw(ticks);
        tower4.draw(ticks);
        fireWall.draw(ticks);
        shot.draw(ticks);

        renderer.present();

        t2 = SDL_GetTicks();
        elapsedTime = t2 - t1 + delta;
        delayTime = s - elapsedTime;
        if (elapsedTime < s) {
            SDL_Delay(delayTime);
            delta = SDL_GetTicks() - t2 - delayTime;
        } else
            delta = elapsedTime - s;
    }
    SDL_Quit();
}