#include "gui.h"

void GUI::questionPrompt(std::string question, std::vector<std::string> options, void (GUI::*caller)(std::string))
{
    Menu *questionPrompt = addMenu(38,6.5,(double)al_get_text_width(czcionka,question.c_str())/(double)dispx*100+10,
            (options.size()+1)*6.5,Y_LAYOUT,"questionPrompt");

    questionPrompt->fillWithLayouts(options.size()+1, 5, X_LAYOUT);
    questionPrompt->findLayoutByVector({1})->addLabel(question);
    movingBlocked=true;

    for(int i=0; i<options.size(); i++)
    {
        questionPrompt->findLayoutByVector({i+2})->addButton(options.at(i), caller, options.at(i));
    }
}

void GUI::removeQuestionPrompt()
{
    if(movingBlocked)
        movingBlocked = false;
    removeMenu("questionPrompt");
}

void GUI::showNotification(std::string notification)
{
    Menu *notificationMenu = addMenu(38,6.5,(double)al_get_text_width(czcionka,notification.c_str())/(double)dispx*100+10,13,Y_LAYOUT,"notification");

    notificationMenu->fillWithLayouts(2, 5, X_LAYOUT);
    notificationMenu->findLayoutByVector({1})->addLabel(notification);
    movingBlocked=true;

    notificationMenu->findLayoutByVector({2})->addButton("OK", &GUI::removeNotification, "");
}

void GUI::removeNotification(std::string arg)
{
    movingBlocked = false;
    removeMenu("notification");
}

void GUI::quantityQuestionPrompt(std::string question, double min, double max, double step, void (GUI::*caller)(std::string))
{
    Menu *quantityMenu = addMenu(38,6.5,(double)al_get_text_width(czcionka,question.c_str())/(double)dispx*100+10,13,Y_LAYOUT,"quantityQuestion");

    quantityMenu->fillWithLayouts(3, 5, X_LAYOUT);
    quantityMenu->findLayoutByVector({1})->addLabel(question);

    movingBlocked=true;

    quantityMenu->findLayoutByVector({2})->addSlider(min, max, step);
    quantityMenu->findLayoutByVector({3})->addButton("OK", caller, "something");
}

void GUI::removeQuantityQuestionPrompt()
{
    removeMenu("quantityQuestion");
    movingBlocked=false;
    quantityReturnValue = 0;
}

//DO PRZENIESIENIA!!!
void Menu::fillWithLayouts(int layoutsNumber, int marginSize, eLayout layoutsType)
{
    if(this->layout == X_LAYOUT)
    {
        for(int i=0;i<layoutsNumber;i++)
        {
            Layout layout;
            layout.height = this->height;
            layout.width = (this->width/layoutsNumber);
            layout.marginSize = marginSize;
            layout.layoutType = layoutsType;
            layout.relX = ((this->width/layoutsNumber)*i)+this->x;
            layout.relY = this->y;
            layout.parent = NULL;
            layout.parentMenu = this;
            layout.highlight = false;
            this->layouts.push_back(layout);
        }
    }
    else if(this->layout == Y_LAYOUT)
    {
        for(int i=0;i<layoutsNumber;i++)
        {
            Layout layout;
            layout.height = (this->height/layoutsNumber);
            layout.width = this->width;
            layout.marginSize = marginSize;
            layout.layoutType = layoutsType;
            layout.relX = this->x;
            layout.relY = ((this->height/layoutsNumber)*i)+this->y;
            layout.parent = NULL;
            layout.parentMenu = this;
            layout.highlight = false;
            this->layouts.push_back(layout);
        }
    }
}

void Layout::fillWithLayouts(int layoutsNumber, int marginSize, eLayout layoutsType)
{
    if(this->layoutType == X_LAYOUT)
    {
        for(int i=0;i<layoutsNumber;i++)
        {
            Layout layout;
            layout.height = this->height;
            layout.width = (this->width/layoutsNumber);
            layout.marginSize = marginSize;
            layout.layoutType = layoutsType;
            layout.relX = ((this->width/layoutsNumber)*i)+this->relX;
            layout.relY = this->relY;
            layout.parent = this;
            layout.parentMenu = this->parentMenu;
            layout.highlight = false;
            this->layouts.push_back(layout);
        }
    }
    else if(this->layoutType == Y_LAYOUT)
    {
        for(int i=0;i<layoutsNumber;i++)
        {
            Layout layout;
            layout.height = (this->height/layoutsNumber);
            layout.width = this->width;
            layout.marginSize = marginSize;
            layout.layoutType = layoutsType;
            layout.relX = this->relX;
            layout.relY = ((this->height/layoutsNumber)*i)+this->relY;
            layout.parent = this;
            layout.parentMenu = this->parentMenu;
            layout.highlight = false;
            this->layouts.push_back(layout);
        }
    }
}

