/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__
#define MAX_SCROLL_SPEED 5
#define SPRITE_SCALE_FACTOR 0.25f //����������� ��������������� ��� �������� �������

#define MAX_PLAYER_PROJECTILES 10 //������������ ���������� ������� ������

#define NUM_ASTEROIDS 30 //������������ ���������� ����������
#define ASTEROID_SPAWN_INTERVAL 0.2f //������������ �������� ��� �������� ����� ����������
#define ASTEROID_SPAWN_START 3.0f //����� ����, ����� �������� ���������� ���������
#define ASTEROID_SPAWN_END 20.0f //������� �����, ����� ��������� ���������� ���������

#define MAX_PLAYER_HEALTH 10.0f // ������������ �������� ������������ ������� ������

#define MAX_PLAYER_MINES 10 // ������������ ���������� ��� ������
#define MAX_ENEMY_PROJECTILES 40 // ������������ ���������� ��������� �������
#define MAX_ENEMY_HEALTH 50.0f // ������������ �������� ������������ ������� �����


#include "cocos2d.h"

class HelloWorld : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);

    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchMoved(cocos2d::Touch * touches, cocos2d::Event * event);
    //void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event * event);

    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);

private:
    //Window width/height
    int _winWidth;
    int _winHeight;

    //Origin
    cocos2d::CCPoint _origin;

    //Batch node for loading the spritesheet
    cocos2d::CCSpriteBatchNode* _spriteBatchNode;

    int _playerY;

    //�������
    float _gameTime;//������� ������
    float _laserTime;//������ ��������

    //�������� �� ����������
    float _scrollAxis;
    float _scrollSpeed;
    bool _accelerate;

    //������� �������
    cocos2d::CCSprite* _background1;
    cocos2d::CCSprite* _background2;

    //������ ������������ ������� ������
    cocos2d::CCSprite* _player;

    //������� ������ ������
    int _playerProjectileCount;

    //�������� ������� ������
    cocos2d::CCSprite* _playerProjectiles[MAX_PLAYER_PROJECTILES];

    //������ �� ������� ����������
    int _asteroidCount;

    // ����� � ������� ���������� ��������� ���������
    float _asteroidSpawnTime;

    // ������� ����������
    cocos2d::CCSprite* _asteroids[NUM_ASTEROIDS];

    // ��������� ������������ ������� ������
    float _playerHealth;

    //������ �� �������� ������ ������
    int _playerMineCount;

    //������� ��� ������
    cocos2d::CCSprite* _playerMines[MAX_PLAYER_MINES];

    // ������ �� ���������� ����� ��� ���������� ��������
    bool _spawnEnemy;

    //����������� ������� ������
    int _enemyProjectileCount;


    // ������� ��������� ���������� ������������ �������
    int _enemyHealth;

    // ������ ���������� ������������ �������
    cocos2d::CCSprite* _enemy;

    // ������ � ����� ����� �������� ������
    cocos2d::CCSprite* _playerHealthBar;
    cocos2d::CCLabelTTF* _playerHealthLabel;

    // ������ � ����� ����� �������� �����
    cocos2d::CCSprite* _enemyHealthBar;
    cocos2d::CCLabelTTF* _enemyHealthLabel;

    //Game Over ������
    int _gameOver;

    // ����������� ���������� ����
    cocos2d::CCLabelTTF* _gameResultLabel;

    //Main menu
    cocos2d::CCMenu* _mainMenu;

    //Restart Game
    cocos2d::CCMenuItemImage* _restartMenuItem;

    //������� ��������� �������
    cocos2d::CCSprite* _enemyProjectiles[MAX_ENEMY_PROJECTILES];

    // ���������� ������������ �������� �������.
    void spriteMoveFinished(cocos2d::CCNode* sender);

    // ����� ���������� ����
    void updateGame(float dt);

    // ������� ���������
    void backgroundScrolling();

    // �������� ����������
    void spawnAsteroids();

    // �������� ����� ����� 5 ������ ����� ��������� ���������� ���������
    void spawnEnemy();

    // ��������
    void fire();

    //�andom number between min and max
    float randomRange(float min, float max);

    // ������� ������ ������� � ������� ����� plist � �������
    void createParticleEffect(const char* filename, float x, float y);

    // ������� ������ �������, ��������� ���� plist, �������, ��������� ����, ����������������� � �������� ������
    void createParticleEffect(const char* filename, float x, float y, cocos2d::ccColor4F startColor, float duration, float endSize);

    // ��������� �������� ���������� ������� ���������
    void enemyMoveFinished(cocos2d::CCNode* sender);

    // ������� ����� �������� ��� �����
    void enemyAnimate();

    // ���������� ����� �������� ������
    void updatePlayerHealthBar();

    // �������� ����� �������� �����
    void updateEnemyHealthBar();

    //��������� win/lose
    void endGame(bool won);
    void menuRestartCallback(Ref* Scene);
};

#endif // __HELLOWORLD_SCENE_H__
