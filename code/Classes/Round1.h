#pragma once
#ifndef __ROUND1_H__
#define __ROUND1_H__

#include "cocos2d.h"
#include<iomanip>
using namespace std;
USING_NS_CC;
class Round1 : public Layer {
public:
	static PhysicsWorld* world;
	static cocos2d::Scene* createScene();
	void setPhysicsWorld(PhysicsWorld * world);
	virtual bool init();

	void addSprite();   // ��ӱ����͸��־���
	void addPlayer();   // ������
	void addListener(); // ��Ӽ�����

	void update(float dt);
	//new added
	void bombFall(float dt);
	bool onConcactBegin(PhysicsContact & contact);
	void createExplosion();
	void loadExplosion();

	void onKeyPressed(EventKeyboard::KeyCode code, Event * event);
	void onKeyReleased(EventKeyboard::KeyCode code, Event * event);

	void preloadMusic();                   // Ԥ��������
	void GameOver();                      // ��Ϸ����
	void loadAnimation(string filepath); // ���ض���

										 // ������˳���ť��Ӧ����
	void replayCallback(Ref * pSender);
	void exitCallback(Ref * pSender);
	// implement the "static create()" method manually
	CREATE_FUNC(Round1);

private:
	PhysicsWorld* m_world;
	Size visibleSize;

	Sprite* ship;

	Node* edgeNode;

	Sprite* player1;
	Sprite* player2;

	SpriteFrame* frame1;
	SpriteFrame* frame2;

	char LastPlayer1Press;

	bool IsPlayer1Jump;
	bool IsPlayer1Left;
	bool IsPlayer1Right;

	bool isRotate;
	bool isGrounded;
	unsigned count;
	unsigned doortime;
	Sprite* door;
	//
	vector<Sprite*> floatingfloors;
	vector<Sprite*> sweepingfloors;
	vector<Sprite*> revolvingfloors;
	vector<Sprite*> floors;
	cocos2d::Vector<SpriteFrame*> explosion;
	cocos2d::Label* Score;
	//2�����£� 3������
	int floorType[4][7] = {
		{ 0,1,0,0,2,0,1 },
		{ 0,0,2,1,0,0,3 },
		{ 0,1,0,0,0,2,0 },
		{ 1,0,0,3,0,2,0 }
	};
	vector<int> staticFloors;
	int score;
};

#endif // __FRIEND_SHIP_H__
