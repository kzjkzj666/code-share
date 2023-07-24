#include "addteam.h"
#include "ui_addteam.h"

addteam::addteam(QString id,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::addteam)
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
        creat();
    });

}
void addteam::closeEvent(QCloseEvent *)
{
    emit back("");
}
addteam::~addteam()
{
    delete ui;
}
void  addteam::creat()
{
    if(ui->name->text()=="")
    {
        QMessageBox::information(this,"提示","请输入团队名称");
        return;
    }
    else if(ui->password->text()=="")
    {
        QMessageBox::information(this,"提示","请输入身份验证码");
        return;
    }
    flag=0;
    ui->ok->setText("正在创建，请稍候");
    ui->ok->setDisabled(1);
    QJsonObject obj;
    obj.insert("type","5");//创建团队，类型为5，
    obj.insert("id",this->m_id);
    obj.insert("name",ui->name->text());
    obj.insert("password",ui->password->text());
    QJsonDocument doc(obj);
    QByteArray pack=doc.toJson();
    send *m_send=new send;
    m_send->sendfile(pack);
    QTimer timer;
    timer.setInterval(5000);  // 设置超时时间5秒
    timer.setSingleShot(true);  // 单次触发
    QEventLoop loop;
    connect(&timer, &QTimer::timeout,&loop, &QEventLoop::quit);
    connect(m_send, &send::success, &loop, &QEventLoop::quit);
    connect(m_send, &send::fail, &loop, &QEventLoop::quit);
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
        QMessageBox::information(this,"提示","创建团队成功！");

        ui->password->clear();
        ui->ok->setText("确定");
        ui->ok->setDisabled(0);
        flag=0;
        emit back(ui->name->text());
          ui->name->clear();
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
        QMessageBox::information(this,"创建团队失败","团队名已存在，请换一个");
        ui->ok->setText("确定");
        ui->ok->setDisabled(0);
        ui->name->clear();
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
        QMessageBox::information(this,"创建团队失败","网络错误，请求超时!");
        ui->ok->setText("确定");
        ui->ok->setDisabled(0);
    }

}
