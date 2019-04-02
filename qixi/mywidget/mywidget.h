#pragma once

#include <QtWidgets/QWidget>
#include "ui_mywidget.h"
#include"winsock.h"
#include<QTcpServer>
#include<QTcpSocket>
#include <QTextEdit>
#include<string>
#include <QSql>
#include <QSqlDatabase>
#include "ProCes.h"

#define URO 0 //ULO,上行L侧；URO,上行R侧；UTO,上行温度；UDO,上行所有3个传感器
#define ULO 0 
#define UTO 0
#define UDO 1

using namespace std;
const int gcDaqThreadCount = 3;		// 6个传感器

//全局函数
//原始数据写入文本
void WriteToTextU();
void OutToText(std::ofstream & _io, const vector<double> & icv_);
//原始数据入数据库
bool insertintoDatabase_2(vector<double>&T_sensor, ProCes &ProL, ProCes &ProR);
//读数据库
bool readFromDatabase_2();
class mywidget : public QWidget
{
	Q_OBJECT

public:
	mywidget(QWidget *parent = Q_NULLPTR);
	~mywidget();

	
	
private:
	Ui::mywidgetClass ui;
	//tcp传输对象
	
	QTcpServer *tcpsever1;
	QTcpSocket  *tcpsocket1;
	//检测到车辆来 线程开启标志位
	
	//计数标志位
	int count1 = 0;

signals:
	void sendInfo(QString str);
public slots:
	void on_manualstart_2_clicked();
	void recvinfo(QString str);
	void on_pushButtonstop_clicked();
	void on_pushButtoninit_clicked();
	void on_ConNectPLC_clicked();
	void on_manualstop_2_clicked();

	// 对采集数据的处理
private:
	//QSqlDatabase db1;
	//QSqlDatabase db2;
public:
	// 初始化数据库连接
	/*static bool InitDBConn_1(QSqlDatabase &db1);
	static bool InitDBConn_2(QSqlDatabase &db2);*/

};


