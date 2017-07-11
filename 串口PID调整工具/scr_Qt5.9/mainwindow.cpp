#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QSettings>
#include <QString>
#include <QDir>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags()& ~Qt::WindowMaximizeButtonHint);  //禁止最大化
    setFixedSize(this->width(), this->height());  //禁止调节窗口大小
    qDebug()<<"current applicationDirPath: "<<QCoreApplication::applicationDirPath();
    qDebug()<<"current currentPath: "<<QDir::currentPath();
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())//遍历串口
     {
        ui->Port_Number_comboBox->addItem(info.portName());//在选择框中添加串口
     }
    QSettings *settings = new QSettings("Settings.ini", QSettings::IniFormat);

    QString Type_Flag_String = settings->value("/1_Data/Type_Flag").toString();//读取标识符
    if(!Type_Flag_String.isEmpty())//如果标识符不为空
     {
            ui->data_1_Type_Flag_TextEdit->setText(Type_Flag_String);//写入TextEdit
     }
    QString Min_String = settings->value("/1_Data/Min").toString();//读取Min
    if(!Min_String.isEmpty())//如果不为空
     {
            ui->data_1_Min_TextEdit->setText(Min_String);//写入TextEdit
     }
    QString Max_String = settings->value("/1_Data/Max").toString();//读取Max
    if(!Max_String.isEmpty())//如果不为空
     {
            ui->data_1_Max_TextEdit->setText(Max_String);//写入TextEdit
     }


    Type_Flag_String = settings->value("/2_Data/Type_Flag").toString();//读取标识符
    if(!Type_Flag_String.isEmpty())//如果标识符不为空
    {
        ui->data_2_Type_Flag_TextEdit->setText(Type_Flag_String);//写入TextEdit
    }
    Min_String = settings->value("/2_Data/Min").toString();//读取Min
    if(!Min_String.isEmpty())//如果不为空
    {
        ui->data_2_Min_TextEdit->setText(Min_String);//写入TextEdit
    }
    Max_String = settings->value("/2_Data/Max").toString();//读取Max
    if(!Max_String.isEmpty())//如果不为空
    {
        ui->data_2_Max_TextEdit->setText(Max_String);//写入TextEdit
    }


    Type_Flag_String = settings->value("/3_Data/Type_Flag").toString();//读取标识符
    if(!Type_Flag_String.isEmpty())//如果标识符不为空
    {
        ui->data_3_Type_Flag_TextEdit->setText(Type_Flag_String);//写入TextEdit
    }
    Min_String = settings->value("/3_Data/Min").toString();//读取Min
    if(!Min_String.isEmpty())//如果不为空
    {
        ui->data_3_Min_TextEdit->setText(Min_String);//写入TextEdit
    }
    Max_String = settings->value("/3_Data/Max").toString();//读取Max
    if(!Max_String.isEmpty())//如果不为空
    {
        ui->data_3_Max_TextEdit->setText(Max_String);//写入TextEdit
    }


    Type_Flag_String = settings->value("/4_Data/Type_Flag").toString();//读取标识符
    if(!Type_Flag_String.isEmpty())//如果标识符不为空
    {
        ui->data_4_Type_Flag_TextEdit->setText(Type_Flag_String);//写入TextEdit
    }
    Min_String = settings->value("/4_Data/Min").toString();//读取Min
    if(!Min_String.isEmpty())//如果不为空
    {
        ui->data_4_Min_TextEdit->setText(Min_String);//写入TextEdit
    }
    Max_String = settings->value("/4_Data/Max").toString();//读取Max
    if(!Max_String.isEmpty())//如果不为空
    {
        ui->data_4_Max_TextEdit->setText(Max_String);//写入TextEdit
    }


    Type_Flag_String = settings->value("/5_Data/Type_Flag").toString();//读取标识符
    if(!Type_Flag_String.isEmpty())//如果标识符不为空
    {
        ui->data_5_Type_Flag_TextEdit->setText(Type_Flag_String);//写入TextEdit
    }
    Min_String = settings->value("/5_Data/Min").toString();//读取Min
    if(!Min_String.isEmpty())//如果不为空
    {
        ui->data_5_Min_TextEdit->setText(Min_String);//写入TextEdit
    }
    Max_String = settings->value("/5_Data/Max").toString();//读取Max
    if(!Max_String.isEmpty())//如果不为空
    {
        ui->data_5_Max_TextEdit->setText(Max_String);//写入TextEdit
    }


    Type_Flag_String = settings->value("/6_Data/Type_Flag").toString();//读取标识符
    if(!Type_Flag_String.isEmpty())//如果标识符不为空
    {
        ui->data_6_Type_Flag_TextEdit->setText(Type_Flag_String);//写入TextEdit
    }
    Min_String = settings->value("/6_Data/Min").toString();//读取Min
    if(!Min_String.isEmpty())//如果不为空
    {
        ui->data_6_Min_TextEdit->setText(Min_String);//写入TextEdit
    }
    Max_String = settings->value("/6_Data/Max").toString();//读取Max
    if(!Max_String.isEmpty())//如果不为空
    {
        ui->data_6_Max_TextEdit->setText(Max_String);//写入TextEdit
    }



}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_openButton_clicked()
{
    if(ui->openButton->text()==tr("打开串口"))
    {
        serial = new QSerialPort;
        //设置串口名
        serial->setPortName( ui->Port_Number_comboBox->currentText());
        //打开串口
        serial->open(QIODevice::ReadWrite);
        //设置波特率
        serial->setBaudRate(115200);
        //设置数据位数
        serial->setDataBits(QSerialPort::Data8);//八位数据
        //设置停止位
       serial->setStopBits(QSerialPort::OneStop);//一位停止
        //设置流控制
        serial->setFlowControl(QSerialPort::NoFlowControl); //无流控
        //设置按钮状态
        ui->openButton->setText(tr("关闭串口"));
        //连接信号槽
        QObject::connect(serial, &QSerialPort::readyRead, this, &MainWindow::Read_Data);
    }
    else
    {
        //关闭串口
        serial->clear();
        serial->close();
        serial->deleteLater();
       //设置按钮状态
        ui->openButton->setText(tr("打开串口"));

    }
}

