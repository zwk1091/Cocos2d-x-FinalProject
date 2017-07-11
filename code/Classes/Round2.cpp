#pragma execution_character_set("utf-8")
#include "Round2.h"
#include "Round3.h"
#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#define database UserDefault::getInstance()

USING_NS_CC;
using namespace CocosDenshion;

void Round2::setPhysicsWorld(PhysicsWorld* world) { m_world = world; }

Scene* Round2::createScene() {
	srand((unsigned)time(NULL));
	auto scene = Scene::createWithPhysics();

	scene->getPhysicsWorld()->setAutoStep(false);

	// Debug 模式
	// 开启debug模式需要setAutoStep(true) 并注释掉update函数第一行
	//scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);

	scene->getPhysicsWorld()->setGravity(Vec2(0, -300.0f));
	auto layer = Round2::create();
	layer->setPhysicsWorld(scene->getPhysicsWorld());
	scene->addChild(layer);
	return scene;
}

// on "init" you need to initialize your instance
bool Round2::init() {
	//////////////////////////////
	// 1. super init first
	if (!Layer::init()) {
		return false;
	}
	visibleSize = Director::getInstance()->getVisibleSize();

	preloadMusic(); // 预加载音效

	addSprite();    // 添加背景和各种精灵
	addListener();  // 添加监听器
	addPlayer();    // 添加玩家

					// 添加调度器
	schedule(schedule_selector(Round2::update), 0.01f, kRepeatForever, 0.1f);
	schedule(schedule_selector(Round2::initKnief), 2.0f, kRepeatForever, 0.1f);

	isRotate = false;
	doortime = 0;
	//字体
	TTFConfig ttfConfig;
	ttfConfig.fontFilePath = "fonts/STXINWEI.TTF";
	ttfConfig.fontSize = 36;
	scoreLabel = Label::createWithTTF(ttfConfig, "0");
	lifeLabel = Label::createWithTTF(ttfConfig, "3");
	score = 0;

	auto origin = Director::getInstance()->getVisibleOrigin();
	//添加分数和生命数显示
	scoreLabel->setPosition(Vec2(origin.x + 100,
		origin.y + visibleSize.height - scoreLabel->getContentSize().height));
	addChild(scoreLabel);

	life = 3;
	lifeLabel->setPosition(Vec2(origin.x + 250,
		origin.y + visibleSize.height - scoreLabel->getContentSize().height));
	addChild(lifeLabel);

	//添加边界
	Vec2* points = new Vec2[4];
	points[0] = Vec2(-visibleSize.width / 2, visibleSize.height * 0.6);
	points[1] = Vec2(visibleSize.width / 2, visibleSize.height * 0.6);
	points[2] = Vec2(visibleSize.width / 2, -visibleSize.height / 2);
	points[3] = Vec2(-visibleSize.width / 2, -visibleSize.height / 2);
	auto body = PhysicsBody::createEdgePolygon(points, 4, PhysicsMaterial(1.0f, 0.0f, 1.0f));
	edgeNode = Node::create();
	edgeNode->setPosition(visibleSize / 2);
	edgeNode->setPhysicsBody(body);
	this->addChild(edgeNode);
	return true;
}
//发射飞刀
void Round2::initKnief(float dt) {
	int posxk[4] = { 1200,100,1200,100 };
	int i = 0;
	for (auto it = kniefs.begin(); it != kniefs.end(); it++) {
		(*it)->setPosition(posxk[i], 120 * i + 250);
		if (i % 2 == 0) {
			(*it)->getPhysicsBody()->setVelocity(Vec2(-600, 0));
		}
		else {
			(*it)->getPhysicsBody()->setVelocity(Vec2(600, 0));
		}
		i++;
	}

}
// 预加载音效
void Round2::preloadMusic() {

	auto sae = SimpleAudioEngine::getInstance();
	sae->preloadEffect("explore.wav");
	sae->preloadEffect("jump.mp3");
	sae->preloadBackgroundMusic("backgroundmusic2.mp3");

	sae->playBackgroundMusic("backgroundmusic2.mp3", true);
}

