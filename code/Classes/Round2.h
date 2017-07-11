#ifndef __Round2_H__
#define __Round2_H__

#include "cocos2d.h"
using namespace std;
USING_NS_CC;
class Round2 : public Layer {
public:
	static PhysicsWorld* world;
	static cocos2d::Scene* createScene();
	void setPhysicsWorld(PhysicsWorld * world);
	virtual bool init();

	void addSprite();   // 添加背景和各种精灵
	void addPlayer();   // 添加玩家
	void addListener(); // 添加监听器

	void update(float dt);
	void initKnief(float dt);

	void onKeyPressed(EventKeyboard::KeyCode code, Event * event);
	void onKeyReleased(EventKeyboard::KeyCode code, Event * event);

	void preloadMusic();                   // 预加载音乐
	void GameOver();                      // 游戏结束
	void loadAnimation(string filepath); // 加载动画

										 // 重玩或退出按钮响应函数
	void replayCallback(Ref * pSender);
	void exitCallback(Ref * pSender);
	// implement the "static create()" method manually
	void updateScore();
	void updateLife();
	bool onConcactBegin(PhysicsContact & contact);
	CREATE_FUNC(Round2);

private:
	PhysicsWorld* m_world;
	Size visibleSize;

	Sprite* ship;

	Node* edgeNode;

	Sprite* player1;
	Sprite* player2;
	//Sprite* kniefSprite;
	unsigned doortime;
	Sprite* door;
	vector<Sprite*> floatingfloors;
	vector<Sprite*> sweepingfloors;
	vector<Sprite*> revolvingfloors;
	vector<Sprite*> floors;
	SpriteFrame* frame1;
	SpriteFrame* frame2;

	list<Sprite*> kniefs;
	list<Sprite*> bombs;
	list<Sprite*> food;

	char LastPlayer1Press;

	bool IsPlayer1Jump;
	bool IsPlayer1Left;
	bool IsPlayer1Right;

	bool isRotate;

	int life;
	int score;

	cocos2d::Label* scoreLabel;
	cocos2d::Label* lifeLabel;
};

#endif // __FRIEND_SHIP_H__
