#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#pragma pack(1)
struct  DataPacket
{
    short head_flag;//0x5FA
    char size;
    char Command;
    char Type;
    float Data;
};

union DataPacket_Union
{
    char to_char_array[sizeof(struct DataPacket)];
    struct DataPacket  to_struct;
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QSerialPort *serial;


private slots:
    void on_openButton_clicked();
    void Read_Data();
    void on_data_1_Min_TextEdit_textChanged();
    void on_data_1_Max_TextEdit_textChanged();
    void on_data_1_value_Qslider_sliderReleased();
    void on_data_2_Min_TextEdit_textChanged();
    void on_data_2_Max_TextEdit_textChanged();
    void on_data_2_value_Qslider_sliderReleased();
    void on_data_3_Min_TextEdit_textChanged();
    void on_data_3_value_Qslider_sliderReleased();
    void on_data_3_Max_TextEdit_textChanged();
    void on_data_4_Min_TextEdit_textChanged();
    void on_data_5_Min_TextEdit_textChanged();
    void on_data_6_Min_TextEdit_textChanged();
    void on_data_4_value_Qslider_sliderReleased();
    void on_data_5_value_Qslider_sliderReleased();
    void on_data_6_value_Qslider_sliderReleased();
    void on_data_5_Max_TextEdit_textChanged();
    void on_data_4_Max_TextEdit_textChanged();
    void on_data_6_Max_TextEdit_textChanged();
};

#endif // MAINWINDOW_H
