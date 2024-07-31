#ifndef PLAYSCENE_H
#define PLAYSCENE_H

#include <QMainWindow>
#include "mycoin.h"

class PlayScene : public QMainWindow
{
    Q_OBJECT
public:
    //explicit PlayScene(QWidget *parent = nullptr);

    PlayScene(int levelNum);

    int levelIndex ; //内部成员属性 记录所选的关卡

    //重写paintEvent事件
    void paintEvent(QPaintEvent *);

    //二维数组维护每关关卡的具体数据
    int gameArray[4][4];
    MyCoin * coinBtn[4][4];

    //判断是否胜利
    bool isWin=false;

signals:
    void chooseSceneBack();

public slots:
};

#endif // PLAYSCENE_H
