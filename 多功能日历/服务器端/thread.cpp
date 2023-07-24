#include "thread.h"
#include<QFileInfo>
thread::thread(QTcpSocket* t ,QObject *parent)
    : QThread{parent}
{
    this->tcp=t;
}
void thread::run()
{
    connect(tcp,&QIODevice::readyRead,[=]
            {
                QByteArray data=tcp->readAll();
                QJsonDocument doc=QJsonDocument::fromJson(data);
                QJsonObject obj=doc.object();
                if(obj.value("type")=="0")
                {                    QString filepath= QDir::currentPath()+"/user.json";
                    QFile file(filepath);
                    //打开文件
                    file.open(QIODevice::ReadOnly| QIODevice::Text);
                    QByteArray data_user=file.readAll();
                    file.close();
                    QJsonArray array_user;
                    QJsonDocument doc_user=QJsonDocument::fromJson(data_user);
                    array_user=doc_user.array();
                    bool flag=1;
                    for(int i=0;i<array_user.size();i++)
                    {
                        QJsonObject temp=array_user[i].toObject();
                        if(temp.value("id")==obj.value("id"))
                        {
                            flag=0;
                            break;
                        }
                    }
                    if(flag==1)
                    {
                        array_user.prepend(obj);
                        file.open(QIODevice::WriteOnly| QIODevice::Text);
                        file.write(QByteArray(QJsonDocument(array_user).toJson()));
                        file.close();
                        //接收客户端的注册信息完成，下面返回一个json，告诉客户端注册成功
                        QJsonObject c_obj;
                        c_obj.insert("type","0");//0--注册状态返回
                        c_obj.insert("status","1");//1--成功，0--发现重名，注册失败！
                        QJsonDocument c_doc(c_obj);
                        QByteArray pack=c_doc.toJson();
                        tcp->write(pack);
                    }
                    else
                    {
                        //接收客户端的注册信息完成，下面返回一个json，告诉客户端注册失败
                        QJsonObject c_obj;
                        c_obj.insert("type","0");//0--注册状态返回
                        c_obj.insert("status","0");//1--成功，0--发现重名，注册失败！
                        QJsonDocument c_doc(c_obj);
                        QByteArray pack=c_doc.toJson();
                        tcp->write(pack);
                    }}
                else if(obj.value("type")=="1")
                {

                    QString filepath= QDir::currentPath()+"/user.json";
                    QFile file(filepath);
                    //打开文件
                    file.open(QIODevice::ReadOnly| QIODevice::Text);
                    QByteArray data_user=file.readAll();
                    file.close();
                    QJsonArray array_user;
                    QJsonDocument doc_user=QJsonDocument::fromJson(data_user);
                    array_user=doc_user.array();
                    bool check_id=0,check_password=0;
                    for(int i=0;i<array_user.size();i++)
                    {
                        QJsonObject temp=array_user[i].toObject();
                        if(temp.value("id")==obj.value("id"))
                        {
                            check_id=1;
                            if(temp.value("password")==obj.value("password"))
                            {
                                check_password=1;
                            }
                            break;
                        }
                    }
                    if(check_id==0)//用户不存在
                    {
                        QJsonObject c_obj;
                        c_obj.insert("type","1");//1--登录状态返回
                        c_obj.insert("status","2");//1--成功，0--密码错误，2--用户不存在
                        QJsonDocument c_doc(c_obj);
                        QByteArray pack=c_doc.toJson();
                        tcp->write(pack);
                    }
                    else
                    {
                        if(check_password==0)//密码错误
                        {
                            QJsonObject c_obj;
                            c_obj.insert("type","1");//1--登录状态返回
                            c_obj.insert("status","0");//1--成功，0--密码错误，2--用户不存在
                            QJsonDocument c_doc(c_obj);
                            QByteArray pack=c_doc.toJson();
                            tcp->write(pack);
                        }
                        else//登录成功
                        {
                            QJsonObject c_obj;
                            c_obj.insert("type","1");//1--登录状态返回
                            c_obj.insert("status","1");
                            QJsonDocument c_doc(c_obj);
                            QByteArray pack=c_doc.toJson();
                            tcp->write(pack);
                            qDebug()<<c_obj;
                            //同步文件
                        }
                    }

                }
                else if(obj.value("type")=="2"||obj.value("type")=="12")
                {
                    //个人日程数据，其他人个人日程汇总，团队列表
                    QJsonObject information;
                    QFile file_1(QDir::currentPath()+"/"+ obj.value("id").toString()+"_plan.json");
                    file_1.open(QIODeviceBase::ReadOnly);
                    information.insert("myplan",QJsonDocument::fromJson(file_1.readAll()).array());
                    QFile file_2(QDir::currentPath()+"/"+ "user_plan_sum.json");
                    file_2.open(QIODeviceBase::ReadOnly);
                    information.insert("otherplan",QJsonDocument::fromJson(file_2.readAll()).array());
                    QFile file_3( QDir::currentPath()+"/teamlist.json");
                    file_3.open(QIODeviceBase::ReadOnly);
                    QJsonArray arr=QJsonDocument::fromJson(file_3.readAll()).array();
                    QJsonArray arr_1;
                    for(int i=0;i<arr.size();i++)
                    {

                        QFile f(QDir::currentPath()+"/"+arr[i].toObject().value("name").toString()+"_member.json");
                        f.open(QIODeviceBase::ReadOnly);
                        QJsonArray ar=QJsonDocument::fromJson(f.readAll()).array();
                        f.close();
                        int flag=-1;
                        for(int j=0;j<ar.size();j++)
                        {
                            if(ar[j].toString()==obj.value("id").toString())
                            {
                                flag=1;
                                break;
                            }
                        }
                        if(flag==1)
                        {

                            QJsonObject ob;
                            ob.insert("name",arr[i].toObject().value("name"));
                            ob.insert("member",ar);
                            arr_1.append(ob);
                        }
                    }
                    information.insert( obj.value("id").toString()+"_teamlist",arr_1);
                    if(obj.value("name").toString()!="")
                    {
                    QFile file_4(QDir::currentPath()+"/"+ obj.value("name").toString()+"_teamplan.json");
                    file_4.open(QIODeviceBase::ReadOnly);
                    information.insert("teamplan",QJsonDocument::fromJson(file_4.readAll()).array());
                    information.insert("name",obj.value("name"));
                    file_4.close();
                    }
                    file_1.close();
                    file_2.close();
                    file_3.close(); 
                    information.insert("type",obj.value("type"));
                    information.insert("id",obj.value("id"));
                    QByteArray pack=QJsonDocument(information).toJson();
                    tcp->write(pack);
                }
                else if(obj.value("type")=="4")
                {
                    QString filepath= QDir::currentPath()+"/teamlist.json";
                    QFile file(filepath);
                    //打开文件
                    file.open(QIODevice::ReadOnly| QIODevice::Text);
                    QByteArray data_user=file.readAll();
                    file.close();
                    QJsonArray array_user;
                    QJsonDocument doc_user=QJsonDocument::fromJson(data_user);
                    array_user=doc_user.array();
                    bool check_name=0,check_password=0,check_joined=0;
                    for(int i=0;i<array_user.size();i++)
                    {
                        QJsonObject temp=array_user[i].toObject();
                        if(temp.value("name")==obj.value("name"))
                        {
                            check_name=1;
                            if(temp.value("password")==obj.value("password"))
                            {
                                check_password=1;
                                QString filepath_j= QDir::currentPath()+"/"+ obj.value("name").toString()+"_member.json";
                                QFile file_j(filepath_j);
                                file_j.open(QIODevice::ReadOnly| QIODevice::Text);
                                QByteArray data_user_j=file_j.readAll();
                                file_j.close();
                                QJsonArray array_user_j=QJsonDocument::fromJson(data_user_j).array();
                                for(int i=0;i<array_user_j.size();i++)
                                {
                                    if( obj.value("id").toString()==array_user_j[i].toString())
                                    {
                                        check_joined=1;
                                        break;
                                    }
                                }
                            }
                            break;

                        }
                    }
                    if(check_name==0)//团队不存在
                    {
                        QJsonObject c_obj;
                        c_obj.insert("type","4");//1--状态返回
                        c_obj.insert("status","2");//1--成功，0--密码错误，2--团队不存在
                        QJsonDocument c_doc(c_obj);
                        QByteArray pack=c_doc.toJson();
                        tcp->write(pack);
                    }
                    else  if(check_password==0)//密码错误
                    {

                        QJsonObject c_obj;
                        c_obj.insert("type","4");//1--状态返回
                        c_obj.insert("status","0");//1--成功，0--密码错误，2--用户不存在
                        QJsonDocument c_doc(c_obj);
                        QByteArray pack=c_doc.toJson();
                        tcp->write(pack);
                    }
                    else if(check_joined==1)
                    {
                        //已经加入过
                        QJsonObject c_obj;
                        c_obj.insert("type","4");//1--状态返回
                        c_obj.insert("status","3");//1--成功，0--密码错误，2--用户不存在，3--已加入过
                        QJsonDocument c_doc(c_obj);
                        QByteArray pack=c_doc.toJson();
                        tcp->write(pack);
                    }
                    else//加入成功
                    {
                        QJsonObject c_obj;
                        c_obj.insert("type","4");//1--状态返回
                        c_obj.insert("status","1");
                        QJsonDocument c_doc(c_obj);
                        QByteArray pack=c_doc.toJson();
                        tcp->write(pack);
                        //服务器存储团队列表
                        QString filepath_j= QDir::currentPath()+"/"+ obj.value("name").toString()+"_member.json";
                        QFile file_j(filepath_j);
                        file_j.open(QIODevice::ReadOnly| QIODevice::Text);
                        QByteArray data_user_j=file_j.readAll();
                        file_j.close();
                        QJsonArray arr=QJsonDocument::fromJson(data_user_j).array();
                        arr.append(obj.value("id"));
                        QJsonDocument doc_t(arr);
                        file_j.open(QIODevice::WriteOnly| QIODevice::Text);
                        file_j.write(doc_t.toJson());
                        file_j.close();
                    }

                }
                else if(obj.value("type")=="5")

                {
                    QString filepath= QDir::currentPath()+"/teamlist.json";
                    QFile file(filepath);
                    //打开文件
                    file.open(QIODevice::ReadOnly| QIODevice::Text);
                    QByteArray data_user=file.readAll();
                    file.close();
                    QJsonArray array_user;
                    QJsonDocument doc_user=QJsonDocument::fromJson(data_user);
                    array_user=doc_user.array();
                    bool flag=1;
                    for(int i=0;i<array_user.size();i++)
                    {
                        QJsonObject temp=array_user[i].toObject();
                        if(temp.value("name")==obj.value("name"))
                        {
                            flag=0;
                            break;
                        }
                    }
                    if(flag==1)
                    {
                        array_user.prepend(obj);
                        file.open(QIODevice::WriteOnly| QIODevice::Text);
                        file.write(QByteArray(QJsonDocument(array_user).toJson()));
                        file.close();
                        emit new_team_add(obj.value("name").toString());
                        //接收客户端的新建团队信息完成，下面返回一个json，告诉客户端
                        QJsonObject c_obj;
                        c_obj.insert("type","5");//0--新建团队状态返回
                        c_obj.insert("status","1");//1--成功，0--发现重名，失败！
                        QJsonDocument c_doc(c_obj);
                        QByteArray pack=c_doc.toJson();
                        tcp->write(pack);
                        //服务器
                        QJsonArray arr;
                        arr.insert(0,obj.value("id"));
                        QJsonDocument doc_t(arr);
                        QString filepath_j= QDir::currentPath()+"/"+ obj.value("name").toString()+"_member.json";
                        QFile file_j(filepath_j);
                        file_j.open(QIODevice::WriteOnly| QIODevice::Text);
                        file_j.write(doc_t.toJson());
                        file_j.close();
                    }
                    else
                    {
                        //接收客户端的注册信息完成，下面返回一个json，告诉客户端
                        QJsonObject c_obj;
                        c_obj.insert("type","5");//0--注册状态返回
                        c_obj.insert("status","0");//1--成功，0--发现重名，
                        QJsonDocument c_doc(c_obj);
                        QByteArray pack=c_doc.toJson();
                        tcp->write(pack);
                    }

                }
                else if(obj.value("type")=="6")

                {
                    QString filepath= QDir::currentPath()+"/"+ obj.value("id").toString()+"_plan.json";
                    QFile file(filepath);
                    //打开文件
                    file.open(QIODevice::ReadOnly| QIODevice::Text);
                    QByteArray data_user=file.readAll();
                    file.close();
                    QJsonArray array_user;
                    QJsonDocument doc_user=QJsonDocument::fromJson(data_user);
                    array_user=doc_user.array();
                    array_user.prepend(obj);
                    file.open(QIODevice::WriteOnly| QIODevice::Text);
                    file.write(QByteArray(QJsonDocument(array_user).toJson()));
                    file.close();


                    QString sum_path= QDir::currentPath()+"/"+"user_plan_sum.json";
                    QFile sum(sum_path);
                    //打开文件
                    sum.open(QIODevice::ReadOnly| QIODevice::Text);
                    QByteArray sum_text=sum.readAll();
                    sum.close();
                    QJsonArray sum_array=QJsonDocument::fromJson(sum_text).array();
                    QJsonObject sum_obj;
                    sum_obj.insert("type","7");//去掉隐私的个人日程汇总，编号为7
                    sum_obj.insert("time",obj.value("time"));
                    sum_obj.insert("endtime",obj.value("endtime"));
                    sum_obj.insert("creat_time",obj.value("creat_time"));
                    sum_obj.insert("date",obj.value("date"));
                    sum_obj.insert("id",obj.value("id"));
                    sum_array.prepend(sum_obj);
                    sum.open(QIODevice::WriteOnly| QIODevice::Text);
                    sum.write(QByteArray(QJsonDocument(sum_array).toJson()));
                    sum.close();
                    //接收客户端的添加日程信息完成，下面返回一个json，告诉客户端注册成功
                    QJsonObject c_obj;
                    c_obj.insert("type","6");//6--添加日程状态返回
                    c_obj.insert("status","1");//1--成功，0--失败
                    QJsonDocument c_doc(c_obj);
                    QByteArray pack=c_doc.toJson();
                    tcp->write(pack);

                }
                else if(obj.value("type")=="8")

                {
                    QString filepath= QDir::currentPath()+"/"+ obj.value("name").toString()+"_teamplan.json";
                    QFile file(filepath);
                    //打开文件
                    file.open(QIODevice::ReadOnly| QIODevice::Text);
                    QByteArray data_user=file.readAll();
                    file.close();
                    QJsonArray array_user;
                    QJsonDocument doc_user=QJsonDocument::fromJson(data_user);
                    array_user=doc_user.array();
                    array_user.prepend(obj);
                    file.open(QIODevice::WriteOnly| QIODevice::Text);
                    file.write(QByteArray(QJsonDocument(array_user).toJson()));
                    file.close();
                    //接收客户端的添加团队日程信息完成，下面返回一个json，告诉客户端添加成功
                    QJsonObject c_obj;
                    c_obj.insert("type","6");//6--添加日程状态返回
                    c_obj.insert("status","1");//1--成功，0--失败
                    QJsonDocument c_doc(c_obj);
                    QByteArray pack=c_doc.toJson();
                    tcp->write(pack);
                }
                else if(obj.value("type")=="10")
                {
                    QString target=obj.value("target").toString();
                    QString id=obj.value("id").toString();
                    QFile f1(QDir::currentPath()+"/"+ obj.value("id").toString()+"_plan.json");
                    f1.open(QIODeviceBase::ReadOnly);
                    QJsonArray arr1=QJsonDocument::fromJson(f1.readAll()).array();
                    f1.close();
                    for(int j=0;j<arr1.size();j++)
                    {
                        if(target==arr1[j].toObject().value("creat_time").toString())
                        {
                            arr1.removeAt(j);
                            f1.open(QIODeviceBase::WriteOnly);
                            f1.write(QJsonDocument(arr1).toJson());
                            f1.close();
                        }
                    }
                    QFile f2(QDir::currentPath()+"/"+ "user_plan_sum.json");
                    f2.open(QIODeviceBase::ReadOnly);
                    QJsonArray arr2=QJsonDocument::fromJson(f2.readAll()).array();
                    f2.close();
                    for(int j=0;j<arr2.size();j++)
                    {
                        if(target==arr2[j].toObject().value("creat_time").toString())
                        {
                            arr2.removeAt(j);
                            f2.open(QIODeviceBase::WriteOnly);
                            f2.write(QJsonDocument(arr2).toJson());
                            f2.close();
                        }
                    }
                }


            });
}
