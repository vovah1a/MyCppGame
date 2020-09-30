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
#define SPRITE_SCALE_FACTOR 0.25f //Коэффициент масштабирования для ресурсов спрайта

#define MAX_PLAYER_PROJECTILES 10 //Максимальное количество лазеров игрока

#define NUM_ASTEROIDS 30 //Максимальное количество астероидов
#define ASTEROID_SPAWN_INTERVAL 0.2f //Интервальная задержка для создания новых астероидов
#define ASTEROID_SPAWN_START 3.0f //Время игры, когда начинают появляться астероиды
#define ASTEROID_SPAWN_END 20.0f //Игровое время, когда перестают появляться астероиды

#define MAX_PLAYER_HEALTH 10.0f // Максимальное здоровье космического корабля игрока

#define MAX_PLAYER_MINES 10 // Максимальное количество мин игрока
#define MAX_ENEMY_PROJECTILES 40 // Максимальное количество вражеских лазеров
#define MAX_ENEMY_HEALTH 50.0f // Максимальное здоровье космического корабля врага


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

    //Таймеры
    float _gameTime;//игровой таймер
    float _laserTime;//Таймер стрельбы

    //слежение за прокруткой
    float _scrollAxis;
    float _scrollSpeed;
    bool _accelerate;

    //Фоновые спрайты
    cocos2d::CCSprite* _background1;
    cocos2d::CCSprite* _background2;

    //Спрайт космического корабля игрока
    cocos2d::CCSprite* _player;

    //Текущий снаряд игрока
    int _playerProjectileCount;

    //Лазерные спрайты игрока
    cocos2d::CCSprite* _playerProjectiles[MAX_PLAYER_PROJECTILES];

    //Следим за текущим астероидом
    int _asteroidCount;

    // Время с момента последнего появления астероида
    float _asteroidSpawnTime;

    // Спрайты астероидов
    cocos2d::CCSprite* _asteroids[NUM_ASTEROIDS];

    // Состояние космического корабля игрока
    float _playerHealth;

    //Следим за текущеей минной игрока
    int _playerMineCount;

    //Спрайты мин игрока
    cocos2d::CCSprite* _playerMines[MAX_PLAYER_MINES];

    // Следим за появлением врага для начального движения
    bool _spawnEnemy;

    //Отслеживаем текущий снаряд
    int _enemyProjectileCount;


    // Текущее состояние вражеского космического корабля
    int _enemyHealth;

    // Спрайт вражеского космического корабля
    cocos2d::CCSprite* _enemy;

    // Спрайт и метка шкалы здоровья игрока
    cocos2d::CCSprite* _playerHealthBar;
    cocos2d::CCLabelTTF* _playerHealthLabel;

    // Спрайт и метка шкалы здоровья врага
    cocos2d::CCSprite* _enemyHealthBar;
    cocos2d::CCLabelTTF* _enemyHealthLabel;

    //Game Over статус
    int _gameOver;

    // Отображение результата игры
    cocos2d::CCLabelTTF* _gameResultLabel;

    //Main menu
    cocos2d::CCMenu* _mainMenu;

    //Restart Game
    cocos2d::CCMenuItemImage* _restartMenuItem;

    //Спрайты вражеских лазеров
    cocos2d::CCSprite* _enemyProjectiles[MAX_ENEMY_PROJECTILES];

    // Обработчик стандартного действия спрайта.
    void spriteMoveFinished(cocos2d::CCNode* sender);

    // Метод обновления игры
    void updateGame(float dt);

    // Фоновая прокрутка
    void backgroundScrolling();

    // Создание астероидов
    void spawnAsteroids();

    // Создание врага через 5 секунд после появления последнего астероида
    void spawnEnemy();

    // стрельба
    void fire();

    //Кandom number between min and max
    float randomRange(float min, float max);

    // Создаем эффект частицы с помощью файла plist и позиции
    void createParticleEffect(const char* filename, float x, float y);

    // Создаем эффект частицы, используя файл plist, позицию, начальный цвет, продолжительность и конечный размер
    void createParticleEffect(const char* filename, float x, float y, cocos2d::ccColor4F startColor, float duration, float endSize);

    // Обработка действия вражеского спрайта завершена
    void enemyMoveFinished(cocos2d::CCNode* sender);

    // Создаем новое действие для врага
    void enemyAnimate();

    // Обновление шкалы здоровья игрока
    void updatePlayerHealthBar();

    // Обновить шкалу здоровья врага
    void updateEnemyHealthBar();

    //Обработка win/lose
    void endGame(bool won);
    void menuRestartCallback(Ref* Scene);
};

#endif // __HELLOWORLD_SCENE_H__
