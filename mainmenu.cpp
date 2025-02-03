#include "mainmenu.h"
#include "ui_mainmenu.h"
#include <qdebug.h>
#include "global.h"
#include <QProcess>
#include <QDebug>
#include "playershortinfo.h"
#include <QCheckBox>
#include "deleteuser.h"

MainMenu::MainMenu(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainMenu)
{
    ui->setupUi (this);
    connect (ui->BTN_Heroes,&QPushLabel::clicked,this,&MainMenu::showHeroesMenu);
    connect (ui->LB_OpenMainMenu,&QPushLabel::clicked,this,&MainMenu::showMainMenu);
    connect (ui->PB_PlayerAvatar,&QPushLabel::clicked,this,&MainMenu::showPlayerProfile);
    connect(ui->PB_AEnter, &QPushButton::clicked, this, &MainMenu::tryAutorization);
    connect(ui->LB_ProfileAvatar, &QPushLabel::clicked, [this]{ selectProfileAvatar(m_PlayerID); });
    connect(ui->BTN_ChangeBackgroundImage_2, &QPushButton::clicked, [this]{ selectProfileBackground(m_PlayerID); });
    connect(ui->DOTA_Hero_Add_Icon, &QPushLabel::doubleClicked, this, &MainMenu::selectHeroIcon);
    connect(ui->DOTA_Hero_Add, &QPushButton::clicked, this, &MainMenu::addNewHero);
    connect (ui->btn_delete_user,&QPushButton::clicked,this,&MainMenu::deleteUser);
    connect (ui->BTN_Autho,&QPushButton::clicked,this,&MainMenu::showAutorization);
}

MainMenu::~MainMenu()
{
    delete ui;
}

void MainMenu::showMainMenu()
{
    ui->frame_profile->setGeometry (ui-> frame_profile ->x(),721, ui->frame_profile->width(), ui->frame_profile->height ());
    ui->Heroes->setGeometry (ui-> Heroes ->x(),721, ui->Heroes->width(), ui->Heroes->height ());
    ui->frame_main->setGeometry(ui->frame_main->x(),40,ui->frame_main->width(),ui->frame_main->height());
    ui->LB_OpenMainMenu->setGeometry (ui-> LB_OpenMainMenu ->x(),0, ui->LB_OpenMainMenu->width(), ui->LB_OpenMainMenu->height ());
    ui->frame->setGeometry (ui-> frame ->x(),0, ui->frame->width(), ui->frame->height());
    ui->frame_2->setGeometry (ui-> frame_2 ->x(),0, ui->frame_2->width(), ui->frame_2->height ());
    ui->Heroes->setEnabled(false);
    ui->frame_main->setEnabled(true);
    ui->frame_profile->setEnabled(false);
    getInfoAboutPlayer(m_PlayerID);
}

void MainMenu::showHeroesMenu()
{
    ui->frame_main -> setGeometry (ui-> frame_main ->x(),721, ui->frame_main->width(), ui->frame_main->height ());
    ui->frame_profile -> setGeometry (ui-> frame_profile ->x(),721, ui->frame_profile->width(), ui->frame_profile->height ());
    ui->Heroes->setGeometry(ui->Heroes->x(),0,ui->Heroes->width(),ui->Heroes->height());
    ui->Heroes->setEnabled(true);
    ui->frame_main->setEnabled(false);
    ui->frame_profile->setEnabled(false);
    loadHeroes();
}

void MainMenu::showPlayerProfile()
{
    ui->frame_main -> setGeometry (ui-> frame_main ->x(),721, ui->frame_main->width(), ui->frame_main->height ());
    ui->Heroes -> setGeometry (ui-> Heroes ->x(),721, ui->Heroes->width(), ui->Heroes->height ());
    ui->frame_profile->setGeometry(ui->frame_profile->x(),40,ui->frame_profile->width(),ui->frame_profile->height());
    ui->Heroes->setEnabled(false);
    ui->frame_main->setEnabled(false);
    ui->frame_profile->setEnabled(true);
}

