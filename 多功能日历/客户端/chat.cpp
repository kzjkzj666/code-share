#include "chat.h"
#include "send.h"
#include "ui_chat.h"
#include<QTimer>
chat::chat(QString id,QString name,QJsonArray member,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::chat)
{
    ui->setupUi(this);
     setWindowIcon(QIcon(":/res/icon.png"));
        setWindowFlags(windowFlags()&~Qt::WindowMinMaxButtonsHint|Qt::WindowMinimizeButtonHint);
    this->member=member;
    ui->name->setText(name);
    this->setWindowTitle(name);
    ui->all->addItem("所有成员：");
    //构造函数中对其调用
    ui->send->setFocus();
    ui->send->setDefault(true);
    ui->input->installEventFilter(this);//设置完后自动调用其eventFilter函数
    for(int i=0;i<member.size();i++)
    {
         ui->all->addItem(member[i].toString());
    }
    connect(ui->close,&QPushButton::clicked,[=]
            {
        if(sock!=NULL)
            {
            delete sock;
        }
        emit back();
    });
    this->id=id;
    sock=new QTcpSocket;
    sock->connectToHost(QHostAddress(IP), PORT_CHAT);
    QJsonObject obj;
    obj.insert("type","0");
    obj.insert("name",name);
    obj.insert("id",id);
   QTimer* t_time = new QTimer(this);
    t_time->setSingleShot(1);
    t_time->start(500);//设置时间间隔为1秒
    connect(t_time, &QTimer::timeout,[=]
            {
                sock->write(QJsonDocument(obj).toJson());
            }
            );
    connect(sock,&QIODevice::readyRead,this,[=]
            {
        hear();
            });
    connect(ui->send,&QPushButton::clicked,[=]
            {
        if(ui->input->toPlainText()!="")
        {
            send();
        }
    });
}
void chat::closeEvent(QCloseEvent *)
{
    if(this->sock!=NULL)
    {
         delete sock;
    }
    emit back();
}
chat::~chat()
{
    delete ui;
}
void chat::hear()
{
    QJsonObject obj=QJsonDocument::fromJson(sock->readAll()).object();
    if(obj.value("type")=="1")
    {
         ui->listWidget->clear();
        QJsonArray arr=obj.value("arr").toArray();
        ui->listWidget->clear();
        ui->label->setText("群成员 "+QString::number(arr.size())+"/"+QString::number(member.size()));
        ui->listWidget->addItem("当前在线：");
        for(int i=0;i<arr.size();i++)
        {       
            ui->listWidget->addItem(arr[i].toString());
        }
    }
    else if(obj.value("type")=="2")
    {
        ui->message->append(obj.value("id").toString()+": "+obj.value("text").toString());
    }
}

void chat::send()
{
    QString text=ui->input->toPlainText();
    QJsonObject obj;
    obj.insert("type","2");//2消息
    obj.insert("text",text);
    obj.insert("id",id);
    sock->write(QJsonDocument(obj).toJson());
    ui->input->clear();
}
bool chat::eventFilter(QObject *target, QEvent *event)
{
    if(target == ui->input)
    {
        if(event->type() == QEvent::KeyPress)//回车键
        {
            QKeyEvent *k = static_cast<QKeyEvent *>(event);
            if(k->key() == Qt::Key_Return)
            {

                send();//替换为需要响应的函数事件，以这里的按钮为例
                return true;
            }
        }
    }
    return QWidget::eventFilter(target,event);
}




