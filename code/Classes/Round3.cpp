#pragma execution_character_set("utf-8")
#include "Round3.h"
#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#define database UserDefault::getInstance()
USING_NS_CC;
using namespace CocosDenshion;
void Round3::setPhysicsWorld(PhysicsWorld* world) { m_world = world; }

int Round3::which = 4;
bool Round3::IsFoodEaten = false;
bool Round3::IsTeaDrunk = false;
Scene* Round3::createScene() {
	srand((unsigned)time(NULL));
	auto scene = Scene::createWithPhysics();
	scene->getPhysicsWorld()->setAutoStep(true);
	// Debug 模式
	// 开启debug模式需要setAutoStep(true) 并注释掉update函数第一行
	//scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
	scene->getPhysicsWorld()->setGravity(Vec2(0, -1000.0f));
	auto layer = Round3::create();
	layer->setPhysicsWorld(scene->getPhysicsWorld());
	scene->addChild(layer);
	return scene;
}

// on "init" you need to initialize your instance
bool Round3::init() {
	//////////////////////////////
	// 1. super init first
	if (!Layer::init()) {
		return false;
	}
	visibleSize = Director::getInstance()->getVisibleSize();
	xOffset = 6;
	yOffset = 0;
	IsMoveEnable = true;
	preloadMusic(); // 预加载音效
	addSprite();    // 添加背景和各种精灵
	addListener();  // 添加监听器
	addPlayer();    // 添加玩家
	addFloor();		// 添加场景
	schedule(schedule_selector(Round3::update), 0.01f, kRepeatForever, 0.1f);
	isRotate = false;
	return true;
}

// 预加载音效
void Round3::preloadMusic() {
	auto sae = SimpleAudioEngine::getInstance();
	sae->preloadEffect("gameover.mp3");
	sae->preloadEffect("jump.mp3");
	sae->preloadBackgroundMusic("backgroundmusic3.mp3");
	sae->playBackgroundMusic("backgroundmusic3.mp3", true);
}
// 添加背景和各种精灵
void Round3::addSprite() {
	// 添加背景
	auto bgSprite = Sprite::create("bg.png");
	bgSprite->setPosition(visibleSize / 2);
	bgSprite->setScale(visibleSize.width / bgSprite->getContentSize().width, visibleSize.height / bgSprite->getContentSize().height);
	this->addChild(bgSprite, 0);
	auto label1 = Label::createWithTTF("下午茶", "fonts/STXINWEI.TTF", 30);
	label1->setColor(Color3B(255, 0, 0));
	label1->setPosition(60, visibleSize.height - 40);
	this->addChild(label1, 5);
	// Tea
	if (!IsTeaDrunk) {
		tea = Label::createWithTTF("咖啡: 0", "fonts/STXINWEI.TTF", 30);
		tea->setColor(Color3B(0, 0, 0));
		tea->setPosition(60, visibleSize.height - 100);
		this->addChild(tea, 5);
	}
	else {
		tea = Label::createWithTTF("咖啡: 1", "fonts/STXINWEI.TTF", 30);
		tea->setColor(Color3B(0, 0, 0));
		tea->setPosition(60, visibleSize.height - 100);
		this->addChild(tea, 5);
	}
	// Food
	if (!IsFoodEaten) {
		food = Label::createWithTTF("汉堡: 0", "fonts/STXINWEI.TTF", 30);
		food->setColor(Color3B(0, 0, 0));
		food->setPosition(60, visibleSize.height - 160);
		this->addChild(food, 5);
	}
	else {
		food = Label::createWithTTF("汉堡: 1", "fonts/STXINWEI.TTF", 30);
		food->setColor(Color3B(0, 0, 0));
		food->setPosition(60, visibleSize.height - 160);
		this->addChild(food, 5);
	}
	// 操作提示
	auto label2 = Label::createWithTTF("操作提示", "fonts/STXINWEI.TTF", 30);
	label2->setColor(Color3B(255, 0, 0));
	label2->setPosition(60, visibleSize.height - 220);
	this->addChild(label2, 5);
	auto label3 = Label::createWithTTF("小跳: Space", "fonts/STXINWEI.TTF", 30);
	label3->setColor(Color3B(0, 0, 0));
	label3->setPosition(75, visibleSize.height - 280);
	this->addChild(label3, 5);
	auto label4 = Label::createWithTTF("大跳: ↑", "fonts/STXINWEI.TTF", 30);
	label4->setColor(Color3B(0, 0, 0));
	label4->setPosition(60, visibleSize.height - 340);
	this->addChild(label4, 5);
	auto label5 = Label::createWithTTF("左移: ←", "fonts/STXINWEI.TTF", 30);
	label5->setColor(Color3B(0, 0, 0));
	label5->setPosition(60, visibleSize.height - 400);
	this->addChild(label5, 5);
	auto label6 = Label::createWithTTF("右移: →", "fonts/STXINWEI.TTF", 30);
	label6->setColor(Color3B(0, 0, 0));
	label6->setPosition(60, visibleSize.height - 460);
	this->addChild(label6, 5);
	// 游戏目标
	auto label7 = Label::createWithTTF("目标", "fonts/STXINWEI.TTF", 30);
	label7->setColor(Color3B(255, 0, 0));
	label7->setPosition(60, visibleSize.height - 520);
	this->addChild(label7, 5);
	auto label8 = Label::createWithTTF("汉堡 X 1", "fonts/STXINWEI.TTF", 30);
	label8->setColor(Color3B(0, 0, 0));
	label8->setPosition(60, visibleSize.height - 560);
	this->addChild(label8, 5);
	auto label9 = Label::createWithTTF("咖啡 X 1", "fonts/STXINWEI.TTF", 30);
	label9->setColor(Color3B(0, 0, 0));
	label9->setPosition(60, visibleSize.height - 600);
	this->addChild(label9, 5);
	/*
	//添加粒子特效
	ParticleFireworks* fireworks = ParticleFireworks::create();
	int xpos = visibleSize.width / 2;
	fireworks->setPosition(Vec2(xpos, 100));
	fireworks->setDuration(3);
	fireworks->setEmissionRate(500);
	this->addChild(fireworks);
	*/
}

