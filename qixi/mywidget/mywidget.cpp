#include "stdafx.h"
#include "mywidget.h"
#include <QSqlError>
#include <QDebug>
#include <QSqlQuery>
#include<QString>
#include<QLibrary>
#include<QByteArray>
#include<windows.h>
#include<QFile>
#include <QDateTime>
#include<QTextStream>
#include <QTextEdit>
#include <windows.h>
#include <conio.h>
#include <fstream>
#include <vector>
#include <ostream>
#include<fstream>
#include <QDateTime>
#include "Process.h"
#include "ProCes.h"
#include "../MEDAQLib.h"
#include <iostream>


bool flag2 = true;

bool geflag2 = false;

//初始化标志位
bool L_flag = false;
bool R_flag = false;
bool T_flag = false;
//主控端奇偶位
bool parity = true;

//PLC通信自检标志位 2018/6/29
int plc_com = 1;
//左激光通信自检标志位 2018/6/29
int L1_com = 1;
//右激光通信自检标志位 2018/6/29
int L2_com = 1;
//测温仪通信自检标志位 2018/6/29
int tem_com = 1;

double calibu_l[4];
double calibu_r[4];

#pragma comment(lib, "ws2_32.lib")
#pragma execution_character_set("utf-8")
//
//
//#ifndef _WIN64
//#pragma comment (lib, "../Debug/MEDAQLib.lib")
//#else 
//#pragma comment (lib, "../Release/MEDAQLib.lib")
//#endif

#ifndef aver
#define aver(a,b)          floor(((a + b) / 2)  * 100.000f + 0.5) / 100.000f
#endif

#ifndef tofixed
#define tofixed(a)          floor(a * 100.000f + 0.5) / 100.000f
#endif

#ifndef _countof
#define _countof(x) (sizeof (x)/sizeof (x[0]))
#endif // _countof

using namespace std;


string g_sFilePrefix;


std::string gMeasureTimeU = "20171016102540";

//车号

QString gsTrainNoU = "###***";

//主控端
QString controlNo = "1000";


HANDLE hThreadDaq[gcDaqThreadCount] = { nullptr };
unsigned int ThreadID[gcDaqThreadCount] = { 0 };

HANDLE hThreadProU = NULL;
vector<vector<double>> qixiData(2);

vector<double>tempDatau;


//声明采集线程函数

unsigned int _stdcall getdatTest4(void* pM);
unsigned int _stdcall getdatTest5(void* pM);
unsigned int _stdcall getdatTest6(void* pM);
//声明数据处理线程函数
unsigned int _stdcall DataProcessU(void* pM);
//插入数据库线程函数

mywidget::mywidget(QWidget *parent)
	: QWidget(parent)
	
{
	
	ui.setupUi(this);


	connect(this, SIGNAL(sendInfo(QString)), this, SLOT(recvinfo(QString)));
	
	//textEditConnect->append(QStringLiteral("123！"));
	
	//除了连接按钮，其他按钮全部设置为FALSE
	//----------------------信号槽连接函数---------------
	//connect(ui->pushButtonConnect,&QPushButton::clicked,this,&MyWidget::on_pushButtonConnect_clicked);
	//connect(ui->pushButtonADset,&QPushButton::clicked,this,&MyWidget::on_pushButtonADset_clicked);

	geflag2 = false;
	hThreadDaq[4] = (HANDLE)_beginthreadex(NULL, 0, getdatTest4, this, 0, &ThreadID[4]);
	hThreadDaq[5] = (HANDLE)_beginthreadex(NULL, 0, getdatTest5, this, 0, &ThreadID[5]);
	hThreadDaq[6] = (HANDLE)_beginthreadex(NULL, 0, getdatTest6, this, 0, &ThreadID[6]);

	ui.textEditConnect->append("程序初始化...");

	if (!L_flag||!R_flag||!T_flag)
	{

		Sleep(1000);
	}

	tcpsever1 = NULL;
	tcpsocket1 = NULL;
	//监听套接字制定父对象 上行上位机作服务器 端口号6666
	tcpsever1 = new QTcpServer(this);
	tcpsever1->listen(QHostAddress::Any, 6666);

	//连接plc
	connect(tcpsever1, &QTcpServer::newConnection,
		[=]()
	{
		tcpsocket1 = tcpsever1->nextPendingConnection();
		//检查与PLC的通讯 2018/6/29
		if (!tcpsocket1)
		{
			emit sendInfo("PLC连接失败");
			plc_com = 1;
		}
		else
		{
			plc_com = 0;
		}

		QString ip = tcpsocket1->peerAddress().toString();
		qint16 port = tcpsocket1->peerPort();
		QString temp = QString("[%1 %2]:上行PLC连接成功").arg(ip).arg(port);
		emit sendInfo(temp);

		connect(tcpsocket1, &QTcpSocket::readyRead,
			[=]()
		{
			gMeasureTimeU = QDateTime::currentDateTime().toString("yyyyMMddHHmmss").toStdString();
			//从通信套接字中取出内容
			QByteArray array = tcpsocket1->readAll();
			if (array.size() > 1)
			{
				controlNo = array.mid(2, 3);
				if (controlNo =="04A")
					controlNo = "0"+ array.left(2);
				/*int controlnum;
				controlnum = controlNo.toInt();
				if (controlnum % 2 == 0)
				{
					controlnum = controlnum - 1;
					if (controlnum < 100&& controlnum>=10)
					{
						controlNo = "0" + QString::number(controlnum);
					}
					if(controlnum>=100)
						controlNo = QString::number(controlnum);
					if(controlnum<10)
						controlNo = "00" + QString::number(controlnum);
				}*/
			}
			emit sendInfo(controlNo);
			//num = number.toStdString();
			ui.CarNo->setText(gsTrainNoU);
			//使用数据流的方式读取
			QDataStream out(&array, QIODevice::ReadWrite);
			//批量进行转换

			while (!out.atEnd())
			{
				qint8 outChar = 0;
				//每字节填充一次，直到结束
				out >> outChar;
				//十六进制的转换
				QString str = QString("%1").arg(outChar & 0xFF, 2, 16, QLatin1Char('0'));
				//qDebug() << str;
				if (str == "23")
				{

					//第一次采集到标志位开启采集线程

					flag2 = false;
					count1++;
					if (count1 == 1)
					{
						flag2 = false;
						//发送信号
						emit sendInfo("将上行传感器数据写入txt!");
					}

				}
				if (str == "2a")
				{
					/*停止采集*/
					flag2 = true;
					emit sendInfo("上行传感器数据采集停止");

					//开启数据处理线程
					CloseHandle((HANDLE)_beginthreadex(NULL, 0, DataProcessU, this, 0, NULL));

					count1 = 0;


				}
			}

			//取出车牌号码
			emit sendInfo("上行车号为：" + array.left(8));
			count1 = 0;

		}
		);

	}
	);



}