void MainMenu::showAutorization()
{
    m_PlayerID.clear();
    if(!ui->Autorization->isVisible())
    {
        ui->Autorization->setVisible(true);
        ui->lineEdit_Login->setFocus();
    }
    ui->Autorization-> setGeometry (ui-> Autorization ->x(),0, ui->Autorization->width(), ui->Autorization->height ());
    ui->frame-> setGeometry (ui-> frame ->x(),1000, ui->frame->width(), ui->frame->height ());
    ui->frame_2-> setGeometry (ui-> frame_2 ->x(),1000, ui->frame_2->width(), ui->frame_2->height ());
    ui->LB_OpenMainMenu-> setGeometry (ui-> LB_OpenMainMenu ->x(),1000, ui->LB_OpenMainMenu->width(), ui->LB_OpenMainMenu->height ());
    connect(ui->PB_AEnter, &QPushButton::clicked, this, &MainMenu::tryAutorization);
}

void MainMenu::tryAutorization()
{
    if(ui->lineEdit_Login->text().remove(" ").length() > 0 && ui->lineEdit_Password->text().remove(" ").length() > 0)
    {
        db = QSqlDatabase::database("dota2");
        if(getDBConnection(db))
        {
            QSqlQuery query(db);
            query.prepare("select id from player inner join authorization on player.id = authorization.id_player where player.player_name = ? and authorization.password = ?;");
            query.addBindValue(ui->lineEdit_Login->text());
            QString userLoginValue = ui->lineEdit_Login->text();
            GlobalVars::userLogin = userLoginValue;
            query.addBindValue(ui->lineEdit_Password->text());
            query.exec();
            if(query.next())
            {
                m_PlayerID = query.value(0).toString();
                ui->lineEdit_Login->clear();
                ui->lineEdit_Password->clear();
                disconnect(ui->PB_AEnter, &QPushButton::clicked, this, &MainMenu::tryAutorization);
                ui->Autorization->setVisible(false);
                ui->frame_main->setEnabled(true);
                showMainMenu();
            }
            else
            {
                QList<QMessageBox::Button> buttons;
                buttons.append(QMessageBox::Ok);
                buttons.append(QMessageBox::Cancel);
                if(showMessage("Нет игроков с такими данными.\nВы хотите создать новый аккаунт?", "Confirm choice", buttons, QMessageBox::Question) == QMessageBox::Ok)
                {
                    query.prepare("insert into player (player_name, registration) values (?, ?);");
                    query.addBindValue(ui->lineEdit_Login->text());
                    query.addBindValue(QDate::currentDate().toString("yyyy-MM-dd"));
                    if(query.exec())
                    {
                        query.exec("select max(id) from player;");
                        if(query.next())
                        {
                            m_PlayerID = query.value(0).toString();
                            query.prepare("insert into authorization (id_player, password) values (?, ?);");
                            query.addBindValue(m_PlayerID);
                            query.addBindValue(ui->lineEdit_Password->text());
                            if(query.exec())
                            {
                                ui->lineEdit_Login->clear();
                                ui->lineEdit_Password->clear();
                                disconnect(ui->PB_AEnter, &QPushButton::clicked, this, &MainMenu::tryAutorization);
                                ui->Autorization->setVisible(false);
                                ui->frame_main->setEnabled(true);
                                showMainMenu();
                            }
                            else
                            {
                                showMessage("Something went wrong while creating new account.\nWe're sorry, please, try again later.", "Unexpected error", QMessageBox::Button::Ok, QMessageBox::Question);
                                ui->lineEdit_Login->clear();
                                ui->lineEdit_Password->clear();
                                ui->lineEdit_Login->setFocus();
                            }
                        }
                        else
                        {
                            showMessage("Something went wrong while creating new account.\nWe're sorry, please, try again later.", "Unexpected error", QMessageBox::Button::Ok, QMessageBox::Question);
                            ui->lineEdit_Login->clear();
                            ui->lineEdit_Password->clear();
                            ui->lineEdit_Login->setFocus();
                        }
                    }
                    else
                    {
                        showMessage("Something went wrong while creating new account.\nWe're sorry, please, try again later.", "Unexpected error", QMessageBox::Button::Ok, QMessageBox::Question);
                        ui->lineEdit_Login->clear();
                        ui->lineEdit_Password->clear();
                        ui->lineEdit_Login->setFocus();
                    }
                }
                else
                {
                    ui->lineEdit_Password->clear();
                    ui->lineEdit_Password->setFocus();
                }
            }
        }
    }
}

void MainMenu::updatePlayerStatus(int value)
{
    db = QSqlDatabase::database("dota2");
    if(getDBConnection(db))
    {
        QSqlQuery query(db);
        query.prepare("update player set status = ? where id = ?;");
        query.addBindValue(value);
        query.addBindValue(m_PlayerID);
        query.exec();
    }
}

