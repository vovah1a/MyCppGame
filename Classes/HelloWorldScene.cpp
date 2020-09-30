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
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES O F MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "HelloWorldScene.h"


USING_NS_CC;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

    CCSize  visibleSize = Director::getInstance()->getVisibleSize();
    CCPoint  origin = Director::getInstance()->getVisibleOrigin();

   
    do {
        //Initialize game time
        _gameTime = 0;
        //Initialize scrolling variables
        _scrollSpeed = 0;
        _accelerate = true;
        _scrollAxis = 0;

        //Get window width/height
        CCSize winSize = CCDirector::sharedDirector()->getWinSize();
        _winHeight = winSize.height;
        _winWidth = winSize.width;

        //Get origin
        _origin = CCDirector::sharedDirector()->getVisibleOrigin();

        // Загружаем таблицу спрайтов
        _spriteBatchNode = CCSpriteBatchNode::create("spriteSheet.png");
        this->addChild(_spriteBatchNode);
        CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("spriteSheet.plist");

        // Загружаем фоновые спрайты
        _background1 = CCSprite::createWithSpriteFrameName("background.png");
        _background1->setAnchorPoint(ccp(0.0f, 0.0f));
        _background1->setPosition(ccp(0.0f, 0.0f));

        CCRect bgRect = _background1->getTextureRect();
        _background1->setScaleX(winSize.width / bgRect.size.width);
        _background1->setScaleY(winSize.height / bgRect.size.height);

        _background2 = CCSprite::createWithSpriteFrameName("background.png");
        _background2->setAnchorPoint(ccp(0.0f, 0.0f));
        _background2->setPosition(ccp(0.0f, winSize.height));

        _background2->setScaleX(winSize.width / bgRect.size.width);
        _background2->setScaleY(winSize.height / bgRect.size.height);
        this->addChild(_background2);
        this->addChild(_background1);

        // Загрузка спрайта космического корабля игрока
        _player = CCSprite::createWithSpriteFrameName("playership.png");
        _player->setScale(SPRITE_SCALE_FACTOR);
        // Поместите спрайт игрока в нижнюю часть экрана
        _playerY = _origin.y + (_player->getScaleY() * _player->getContentSize().height);
        _player->setPosition(ccp(_winWidth / 2, _playerY));
        this->addChild(_player, 10); // Наивысший Z-порядок, так как спрайт игрока должен всегда быть наверху        

        // Инициализируем счетчик снарядов игрока
        _playerProjectileCount = 0;
        
        // Инициализируем спрайты снарядов игрока
        for (int i = 0; i < MAX_PLAYER_PROJECTILES; i++) {
            _playerProjectiles[i] = CCSprite::createWithSpriteFrameName(
                "playerlaser.png");
            _playerProjectiles[i]->setScale(SPRITE_SCALE_FACTOR);
            _playerProjectiles[i]->setVisible(false);
            _playerProjectiles[i]->setPosition(ccp(_player->getPositionX(), _player->getPositionY() + (_player->getScaleY() * _player->getContentSize().height)));
            this->addChild(_playerProjectiles[i]);
        }

        auto listener = EventListenerTouchOneByOne::create();
        listener->setSwallowTouches(true);

        listener->onTouchBegan = CC_CALLBACK_2(HelloWorld::onTouchBegan, this);
        listener->onTouchMoved = CC_CALLBACK_2(HelloWorld::onTouchMoved, this);
        //listener->onTouchEnded = CC_CALLBACK_2(HelloWorld::onTouchEnded, this);

        _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

        // Инициализировать текущее количество астероидов и время появления
        _asteroidSpawnTime = 0;
        _asteroidCount = 0;

        // Инициализируем спрайты астероидов
        for (int i = 0; i < NUM_ASTEROIDS; ++i) {
            _asteroids[i] = CCSprite::createWithSpriteFrameName("asteroid.png");        
            _asteroids[i]->setVisible(false);
            _asteroids[i]->setPosition(ccp(_winWidth, _winHeight + _asteroids[_asteroidCount]->getScaleY() * _asteroids[_asteroidCount]->getContentSize().height));
            this->addChild(_asteroids[i]);
        }

        //Инициализировать здоровье игрока
        _playerHealth = MAX_PLAYER_HEALTH;

        //Инициализировать счетчик мин игрока
        _playerMineCount = 0;

        // Инициализировать статус появления врага, текущее количество снарядов и здоровье
        _spawnEnemy = false;
        _enemyProjectileCount = 0;
        _enemyHealth = MAX_ENEMY_HEALTH;

        // Инициализируем спрайты Player Mine
        for (int i = 0; i < MAX_PLAYER_MINES; ++i) {
            _playerMines[i] = CCSprite::createWithSpriteFrameName("playermine.png");           
            _playerMines[i]->setScale(SPRITE_SCALE_FACTOR);
            _playerMines[i]->setVisible(false);
            _playerMines[i]->setPosition(ccp(_player->getPositionX(), _player->getPositionY() - (_player->getScaleY() * _player->getContentSize().height)));
            this->addChild(_playerMines[i]);
        }

        // Загружаем спрайт вражеского космического корабля
        _enemy = CCSprite::createWithSpriteFrameName("enemyship.png");
        _enemy->setScale(SPRITE_SCALE_FACTOR);
        _enemy->setPosition(ccp(_winWidth / 2, _origin.y - (_enemy->getContentSize().height * _enemy->getScaleY())));
        _enemy->setVisible(false);
        this->addChild(_enemy);

        // Инициализировать спрайты вражеских снарядов
        for (int i = 0; i < MAX_ENEMY_PROJECTILES; ++i) {
            _enemyProjectiles[i] = CCSprite::createWithSpriteFrameName("enemylaser.png");             
            _enemyProjectiles[i]->setScale(SPRITE_SCALE_FACTOR);
            _enemyProjectiles[i]->setVisible(false);
            _enemyProjectiles[i]->setPosition(ccp(_enemy->getPositionX(), _enemy->getPositionY() + (_enemy->getScaleY() * _enemy->getContentSize().height)));
            this->addChild(_enemyProjectiles[i]);
        }

        // Инициализируем спрайты шкалы здоровья игрока и врага
        _playerHealthBar = CCSprite::createWithSpriteFrameName("health_bar_green.png");
        _playerHealthBar->setScale(SPRITE_SCALE_FACTOR);
        _playerHealthBar->setPosition(ccp(_origin.x + _playerHealthBar->getScaleX() * _playerHealthBar->getContentSize().width / 2, _winHeight - _playerHealthBar->getScaleY() * _playerHealthBar->getContentSize().height));
        this->addChild(_playerHealthBar, 10);

        _playerHealthLabel = CCLabelTTF::create("PLAYER", "Arial", 24);
        _playerHealthLabel->setPosition(ccp(_origin.x + _playerHealthLabel->getContentSize().width / 2, _winHeight - _playerHealthLabel->getContentSize().height - _playerHealthBar->getScaleY() * _playerHealthBar->getContentSize().height));
        this->addChild(_playerHealthLabel, 10);

        updatePlayerHealthBar();

        _enemyHealthBar = CCSprite::createWithSpriteFrameName("health_bar_green.png");
        _enemyHealthBar->setScaleY(SPRITE_SCALE_FACTOR);
        _enemyHealthBar->setPosition(ccp(_origin.x + _enemyHealthBar->getScaleX() * _enemyHealthBar->getContentSize().width / 2, _origin.y + _enemyHealthBar->getScaleY() * _enemyHealthBar->getContentSize().height));

        _enemyHealthBar->setVisible(false);
        this->addChild(_enemyHealthBar, 10);

        _enemyHealthLabel = CCLabelTTF::create("ENEMY", "Arial", 24);
        _enemyHealthLabel->setPosition(ccp(_origin.x + _enemyHealthLabel->getContentSize().width / 2, _origin.y + _enemyHealthLabel->getContentSize().height + _enemyHealthBar->getScaleY() * _enemyHealthBar->getContentSize().height));
        _enemyHealthLabel->setVisible(false);
        this->addChild(_enemyHealthLabel, 10);

        updateEnemyHealthBar();

        _gameOver = false;

        //Initialize game result label
        _gameResultLabel = CCLabelTTF::create("", "Arial", 60);
        _gameResultLabel->setVisible(false);
        this->addChild(_gameResultLabel, 1);
        //Initialize restart menu item

        //Initialize restart menu item
        _restartMenuItem = CCMenuItemImage::create();
        _restartMenuItem->setNormalSpriteFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("restart.png"));
        _restartMenuItem->setTarget(this, menu_selector(HelloWorld::menuRestartCallback));
        _restartMenuItem->setScale(0.2f);
        _restartMenuItem->setPosition(_winWidth / 2, _winHeight / 2 + _restartMenuItem->getScaleY() * _restartMenuItem->getContentSize().height);
        _restartMenuItem->setEnabled(false);
        _restartMenuItem->setVisible(false);

        //Create main menu
        _mainMenu = CCMenu::create(_restartMenuItem, NULL);
        _mainMenu->setPosition(Vec2::ZERO);
        this->addChild(_mainMenu, 1);
        //schedule updateGame to update the game at regular intervals
        this->schedule(schedule_selector(HelloWorld::updateGame));
    } while (0);
    return true;
}