mywidget::~mywidget()
{

}

void mywidget::on_manualstart_2_clicked()
{
	gMeasureTimeU = QDateTime::currentDateTime().toString("yyyyMMddHHmmss").toStdString();	
	flag2 = false;
	count1++;
	if (count1 == 1)
	{
		flag2 = false;

		emit sendInfo("将上行传感器数据写入txt!");

	}
}

void mywidget::on_manualstop_2_clicked()
{
	flag2 = true;
	count1 = 0;

	ui.textEditConnect->append("上行数据采集停止");
	CloseHandle((HANDLE)_beginthreadex(NULL, 0, DataProcessU, this, 0, NULL));
	
}

void mywidget::recvinfo(QString str)
{
	ui.textEditConnect->append(str);
}

void mywidget::on_ConNectPLC_clicked()
{

	tcpsever1 = NULL;
	tcpsocket1 = NULL;
	//监听套接字制定父对象 上行上位机作服务器 端口号6666
	tcpsever1 = new QTcpServer(this);
	tcpsever1->listen(QHostAddress::Any, 6666);

	ui.textEditConnect->append("等待PLC新连接...");
	connect(tcpsever1, &QTcpServer::newConnection,
		[=]()
	{
		tcpsocket1 = tcpsever1->nextPendingConnection();

		QString ip = tcpsocket1->peerAddress().toString();
		qint16 port = tcpsocket1->peerPort();
		QString temp = QString("[%1 %2]:上行PLC连接成功").arg(ip).arg(port);
		ui.textEditConnect->append(temp);

		connect(tcpsocket1, &QTcpSocket::readyRead,
			[=]()
		{
			gMeasureTimeU = QDateTime::currentDateTime().toString("yyyyMMddHHmmss").toStdString();
			//从通信套接字中取出内容
			QByteArray array = tcpsocket1->readAll();
			QString gsTrainNoU = array.left(6);
			//num = number.toStdString();
			ui.CarNo->setText(gsTrainNoU);
			//使用数据流的方式读取
			QDataStream out(&array, QIODevice::ReadWrite);
			//批量进行转换

			while (!out.atEnd())
			{
				qint8 outChar = 0;
				//每字节填充一次，直到结束
				out >> outChar;
				//十六进制的转换
				QString str = QString("%1").arg(outChar & 0xFF, 2, 16, QLatin1Char('0'));
				//qDebug() << str;
				if (str == "23")
				{

					//第一次采集到标志位开启采集线程
					
					flag2 = false;
					count1++;
					if (count1 == 1)
					{
						flag2 = false;
						//发送信号
						emit sendInfo("将上行传感器数据写入txt!");
					}

				}
				if (str == "2a")
				{
					/*停止采集*/
					flag2 = true;
					ui.textEditConnect->append("上行传感器数据采集停止");
					
					CloseHandle((HANDLE)_beginthreadex(NULL, 0, DataProcessU, this, 0, NULL));

					count1 = 0;


				}
			}

			//取出车牌号码
			ui.textEditConnect->append("上行车号为：" + array.left(6));
			count1 = 0;

		}
		);

	}
	);
}

void mywidget::on_pushButtonstop_clicked()
{
	
	geflag2 = true;
	
	CloseHandle(hThreadDaq[4]);
	CloseHandle(hThreadDaq[5]);
	CloseHandle(hThreadDaq[6]);

	hThreadDaq[4] = NULL;
	hThreadDaq[5] = NULL;
	hThreadDaq[6] = NULL;
	ui.textEditConnect->append("数据采集结束");

}


void mywidget::on_pushButtoninit_clicked()
{
	geflag2 = false;
	hThreadDaq[4] = (HANDLE)_beginthreadex(NULL, 0, getdatTest4, this, 0, &ThreadID[4]);
	hThreadDaq[5] = (HANDLE)_beginthreadex(NULL, 0, getdatTest5, this, 0, &ThreadID[5]);
	hThreadDaq[6] = (HANDLE)_beginthreadex(NULL, 0, getdatTest6, this, 0, &ThreadID[6]);

	ui.textEditConnect->append("程序初始化...");


}

