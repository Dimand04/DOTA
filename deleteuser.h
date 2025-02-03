#ifndef DELETEUSER_H
#define DELETEUSER_H

#include <QWidget>
#include "mainmenu.h"

namespace Ui {
class deleteuser;
}

class deleteuser : public QWidget
{
    Q_OBJECT

signals:
    void userDeleted();
    void userDeleteCancel();

public:
    explicit deleteuser(QWidget *parent = nullptr);
    ~deleteuser();

public slots:
    void delusr();
    void deleteCancel();

private:
    Ui::deleteuser *ui;
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", "animanga");
    QString userID;
};

#endif // DELETEUSER_H
