#ifndef TEAM_H
#define TEAM_H

#include <QWidget>

namespace Ui {
class team;
}

class team : public QWidget
{
    Q_OBJECT

public:
    explicit team(QString,QWidget *parent = nullptr);
    ~team();
    QString id;
    void refresh();
    void closeEvent(QCloseEvent *);
signals:
    void back();
private:
    Ui::team *ui;
};

#endif // TEAM_H
