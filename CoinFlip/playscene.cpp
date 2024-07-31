#include "playscene.h"
#include <QDebug>
#include <QMenuBar>
#include <QPainter>
#include "mypushbutton.h"
#include <QTimer>
#include <QLabel>
#include "mycoin.h"
#include "dataconfig.h"
#include <QPropertyAnimation>

//PlayScene::PlayScene(QWidget *parent) : QMainWindow(parent)
//{

//}

PlayScene::PlayScene(int levelNum)
{
    QString str = QString("进入了第 %1 关 ").arg(levelNum);
    qDebug() << str;
    this->levelIndex = levelNum;

    //初始化游戏场景
    //设置固定大小
    this->setFixedSize(320,588);
    //设置图标
    this->setWindowIcon(QPixmap(":/res/Coin0001.png"));
    //设置标题
    this->setWindowTitle("翻金币场景");

    //创建菜单栏
    QMenuBar * bar = menuBar();
    setMenuBar(bar);

    //创建开始菜单
    QMenu * startMenu = bar->addMenu("开始");

    //创建退出 菜单项
    QAction *  quitAction = startMenu->addAction("退出");

    //点击退出 实现退出游戏
    connect(quitAction,&QAction::triggered,[=](){
        this->close();
    });

    //返回按钮
    MyPushButton * backBtn = new MyPushButton(":/res/BackButton.png" , ":/res/BackButtonSelected.png");
    backBtn->setParent(this);
    backBtn->move(this->width() - backBtn->width() , this->height() - backBtn->height());

    //点击返回
    connect(backBtn,&MyPushButton::clicked,[=](){
        qDebug() << "翻金币场景中：点击了返回按钮";

        QTimer::singleShot(500,this,[=](){
            emit this->chooseSceneBack();
        });
    });

    //显示当前关卡数
    QLabel * label = new QLabel;
    label->setParent(this);
    QFont font;
    font.setFamily("华文新魏");
    font.setPointSize(20);
    QString str1 = QString("Level: %1").arg(this->levelIndex);
    //将字体设置到标签控件中
    label->setFont(font);
    label->setText( str1);
    label->setGeometry(30, this->height() - 50,130, 50);

    //初始化每个关卡的二维数组
    dataConfig config;
    for(int i=0;i<4;i++)
    {
        for(int j=0;j<4;j++)
        {
            this->gameArray[i][j]=config.mData[this->levelIndex][i][j];
        }
    }

    //胜利图片显示 默认在背景外
    QLabel * winLabel = new QLabel;
    QPixmap tmpPix;
    tmpPix.load(":/res/LevelCompletedDialogBg.png");
    winLabel->setPixmap(tmpPix);
    winLabel->setParent(this);
    winLabel->move((this->width() - tmpPix.width()) / 2, -400);

    //显示金币的背景图
    for(int i=0;i<4;i++)
    {
        for(int j=0;j<4;j++)
        {
            //绘制背景图片
            QPixmap pix=QPixmap(":/res/BoardNode.png");
            QLabel *label=new QLabel;
            label->setGeometry(0,0,50,50);
            label->setPixmap(pix);
            label->setParent(this);
            label->move(57+i*50,200+j*50);

            //创建金币
            QString str;
            if(this->gameArray[i][j]==1)
            {
                str=":/res/Coin0001.png";
            }
            else
            {
                str=":/res/Coin0008.png";
            }

            MyCoin *coin=new MyCoin(str);
            coin->setParent(this);
            coin->move(59+i*50,204+j*50);

            //给金币的属性赋值
            coin->posX=i;
            coin->posY=j;
            coin->flag=this->gameArray[i][j]; //  1正面 0反面
            //将金币放入到金币的二维数组里面
            coinBtn[i][j]=coin;

            //点击金币进行翻转
            connect(coin,&MyCoin::clicked,[=](){
                //在翻转金币的时候禁用其他按钮
                for(int i = 0 ; i < 4; i++)
                {
                    for(int j = 0 ; j < 4 ; j++)
                    {
                        this->coinBtn[i][j]->isWin = true;
                    }
                }

                coin->changeFlag();
                this->gameArray[i][j]=this->gameArray[i][j]==0?1:0;

                QTimer::singleShot(100,this,[=](){
                    // 翻转周围硬币的操作，延时翻转
                    if(coin->posX+1<=3)
                    {// 周围的右侧硬币翻转的条件
                        coinBtn[coin->posX+1][coin->posY]->changeFlag();
                        this->gameArray[coin->posX+1][coin->posY]=this->gameArray[coin->posX+1][coin->posY]==0?1:0;

                    }
                    if(coin->posX-1>=0) // 周围左侧硬币的翻转条件
                    {
                        coinBtn[coin->posX-1][coin->posY]->changeFlag();
                        this->gameArray[coin->posX-1][coin->posY]=this->gameArray[coin->posX-1][coin->posY]==0?1:0;
                    }
                    if(coin->posY+1<=3) // 周围下侧硬币的翻转条件
                    {
                        coinBtn[coin->posX][coin->posY+1]->changeFlag();
                        this->gameArray[coin->posX][coin->posY+1]=this->gameArray[coin->posX][coin->posY+1]==0?1:0;
                    }
                    if(coin->posY-1>=0) // 周围上侧硬币的翻转条件
                    {
                        coinBtn[coin->posX][coin->posY-1]->changeFlag();
                        this->gameArray[coin->posX][coin->posY-1]=this->gameArray[coin->posX][coin->posY-1]==0?1:0;
                    }

                    //在翻转完金币的时候释放其他按钮
                    for(int i = 0 ; i < 4; i++)
                    {
                        for(int j = 0 ; j < 4 ; j++)
                        {
                            this->coinBtn[i][j]->isWin = false;
                        }
                    }

                    //判断是否胜利
                    this->isWin=true;
                    for(int i=0;i<4;i++)
                    {
                        for(int j=0;j<4;j++){
                            if(coinBtn[i][j]->flag==false){
                                this->isWin=false;
                                break;
                            }
                        }
                    }
                    if(this->isWin==true){
                        //this->backBtn->m_flag = true;
                        //胜利了
                        qDebug() << "胜利了";
                        //将所有按钮胜利的标志改为 true,如果再次点击按钮直接return
                        for(int i=0;i<4;i++)
                        {
                            for(int j=0;j<4;j++){
                                coinBtn[i][j]->isWin=true;
                            }
                        }
                        //将胜利图片移动下来
                        QPropertyAnimation * an = new QPropertyAnimation(winLabel,"geometry");
                        //设置时间间隔
                        an->setDuration(1000);
                        //设置开始位置
                        an->setStartValue(QRect(winLabel->x(),winLabel->y(),winLabel->width(),winLabel->height()));
                        //设置结束位置
                        an->setEndValue(QRect(winLabel->x(),winLabel->y() + 300,winLabel->width(),winLabel->height()));
                        //设置缓和曲线
                        an->setEasingCurve(QEasingCurve::OutBounce);
                        //执行动画
                        an->start();

//                        QSound * soud1 = new QSound(":/res/LevelWinSound.wav",this);
//                        soud1->play();

//                        QTimer::singleShot(3000,[=](){
//                            this->backBtn->m_flag = false;
//                            //发送一个申请下一关的信号
//                            soud1->stop();
//                            qDebug() << "下一关啦！";
//                            emit gotoNext(m_levelIndex);
//                        });
                    }
                });
            });
        }
    }
}

void PlayScene::paintEvent(QPaintEvent *)
{
    //创建背景
    QPainter painter(this);
    QPixmap pix;
    pix.load(":/res/PlayLevelSceneBg.png");
    painter.drawPixmap(0,0,this->width(),this->height(),pix);

    //加载标题
    pix.load(":/res/Title.png");
    pix = pix.scaled(pix.width()*0.5,pix.height()*0.5);
    painter.drawPixmap( 10,30,pix.width(),pix.height(),pix);
}