// 添加背景和各种精灵
void Round2::addSprite() {
	// add background
	auto bgSprite = Sprite::create("bg4.png");
	bgSprite->setPosition(visibleSize / 2);
	bgSprite->setScale(visibleSize.width / bgSprite->getContentSize().width, visibleSize.height / bgSprite->getContentSize().height);
	this->addChild(bgSprite, 0);

	// add ship
	ship = Sprite::create("bottom.png");
	ship->setScale(visibleSize.width / ship->getContentSize().width * 0.97, 1.2f);
	ship->setPosition(visibleSize.width / 2, 20);
	auto shipbody = PhysicsBody::createBox(ship->getContentSize(), PhysicsMaterial(999.0f, 0.2f, 1.0f));
	shipbody->setCategoryBitmask(0xFFFFFFFF);
	shipbody->setCollisionBitmask(0xFFFFFFFF);
	shipbody->setContactTestBitmask(0xFFFFFFFF);
	shipbody->setDynamic(false);
	ship->setPhysicsBody(shipbody);

	this->addChild(ship, 1);

	//添加粒子特效
	ParticleFireworks* fireworks = ParticleFireworks::create();
	int xpos = visibleSize.width / 2;
	fireworks->setPosition(Vec2(xpos, 100));
	fireworks->setDuration(3);
	fireworks->setEmissionRate(500);
	this->addChild(fireworks);
	
	
	//add food
	int posxf[5] = { 650,750,150,750,80 };
	for (int i = 0; i < 5; i++) {
		auto foodSprite = Sprite::create("craberg2.png");
		foodSprite->setPosition(posxf[i], 120 * (i - 1) + 190);

		foodSprite->setTag(1);
		auto foodbody = PhysicsBody::createBox(foodSprite->getContentSize(), PhysicsMaterial(999.0f, 0.01f, 1.0f));
		foodbody->setCategoryBitmask(3);
		foodbody->setCollisionBitmask(0);
		foodbody->setContactTestBitmask(0xFFFFFFFF);
		foodbody->setDynamic(false);

		foodSprite->setPhysicsBody(foodbody);
		food.push_back(foodSprite);
		this->addChild(foodSprite);
	}

	//add bombs
	int posx[5] = { 750,200,600,400,700 };
	for (int i = 0; i < 5; i++) {
		auto bombSprite = Sprite::create("bomb1.png");
		bombSprite->setPosition(posx[i], 120 * (i - 1) + 200);
		//set tag
		bombSprite->setTag(2);

		auto bombbody = PhysicsBody::createBox(bombSprite->getContentSize(), PhysicsMaterial(999.0f, 0.01f, 1.0f));
		bombbody->setCategoryBitmask(6);
		bombbody->setCollisionBitmask(0);
		bombbody->setContactTestBitmask(0xFFFFFFFF);
		bombbody->setDynamic(false);

		bombSprite->setPhysicsBody(bombbody);
		bombs.push_back(bombSprite);
		this->addChild(bombSprite);
	}

	//add floor

	int randomnum[4] = { 4,1,4,2 };
	for (int j = 0; j < 4; j++) {

		for (int i = 0; i < 6; i++) {
			if (i != randomnum[j]) {
				auto floorSprite = Sprite::create("floor1.png");
				floorSprite->setPosition(i * 260, 120 * j + 170);

				auto floorbody = PhysicsBody::createBox(floorSprite->getContentSize(), PhysicsMaterial(999.0f, 0.001f, 1.0f));
				floorbody->setCategoryBitmask(0xFFFFFFFF);
				floorbody->setCollisionBitmask(0xFFFFFFFF);
				floorbody->setContactTestBitmask(0xFFFFFFFF);
				floorbody->setDynamic(false);

				floorSprite->setPhysicsBody(floorbody);
				this->addChild(floorSprite);
			}
		}
	}

	//添加飞刀
	int posxk[4] = { 1200,100,1200,100 };
	for (int i = 2; i < 6; i++) {
		auto kniefSprite = Sprite::create("knief1.png");
		if (i % 2 == 1) {
			kniefSprite->setFlipX(180);
		}
		kniefSprite->setPosition(posxk[i], 120 * (i - 1) + 250);
		kniefSprite->setTag(3);
		auto kniefbody = PhysicsBody::createBox(kniefSprite->getContentSize(), PhysicsMaterial(PhysicsMaterial(999.0f, 0.2f, 1.0f)));
		kniefbody->setCategoryBitmask(0xFFFFFFFF);
		kniefbody->setCollisionBitmask(0);
		kniefbody->setContactTestBitmask(0xFFFFFFFF);
		kniefbody->setGravityEnable(false);

		kniefSprite->setPhysicsBody(kniefbody);
		kniefs.push_back(kniefSprite);
		this->addChild(kniefSprite);
	}

	//添加心形，可以补血
	auto heartSprite = Sprite::create("heart.png");
	auto heartbody = PhysicsBody::createBox(heartSprite->getContentSize(), PhysicsMaterial(PhysicsMaterial(999.0f, 0.2f, 1.0f)));
	heartbody->setCategoryBitmask(0xFFFFFFFF);
	heartbody->setCollisionBitmask(0);
	heartbody->setContactTestBitmask(0xFFFFFFFF);
	heartbody->setDynamic(false);
	heartSprite->setPhysicsBody(heartbody);
	heartSprite->setPosition(70, 120 + 250);
	heartSprite->setTag(4);
	this->addChild(heartSprite);
}

