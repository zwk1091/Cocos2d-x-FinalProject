#ifndef __ROUND3_H__
#define __ROUND3_H__

#include "cocos2d.h"
using namespace std;
USING_NS_CC;
class Round3 : public Layer {
public:
	static PhysicsWorld* world;
	static cocos2d::Scene* createScene();
	void setPhysicsWorld(PhysicsWorld * world);
	virtual bool init();

	void addSprite();   // ��ӱ����͸��־���
	void addTrap(int x, int y, bool flag, int xx = 0, int yy = 0, int tag = 10);		// �������
	void addCheckpoint(int x, int y, int which);	//����
	void addTransparent(int x, int y, int xx = 0, int yy = 0, int tag = 40);
	void addFood(int x, int y, int type, int xx = 0, int yy = 0, int tag = 48);
	void addFloor();
	void addPlayer();   // ������
	void addListener(); // ��Ӽ�����
	void addBlock(int x, int y);
	void addHorizontal(int x, int y);
	void addVertical(int x, int y);
	void update(float dt);

	bool onContactBegin(PhysicsContact & contact);
	void onKeyPressed(EventKeyboard::KeyCode code, Event * event);
	void onKeyReleased(EventKeyboard::KeyCode code, Event * event);

	void preloadMusic();                   // Ԥ��������
	void GameWin();
	void GameOver();                      // ��Ϸ����
	void loadAnimation(string filepath); // ���ض���

										 // ������˳���ť��Ӧ����
	void replayCallback(Ref * pSender);
	void exitCallback(Ref * pSender);
	// implement the "static create()" method manually
	CREATE_FUNC(Round3);

private:
	PhysicsWorld* m_world;
	Size visibleSize;
	Label* tea;
	Label* food;
	int xOffset;
	int yOffset;
	static int which;
	static bool IsFoodEaten;
	static bool IsTeaDrunk;
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
	bool IsMoveEnable;
};

#endif // __FRIEND_SHIP_H__
