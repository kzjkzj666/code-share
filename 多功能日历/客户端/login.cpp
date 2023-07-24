#include "login.h"
#include "ui_login.h"
#include"signup.h"
#include"mainwindow.h"
#include<QDir>
#include<QMovie>
login::login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::login)
{
    ui->setupUi(this);
     setWindowIcon(QIcon(":/res/icon.png"));
        setWindowFlags(windowFlags()&~Qt::WindowMinMaxButtonsHint|Qt::WindowMinimizeButtonHint);
//    ui->bg->setScaledContents(true);
//     QMovie *iconShow = new QMovie(":/res/bg.gif");
//     ui->bg->setMovie(iconShow);
//     iconShow->start();
    ui->id->setFocus();
    ui->log->setDefault(true);
    ui->password->installEventFilter(this);//设置完后自动调用其eventFilter函数
    ui->id->installEventFilter(this);
    signup* sg=new signup;
    connect(ui->signup,&QPushButton::clicked,[=]
            {
        this->hide();
        sg->show();
            });
    connect(sg,&signup::back,[=]
            {
                sg->hide();
                this->show();
            });

    connect(ui->log,&QPushButton::clicked,[=]
            {
                log();
            });
}
void login::log()
{
    flag=0;
    QString id=ui->id->text();
    QString password=ui->password->text();
    if(id=="")
    {
        QMessageBox::information(this,"提示","请输入用户名");
        return;
    }
    if(password=="")
    {
        QMessageBox::information(this,"提示","请输入密码");
        return;
    }
    ui->log->setText("正在登录，请稍候");
    ui->log->setDisabled(1);
    QJsonObject obj;
    obj.insert("type","1");
    obj.insert("id",id);
    obj.insert("password",password);
    QJsonDocument doc(obj);
    QByteArray pack=doc.toJson();
    send *m_send=new send;
    m_send->sendfile(pack);
    connect(m_send, &send::id_not_exist, [=]
            {
              flag=1;
            });
    connect(m_send, &send::password_error, [=]
            {
                flag=2;
            });
    connect(m_send, &send::log_success, [=]
            {
                flag=3;
            });
     QTimer timer;
    timer.setInterval(3000);  // 设置超时时间5秒
    timer.setSingleShot(true);  // 单次触发
    QEventLoop loop;
    connect(&timer, &QTimer::timeout,&loop,&QEventLoop::quit);
    connect(m_send, &send::id_not_exist, &loop,&QEventLoop::quit);
    connect(m_send, &send::password_error, &loop,&QEventLoop::quit);
    connect(m_send, &send::log_success, &loop,&QEventLoop::quit);
       timer.start();
       loop.exec();
     if(flag==1)
     {
        disconnect(&timer, &QTimer::timeout,&loop,&QEventLoop::quit);
        if(timer.isActive())
        {
            timer.stop();
        }
        if(m_send!=NULL)
        {
            delete m_send;
        }

        flag=0;
        QMessageBox::information(this,"登录失败","用户名不存在");
        ui->log->setText("登录");
        ui->log->setDisabled(0);
     }
     else if(flag==2)
     {
        disconnect(&timer, &QTimer::timeout,&loop,&QEventLoop::quit);
        if(timer.isActive())
        {
            timer.stop();
        }
        if(m_send!=NULL)
        {
            delete m_send;
        }

         flag=0;
        QMessageBox::information(this,"登录失败","密码错误");
        ui->log->setText("登录");
        ui->log->setDisabled(0);

     }
     else if(flag==3)
     {
         disconnect(&timer, &QTimer::timeout,&loop,&QEventLoop::quit);
        // 处理响应
         if(timer.isActive())
         {
            timer.stop();
         }
         if(m_send!=NULL)
         {
            delete m_send;
         }
         ui->id->clear();
         ui->password->clear();
         ui->log->setText("登录");
         ui->log->setDisabled(0);
        w=new MainWindow(id);
        this->hide();
        w->show();
        flag=0;
        connect(w,&MainWindow::back,[=]
                {
                    w->hide();
                    this->show();
                });
     }
     else if(flag==0)
    {
        if(timer.isActive())
        {
            timer.stop();
        }
        if(m_send!=NULL)
        {
            delete m_send;
        }
        disconnect(m_send, &send::id_not_exist, &loop,&QEventLoop::quit);
        disconnect(m_send, &send::password_error, &loop,&QEventLoop::quit);
        disconnect(m_send, &send::log_success, &loop,&QEventLoop::quit);
    send *m_send1=new send;
    m_send1->sendfile(pack);
    connect(m_send1, &send::id_not_exist, [=]
            {
              flag=1;
            });
    connect(m_send1, &send::password_error, [=]
            {
                flag=2;
            });
    connect(m_send1, &send::log_success, [=]
            {
                flag=3;
            });
     QTimer timer1;
    timer1.setInterval(3000);  // 设置超时时间5秒
    timer1.setSingleShot(true);  // 单次触发
    QEventLoop loop1;
    connect(&timer1, &QTimer::timeout,&loop1,&QEventLoop::quit);
    connect(m_send1, &send::id_not_exist, &loop1,&QEventLoop::quit);
    connect(m_send1, &send::password_error, &loop1,&QEventLoop::quit);
    connect(m_send1, &send::log_success, &loop1,&QEventLoop::quit);
       timer1.start();
       loop1.exec();
     if(flag==1)
     {
        disconnect(&timer1, &QTimer::timeout,&loop1,&QEventLoop::quit);
        if(timer1.isActive())
        {
            timer1.stop();
        }
        if(m_send1!=NULL)
        {
            delete m_send1;
        }
        flag=0;
        QMessageBox::information(this,"登录失败","用户名不存在");
        ui->log->setText("登录");
        ui->log->setDisabled(0);
     }
     else if(flag==2)
     {
        disconnect(&timer1, &QTimer::timeout,&loop1,&QEventLoop::quit);
        if(timer1.isActive())
        {
            timer1.stop();
        }
        if(m_send1!=NULL)
        {
            delete m_send1;
        }

         flag=0;
        QMessageBox::information(this,"登录失败","密码错误");
        ui->log->setText("登录");
        ui->log->setDisabled(0);

     }
     else if(flag==3)
     {
         disconnect(&timer1, &QTimer::timeout,&loop1,&QEventLoop::quit);
        // 处理响应
         if(timer1.isActive())
         {
            timer1.stop();
         }
         if(m_send1!=NULL)
         {
            delete m_send1;
         }
         ui->id->clear();
         ui->password->clear();
         ui->log->setText("登录");
         ui->log->setDisabled(0);
        w=new MainWindow(id);
        this->hide();
        w->show();
        flag=0;
        connect(w,&MainWindow::back,[=]
                {
                    w->hide();
                    this->show();
                });
     }
     else if(flag==0)
    {
        if(timer1.isActive())
        {
            timer1.stop();
        }
        if(m_send1!=NULL)
        {
            delete m_send1;
        }
        disconnect(m_send1, &send::id_not_exist, &loop1,&QEventLoop::quit);
        disconnect(m_send1, &send::password_error, &loop1,&QEventLoop::quit);
        disconnect(m_send1, &send::log_success, &loop1,&QEventLoop::quit);
       QMessageBox::information(this,"登录失败","网络错误，请求超时!");
        ui->log->setText("登录");
        ui->log->setDisabled(0);
    }
}
}
login::~login()
{
    delete ui;
}
bool login::eventFilter(QObject *target, QEvent *event)
{
    if(target == ui->password||target==ui->id)
    {
        if(event->type() == QEvent::KeyPress)//回车键
        {
            QKeyEvent *k = static_cast<QKeyEvent *>(event);
            if(k->key() == Qt::Key_Return)
            {
                if(ui->password->text()=="")
                {
                    ui->password->setFocus();
                }
                else
                {
                    ui->log->setFocus();
                log();//需要响应的函数事件
                 return true;

                }

            }
        }
    }
    return QWidget::eventFilter(target,event);
}