// 添加监听器
void Round2::addListener() {
	auto keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(Round2::onKeyPressed, this);
	keyboardListener->onKeyReleased = CC_CALLBACK_2(Round2::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

	auto contactListener = EventListenerPhysicsContact::create();
	contactListener->onContactBegin = CC_CALLBACK_1(Round2::onConcactBegin, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);
}

//关于碰撞
bool Round2::onConcactBegin(PhysicsContact & contact) {
	auto shapeA = contact.getShapeA();
	auto shapeB = contact.getShapeB();
	auto spriteA = (Sprite*)contact.getShapeA()->getBody()->getNode();
	auto spriteB = (Sprite*)contact.getShapeB()->getBody()->getNode();
	int maskA = shapeA->getCollisionBitmask();
	int maskB = shapeB->getCollisionBitmask();
	int tagA = spriteA->getTag();
	int tagB = spriteB->getTag();
	//碰撞到food,
	if (tagA == 0 && tagB == 1) {
		score++;
		removeChild(spriteB);
	}
	else if (tagA == 1 && tagB == 0) {
		score++;
		removeChild(spriteA);
	}
	//碰撞到bomb
	if (tagA == 0 && tagB == 2) {
		life--;
		removeChild(spriteB);
		SimpleAudioEngine::getInstance()->playEffect("explore.wav", false);
	}
	else if (tagA == 2 && tagB == 0) {
		life--;
		removeChild(spriteA);
		SimpleAudioEngine::getInstance()->playEffect("explore.wav", false);
	}
	//碰撞到飞刀
	if ((tagA == 3 && tagB == 0) || (tagA == 0 && tagB == 3)) {
		life--;

		//SimpleAudioEngine::getInstance()->playEffect("explore.wav", false);
	}
	//碰撞到心形
	if (tagA == 0 && tagB == 4) {
		life++;
		removeChild(spriteB);
	}
	else if (tagA == 4 && tagB == 0) {
		life++;
		removeChild(spriteA);
	}
	return true;
}

// 创建角色
void Round2::addPlayer() {
	IsPlayer1Jump = false;
	IsPlayer1Left = false;
	IsPlayer1Right = false;

	LastPlayer1Press = 'D';

	//create player1
	auto texture1 = Director::getInstance()->getTextureCache()->addImage("patrick_idle2.png");
	frame1 = SpriteFrame::createWithTexture(texture1, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 25, 32)));
	player1 = Sprite::createWithSpriteFrame(frame1);
	int xpos = visibleSize.width / 2;
	player1->setPosition(Vec2(xpos, 100));
	player1->setTag(0);
	// 设置角色刚体属性
	// Todo
	auto physicsBody = PhysicsBody::createBox(Size(32.0f, 36.0f), PhysicsMaterial(0.2f, 0.001f, 0.0f));
	physicsBody->setDynamic(true);
	physicsBody->setCategoryBitmask(2);
	physicsBody->setCollisionBitmask(2);
	physicsBody->setContactTestBitmask(0xffffffff);
	physicsBody->setRotationEnable(false);
	player1->setPhysicsBody(physicsBody);
	player1->setName("player1");
	this->addChild(player1, 2);

	loadAnimation("player1"); // 加载人物动画

}