void MainWindow::Read_Data()//读到数据
{
    QByteArray buf;
    buf = serial->readAll();
    if(!buf.isEmpty())
    {
       /*QString str = ui->textEdit->toPlainText();
        str+=tr(buf);
        ui->textEdit->clear();
        ui->textEdit->append(str);*/
    }
    buf.clear();
}





void MainWindow::on_data_1_Min_TextEdit_textChanged()
{
     ui->data_1_value_Qslider->setMinimum(ui->data_1_Min_TextEdit->toPlainText().toFloat()*100000);//设置最小值
}

void MainWindow::on_data_1_Max_TextEdit_textChanged()
{
    ui->data_1_value_Qslider->setMaximum(ui->data_1_Max_TextEdit->toPlainText().toFloat()*100000);//设置最大值
}

void MainWindow::on_data_1_value_Qslider_sliderReleased()//进度条被释放
{
    if(ui->data_1_enable_CheckBox->isChecked())//如果本数据被使能
    {
        if(!(ui->data_1_Type_Flag_TextEdit->toPlainText().isEmpty()))//如果Type_Flag编辑框内容不为空
        {
            ui->data_1_value_QLabel->setText( QString("%1").arg((double)(ui->data_1_value_Qslider->value()/100000.0),10));//将进度条数值放入标签
            DataPacket_Union tmp_buff;//创建一个共用体
            tmp_buff.to_struct.Command=ui->data_1_Type_Flag_TextEdit->toPlainText().toInt();//向共用体中的结构体成员装填数据
            tmp_buff.to_struct.Type=4;//向共用体中的结构体成员装填数据
            tmp_buff.to_struct.Data=ui->data_1_value_QLabel->text().toFloat();//向共用体中的结构体成员装填数据
            tmp_buff.to_struct.head_flag=~0x5FA;//向共用体中的结构体成员装填数据
            tmp_buff.to_struct.size=sizeof(tmp_buff.to_struct)-3;//向共用体中的结构体成员装填数据
           serial->write(tmp_buff.to_char_array,sizeof(tmp_buff.to_char_array));//发送数据
        }
    }
}

void MainWindow::on_data_2_Min_TextEdit_textChanged()
{
    ui->data_2_value_Qslider->setMinimum(ui->data_2_Min_TextEdit->toPlainText().toFloat()*100000);//设置最小值
}

