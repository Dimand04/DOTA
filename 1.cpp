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