//加载玩家动画帧, 共8种 :)
void Round2::loadAnimation(string filepath) {

	Vector<SpriteFrame*> PlayerRunWithoutBox;
	auto patrickRun = Director::getInstance()->getTextureCache()->addImage("patrick_walk2.png");
	PlayerRunWithoutBox.reserve(6);
	for (int i = 0; i < 6; i++) {
		auto frame = SpriteFrame::createWithTexture(patrickRun, CC_RECT_PIXELS_TO_POINTS(Rect(25 * i, 0, 25, 32)));
		PlayerRunWithoutBox.pushBack(frame);
	}
	Animation* PlayerRunWithoutBoxAnimation = Animation::createWithSpriteFrames(PlayerRunWithoutBox, 0.1f);
	AnimationCache::getInstance()->addAnimation(PlayerRunWithoutBoxAnimation, "patrickRun");
	
	Vector<SpriteFrame*> PlayerIdleWithoutBox;

	Vector<SpriteFrame*> PlayerRunWithBox;
	Vector<SpriteFrame*> PlayerJumpWithoutBox;

	auto PlayerImage = Director::getInstance()->getTextureCache()->addImage(filepath + ".png");

	PlayerIdleWithoutBox.reserve(3);
	for (int i = 0; i < 3; i++) {
		auto frame = SpriteFrame::createWithTexture(PlayerImage, CC_RECT_PIXELS_TO_POINTS(Rect(25 * i, 5, 25, 32)));
		PlayerIdleWithoutBox.pushBack(frame);
	}
	Animation* PlayerIdelWithoutBoxAnimation = Animation::createWithSpriteFrames(PlayerIdleWithoutBox, 0.1f);
	AnimationCache::getInstance()->addAnimation(PlayerIdelWithoutBoxAnimation, filepath + "IdleWithoutBoxAnimation");

	PlayerJumpWithoutBox.pushBack(SpriteFrame::createWithTexture(PlayerImage, CC_RECT_PIXELS_TO_POINTS(Rect(64, 41, 32, 36))));
	if (filepath == "player1")
		PlayerJumpWithoutBox.pushBack(frame1);
	else
		PlayerJumpWithoutBox.pushBack(frame2);
	Animation* PlayerJumpWithoutBoxAnimation = Animation::createWithSpriteFrames(PlayerJumpWithoutBox, 1.0f);
	AnimationCache::getInstance()->addAnimation(PlayerJumpWithoutBoxAnimation, filepath + "JumpWithoutBoxAnimation");
	
}

void Round2::update(float dt) {
	this->getScene()->getPhysicsWorld()->step(1 / 100.0f);

	auto winsize = Director::getInstance()->getWinSize();
	auto position = player1->getPosition();
	int x = position.x;


	//succeed
	if (door && player1->getBoundingBox().intersectsRect(door->getBoundingBox())) {
		unschedule(schedule_selector(Round2::update));
		floatingfloors.clear();
		sweepingfloors.clear();
		//切换场景
		auto newScene = Round3::createScene();
		Director::getInstance()->replaceScene(TransitionFade::create(0.5,newScene,Color3B(0,255,255)));
	}

	//移动的时候播放动画
	if (IsPlayer1Left || IsPlayer1Right) {
		if (player1->getSpriteFrame() == frame1) {
			auto animation = RepeatForever::create(Animate::create(AnimationCache::getInstance()->getAnimation("patrickRun")));
			animation->setTag(11);
			player1->runAction(animation);
		}
	}
	//关于门
	doortime = (doortime + 1) % 2000;
	if (!doortime && score>2) 
	{
		//add door
		if (!door) {
			door = Sprite::create("door1.png");
			door->setAnchorPoint(Vec2(0, 0));
			door->setScaleX(60 / door->getContentSize().width);
			door->setScaleY(80 / door->getContentSize().height);
			this->addChild(door);
		}
		door->setPosition(Vec2(800, 530));
	}

	updateScore();
	updateLife();

	if (life == 0) {
		GameOver();
	}
}

//更新分数
void Round2::updateScore() {

	String* s = String::createWithFormat("socre: %d", score);
	this->scoreLabel->setString(s->getCString());

}

