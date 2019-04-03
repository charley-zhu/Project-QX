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

#define URO 0 //ULO,����L�ࣻURO,����R�ࣻUTO,�����¶ȣ�UDO,��������3��������
#define ULO 0 
#define UTO 0
#define UDO 1

using namespace std;
const int gcDaqThreadCount = 3;		// 6��������

//ȫ�ֺ���
//ԭʼ����д���ı�
void WriteToTextU();
void OutToText(std::ofstream & _io, const vector<double> & icv_);
//ԭʼ���������ݿ�
bool insertintoDatabase_2(vector<double>&T_sensor, ProCes &ProL, ProCes &ProR);
//�����ݿ�
bool readFromDatabase_2();
class mywidget : public QWidget
{
	Q_OBJECT

public:
	mywidget(QWidget *parent = Q_NULLPTR);
	~mywidget();

	
	
private:
	Ui::mywidgetClass ui;
	//tcp�������
	
	QTcpServer *tcpsever1;
	QTcpSocket  *tcpsocket1;
	//��⵽������ �߳̿�����־λ
	
	//������־λ
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

	// �Բɼ����ݵĴ���
private:
	//QSqlDatabase db1;
	//QSqlDatabase db2;
public:
	// ��ʼ�����ݿ�����
	/*static bool InitDBConn_1(QSqlDatabase &db1);
	static bool InitDBConn_2(QSqlDatabase &db2);*/

};


