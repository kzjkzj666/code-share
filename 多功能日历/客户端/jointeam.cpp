#include "jointeam.h"
#include "send.h"
#include "ui_jointeam.h"

jointeam::jointeam(QString id,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::jointeam)
{
    ui->setupUi(this);
     setWindowIcon(QIcon(":/res/icon.png"));
        setWindowFlags(windowFlags()&~Qt::WindowMinMaxButtonsHint|Qt::WindowMinimizeButtonHint);
    m_id=id;
    connect(ui->cancel,&QPushButton::clicked,[=]
            {
                emit back("");
            });
    connect(ui->ok,&QPushButton::clicked,[=]
            {
               join();
            });
}
void jointeam::closeEvent(QCloseEvent *)
{
    emit back("");
}
jointeam::~jointeam()
{
    delete ui;
}
void jointeam::join()
{
    if(ui->name->text()=="")
    {
        QMessageBox::information(this,"提示","请输入团队名称");
        return;
    }
    else if(ui->password->text()=="")
    {
        QMessageBox::information(this,"提示","请输入邀请密码");
        return;
    }
    flag=0;
    ui->ok->setText("正在加入，请稍候");
    ui->ok->setDisabled(1);
    QJsonObject obj;
    obj.insert("type","4");//申请加入团队，类型为4
    obj.insert("id",this->m_id);
    obj.insert("name",ui->name->text());
    obj.insert("password",ui->password->text());
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
    connect(m_send, &send::joined, [=]
            {
                flag=4;
            });
     QTimer timer;
    timer.setInterval(5000);  // 设置超时时间5秒
    timer.setSingleShot(true);  // 单次触发
    QEventLoop loop;
    connect(&timer, &QTimer::timeout,&loop,&QEventLoop::quit);
    connect(m_send, &send::id_not_exist, &loop,&QEventLoop::quit);
    connect(m_send, &send::password_error, &loop,&QEventLoop::quit);
    connect(m_send, &send::log_success, &loop,&QEventLoop::quit);
    connect(m_send, &send::joined, &loop,&QEventLoop::quit);
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
        QMessageBox::information(this,"加入失败","团队不存在");
        ui->ok->setText("确定");
        ui->ok->setDisabled(0);
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
        QMessageBox::information(this,"加入失败","密码错误");
        ui->ok->setText("确定");
        ui->ok->setDisabled(0);

     }
     else if(flag==3)
     {
         disconnect(&timer, &QTimer::timeout,&loop,&QEventLoop::quit);
        // 处理响应
         if(timer.isActive())
         {
            timer.stop();
         }


         ui->password->clear();
         ui->ok->setText("确定");
         ui->ok->setDisabled(0);
         flag=0;
          QMessageBox::information(this,"提示！","加入团队成功！");
          emit back(ui->name->text());
           ui->name->clear();
     }
     else if(flag==4)
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
        QMessageBox::information(this,"提示","你已经该在团队中");
        ui->ok->setText("确定");
        ui->ok->setDisabled(0);
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
       QMessageBox::information(this,"加入失败","网络错误，请求超时!");
        ui->ok->setText("确定");
        ui->ok->setDisabled(0);
    }
}
