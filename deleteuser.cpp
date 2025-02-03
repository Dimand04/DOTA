#include "deleteuser.h"
#include "ui_deleteuser.h"
#include "global.h"

deleteuser::deleteuser(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::deleteuser)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    setGeometry(440, 210, 400, 300);
    connect (ui->btn_delete,&QPushButton::clicked,this,&deleteuser::delusr);
    connect (ui->btn_delete_cancel,&QPushButton::clicked,this,&deleteuser::deleteCancel);
}

deleteuser::~deleteuser()
{
    delete ui;
}

void deleteuser::delusr()
{
    QString userLocalLogin = GlobalVars::userLogin;
    qDebug() << "lg:" << userLocalLogin;
    if (ui->le_password_del->text().isEmpty())
    {
        QMessageBox::warning(this, "Ошибка", "Пожалуйста заполните поле 'Пароль'.");
        return;
    }
    db = QSqlDatabase::database("dota2");
    if(getDBConnection(db))
    {
        QSqlQuery query(db);
        query.prepare("select id from player inner join authorization on player.id = authorization.id_player where player.player_name = ? and authorization.password = ?;");
        query.addBindValue(userLocalLogin);
        query.addBindValue(ui->le_password_del->text());
        query.exec();
        if(query.next())
        {
            QList<QMessageBox::Button> buttons;
            buttons.append(QMessageBox::Ok);
            buttons.append(QMessageBox::Cancel);
            if(showMessage("Вы уверены что хотите удалить аккаунт?", "Подтвердите", buttons, QMessageBox::Question) == QMessageBox::Ok)
            {
                userID = query.value(0).toString();
                query.prepare("DELETE FROM authorization WHERE id_player = ?");
                query.addBindValue(userID);
                query.exec();
                query.prepare("DELETE FROM player WHERE id = ?");
                query.addBindValue(userID);
                query.exec();

                if(query.exec())
                {
                    ui->le_password_del->clear();
                    emit userDeleted();
                }
            }
            else
            {
                ui->le_password_del->clear();
            }
        }
        else
        {
            ui->le_password_del->clear();
            QMessageBox::warning(this, "Ошибка", "В процессе удаления аккаунта произошла ошибка. Пользователя с такими данными не существует.");
        }
    }
}

void deleteuser::deleteCancel()
{
    emit userDeleteCancel();
}