void Round2::updateLife() {
	String* s = String::createWithFormat("life: %d", life);
	this->lifeLabel->setString(s->getCString());
}
// 上左右 
void Round2::onKeyPressed(EventKeyboard::KeyCode code, Event* event) {
	Action* animation;
	float x = player1->getPositionX();
	auto winsize = Director::getInstance()->getWinSize();
	//float Vx = player1->getPhysicsBody()->getVelocity().x;
	switch (code) {
	case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		player1->stopAllActions();
		player1->setSpriteFrame(frame1);
		player1->getPhysicsBody()->setVelocity(Vec2(0, 0));
		if (isRotate == false) {
			player1->setFlipX(180);

			isRotate = true;
		}

		player1->getPhysicsBody()->setVelocity(Vec2(-200, 0));
		IsPlayer1Left = true;
		IsPlayer1Right = false;
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		player1->stopAllActions();
		player1->setSpriteFrame(frame1);
		player1->getPhysicsBody()->setVelocity(Vec2(0, 0));
		// 左右移动
		// Todo
		if (isRotate) {
			player1->setFlipX(0);

			isRotate = false;
		}
		player1->getPhysicsBody()->setVelocity(Vec2(200, 0));
		IsPlayer1Left = false;
		IsPlayer1Right = true;
		break;

	case cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW:
		// 跳
		// Todo
		IsPlayer1Jump = true;
		IsPlayer1Left = false;
		IsPlayer1Right = false;
		CCLOG("%.2f\n", player1->getPhysicsBody()->getVelocity().y);
		if (player1->getPhysicsBody()->getVelocity().y < 0.01 && player1->getPhysicsBody()->getVelocity().y> -0.01) {
			player1->stopAllActions();
			player1->getPhysicsBody()->applyImpulse(Vec2(0, 70000));
		}

		break;

	default:
		break;
	}
}


// 释放按键

void Round2::onKeyReleased(EventKeyboard::KeyCode code, Event* event) {
	switch (code) {
	case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		IsPlayer1Left = false;
		if (player1->getPhysicsBody()->getVelocity().x < 0) {
			player1->stopAllActions();
			player1->setSpriteFrame(frame1);
			player1->getPhysicsBody()->setVelocity(Vec2(0, 0));
		}
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		IsPlayer1Right = false;
		if (player1->getPhysicsBody()->getVelocity().x > 0) {
			player1->stopAllActions();
			player1->setSpriteFrame(frame1);
			player1->getPhysicsBody()->setVelocity(Vec2(0, 0));
		}


		break;
	}
	// 停止动画和运动
	// Todo


}


// 游戏结束
void Round2::GameOver() {

	_eventDispatcher->removeAllEventListeners();
	unschedule(schedule_selector(Round2::update));

	SimpleAudioEngine::getInstance()->stopBackgroundMusic("backgroundmusic2.mp3");

	player1->setVisible(false);

	auto label1 = Label::createWithTTF("YOU DIED", "fonts/STXINWEI.TTF", 60);
	label1->setColor(Color3B(255, 0, 0));
	label1->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	this->addChild(label1);

	auto label2 = Label::createWithTTF("reStart", "fonts/STXINWEI.TTF", 40);
	label2->setColor(Color3B(0, 0, 0));
	auto replayBtn = MenuItemLabel::create(label2, CC_CALLBACK_1(Round2::replayCallback, this));
	Menu* replay = Menu::create(replayBtn, NULL);
	replay->setPosition(visibleSize.width / 2 - 80, visibleSize.height / 2 - 100);
	this->addChild(replay);

	auto label3 = Label::createWithTTF("exit", "fonts/STXINWEI.TTF", 40);
	label3->setColor(Color3B(0, 0, 0));
	auto exitBtn = MenuItemLabel::create(label3, CC_CALLBACK_1(Round2::exitCallback, this));
	Menu* exit = Menu::create(exitBtn, NULL);
	exit->setPosition(visibleSize.width / 2 + 90, visibleSize.height / 2 - 100);
	this->addChild(exit);
}

// 继续或重玩按钮响应函数
void Round2::replayCallback(Ref * pSender) {
	Director::getInstance()->replaceScene(Round2::createScene());
}

// 退出
void Round2::exitCallback(Ref * pSender) {
	Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}