void HelloWorld::updateGame(float dt) {
    _gameTime += dt;// Добавляем dt к игровому времени
    
    backgroundScrolling();// Фоновая прокрутка

    if (!_gameOver) {
        _asteroidSpawnTime += dt;
        spawnAsteroids();//Создание астероидов

        spawnEnemy();// Создание врага

        fire();// стрельба

        // Обнаружение столкновения Астероид <-> Игрок, Астероид <-> Снаряды игрока
        for (int i = 0; i < NUM_ASTEROIDS; ++i) {
            // Проверять столкновения только в том случае, если текущий астероид и игрок видны
            if (_asteroids[i]->isVisible() && _player->isVisible()) {
                if (_asteroids[i]->boundingBox().intersectsRect(_player->boundingBox())) {
                    --_playerHealth;
                    updatePlayerHealthBar();
                    if (_playerHealth <= 0) {
                        _player->stopAllActions();
                        endGame(false);
                        _player->setVisible(false);
                        // Уничтожить космический корабль игрока с эффектом
                        ccColor4F startColor = { 0.4f, 0.5f, 1.0f, 1.0f };
                        createParticleEffect("Particles/ExplodingRing.plist", _player->getPositionX(), _player->getPositionY(), startColor, 1.0f, 100.0f);
                    }
                    else {
                        _asteroids[i]->setVisible(false);
                        // Создаем эффект взрывающейся кольцевой частицы для разрушения астероида
                        createParticleEffect("Particles/ExplodingRing.plist", _asteroids[i]->getPositionX(), _asteroids[i]->getPositionY());
                        // Создаем мигающий эффект, обозначающий повреждение корабля игрока
                        CCBlink* blinkAction = CCBlink::create(1.0f, 10);
                        CCShow* showAction = CCShow::create();
                        CCSequence* action = CCSequence::create(blinkAction, showAction, NULL);
                        _player->runAction(action);
                    }
                }

                // Для каждого снаряда игрока проверяется на столкновение с текущим астероидом
                for (int j = 0; j < MAX_PLAYER_PROJECTILES; ++j) {
                    if (_playerProjectiles[j]->isVisible()) {
                        if (_playerProjectiles[j]->boundingBox().intersectsRect(_asteroids[i]->boundingBox())) {
                            _asteroids[i]->setVisible(false);
                            _playerProjectiles[j]->setVisible(false);
                            createParticleEffect("Particles/ExplodingRing.plist", _asteroids[i]->getPositionX(), _asteroids[i]->getPositionY());
                        }
                    }
                }
            }
        }

        //Обнаружение столкновения мины<->враг
        for (int j = 0; j < MAX_PLAYER_MINES; ++j) {     // Проверять столкновения только в том случае, если текущий снаряд игрока и враг видны
            if (_playerMines[j]->isVisible() && _enemy->isVisible()) {
                if (_playerMines[j]->boundingBox().intersectsRect(_enemy->boundingBox())) {
                    _playerMines[j]->setVisible(false);
                    // Создание эффекта взрывающейся кольцевой частицы для взрыва мины
                    createParticleEffect("Particles/ExplodingRing.plist", _playerMines[j]->getPositionX(), _playerMines[j]->getPositionY());
                    --_enemyHealth;
                    updateEnemyHealthBar();
                    // Создание эффекта малых частиц Солнца для обозначения урона врага
                    createParticleEffect("Particles/SmallSun.plist", _enemy->getPositionX(), _enemy->getPositionY());
                    if (_enemyHealth <= 0) {
                        // Уничтожить космический корабль врага с эффектом
                        endGame(true);
                        _enemy->setVisible(false);
                        ccColor4F startColor = { 1.0f, 0.5f, 0.5f, 1.0f };
                        createParticleEffect("Particles/Galaxy.plist", _enemy->getPositionX(), _enemy->getPositionY(), startColor, 1.0f, 200.0f);
                    }
                }
            }
        }

        //Обнаружение столкновения Снаряды противника <-> Игрок
        for (int j = 0; j < MAX_ENEMY_PROJECTILES; ++j) { // Проверить столкновения, если текущий снаряд врага и игрок видны
            if (_enemyProjectiles[j]->isVisible() && _player->isVisible()) {
                if (_enemyProjectiles[j]->boundingBox().intersectsRect(_player->boundingBox())) {
                    _enemyProjectiles[j]->setVisible(false);
                    --_playerHealth;
                    updatePlayerHealthBar();
                    // Создание эффекта малых частиц Солнца, чтобы обозначить урон игрока
                    createParticleEffect("Particles/SmallSun.plist", _player->getPositionX(), _player->getPositionY());
                    if (_playerHealth <= 0) {
                        _player->stopAllActions();
                        endGame(false);
                        _player->setVisible(false);
                        // Уничтожить космический корабль игрока с настраиваемым эффектом взрывающегося кольца
                        ccColor4F startColor = { 0.4f, 0.5f, 1.0f, 1.0f };
                        createParticleEffect("Particles/ExplodingRing.plist", _player->getPositionX(), _player->getPositionY(), startColor, 1.0f, 100.0f);
                    }
                    else {
                        // Создаем мигающий эффект, указывающий на повреждение космического корабля игрока
                        CCBlink* blinkAction = CCBlink::create(1.0f, 10);
                        CCShow* showAction = CCShow::create();
                        CCSequence* action = CCSequence::create(blinkAction, showAction, NULL);
                        _player->runAction(action);
                    }
                }
            }
        }
    }
    
}

