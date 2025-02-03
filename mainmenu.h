#ifndef MAINMENU_H
#define MAINMENU_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QProcess>
#include <QSqlQuery>
#include <QDate>
#include <QtWidgets/QMainWindow>
#include <QListWidget>
#include <QTableWidget>
#include <QBuffer>
#include <QFileDialog>
#include <QTimer>
#include "heroitem.h"
#include "global.h"
#include <exception>


QT_BEGIN_NAMESPACE
namespace Ui { class MainMenu; }
QT_END_NAMESPACE

class MainMenu : public QMainWindow
{
    Q_OBJECT

public:
    MainMenu(QWidget *parent = nullptr);
    ~MainMenu();
public slots:
    void showMainMenu();
    void showHeroesMenu();
    void showPlayerProfile();
    void showAutorization();
    void tryAutorization();
    void selectProfileAvatar(QString);
    void selectProfileBackground(QString);
    void addNewHero();
    void loadHeroes();
    void loadInfoAboutHero(QString);
    void selectHeroIcon();
    void deleteUser();

private:
    Ui::MainMenu *ui;
    QProcess *test;
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", "dota2");
    QString m_PlayerID,
            m_currentFrame,
            currentIDPlayer;
    int m_selectedFavHero = 1;
    enum class PlayerStatus { MainMenu = 1, NotHere, Unranked, Ranked, Disconnected, InLobby };

private:
    void toggleFrame(const int, QString);
    void toggleFrame(bool);
    void toggleFrame(bool, QString);
    void toggleFrame();
    void updatePlayerStatus(int);
    void getInfoAboutPlayer(QString);
    QPixmap getImageFromDB(QByteArray);
    QPixmap getRankImage(int);
    void clearHeroInfoLines();
    void clearRowsInTable(QTableWidget*);
    void addHeroToTable(QTableWidget*, QSqlQuery);
    QList<QString> getSelectedIDRoles();
    void clearHeroAddingLines();
};
#endif // MAINMENU_H
