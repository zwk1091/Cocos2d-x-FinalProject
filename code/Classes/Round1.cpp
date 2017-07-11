#pragma execution_character_set("utf-8")
#include "Round1.h"
#include "Round2.h"
#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#define database UserDefault::getInstance()

USING_NS_CC;
using namespace CocosDenshion;

void Round1::setPhysicsWorld(PhysicsWorld* world) { m_world = world; }

Scene* Round1::createScene() {
	srand((unsigned)time(NULL));
	auto scene = Scene::createWithPhysics();

	scene->getPhysicsWorld()->setAutoStep(false);

	// Debug 模式
	// 开启debug模式需要setAutoStep(true) 并注释掉update函数第一行
	//scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);

	scene->getPhysicsWorld()->setGravity(Vec2(0, -300.0f));
	auto layer = Round1::create();
	layer->setPhysicsWorld(scene->getPhysicsWorld());
	scene->addChild(layer);
	return scene;
}

// on "init" you need to initialize your instance
bool Round1::init() {
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
	schedule(schedule_selector(Round1::update), 0.01f, kRepeatForever, 0.1f);
	schedule(schedule_selector(Round1::bombFall), 4.0f, kRepeatForever, 0);

	isRotate = false;

	//start modified
	player1->setTag(2);

	count = 0;
	doortime = 0;
	isGrounded = true;
	score = 0;

	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 7; ++j) {
			if (floorType[i][j] == 1)
				staticFloors.push_back(7 * i + j);
		}
	}

	loadExplosion();

	TTFConfig ttfConfig;
	ttfConfig.fontFilePath = "fonts/STXINWEI.TTF";
	ttfConfig.fontSize = 40;
	//倒计时
	Score = Label::createWithTTF(ttfConfig, "180");
	Score->setAnchorPoint(Vec2(0, 0));
	Score->setPosition(Vec2(10, visibleSize.height - Score->getContentSize().height));
	this->addChild(Score);
	//end modified

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

// 预加载音效
void Round1::preloadMusic() {

	auto sae = SimpleAudioEngine::getInstance();
	sae->preloadEffect("gameover.mp3");
	sae->preloadEffect("jump.mp3");
	sae->preloadBackgroundMusic("backgroundmusic3.mp3");

	sae->playBackgroundMusic("backgroundmusic3.mp3", true);
}

// 添加背景和各种精灵
void Round1::addSprite() {
	// add background
	auto bgSprite = Sprite::create("bg3.png");
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



	ParticleFireworks* fireworks = ParticleFireworks::create();
	int xpos = visibleSize.width / 2;
	fireworks->setPosition(Vec2(xpos, 100));
	fireworks->setDuration(3);
	fireworks->setEmissionRate(500);
	this->addChild(fireworks);
	//add floor
	float w = visibleSize.width / 7;
	float h = 120, floorheight;
	for (int row = 0; row < 4; ++row) {
		for (int col = 0; col < 7; ++col) {
			if (floorType[row][col] != 0) {
				auto floorSprite = Sprite::create("floor3.png");
				floorheight = floorSprite->getContentSize().height;
				floorSprite->setAnchorPoint(Vec2(0, 0));
				floorSprite->setPosition(col * w, h * row + 170);
				floorSprite->setScaleX(w / floorSprite->getContentSize().width);

				auto floorbody = PhysicsBody::createBox(floorSprite->getContentSize(), PhysicsMaterial(0.001f, 0.2f, 5.0f));
				floorbody->setCategoryBitmask(0xFFFF0000);
				floorbody->setCollisionBitmask(0xFFFF0000);
				floorbody->setContactTestBitmask(0xFFFF0000);
				floorbody->setDynamic(false);

				floorSprite->setPhysicsBody(floorbody);
				this->addChild(floorSprite);
				floors.push_back(floorSprite);

				if (floorType[row][col] == 2) {
					floatingfloors.push_back(floorSprite);
				}
				else if (floorType[row][col] == 3) {
					sweepingfloors.push_back(floorSprite);
				}
				else {
					auto food = Sprite::create("craberg2.png");
					food->setScaleX(20 / food->getContentSize().width);
					food->setScaleY(20 / food->getContentSize().height);
					food->setTag(3);
					food->setAnchorPoint(Vec2(0, 0));
					food->setPosition(Vec2(row*w + 100, 170 + col*h + floorheight));
					auto foodBody = PhysicsBody::createBox(food->getContentSize(), PhysicsMaterial(999.0f, 0.0f, 1.0f));
					foodBody->setCategoryBitmask(0xFFFF0000);
					foodBody->setCollisionBitmask(0xFFFF0000);
					foodBody->setContactTestBitmask(0xFFFF0000);
					food->setPhysicsBody(foodBody);
					this->addChild(food);
				}
			}
		}
	}
	//end modified
}