void HelloWorld::backgroundScrolling() {
    if (_accelerate) { //ускоряться
        //Перемещайте ось прокрутки со скоростью прокрутки
        _scrollAxis -= _scrollSpeed;

        //Eсли мы прокрутили оба фона, сбросить ось прокрутки
        if (_scrollAxis <= -_winHeight) {
            _scrollAxis = 0;
        }

        //Обновить положение фоновых спрайтов
        _background1->setPosition(ccp(0.0f, _scrollAxis));
        _background2->setPosition(ccp(0.0f, _scrollAxis + _winHeight));


        //Увеличиваем скорость прокрутки, пока не приблизимся к максимальной
        if (_scrollSpeed < MAX_SCROLL_SPEED) {
            _scrollSpeed += 0.01;
        }
    }
    else { // замедляемся
        // Перемещаем ось прокрутки со скоростью прокрутки
        _scrollAxis -= _scrollSpeed;

        // если мы прокрутили оба фона, сбросить ось прокрутки
        if (_scrollAxis <= -_winHeight) {
            _scrollAxis = 0;
        }
        // Продолжаем уменьшать скорость прокрутки, пока не приблизимся к нулю
        if (_scrollSpeed > 0) {
            _scrollSpeed -= 0.01;
            _background1->setPosition(ccp(0.0f, _scrollAxis));
            _background2->setPosition(ccp(0.0f, _scrollAxis + _winHeight));
        }
    }
}

