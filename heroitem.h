#ifndef HEROITEM_H
#define HEROITEM_H

#include <QtWidgets/QWidget>

namespace Ui { class HeroItem; }

class HeroItem : public QWidget
{
    Q_OBJECT

public:
    explicit HeroItem(QWidget *parent = nullptr);
    ~HeroItem();
    void loadData(QString, QPixmap);
    QString getID() { return ID; }

public slots:
    void getClicked();

signals:
    void clicked(QString);


private:
    Ui::HeroItem *ui;
    QString ID;
};

#endif // HEROITEM_H