void MainWindow::on_data_2_Max_TextEdit_textChanged()
{
    ui->data_2_value_Qslider->setMaximum(ui->data_2_Max_TextEdit->toPlainText().toFloat()*100000);//设置最大值
}

void MainWindow::on_data_2_value_Qslider_sliderReleased()//进度条被释放
{
    if(ui->data_2_enable_CheckBox->isChecked())//如果本数据被使能
    {
        if(!(ui->data_2_Type_Flag_TextEdit->toPlainText().isEmpty()))//如果Type_Flag编辑框内容不为空
        {
            ui->data_2_value_QLabel->setText( QString("%1").arg((double)(ui->data_2_value_Qslider->value()/100000.0),10));//将进度条数值放入标签
            DataPacket_Union tmp_buff;//创建一个共用体
            tmp_buff.to_struct.Command=ui->data_2_Type_Flag_TextEdit->toPlainText().toInt();//向共用体中的结构体成员装填数据
            tmp_buff.to_struct.Type=4;//向共用体中的结构体成员装填数据
            tmp_buff.to_struct.Data=ui->data_2_value_QLabel->text().toFloat();//向共用体中的结构体成员装填数据
            tmp_buff.to_struct.head_flag=~0x5FA;//向共用体中的结构体成员装填数据
            tmp_buff.to_struct.size=sizeof(tmp_buff.to_struct)-3;//向共用体中的结构体成员装填数据
           serial->write(tmp_buff.to_char_array,sizeof(tmp_buff.to_char_array));//发送数据
        }
    }
}



void MainWindow::on_data_3_Min_TextEdit_textChanged()
{
    ui->data_3_value_Qslider->setMinimum(ui->data_3_Min_TextEdit->toPlainText().toFloat()*100000);//设置最小值
}

void MainWindow::on_data_3_Max_TextEdit_textChanged()
{
    ui->data_3_value_Qslider->setMaximum(ui->data_3_Max_TextEdit->toPlainText().toFloat()*100000);//设置最大值
}

void MainWindow::on_data_3_value_Qslider_sliderReleased()//进度条被释放
{
    if(ui->data_3_enable_CheckBox->isChecked())//如果本数据被使能
    {
        if(!(ui->data_3_Type_Flag_TextEdit->toPlainText().isEmpty()))//如果Type_Flag编辑框内容不为空
        {
            ui->data_3_value_QLabel->setText( QString("%1").arg((double)(ui->data_3_value_Qslider->value()/100000.0),10));//将进度条数值放入标签
            DataPacket_Union tmp_buff;//创建一个共用体
            tmp_buff.to_struct.Command=ui->data_3_Type_Flag_TextEdit->toPlainText().toInt();//向共用体中的结构体成员装填数据
            tmp_buff.to_struct.Type=4;//向共用体中的结构体成员装填数据
            tmp_buff.to_struct.Data=ui->data_3_value_QLabel->text().toFloat();//向共用体中的结构体成员装填数据
            tmp_buff.to_struct.head_flag=~0x5FA;//向共用体中的结构体成员装填数据
            tmp_buff.to_struct.size=sizeof(tmp_buff.to_struct)-3;//向共用体中的结构体成员装填数据
           serial->write(tmp_buff.to_char_array,sizeof(tmp_buff.to_char_array));//发送数据
        }
    }
}


void MainWindow::on_data_4_Min_TextEdit_textChanged()
{
    ui->data_4_value_Qslider->setMinimum(ui->data_4_Min_TextEdit->toPlainText().toFloat()*100000);//设置最小值
}

void MainWindow::on_data_4_Max_TextEdit_textChanged()
{
    ui->data_4_value_Qslider->setMaximum(ui->data_4_Max_TextEdit->toPlainText().toFloat()*100000);//设置最大值
}

