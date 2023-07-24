#include "signup.h"
#include "ui_signup.h"
#include <QTime>
#include <QChar>
#include <QPainter>
#include<QMessageBox>
signup::signup(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::signup)
{

    ui->setupUi(this);
     setWindowIcon(QIcon(":/res/icon.png"));
        setWindowFlags(windowFlags()&~Qt::WindowMinMaxButtonsHint|Qt::WindowMinimizeButtonHint);
    //随机数种子初始化
    srand(QTime(0,0,0).secsTo(QTime::currentTime()));
    //生成随机数
    generate();
    connect(ui->refresh,&QPushButton::clicked,[=]
    {
        //重新生成验证码
        generate();
        //实时更新
        update();
        qDebug()<<this->vercode;
    });
    connect(ui->cancel,&QPushButton::clicked,[=]
            {
             back();

    });
    connect(ui->ok,&QPushButton::clicked,[=]
            {

        regs();
    });
}
void signup::closeEvent(QCloseEvent *)
{
    emit back();
}
signup::~signup()
{
    delete ui;
}


void signup::regs()
{
        flag=0;
        QString id=ui->user_id->text();
        if(id=="")
        {
            QMessageBox::information(this,"注册失败！","请输入用户名");
            generate();
            update();
            return;
        }
        if(id.size()<3||id.size()>9)
            {
            QMessageBox::information(this,"注册失败！","请输入3-9位的用户名");
            generate();
            update();
                return;
        }
        QString password=ui->password->text();
        QString password2=ui->password2->text();
        if(password.size()<3||password.size()>9)
        {
            QMessageBox::information(this,"注册失败！","请输入3-9位的密码");
            generate();
            update();
                return;
        }
        if(password!=password2)
        {
            QMessageBox::information(this,"注册失败！","前后两次输入密码不一致！");
            generate();
            update();
            return;
        }
        QString ver=ui->ver->text();
        if(ver!=this->vercode)
        {
             QMessageBox::information(this,"注册失败！","验证码错误！");
            generate();
            update();
            return;
        }

        ui->ok->setText("正在注册，请稍候");
        ui->ok->setDisabled(1);
        QJsonObject obj;
        obj.insert("type","0");
        obj.insert("id",id);
        obj.insert("password",password);
        QJsonDocument doc(obj);
        QByteArray pack=doc.toJson();
        send *m_send=new send;
         m_send->sendfile(pack);
        QTimer timer;
        timer.setInterval(5000);  // 设置超时时间5秒
        timer.setSingleShot(true);  // 单次触发
        QEventLoop loop;
        connect(&timer, &QTimer::timeout,&loop, &QEventLoop::quit);
        connect(m_send, &send::fail, &loop, &QEventLoop::quit);
        connect(m_send, &send::success, &loop, &QEventLoop::quit);
        connect(m_send, &send::success, [=]
           {
            flag=1;
        });
        connect(m_send, &send::fail, [=]
                {
            flag=2;
                });
        timer.start();
        loop.exec();  // 启动事件循环
        if(flag==1)
        {
            disconnect(&timer, &QTimer::timeout,&loop, &QEventLoop::quit);
            if(timer.isActive())
            {
                timer.stop();
            }
            if(m_send!=NULL)
            {
                 delete m_send;
            }
               QMessageBox::information(this,"提示","注册成功！");
            ui->user_id->clear();
            ui->password->clear();
            ui->password2->clear();
            ui->ver->clear();
            ui->ok->setText("注册");
            ui->ok->setDisabled(0);
            generate();
            update();
            flag=0;

            emit back();
        }
        else if(flag==2)
        {
            disconnect(&timer, &QTimer::timeout,&loop, &QEventLoop::quit);
            if(timer.isActive())
            {
                 timer.stop();
            }
            if(m_send!=NULL)
            {
                 delete m_send;
            }
    QMessageBox::information(this,"注册失败","用户名已存在，请换一个");
            ui->ok->setText("注册");
            ui->ok->setDisabled(0);
            ui->user_id->clear();
            generate();
            update();
            flag=0;

        }
        else if(flag==0)
        {  // 处理超时
            disconnect(m_send, &send::success, &loop, &QEventLoop::quit);
            disconnect(m_send, &send::fail, &loop, &QEventLoop::quit);
            if(m_send!=NULL)
            {
                 delete m_send;
            }
            QMessageBox::information(this,"注册失败","网络错误，请求超时!");
            ui->ok->setText("注册");
            ui->ok->setDisabled(0);
            generate();
            update();
        }
}
void signup::generate()
{
    this->vercode.clear();
    for(int i = 0;i <5;++i)
    {
        int num = rand()%3;
        if(num == 0)
        {
            //数字
            vercode += QString::number(rand()%10);
        }
        else if(num == 1)
        {
            //大写字母
            int temp = 'A';
            vercode += (QChar)(temp + rand()%26);
        }
        else if(num == 2)
        {
            //小写字母
            int temp = 'a';
            vercode += (QChar)(temp + rand()%26);
        }
    }
}

void signup::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QPen pen;
    QFont font("楷体",25,QFont::Bold,true);
    painter.setFont(font);
    //画点
    for(int i=0;i<256;i++)
    {
        pen = QPen(QColor(rand()%256,rand()%256,rand()%256));
        painter.setPen(pen);
        painter.drawPoint(300+rand()%180,180+rand()%80);
    }
    for(int i=0;i<9;i++)

    //画线
    {
        pen = QPen(QColor(rand()%256,rand()%256,rand()%256));
        painter.setPen(pen);
        painter.drawLine(300+rand()%180,180+rand()%80,330+rand()%180,220+rand()%80);
    }
    //绘制验证码
    for(int i=0;i<5;i++)
    {
        pen = QPen(QColor(rand()%255,rand()%255,rand()%255));
        painter.setPen(pen);
        painter.drawText(300+15+35*i,180+10+rand()%15,30,40,Qt::AlignCenter, QString(vercode[i]));
    }
}