// 添加监听器
void Round1::addListener() {
	auto keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(Round1::onKeyPressed, this);
	keyboardListener->onKeyReleased = CC_CALLBACK_2(Round1::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

	auto contactListener = EventListenerPhysicsContact::create();
	contactListener->onContactBegin = CC_CALLBACK_1(Round1::onConcactBegin, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);
}

// 创建角色
void Round1::addPlayer() {
	IsPlayer1Jump = false;
	IsPlayer1Left = false;
	IsPlayer1Right = false;

	LastPlayer1Press = 'D';

	//create player1
	auto texture1 = Director::getInstance()->getTextureCache()->addImage("patrick_idle1.png");
	frame1 = SpriteFrame::createWithTexture(texture1, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 41.67, 54)));
	player1 = Sprite::createWithSpriteFrame(frame1);
	int xpos = visibleSize.width / 2;
	player1->setPosition(Vec2(xpos, 120));

	// 设置角色刚体属性
	// Todo
	auto physicsBody = PhysicsBody::createBox(player1->getContentSize(), PhysicsMaterial(0.5f, 0.2f, 0.0f));
	physicsBody->setDynamic(true);
	physicsBody->setCategoryBitmask(0xFFFFFFFF);
	physicsBody->setCollisionBitmask(0xFFFFFFFF);
	physicsBody->setContactTestBitmask(0xFFFFFFFF);
	physicsBody->setRotationEnable(false);
	player1->setPhysicsBody(physicsBody);
	player1->setName("player1");
	this->addChild(player1, 2);

	loadAnimation("player1");
}

