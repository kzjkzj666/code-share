#ifndef JOINTEAM_H
#define JOINTEAM_H

#include <QWidget>
#include<QMessageBox>
#include<QJsonObject>
#include<QJsonDocument>
#include<QTimer>
namespace Ui {
class jointeam;
}

class jointeam : public QWidget
{
    Q_OBJECT
private:
    int flag;
    QString m_id;
public:
    explicit jointeam(QString ,QWidget *parent = nullptr);
    ~jointeam(); 
    void join();
    void closeEvent(QCloseEvent *);
signals:
    void back(QString name);


private:
    Ui::jointeam *ui;
};

#endif // JOINTEAM_H
