#pragma once
#include <vector>

using namespace std;
class ProCes
{
public:
	ProCes(vector<double>& raw, vector<double>& rawT,double (&caliB)[4], bool &Parity);
	~ProCes();

public:
	void MainProcess();    //主处理函数
	//需要插入数据库的容器

	vector<vector<double> >curve;
	vector<vector<double> >value_save;//二维容器rough_value,存放(motor_num)个电机的所有值(包括电机内部干扰数据的值)

	//vector<vector<double> >mgmax_qx;//二维容器mgmax_qx(行数即电机数),每行存放每个电机所有齿气隙最大值
	vector<vector<double> >mgmin_qx;//二维容器mgmin_qx(行数即电机数),每行存放每个电机所有齿气隙最小值
	//vector<vector<double> >mgmean_qx;//二维容器mgmean_qx(行数即电机数),每行存放每个电机所有齿气隙平均值
	vector<double>qxmax;//一维容器qxmax存储每个电机气隙最大值(8个)
	double max_qx;//所有电机的气隙最大值(1个)
	vector<double>qxmin;//一维容器qxmin存储每个电机气隙最小值(8个)
	double min_qx;//所有电机的气隙最小值(1个)
	vector<double>qxmean;//一维容器qxmean存储电机气隙平均值(8个)
	double mean_qx;//所有电机的气隙平均值(1个)

	vector<int>qmin_gid;//存储8个电机气隙最小值对应的齿的编号(8个)
	vector<int>qmax_gid;//存储8个电机气隙最大值对应的齿的编号(8个)
	int qmin_mid;//变量qmax_mid,存储气隙最小中的最大值对应的电机的编号(1个)
	int qmax_mid;//变量qmin_mid,存储气隙最小中的最小值对应的电机的编号(1个)
	
	vector<vector<double> >mgmax_cx;//二维容器mgmax_cx(行数即电机数),每行存放每个电机所有齿的槽楔最大值
	//vector<vector<double> >mgmin_cx;//二维容器mgmin_cx(行数即电机数),每行存放每个电机所有齿的槽楔最小值
	//vector<vector<double> >mgmean_cx;//二维容器mgmean_cx(行数即电机数),每行存放每个电机所有齿的槽楔平均值
	vector<double>cxmax;//一维容器cxmax存储每个电机槽楔最大值(8个)
	double max_cx;//所有电机的槽楔最大值(1个)
	vector<double>cxmin;//一维容器cxmin存储每个电机槽楔最小值(8个)
	double min_cx;//所有电机的槽楔最小值(1个)
	vector<double>cxmean;//一维容器cxmean存储电机槽楔平均值
	double mean_cx;//所有电机的槽楔平均值(1个)

	vector<int>cmin_gid;//存储8个电机槽楔最小值对应的齿的编号(8个)
	vector<int>cmax_gid;//存储8个电机槽楔最大值对应的齿的编号(8个)
	int cmin_mid;//变量cmin_mid,存储槽楔最大中的最小值对应的电机的编号(1个)
	int cmax_mid;//变量cmax_mid,存储槽楔最大中的最大值对应的电机的编号(1个)

	vector<vector<double> >cs;//二维容器cs;存储8个电机的齿槽深(槽楔最大减去气隙最小)(每行79个)
	vector<double>cs_min;//一维容器cs_min;存储8个电机对应的齿槽深的最小值(8个)
	vector<double>cs_max;//一维容器cs_max;存储8个电机对应的齿槽深的最大值(8个)
	vector<int>csmin_gid;//一维容器csmin_gid;存储8个电机齿槽深最小值对应的齿编号(8个)
	vector<int>csmax_gid;//一维容器csmax_gid;存储8个电机齿槽深最大值对应的齿编号(8个)
	vector<double>csmean; //一维容器csmean; 存储8个电机对应的齿槽深的平均值(8个)
	int max_csmid;//一维容器max_csid;存储一列车齿槽深最大机齿槽深最大值对应的齿编号(1个)
	int min_csmid;//一维容器min_csid;存储一列车齿槽深最小的电机编号(1个)
	double min_cs;//一维容器min_cs;存储一列车齿槽深最小值(1个)
	double max_cs;//一维容器max_cs;存储一列车齿槽深最大值(1个)
	double mean_cs;//一列车槽深平均值(1个)

	vector<double>qmean3_13;//一维容器qmean3_13,存储每个电机第3个齿气隙到第13个齿气隙(共11个齿气隙)的平均值(8个)
	vector<double>qmean68_78;//一维容器qmean68_78,存储每个电机第68个齿气隙到第78个齿气隙(共11个齿气隙)的平均值(8个)
	vector<double>max_qmean;//一维容器max_qmean,存储前11个齿气隙平均值与后11个齿气隙平均值的较大者(8个)
	vector<double>min_qmean;//一维容器min_qmean,存储前11个齿气隙平均值与后11个齿气隙平均值的较小者(8个)

	vector<double>t_max, t_min, t_mean;//一维容器t_max, t_min, t_mean;分别存储每个电机温度的最大值(8个)、最小值(8个)、平均值(8个)
	double max_temp, min_temp, mean_temp;//变量max_temp, min_temp, mean_temp;分别是一列车的电机温度的最大值(1个)、最小值(1个)、平均值(1个)
	vector<int>tmax_id, t_min_id;//一维容器t_max, t_min;分别存储每个电机温度的最大值的编号(8个)、最小值的编号(8个)
	int t_maxid, t_minid;//存储每列车电机温度最大、小值的电机编号（1个）

	vector<vector<double> >wd;//二维容器wd,每行存储一个电机的温度值

	vector<double>mg3min_qx;//一维容器mg3min_qx,存储第三个齿的气隙最小值(8个)
	int qmin_3id;//变量qmin_3id,存储8个电机中第三个齿的气隙最小值对应的电机编号(1个)

	vector<double>mg78min_qx;//一维容器mg78min_qx,存储第78个齿的气隙最小值(8个)
	int qmin_78id;//变量qmin_78id,存储8个电机中第78个齿的气隙最小值对应的电机编号(1个)

	vector<double>motor_speed;//一维容器motor_speed,存放8个电机的速度
	vector<double> pilot_value; //一维容器，排障器高度
	double pilot_min;

private:
	int motor_num;
	vector<double> L1;   //2路激光传感器信号
	vector<double> T1;     //1路温度信号 
	double C1[4];
	bool Pflag;//标志位
};