void MainMenu::deleteUser()
{
    deleteuser *del = new deleteuser;
    del->show();
    ui->frame_profile->setGeometry (ui-> frame_profile ->x(),1000, ui->frame_profile->width(), ui->frame_profile->height ());
    ui->LB_OpenMainMenu->setGeometry (ui-> LB_OpenMainMenu ->x(),1000, ui->LB_OpenMainMenu->width(), ui->LB_OpenMainMenu->height ());
    ui->frame->setGeometry (ui-> frame ->x(),1000, ui->frame->width(), ui->frame->height ());
    ui->frame_2->setGeometry (ui-> frame_2 ->x(),1000, ui->frame_2->width(), ui->frame_2->height ());
    ui->frame_main -> setGeometry (ui-> frame_main ->x(),721, ui->frame_main->width(), ui->frame_main->height ());
    ui->Heroes -> setGeometry (ui-> Heroes ->x(),721, ui->Heroes->width(), ui->Heroes->height ());

    ui->frame_profile->setEnabled(false);
    //ui->LB_OpenMainMenu->setEnabled(false);
    //ui->frame->setEnabled(false);
    //ui->frame_main->setEnabled(false);
    //ui->frame_2->setEnabled(false);
    connect(del, &deleteuser::userDeleted, [=]()
            {
                del->hide();
                QList<QMessageBox::Button>buttons={QMessageBox::Ok};
                showMessage("Аккаунт успешно удалён!","",QMessageBox::Information,buttons);
                showAutorization();
            });
    connect(del, &deleteuser::userDeleteCancel, [=]()
            {
                del->hide();
                ui->LB_OpenMainMenu->setGeometry (ui-> LB_OpenMainMenu ->x(),0, ui->LB_OpenMainMenu->width(), ui->LB_OpenMainMenu->height ());
                ui->frame->setGeometry (ui-> frame ->x(),0, ui->frame->width(), ui->frame->height ());
                ui->frame_2->setGeometry (ui-> frame_2 ->x(),0, ui->frame_2->width(), ui->frame_2->height ());
                ui->frame_profile->setEnabled(true);
                ui->LB_OpenMainMenu->setEnabled(true);
                ui->frame->setEnabled(true);
                ui->frame_main->setEnabled(true);
                ui->frame_2->setEnabled(true);
                showPlayerProfile();
            });
}

void MainMenu::selectProfileAvatar(QString ID)
{
    QStringList imageName;
    QFileDialog fd;
    fd.setAcceptMode(QFileDialog::AcceptOpen);
    fd.setViewMode(QFileDialog::Detail);
    fd.setFileMode(QFileDialog::ExistingFile);
    fd.setWindowTitle("Select new profile's avatar");
    fd.setNameFilter("Image files (*.png *.jpeg *.jpg)");
    if(fd.exec())
    {
        imageName = fd.selectedFiles();
    }
    if(!imageName.isEmpty())
    {
        QPixmap image;
        image.load(imageName.at(0));
        image.scaled(QSize(200,200), Qt::KeepAspectRatioByExpanding);
        db = QSqlDatabase::database("dota2");
        if(getDBConnection(db))
        {
            QByteArray arr;
            QBuffer buffer(&arr);
            buffer.open(QIODevice::WriteOnly);
            image.save(&buffer, "PNG");

            QSqlQuery query(db);
            query.prepare("update player set avatar = ? where id = ?;");
            query.addBindValue(arr);
            query.addBindValue(ID);
            if(query.exec())
            {
                ui->LB_ProfileAvatar->setPixmap(image);
            }
        }
    }
}

void MainMenu::selectProfileBackground(QString ID)
{
    QStringList imageName;
    QFileDialog fd;
    fd.setAcceptMode(QFileDialog::AcceptOpen);
    fd.setViewMode(QFileDialog::Detail);
    fd.setFileMode(QFileDialog::ExistingFile);
    fd.setWindowTitle("Select new profile's avatar");
    fd.setNameFilter("Image files (*.png *.jpeg *.jpg)");
    if(fd.exec())
    {
        imageName = fd.selectedFiles();
    }
    if(!imageName.isEmpty())
    {
        QPixmap image;
        image.load(imageName.at(0));
        image.scaled(QSize(200,200), Qt::KeepAspectRatioByExpanding);
        db = QSqlDatabase::database("dota2");
        if(getDBConnection(db))
        {
            QByteArray arr;
            QBuffer buffer(&arr);
            buffer.open(QIODevice::WriteOnly);
            image.save(&buffer, "PNG");

            QSqlQuery query(db);
            query.prepare("update player set background = ? where id = ?;");
            query.addBindValue(arr);
            query.addBindValue(ID);
            if(query.exec())
            {
                ui->LB_ProfileBackground->setPixmap(image);
            }
        }
    }
}

