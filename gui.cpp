#include "gui.h"
#include "base.h"

GUI::GUI(int displayx, int displayy, ALLEGRO_FONT *czcionkaPtr) :
    dispx(displayx),
    dispy(displayy),
    czcionka(czcionkaPtr),
    background(al_load_bitmap("./res/background_2000.jpeg")),
    restTime(0),
    movingBlocked(false)
{
    initMainMenu();
}

GUI::~GUI(){}

Menu* GUI::findMenuByName(std::string name)
{
    std::list<Menu>::iterator it;
    for(it=menus.begin();it!=menus.end();it++)
    {
        if(it->nameId == name)
        {
           return &*it;
        }
    }

    return nullptr;
}

void GUI::callRest(double secTime)
{
    al_rest(secTime);
    al_clear_to_color(al_map_rgb(255,255,255));
    Base::instance()->getMapPtr()->redrawRegions();
    redrawGui();
    al_flip_display();
}

void GUI::redrawGui()
{
    std::list<Menu>::iterator it;
    for(it = menus.begin();it!=menus.end();it++)
    {
        if(it->visible==false)
            continue;

        al_draw_scaled_bitmap(it->background, 0, 0, al_get_bitmap_width(it->background), al_get_bitmap_height(it->background),
                              it->x, it->y, it->width, it->height, 0);

        if(!it->isCustom)
            al_draw_rectangle(it->x,it->y,it->x+it->width,it->y+it->height,al_map_rgb(0,0,0), 2);

        std::list<std::list<Layout> * > layTPtr;
        layTPtr.push_back(&it->layouts);
        std::list<std::list<Layout> * > layTPtrNew;

        while(layTPtr.size()!=0)
        {
            std::list<std::list<Layout> * >::iterator it2;
            for(it2=layTPtr.begin();it2!=layTPtr.end();it2++)
            {
                std::list<Layout> *layoutPtr = *it2;

                std::list<Layout>::iterator it3;
                for(it3 = layoutPtr->begin(); it3!=layoutPtr->end();it3++)
                {
                    if(it3->buttons.size() != 0)
                    {
                        Button *przycisk = &it3->buttons.at(0);
                        Layout *layout = &*it3;

                        if(!it->isCustom)
                        {
                            ALLEGRO_COLOR color;
                            if(przycisk->blocked)
                                color=al_map_rgb(90,90,90);
                            else
                                color=it->fontColors;

                            al_draw_rectangle(layout->relX, layout->relY,layout->relX+layout->width,
                                            layout->relY+layout->height, al_map_rgb(0,0,0), 1);

                            Engine *engine = Base::instance()->getEngine();
                            sWH size = engine->getTextWH(przycisk->label, przycisk->font);
                            engine->writeText(przycisk->label, {layout->relX+(layout->width/2)-(size.width/2),
                                        layout->relY+(layout->height/2)-(size.height/2)},color, przycisk->font);
                        }
                        else if(it->isCustom)
                        {
                            ALLEGRO_COLOR color;
                            color=it->fontColors;

                            al_draw_text(czcionka, color, layout->relX+(layout->width/2)-
                                         (al_get_text_width(czcionka, przycisk->label.c_str())/2),
                                     layout->relY+(layout->height/2)-(al_get_font_line_height(czcionka)/2),
                                         0, przycisk->label.c_str());
                        }
                    }

                    else if(it3->images.size() != 0)
                    {
                        Image *img = &it3->images.at(0);
                        Layout *layout = &*it3;

                        al_draw_scaled_bitmap(img->image,0,0, al_get_bitmap_width(img->image), al_get_bitmap_height(img->image),
                                              layout->relX, layout->relY, layout->width, layout->height, 0);
                    }

                    else if(it3->imageButtons.size() != 0)
                    {
                        ImageButton *img = &it3->imageButtons.at(0);
                        Layout *layout = &*it3;

                        al_draw_scaled_bitmap(img->image,0,0, al_get_bitmap_width(img->image), al_get_bitmap_height(img->image),
                                              layout->relX, layout->relY, layout->width, layout->height, 0);
                    }

                    else if(it3->labels.size() != 0)
                    {
                        Label *label = &it3->labels.at(0);
                        Layout *layout = &*it3;

                        al_draw_text(czcionka, al_map_rgb(0,0,0), layout->relX+(layout->width/2)-
                                     (al_get_text_width(czcionka, label->text.c_str())/2),
                                     layout->relY+(layout->height/2)-(al_get_font_line_height(czcionka)/2),
                                     0, label->text.c_str());
                    }

                    else if(it3->sliders.size() != 0)
                    {
                        Slider *slider = &it3->sliders.at(0);
                        Layout *layout = &*it3;

                        int sliderPos = ((double)(layout->width-13)*
                                                      (slider->value/(slider->max-slider->min)));

                        al_draw_line(layout->relX+5, layout->relY+(layout->height/2), layout->relX+layout->width-5,
                                     layout->relY+(layout->height/2), al_map_rgb(0,0,0), 2);
                        al_draw_filled_rectangle(layout->relX+sliderPos+5, layout->relY+(layout->height/2)-5,
                                          layout->relX+sliderPos+8,layout->relY+(layout->height/2)+5, al_map_rgb(0,0,0));

                    }

                    else if(it3->layouts.size()!=0)
                    {
                        layTPtrNew.push_back(&it3->layouts);
                    }

                    if(it3->highlight)
                    {
                        Layout *lay = &*it3;
                        al_draw_rectangle(lay->relX, lay->relY, lay->relX+lay->width, lay->relY+lay->height, al_map_rgb(0,0,0), 2);
                    }
                }
            }

            layTPtr.clear();
            layTPtr = layTPtrNew;
            layTPtrNew.clear();
        }
    }
}