/************************************************上行采集线程***********************************************/
unsigned int _stdcall getdatTest4(void* pM)
{
	mywidget *demo = (mywidget*)pM;
	DWORD sensor1 = CreateSensorInstance(SENSOR_ILD2300);
	SetParameterString(sensor1, "IP_Interface", "TCP/IP");
	SetParameterString(sensor1, "IP_RemoteAddr", "192.168.1.4");
	OpenSensor(sensor1);

	SetParameterInt(sensor1, "SP_DataOutInterface", 2);
	ExecSCmd(sensor1, "Set_DataOutInterface");

	demo->emit sendInfo("上行左侧激光传感器就绪！");
	L_flag = true;
	//采集参数


	while (!geflag2)
	{
		//激光传感器通讯自检 2018/6/29
		if (!flag2)
		{
			SetParameterString(sensor1, "IP_Interface", "TCP/IP");
			SetParameterString(sensor1, "IP_RemoteAddr", "192.168.1.4");
			L1_com = OpenSensor(sensor1);
			if (L1_com != ERR_ALREADY_OPEN&& L1_com != ERR_NOERROR&& L1_com != ERR_WARNING)
				L1_com = 1;
			else 
				L1_com = 0;

			SetParameterInt(sensor1, "SP_DataOutInterface", 2);
			ExecSCmd(sensor1, "Set_DataOutInterface");

		}
		while (!flag2)
		{
			int avail, read;
			DataAvail(sensor1, &avail);
			read = avail;

			int *raw = new int[read];
			double *scaled = new double[read];
			double y;
			TransferData(sensor1, raw, scaled, read, &read);

			if (read > 0)
			{

				for (int j = 0; j < read; j++)
				{
					y = scaled[j] + 70;
					y = floor(y * 100.000f + 0.5) / 100.000f;

					if ((y > 70) && (y < 170))
						qixiData[0].push_back(y);
					else qixiData[0].push_back(-10);

					if (true == flag2)
					{
						break;
					}
				}
			}

			Sleep(1);	// 等待新数据
			delete[]scaled;
			delete[]raw;
		}
		Sleep(10);

	}

	emit demo->sendInfo("上行左侧激光传感器采集结束!");

	CloseSensor(sensor1);

	ReleaseSensorInstance(sensor1);

	return 0;
}

unsigned int _stdcall getdatTest5(void* pM)
{
	mywidget *demo = (mywidget*)pM;
	//传感器参数初始化
	DWORD sensor2 = CreateSensorInstance(SENSOR_ILD2300);
	SetParameterString(sensor2, "IP_Interface", "TCP/IP");
	SetParameterString(sensor2, "IP_RemoteAddr", "192.168.1.3");

	OpenSensor(sensor2);
	SetParameterInt(sensor2, "SP_DataOutInterface", 2);
	ExecSCmd(sensor2, "Set_DataOutInterface");

	demo->emit sendInfo("上行右侧激光传感器就绪！");
	R_flag = true;
	//采集参数



	//采集标志位为false，TransferData()发送数据，并将数据写入text，否则关闭文件保存数据，关闭传感器实例
	while (!geflag2)
	{
		if (!flag2)
		{
			SetParameterString(sensor2, "IP_Interface", "TCP/IP");
			SetParameterString(sensor2, "IP_RemoteAddr", "192.168.1.3");

			L2_com = OpenSensor(sensor2); //2018
			if (L2_com != ERR_ALREADY_OPEN&& L2_com != ERR_NOERROR&& L2_com != ERR_WARNING)
				L2_com = 1;
			else
				L2_com = 0;
			SetParameterInt(sensor2, "SP_DataOutInterface", 2);
			ExecSCmd(sensor2, "Set_DataOutInterface");
		}

		while (!flag2)
		{
			int avail, read;
			DataAvail(sensor2, &avail);
			read = avail;

			int *raw = new int[read];
			double *scaled = new double[read];
			double y;
			TransferData(sensor2, raw, scaled, read, &read);

			//scaled为double型数组，占8个字节，sizeof即为80000			
			//增大采集频率，要减小算法时间复杂度，并增加算法使采集数据保持均匀
			if (read > 0)
			{

				for (int j = 0; j < read; j++)
				{
					//传感器量程0~100mm，气隙值要加70mm，在70~170mm范围外输出error
					y = scaled[j] + 70;
					y = floor(y * 100.000f + 0.5) / 100.000f;

					if ((y > 70) && (y < 170))
						qixiData[1].push_back(y);
					else qixiData[1].push_back(-10);

					if (true == flag2)
					{
						break;
					}
				}

			}
			//一帧数据抓取耗时50到80ms左右，此处应该把等待时间尽量减小，5ms比较合适可以试试
			Sleep(1);	//  等待新数据
			delete[]scaled;
			delete[]raw;
		}
		Sleep(10);

	}

	emit demo->sendInfo("上行右侧激光传感器采集结束!");
	CloseSensor(sensor2);
	ReleaseSensorInstance(sensor2);

	return 0;
}

unsigned int _stdcall getdatTest6(void* pM)
{
	mywidget *demo = (mywidget*)pM;
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD(1, 1);//第一个参数为低位字节；第二个参数为高位字节
	err = WSAStartup(wVersionRequested, &wsaData);//对winsock DLL（动态链接库文件）进行初始化，协商Winsock的版本支持，并分配必要的资源。
	if (err != 0)
	{
		return 0;
	}

	if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1)//LOBYTE（）取得16进制数最低位；HIBYTE（）取得16进制数最高（最左边）那个字节的内容
	{
		::WSACleanup();
		return 0;
	}
	SOCKET sockClient = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN addrClt;//需要包含服务端IP信息
	addrClt.sin_addr.S_un.S_addr = inet_addr("192.168.1.151");// inet_addr将IP地址从点数格式转换成网络字节格式整型。
	addrClt.sin_family = AF_INET;
	addrClt.sin_port = htons(4001);
	::connect(sockClient, (SOCKADDR*)&addrClt, sizeof(SOCKADDR));//客户机向服务器发出连接请求		



	demo->emit sendInfo("上行温度传感器就绪！");
	T_flag = true;


	//QString str0 = "当前温度为:";
	while (!geflag2)
	{
		if (!flag2)
		{
			
		}
		while (!flag2)
		{
			double show[10240];

			for (int i = 0; i < 10240; i++)
			{
				char sendBuf[2];
				sendBuf[0] = 0xB1;
				sendBuf[1] = 0x01;
				send(sockClient, sendBuf, strlen(sendBuf), 0);

				char recvBuf[10240];
				//测温仪接收数据自检 2018/6/29
				tem_com = recv(sockClient, recvBuf, 2, 0);
				if (SOCKET_ERROR == tem_com)
				{
					tem_com = 1;
				}
				else
					tem_com = 0;

				byte a = recvBuf[1];
				byte b = recvBuf[0];
				double t0 = b;
				double t1 = a;
				double y = (t0 * 256 + t1 - 1000) / 10;
				show[i] = y; //温度采集 2018
				if (0 < y < 200)
				{
					tempDatau.push_back(y);
				}
				Sleep(1);
				if (true == flag2)
				{
					break;
				}

			}

		}
		Sleep(10);

	}


	emit demo->sendInfo("上行温度传感器采集结束!");
	closesocket(sockClient);
	WSACleanup();


	return 0;
}