void Round3::addTrap(int x, int y, bool flag, int xx, int yy, int tag) {
	auto yLen = visibleSize.height / 19;
	auto xLen = visibleSize.width / 25 / 1.4;
	auto tri = Sprite::create("triangle.png");
	tri->setTag(tag);
	tri->setFlipY(flag);
	tri->setScale(0.6);
	tri->setAnchorPoint(Vec2(0, 0));
	auto triBody = PhysicsBody::create();
	if (flag) {
		Vec2 verts[] = { Vec2(0, 22), Vec2(24, -20), Vec2(-24, -20) };
		triBody->addShape(PhysicsShapePolygon::create(verts, 3));
		tri->setPhysicsBody(triBody);
		triBody->setDynamic(false);
		triBody->setCategoryBitmask(0xFFFFFFFF);
		triBody->setCollisionBitmask(0xFFFFFFFF);
		triBody->setContactTestBitmask(0xFFFFFFFF);
		tri->setPosition((x + xOffset) * xLen + xx, (y + yOffset) * yLen - 2 + yy);
		this->addChild(tri);
	}
	else {
		Vec2 verts[] = { Vec2(24, 20), Vec2(0, -22), Vec2(-24, 20) };
		triBody->addShape(PhysicsShapePolygon::create(verts, 3));
		tri->setPhysicsBody(triBody);
		triBody->setDynamic(false);
		triBody->setCategoryBitmask(0xFFFFFFFF);
		triBody->setCollisionBitmask(0xFFFFFFFF);
		triBody->setContactTestBitmask(0xFFFFFFFF);
		tri->setPosition((x + xOffset) * xLen + 1 + xx, (y + yOffset) * yLen + 5 + yy);
		this->addChild(tri);
	}
}

void Round3::addFood(int x, int y, int type, int xx, int yy, int tag) {
	auto yLen = visibleSize.height / 19;
	auto xLen = visibleSize.width / 25 / 1.4;
	Sprite* h;
	if (type == 0) {
		h = Sprite::create("food.png");
		tag = 201;
	}
	else if (type == 1) {
		h = Sprite::create("coffee.png");
		tag = 202;
	}
	else if (type == 2) {
		h = Sprite::create("exit.png");
		tag = 203;
	}
	h->setPosition((x + xOffset) * xLen, (y + yOffset) * yLen);
	h->setScale(0.65);
	h->setAnchorPoint(Vec2(0, 0));
	h->setTag(tag);
	auto hBody = PhysicsBody::createBox(h->getContentSize(), PhysicsMaterial(1.0f, 0.0f, 0.0f));
	hBody->setCategoryBitmask(0xFFFFFFFF);
	hBody->setCollisionBitmask(0x000000000);
	hBody->setContactTestBitmask(0xFFFFFFFF);
	hBody->setDynamic(false);
	h->setPhysicsBody(hBody);
	this->addChild(h);
}
void Round3::addHorizontal(int x, int y) {
	auto yLen = visibleSize.height / 19;
	auto xLen = visibleSize.width / 25 / 1.4;
	auto h = Sprite::create("horizontal.png");
	h->setPosition((x + xOffset) * xLen, (y + yOffset) * yLen);
	h->setScale(0.65);
	h->setAnchorPoint(Vec2(0, 0));
	auto hBody = PhysicsBody::createBox(h->getContentSize(), PhysicsMaterial(1.0f, 0.0f, 0.0f));
	hBody->setCategoryBitmask(0xFFFFFFFF);
	hBody->setCollisionBitmask(0xFFFFFFFF);
	hBody->setContactTestBitmask(0xFFFFFFFF);
	hBody->setDynamic(false);
	h->setPhysicsBody(hBody);
	this->addChild(h);
}

void Round3::addVertical(int x, int y) {
	auto yLen = visibleSize.height / 19;
	auto xLen = visibleSize.width / 25 / 1.4;
	auto h = Sprite::create("vertical.png");
	h->setPosition((x + xOffset) * xLen, (y + yOffset) * yLen);
	h->setScale(0.65);
	h->setAnchorPoint(Vec2(0, 0));
	auto hBody = PhysicsBody::createBox(h->getContentSize(), PhysicsMaterial(1.0f, 0.0f, 0.0f));
	hBody->setCategoryBitmask(0xFFFFFFFF);
	hBody->setCollisionBitmask(0xFFFFFFFF);
	hBody->setContactTestBitmask(0xFFFFFFFF);
	hBody->setDynamic(false);
	h->setPhysicsBody(hBody);
	this->addChild(h);
}

