#include "mainwindow.h"
#include "ui_mainwindow.h"
#include"displayplan.h"
#include"team.h"
#include<QTimer>
#include <QHttpMultiPart>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QMetaObject>
#include <QEventLoop>
#include <QJsonDocument>
#include <QMovie>
MainWindow::MainWindow(QString id,QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
      setWindowIcon(QIcon(":/res/icon.png"));
        QMovie *movie = new QMovie(":/res/img15.gif");
      ui->label_5->setMovie(movie); // 1. 设置要显示的 GIF 动画图片
        movie->start(); // 2. 启动动画

    //设置不允许最大化
    setWindowFlags(windowFlags()&~Qt::WindowMinMaxButtonsHint|Qt::WindowMinimizeButtonHint);
    ui->tableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->tableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->tableWidget->verticalHeader()->setStyleSheet("background-color: transparent;");
    m_i=m_j=0;
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->hourweather->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->hourweather->setSelectionMode(QAbstractItemView::NoSelection);
    t_time = new QTimer(this);
    t_time->start(1000);//设置时间间隔为1秒
    connect(t_time, &QTimer::timeout,[=]
            {
                ui->now->setText(QDate::currentDate().toString("               yyyy年MM月dd日  ")+QTime::currentTime().toString("hh:mm:ss"));
            }
             );
    connect(t_time, &QTimer::timeout,this,&MainWindow::link2);
    city="广州";
    city_id="101280101";
    ui->label_change->hide();
    ui->input_citychage->hide();
    ui->ok_citychange->hide();
//    ui->tableWidget->item(0,0)->setText("666");
//    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
    m_id=id;
    date_now=QDate::currentDate();
    date.setDate(date_now.year(),date_now.month(),1);
    showcalendar();
     ui->user->setText("当前登录用户: "+id);
    connect(ui->back,&QPushButton::clicked,[=]
            {
                back();
            });
     connect(ui->tableWidget,&QTableWidget::itemClicked,[=](QTableWidgetItem* item)
            {
        if(inf.contains(item))
            {
            ui->information->setText(inf.value(item));
        }
        else
            {
            ui->information->clear();
        }
    });
    connect(ui->hourweather,&QTableWidget::itemClicked,[=](QTableWidgetItem* item)
            {
                if(inf2.contains(item))
                {
                    ui->information->setText(inf2.value(item));
                }
                else
                    {
                      ui->information->clear();
                }
            });
    displayplan * dpl =new displayplan(m_id);
    connect(ui->btn2,&QPushButton::clicked,[=]
            {
        this->hide();
        dpl->refresh();
        dpl->display();
        dpl->show();
            });
    connect(dpl,&displayplan::back,[=]
            {
                this->show();
                dpl->hide();
            }
            );
    team* tm=new team(id);
    connect(ui->btn3,&QPushButton::clicked,[=]
            {
                tm->refresh();
                this->hide();
                tm->show();
            });
    connect(tm,&team::back,[=]
            {
        QJsonObject c_obj;
        c_obj.insert("type","12");//12--向服务器发出数据同步请求
        c_obj.insert("id",m_id);
        QJsonDocument c_doc(c_obj);
        QByteArray pack=c_doc.toJson();
        send * m_send=new send;
        m_send->sendfile(pack);
                this->show();
                tm->hide();
            }
            );
    connect(ui->link,&QPushButton::clicked,[=]
            {
                link();
            });
    connect(ui->weather,&QCheckBox::stateChanged,[=](int status)
            {
        if(status==0)
            {
             ui->tableWidget->setIconSize(QSize(0,0));
        }
        else
             {
              weather();
        }
    });
    connect(ui->last,&QPushButton::clicked,[=]
            {
        ui->tableWidget->item(m_i,m_j)->setBackground(QBrush(QColor(0,0,0,0)));
         ui->information->clear();
        date=date.addMonths(-1);
        ui->tableWidget->setIconSize(QSize(0,0));
        showcalendar();
                 weather();
    });
    connect(ui->next,&QPushButton::clicked,[=]
            {
        ui->tableWidget->item(m_i,m_j)->setBackground(QBrush(QColor(0,0,0,0)));
        date=date.addMonths(1);
        ui->tableWidget->setIconSize(QSize(0,0));
        showcalendar();
         weather();
            });
    hourweather();
    connect(ui->changecity,&QPushButton::clicked,[=]
            {

                ui->label_change->show();
                ui->input_citychage->show();
                ui->ok_citychange->show();
                connect(ui->ok_citychange,&QPushButton::clicked,[=]
                        {
                            if(ui->input_citychage->text()!="")
                            {

                                ui->label_change->hide();
                                ui->input_citychage->hide();
                                ui->ok_citychange->hide();
                                QNetworkRequest request;
                                QNetworkAccessManager manager;
                                MainWindow::connect(&manager, &QNetworkAccessManager::finished,[=](QNetworkReply *reply)
                                                    {
                                                        QByteArray text=reply->readAll();
                                                        qDebug()<<QJsonDocument::fromJson(text).object().value("code").toString();

                                                        if(QJsonDocument::fromJson(text).object().value("code").toString()!="200")
                                                        {
                                                            QMessageBox *m_box = new QMessageBox(QMessageBox::Information,QString("提示"),QString("城市修改失败，请输入区/县级以上的行政级别地市名称"));
                                                            QTimer::singleShot(800,m_box,SLOT(accept()));
                                                            m_box->exec();

                                                        }
                                                        else if(text.isEmpty())
                                                        {
                                                            QMessageBox::information(this,"提示","请求失败！");

                                                        }
                                                        else //成功
                                                        {
                                                             ui->information->clear();
                                                            this->city=QJsonDocument::fromJson(text).object().value("location").toArray()[0].toObject().value("name").toString();
                                                            ui->city->setText(this->city);
                                                            city_id=QJsonDocument::fromJson(text).object().value("location").toArray()[0].toObject().value("id").toString();
                                                             hourweather();
                                                            if(ui->weather->checkState()!=0)
                                                            {
                                                                weather();
                                                            }
                                                        }
                                                    });
                                request.setUrl(QUrl("https://geoapi.qweather.com/v2/city/lookup?location="+ui->input_citychage->text()+"&key=cebe8a9310a540b9b3d2a1007a2677bb"));
                                ui->input_citychage->clear();
                                request.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/json;charset=utf-8"));
                                QNetworkReply *reply = manager.get(request);
                                //需要加一个时钟
                                QTimer timer;
                                timer.setInterval(5000);
                                timer.setSingleShot(1);
                                QEventLoop loop;
                                connect(reply,&QNetworkReply::finished,&loop,&QEventLoop::quit);
                                connect(&timer, &QTimer::timeout,[=]
                                        {
                                            QMessageBox::information(this,"提示","请求超时");
                                        });
                                connect(&timer,&QTimer::timeout,&loop,&QEventLoop::quit);
                                timer.start();
                                loop.exec();
                            }

                        });
            });

}
void MainWindow::link()
{
    flag=0;
    ui->link->setText("正在同步");
    ui->link->setDisabled(1);
    QJsonObject c_obj;
    c_obj.insert("type","2");//2--向服务器发出数据同步请求
    c_obj.insert("id",m_id);
    QJsonDocument c_doc(c_obj);
    QByteArray pack=c_doc.toJson();
    send * m_send=new send;
    m_send->sendfile(pack);
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
        ui->link->setDisabled(0);
        ui->link->setText("数据同步");
        QMessageBox::information(this,"提示","数据同步成功！");
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
        ui->link->setDisabled(0);
        ui->link->setText("数据同步");
        QMessageBox::information(this,"数据同步失败","网络连接超时！");
    }
}
MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::closeEvent(QCloseEvent *)
{
    emit back();
}
void MainWindow::showcalendar()
{
    inf.clear();
    for(int i=0;i<6;i++)
    {

        for(int j=0;j<7;j++)
        {
            ui->tableWidget->item(i,j)->setText("");
            ui->tableWidget->item(i,j)->setIcon(QIcon());
        }
    }
    hash.clear();
    QDate d=date;
    ui->label->setText(date.toString("yyyy年MM月"));
    int num=date.dayOfWeek();
    QFont f;
    f.setFamily("宋体");
    f.setPointSize(11);
    for(int i=0;i<6;i++)
    {
        if(i==0)
        {
        for(int j=0;j<7;j++)
        {
                if(j<num-1) continue;
                else                 
                {
                    if(d==date_now)
                    {
                        ui->tableWidget->item(i,j)->setBackground(QBrush(QColor(0,255,127)));
                        m_i=i;
                        m_j=j;
                    }
                    QString festival="";
                    if(d.month()==1&&d.day()==1)
                    {
                        festival="元旦";
                    }
                    else if(d.month()==2&&d.day()==14)
                    {
                        festival="情人节";
                    }
                    else if(d.month()==3&&d.day()==8)
                    {
                        festival="妇女节";
                    }
                    else if(d.month()==3&&d.day()==12)
                    {
                        festival="植树节";
                    }
                    else if(d.month()==5&&d.day()==1)
                    {
                        festival="劳动节";
                    }
                    else if(d.month()==5&&d.day()==4)
                    {
                        festival="青年节";
                    }
                    else if(d.month()==6&&d.day()==1)
                    {
                        festival="儿童节";
                    }
                    else if(d.month()==7&&d.day()==1)
                    {
                        festival="建党节";
                    }
                    else if(d.month()==8&&d.day()==1)
                    {
                        festival="建军节";
                    }
                    else if(d.month()==9&&d.day()==3)
                    {
                        festival="胜利日";
                    }
                    else if(d.month()==9&&d.day()==10)
                    {
                        festival="教师节";
                    }
                    else if(d.month()==10&&d.day()==1)
                    {
                        festival="国庆节";
                    }
                  ui->tableWidget->item(i,j)->setText(QString::number(d.day())+"\n\n"+festival);
                     hash.insert(d,ui->tableWidget->item(i,j));
                    ui->tableWidget->item(i,j)->setTextAlignment(Qt::AlignHCenter|Qt::AlignTop);
                     ui->tableWidget->item(i,j)->setFont(f);
                    d=d.addDays(1);
                }
        }
        }
        else
        {
        for(int j=0;j<7;j++)
        {
                if(d==date_now)
                {
                    ui->tableWidget->item(i,j)->setBackground(QBrush(QColor(0,255,127)));
                    m_i=i;
                    m_j=j;
                }
                QString festival="";
                if(d.month()==1&&d.day()==1)
                {
                    festival="元旦";
                }
                else if(d.month()==2&&d.day()==14)
                {
                    festival="情人节";
                }
                else if(d.month()==3&&d.day()==8)
                {
                    festival="妇女节";
                }
                else if(d.month()==3&&d.day()==12)
                {
                    festival="植树节";
                }
                else if(d.month()==5&&d.day()==1)
                {
                    festival="劳动节";
                }
                else if(d.month()==5&&d.day()==4)
                {
                    festival="青年节";
                }
                else if(d.month()==6&&d.day()==1)
                {
                    festival="儿童节";
                }
                else if(d.month()==7&&d.day()==1)
                {
                    festival="建党节";
                }
                else if(d.month()==8&&d.day()==1)
                {
                    festival="建军节";
                }
                else if(d.month()==9&&d.day()==3)
                {
                    festival="胜利日";
                }
                else if(d.month()==9&&d.day()==10)
                {
                    festival="教师节";
                }
                else if(d.month()==10&&d.day()==1)
                {
                    festival="国庆节";
                }
                  ui->tableWidget->item(i,j)->setText(QString::number(d.day())+"\n\n"+festival);
                hash.insert(d,ui->tableWidget->item(i,j));
                ui->tableWidget->item(i,j)->setTextAlignment(Qt::AlignHCenter|Qt::AlignTop);
                ui->tableWidget->item(i,j)->setFont(f);
                d=d.addDays(1);
                if(d.month()!=date.month())
                    break;
        }
        }
        if(d.month()!=date.month())break;
    }


}
void MainWindow::weather()
{
    if(ui->weather->checkState()==0)return;
    if(date.year()==date_now.year())
    {
        if(date.month()-date_now.month()>=2||date.month()-date_now.month()<=-2)return;

    }
    else if(date.year()>date_now.year())
    {
        if(date.month()+12-date_now.month()>=2) return;
    }
    else if(date.year()<date_now.year())
    {
        if(date_now.month()+12-date.month()>=2) return;
    }
   ui->tableWidget->setIconSize(QSize(0,0));
    QNetworkRequest request1;
    QNetworkAccessManager manager1;
    connect(&manager1, &QNetworkAccessManager::finished,[=](QNetworkReply *reply1)
            {
                QByteArray text=reply1->readAll();
                QJsonObject obj=QJsonDocument::fromJson(text).object();
                if(text.isEmpty())
                {
                    QMessageBox::information(this,"提示","请求失败！");
                    ui->weather->setChecked(0);
                }
                //                                                    QString updateTime=obj.value("updateTime").toString();
                //
                else
                {
                    QJsonArray arr=obj.value("daily").toArray();
                    for(int i=0;i<arr.size();i++)
                    {
                        QDate fxDate=QDate::fromString(arr[i].toObject().value("fxDate").toString(),"yyyy-MM-dd");

                        if(hash.value(fxDate)!=NULL)
                        {
                            QJsonObject op=arr[i].toObject();
                            QString iconDay=arr[i].toObject().value("iconDay").toString();
                            ui->tableWidget->setIconSize(QSize(40,40));

                            hash.value(fxDate)->setIcon( QIcon(":/res/weather/"+iconDay+".svg"));
//                            "sunrise":"05:41","sunset":"19:13","moonrise":"00:05","moonset":"11:45"
//                                ,"moonPhase":"亏凸月","moonPhaseIcon":"805","tempMax":"33","tempMin":"25",

//      "iconDay":"306","textDay":"中雨","iconNight":"306","textNight":"中雨","wind360Day":"29",
//                            "windDirDay":"东北风","windScaleDay":"1-2","windSpeedDay":"7","wind360Night":"0","windDirNight":"北风",
//                            "windScaleNight":"1-2","windSpeedNight":"3","humidity":"87","precip":"5.1","pressure":"999","vis":"22",
//                "cloud":"75","uvIndex":"3"},
                            QString sunrise= op.value("sunrise").toString();
                             QString sunset= op.value("sunset").toString();
                             QString moonrise= op.value("moonrise").toString();
                              QString moonset= op.value("moonset").toString();
                              QString moonPhase= op.value("moonPhase").toString();
                              QString tempMax= op.value("tempMax").toString();
                              QString tempMin= op.value("tempMin").toString();
                              QString textDay= op.value("textDay").toString();
                              QString textNight= op.value("textNight").toString();
                              QString windDirDay= op.value("windDirDay").toString();
                              QString windScaleDay= op.value("windScaleDay").toString();
                              QString windDirNight= op.value("windDirNight").toString();
                              QString windScaleNight= op.value("windScaleNight").toString();
                              QString humidity= op.value("humidity").toString();
                              QString pressure= op.value("pressure").toString();
                              QString str=this->city+"  "+op.value("fxDate").toString().mid(0,4)+"年"+op.value("fxDate").toString().mid(5,2)+"月"+
                              op.value("fxDate").toString().mid(8,2)+"日"+"\n"
                              +"日出:"+sunrise+"  "+"日落:"+sunset+"  \n"+
                              "月出:"+moonrise+"  月落:"+moonset+"  月相:"+moonPhase+"  \n"+
                              "温度: "+tempMin+"°C-"+tempMax+"°C\n"+
                              "白天:"+textDay+"  "+windDirDay+windScaleDay+"级\n"+
                              "夜间:"+textNight+"  "+windDirNight+windScaleNight+"级\n"+
                              "相对湿度:"+humidity+"%  "+"大气压强:"+pressure+"hPa";
                             this->inf.insert(hash.value(fxDate),str);
                }



                //                                                        QString textDay=arr[i].toObject().value("textDay").toString();
                //                                                        qDebug()<<fxDate<<"   "<<textDay;
            }
        }
    });
    request1.setUrl(QUrl("https://devapi.qweather.com/v7/weather/7d?location="+city_id+"&key=cebe8a9310a540b9b3d2a1007a2677bb"));
    request1.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/json;charset=utf-8"));
    QNetworkReply *reply1 = manager1.get(request1);
    QTimer timer;
    timer.setInterval(5000);
    timer.setSingleShot(1);
    QEventLoop loop;
    connect(reply1,&QNetworkReply::finished,&loop,&QEventLoop::quit);
    connect(&timer, &QTimer::timeout,[=]
            {
                QMessageBox::information(this,"提示","请求超时");
            });
    connect(&timer,&QTimer::timeout,&loop,&QEventLoop::quit);
    timer.start();
    loop.exec();
}
void MainWindow:: hourweather()
{
    ui->hourweather->setIconSize(QSize(28,28));
    QNetworkRequest request;
    QNetworkAccessManager manager;
    MainWindow::connect(&manager, &QNetworkAccessManager::finished,[=](QNetworkReply *reply)
                        {
        QJsonObject obj=QJsonDocument::fromJson(reply->readAll()).object();
        if(obj.value("code").toString()=="200")
            {
            QJsonArray arr=obj.value("hourly").toArray();
            QTime t;
            int num=0;
            this->inf2.clear();
            for(int i=0;i<arr.size();i+=2)
            {
                QJsonObject op=arr[i].toObject();
//                t=QTime::fromString(op.value("fxTime").toString().mid(11,5),"hh:mm");
                ui->hourweather->item((num/6)*3,num%6)->setText(op.value("fxTime").toString().mid(11,5));
                QString temp= op.value("temp").toString();

                QString text= op.value("text").toString();

                QString windDir= op.value("windDir").toString();
                QString windScale= op.value("windScale").toString();

                QString humidity= op.value("humidity").toString();
                QString pressure= op.value("pressure").toString();
                QString str= this->city+"  "+op.value("fxTime").toString().mid(0,4)+"年"+op.value("fxTime").toString().mid(5,2)+"月"+
          op.value("fxTime").toString().mid(8,2)+"日 "+op.value("fxTime").toString().mid(11,5)+"\n"
                              +"温度: "+temp+"°C\n"+
                              "天气:"+text+"  \n风力: "+windDir+windScale+"级\n"+
                             + "相对湿度:"+humidity+"%  "+"大气压强:"+pressure+"hPa";
                ui->hourweather->item((num/6)*3+1,num%6)->setIcon(QIcon(":/res/weather/"+op.value("icon").toString()+".svg"));
                 ui->hourweather->item((num/6)*3+2,num%6)->setText(op.value("temp").toString()+"°C");
                   this->inf2.insert(ui->hourweather->item((num/6)*3,num%6),str);
                    this->inf2.insert(ui->hourweather->item((num/6)*3+1,num%6),str);
                     this->inf2.insert(ui->hourweather->item((num/6)*3+2,num%6),str);
                num++;
            }

        }

                        });
    request.setUrl(QUrl("https://devapi.qweather.com/v7/weather/24h?location="+this->city_id+"&key=cebe8a9310a540b9b3d2a1007a2677bb"));
    request.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/json;charset=utf-8"));
    QNetworkReply *reply = manager.get(request);
    //需要加一个时钟
    QTimer timer;
    timer.setInterval(5000);
    timer.setSingleShot(1);
    QEventLoop loop;
    connect(reply,&QNetworkReply::finished,&loop,&QEventLoop::quit);
    connect(&timer, &QTimer::timeout,[=]
            {
        QMessageBox::information(this,"提示","请求超时");
            });
    connect(&timer,&QTimer::timeout,&loop,&QEventLoop::quit);
    timer.start();
    loop.exec();
}

void MainWindow::link2()
{
    disconnect(t_time, &QTimer::timeout,this,&MainWindow::link2);
    QJsonObject c_obj;
    c_obj.insert("type","12");//2--向服务器发出数据同步请求
    c_obj.insert("id",m_id);
    QJsonDocument c_doc(c_obj);
    QByteArray pack=c_doc.toJson();
    send * m_send=new send;
    m_send->sendfile(pack);
    connect(this,&MainWindow::back,[=]
            {
        if(m_send!=NULL)
            {
            delete m_send;
        }
    });
}
