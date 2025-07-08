// src/MyGame.cpp

#include "MyGame.h"
#include "Scenes/BattleScene.h"

MyGame::MyGame(QWidget *parent) : QMainWindow(parent) {
    // 1. 创建你的场景
    battleScene = new BattleScene(this);

    // 2. 创建用于显示场景的视图
    view = new QGraphicsView(this);
    view->setScene(battleScene);

    // [FIXED] 直接设置视图的大小为你想要的游戏分辨率
    // 不要从一个还未完全初始化的场景中读取尺寸
    view->setFixedSize(1280, 720);

    // 关闭滚动条，因为游戏场景大小是固定的
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // 3. 将视图设置为主窗口的中央控件
    setCentralWidget(view);

    // [FIXED] 让主窗口的大小自动调整以紧密包裹中央控件（即我们的视图）
    // 这比 setFixedSize(view->sizeHint()) 更可靠
    adjustSize();
    setFixedSize(size()); // 在调整后，将大小固定下来，防止用户拖动改变窗口大小

    // 4. 启动游戏的主循环
    battleScene->startLoop();
}