void HelloWorld::spawnAsteroids(){
    if (_asteroidSpawnTime > ASTEROID_SPAWN_INTERVAL && _gameTime > ASTEROID_SPAWN_START && _gameTime < ASTEROID_SPAWN_END) {
        _asteroidSpawnTime = 0;
        // Произвольно масштабируем астероид перед его созданием          
        float randnum = randomRange(0.1f, 0.25f);
        _asteroids[_asteroidCount]->setScaleX(randnum);
        _asteroids[_asteroidCount]->setScaleY(randnum);

        // Создать в случайной позиции X в зависимости от ширины экрана
        float positionX = randomRange(_asteroids[_asteroidCount]->getContentSize().width / 2 * _asteroids[_asteroidCount]->getScaleX(), _winWidth - _asteroids[_asteroidCount]->getContentSize().width / 2 * _asteroids[_asteroidCount]->getScaleX());
        _asteroids[_asteroidCount]->stopAllActions();
        _asteroids[_asteroidCount]->setPosition(ccp(positionX, (_winHeight + _asteroids[_asteroidCount]->getScaleY() * _asteroids[_asteroidCount]->getContentSize().height)));
        _asteroids[_asteroidCount]->setVisible(true);

        // Создание случайного действия перемещения астероида за [2.0,8.0] секунд
        _asteroids[_asteroidCount]->runAction(CCSequence::create(CCMoveTo::create(randomRange(2.0f, 8.0f), ccp(_asteroids[_asteroidCount]->getPositionX(), _origin.y - _asteroids[_asteroidCount]->getContentSize().height)), CCCallFuncN::create(this, callfuncN_selector(HelloWorld::spriteMoveFinished)), NULL));
        ++_asteroidCount;

        
        // Если израсходованы все доступные спрайты, сбрасываем счетчик для повторного использования спрайтов
        if (_asteroidCount >= NUM_ASTEROIDS) {
            _asteroidCount = 0;
        }
    }
}