void Round3::addBlock(int x, int y) {
	auto yLen = visibleSize.height / 19;
	auto xLen = visibleSize.width / 25 / 1.4;
	auto h = Sprite::create("block.png");
	h->setPosition((x + xOffset) * xLen, (y + yOffset) * yLen);
	h->setScale(0.65);
	h->setAnchorPoint(Vec2(0, 0));
	auto hBody = PhysicsBody::createBox(h->getContentSize(), PhysicsMaterial(1.0f, 0.0f, 0.0f));
	hBody->setCategoryBitmask(0xFFFFFFFF);
	hBody->setCollisionBitmask(0xFFFFFFFF);
	hBody->setContactTestBitmask(0xFFFFFFFF);
	hBody->setDynamic(false);
	h->setPhysicsBody(hBody);
	this->addChild(h);
}

void Round3::addCheckpoint(int x, int y, int which) {
	auto yLen = visibleSize.height / 19;
	auto xLen = visibleSize.width / 25 / 1.4;
	auto h = Sprite::create("checkpoint.png");
	h->setPosition((x + xOffset) * xLen, (y + yOffset) * yLen);
	h->setScale(0.65);
	h->setAnchorPoint(Vec2(0, 0));
	h->setTag(which);
	auto hBody = PhysicsBody::createBox(h->getContentSize(), PhysicsMaterial(1.0f, 0.0f, 0.0f));
	hBody->setCategoryBitmask(0xFFFFFFFF);
	hBody->setCollisionBitmask(0x000000000);
	hBody->setContactTestBitmask(0xFFFFFFFF);
	hBody->setDynamic(false);
	h->setPhysicsBody(hBody);
	this->addChild(h);
}
void Round3::addTransparent(int x, int y, int xx, int yy, int tag) {
	auto yLen = visibleSize.height / 19;
	auto xLen = visibleSize.width / 25 / 1.4;
	auto h = Sprite::create("transparent.png");
	h->setPosition((x + xOffset) * xLen, (y + yOffset) * yLen);
	h->setScale(0.65);
	h->setAnchorPoint(Vec2(0, 0));
	h->setTag(tag);
	auto hBody = PhysicsBody::createBox(h->getContentSize(), PhysicsMaterial(1.0f, 0.0f, 0.0f));
	hBody->setCategoryBitmask(0xFFFFFFFF);
	hBody->setCollisionBitmask(0x000000000);
	hBody->setContactTestBitmask(0xFFFFFFFF);
	hBody->setDynamic(false);
	h->setPhysicsBody(hBody);
	this->addChild(h);
}
// 添加监听器
void Round3::addListener() {
	auto keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(Round3::onKeyPressed, this);
	keyboardListener->onKeyReleased = CC_CALLBACK_2(Round3::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);
	auto contactListener = EventListenerPhysicsContact::create();
	contactListener->onContactBegin = CC_CALLBACK_1(Round3::onContactBegin, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);
}

