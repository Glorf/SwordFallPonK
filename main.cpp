#include "mapa.h"
#include "gui.h"
#include "config.h"
#include "base.h"
#include "engine.h"

int main()
{
    Base *base = new Base();
    Config *config = new Config();
    base->setConfig(config);
    config->loadConfig();

    int displaysizex=std::stoi(config->getConfigValues("resolution").at(0));
    int displaysizey=std::stoi(config->getConfigValues("resolution").at(1));

    int FPSRate=std::stoi(config->getConfigValues("fpsrate").at(0));

    Engine *engine = new Engine();
    sCoords displaysize; displaysize.x=displaysizex; displaysize.y=displaysizey;
    sCoords displaypos; displaypos.x=50; displaypos.y=50;
    bool fullscreen = std::stoi(config->getConfigValues("fullscreen").at(0));
    bool antialiasing = std::stoi(config->getConfigValues("antialiasing").at(0));
    engine->initDisplay(fullscreen, antialiasing, displaysize, displaypos);
    base->setEngine(engine);

    ALLEGRO_FONT *font = al_load_ttf_font("./res/ComicRelief.ttf", 0.013*displaysizex, 0);
    engine->addFont("default", "./res/ComicRelief.ttf");
    engine->addFont("arrows", "./res/Arrows.ttf", 2);
    engine->setDefaultFont("default");

    Map *map = new Map();

    GUI *gui = new GUI(displaysizex, displaysizey, font);

    std::pair<int, int> nullcord;
    nullcord.first = 0;
    nullcord.second = 0;

    base->setMap(map);
    base->setGUI(gui);
    base->setCoords(nullcord);

    ALLEGRO_MOUSE_CURSOR *cursor = al_create_mouse_cursor(al_load_bitmap("./res/sword.png"),0,0);
    al_set_mouse_cursor(engine->getDisplay(), cursor);

    int height;
    int width;

    std::pair<int,int> *xycoords = base->getCoords();

    std::pair<int*,int*> mapDimensions;
    mapDimensions = map->getMapaWH();

    engine->initEventSystem();

    ALLEGRO_EVENT_QUEUE *eventQueue = al_create_event_queue();
    al_register_event_source(eventQueue, al_get_display_event_source(engine->getDisplay()));

    bool mouseDown=false;
    bool mouseDown2=false;

    while(true)
    {
        if(Base::instance()->endGame)
            break;

        if(!al_is_event_queue_empty(eventQueue))
        {
            ALLEGRO_EVENT ev;
            al_wait_for_event(eventQueue, &ev);
            if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
                break;
        }

        engine->updateInput();

        if(al_mouse_button_down(engine->getMouse(), 1) && !mouseDown)
        {
            bool isGui = gui->isGui(engine->getMouse()->x, engine->getMouse()->y);

            if(engine->getMouse()->buttons == 1 && isGui)
            {
                gui->selectButton(engine->getMouse()->x, engine->getMouse()->y);
                mouseDown=true;
                continue;
            }
            else if(engine->getMouse()->buttons == 1 &&!gui->mapMovingBlocked())
            {
                map->selectRegion(engine->getMouse()->x, engine->getMouse()->y);
                mouseDown=true;
                continue;
            }
        }
        else if(al_mouse_button_down(engine->getMouse(), 2) && !mouseDown2)
        {
            bool isGui = gui->isGui(engine->getMouse()->x, engine->getMouse()->y);

            if(!gui->mapMovingBlocked()&&!isGui)
            {
                map->activateRegion(engine->getMouse()->x, engine->getMouse()->y);
                mouseDown2=true;
            }
        }
        else if(!al_mouse_button_down(engine->getMouse(), 1) && mouseDown)
        {
            mouseDown = false;
        }
        else if(!al_mouse_button_down(engine->getMouse(), 1) && mouseDown2)
        {
            mouseDown2 = false;
        }

        al_clear_to_color(al_map_rgb(255,255,255));

        height = *map->getMapaWH().second;
        width = *map->getMapaWH().first;

        if(!gui->mapMovingBlocked())
        {
            if(al_key_down(engine->getKeyboard(), ALLEGRO_KEY_LEFT) && (xycoords->first)<0)
                (xycoords->first)+=10;
            else if(al_key_down(engine->getKeyboard(), ALLEGRO_KEY_RIGHT) && (xycoords->first)>(-width+displaysizex+8)) //ALIGN
                (xycoords->first)-=10;
            else if(al_key_down(engine->getKeyboard(), ALLEGRO_KEY_UP) && (xycoords->second)<0)
                (xycoords->second)+=10;
            else if(al_key_down(engine->getKeyboard(), ALLEGRO_KEY_DOWN) && (xycoords->second)
                    >(-height+displaysizey+8-(0.1*displaysizey))) //DOWN GUI+ALIGN
                (xycoords->second)-=10;
        }

        map->redrawRegions();

        bool isGui = gui->isGui(engine->getMouse()->x, engine->getMouse()->y);

        if(!isGui&&!gui->mapMovingBlocked())
        {
            map->checkRegionDrawInfo(engine->getMouse()->x, engine->getMouse()->y);

            if(!al_mouse_button_down(engine->getMouse(), 1))
                mouseDown = false;
        }

        gui->redrawGui();

        if(std::stoi(config->getConfigValues("xyindicator").at(0))==1)
        {
            std::string messg = "X: ";
            messg.append(std::to_string(engine->getMouse()->x-(xycoords->first)));
            messg.append(" Y: ");
            messg.append(std::to_string(engine->getMouse()->y-(xycoords->second)));

            al_draw_text(font, al_map_rgb(5,9,0), 0,0,0,messg.c_str());
        }

        al_flip_display();
        al_rest(1.0/FPSRate);
    }

    al_destroy_bitmap(map->getMapaPtr());
    delete(map);
    delete(gui);
    delete(config);
    al_destroy_event_queue(eventQueue);
    delete (engine);
    delete(base);
}
