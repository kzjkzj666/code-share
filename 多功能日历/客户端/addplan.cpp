#include "addplan.h"
#include "qtimer.h"
#include "ui_addplan.h"
#include<qstring.h>
#include<QDir>
addplan::addplan(int type,QString id,QString name,QWidget *parent):
    QDialog(parent),
    ui(new Ui::addplan)
{
    ui->setupUi(this);
      setWindowIcon(QIcon(":/res/icon.png"));
        setWindowFlags(windowFlags()&~Qt::WindowMinMaxButtonsHint|Qt::WindowMinimizeButtonHint);
    this->type=type;//个人0，团队1
    if(type==1)
    {
    ui->label->setText("新建团队日程");
    }
    else
    {
     ui->label->setText("新建个人日程");
    }
    m_id=id;
    this->name=name;
    ui->dateEdit->setDate(QDate::currentDate());
     ui->timeEdit->setTime(QTime::currentTime());
    ui->endtime->setTime(QTime::currentTime().addSecs(3600));
    ui->sure->setFocus();
    connect(ui->sure,&QPushButton::clicked,[=]
    {
        if(ui->timeEdit->time()>=ui->endtime->time())
            {
            QMessageBox::information(this,"提示","日程开始时间应早于结束时间");
        }
        else
        {
        flag=0;
        ui->sure->setText("正在添加");
        ui->sure->setDisabled(1);
        QString title=ui->lineEdit->text();
        QString content=ui->textEdit->toPlainText();
        if(content=="")
        {
            content="无";
        }
        QDate t_date=ui->dateEdit->date();
        QString date=t_date.toString("yyyyMMdd");
        QTime t_time=ui->timeEdit->time();
        QString time=t_time.toString("hhmm");
        QString endtime=ui->endtime->time().toString("hhmm");
        QString privacy;
          privacy="1";
       //1表示团队不可见
        QString place=ui->place_edit->text();
        if(place=="")
        {
            place="无";
        }
        //日程json文件
        QJsonObject obj;
        if(title!="")
        {
            if(this->type==0)//个人日程
            {
            obj.insert("type","6");//json类型，type=6表示个人日程信息
            obj.insert("plan_type","0");//0表示个人日程，1表示团队日程
            obj.insert("id",m_id);
            obj.insert("creat_time",
                       QDate::currentDate().toString("yyyyMMdd")+QTime::currentTime().toString("hhmmss"));
            obj.insert("date",date);
            obj.insert("time",time);
            obj.insert("endtime",endtime);
            obj.insert("privacy",privacy);
            obj.insert("title",title);
            obj.insert("content",content);
            obj.insert("place",place);
            QString filepath= QDir::currentPath()+"/"+m_id+"_plan.json";
            QFile file(filepath);
            //打开文件
            file.open(QIODevice::ReadOnly| QIODevice::Text);
            QByteArray data_user=file.readAll();
            file.close();
            QJsonArray array_user;
            QJsonDocument doc_user=QJsonDocument::fromJson(data_user);
            array_user=doc_user.array();
            array_user.append(obj);
            file.open(QIODevice::WriteOnly| QIODevice::Text);
            file.write(QByteArray(QJsonDocument(array_user).toJson()));
            file.close();
            }
            else if(this->type==1)
            {
            obj.insert("type","8");//json类型，type=8表示团队日程信息
            obj.insert("plan_type","0");//0表示个人日程，1表示团队日程
            obj.insert("name",this->name);//是哪一个团队的日程
            obj.insert("id",m_id);
            obj.insert("creat_time",
                       QDate::currentDate().toString("yyyyMMdd")+QTime::currentTime().toString("hhmmss"));
            obj.insert("date",date);
            obj.insert("time",time);
            obj.insert("endtime",endtime);
            obj.insert("title",title);
            obj.insert("content",content);
            obj.insert("place",place);
            QString filepath= QDir::currentPath()+"/"+name+"_teamplan.json";
            QFile file(filepath);
            //打开文件
            file.open(QIODevice::ReadOnly| QIODevice::Text);
            QByteArray data_user=file.readAll();
            file.close();
            QJsonArray array_user;
            QJsonDocument doc_user=QJsonDocument::fromJson(data_user);
            array_user=doc_user.array();
            array_user.append(obj);
            file.open(QIODevice::WriteOnly| QIODevice::Text);
            file.write(QByteArray(QJsonDocument(array_user).toJson()));
            file.close();
            }

          send*m_send=new send;
          m_send->sendfile(QByteArray(QJsonDocument(obj).toJson()));
            QTimer timer;
            timer.setInterval(5000);  // 设置超时时间5秒
            timer.setSingleShot(true);  // 单次触发
            QEventLoop loop;
            connect(&timer, &QTimer::timeout,&loop, &QEventLoop::quit);
            connect(m_send, &send::get_message, &loop, &QEventLoop::quit);
            connect(m_send, &send::get_message, [=]
                    {
                flag=1;
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
                ui->sure->setDisabled(0);
                ui->sure->setText("确定");
                QMessageBox::information(this,"提示","本地添加成功！同步成功！");
                ui->dateEdit->setDate(QDate::currentDate());
                ui->timeEdit->setTime(QTime::currentTime());
                ui->lineEdit->clear();
                ui->place_edit->clear();
                ui->textEdit->clear();

                emit ok();
            }

            else
            {
                disconnect(m_send, &send::get_message, &loop, &QEventLoop::quit);
                if(timer.isActive())
                {
                    timer.stop();
                }
                if(m_send!=NULL)
                {
                    delete m_send;
                }
                ui->sure->setDisabled(0);
                ui->sure->setText("确定");
                QMessageBox::information(this,"提示","本地添加成功！同步失败！");
                ui->dateEdit->setDate(QDate::currentDate());
                ui->timeEdit->setTime(QTime::currentTime());
                ui->lineEdit->clear();
                ui->place_edit->clear();
                ui->textEdit->clear();

                emit back();
            }
        }

        else
        {
            QMessageBox::information(this,"添加失败","请输入日程标题！");
            ui->sure->setDisabled(0);
            ui->sure->setText("确定");
        }
    }
            });

     connect(ui->cancel,&QPushButton::clicked,[=]
     {
         ui->dateEdit->setDate(QDate::currentDate());
         ui->timeEdit->setTime(QTime::currentTime());
         ui->lineEdit->clear();
         ui->place_edit->clear();
         ui->textEdit->clear();

         back();
     });

}

addplan::~addplan()
{
    delete ui;
}
void addplan::closeEvent(QCloseEvent *)
{
    emit back();
}
