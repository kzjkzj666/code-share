#include "displayplan.h"
#include "qdialog.h"
#include "ui_displayplan.h"
#include<QDir>
#include"addplan.h"
displayplan::displayplan(QString id,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::displayplan)
{
    ui->setupUi(this);
     setWindowIcon(QIcon(":/res/icon.png"));
        setWindowFlags(windowFlags()&~Qt::WindowMinMaxButtonsHint|Qt::WindowMinimizeButtonHint);
    m_id=id;
    date=QDate::currentDate().toString("yyyyMM");
    connect(this,&displayplan::should_dis,[=]
            {
        this->clear();
        this->clear_arr_plan();
        refresh();
        display();
    });
    addplan* addpl=new addplan(0,m_id);
    connect(ui->add,&QPushButton::clicked,[=]()
            {
                this->hide();
                this->clear();
                this->clear_arr_plan();
                addpl->show();
            });
    connect(addpl,&addplan::back,[=]
            {
                addpl->hide();
                this->refresh();
                this->display();
                this->show();
            });
    connect(addpl,&addplan::ok,[=]
            {
                addpl->hide();
                this->refresh();
                this->display();
                this->show();
            });
    connect(ui->btn1,&QPushButton::clicked,[=]
    {
       int year=date.left(4).toInt();
       int month=date.right(2).toInt();
       if(month==1)
       {
           month=12;
           year--;
       }
       else
       {
           month--;
       }
       if(month<=9)
       {
            date = QString::number(year)+ "0"+QString::number(month);
       }
       else
       {
           date = QString::number(year)+QString::number(month);
       }
       this->clear();
       this->display();
    });
    connect(ui->btn2,&QPushButton::clicked,[=]
    {
       int year=date.left(4).toInt();
       int month=date.right(2).toInt();
       if(month==12)
       {
           month=1;
           year++;
       }
       else
       {
           month++;
       }
       if(month<=9)
       {
            date = QString::number(year)+ "0"+QString::number(month);
       }
       else
       {
           date = QString::number(year)+QString::number(month);
       }
       this->clear();
       this->display();
    });
}
bool cmp(plan_s x,plan_s y)
{
    if(x.date>y.date)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
void displayplan::display()
{
    vector<plan_s>arr_now_month;
    for(int i=0;i<arr_plan.size();i++)
    {
        if(date==arr_plan[i].date.left(arr_plan[i].date.size()-2))
        {
            arr_now_month.push_back(arr_plan[i]);

        }
    }
    sort(arr_now_month.begin(),arr_now_month.end(),cmp);

    if(!arr_now_month.empty())
    {
        for(int i=0;i<arr_now_month.size();i++)
        {

            QLabel * lab1=new QLabel;
            lab1->setParent(this);
            lab1->setFixedSize(120,28);
            lab1->move(70,100+i*40);
            lab1->setText(arr_now_month[i].date.left(4)+"年"+arr_now_month[i].date.mid(4,2)+"月"
                         +arr_now_month[i].date.right(2)+"日");
            lab1->show();
            arr_label.push_back(lab1);
            QLabel * lab2=new QLabel;
            lab2->setParent(this);
            lab2->setFixedSize(120,28);
            lab2->move(250,100+i*40);
            lab2->setText(arr_now_month[i].title);
            lab2->show();
            arr_label.push_back(lab2);
            QLabel * lab3=new QLabel;
            lab3->setParent(this);
            lab3->setFixedSize(120,28);
            lab3->move(420,100+i*40);
            lab3->setText(arr_now_month[i].place);
            lab3->show();
            arr_label.push_back(lab3);
            QPushButton* btn=new QPushButton;
            btn->setParent(this);
            btn->setFixedSize(60,28);
            btn->setText("查看详情");
            btn->setStyleSheet("background:transparent;");
            btn->setCursor(QCursor(Qt::PointingHandCursor));
            btn->move(620,100+i*40);
            btn->show();
            arr_btn.push_back(btn);
            QPushButton* del=new QPushButton;
            del->setParent(this);
            del->setFixedSize(60,28);
            del->setText("删除日程");
            del->setStyleSheet("background:transparent;");
            del->setCursor(QCursor(Qt::PointingHandCursor));
            del->move(768,100+i*40);
            del->show();
            arr_del.push_back(del);
            hash.insert(del,arr_now_month[i].creat_time);
            connect(del,&QPushButton::clicked,[=]
                    {
                        QFile f(QDir::currentPath()+"/"+m_id+"_plan.json");
                        f.open(QIODeviceBase::ReadOnly);
                        QJsonArray arr=QJsonDocument::fromJson(f.readAll()).array();
                        f.close();
                        QString target=arr_now_month[i].creat_time;
                        for(int j=0;j<arr.size();j++)
                        {
                            if(target==arr[j].toObject().value("creat_time").toString())
                            {
                                arr.removeAt(j);
                                f.open(QIODeviceBase::WriteOnly);
                                f.write(QJsonDocument(arr).toJson());
                                f.close();
                                QJsonObject obj;
                                obj.insert("type","10");
                                obj.insert("target",target);
                                obj.insert("id",this->m_id);
                                send * m_send=new send;
                                m_send->sendfile(QJsonDocument(obj).toJson());
                                emit should_dis();
                                break;
                            }
                        }
                    });
        }
        }
    ui->label->setText("查看日程   "+date.left(4)+"-"+date.right(2));
   for(int i=0;i<arr_btn.size();i++)
   {
       connect(arr_btn[i],&QPushButton::clicked,[=]
       {
           QDialog * dia=new QDialog;
           dia->setFixedSize(500,500);
           QLabel * lab1=new QLabel;
           lab1->setParent(dia);
           lab1->setFixedSize(120,28);
           lab1->move(50,50);
           lab1->setText("日期:"+arr_now_month[i].date.left(4)+"年"+arr_now_month[i].date.mid(4,2)+"月"
                        +arr_now_month[i].date.right(2)+"日");
           lab1->show();
           QLabel * lab2=new QLabel;
           lab2->setParent(dia);
           lab2->setFixedSize(120,28);
           lab2->move(50,10);
           lab2->setText("标题: "+arr_now_month[i].title);
           lab2->show();
           QLabel * lab3=new QLabel;
           lab3->setParent(dia);
           lab3->setFixedSize(300,28);
           lab3->move(50,90);
           lab3->setText("地点: "+arr_now_month[i].place);
           lab3->show();
           QLabel * lab4=new QLabel;
           lab4->setParent(dia);
           lab4->setFixedSize(300,28);
           lab4->move(50,115);
           lab4->setText("时间: "+arr_now_month[i].time.left(2)+":"+arr_now_month[i].time.right(2)+"~"+arr_now_month[i].endtime.left(2)+":"+arr_now_month[i].endtime.right(2));
            lab4->show();
           QLabel * lab5=new QLabel;
           lab5->setParent(dia);
           lab5->setFixedSize(200,100);
           lab5->move(50,120);
           lab5->setText("内容: "+arr_now_month[i].content);
           lab5->show();
           dia->show();
       });
   }

}
void displayplan::closeEvent(QCloseEvent *)
{
   this->clear();
   this->clear_arr_plan();
   emit back();
}
void displayplan:: clear_arr_plan()
{
      memset(&arr_plan,0,sizeof(arr_plan));
}
void displayplan::clear()
{
    hash.clear();
    for(int i=arr_btn.size()-1;i>=0;i--)
    {
        delete arr_btn[i];
        arr_btn.pop_back();
    }
    for(int i=arr_del.size()-1;i>=0;i--)
    {
        delete arr_del[i];
        arr_del.pop_back();
    }
    for(int i=arr_label.size()-1;i>=0;i--)
    {
        delete arr_label[i];
        arr_label.pop_back();
    }
}
void displayplan::refresh()
{
    QString filepath= QDir::currentPath()+"/"+m_id+"_plan.json";
    //打开文件
    QFile file(filepath);
    file.open(QIODevice::ReadOnly| QIODevice::Text);
    QByteArray data_user=file.readAll();
    file.close();
    QJsonArray array_user=QJsonDocument::fromJson(data_user).array();
    for(int i=0;i<array_user.size();i++)
    {
        plan_s p;
        p.id=array_user[i].toObject().value("id").toString();
        p.date=array_user[i].toObject().value("date").toString();
        p.time=array_user[i].toObject().value("time").toString();
        p.privacy=array_user[i].toObject().value("privacy").toString();
        p.title=array_user[i].toObject().value("title").toString();
        p.content=array_user[i].toObject().value("content").toString();
        p.place=array_user[i].toObject().value("place").toString();
        p.creat_time=array_user[i].toObject().value("creat_time").toString();
        p.endtime=array_user[i].toObject().value("endtime").toString();
        arr_plan.push_back(p);
    }
}
displayplan::~displayplan()
{
    delete ui;
}