bool Round3::onContactBegin(PhysicsContact & contact) {
	auto nodeA = contact.getShapeA()->getBody()->getNode();
	auto nodeB = contact.getShapeB()->getBody()->getNode();
	if (nodeA != nullptr && nodeB != nullptr) {
		if (nodeA->getTag() == 10 && nodeB->getTag() == 20) {
			nodeB->removeFromParentAndCleanup(true);
			GameOver();
		}
		else if (nodeA->getTag() == 20 && nodeB->getTag() == 10) {
			nodeA->removeFromParentAndCleanup(true);
			GameOver();
		}
		// died
		else if (nodeA->getTag() == 50 && nodeB->getTag() == 20) {
			nodeB->removeFromParentAndCleanup(true);
			GameOver();
		}
		else if (nodeA->getTag() == 20 && nodeB->getTag() == 50) {
			nodeA->removeFromParentAndCleanup(true);
			GameOver();
		}
		// drop
		else if (nodeA->getTag() == 1024 && nodeB->getTag() == 20) {
			auto a = getChildByTag(512);
			a->runAction(MoveBy::create(1.0f, Vec3(0, -500, 0)));
		}
		else if (nodeA->getTag() == 20 && nodeB->getTag() == 1024) {
			auto a = getChildByTag(512);
			a->runAction(MoveBy::create(1.0f, Vec3(0, -500, 0)));
		}
		// died
		else if (nodeA->getTag() == 512 && nodeB->getTag() == 20) {
			nodeB->removeFromParentAndCleanup(true);
			GameOver();
		}
		else if (nodeA->getTag() == 20 && nodeB->getTag() == 512) {
			nodeA->removeFromParentAndCleanup(true);
			GameOver();
		}
		// sweep of 60
		else if (nodeA->getTag() == 20 && nodeB->getTag() == 110) {
			auto a = getChildByTag(60);
			a->runAction(MoveBy::create(1.5f, Vec3(-1000, 0, 0)));
		}
		else if (nodeA->getTag() == 110 && nodeB->getTag() == 20) {
			auto a = getChildByTag(60);
			a->runAction(MoveBy::create(1.5f, Vec3(-1000, 0, 0)));
		}
		// sweep to died
		else if (nodeA->getTag() == 20 && nodeB->getTag() == 60) {
			nodeA->removeFromParentAndCleanup(true);
			GameOver();
		}
		else if (nodeA->getTag() == 60 && nodeB->getTag() == 20) {
			nodeB->removeFromParentAndCleanup(true);
			GameOver();
		}
		// sweep of 70
		else if (nodeA->getTag() == 20 && nodeB->getTag() == 120) {
			auto a = getChildByTag(70);
			a->runAction(MoveBy::create(1.5f, Vec3(-1000, 0, 0)));
		}
		else if (nodeA->getTag() == 120 && nodeB->getTag() == 20) {
			auto a = getChildByTag(70);
			a->runAction(MoveBy::create(1.5f, Vec3(-1000, 0, 0)));
		}
		// sweep to died
		else if (nodeA->getTag() == 20 && nodeB->getTag() == 70) {
			nodeA->removeFromParentAndCleanup(true);
			GameOver();
		}
		else if (nodeA->getTag() == 70 && nodeB->getTag() == 20) {
			nodeB->removeFromParentAndCleanup(true);
			GameOver();
		}
		// trigger falling trap
		else if (nodeA->getTag() == 20 && nodeB->getTag() == 40) {
			auto a = getChildByTag(50);
			a->runAction(MoveBy::create(0.5f, Vec3(0, -400, 0)));
		}
		else if (nodeA->getTag() == 40 && nodeB->getTag() == 20) {
			auto a = getChildByTag(50);
			a->runAction(MoveBy::create(0.5f, Vec3(0, -400, 0)));
		}
		// died
		else if ((nodeA->getTag() == 200 || nodeA->getTag() == 500) && nodeB->getTag() == 20) {
			nodeB->removeFromParentAndCleanup(true);
			GameOver();
		}
		else if (nodeA->getTag() == 20 && (nodeB->getTag() == 200 || nodeB->getTag() == 500)) {
			nodeA->removeFromParentAndCleanup(true);
			GameOver();
		}
		// food or drink
		else if ((nodeA->getTag() == 201 || nodeA->getTag() == 202 || nodeA->getTag() == 203) && nodeB->getTag() == 20) {
			if (nodeA->getTag() == 201) {
				food->setString("汉堡: 1");
				IsFoodEaten = true;
				nodeA->removeFromParentAndCleanup(true);
			}
			else if (nodeA->getTag() == 202) {
				tea->setString("咖啡: 1");
				IsTeaDrunk = true;
				nodeA->removeFromParentAndCleanup(true);
			}
			else if (nodeA->getTag() == 203) {
				if (IsFoodEaten && IsTeaDrunk) {
					GameWin();
					nodeA->removeFromParentAndCleanup(true);//win
				}
			}

		}
		else if ((nodeB->getTag() == 201 || nodeB->getTag() == 202 || nodeB->getTag() == 203) && nodeA->getTag() == 20) {
			if (nodeB->getTag() == 201) {
				food->setString("汉堡: 1");
				IsFoodEaten = true;
				nodeB->removeFromParentAndCleanup(true);
			}
			else if (nodeB->getTag() == 202) {
				tea->setString("咖啡: 1");
				IsTeaDrunk = true;
				nodeB->removeFromParentAndCleanup(true);
			}
			else if (nodeB->getTag() == 203) {
				if (IsFoodEaten && IsTeaDrunk) {
					GameWin();
					nodeB->removeFromParentAndCleanup(true);//win
				}
			}
		}
		else {
			if (nodeA->getTag() == 101 || nodeA->getTag() == 102 || nodeA->getTag() == 103 || nodeA->getTag() == 104) {
				CCLOG("checkpoint %d", nodeA->getTag());
				if (nodeB->getTag() == 20)
					which = nodeA->getTag() - 100;
				if (nodeA->getTag() == 103) {
					auto a = getChildByTag(200);
					if (a != NULL) {
						auto se = Sequence::create(MoveBy::create(1.0f, Vec3(32, 0, 0)), RemoveSelf::create(), nullptr);
						a->runAction(se);
					}
					auto b = getChildByTag(500);
					if (b != NULL) {
						auto se = Sequence::create(MoveBy::create(1.0f, Vec3(32, 0, 0)), RemoveSelf::create(), nullptr);
						b->runAction(se);
					}
				}
				else if (nodeA->getTag() == 101) {
					auto b = getChildByTag(500);
					if (b != NULL) {
						auto se = Sequence::create(MoveBy::create(1.0f, Vec3(32, 0, 0)), RemoveSelf::create(), nullptr);
						b->runAction(se);
					}
				}
			}
			else if (nodeB->getTag() == 101 || nodeB->getTag() == 102 || nodeB->getTag() == 103 || nodeB->getTag() == 104) {
				CCLOG("checkpoint %d", nodeB->getTag());
				if (nodeA->getTag() == 20)
					which = nodeB->getTag() - 100;
				if (nodeB->getTag() == 103) {
					auto a = getChildByTag(200);
					if (a != NULL) {
						auto se = Sequence::create(MoveBy::create(1.0f, Vec3(32, 0, 0)), RemoveSelf::create(), nullptr);
						a->runAction(se);
					}
					auto b = getChildByTag(500);
					if (b != NULL) {
						auto se = Sequence::create(MoveBy::create(1.0f, Vec3(32, 0, 0)), RemoveSelf::create(), nullptr);
						b->runAction(se);
					}
				}
				else if (nodeB->getTag() == 101) {
					auto b = getChildByTag(500);
					if (b != NULL) {
						auto se = Sequence::create(MoveBy::create(1.0f, Vec3(32, 0, 0)), RemoveSelf::create(), nullptr);
						b->runAction(se);
					}
				}
			}
		}
	}

	return true;
}
// 创建角色
void Round3::addPlayer() {
	auto yLen = visibleSize.height / 19;
	auto xLen = visibleSize.width / 25 / 1.4;
	IsPlayer1Jump = false;
	IsPlayer1Left = false;
	IsPlayer1Right = false;
	LastPlayer1Press = 'D';
	//create player1
	auto texture1 = Director::getInstance()->getTextureCache()->addImage("patrick_idle2.png");
	frame1 = SpriteFrame::createWithTexture(texture1, CC_RECT_PIXELS_TO_POINTS(Rect(0, 4, 25, 32)));
	player1 = Sprite::createWithSpriteFrame(frame1);
	if (which == 1) {
		player1->setPosition(Vec2(visibleSize.width / 4 - 60 + xLen, 46));
	}
	else if (which == 2) {
		player1->setPosition(224 + xLen, visibleSize.height / 2 - 40);
	}
	else if (which == 3) {
		player1->setPosition(Vec2(visibleSize.width / 4 * 3 + 50 + xLen, visibleSize.height - 120));
	}
	else if (which == 4) {
		player1->setPosition(20 * xLen - 18 + xLen, 5 * yLen - 19);
	}
	player1->setTag(20);
	// 设置角色刚体属性
	// Todo
	auto physicsBody = PhysicsBody::createBox(Size(32.0f, 36.0f), PhysicsMaterial(1.0f, 1.0f, 0.0f));
	physicsBody->setDynamic(true);
	physicsBody->setCategoryBitmask(0xFFFFFFF);
	physicsBody->setCollisionBitmask(0xFFFFFFF);
	physicsBody->setContactTestBitmask(0xFFFFFFFF);
	physicsBody->setRotationEnable(false);
	player1->setPhysicsBody(physicsBody);
	player1->setName("player1");
	player1->setScale(0.50);
	this->addChild(player1, 2);
	player1->getPhysicsBody()->setVelocity(Vec2(0, 0));
	loadAnimation("player1"); // 加载人物动画
}

