#include "playershortinfo.h"
#include "ui_playershortinfo.h"

Playershortinfo::Playershortinfo(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Playershortinfo)
{
    ui->setupUi(this);
    connect(ui->DOTA_FriendPlayerAvatar, &QPushLabel::clicked, this, &Playershortinfo::getClicked);
}

Playershortinfo::~Playershortinfo()
{
    delete ui;
}

void Playershortinfo::loadData(QString id, QString name, QString status, QPixmap image)
{
    this->id = id;
    ui->DOTA_FriendPlayerName->setText(name);
    ui->DOTA_FriendPlayerStatus->setText(status);
    ui->DOTA_FriendPlayerAvatar->setPixmap(image);
}

void Playershortinfo::getClicked()
{
    emit clicked(id);
}