bool insertintoDatabase_2(vector<double>&T_sensor, ProCes &ProL, ProCes &ProR)
{
	QSqlDatabase db2;

	QString measuretime = QString::fromStdString(gMeasureTimeU);
	QString connecName = "B -" + measuretime;

	if (QSqlDatabase::contains(connecName))
		db2 = QSqlDatabase::database(connecName);
	else
		db2 = QSqlDatabase::addDatabase("QMYSQL", connecName);

	db2.setDatabaseName("qixi2");
	db2.setUserName("root");
	db2.setPassword("zq930219");

	qDebug() << ProL.qxmean[0];
	if (db2.open())
	{
		qDebug() << "open ok";

	}
	else
	{
		qDebug() << "open failed";
	}
	qDebug() << "数据库初始化完毕！";
	//Sleep(10000);
	//插入数据（开启事务）
	QVector<QVariantList> initdatalist;

	QString trainNo = gsTrainNoU;//车号
	QString measureTime(gMeasureTimeU.c_str());//行车时间
	QString insertTime;//行车时间
	QString place = "飞沙角";//检测地点
	int direction = 0;//行驶方向
	int trainStat = 11;

	insertTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
	//insertTime = "2017-10-17 11:33:44";

	QDateTime Tim = QDateTime::fromString(measureTime, "yyyyMMddHHmmss");

	//激光原始数据时间戳
	QString measureTime_qx;

	//QSqlDatabase::database("A").transaction();
	QSqlQuery query(db2);

	double para1 = 0;
	double para2 = 0;
	double para3 = 0;
	double para4 = 0;
	double para5 = 0;
	double para6 = 0;
	double para7 = 0;
	double para8 = 0;
	double para9 = 0;
	double para10 = 0;
	double para11 = 0;
	double para12 = 0;
	double para13 = 0;
	double para14 = 0;

	query.exec("set global innodb_flush_log_at_trx_commit = 0;");
	query.exec("set global innodb_autoextend_increment = 128M;");
	query.exec("set global innodb_log_buffer_size = 16M;");
	query.exec("set global innodb_log_file_size  = 128M;");

	query.exec("select * from train_param where ud_flag = 0 ");


	if (query.last()) //seek指定当前的位置
	{
		para1 = query.value(1).toDouble();
		para2 = query.value(2).toDouble();
		para3 = query.value(3).toDouble();
		para4 = query.value(4).toDouble();
		para5 = query.value(5).toDouble();
		para6 = query.value(6).toDouble();
		para7 = query.value(7).toDouble();
		para8 = query.value(8).toDouble();
		para9 = query.value(9).toDouble();
		para10 = query.value(10).toDouble();
		para11 = query.value(11).toDouble();
		para12 = query.value(12).toDouble();
		para13 = query.value(13).toDouble();
		para14 = query.value(14).toDouble();
		qDebug() << "报警值设置：" << para1 << para2 << para3 << para4 << para5 << para6 << para7 << para8 << para9 << para10;
	}


	//向表"train_info"	//异常值计算
	vector<double> excp_alar;
	//四角2018/6/27
	for (int i = 0; i < 8; ++i)
	{
		excp_alar.push_back(max(ProL.max_qmean[i], ProR.max_qmean[i]) - min(ProL.min_qmean[i], ProR.min_qmean[i]));
		
	}

	
	double Excp_Alar = *max_element(begin(excp_alar), end(excp_alar));



	//异常报警
	if (Excp_Alar >para10)
		trainStat = 1;

	//气隙报警
	else if (ProL.min_qx <= para2 || ProR.min_qx <= para2)
		trainStat = 2;
	//槽深报警
	else if (ProL.min_cs <= para8 || ProR.min_cs <= para8 || ProL.max_cs >= para12 || ProR.max_cs >= para12)
		trainStat = 3;
	//槽隙报警
	else if (ProL.max_cx >= para5 || ProR.max_cx >= para5)
		trainStat = 4;
	//温度报警
	else if (ProL.t_max[ProL.t_maxid]>para4)
		trainStat = 5;
	//异常预警
	else if (Excp_Alar >para9)
		trainStat = 6;
	//气隙预警
	else if (ProL.min_qx <= para1 || ProR.min_qx <= para1)
		trainStat = 7;
	//槽深预警
	else if (ProL.min_cs <= para7 || ProR.min_cs <= para7 || ProL.max_cs >= para11 || ProR.max_cs >= para11)
		trainStat = 8;
	//槽隙预警
	else if (ProL.max_cx >= para6 || ProR.max_cx >= para6)
		trainStat = 9;
	//温度预警
	else if (ProL.t_max[ProL.t_maxid]>para3)
		trainStat = 10;
	//排障器预警
	else if (ProL.pilot_min <= para13 || ProR.pilot_min <= para13)
		trainStat = 12;
	//排障器报警
	else if (ProL.pilot_min <= para14 || ProR.pilot_min <= para14)
		trainStat = 13;
	else
		trainStat = 11;


	QString sql = QString("INSERT INTO train_info(train_station,train_id,control_num,train_direction,train_state,train_date,insert_time) VALUES ('%1','%2', '%3','%4','%5','%6','%7')").arg(place).arg(trainNo).arg(controlNo).arg(direction).arg(trainStat).arg(Tim.toString("yyyy-MM-dd HH:mm:ss")).arg(insertTime);
	query.exec(sql);
	int id = query.lastInsertId().toInt();	// 获取最后一次插入时的编号
	query.clear();
	//插入train_laser
	query.prepare("INSERT INTO train_laser(train_onlyid,left_laser,right_laser,train_date,motor_num) VALUES (?,?,?,?,?)");
	initdatalist.resize(5);

	for (int i = 0; i < ProL.value_save.size() && i < ProR.value_save.size(); i++)
	{
		//i为1、3、5、7
		if (i % 2)
		{
			for (int j = min(ProL.value_save[i].size(), ProR.value_save[i].size()) - 1; j >= 0; --j)
			{
				initdatalist[0] << id;
				initdatalist[1] << ProL.value_save[i][j];
				initdatalist[2] << ProR.value_save[i][j];
				initdatalist[3] << Tim.toString("yyyy-MM-dd HH:mm:ss");
				initdatalist[4] << i + 1;

			}
		}
		//i为0、2、4、6
		else
		{
			for (int j = 0; j < ProL.value_save[i].size() && j < ProR.value_save[i].size(); ++j)
			{
				initdatalist[0] << id;
				initdatalist[1] << ProL.value_save[i][j];
				initdatalist[2] << ProR.value_save[i][j];
				initdatalist[3] << Tim.toString("yyyy-MM-dd HH:mm:ss");
				initdatalist[4] << i + 1;

			}
		}
	}

	query.addBindValue(initdatalist.at(0));
	query.addBindValue(initdatalist.at(1));
	query.addBindValue(initdatalist.at(2));
	query.addBindValue(initdatalist.at(3));
	query.addBindValue(initdatalist.at(4));
	if (!query.execBatch());
	{
		qDebug() << query.lastError();
	}
	//QSqlDatabase::database("A").commit();

	qDebug() << "上行激光原始数据入数据库完毕！";
	initdatalist.clear();
	initdatalist.swap(QVector<QVariantList>());

	query.clear();

	//插入train_temp
	query.prepare("INSERT INTO train_temp(train_onlyid,train_temp,train_date,motor_num) VALUES (?,?,?,?)");
	initdatalist.resize(4);
	if (parity)
	{
		for (int i = 0; i < ProL.wd.size(); i++)
		{
			if (i % 2)
			{
				for (int j = ProL.wd[i].size() - 1; j >= 0; --j)
				{
					initdatalist[0] << id;
					initdatalist[1] << ProL.wd[i][j];
					initdatalist[2] << Tim.toString("yyyy-MM-dd HH:mm:ss");
					initdatalist[3] << i + 1;

				}
			}
			else
			{
				for (int j = 0; j < ProL.wd[i].size(); ++j)
				{
					initdatalist[0] << id;
					initdatalist[1] << ProL.wd[i][j];
					initdatalist[2] << Tim.toString("yyyy-MM-dd HH:mm:ss");
					initdatalist[3] << i + 1;

				}
			}
		}
	}
	else
	{
		for (int i = ProL.wd.size() - 1; i >= 0; --i)
		{
			if (i % 2)
			{
				for (int j = 0; j < ProL.wd[i].size(); ++j)
				{
					initdatalist[0] << id;
					initdatalist[1] << ProL.wd[i][j];
					initdatalist[2] << Tim.toString("yyyy-MM-dd HH:mm:ss");
					initdatalist[3] << ProL.wd.size() - i;

				}
			}
			else
			{
				for (int j = ProL.wd[i].size() - 1; j >= 0; --j)
				{
					initdatalist[0] << id;
					initdatalist[1] << ProL.wd[i][j];
					initdatalist[2] << Tim.toString("yyyy-MM-dd HH:mm:ss");
					initdatalist[3] << ProL.wd.size() - i;

				}
			}
		}
	}
	query.addBindValue(initdatalist.at(0));
	query.addBindValue(initdatalist.at(1));
	query.addBindValue(initdatalist.at(2));
	query.addBindValue(initdatalist.at(3));
	if (!query.execBatch());
	{
		qDebug() << query.lastError();
	}

	//QSqlDatabase::database("A").commit();

	qDebug() << "上行温度原始数据入数据库完毕！";
	initdatalist.clear();
	initdatalist.swap(QVector<QVariantList>());

	query.clear();


	//插入train_data
	query.prepare("INSERT INTO train_data VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)");
	query.addBindValue(id);
	query.addBindValue(ProL.t_maxid + 1);
	query.addBindValue(tofixed(ProL.mean_temp));
	query.addBindValue(tofixed(min(ProL.min_qx, ProR.min_qx)));
	if (ProL.min_qx <= ProR.min_qx)
	{
		query.addBindValue(ProL.qmin_mid + 1);
	}
	else
		query.addBindValue(ProR.qmin_mid + 1);
	query.addBindValue(ProL.qmax_mid + 1);
	query.addBindValue(ProL.qmin_mid + 1);
	query.addBindValue(ProR.qmax_mid + 1);
	query.addBindValue(ProR.qmin_mid + 1);
	query.addBindValue(tofixed(ProR.mean_qx));
	query.addBindValue(tofixed(ProL.mean_qx));
	query.addBindValue(ProR.max_csmid + 1);
	query.addBindValue(ProR.min_csmid + 1);
	query.addBindValue(ProL.max_csmid + 1);
	query.addBindValue(ProL.min_csmid + 1);
	query.addBindValue(tofixed(ProR.mean_cs));
	query.addBindValue(tofixed(ProL.mean_cs));
	query.addBindValue(ProL.cmax_mid + 1);
	query.addBindValue(ProR.cmax_mid + 1);
	query.addBindValue(tofixed(ProL.mean_cx));
	query.addBindValue(tofixed(ProR.mean_cx));
	query.addBindValue(tofixed(ProL.pilot_min));
	query.addBindValue(tofixed(ProR.pilot_min));
	query.addBindValue(Tim.toString("yyyy-MM-dd HH:mm:ss"));

	query.exec();
	query.clear();
	qDebug() << "列车综合数据入数据库完毕！";


	//向表motor_info插入数据

	//电机关联编号
	QStringList M_id;

	for (int i = 0; i < 8; ++i)
	{
		query.prepare("INSERT INTO motor_info(train_onlyid,motor_num,motor_speed,temp_max,temp_min,temp_average,lgap_max,lgap_min,lgap_average,rgap_max,rgap_min,rgap_average,lslot_min,lslot_max,lslot_average,rslot_min,rslot_max,rslot_average,lsloth_max,rsloth_max,exc_value,motor_date,lpilot_value,rpilot_value) VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)");

		query.addBindValue(id);
		query.addBindValue(i + 1);
		query.addBindValue(tofixed(ProL.motor_speed[i]));
		query.addBindValue(tofixed(ProL.t_max[i]));
		query.addBindValue(tofixed(ProL.t_min[i]));
		query.addBindValue(tofixed(ProL.t_mean[i]));
		query.addBindValue(ProL.qmax_gid[i] + 1);
		query.addBindValue(ProL.qmin_gid[i] + 1);
		query.addBindValue(tofixed(ProL.qxmean[i]));
		query.addBindValue(ProR.qmax_gid[i] + 1);
		query.addBindValue(ProR.qmin_gid[i] + 1);
		query.addBindValue(tofixed(ProR.qxmean[i]));
		query.addBindValue(ProL.csmin_gid[i] + 1);
		query.addBindValue(ProL.csmax_gid[i] + 1);
		query.addBindValue(tofixed(ProL.csmean[i]));
		query.addBindValue(ProR.csmin_gid[i] + 1);
		query.addBindValue(ProR.csmax_gid[i] + 1);
		query.addBindValue(tofixed(ProR.csmean[i]));
		query.addBindValue(ProL.cmax_gid[i] + 1);
		query.addBindValue(ProR.cmax_gid[i] + 1);
		// 四角异常值插入数据库motor_info 2018/6/27
		query.addBindValue(tofixed(excp_alar[i]));
		query.addBindValue(Tim.toString("yyyy-MM-dd HH:mm:ss"));
		query.addBindValue(tofixed(ProL.pilot_value[i]));
		query.addBindValue(tofixed(ProR.pilot_value[i]));
		query.exec();
		M_id << query.lastInsertId().toString();

		//qDebug()<< M_id[i].toInt();
		query.clear();

	}
	excp_alar.clear();
	excp_alar.swap(vector<double>());

	qDebug() << "列车电机数据入数据库完毕！";


	//向表gear_info插入数据
	query.prepare("INSERT INTO gear_info(gear_num,lslot_value,rslot_value,lgap_value,rgap_value,lslot_depth,rslot_depth,motor_id,gear_date) VALUES (?,?,?,?,?,?,?,?,?)");
	initdatalist.resize(9);


	for (int i = 0; i < 8; ++i)
	{


		for (int j = 0; j < 79; j++)
		{
			initdatalist[0] << j + 1;
			initdatalist[1] << tofixed(ProL.mgmax_cx[i][j]);
			initdatalist[2] << tofixed(ProR.mgmax_cx[i][j]);
			initdatalist[3] << tofixed(ProL.mgmin_qx[i][j]);
			initdatalist[4] << tofixed(ProR.mgmin_qx[i][j]);
			initdatalist[5] << tofixed(ProL.cs[i][j]);
			initdatalist[6] << tofixed(ProR.cs[i][j]);
			initdatalist[7] << M_id[i].toLong();
			initdatalist[8] << Tim.toString("yyyy-MM-dd HH:mm:ss");
		}
		initdatalist[0] << 80;
		initdatalist[1] << 0;
		initdatalist[2] << 0;
		initdatalist[3] << tofixed(ProL.mgmin_qx[i][79]);
		initdatalist[4] << tofixed(ProR.mgmin_qx[i][79]);
		initdatalist[5] << 0;
		initdatalist[6] << 0;
		initdatalist[7] << M_id[i].toLong();
		initdatalist[8] << Tim.toString("yyyy-MM-dd HH:mm:ss");

	}

	query.addBindValue(initdatalist.at(0));
	query.addBindValue(initdatalist.at(1));
	query.addBindValue(initdatalist.at(2));
	query.addBindValue(initdatalist.at(3));
	query.addBindValue(initdatalist.at(4));
	query.addBindValue(initdatalist.at(5));
	query.addBindValue(initdatalist.at(6));
	query.addBindValue(initdatalist.at(7));
	query.addBindValue(initdatalist.at(8));
	query.execBatch();

	initdatalist.clear();
	initdatalist.swap(QVector<QVariantList>());
	query.clear();

	//向表train_fault插入数据
	if (Excp_Alar >para10)
	{
		QString sql = QString("INSERT INTO train_fault(train_onlyid,fault_type,train_date) VALUES ('%1','%2', '%3')").arg(id).arg(1).arg(measureTime);
		query.exec(sql);
		query.clear();
	}

	if (ProL.min_qx <= (para2) || ProR.min_qx <= (para2))
	{
		QString sql = QString("INSERT INTO train_fault(train_onlyid,fault_type,train_date) VALUES ('%1','%2', '%3')").arg(id).arg(2).arg(measureTime);
		query.exec(sql);
		query.clear();
	}

	if (ProL.min_cs <= para8 || ProR.min_cs <= para8 || ProL.max_cs >= para12 || ProR.max_cs >= para12) //槽深报警
	{
		QString sql = QString("INSERT INTO train_fault(train_onlyid,fault_type,train_date) VALUES ('%1','%2', '%3')").arg(id).arg(3).arg(measureTime);
		query.exec(sql);
		query.clear();
	}

	if (ProL.max_cx >= (para5) || ProR.max_cx >= (para5))
	{
		QString sql = QString("INSERT INTO train_fault(train_onlyid,fault_type,train_date) VALUES ('%1','%2', '%3')").arg(id).arg(4).arg(measureTime);
		query.exec(sql);
		query.clear();
	}

	if (ProL.t_max[ProL.t_maxid]>para4)
	{
		QString sql = QString("INSERT INTO train_fault(train_onlyid,fault_type,train_date) VALUES ('%1','%2', '%3')").arg(id).arg(5).arg(measureTime);
		query.exec(sql);
		query.clear();
	}

	if (Excp_Alar >para9&&Excp_Alar <para10)
	{
		QString sql = QString("INSERT INTO train_fault(train_onlyid,fault_type,train_date) VALUES ('%1','%2', '%3')").arg(id).arg(6).arg(measureTime);
		query.exec(sql);
		query.clear();
	}

	if ((ProL.min_qx >= para2&&ProL.min_qx <= para1) || (ProR.min_qx >= para2&&ProR.min_qx <= para1))
	{
		QString sql = QString("INSERT INTO train_fault(train_onlyid,fault_type,train_date) VALUES ('%1','%2', '%3')").arg(id).arg(7).arg(measureTime);
		query.exec(sql);
		query.clear();
	}

	if ((ProL.min_cs <= para7&&ProL.min_cs >= para8) || (ProR.min_cs <= para7&&ProR.min_cs >= para8) || (ProL.max_cs <= para12&&ProL.max_cs >= para11) || (ProR.max_cs <= para12 && ProR.max_cs >= para11)) //槽深报警
	{
		QString sql = QString("INSERT INTO train_fault(train_onlyid,fault_type,train_date) VALUES ('%1','%2', '%3')").arg(id).arg(8).arg(measureTime);
		query.exec(sql);
		query.clear();
	}

	if ((ProL.max_cx >= para6&&ProL.max_cx <= para5) || (ProR.max_cx >= para6&&ProR.max_cx <= para5))
	{
		QString sql = QString("INSERT INTO train_fault(train_onlyid,fault_type,train_date) VALUES ('%1','%2', '%3')").arg(id).arg(9).arg(measureTime);
		query.exec(sql);
		query.clear();
	}

	if (ProL.t_max[ProL.t_maxid]>para3&&ProL.t_max[ProL.t_maxid] <= para4)
	{
		QString sql = QString("INSERT INTO train_fault(train_onlyid,fault_type,train_date) VALUES ('%1','%2', '%3')").arg(id).arg(10).arg(measureTime);
		query.exec(sql);
		query.clear();
	}

	if ((ProL.pilot_min<=para13&&ProL.pilot_min>=para14) || (ProR.pilot_min <= para13 && ProR.pilot_min >= para14))
	{
		QString sql = QString("INSERT INTO train_fault(train_onlyid,fault_type,train_date) VALUES ('%1','%2', '%3')").arg(id).arg(11).arg(measureTime);
		query.exec(sql);
		query.clear();
	}

	if (ProL.pilot_min <= para14 || ProR.pilot_min <= para14)
	{
		QString sql = QString("INSERT INTO train_fault(train_onlyid,fault_type,train_date) VALUES ('%1','%2', '%3')").arg(id).arg(12).arg(measureTime);
		query.exec(sql);
		query.clear();
	}

	//向system_info表插数据
	sql = QString("INSERT INTO system_info(train_onlyid,plc_state,llaser_state,rlaser_state,temp_state,ud_flag,train_date) VALUES ('%1','%2', '%3','%4','%5','%6','%7')")
		.arg(id).arg(plc_com).arg(L1_com).arg(L2_com).arg(tem_com).arg(0).arg(measureTime);
	query.exec(sql);
	query.clear();




	QString name;
	{
		name = QSqlDatabase::database().connectionName();
	}//超出作用域，隐含对象QSqlDatabase::database()被删除。
	QSqlDatabase::removeDatabase(name);

	qDebug() << "上行采集系统等待下一次采集！";

	db2.close();
	return true;
}
//数据处理线程