//加载玩家动画帧, 共8种 :)
void Round3::loadAnimation(string filepath) {

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

	Vector<SpriteFrame*> PlayerJumpWithoutBox;

	auto PlayerImage = Director::getInstance()->getTextureCache()->addImage(filepath + ".png");

	PlayerIdleWithoutBox.reserve(3);
	for (int i = 0; i < 3; i++) {
		auto frame = SpriteFrame::createWithTexture(PlayerImage, CC_RECT_PIXELS_TO_POINTS(Rect(32 * i, 5, 32, 36)));
		PlayerIdleWithoutBox.pushBack(frame);
	}
	Animation* PlayerIdelWithoutBoxAnimation = Animation::createWithSpriteFrames(PlayerIdleWithoutBox, 0.1f);
	AnimationCache::getInstance()->addAnimation(PlayerIdelWithoutBoxAnimation, filepath + "IdleWithoutBoxAnimation");
	/*

	PlayerRunWithoutBox.reserve(8);
	for (int i = 0; i < 8; i++) {
		auto frame = SpriteFrame::createWithTexture(PlayerImage, CC_RECT_PIXELS_TO_POINTS(Rect(32 * i, 41, 32, 36)));
		PlayerRunWithoutBox.pushBack(frame);
	}

	Animation* PlayerRunWithoutBoxAnimation = Animation::createWithSpriteFrames(PlayerRunWithoutBox, 0.1f);
	AnimationCache::getInstance()->addAnimation(PlayerRunWithoutBoxAnimation, filepath + "RunWithoutBoxAnimation");

	*/
	PlayerJumpWithoutBox.pushBack(SpriteFrame::createWithTexture(PlayerImage, CC_RECT_PIXELS_TO_POINTS(Rect(64, 41, 32, 36))));
	if (filepath == "player1")
		PlayerJumpWithoutBox.pushBack(frame1);
	else
		PlayerJumpWithoutBox.pushBack(frame2);
	Animation* PlayerJumpWithoutBoxAnimation = Animation::createWithSpriteFrames(PlayerJumpWithoutBox, 1.0f);
	AnimationCache::getInstance()->addAnimation(PlayerJumpWithoutBoxAnimation, filepath + "JumpWithoutBoxAnimation");

}

void Round3::update(float dt) {
	//this->getScene()->getPhysicsWorld()->step(1 / 100.0f);
	auto winsize = Director::getInstance()->getWinSize();
	if (IsMoveEnable) {
		auto position = player1->getPosition();
		int x = position.x;
		//移动
		if (IsPlayer1Left || IsPlayer1Right) {
			if (player1->getSpriteFrame() == frame1) {
				auto animation = RepeatForever::create(Animate::create(AnimationCache::getInstance()->getAnimation("patrickRun")));
				animation->setTag(11);
				player1->runAction(animation);
			}
		}
	}
}