QPixmap MainMenu::getImageFromDB(QByteArray arr)
{
    QPixmap image;
    image.loadFromData(arr);
    return image;
}

void MainMenu::getInfoAboutPlayer(QString ID)
{
    db = QSqlDatabase::database("dota2");
    if(getDBConnection(db))
    {
        QSqlQuery query(db);
        query.prepare("select player_name, player.avatar, background, player_status, g_wins, g_lose, registration, player_level, money from player where id = ?;");
        query.addBindValue(ID);
        query.exec();
        if(query.next())
        {
            ui->LB_PlayerName->setText(query.value(0).toString());
            ui->LB_ProfileNickname->setText(query.value(0).toString());
            ui->LB_ProfileBackground->setPixmap(getImageFromDB(query.value(2).toByteArray()));
            ui->LB_Background->setPixmap(getImageFromDB(query.value(2).toByteArray()));
            ui->LB_ProfileAvatar->setPixmap(getImageFromDB(query.value(1).toByteArray()));
            ui->PB_PlayerAvatar->setPixmap(getImageFromDB(query.value(1).toByteArray()));
            ui->LB_PlayerStatus->setText(query.value(3).toString());
            ui->LB_ProfileStatus->setText(query.value(3).toString());
            ui->label_9->setText(query.value(4).toString());
            ui->LB_ProfileWinsValue->setText(query.value(5).toString());
            ui->LB_ProfileDateVaue->setText(query.value(6).toString());
            ui->LB_ProfileLevelValue->setText(query.value(7).toString());
            ui->LB_Money->setText(query.value(8).toString());
        }
     }
}

void MainMenu::addNewHero()
{
    QList<QString> ID = getSelectedIDRoles();

    if(ui->DOTA_Hero_Add_Name->text().remove(" ").length() > 0 && ui->DOTA_Hero_Add_Description->toPlainText().remove(" ").length() > 0 &&
        ui->DOTA_Hero_Add_Type->currentIndex() > -1 && ui->DOTA_Hero_Add_Side->currentIndex() > -1 &&
        ui->DOTA_Hero_Add_BA->value() > 0 && ui->DOTA_Hero_Add_BS->value() > 0 && ui->DOTA_Hero_Add_BI->value() > 0 &&
        ui->DOTA_Hero_Add_SG->value() > 0 && ui->DOTA_Hero_Add_AG->value() > 0 && ui->DOTA_Hero_Add_IG->value() > 0 &&
        !ui->DOTA_Hero_Add_Icon->pixmap().isNull())
    {
        db = QSqlDatabase::database("dota2");
        if(getDBConnection(db))
        {
            QString ID_Type, ID_Side;
            QSqlQuery query(db);
            query.prepare("select id from type where typevalue = ?;");
            query.addBindValue(ui->DOTA_Hero_Add_Type->currentText());
            query.exec();
            if(query.next())
            {
                ID_Type = query.value(0).toString();
            }
            else
            {
                showMessage("1", "error", QMessageBox::Ok, QMessageBox::Critical);
                return;
            }
            query.prepare("select id from side where sidevalue = ?;");
            query.addBindValue(ui->DOTA_Hero_Add_Side->currentText());
            query.exec();
            if(query.next())
            {
                ID_Side = query.value(0).toString();
            }
            else
            {
                showMessage("error 0xc99f: can't get ID hero's side", "error", QMessageBox::Ok, QMessageBox::Critical);
                return;
            }
            QPixmap image = ui->DOTA_Hero_Add_Icon->pixmap();
            QByteArray arr;
            QBuffer buffer(&arr);
            buffer.open(QIODevice::WriteOnly);
            image.save(&buffer, "PNG");
            query.prepare("insert into hero (name, description, type, side, icon, b_strength, b_agility, b_intelligence, g_strength, g_agility, g_intelligence) values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);");
            query.addBindValue(ui->DOTA_Hero_Add_Name->text());
            query.addBindValue(ui->DOTA_Hero_Add_Description->toPlainText());
            query.addBindValue(ID_Type);
            query.addBindValue(ID_Side);
            query.addBindValue(arr);
            query.addBindValue(ui->DOTA_Hero_Add_BS->value());
            query.addBindValue(ui->DOTA_Hero_Add_BA->value());
            query.addBindValue(ui->DOTA_Hero_Add_BI->value());
            query.addBindValue(ui->DOTA_Hero_Add_SG->value());
            query.addBindValue(ui->DOTA_Hero_Add_AG->value());
            query.addBindValue(ui->DOTA_Hero_Add_IG->value());
            if(query.exec())
            {
                QString currentIDHero;
                query.exec("select max(id) from hero;");
                if(query.next())
                {
                    currentIDHero = query.value(0).toString();

                    for(QString id : ID)
                    {
                        query.prepare("insert into heroesrole (id_hero, id_role) values (?, ?);");
                        query.addBindValue(currentIDHero);
                        query.addBindValue(id);
                        query.exec();
                    }
                    showMessage("confirm. new hero has been added.", "success", QMessageBox::Ok, QMessageBox::Information);
                    clearHeroAddingLines();
                    loadHeroes();
                }
            }
        }
    }
}