void HelloWorld::spawnEnemy() {
    if (_gameTime > ASTEROID_SPAWN_END + 5.0f && !_spawnEnemy) {
        _enemyHealthBar->setVisible(true);
        _enemyHealthLabel->setVisible(true);
        _laserTime = _gameTime;
        // Остановить обратную прокрутку
        _accelerate = false;
        _enemy->setVisible(true);

        _playerY = _winHeight / 2 + _player->getContentSize().height * _player->getScaleY();
        CCMoveTo* playerMove = CCMoveTo::create(1.5, ccp(_winWidth / 2, _playerY));
        _player->runAction(CCSequence::create(playerMove, NULL));

        CCMoveTo* enemyMove = CCMoveTo::create(1.5, ccp(_winWidth / 2, _origin.y + _enemy->getContentSize().height * _enemy->getScaleY() / 2));
        _enemy->runAction(CCSequence::create(enemyMove, CCCallFuncN::create(this, callfuncN_selector(HelloWorld::enemyMoveFinished)), NULL));
        _spawnEnemy = true;
    }
}

void HelloWorld::fire() {
    if (!_spawnEnemy) { //Стрельба лазерами
        if (_laserTime == 0) {
            _laserTime = _gameTime;
            // Выбираем лазерный спрайт из массива снарядов и запускаем над ним действие
            _playerProjectiles[_playerProjectileCount]->stopAllActions();
            _playerProjectiles[_playerProjectileCount]->setPosition(ccp(_player->getPositionX(), _player->getPositionY() + (_player->getScaleY() * _player->getContentSize().height)));
            _playerProjectiles[_playerProjectileCount]->setVisible(true);

            // Создаем действие перемещения длиной 0,6 секунды и перемещаем снаряд, начиная с позиции игрока, в верхнюю часть экрана.
            _playerProjectiles[_playerProjectileCount]->runAction(CCSequence::create(CCMoveTo::create(0.6f, ccp(_player->getPositionX(), _winHeight)), CCCallFuncN::create(this, callfuncN_selector(HelloWorld::spriteMoveFinished)), NULL));
            ++_playerProjectileCount;


            // Если достигнуто максимальное количество спрайтов, сбрасываем счетчик для повторного использования спрайтов
            if (_playerProjectileCount >= MAX_PLAYER_PROJECTILES) {
                _playerProjectileCount = 0;
            }
        }
        else {
            if (_laserTime < _gameTime - 0.2f) {
                _laserTime = 0;
            }
        }

    }// Альтернативный режим огня - выпуск мин из задней части корабля
    else {
        // Выбираем спрайт мины из массива снарядов и запускаем на нем действие
        if (_laserTime == 0) {
            _laserTime = _gameTime;
            _playerMines[_playerMineCount]->stopAllActions();
            _playerMines[_playerMineCount]->setPosition(ccp(_player->getPositionX(), _player->getPositionY() - (_player->getScaleY() * _player->getContentSize().height)));
            _playerMines[_playerMineCount]->setVisible(true);

            _playerMines[_playerMineCount]->runAction(CCRepeatForever::create(CCRotateBy::create(1.0f, 360.0f)));
            //Create a move action that is 2.0s long and moves the mine starting from the player position to the bottom of the screen
            _playerMines[_playerMineCount]->runAction(CCSequence::create(CCMoveTo::create(2.0f, ccp(_player->getPositionX(), _origin.x)), CCCallFuncN::create(this, callfuncN_selector(HelloWorld::spriteMoveFinished)), NULL));
            ++_playerMineCount;

            //If reached the maximum number of sprites reset the the count to recycle the sprites
            if (_playerMineCount >= MAX_PLAYER_MINES) {
                _playerMineCount = 0;
            }
        }
        else {
            if (_laserTime < _gameTime - 0.4f) {
                _laserTime = 0;
            }
        }
    }
}