unsigned int _stdcall DataProcessU(void* pM)
{
	mywidget *demo = (mywidget *)pM;
	//存数据到文本文件
	//由主控端得到车号
	int Numb = 1001;
	Numb = controlNo.toInt();
	if (Numb % 2)
		parity = true;
	else
		parity = false;
	if (Numb > 100)
	{
		if (Numb % 2 == 0)
		{
			gsTrainNoU = QString::number(Numb - 1, 10) + QString::number(Numb, 10);
		}
		else
		{
			gsTrainNoU = QString::number(Numb, 10) + QString::number(Numb + 1, 10);
		}

	}
	else if (Numb == 100)
	{
		gsTrainNoU = "0" + QString::number(Numb - 1, 10) + QString::number(Numb, 10);
	}
	else if (Numb == 99)
	{
		gsTrainNoU = "0" + QString::number(Numb , 10) + QString::number(Numb + 1, 10);
	}
	else if (Numb == 10)
	{
		gsTrainNoU = "00" + QString::number(Numb - 1, 10) + "0" + QString::number(Numb, 10);
	}
	else if (Numb == 9)
	{
		gsTrainNoU = "00" + QString::number(Numb, 10) + "0" + QString::number(Numb + 1, 10);
	}
	else if (Numb < 9)
	{
		if (Numb % 2 == 0)
		{
			gsTrainNoU = "00" + QString::number(Numb - 1, 10) + "00" + QString::number(Numb, 10);
		}
		else
		{
			gsTrainNoU = "00" + QString::number(Numb, 10) + "00" + QString::number(Numb + 1, 10);
		}
	}
	else
	{
		if (Numb % 2 == 0)
		{
			gsTrainNoU = "0" + QString::number(Numb - 1, 10) + "0" + QString::number(Numb, 10);
		}
		else
		{
			gsTrainNoU = "0" + QString::number(Numb, 10) + "0" + QString::number(Numb + 1, 10);
		}
	}
	if (gsTrainNoU == "10011002") {
		g_sFilePrefix = "### -" + gMeasureTimeU;
	}
	else {
		g_sFilePrefix = gsTrainNoU.toStdString() + "-" + gMeasureTimeU;
	}
	WriteToTextU();
	emit demo->sendInfo("上行原始数据写入txt成功!");

	readFromDatabase_2();

	//存数据库

	vector<double> L_lazer;
	vector<double> R_lazer;
	vector<double> T_sensor;

	L_lazer.swap(qixiData[0]);
	R_lazer.swap(qixiData[1]);
	T_sensor.swap(tempDatau);

	ProCes proL(L_lazer, T_sensor, calibu_l, parity);
	ProCes proR(R_lazer, T_sensor, calibu_r, parity);


	try
	{
		proL.MainProcess();
		qDebug() << proL.value_save.size();
		Sleep(100);

		proR.MainProcess();

		emit demo->sendInfo(QString("上行算法处理成功!"));

		insertintoDatabase_2(T_sensor, proL, proR);

		L_lazer.clear();
		L_lazer.swap(vector<double>());
		R_lazer.clear();
		R_lazer.swap(vector<double>());
		T_sensor.clear();
		T_sensor.swap(vector<double>());

		//主控端置为"000"
		controlNo = "000";

	}

	catch (...)
	{
		emit demo->sendInfo(QString("上行算法处理失败!"));

		L_lazer.clear();
		L_lazer.swap(vector<double>());
		R_lazer.clear();
		R_lazer.swap(vector<double>());
		T_sensor.clear();
		T_sensor.swap(vector<double>());

		return 0;
		//主控端置为"000"
		controlNo = "000";

	}


	return 0;
}
/**************************************************写数据函数********************************************/