void MainMenu::loadHeroes()
{
    db = QSqlDatabase::database("dota2");
    if(getDBConnection(db))
    {
        QSqlQuery query(db);

        query.prepare("select background from player where id = ?;");
        query.addBindValue(m_PlayerID);
        query.exec();
        if(query.next())
        {
            if(!query.value(0).toByteArray().isEmpty())
            {
                ui->DOTA_HeroesMainImage->setPixmap(getImageFromDB(query.value(0).toByteArray()));
            }
        }

        if(ui->DOTA_Hero_Add_Type->count() == 0)
        {
            query.exec("select typevalue from type;");
            while(query.next())
            {
                ui->DOTA_Hero_Add_Type->addItem(query.value(0).toString());
            }
            ui->DOTA_Hero_Add_Type->setCurrentIndex(-1);

            query.exec("select sidevalue from side;");
            while(query.next())
            {
                ui->DOTA_Hero_Add_Side->addItem(query.value(0).toString());
            }
            ui->DOTA_Hero_Add_Side->setCurrentIndex(-1);

            ui->DOTA_Hero_Add_Role_List->setColumnHidden(1, true);
            ui->DOTA_Hero_Add_Role_List->setColumnWidth(0, 35);

            query.exec("select * from role;");
            while(query.next())
            {
                ui->DOTA_Hero_Add_Role_List->insertRow(ui->DOTA_Hero_Add_Role_List->rowCount());
                QCheckBox *chkb = new QCheckBox();
                chkb->setCheckState(Qt::Unchecked);
                ui->DOTA_Hero_Add_Role_List->setCellWidget(ui->DOTA_Hero_Add_Role_List->rowCount()-1, 0, chkb);
                ui->DOTA_Hero_Add_Role_List->setItem(ui->DOTA_Hero_Add_Role_List->rowCount()-1, 1, new QTableWidgetItem(query.value(0).toString()));
                ui->DOTA_Hero_Add_Role_List->setItem(ui->DOTA_Hero_Add_Role_List->rowCount()-1, 2, new QTableWidgetItem(query.value(1).toString()));
            }
            ui->DOTA_Hero_Add_Role_List->resizeRowsToContents();
        }

        clearHeroInfoLines();

        clearRowsInTable(ui->DOTA_Heroes_SR);
        clearRowsInTable(ui->DOTA_Heroes_IR);
        clearRowsInTable(ui->DOTA_Heroes_AR);
        clearRowsInTable(ui->DOTA_Heroes_SD);
        clearRowsInTable(ui->DOTA_Heroes_AD);
        clearRowsInTable(ui->DOTA_Heroes_ID);

        // clearRowsInTable(ui->DOTA_Profile_SR);
        // clearRowsInTable(ui->DOTA_Profile_IR);
        // clearRowsInTable(ui->DOTA_Profile_AR);
        // clearRowsInTable(ui->DOTA_Profile_SD);
        // clearRowsInTable(ui->DOTA_Profile_AD);
        // clearRowsInTable(ui->DOTA_Profile_INTD);

        query.exec("select id, icon, type, side from hero;");
        while(query.next())
        {
            switch(query.value(2).toInt())
            {
            case 1:
            {
                switch(query.value(3).toInt())
                {
                case 1:
                {
                    addHeroToTable(ui->DOTA_Heroes_SR, query);
                    //addHeroToTable(ui->DOTA_Profile_SR, query);
                    break;
                }
                case 2:
                {
                    addHeroToTable(ui->DOTA_Heroes_SD, query);
                    //addHeroToTable(ui->DOTA_Profile_SD, query);
                    break;
                }
                }
                break;
            }
            case 2:
            {
                switch(query.value(3).toInt())
                {
                case 1:
                {
                    addHeroToTable(ui->DOTA_Heroes_AR, query);
                    //addHeroToTable(ui->DOTA_Profile_AR, query);
                    break;
                }
                case 2:
                {
                    addHeroToTable(ui->DOTA_Heroes_AD, query);
                    //addHeroToTable(ui->DOTA_Profile_AD, query);
                    break;
                }
                }
                break;
            }
            case 3:
            {
                switch(query.value(3).toInt())
                {
                case 1:
                {
                    addHeroToTable(ui->DOTA_Heroes_IR, query);
                    //addHeroToTable(ui->DOTA_Profile_IR, query);
                    break;
                }
                case 2:
                {
                    addHeroToTable(ui->DOTA_Heroes_ID, query);
                    //addHeroToTable(ui->DOTA_Profile_INTD, query);
                    break;
                }
                }
                break;
            }
            }
        }
    }
}

