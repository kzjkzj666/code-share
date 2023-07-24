#include "search.h"
#include "qmessagebox.h"
#include "ui_search.h"
#include<QDate>
#include<QTime>
search::search(QString name,QString id,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::search)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/res/icon.png"));
        setWindowFlags(windowFlags()&~Qt::WindowMinMaxButtonsHint|Qt::WindowMinimizeButtonHint);
    m_id=id;
    this->name=name;
    ui->date->setDate(QDate::currentDate());
    ui->time->setTime(QTime::currentTime());
    ui->endtime->setTime(QTime::currentTime().addSecs(3600));
    load();
    connect(ui->back,&QPushButton::clicked,[=]
            {
                emit back();
            });
    connect(ui->ok,&QPushButton::clicked,[=]
            {
        ui->view1->clear();
        QTime time=ui->time->time();
         QTime endtime=ui->endtime->time();
        QDate date=ui->date->date();
         if(time>=endtime)
             {
            QMessageBox::information(this,"提示","开始时间晚于结束时间，请正确输入!");
        }
         else
            {
         int flag=0,flag_t=0;
        for(int i=0;i<arr.size();i++)
            {
             if(arr[i].date==date&&!(arr[i].endtime<time||arr[i].time>endtime))
                {
                if(flag==0)
                    {
                          ui->view1->append(date.toString("MM月dd日")+"所选时间段已有日程安排用户:");
                    }
                ui->view1->append(arr[i].id);
                    flag++;
            }
        }

        for(int i=0;i<arr_team.size();i++)
        {
            if(arr_team[i].endtime.minute()==0)
            {
                    arr_team[i].endtime=arr_team[i].endtime.addSecs(-60);
            }
            if(arr_team[i].time.minute()==0)
            {
                    arr_team[i].time=arr_team[i].time.addSecs(60);
            }
                if(arr_team[i].date==date&&!(arr_team[i].endtime<=time||arr_team[i].time>=endtime))
                {

                        ui->view1->append(date.toString("MM月dd日")+"所选时间段已有团队日程安排");
                        ui->view1->append("日程名称: "+arr_team[i].title);
                        ui->view1->append("日期: "+arr_team[i].date.toString("yyyy年MM月dd日"));
                        ui->view1->append("时间: "+arr_team[i].time.toString("hh:mm")+"~"+arr_team[i].endtime.toString("hh:mm"));
                        ui->view1->append("日程地点: "+arr_team[i].place);
                      ui->view1->append("发起人: "+arr_team[i].id);
                        ui->view1->append("日程内容: "+arr_team[i].content);
                        flag_t++;
                        break;
                }
        }

        if(flag==0&&flag_t==0)
        {
            ui->view1->append(date.toString("MM月dd日")+"所选时间段所有人均空闲");
        }
        else if(flag!=0&&flag_t==0)
        {
            ui->view1->append(QString::number(flag)+"人已有日程安排");
        }
//        if(flag==0)
//        {
//            ui->view1->append(date.toString("MM月dd日")+"所选时间段所有人均空闲");
//        }
//      else
//        {
//            ui->view1->append(QString::number(flag)+"人已有日程安排");
//        }
    }

    });
}
void search::closeEvent(QCloseEvent *)
{
    emit back();
}
search::~search()
{
    delete ui;
}
void search::load()
{

    QFile f2(QDir::currentPath()+"/"+m_id+"_teamlist.json");
    f2.open(QIODeviceBase::ReadOnly);
    QJsonArray b=QJsonDocument::fromJson(f2.readAll()).array();
    f2.close();
    for(int i=0;i<b.size();i++)
    {
        if(b[i].toObject().value("name").toString()==this->name)
        {
            this->member=b[i].toObject().value("member").toArray();
        }
    }

    QFile file(QDir::currentPath()+"/"+ "user_plan_sum.json");
    file.open(QIODeviceBase::ReadOnly);
    QJsonArray a=QJsonDocument::fromJson(file.readAll()).array();
    file.close();
    for(int i=0;i<a.size();i++)
    {
        m_plan p;
        int flag=0;
        p.id=a[i].toObject().value("id").toString();
        for(int j=0;j<member.size();j++)
        {
            if(p.id==member[j].toString())
            {
                flag=1;
                break;
            }
        }
        if(flag==0)
        {
            continue;
        }
        p.date=QDate::fromString(a[i].toObject().value("date").toString(),"yyyyMMdd");
        p.time=QTime::fromString(a[i].toObject().value("time").toString(),"hhmm");
        p.endtime=QTime::fromString(a[i].toObject().value("endtime").toString(),"hhmm");
        arr.push_back(p);
    }
    QFile f1(QDir::currentPath()+"/"+ this->name+"_teamplan.json");
    f1.open(QIODeviceBase::ReadOnly);
    QJsonArray c=QJsonDocument::fromJson(f1.readAll()).array();
    f1.close();
    for(int i=0;i<c.size();i++)
    {
        m_plan_t p;
        p.date=QDate::fromString(c[i].toObject().value("date").toString(),"yyyyMMdd");
        p.time=QTime::fromString(c[i].toObject().value("time").toString(),"hhmm");
        p.endtime=QTime::fromString(c[i].toObject().value("endtime").toString(),"hhmm");
        p.id=c[i].toObject().value("id").toString();
        p.place=c[i].toObject().value("place").toString();
        p.title=c[i].toObject().value("title").toString();
        p.content=c[i].toObject().value("content").toString();
        arr_team.push_back(p);
    }
}