bool HelloWorld::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event)
{
    return true;
}
void HelloWorld::onTouchMoved(cocos2d::Touch* touches, cocos2d::Event* event)
{
    _player->setPosition(ccp(touches->getLocation().x, _playerY));
    CCPoint location = touches->getLocation();
}

float HelloWorld::randomRange(float min, float max) {
    float randnum = (float)rand() / (float)RAND_MAX;
    return min + randnum * (max - min);
}

void HelloWorld::spriteMoveFinished(cocos2d::CCNode* sender) {
    sender->setVisible(false);
}

void HelloWorld::enemyMoveFinished(cocos2d::CCNode* sender) {
    // Если враг не уничтожен, продолжаем анимацию
    if (_enemyHealth > 0) {
        enemyAnimate();
    }
}

void HelloWorld::enemyAnimate() {
    // Устанавливаем позицию врага для движения текущего врага Y и игрока X
    CCPoint enemyPosition = ccp(_player->getPositionX(), _enemy->getPositionY());

    // Если новая позиция врага находится за пределами видимой ширины экрана
    // Устанавливаем максимальную левую или максимальную правую позицию
    if (enemyPosition.x + _enemy->getContentSize().width / 2 * _enemy->getScaleX() > _winWidth) {
        enemyPosition.setPoint(_winWidth - _enemy->getScaleX() * _enemy->getContentSize().width / 2, enemyPosition.y);
    }
    else if (enemyPosition.x - _enemy->getContentSize().width / 2 * _enemy->getScaleX() < _origin.x) {
        enemyPosition.setPoint(_origin.x + _enemy->getScaleX() * _enemy->getContentSize().width / 2, enemyPosition.y);
    }

    _enemy->runAction(CCSequence::create(CCMoveTo::create(0.8f, enemyPosition), CCCallFuncN::create(this, callfuncN_selector(HelloWorld::enemyMoveFinished)), NULL));

    //Задержка 
    float delay = 1.0f;

    // Расстояние между лазерными снарядами врага относительно ширины спрайта врага
    float widthAdjustment = 4.0f;

    // Запуск четырех снарядов с двумя наборами задержек и X-координатами в зависимости от ширины вражеского космического корабля
    for (int i = 0; i < 2; ++i) {
        _enemyProjectiles[_enemyProjectileCount]->stopAllActions();
        _enemyProjectiles[_enemyProjectileCount]->setPosition(ccp(_enemy->getPositionX() - _enemy->getScaleX() * _enemy->getContentSize().width / widthAdjustment, _enemy->getPositionY()));
        _enemyProjectiles[_enemyProjectileCount]->setVisible(true);
        _enemyProjectiles[_enemyProjectileCount]->runAction(CCSequence::create(CCMoveTo::create(delay, ccp(_enemy->getPositionX() - _enemy->getScaleX() * _enemy->getContentSize().width / widthAdjustment, _winHeight)), CCCallFuncN::create(this, callfuncN_selector(HelloWorld::spriteMoveFinished)), NULL));

        ++_enemyProjectileCount;

        _enemyProjectiles[_enemyProjectileCount]->stopAllActions();
        _enemyProjectiles[_enemyProjectileCount]->setPosition(ccp(_enemy->getPositionX() + _enemy->getScaleX() * _enemy->getContentSize().width / widthAdjustment, _enemy->getPositionY()));
        _enemyProjectiles[_enemyProjectileCount]->setVisible(true);
        _enemyProjectiles[_enemyProjectileCount]->runAction(CCSequence::create(CCMoveTo::create(delay, ccp(_enemy->getPositionX() + _enemy->getScaleX() * _enemy->getContentSize().width / widthAdjustment, _winHeight)), CCCallFuncN::create(this, callfuncN_selector(HelloWorld::spriteMoveFinished)), NULL));

        ++_enemyProjectileCount;

        // Для следующих двух снарядов
        // Удваиваем задержку, чтобы внутренние лазеры двигались медленнее
        delay += 0.5f;

        // Делаем ширину ближе к центру космического корабля врага
        widthAdjustment *= 3.0f;
    }

    //If reached the maximum number of sprites reset the the count to recycle the sprites
    if (_enemyProjectileCount >= MAX_ENEMY_PROJECTILES) {
        _enemyProjectileCount = 0;
    }
}