QList<QString> MainMenu::getSelectedIDRoles()
{
    QList<QString> founded;
    for(int i = 0; i < ui->DOTA_Hero_Add_Role_List->rowCount(); i++)
    {
        if(qobject_cast<QCheckBox*>(ui->DOTA_Hero_Add_Role_List->cellWidget(i, 0))->checkState() == Qt::Checked)
        {
            founded.append(ui->DOTA_Hero_Add_Role_List->item(i, 1)->text());
        }
    }
    return founded;
}

void MainMenu::clearHeroAddingLines()
{
    ui->DOTA_Hero_Add_Name->clear();
    ui->DOTA_Hero_Add_Description->clear();
    ui->DOTA_Hero_Add_Type->setCurrentIndex(-1);
    ui->DOTA_Hero_Add_Side->setCurrentIndex(-1);
    ui->DOTA_Hero_Add_Icon->clear();
    ui->DOTA_Hero_Add_BS->setValue(0);
    ui->DOTA_Hero_Add_BA->setValue(0);
    ui->DOTA_Hero_Add_BI->setValue(0);
    ui->DOTA_Hero_Add_SG->setValue(0.0);
    ui->DOTA_Hero_Add_AG->setValue(0.0);
    ui->DOTA_Hero_Add_IG->setValue(0.0);

    // for(int i = 0; i < ui->DOTA_Hero_Add_Role_List->rowCount(); i++)
    // {
    //     QCheckBox* chkb = qobject_cast<QCheckBox*>(ui->DOTA_Hero_Add_Role_List->cellWidget(i, 0));

    //     if(chkb->checkState() == Qt::Checked)
    //     {
    //         chkb->setCheckState(Qt::Unchecked);
    //         ui->DOTA_Hero_Add_Role_List->setCellWidget(i, 0, chkb);
    //     }
    // }
}

void MainMenu::clearHeroInfoLines()
{
    ui->DOTA_Hero_Name->clear();
    ui->DOTA_Hero_About->clear();
    ui->DOTA_Hero_Icon->clear();
    ui->DOTA_Hero_Basic_Type->clear();

    ui->DOTA_Hero_Strength->clear();
    ui->DOTA_Hero_Agility->clear();
    ui->DOTA_Hero_Intelligence->clear();
    //ui->DOTA_Hero_Role->clear();
}

void MainMenu::clearRowsInTable(QTableWidget *table)
{
    int rowCount = table->rowCount();
    for(int i = 0; i < rowCount; i++)
    {
        table->removeRow(0);
    }
}

