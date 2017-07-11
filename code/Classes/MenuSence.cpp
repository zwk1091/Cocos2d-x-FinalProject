#include "MenuSence.h"
#include "Round1.h"
USING_NS_CC;

Scene* MenuSence::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();

    // 'layer' is an autorelease object
    auto layer = MenuSence::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool MenuSence::init()
{

    if ( !Layer::init() )
    {
        return false;
    }

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();


	auto bg = Sprite::create("menu_background.jpg");
	bg->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y - 60));
	this->addChild(bg, 0);



	auto startItem = MenuItemImage::create("start-0.png",
		"start-1.png",
		CC_CALLBACK_1(MenuSence::startGameSence, this)
	);
	startItem->setPosition(Vec2(visibleSize.width + origin.x - 150, origin.y + 150));
	auto menu = Menu::create(startItem, NULL);
	menu->setPosition(Vec2::ZERO);
	addChild(menu, 1);


    return true;
}
void MenuSence::startGameSence(Ref* pSender) {
	auto start = Round1::createScene();
	
	Director::getInstance()->replaceScene(start);
}


