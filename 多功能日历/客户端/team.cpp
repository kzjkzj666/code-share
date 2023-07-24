#include "team.h"
#include "ui_team.h"
#include"addteam.h"
#include"jointeam.h"
#include<QMessageBox>
#include<QJsonObject>
#include<QJsonArray>
#include<QJsonDocument>
#include"send.h"
#include<QDir>
#include<QTimer>
#include"work.h"
team::team(QString id,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::team)
{
    ui->setupUi(this);
     setWindowIcon(QIcon(":/res/icon.png"));
        setWindowFlags(windowFlags()&~Qt::WindowMinMaxButtonsHint|Qt::WindowMinimizeButtonHint);
    this->id=id;
    addteam * atm=new addteam(id);
    connect(ui->creat,&QPushButton::clicked,[=]
            {
        this->hide();
        atm->show();
    });
    connect(atm,&addteam::back,[=](QString name)
        {

            if(name!="")
            {
              ui->comboBox->addItem(name);
                QJsonObject c_obj;
                c_obj.insert("type","12");//2--向服务器发出数据同步请求
                c_obj.insert("id",this->id);
                c_obj.insert("name",ui->comboBox->currentText());
                QJsonDocument c_doc(c_obj);
                QByteArray pack=c_doc.toJson();
                send * m_send=new send;
                m_send->sendfile(pack);
            }
              this->show();
        atm->hide();
    });
    jointeam * jtm=new jointeam(id);
    connect(ui->join,&QPushButton::clicked,[=]
            {
                this->hide();
                jtm->show();
            });
    connect(jtm,&jointeam::back,[=](QString name)
            {
        if(name!="")
        {
            ui->comboBox->addItem(name);
            QJsonObject c_obj;
            c_obj.insert("type","12");//2--向服务器发出数据同步请求
            c_obj.insert("id",this->id);
            c_obj.insert("name",ui->comboBox->currentText());
            QJsonDocument c_doc(c_obj);
            QByteArray pack=c_doc.toJson();
            send * m_send=new send;
            m_send->sendfile(pack);
        }
        this->show();
                jtm->hide();
            });
    refresh();
    connect(ui->enter,&QPushButton::clicked,[=]
            {
        if(ui->comboBox->currentText()!="")
            {

                work * wk=new work(ui->comboBox->currentText(),id);
                this->hide();
                wk->load();
                wk->checkplan();
                wk->show();        
                connect(wk,&work::back,[=]
                        {
                            QJsonObject c_obj;
                            c_obj.insert("type","12");//2--向服务器发出数据同步请求
                            c_obj.insert("id",this->id);
                            c_obj.insert("name",ui->comboBox->currentText());
                            QJsonDocument c_doc(c_obj);
                            QByteArray pack=c_doc.toJson();
                            send * m_send=new send;
                            m_send->sendfile(pack);
                            this->show();
                            wk->hide();
                        });
                    }

    });
}
team::~team()
{
    delete ui;
}
void team::closeEvent(QCloseEvent *)
{
    emit back();
}
void team::refresh()
{
    ui->comboBox->clear();
    QFile file(QDir::currentPath()+"/"+id+"_"+"teamlist.json");
    file.open(QIODeviceBase::ReadOnly);
    QJsonArray arr=QJsonDocument::fromJson(QByteArray(file.readAll())).array();
    QStringList strList;
    for(int i=0;i<arr.size();i++)
    {
       strList<<arr[i].toObject().value("name").toString();
    }
    ui->comboBox->addItems(strList);
}