void Layout::changeLabel(std::string label)
{
    this->labels.at(0).text = label;
}

void Layout::highlightElement()
{
    this->highlight = true;
}

void Layout::clearHighlight()
{
    this->highlight = false;
}

void Layout::addButton(std::string label, void (GUI::*function)(std::string), std::string functionArg, std::string font)
{
    Button przycisk;

    przycisk.function = function;
    przycisk.label = label;
    przycisk.font = font;

    przycisk.blocked = false;
    przycisk.functionArg = functionArg;

    this->buttons.push_back(przycisk);
}

void Layout::removeButton()
{
    this->buttons.clear();
}

void Layout::addImageButton(std::string imageDir, void (GUI::*function)(std::string), std::string functionArg)
{
    ImageButton przycisk;

    ALLEGRO_BITMAP *image = al_load_bitmap(imageDir.c_str());

    przycisk.function = function;
    przycisk.image = image;

    przycisk.blocked = false;
    przycisk.functionArg = functionArg;

    this->imageButtons.push_back(przycisk);

    this->parentMenu->imagesInMenuList.push_back(image);
}

void Layout::addLabel(std::string text, std::string font)
{
    Label label;
    label.text = text;
    label.font = font;

    this->labels.push_back(label);
}

void Layout::addImage(std::string imageDir, std::string nameId)
{
    ALLEGRO_BITMAP *imagePtr = al_load_bitmap(imageDir.c_str());
    Image image;
    image.image=imagePtr;
    image.nameId=nameId;

    this->images.push_back(image);
    this->parentMenu->imagesInMenuList.push_back(imagePtr);
}

void Layout::removeImage()
{
    std::list<ALLEGRO_BITMAP*>::iterator it;
    for(it=this->parentMenu->imagesInMenuList.begin();it!=this->parentMenu->imagesInMenuList.end();it++)
    {
        if(this->images.at(0).image == *it)
        {
            ALLEGRO_BITMAP *bmp = *it;
            this->parentMenu->imagesInMenuList.erase(it);
            al_destroy_bitmap(bmp);
            break;
        }
    }

    this->images.clear();
}

void Layout::addSlider(double min, double max, double step)
{
    this->fillWithLayouts(10, 0 , X_LAYOUT);
    std::vector<int> conn;
    for(int i=2; i<9; i++)
    {
        conn.push_back(i);
    }
    this->connectLayouts(conn);

    this->at(0)->addButton("b", &GUI::sliderLeftButton, this->parentMenu->nameId, "arrows");
    //this->at(0)->addImageButton("./res/arrowl.png", &GUI::sliderLeftButton, this->parentMenu->nameId);

    Slider slider;
    slider.max = max;
    slider.min = min;
    slider.step = step;
    slider.value = min;

    this->at(1)->sliders.push_back(slider);

    this->at(2)->addLabel("0");
    //this->at(3)->addImageButton("./res/arrowr.png", &GUI::sliderRightButton, this->parentMenu->nameId);
    this->at(3)->addButton("a", &GUI::sliderRightButton, this->parentMenu->nameId, "arrows");
}

void Layout::connectLayouts(std::vector<int> layoutsToConnect)
{
    int connectedHeight = this->at(layoutsToConnect.at(0)-1)->height;
    int connectedWidth = this->at(layoutsToConnect.at(0)-1)->width;

    for(unsigned is=1;is<layoutsToConnect.size();is++)
    {
        if(this->layoutType == X_LAYOUT)
        {
            connectedWidth += this->at(layoutsToConnect.at(is)-1)->width;
        }
        else if(this->layoutType == Y_LAYOUT)
        {
            connectedHeight += this->at(layoutsToConnect.at(is)-1)->height;
        }
    }

    for(unsigned iu=1;iu<layoutsToConnect.size();iu++)
    {
        std::list<Layout>::iterator it;
        it = this->layouts.begin();
        std::advance(it,layoutsToConnect.at(iu)-iu);
        this->layouts.erase(it);
    }

    Layout *targetLay = this->at(layoutsToConnect.at(0)-1);
    targetLay->height = connectedHeight;
    targetLay->width = connectedWidth;
}

void Layout::setButtonBlock(bool block)
{
    Button *button = &this->buttons.at(0);
    button->blocked = block;
}

Layout* Layout::at(int num)
{
    std::list<Layout>::iterator it;
    it = this->layouts.begin();
    std::advance(it, num);
    return &*it;
}