//加载玩家动画帧, 共8种 :)
void Round1::loadAnimation(string filepath) {
	//!!!
	Vector<SpriteFrame*> PlayerRunWithoutBox;
	auto patrickRun = Director::getInstance()->getTextureCache()->addImage("patrick_walk1.png");
	PlayerRunWithoutBox.reserve(6);
	for (int i = 0; i < 6; i++) {
		auto frame = SpriteFrame::createWithTexture(patrickRun, CC_RECT_PIXELS_TO_POINTS(Rect(41.67 * i, 0, 41.67, 54)));
		PlayerRunWithoutBox.pushBack(frame);
	}
	Animation* PlayerRunWithoutBoxAnimation = Animation::createWithSpriteFrames(PlayerRunWithoutBox, 0.1f);
	AnimationCache::getInstance()->addAnimation(PlayerRunWithoutBoxAnimation, "patrickRun");

	Vector<SpriteFrame*> angryBird;
	auto bird = Director::getInstance()->getTextureCache()->addImage("angrybird1.png");
	angryBird.reserve(5);
	for (int i = 0; i < 5; i++) {
		auto frame = SpriteFrame::createWithTexture(bird, CC_RECT_PIXELS_TO_POINTS(Rect(0, 79 * (4 - i), 111, 79)));
		angryBird.pushBack(frame);
	}
	Animation* angryBirdAnimation = Animation::createWithSpriteFrames(angryBird, 0.1f);
	AnimationCache::getInstance()->addAnimation(angryBirdAnimation, "angrybird");
	//!!!
	
	Vector<SpriteFrame*> PlayerIdleWithoutBox;
	Vector<SpriteFrame*> PlayerRunWithBox;
	Vector<SpriteFrame*> PlayerJumpWithoutBox;

	auto PlayerImage = Director::getInstance()->getTextureCache()->addImage(filepath + ".png");

	PlayerIdleWithoutBox.reserve(3);
	for (int i = 0; i < 3; i++) {
		auto frame = SpriteFrame::createWithTexture(PlayerImage, CC_RECT_PIXELS_TO_POINTS(Rect(32 * i, 5, 32, 36)));
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

void Round1::update(float dt) {
	this->getScene()->getPhysicsWorld()->step(1 / 100.0f);
	auto winsize = Director::getInstance()->getWinSize();
	auto position = player1->getPosition();
	int x = position.x;
	//移动
	if (player1->getBoundingBox().intersectsRect(ship->getBoundingBox()))
	{
		//isGrounded
		isGrounded = true;
		if (IsPlayer1Left&&x > 50) {
			player1->getPhysicsBody()->setVelocity(Vec2(-100, 0));
		}
		if (IsPlayer1Right&&x < winsize.width - 50) {
			player1->getPhysicsBody()->setVelocity(Vec2(100, 0));
		}
	}
	//succeed
	if (door && player1->getBoundingBox().intersectsRect(door->getBoundingBox())) {
		unschedule(schedule_selector(Round1::update));
		floatingfloors.clear();
		sweepingfloors.clear();
		//切换场景
		auto newScene = Round2::createScene();
		Director::getInstance()->replaceScene(TransitionFade::create(0.5, newScene, Color3B(0, 255, 255)));
	}

	if (IsPlayer1Left || IsPlayer1Right) {
		if (player1->getSpriteFrame() == frame1) {
			auto animation = RepeatForever::create(Animate::create(AnimationCache::getInstance()->getAnimation("patrickRun")));//player1RunWithoutBoxAnimation
			animation->setTag(11);
			player1->runAction(animation);
		}
	}
	//modified, floor摆动
	count = (count + 1) % 100;
	auto fitr = floatingfloors.begin();
	auto sitr = sweepingfloors.begin();
	int len = 3;
	if (count < 25) {//up, left
		for (; fitr != floatingfloors.end(); ++fitr) {
			(*fitr)->setPositionY((*fitr)->getPositionY() + len);
		}
		for (; sitr != sweepingfloors.end(); ++sitr) {
			(*sitr)->setPositionX((*sitr)->getPositionX() - len);
		}
	}
	else if (count < 75) {//down,right
		for (; fitr != floatingfloors.end(); ++fitr) {
			(*fitr)->setPositionY((*fitr)->getPositionY() - len);
		}
		for (; sitr != sweepingfloors.end(); ++sitr) {
			(*sitr)->setPositionX((*sitr)->getPositionX() + len);
		}
	}
	else {//up, left
		for (; fitr != floatingfloors.end(); ++fitr) {
			(*fitr)->setPositionY((*fitr)->getPositionY() + len);
		}
		for (; sitr != sweepingfloors.end(); ++sitr) {
			(*sitr)->setPositionX((*sitr)->getPositionX() - len);
		}
	}

	doortime = (doortime + 1) % 2000;
	if (!doortime && score>3) 
	{
		//add door
		if (!door) {
			door = Sprite::create("door1.png");
			door->setAnchorPoint(Vec2(0, 0));
			door->setScaleX(60 / door->getContentSize().width);
			door->setScaleY(80 / door->getContentSize().height);
			this->addChild(door);
		}

		auto tempfloor = Sprite::create("floor1.png");
		float floorheight = tempfloor->getContentSize().height;
		float w = visibleSize.width / 7;
		float h = 120;
		int randnum = rand() % staticFloors.size();
		int doorPosX = staticFloors[randnum] % 7;
		int doorPosY = staticFloors[randnum] / 7;
		door->setPosition(Vec2(doorPosX*w, 170 + doorPosY*h + floorheight));
		//door->setPosition(Vec2(500, 170));
	}

	auto i = floors.begin();
	for (; i != floors.end(); ++i) {
		if (player1->getBoundingBox().intersectsRect((*i)->getBoundingBox())) {
			isGrounded = true;
			break;
		}
	}
	//score
	char s[20];
	sprintf(s, "score: %d   life: 1", score);
	this->Score->setString(s);
	//end modified
}

// 上左右 
void Round1::onKeyPressed(EventKeyboard::KeyCode code, Event* event) {
	Action* animation;
	float x = player1->getPositionX();
	auto winsize = Director::getInstance()->getWinSize();
	//float Vx = player1->getPhysicsBody()->getVelocity().x;

	switch (code) {
	case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		if (isRotate == false) {
			player1->setFlipX(180);
			isRotate = true;
		}
		if (x>80) {
			player1->getPhysicsBody()->setVelocity(Vec2(-200, 0));
		}

		IsPlayer1Left = true;
		IsPlayer1Right = false;
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		// 左右移动
		// Todo
		if (isRotate) {
			player1->setFlipX(0);
			isRotate = false;
		}
		if (x<winsize.width - 80) {
			player1->getPhysicsBody()->setVelocity(Vec2(200, 0));
		}
		IsPlayer1Left = false;
		IsPlayer1Right = true;
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW:
		// 跳
		/*if (isGrounded) {
			IsPlayer1Jump = true;
			IsPlayer1Left = false;
			IsPlayer1Right = false;
			player1->getPhysicsBody()->setVelocity(Vec2(0, 800));
			isGrounded = false;
		}*/
		IsPlayer1Jump = true;
		IsPlayer1Left = false;
		IsPlayer1Right = false;
		//CCLOG("%.2f\n", player1->getPhysicsBody()->getVelocity().y);
		if (player1->getPhysicsBody()->getVelocity().y < 0.01 && player1->getPhysicsBody()->getVelocity().y> -0.01) {
			player1->stopAllActions();
			player1->getPhysicsBody()->applyImpulse(Vec2(0, 700000));
		}
		break;
	default:
		break;
	}
}

// 释放按键
void Round1::onKeyReleased(EventKeyboard::KeyCode code, Event* event) {
	switch (code) {
	case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		IsPlayer1Left = false;
		player1->stopAllActions();
		player1->setSpriteFrame(frame1);
		player1->getPhysicsBody()->setVelocity(Vec2(0, 0));
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:

		IsPlayer1Right = false;
		player1->stopAllActions();
		player1->setSpriteFrame(frame1);
		player1->getPhysicsBody()->setVelocity(Vec2(0, 0));
		break;
	}

}

// 游戏结束
void Round1::GameOver() {
	//modified
	unschedule(schedule_selector(Round1::update));
	unschedule(schedule_selector(Round1::bombFall));
	//end modified
	SimpleAudioEngine::getInstance()->stopBackgroundMusic("backgroundmusic3.mp3");

	auto label1 = Label::createWithTTF("YOU DIED", "fonts/STXINWEI.TTF", 60);
	label1->setColor(Color3B(255, 0, 0));
	label1->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	this->addChild(label1);

	auto label2 = Label::createWithTTF("reStart", "fonts/STXINWEI.TTF", 40);
	label2->setColor(Color3B(0, 0, 0));
	auto replayBtn = MenuItemLabel::create(label2, CC_CALLBACK_1(Round1::replayCallback, this));
	Menu* replay = Menu::create(replayBtn, NULL);
	replay->setPosition(visibleSize.width / 2 - 80, visibleSize.height / 2 - 100);
	this->addChild(replay);

	auto label3 = Label::createWithTTF("exit", "fonts/STXINWEI.TTF", 40);
	label3->setColor(Color3B(0, 0, 0));
	auto exitBtn = MenuItemLabel::create(label3, CC_CALLBACK_1(Round1::exitCallback, this));
	Menu* exit = Menu::create(exitBtn, NULL);
	exit->setPosition(visibleSize.width / 2 + 90, visibleSize.height / 2 - 100);
	this->addChild(exit);
}

// 继续或重玩按钮响应函数
void Round1::replayCallback(Ref * pSender) {
	Director::getInstance()->replaceScene(Round1::createScene());
}

// 退出
void Round1::exitCallback(Ref * pSender) {
	Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}

void Round1::bombFall(float dt) {
	auto bomb = Sprite::create("angrybird.png");
	bomb->setTag(1);
	bomb->setScaleX(50 / bomb->getContentSize().width);
	bomb->setScaleY(50 / bomb->getContentSize().height);
	bomb->setPosition(rand() % (int)(visibleSize.width - 200) + 100, visibleSize.height);
	auto animation = RepeatForever::create(Animate::create(AnimationCache::getInstance()->getAnimation("angrybird")));
	bomb->runAction(animation);

	int bombBitmask = 0x0000FFFF;
	auto bombBody = PhysicsBody::createBox(bomb->getContentSize(), PhysicsMaterial(100.0f, 0.0f, 1.0f));
	bombBody->setCategoryBitmask(bombBitmask);
	bombBody->setCollisionBitmask(0x00000000);
	bombBody->setContactTestBitmask(bombBitmask);
	bombBody->setDynamic(true);
	bomb->setPhysicsBody(bombBody);
	bombBody->setVelocity(Vec2(0, 0 - ship->getPositionY()));
	this->addChild(bomb);
}

bool Round1::onConcactBegin(PhysicsContact & contact) {
	auto nodeA = contact.getShapeA()->getBody()->getNode();
	auto nodeB = contact.getShapeB()->getBody()->getNode();
	auto animation = RepeatForever::create(Animate::create(AnimationCache::getInstance()->getAnimation("explosionAnimation")));
	if (nodeA && nodeB) {
		if (nodeA->getTag() == 1 || nodeB->getTag() == 1) {//bomb
			if (nodeA->getTag() == 2 || nodeB->getTag() == 2) {
				if (nodeA->getTag() == 1) {
					auto temp = nodeA;
					nodeA->runAction(
						//Sequence::create(
						//animation,
						CallFunc::create([temp, this] {
						temp->removeFromParentAndCleanup(true);
						GameOver();
					})
						//nullptr)
						);
				}
				else {
					auto temp = nodeB;
					nodeB->runAction(
						//Sequence::create(
						//animation,
						CallFunc::create([temp, this] {
						temp->removeFromParentAndCleanup(true);
						GameOver();
					})
						//nullptr)
						);
				}
			}
			else {
				if (nodeA->getTag() == 1) {
					auto temp = nodeA;
					nodeA->runAction(
						CallFunc::create([temp, this] {
						temp->removeFromParentAndCleanup(true);
					})
					);
				}
				else {
					auto temp = nodeB;
					nodeB->runAction(
						CallFunc::create([temp, this] {
						temp->removeFromParentAndCleanup(true);
					})
					);
				}
			}
		}
		if ((nodeA->getTag() == 3 && nodeB->getTag() == 2) ||
			(nodeB->getTag() == 3 && nodeA->getTag() == 2)) {//food:3, player:2
			if (nodeA->getTag() == 3) {
				nodeA->removeFromParentAndCleanup(true);
			}
			else {
				nodeB->removeFromParentAndCleanup(true);
			}
			++score;
		}
	}
	return true;
}

void Round1::createExplosion() {
	explosion.reserve(5);
	auto texture = Director::getInstance()->getTextureCache()->addImage("explosion1.png");
	for (int i = 0; i < 5; ++i) {
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(256 * i, 0, 256, 252)));
		explosion.pushBack(frame);
	}
}

void Round1::loadExplosion() {
	explosion.reserve(8);
	auto texture2 = Director::getInstance()->getTextureCache()->addImage("explosion.png");
	for (int i = 0; i < 5; ++i) {
		auto frame = SpriteFrame::createWithTexture(texture2, CC_RECT_PIXELS_TO_POINTS(Rect(188.8 * i, 0, 188.8, 212.5)));
		explosion.pushBack(frame);
	}
	for (int i = 0; i < 3; ++i) {
		auto frame = SpriteFrame::createWithTexture(texture2, CC_RECT_PIXELS_TO_POINTS(Rect(188.8 * i, 212.5, 188.8, 212.5)));
		explosion.pushBack(frame);
	}

	Animation* PlayerIdelWithoutBoxAnimation = Animation::createWithSpriteFrames(explosion, 0.1f);
	AnimationCache::getInstance()->addAnimation(PlayerIdelWithoutBoxAnimation, "explosionAnimation");
}