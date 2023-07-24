#include "work.h"
#include "qtimer.h"
#include "ui_work.h"
#include"search.h"
#include"addplan.h"
#include<chat.h>
work::work(QString name,QString id,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::work)
{
    ui->setupUi(this);
      setWindowIcon(QIcon(":/res/icon.png"));
        setWindowFlags(windowFlags()&~Qt::WindowMinMaxButtonsHint|Qt::WindowMinimizeButtonHint);
    this->name=name;
    this->m_id=id;

    flag_link=0;
                ui->calendar->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->calendar->setSelectionMode(QAbstractItemView::NoSelection);
    ui->team->setText("当前团队: "+this->name);
                connect(ui->team,&QPushButton::clicked,[=]
                   {
        QDialog * dia=new QDialog;
        dia->setWindowTitle(this->name+"-团队详情");
        dia->setModal(1);
        QTextBrowser * bro=new QTextBrowser(dia);
        dia->setFixedSize(300,300);
        bro->setFixedSize(300,300);
        bro->move(0,0);
        bro->append("团队: "+name);
        bro->append("创建者: "+member[0].toString());
        bro->append("成员列表:");
        for(int i=0;i<member.size();i++)
        {
            bro->append(member[i].toString());
        }
        dia->show();
        });
    connect(ui->calendar,&QTableWidget::itemClicked,[this](QTableWidgetItem *item)
                        {
                            ui->view->clear();
                            QDate day_select=this->date.addDays(item->column());
                            int hour_select=item->row()+7;
                            int flag=0,flag_t=0;
                            for(int i=0;i<arr.size();i++)
                            {
                                QTime t=arr[i].endtime;
                                if(t.minute()==0)t=t.addSecs(-62);
                                if(arr[i].date==day_select&&!(t.hour()<=hour_select-1||arr[i].time.hour()>=hour_select+1))
                                {
                                    if(flag==0)
                                        ui->view->append(day_select.toString("MM月dd日")+"所选时间段已有日程安排用户: ");

                          ui->view->append(arr[i].id);
                           flag++;

                }
        }
        for(int i=0;i<arr_team.size();i++)
        {
                QTime t=arr_team[i].endtime;
                if(t.minute()==0)t=t.addSecs(-62);
                if(arr_team[i].date==day_select&&!(t.hour()<=hour_select-1||arr_team[i].time.hour()>=hour_select+1))
                {
                        ui->view->append(day_select.toString("MM月dd日")+"所选时间段已有团队日程安排");
                        ui->view->append("日程名称: "+arr_team[i].title);
                        ui->view->append("日期: "+arr_team[i].date.toString("yyyy年MM月dd日"));
                        ui->view->append("时间: "+arr_team[i].time.toString("hh:mm")+"~"+arr_team[i].endtime.toString("hh:mm"));
                        ui->view->append("日程地点: "+arr_team[i].place);
                        ui->view->append("发起人: "+arr_team[i].id);
                        ui->view->append("日程内容: "+arr_team[i].content);
                        flag_t++;
                        break;
                }
        }
        if(flag==0&&flag_t==0)
        {
            ui->view->append(day_select.toString("MM月dd日")+"所选时间段所有人均空闲");
        }
        else if(flag!=0&&flag_t==0)
        {
            ui->view->append(QString::number(flag)+"人已有日程安排");
        }
            });
    load();
    QDate date_now=QDate::currentDate();
    int day=date_now.dayOfWeek();
    this->date=date_now.addDays(1-day);
    checkplan();
    connect(ui->btn_last,&QPushButton::clicked,[=]
            {
                date=date.addDays(-7);
        load();
                checkplan();
            });
    connect(ui->btn_next,&QPushButton::clicked,[=]
            {
                date=date.addDays(7);
        load();
                checkplan();
            });
    connect(ui->search,&QPushButton::clicked,[=]
            {
        search * sch=new search(name,id);
        sch->show();
        ui->search->setDisabled(1);
        connect(sch,&search::back,[=]
                {
            if(sch!=NULL)
                {
                     sch->hide();
            }
                    ui->search->setEnabled(1);
                });
    });
    connect(ui->addplan,&QPushButton::clicked,[=]
            {
        if(id==member[0].toString())
            {
        this->hide();
        addplan * add=new addplan(1,m_id,name);
        add->show();
        connect(add,&addplan::ok,[=]
                {
            QJsonObject c_obj;
            c_obj.insert("type","12");//2--向服务器发出数据同步请求
            c_obj.insert("id",id);
            c_obj.insert("name",name);
            QJsonDocument c_doc(c_obj);
            QByteArray pack=c_doc.toJson();
            send * m_send=new send;
            m_send->sendfile(pack);
            QTimer * t_time1 = new QTimer(this);
            t_time1->setSingleShot(1);
            t_time1->start(350);//设置时间间隔为1秒
            connect(t_time1, &QTimer::timeout,[=]
                    {
                        load();
                        checkplan();
                    }
                    );
            this->show();
            add->hide();
        });
        connect(add,&addplan::back,[=]
                {
            this->show();
            add->hide();
        });
        }
        else
        {
        QMessageBox::information(this,"提示","您不是团队创建者,没有添加团队日程的权限");
        }
    });
    connect(ui->link,&QPushButton::clicked,[=]
            {
                qDebug()<<"io";
                QJsonObject c_obj;
                c_obj.insert("type","12");//2--向服务器发出数据同步请求
                c_obj.insert("id",id);
                c_obj.insert("name",name);
                QJsonDocument c_doc(c_obj);
                QByteArray pack=c_doc.toJson();
                send * m_send=new send;
                m_send->sendfile(pack);
                QTimer * t_time1 = new QTimer(this);
                t_time1->setSingleShot(1);
                t_time1->start(350);//设置时间间隔为1秒
                ui->link->setText("正在同步");
                connect(t_time1, &QTimer::timeout,[=]
                        {
                            load();
                            checkplan();
                            ui->link->setText("数据同步");
                        }
                        );
            });
    connect(ui->chat,&QPushButton::clicked,[=]
            {
        this->hide();
        chat * ch=new chat(m_id,name,member);
        ch->show();
        connect(ch,&chat::back,[=]
                 {
            this->show();
            ch->hide();
        });
    });
//    connect(ui->link,&QPushButton::clicked,[=]
//            {
//    flag_link=0;
//    ui->link->setText("正在同步");
//    ui->link->setDisabled(1);
//    QJsonObject c_obj;
//    c_obj.insert("type","2");//2--向服务器发出数据同步请求
//    c_obj.insert("id",m_id);
//    c_obj.insert("name",this->name);
//    QJsonDocument c_doc(c_obj);
//    QByteArray pack=c_doc.toJson();
//    send * m_send=new send;
//    m_send->sendfile(pack);
//    QTimer timer;
//    timer.setInterval(5000);  // 设置超时时间5秒
//    timer.setSingleShot(true);  // 单次触发
//    QEventLoop loop;
//    connect(&timer, &QTimer::timeout,&loop, &QEventLoop::quit);
//    connect(m_send, &send::get_message, &loop, &QEventLoop::quit);
//    connect(m_send, &send::get_message, [=]
//            () mutable{
//                flag_link=1;
//            });
//    timer.start();
//    loop.exec();  // 启动事件循环
//    if(flag_link==1)
//    {
//        disconnect(&timer, &QTimer::timeout,&loop, &QEventLoop::quit);
//        if(timer.isActive())
//        {
//            timer.stop();
//        }
//        if(m_send!=NULL)
//        {
//            delete m_send;
//        }
//        ui->link->setDisabled(0);
//        ui->link->setText("数据同步");
//        QMessageBox::information(this,"提示","数据同步成功！");
//    }
//    else
//    {
//        disconnect(m_send, &send::get_message, &loop, &QEventLoop::quit);
//        if(timer.isActive())
//        {
//            timer.stop();
//        }
//        if(m_send!=NULL)
//        {
//            delete m_send;
//        }
//        ui->link->setDisabled(0);
//        ui->link->setText("数据同步");
//        QMessageBox::information(this,"数据同步失败","网络连接超时！");
//    }
//    });

}
void work::closeEvent(QCloseEvent *)
{
    emit back();
}
work::~work()
{
    delete ui;
}
void work::load()
{
    arr.clear();
    arr_team.clear();
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
        plan p;
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
        plan_t p;
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
void work::checkplan()
{
    ui->range->setText(date.toString("yyyy年MM月dd日")+"----"+date.addDays(6).toString(("yyyy年MM月dd日")));
    for(int i=1;i<=7;i++)
    {
        ui->calendar->item(0,i-1)->setText(date.addDays(i-1).toString("     MM-dd"));
    }
    for(int i=1;i<=14;i++)
    {
        for(int j=0;j<=6;j++)
        {
          ui->calendar->item(i,j)->setBackground(QBrush(QColor(119, 163, 100)));
        }
    }
    for(int i=0;i<arr.size();i++)
    {
        if(arr[i].date>=date&&arr[i].date<=date.addDays(6))
        {
          int day=arr[i].date.dayOfWeek();
          int num1=arr[i].time.hour();
          int num2=arr[i].endtime.hour();
          if(arr[i].endtime.minute()==0)
          {
                num2--;
          }
          if(!(num1<8||num1>=22||num2<8||num2>=22||num1>num2))
          {
              for(int j=num1-7;j<=num2-7;j++)
              {
                  ui->calendar->item(j,day-1)->setBackground(QBrush(QColor(170, 85, 0)));
              }
          }
        }
    }
    for(int i=0;i<arr_team.size();i++)
    {
        if(arr_team[i].date>=date&&arr_team[i].date<=date.addDays(6))
        {
          int day=arr_team[i].date.dayOfWeek();
          int num1=arr_team[i].time.hour();
          int num2=arr_team[i].endtime.hour();
          if(arr_team[i].endtime.minute()==0)
          {
              num2--;
          }
          if(!(num1<8||num1>=22||num2<8||num2>=22||num1>num2))
          {
              for(int j=num1-7;j<=num2-7;j++)
              {
                  ui->calendar->item(j,day-1)->setBackground(QBrush(QColor(0, 150, 200)));
              }
          }
        }
    }

}