void WriteToTextU()
{
	// === 为了测试时不丢失数据，优先存入txt，但是需要延时，可能是用于数据准备吧！ ===
	qDebug() << "延时10秒,上行原始数据开始写入txt!";
	//Sleep(60000);
	//Sleep(10000);	//2016.3.26下午修改

					// 004003 右侧

	ofstream point;
#if URO
	// 004003 右侧
	point.open(g_sFilePrefix + "U-R.txt", ios::out);
	OutToText(point, qixidata[1]);
	point.close();
	Sleep(1);


#endif

#if ULO
	//  左侧
	point.open(g_sFilePrefix + "U-L.txt", ios::out);
	OutToText(point, qixiData[0]);
	point.close();
	Sleep(1);


#endif

#if UTO
	//  温度
	point.open(g_sFilePrefix + "U-T.txt", ios::out);
	OutToText(point, tempDatau);
	point.close();
	Sleep(1);


#endif

#if UDO
	//  右侧
	point.open(g_sFilePrefix + "U-R.txt", ios::out);
	OutToText(point, qixiData[1]);
	point.close();
	Sleep(1);



	//  左侧
	point.open(g_sFilePrefix + "U-L.txt", ios::out);
	OutToText(point, qixiData[0]);
	point.close();
	Sleep(1);


	//温度
	point.open(g_sFilePrefix + "U-T.txt", ios::out);
	OutToText(point, tempDatau);
	point.close();
	Sleep(1);


#endif
	qDebug() << "上行原始数据写入txt成功!";
	// === 数据处理在存TXT之后，是在处理函数中释放了内存 === //

}
void OutToText(std::ofstream & _io, const vector<double> & icv_)
{
	for (vector<double>::const_iterator it = icv_.begin(); it != icv_.end(); ++it) {
		_io << *it << '\n';
	}
	_io << '\n';
}

