#include "heroitem.h"
#include "ui_heroitem.h"

HeroItem::HeroItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HeroItem)
{
    ui->setupUi(this);
    connect(ui->DOTA_Hero_Icon, &QPushLabel::clicked, this, &HeroItem::getClicked);
}

HeroItem::~HeroItem()
{
    delete ui;
}

void HeroItem::loadData(QString id, QPixmap image)
{
    ID = id;
    ui->DOTA_Hero_Icon->setPixmap(image);
}

void HeroItem::getClicked()
{
    emit clicked(ID);
}
