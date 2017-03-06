#ifndef SENDFILE_H
#define SENDFILE_H

#include <QMainWindow>

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#include <QFileDialog>

#include <QTimer>

namespace Ui {
class SendFile;
}

class SendFile : public QMainWindow
{
    Q_OBJECT

public:
    explicit SendFile(QWidget *parent = 0);
    ~SendFile();


private:
    Ui::SendFile *ui;

    int fileSize;
    int pkgsize;        //字节
    int pkgrate;        //ms

    int pkgnum;
    int pkgcnt;

    QSerialPort *my_port;

    QFile *sendfile;
    int sendedByte;


    QByteArray txtcon;

    QTimer *sendTimer;

private slots:
    void slt_comOpen();
    void slt_sendFile();
    void slt_loadFile();

    void slt_sizechange(QString str);
    void slt_ratechange(QString str);

    void slt_timCycle();
};

#endif // SENDFILE_H
