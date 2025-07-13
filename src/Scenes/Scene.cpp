// src/Scenes/Scene.cpp

#include <QDateTime>
#include "Scene.h"

Scene::Scene(QObject *parent) : QGraphicsScene(parent), timer(new QTimer(this)) {
    connect(timer, &QTimer::timeout, this, &Scene::update);
}

void Scene::update() {
    auto currentTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
    if (lastTime == -1) { // first frame
        deltaTime = 0;
    } else {
        deltaTime = currentTime - lastTime;
    }
    lastTime = currentTime;

    // 更新游戏循环，加入物理处理
    processInput();
    processMovement();

    // 【核心改动】在移动之后，进行物理计算和碰撞修正
    processPhysics();

    processPicking();
    this -> advance();
}

void Scene::startLoop() {
    timer->start(1000 / 90); // 90FPS
}

// 提供各个阶段的默认空实现
void Scene::processInput() {}

void Scene::processMovement() {}

void Scene::processPicking() {}

// 【核心改动】为新的物理阶段提供默认空实现
void Scene::processPhysics() {}
