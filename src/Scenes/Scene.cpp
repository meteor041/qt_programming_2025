// src/Scenes/Scene.cpp

#include <QDateTime>
#include "Scene.h"

Scene::Scene(QObject *parent) : QGraphicsScene(parent), timer(new QTimer(this)) {
    connect(timer, &QTimer::timeout, this, &Scene::update);
}

void Scene::update() {
    auto currentTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
    if (lastTime == -1) {
        deltaTime = 0;
    } else {
        deltaTime = currentTime - lastTime;
    }
    lastTime = currentTime;

    // [FIXED] 更新游戏循环以匹配新的函数名和逻辑顺序
    // 1. 物理计算先行，确定环境状态（如是否在地面）
    processPhysics();
    // 2. 角色根据物理结果进行更新（移动、动画）
    processCharacterUpdates();
    // 3. 处理拾取等交互
    processPicking();
    // processInput() 留空，因为输入在 BattleScene 的 key events 中处理
    processInput();
}

void Scene::startLoop() {
    timer->start(1000 / 90); // 90FPS
}

// 提供各个阶段的默认空实现
void Scene::processInput() {}
void Scene::processPicking() {}
void Scene::processPhysics() {}

// [FIXED] 提供重命名后函数的空实现
void Scene::processCharacterUpdates() {}