Menu* GUI::addMenu(float mx, float my, float width, float height, eLayout layout, std::string nameId)
{
    Menu menu;
    menu.height=height*dispy/100;
    menu.width=width*dispx/100;
    menu.x=mx*dispx/100;
    menu.y=my*dispy/100;
    menu.fontColors = al_map_rgb(0,0,0);
    menu.layout = layout;
    menu.nameId = nameId;
    menu.background = background;
    menu.isCustom = false;
    menu.visible = true;
    menu.disabled = false;

    menus.push_back(menu);

    return findMenuByName(nameId);
}

Menu* GUI::addCustomMenu(float mx, float my, float width, float height, eLayout layout, std::string nameId,
                        std::string bgDir, ALLEGRO_COLOR fontColors)
{
    Menu menu;
    menu.height=height*dispy/100;
    menu.width=width*dispx/100;
    menu.x=mx*dispx/100;
    menu.y=my*dispy/100;
    menu.fontColors = fontColors;
    menu.nameId = nameId;
    menu.layout = layout;
    menu.background = al_load_bitmap(bgDir.c_str());
    menu.isCustom = true;
    menu.visible = true;
    menu.disabled = false;

    menus.push_back(menu);

    return findMenuByName(nameId);
}

void GUI::removeMenu(std::string name)
{
    std::list<Menu>::iterator it;
    for(it=menus.begin();it!=menus.end();it++)
    {
        if(it->nameId==name)
        {
            Menu *menu = &*it;

            std::list<ALLEGRO_BITMAP*>::iterator it2;
            for(it2 = menu->imagesInMenuList.begin(); it2!=menu->imagesInMenuList.end();it2++)
            {
                ALLEGRO_BITMAP *image = *it2;
                al_destroy_bitmap(image);
            }

            if(menu->isCustom)
                al_destroy_bitmap(menu->background);

            menus.erase(it);
            return;
        }
    }
}