bool readFromDatabase_2()
{
	QSqlDatabase db2;

	QString measuretime = QString::fromStdString(gMeasureTimeU);
	QString connecName = "READ - B" + measuretime;

	if (QSqlDatabase::contains(connecName))
		db2 = QSqlDatabase::database(connecName);
	else
		db2 = QSqlDatabase::addDatabase("QMYSQL", connecName);

	db2.setDatabaseName("qixi2");
	db2.setUserName("root");
	db2.setPassword("zq930219");
	if (db2.open())
	{
		qDebug() << "open ok";

	}
	else
	{
		qDebug() << "open failed";
	}
	qDebug() << "数据库初始化完毕！";

	QSqlQuery query(db2);

	query.exec("select * from system_cali where ud_flag = 0 ");


	if (query.last()) //seek指定当前的位置
	{
		calibu_l[0] = query.value(1).toDouble();
		calibu_l[1] = query.value(2).toDouble();
		calibu_l[2] = query.value(3).toDouble();
		calibu_r[0] = query.value(4).toDouble();
		calibu_r[1] = query.value(5).toDouble();
		calibu_r[2] = query.value(6).toDouble();
		calibu_l[3] = query.value(7).toDouble();
		calibu_r[3] = query.value(8).toDouble();
		qDebug() << "上行标定值设置：" << calibu_l[0] << calibu_l[1] << calibu_l[2] << calibu_r[0] << calibu_r[1] << calibu_r[2] << calibu_l[3] << calibu_r[3];
	}

	QString name;
	{
		name = QSqlDatabase::database().connectionName();
	}//超出作用域，隐含对象QSqlDatabase::database()被删除。
	QSqlDatabase::removeDatabase(name);

	db2.close();
	return true;





}