void MainMenu::addHeroToTable(QTableWidget *table, QSqlQuery query)
{
    int x = -1,
        y = -1;

    if(table->rowCount() == 0)
    {
        table->insertRow(0);
        table->setItem(0,0, new QTableWidgetItem());
        HeroItem *hero = new HeroItem();
        QPixmap image;
        image.loadFromData(query.value(1).toByteArray());
        hero->loadData(query.value(0).toString(), image);
        if(!table->objectName().contains("DOTA_Profile_"))
            connect(hero, &HeroItem::clicked, this, &MainMenu::loadInfoAboutHero);
        //else
            //connect(hero, &HeroItem::clicked, this, &MainMenu::updateProfileFavHero);
        table->setCellWidget(0,0, hero);
    }
    else
    {
        for(int i = 0; i < table->rowCount(); i++)
        {
            for(int j = 0; j < table->columnCount(); j++)
            {
                QTableWidgetItem *item1(table->item(i, j));
                if (item1) {
                    continue;
                }
                else
                {
                    x = i;
                    y = j;
                    break;
                }
            }
        }
        if(x == -1 || y == -1)
        {
            table->insertRow(table->rowCount());
            table->setItem(table->rowCount()-1,0, new QTableWidgetItem());
            HeroItem *hero = new HeroItem();
            QPixmap image;
            image.loadFromData(query.value(1).toByteArray());
            hero->loadData(query.value(0).toString(), image);
            if(!table->objectName().contains("DOTA_Profile_"))
                connect(hero, &HeroItem::clicked, this, &MainMenu::loadInfoAboutHero);
            else
                //connect(hero, &HeroItem::clicked, this, &MainMenu::updateProfileFavHero);
            table->setCellWidget(table->rowCount()-1,0, hero);
        }
        else
        {
            table->insertRow(table->rowCount());
            table->setItem(x, y, new QTableWidgetItem());
            HeroItem *hero = new HeroItem();
            QPixmap image;
            image.loadFromData(query.value(1).toByteArray());
            hero->loadData(query.value(0).toString(), image);
            if(!table->objectName().contains("DOTA_Profile_"))
                connect(hero, &HeroItem::clicked, this, &MainMenu::loadInfoAboutHero);
            else
                //connect(hero, &HeroItem::clicked, this, &MainMenu::updateProfileFavHero);
            table->setCellWidget(x, y, hero);
        }
    }
}

void MainMenu::loadInfoAboutHero(QString ID)
{
    clearHeroInfoLines();

    db = QSqlDatabase::database("dota2");
    if(getDBConnection(db))
    {
        QSqlQuery query(db);
        query.prepare("select hero.id, hero.name, hero.description, type.typevalue, side.sidevalue, hero.icon, concat(hero.b_strength, ' + ', hero.g_strength) as HeroStrength, concat(hero.b_agility, ' + ', hero.g_agility) as HeroAgility, concat(hero.b_intelligence, ' + ' , hero.g_intelligence) as HeroIntelligence from hero inner join type on type.id = hero.type inner join side on side.id = hero.side where hero.id = ?;");
        query.addBindValue(ID);
        query.exec();
        if(query.next())
        {
            QString currentHeroID = query.value(0).toString();
            ui->DOTA_Hero_Name->setText(query.value(1).toString());
            ui->DOTA_Hero_About->setText(query.value(2).toString());
            ui->DOTA_Hero_Icon->setPixmap(getImageFromDB(query.value(5).toByteArray()));
            ui->DOTA_Hero_Basic_Type->setText(query.value(4).toString());

            ui->DOTA_Hero_Strength->setText(query.value(6).toString());
            ui->DOTA_Hero_Agility->setText(query.value(7).toString());
            ui->DOTA_Hero_Intelligence->setText(query.value(8).toString());

            query.prepare("select rolevalue from role inner join heroesrole on heroesrole.id_role = role.id where heroesrole.id_hero = ?;");
            query.addBindValue(currentHeroID);
            query.exec();
            QString roles;
            while(query.next())
            {
                roles += query.value(0).toString() + ", ";
            }

            if(roles.length() > 2)
            {
                roles.removeAt(roles.length()-2);
                ui->DOTA_Hero_Role->setText(roles);
            }
        }
    }
}

void MainMenu::selectHeroIcon()
{
    QStringList imageName;
    QFileDialog fd;
    fd.setAcceptMode(QFileDialog::AcceptOpen);
    fd.setViewMode(QFileDialog::Detail);
    fd.setFileMode(QFileDialog::ExistingFile);
    fd.setWindowTitle("Выберите иконку героя");
    fd.setNameFilter("Image files (*.png *.jpeg *.jpg)");
    if(fd.exec())
    {
        imageName = fd.selectedFiles();
    }
    if(!imageName.isEmpty())
    {
        QPixmap image;
        image.load(imageName.at(0));
        ui->DOTA_Hero_Add_Icon->setPixmap(image);
    }
}