// 上左右 
void Round3::onKeyPressed(EventKeyboard::KeyCode code, Event* event) {
	if (code == cocos2d::EventKeyboard::KeyCode::KEY_R) {
		Director::getInstance()->replaceScene(Round3::createScene());
	}
	else if (IsMoveEnable) {
		Action* animation;
		float x = player1->getPositionX();
		auto vx = player1->getPhysicsBody()->getVelocity().x;
		auto vy = player1->getPhysicsBody()->getVelocity().y;
		auto winsize = Director::getInstance()->getWinSize();
		switch (code) {
		case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		case cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW:
			if (code == EventKeyboard::KeyCode::KEY_LEFT_ARROW) {
				if (isRotate == false) {
					player1->setFlipX(180);
					isRotate = true;
				}
				if (x>80) {
					player1->getPhysicsBody()->setVelocity(Vec2(-200, vy));
				}
				IsPlayer1Left = true;
				IsPlayer1Right = false;
				//CCLOG("click left");
			}
			if (code == EventKeyboard::KeyCode::KEY_RIGHT_ARROW) {
				if (isRotate) {
					player1->setFlipX(0);
					isRotate = false;
				}
				if (x<winsize.width - 80) {
					player1->getPhysicsBody()->setVelocity(Vec2(200, vy));
				}
				IsPlayer1Left = false;
				IsPlayer1Right = true;
				//CCLOG("click right");
			}
			if (code == EventKeyboard::KeyCode::KEY_UP_ARROW) {
				player1->getPhysicsBody()->setVelocity(Vec2(vx, 400));
				IsPlayer1Jump = true;
				//CCLOG("click jump");
			}
			break;
		case cocos2d::EventKeyboard::KeyCode::KEY_SPACE:
			player1->getPhysicsBody()->setVelocity(Vec2(vx, 250));
			IsPlayer1Jump = true;
			//CCLOG("click space");
		default:
			break;
		}
	}
}

// 释放按键
void Round3::onKeyReleased(EventKeyboard::KeyCode code, Event* event) {
	if (IsMoveEnable) {
		auto v = player1->getPhysicsBody()->getVelocity();
		switch (code) {
		case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
			IsPlayer1Left = false;
			player1->getPhysicsBody()->setVelocity(Vec2(0, v.y));
			player1->stopAllActions();
			player1->setSpriteFrame(frame1);
			break;
		case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
			IsPlayer1Right = false;
			player1->getPhysicsBody()->setVelocity(Vec2(0, v.y));
			player1->stopAllActions();
			player1->setSpriteFrame(frame1);
			break;
		case cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW:
			IsPlayer1Jump = false;
		}
	}
}


// 游戏结束
void Round3::GameOver() {
	IsMoveEnable = false;
	unschedule(schedule_selector(Round3::update));
	SimpleAudioEngine::getInstance()->stopBackgroundMusic("backgroundmusic3.mp3");
	//SimpleAudioEngine::getInstance()->playEffect("gameover.mp3", false);

	auto label1 = Label::createWithTTF("YOU DIED", "fonts/STXINWEI.TTF", 60);
	label1->setColor(Color3B(255, 0, 0));
	label1->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	this->addChild(label1);

	auto label2 = Label::createWithTTF("Press R To Restart", "fonts/STXINWEI.TTF", 40);
	label2->setColor(Color3B(0, 0, 0));
	auto replayBtn = MenuItemLabel::create(label2, CC_CALLBACK_1(Round3::replayCallback, this));
	Menu* replay = Menu::create(replayBtn, NULL);
	replay->setPosition(visibleSize.width / 2, visibleSize.height / 2 - 100);
	this->addChild(replay);

	//auto label3 = Label::createWithTTF("exit", "fonts/STXINWEI.TTF", 40);
	//label3->setColor(Color3B(0, 0, 0));
	//auto exitBtn = MenuItemLabel::create(label3, CC_CALLBACK_1(FinalProject::exitCallback, this));
	//Menu* exit = Menu::create(exitBtn, NULL);
	//exit->setPosition(visibleSize.width / 2 - 80, visibleSize.height / 2 - 150);
	//this->addChild(exit);
}
void Round3::GameWin() {
	IsMoveEnable = false;
	unschedule(schedule_selector(Round3::update));
	SimpleAudioEngine::getInstance()->stopBackgroundMusic("backgroundmusic3.mp3");
	//SimpleAudioEngine::getInstance()->playEffect("gameover.mp3", false);

	auto label1 = Label::createWithTTF("YOU WIN", "fonts/STXINWEI.TTF", 60);
	label1->setColor(Color3B(255, 0, 0));
	label1->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	this->addChild(label1);

	auto label2 = Label::createWithTTF("Press R To Restart", "fonts/STXINWEI.TTF", 40);
	label2->setColor(Color3B(0, 0, 0));
	auto replayBtn = MenuItemLabel::create(label2, CC_CALLBACK_1(Round3::replayCallback, this));
	Menu* replay = Menu::create(replayBtn, NULL);
	replay->setPosition(visibleSize.width / 2, visibleSize.height / 2 - 100);
	this->addChild(replay);

	//auto label3 = Label::createWithTTF("exit", "fonts/STXINWEI.TTF", 40);
	//label3->setColor(Color3B(0, 0, 0));
	//auto exitBtn = MenuItemLabel::create(label3, CC_CALLBACK_1(FinalProject::exitCallback, this));
	//Menu* exit = Menu::create(exitBtn, NULL);
	//exit->setPosition(visibleSize.width / 2 - 80, visibleSize.height / 2 - 150);
	//this->addChild(exit);
}

