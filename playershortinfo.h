#ifndef PLAYERSHORTINFO_H
#define PLAYERSHORTINFO_H

#include <QtWidgets/QWidget>

namespace Ui {
class Playershortinfo;
}

class Playershortinfo : public QWidget
{
    Q_OBJECT

public:
    explicit Playershortinfo(QWidget *parent = nullptr);
    ~Playershortinfo();
    void loadData(QString, QString, QString, QPixmap);
    QString getID() { return id; }
public slots:
    void getClicked();
signals:
    void clicked(QString);
private:
    Ui::Playershortinfo *ui;

    QString id;
};

#endif // PLAYERSHORTINFO_H
