#include "widget.h"
#include "ui_widget.h"
#include <iostream>

using namespace std;

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    ui->pbDisconnect->setEnabled(false);
    ui->pbClear->setEnabled(false);
    ui->pbSend->setEnabled(false);

    socket = nullptr;

}

Widget::~Widget()
{
    delete ui;
}

void Widget::doConnected(){
    QString msg = "Connected to " + socket->peerAddress().toString() +"\n";
    ui->pteMessage->insertPlainText(msg);
}

void Widget::doDisconnected(){
    QString msg = "Disconnected to " + socket->peerAddress().toString() +"\n";
    ui->pteMessage->insertPlainText(msg);
}

void Widget::doReadyRead(){
    QByteArray ba = socket->readAll();
    ui->pteMessage->insertPlainText(ba);
}

void Widget::on_pbConnect_clicked()
{
    ui->pbSend->setEnabled(true);
    if (ui->rbTCP->isChecked()) {
        socket = new QTcpSocket(this);
        socket->connectToHost(ui->leHost->text(), ui->lePort->text().toUShort());
    } else if (ui->rbSSL->isChecked()) {
        socket = new QSslSocket(this);
        static_cast<QSslSocket*>(socket)->connectToHostEncrypted(ui->leHost->text(), ui->lePort->text().toUShort());
    } else {
        cerr << "Connection type isn't selected" << endl;
        return;
    }

    QObject::connect(socket, &QAbstractSocket::connected, this, &Widget::doConnected);
    QObject::connect(socket, &QAbstractSocket::disconnected, this, &Widget::doDisconnected);
    QObject::connect(socket, &QAbstractSocket::readyRead, this, &Widget::doReadyRead);

    buttonChanged(socket->state());

}

void Widget::buttonChanged(QAbstractSocket::SocketState state)
{
    ui->pbDisconnect->setEnabled(state == QAbstractSocket::ConnectedState);
    ui->pbClear->setEnabled(!ui->pteMessage->toPlainText().isEmpty());
}


void Widget::on_pbDisconnect_clicked()
{
    socket->close();
    buttonChanged(socket->state());
}


void Widget::on_pbSend_clicked()
{
    socket->write(ui->pteSend->toPlainText().toUtf8()); // to byte arrra
    buttonChanged(socket->state());
}


void Widget::on_pbClear_clicked()
{
    ui->pteMessage->clear();
    buttonChanged(socket->state());
}