void HelloWorld::createParticleEffect(const char* filename, float x, float y) {
    CCParticleSystem* emitter = CCParticleSystemQuad::create(filename);
    emitter->setPosition(x, y);
    emitter->setAutoRemoveOnFinish(true);
    addChild(emitter, 10);
}

void HelloWorld::createParticleEffect(const char* filename, float x, float y, ccColor4F startColor, float duration, float endSize) {
    CCParticleSystem* emitter = CCParticleSystemQuad::create(filename);
    emitter->setPosition(x, y);
    emitter->setStartColor(startColor);
    emitter->setDuration(duration);
    emitter->setEndSize(endSize);
    emitter->setAutoRemoveOnFinish(true);
    addChild(emitter, 10);
}

void HelloWorld::updatePlayerHealthBar() {
    //Масштабируйте шкалу здоровья, разделив текущее здоровье на максимальное.
    float scaleFactor = ((float)_playerHealth / MAX_PLAYER_HEALTH);
    _playerHealthBar->setScaleX(scaleFactor);
    _playerHealthBar->setPosition(ccp(_origin.x + _playerHealthBar->getScaleX() * _playerHealthBar->getContentSize().width / 2, _winHeight - _playerHealthBar->getScaleY() * _playerHealthBar->getContentSize().height));
    // Если здоровье в пределах 40-60%, отобразить желтую полосу
    if (scaleFactor * 10 <= 6 && scaleFactor * 10 >= 4) {
        _playerHealthBar->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("health_bar_yellow.png"));
    }
    else if (scaleFactor * 10 < 4) {// Если здоровье меньше 40%, отображается мигающая красная полоса       
        _playerHealthBar->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("health_bar_red.png"));
        // Создаем мерцающий эффект, указывающий на низкое здоровье
        CCBlink* blinkAction = CCBlink::create(1.0f, 4);
        CCShow* showAction = CCShow::create();
        CCSequence* action = CCSequence::create(blinkAction, showAction, NULL);
        CCRepeatForever* repeatAction = CCRepeatForever::create(action);
        _playerHealthBar->runAction(repeatAction);
    }

}

void HelloWorld::updateEnemyHealthBar() {
    // Масштабирование шкалы здоровья путем деления текущего здоровья на максимальное.
    float scaleFactor = ((float)_enemyHealth / MAX_ENEMY_HEALTH);
    _enemyHealthBar->setScaleX(scaleFactor);
    _enemyHealthBar->setPosition(ccp(_origin.x + _enemyHealthBar->getScaleX() * _enemyHealthBar->getContentSize().width / 2, _origin.y + _enemyHealthBar->getScaleY() * _enemyHealthBar->getContentSize().height));
    // Если здоровье в пределах 40-60%, отобразить желтую полосу
    if (scaleFactor * 10 <= 6 && scaleFactor * 10 >= 4) {
        _enemyHealthBar->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("health_bar_yellow.png"));
    }
    else if (scaleFactor * 10 < 4) {
        _enemyHealthBar->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("health_bar_red.png"));
        // Создаем мерцающий эффект, указывающий на низкое здоровье
        CCBlink* blinkAction = CCBlink::create(1.0f, 4);
        CCShow* showAction = CCShow::create();
        CCSequence* action = CCSequence::create(blinkAction, showAction, NULL);
        CCRepeatForever* repeatAction = CCRepeatForever::create(action);
        _enemyHealthBar->runAction(repeatAction);
    }
}

void HelloWorld::menuRestartCallback(Ref* Scene) {
    CCDirector::sharedDirector()->replaceScene(CCTransitionRotoZoom::create(1.0, this->createScene()));
}

void HelloWorld::endGame(bool won) {
    if (won) {
        _enemyHealthLabel->setVisible(false);
        _enemyHealthBar->setVisible(false);
        _gameResultLabel->setString("YOU WIN!");

    }
    else {
        _playerHealthLabel->setVisible(false);
        _playerHealthBar->setVisible(false);
        _gameResultLabel->setString("GAME OVER");
    }
    _gameResultLabel->setPosition(ccp(_winWidth / 2, _winHeight / 2 - _gameResultLabel->getContentSize().height));
    _gameResultLabel->setVisible(true);
    _gameOver = true;
    _restartMenuItem->setVisible(true);
    _restartMenuItem->setEnabled(true);
}

void HelloWorld::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);


}