// 继续或重玩按钮响应函数
void Round3::replayCallback(Ref * pSender) {
	Director::getInstance()->replaceScene(Round3::createScene());
}

// 退出
void Round3::exitCallback(Ref * pSender) {
	Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}
void Round3::addFloor() {
	//add floor
	auto block = Sprite::create("block.png");
	auto vertical = Sprite::create("vertical.png");
	auto horizontal = Sprite::create("horizontal.png");
	// row below sight
	for (int i = 12; i <= 20; i += 2) {
		addBlock(i, -1);
		addBlock(i, -2);
	}
	addTrap(13, -3, true);
	addTrap(15, -3, true);
	addTrap(17, -3, true);
	addTrap(19, -3, true);
	// row 1
	addBlock(0, 0);
	for (int i = 1; i <= 3; i++)
		addHorizontal(i, 0);
	addBlock(4, 0);
	addHorizontal(5, 0);
	addBlock(6, 0);
	for (int i = 7; i <= 9; i++)
		addHorizontal(i, 0);
	addBlock(10, 0);
	addHorizontal(11, 0);
	addBlock(12, 0);
	addBlock(14, 0);
	addBlock(16, 0);
	addBlock(18, 0);
	addBlock(20, 0);
	for (int i = 21; i <= 23; i++)
		addHorizontal(i, 0);
	addBlock(24, 0);
	// row 2 
	if (!IsTeaDrunk) {
		addFood(0, 1, 1);
	}
	addBlock(4, 1);
	addHorizontal(5, 1);
	addBlock(6, 1);
	for (int i = 7; i <= 9; i++)
		addHorizontal(i, 1);
	addVertical(10, 1);
	addBlock(11, 1);
	addBlock(20, 1);
	for (int i = 21; i <= 22; i++)
		addHorizontal(i, 1);
	addBlock(23, 1);
	addVertical(24, 1);
	// row 3
	addTrap(4, 2, true);
	addBlock(5, 2);
	addVertical(6, 2);
	for (int i = 7; i <= 9; i++)
		addHorizontal(i, 2);
	addBlock(10, 2);
	addTrap(11, 2, true);
	addTrap(14, 2, false);
	addTrap(15, 2, false);
	addTrap(17, 2, false);
	addTrap(18, 2, false);
	addBlock(21, 2);
	for (int i = 22; i <= 23; i++)
		addHorizontal(i, 2);
	addBlock(24, 2);
	// row 4
	addTrap(5, 3, true);
	addBlock(6, 3);
	for (int i = 7; i <= 8; i++)
		addHorizontal(i, 3);
	addBlock(9, 3);
	addTrap(10, 3, true);
	addBlock(13, 3);
	for (int i = 14; i <= 18; i++)
		addHorizontal(i, 3);
	addBlock(19, 3);
	addBlock(22, 3);
	addHorizontal(23, 3);
	addBlock(24, 3);
	// row 5
	addCheckpoint(2, 4, 101);
	addBlock(3, 4);
	for (int i = 7; i <= 8; i++)
		addTrap(i, 4, true);
	addBlock(12, 4);
	addVertical(13, 4);
	addCheckpoint(14, 4, 104);
	addTransparent(16, 4);
	addTransparent(16, 5);
	addTransparent(16, 6);
	addTransparent(19, 4, 0, 0, 110);// wait to sweep
	addTrap(23, 4l, true, 0, 0, 60);// hey hey hey
	addVertical(24, 4);
	// row 6
	addVertical(3, 5);
	addBlock(11, 5);
	addVertical(12, 5);
	addVertical(13, 5);
	addTransparent(19, 5, 0, 0, 120);// wait to sweep
	addTrap(23, 5, false, 0, 0, 70);// hey hey hey
	addVertical(24, 5);
	// row 7
	addBlock(0, 6);
	for (int i = 1; i <= 2; i++)
		addHorizontal(i, 6);
	addVertical(3, 6);
	for (int i = 4; i <= 5; i++)
		addHorizontal(i, 6);
	addBlock(6, 6);
	addBlock(9, 6);
	for (int i = 10; i <= 12; i++)
		addHorizontal(i, 6);
	addBlock(13, 6);
	addTrap(22, 6, false);
	addBlock(23, 6);
	addVertical(24, 6);
	// row 8
	addBlock(1, 7);
	addHorizontal(2, 7);
	addBlock(3, 7);
	for (int i = 4; i <= 10; i++)
		addHorizontal(i, 7);
	addVertical(11, 7);
	addBlock(12, 7);
	addTrap(15, 7, false);
	addBlock(16, 7);
	for (int i = 17; i <= 21; i++)
		addHorizontal(i, 7);
	addBlock(22, 7);
	addVertical(23, 7);
	addVertical(24, 7);
	// row 9
	addCheckpoint(1, 8, 102);
	addBlock(2, 8);
	for (int i = 3; i <= 10; i++)
		addHorizontal(i, 8);
	addBlock(11, 8);
	addTrap(14, 8, false);
	addBlock(15, 8);
	addTrap(17, 8, true, 10, 0);
	addTrap(20, 8, true);
	for (int i = 23; i <= 24; i++)
		addVertical(i, 8);
	// row 10
	addTrap(4, 9, true, 10, 0);
	addTrap(7, 9, true);
	addTrap(10, 9, true);
	addTrap(13, 9, false);
	addBlock(14, 9);
	addTrap(15, 9, true);
	addBlock(23, 9);
	addVertical(24, 9);
	// row 11
	addBlock(1, 10);
	for (int i = 2; i <= 3; i++)
		addTrap(i, 10, false);
	addTrap(5, 10, false, 20, 0);
	addTrap(8, 10, false, 10, 0);
	addTrap(12, 10, false);
	addBlock(13, 10);
	addTrap(16, 10, false, 0, 0, 50);// wait to fall
	addBlock(17, 10);
	addTrap(18, 10, false);
	addBlock(19, 10);
	addBlock(21, 10);
	addVertical(24, 10);
	// row 12
	addBlock(2, 11);
	for (int i = 3; i <= 4; i++)
		addHorizontal(i, 11);
	addBlock(5, 11);
	addHorizontal(6, 11);
	addBlock(7, 11);
	addVertical(8, 11);
	addBlock(9, 11);
	for (int i = 10; i <= 11; i++)
		addHorizontal(i, 11);
	addBlock(12, 11);
	addTrap(13, 11, true);
	addBlock(16, 11);
	addVertical(17, 11);
	addBlock(18, 11);
	addTrap(19, 11, true);
	addBlock(20, 11);
	addBlock(23, 11);
	addVertical(24, 11);
	// row 13
	for (int i = 1; i <= 2; i++)
		addBlock(i, 12);
	addHorizontal(3, 12);
	addBlock(4, 12);
	addVertical(5, 12);
	addBlock(6, 12);
	addBlock(8, 12);
	addBlock(10, 12);
	addTrap(14, 12, false);
	addBlock(15, 12);
	for (int i = 16; i <= 18; i++)
		addVertical(i, 12);
	for (int i = 23; i <= 24; i++)
		addVertical(i, 12);
	// row 14
	addVertical(5, 13);
	addTrap(8, 13, true);
	addTrap(10, 13, true);
	addBlock(14, 13);
	for (int i = 15; i <= 18; i++)
		addVertical(i, 13);
	addBlock(21, 13);
	addTrap(22, 13, false);
	addBlock(23, 13);
	addVertical(24, 13);
	// row 15
	addBlock(0, 14);
	addTrap(3, 14, false);
	addVertical(5, 14);
	addBlock(7, 14);
	addBlock(9, 14);
	addBlock(13, 14);
	addVertical(14, 14);
	addBlock(15, 14);
	for (int i = 16; i <= 17; i++)
		addHorizontal(i, 14);
	addBlock(18, 14);
	if (!IsTeaDrunk) {
		addTrap(21, 14, true, 0, 0, 500);
	}
	addBlock(22, 14);
	addHorizontal(23, 14);
	addBlock(24, 14);
	// row 16
	addVertical(0, 15);
	addTransparent(2, 13, 0, 0, 1024);// trigger
	addTrap(2, 15, false, 0, 0, 512);// wait to drop
	addBlock(3, 15);
	addBlock(5, 15);
	for (int i = 11; i <= 12; i++)
		addBlock(i, 15);
	addVertical(13, 15);
	addBlock(14, 15);
	addHorizontal(15, 15);
	addBlock(16, 15);
	addVertical(17, 15);
	addBlock(20, 15);
	addBlock(24, 15);
	// row 17
	addVertical(0, 16);
	addBlock(1, 16);
	addHorizontal(2, 16);
	addBlock(3, 16);
	addBlock(7, 16);
	for (int i = 8; i <= 9; i++)
		addHorizontal(i, 16);
	addBlock(10, 16);
	for (int i = 11; i <= 12; i++)
		addVertical(i, 16);
	addBlock(13, 16);
	for (int i = 14; i <= 16; i++)
		addHorizontal(i, 16);
	addBlock(17, 16);
	addBlock(19, 16);
	addVertical(20, 16);
	if (!IsFoodEaten) {
		addFood(24, 16, 0, 0, 0);
	}
	// row 18
	addVertical(0, 17);
	for (int i = 1; i <= 10; i++)
		addHorizontal(i, 17);
	for (int i = 11; i <= 12; i++)
		addBlock(i, 17);
	for (int i = 13; i <= 16; i++)
		addHorizontal(i, 17);
	addBlock(17, 17);
	addBlock(20, 17);
	addCheckpoint(21, 17, 103);
	// row 19
	addBlock(0, 18);
	for (int i = 1; i <= 16; i++)
		addHorizontal(i, 18);
	addBlock(17, 18);
	if (!IsFoodEaten || !IsTeaDrunk) {
		addFood(18, 18, 2);
	}
	if (!IsFoodEaten) {
		addTrap(18, 17, false, 0, 0, 200);
	}
	addBlock(19, 18);
	for (int i = 20; i <= 23; i++)
		addHorizontal(i, 18);
	addBlock(24, 18);
	// row 20
	addBlock(18, 19);//block above exit
					 // col -1
	addBlock(-1, 0);
	for (int i = 1; i <= 17; i++)
		addVertical(-1, i);
	addBlock(-1, 18);
	// col 25
	addBlock(25, 0);
	for (int i = 1; i <= 17; i++)
		addVertical(25, i);
	addBlock(25, 18);
}