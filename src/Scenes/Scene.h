// src/Scenes/Scene.h

#ifndef QT_PROGRAMMING_2024_SCENE_H
#define QT_PROGRAMMING_2024_SCENE_H

#include <QGraphicsScene>
#include <QTimer>

class Scene : public QGraphicsScene {
    Q_OBJECT

public:
    explicit Scene(QObject *parent);

    void startLoop();

    // 声明游戏循环的各个阶段
    virtual void processInput();
    virtual void processMovement();
    virtual void processPicking();

    // 【核心改动】新增物理处理阶段的虚函数
    virtual void processPhysics();

protected slots:
    // 主更新函数
    virtual void update();

protected:
    qint64 deltaTime{};

private:
    QTimer *timer;
    qint64 lastTime{-1};
};

#endif //QT_PROGRAMMING_2024_SCENE_H
