#include "sendfile.h"
#include "ui_sendfile.h"
#include <QDebug>

void HexString(quint8 vhex,QByteArray &vba);

SendFile::SendFile(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SendFile)
{
    ui->setupUi(this);

    //自适应串口号
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        qDebug() << "Name         : " << info.portName();
        qDebug() << "Description  : " << info.description();
        qDebug() << "Manufacturer : " << info.manufacturer();
        qDebug() << "Serial number: " << info.serialNumber();
        ui->cmb_com->addItem(info.portName());
    }
    my_port=new QSerialPort();

    sendTimer=new QTimer(this);
    pkgsize=0;
    pkgrate=0;

    connect(ui->btn_open,SIGNAL(clicked(bool)),this,SLOT(slt_comOpen()));
    connect(ui->btn_fileload,SIGNAL(clicked(bool)),SLOT(slt_loadFile()));
    connect(ui->btn_filesend,SIGNAL(clicked(bool)),SLOT(slt_sendFile()));

    connect(ui->le_pksize,SIGNAL(textChanged(QString)),this,SLOT(slt_sizechange(QString)));
    connect(ui->le_pkrate,SIGNAL(textChanged(QString)),this,SLOT(slt_ratechange(QString)));

    connect(sendTimer,SIGNAL(timeout()),this,SLOT(slt_timCycle()));
}

SendFile::~SendFile()
{
    delete ui;
}

void SendFile::slt_comOpen()
{
    if(ui->btn_open->text()=="open")
    {
        if(ui->cmb_com->count()==0) return;     //没有可用的串口

        my_port->setPortName(ui->cmb_com->currentText());
        if(my_port->open(QIODevice::ReadWrite))     //串口打开成功
        {
            my_port->setBaudRate(ui->cmb_barate->currentText().toInt());

            ui->btn_open->setText("close");

            ui->cmb_com->setEnabled(false);
            ui->cmb_barate->setEnabled(false);
        }
        else
        {
            qDebug()<<"open com err";
        }
    }
    else
    {
        my_port->close();
        ui->btn_open->setText("open");

        ui->cmb_com->setEnabled(true);
        ui->cmb_barate->setEnabled(true);
    }
}

void SendFile::slt_loadFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),QDir::currentPath(),tr("ALL File(*);;TEXT File(*.txt);;JPG File(*.jpg);;PNG Files(*.png)"));
    if (fileName.isEmpty())
        return;

    ui->le_filename->setText(fileName);

    sendfile=new QFile(fileName);
    if (!sendfile->open(QFile::ReadOnly)) {
        qDebug("Cannot read file %s:\n%s.",fileName,sendfile->errorString());
        return;
    }

    txtcon=sendfile->readAll();
    //QString strTxt=txtcon;
    fileSize=txtcon.size();     //文件大小
    qDebug("fileSize=%d",fileSize);

    QByteArray showhex;
    foreach(quint8 chr,txtcon)
    {
        HexString(chr,showhex);
    }

    ui->te_hexshow->setPlainText(showhex);
}

void SendFile::slt_sendFile()
{
    if(pkgsize==0||pkgrate==0)
        return;

    sendedByte=0;
    ui->le_pksize->setEnabled(false);
    ui->le_pkrate->setEnabled(false);
    ui->le_filename->setEnabled(false);
    ui->btn_fileload->setEnabled(false);
    ui->btn_filesend->setEnabled(false);
    //sendTimer->setInterval(pkgrate);
    //sendTimer->set
    sendTimer->start(pkgrate);
    slt_timCycle();
}

void SendFile::slt_timCycle()
{
    int remainlen=fileSize-sendedByte;
    qDebug("remain=%d",remainlen);
    if(remainlen>=pkgsize)  remainlen=pkgsize;

    QByteArray sendBa=txtcon.mid(sendedByte,remainlen);

    my_port->write(sendBa,remainlen);
    sendedByte+=remainlen;

    int sendpro=sendedByte*100/fileSize;
    ui->pb_send->setValue(sendpro);

    qDebug("sendedByte=%d",sendedByte);

    if(sendedByte==fileSize)
    {
        qDebug()<<"send end";
        sendTimer->stop();
        ui->le_pksize->setEnabled(true);
        ui->le_pkrate->setEnabled(true);
        ui->le_filename->setEnabled(true);
        ui->btn_fileload->setEnabled(true);
        ui->btn_filesend->setEnabled(true);
    }
}

void SendFile::slt_sizechange(QString str)
{
    //if(str.isEmpty())
    pkgsize=str.toInt();
    qDebug("pkgsize=%d",pkgsize);

//    pkgnum=fileSize/pkgsize;
//    if((fileSize%pkgsize)!=0)
//        pkgnum++;
}

void SendFile::slt_ratechange(QString str)
{
    pkgrate=str.toInt();
    qDebug("pkgrate=%d",pkgrate);
}


void HexString(quint8 vhex,QByteArray &vba)
{
    quint8 vh=vhex/16;
    quint8 vl=vhex%16;

    if(vh>=0&&vh<=9)    vh=vh+'0';
    else if(vh>=10&&vh<=15) vh=vh+'a'-10;

    if(vl>=0&&vl<=9)    vl=vl+'0';
    else if(vl>=10&&vl<=15) vl=vl+'a'-10;

    //qDebug("vh=%c,vl=%c",vh,vl);

    vba.append(vh);
    vba.append(vl);
    vba.append(0x20);
}