void GUI::selectButton(int x, int y)
{
    std::list<Menu>::reverse_iterator it;
    for(it=menus.rbegin();it!=menus.rend();it++)
    {
        if(!(x>=it->x && x<=it->x+it->width) &&
                (y>=it->y && y<=it->y+it->height))
        {
            continue;
        }

        if(it->disabled || !it->visible)
            return;

        Menu *last=&menus.back();

        if(last->nameId=="quantityQuestion" ||
           last->nameId=="notification" ||
           last->nameId=="questionPrompt")
        {
            if(it->nameId != last->nameId) //QUESTIONS SHOULD BE PASSED FIRST!
                return;
        }

        std::list<std::list<Layout> * > layTPtr;
        layTPtr.push_back(&it->layouts);
        std::list<std::list<Layout> * > layTPtrNew;

        while(layTPtr.size()!=0)
        {
            std::list<std::list<Layout> * >::iterator it2;
            for(it2=layTPtr.begin();it2!=layTPtr.end();it2++)
            {
                std::list<Layout> *layoutsPtr = *it2;

                std::list<Layout>::iterator it3;
                for(it3 = layoutsPtr->begin(); it3!=layoutsPtr->end(); it3++)
                {
                    Layout *layout = &*it3;

                    if((layout->buttons.size() != 0) && (x>=layout->relX && x<=layout->relX+layout->width) &&
                            (y>=layout->relY && y<=layout->relY+layout->height))
                    {

                        if(!it3->buttons.at(0).blocked)
                        {
                            Button *przycisk = &it3->buttons.at(0);

                            przycisk->activate();
                            return;
                        }

                    }

                    else if((layout->imageButtons.size() != 0) && (x>=layout->relX && x<=layout->relX+layout->width) &&
                            (y>=layout->relY && y<=layout->relY+layout->height))
                    {

                        if(!it3->imageButtons.at(0).blocked)
                        {
                            ImageButton *przycisk = &it3->imageButtons.at(0);

                            przycisk->activate();
                            return;
                        }

                    }

                    else if((layout->sliders.size() !=0) && (x>=layout->relX && x<=layout->relX+layout->width) &&
                            (y>=layout->relY && y<=layout->relY+layout->height))
                    {
                        double position = ((double)x-(double)layout->relX)/(double)layout->width;
                        Slider *slider = &layout->sliders.at(0);

                        double quantity = (slider->max - slider->min)/slider->step;
                        double calculated = (quantity*position);
                        double final = slider->min+(calculated*slider->step);

                        for(double i=slider->min;i<=slider->max;i+=slider->step)
                        {
                            if(final==i)
                            {
                                slider->value = i;
                                break;
                            }
                            else if(final>i && final < (i+(slider->step/2)))
                            {
                                slider->value = i;
                                break;
                            }
                            else if(final<i && final >= (i-(slider->step/2)))
                            {
                                slider->value = i;
                                break;
                            }
                        }

                        Layout *labelLayout = layout->parent->at(2);
                        std::string str = std::to_string(slider->value);
                        str.erase(str.find_last_not_of('0') + 1, std::string::npos);
                        str.erase(str.find_last_not_of('.') + 1, std::string::npos);
                        labelLayout->labels.at(0).text = str;

                        quantityReturnValue = slider->value;

                    }

                    else if((it3->layouts.size()!=0) && (x>=layout->relX && x<=layout->relX+layout->width) &&
                            (y>=layout->relY && y<=layout->relY+layout->height))
                    {
                        layTPtrNew.push_back(&it3->layouts);
                    }
                }
            }

            layTPtr = layTPtrNew;
            layTPtrNew.clear();
        }
    }
}

bool GUI::isGui(int x, int y)
{
    std::list<Menu>::iterator it;
    for(it=menus.begin();it!=menus.end();it++)
    {
        if(it->visible==false)
            continue;

        if((x>=it->x && x<=it->x+it->width) &&
                (y>=it->y && y<=it->y+it->height))
        {
           return true;
        }
    }

    return false;
}

void GUI::disableAllMenusExcept(std::string menu)
{
    std::list<Menu>::iterator it;
    for(it=menus.begin();it!=menus.end();it++)
    {
        if(it->nameId == menu)
            it->disabled=false;
        else
            it->disabled=true;
    }
}

void GUI::enableAllMenus()
{
    std::list<Menu>::iterator it;
    for(it=menus.begin();it!=menus.end();it++)
    {
        it->disabled=false;
    }
}

bool GUI::mapMovingBlocked()
{
    if(movingBlocked)
    {
        return true;
    }

    std::list<Menu>::iterator it;
    for(it=menus.begin();it!=menus.end();it++)
    {
        if((it->nameId!="upMenu") &&
           (it->nameId!="upMenuBar") &&
           (it->nameId!="dolnyPasek"))
        {
            return true;
        }
    }

    return false;
}

//DO PRZENIESIENIA!!!
void ButtonBase::activate()
{
    GUI *guiPtr = Base::instance()->getGuiPtr();

    if(function!=NULL)
        (guiPtr->*function)(functionArg);
}

Layout *Menu::findLayoutByVector(std::vector<int> v)
{
    Layout *layoutPtr = this->at(v.at(0)-1);
    for(unsigned ir=1;ir<v.size();ir++)
    {
        layoutPtr = layoutPtr->at(v.at(ir)-1);
    }

    return layoutPtr;
}

Layout* Menu::at(int num)
{
    std::list<Layout>::iterator it;
    it = this->layouts.begin();
    std::advance(it, num);
    return &*it;
}