void MainWindow::on_data_4_value_Qslider_sliderReleased()//进度条被释放
{
    if(ui->data_4_enable_CheckBox->isChecked())//如果本数据被使能
    {
        if(!(ui->data_4_Type_Flag_TextEdit->toPlainText().isEmpty()))//如果Type_Flag编辑框内容不为空
        {
            ui->data_4_value_QLabel->setText( QString("%1").arg((double)(ui->data_4_value_Qslider->value()/100000.0),10));//将进度条数值放入标签
            DataPacket_Union tmp_buff;//创建一个共用体
            tmp_buff.to_struct.Command=ui->data_4_Type_Flag_TextEdit->toPlainText().toInt();//向共用体中的结构体成员装填数据
            tmp_buff.to_struct.Type=4;//向共用体中的结构体成员装填数据
            tmp_buff.to_struct.Data=ui->data_4_value_QLabel->text().toFloat();//向共用体中的结构体成员装填数据
            tmp_buff.to_struct.head_flag=~0x5FA;//向共用体中的结构体成员装填数据
            tmp_buff.to_struct.size=sizeof(tmp_buff.to_struct)-3;//向共用体中的结构体成员装填数据
           serial->write(tmp_buff.to_char_array,sizeof(tmp_buff.to_char_array));//发送数据
        }
    }
}


void MainWindow::on_data_5_Min_TextEdit_textChanged()
{
    ui->data_5_value_Qslider->setMinimum(ui->data_5_Min_TextEdit->toPlainText().toFloat()*100000);//设置最小值
}

void MainWindow::on_data_5_Max_TextEdit_textChanged()
{
    ui->data_5_value_Qslider->setMaximum(ui->data_5_Max_TextEdit->toPlainText().toFloat()*100000);//设置最大值
}

void MainWindow::on_data_5_value_Qslider_sliderReleased()//进度条被释放
{
    if(ui->data_5_enable_CheckBox->isChecked())//如果本数据被使能
    {
        if(!(ui->data_5_Type_Flag_TextEdit->toPlainText().isEmpty()))//如果Type_Flag编辑框内容不为空
        {
            ui->data_5_value_QLabel->setText( QString("%1").arg((double)(ui->data_5_value_Qslider->value()/100000.0),10));//将进度条数值放入标签
            DataPacket_Union tmp_buff;//创建一个共用体
            tmp_buff.to_struct.Command=ui->data_5_Type_Flag_TextEdit->toPlainText().toInt();//向共用体中的结构体成员装填数据
            tmp_buff.to_struct.Type=4;//向共用体中的结构体成员装填数据
            tmp_buff.to_struct.Data=ui->data_5_value_QLabel->text().toFloat();//向共用体中的结构体成员装填数据
            tmp_buff.to_struct.head_flag=~0x5FA;//向共用体中的结构体成员装填数据
            tmp_buff.to_struct.size=sizeof(tmp_buff.to_struct)-3;//向共用体中的结构体成员装填数据
           serial->write(tmp_buff.to_char_array,sizeof(tmp_buff.to_char_array));//发送数据
        }
    }
}


void MainWindow::on_data_6_Min_TextEdit_textChanged()
{
    ui->data_6_value_Qslider->setMinimum(ui->data_6_Min_TextEdit->toPlainText().toFloat()*100000);//设置最小值
}

void MainWindow::on_data_6_Max_TextEdit_textChanged()
{
    ui->data_6_value_Qslider->setMaximum(ui->data_6_Max_TextEdit->toPlainText().toFloat()*100000);//设置最大值
}

void MainWindow::on_data_6_value_Qslider_sliderReleased()//进度条被释放
{
    if(ui->data_6_enable_CheckBox->isChecked())//如果本数据被使能
    {
        if(!(ui->data_6_Type_Flag_TextEdit->toPlainText().isEmpty()))//如果Type_Flag编辑框内容不为空
        {
            ui->data_6_value_QLabel->setText( QString("%1").arg((double)(ui->data_6_value_Qslider->value()/100000.0),10));//将进度条数值放入标签
            DataPacket_Union tmp_buff;//创建一个共用体
            tmp_buff.to_struct.Command=ui->data_6_Type_Flag_TextEdit->toPlainText().toInt();//向共用体中的结构体成员装填数据
            tmp_buff.to_struct.Type=4;//向共用体中的结构体成员装填数据
            tmp_buff.to_struct.Data=ui->data_6_value_QLabel->text().toFloat();//向共用体中的结构体成员装填数据
            tmp_buff.to_struct.head_flag=~0x5FA;//向共用体中的结构体成员装填数据
            tmp_buff.to_struct.size=sizeof(tmp_buff.to_struct)-3;//向共用体中的结构体成员装填数据
           serial->write(tmp_buff.to_char_array,sizeof(tmp_buff.to_char_array));//发送数据
        }
    }
}



