#pragma once
#include <vector>

using namespace std;
class ProCes
{
public:
	ProCes(vector<double>& raw, vector<double>& rawT,double (&caliB)[4], bool &Parity);
	~ProCes();

public:
	void MainProcess();    //��������
	//��Ҫ�������ݿ������

	vector<vector<double> >curve;
	vector<vector<double> >value_save;//��ά����rough_value,���(motor_num)�����������ֵ(��������ڲ��������ݵ�ֵ)

	//vector<vector<double> >mgmax_qx;//��ά����mgmax_qx(�����������),ÿ�д��ÿ��������г���϶���ֵ
	vector<vector<double> >mgmin_qx;//��ά����mgmin_qx(�����������),ÿ�д��ÿ��������г���϶��Сֵ
	//vector<vector<double> >mgmean_qx;//��ά����mgmean_qx(�����������),ÿ�д��ÿ��������г���϶ƽ��ֵ
	vector<double>qxmax;//һά����qxmax�洢ÿ�������϶���ֵ(8��)
	double max_qx;//���е������϶���ֵ(1��)
	vector<double>qxmin;//һά����qxmin�洢ÿ�������϶��Сֵ(8��)
	double min_qx;//���е������϶��Сֵ(1��)
	vector<double>qxmean;//һά����qxmean�洢�����϶ƽ��ֵ(8��)
	double mean_qx;//���е������϶ƽ��ֵ(1��)

	vector<int>qmin_gid;//�洢8�������϶��Сֵ��Ӧ�ĳݵı��(8��)
	vector<int>qmax_gid;//�洢8�������϶���ֵ��Ӧ�ĳݵı��(8��)
	int qmin_mid;//����qmax_mid,�洢��϶��С�е����ֵ��Ӧ�ĵ���ı��(1��)
	int qmax_mid;//����qmin_mid,�洢��϶��С�е���Сֵ��Ӧ�ĵ���ı��(1��)
	
	vector<vector<double> >mgmax_cx;//��ά����mgmax_cx(�����������),ÿ�д��ÿ��������гݵĲ�Ш���ֵ
	//vector<vector<double> >mgmin_cx;//��ά����mgmin_cx(�����������),ÿ�д��ÿ��������гݵĲ�Ш��Сֵ
	//vector<vector<double> >mgmean_cx;//��ά����mgmean_cx(�����������),ÿ�д��ÿ��������гݵĲ�Шƽ��ֵ
	vector<double>cxmax;//һά����cxmax�洢ÿ�������Ш���ֵ(8��)
	double max_cx;//���е���Ĳ�Ш���ֵ(1��)
	vector<double>cxmin;//һά����cxmin�洢ÿ�������Ш��Сֵ(8��)
	double min_cx;//���е���Ĳ�Ш��Сֵ(1��)
	vector<double>cxmean;//һά����cxmean�洢�����Шƽ��ֵ
	double mean_cx;//���е���Ĳ�Шƽ��ֵ(1��)

	vector<int>cmin_gid;//�洢8�������Ш��Сֵ��Ӧ�ĳݵı��(8��)
	vector<int>cmax_gid;//�洢8�������Ш���ֵ��Ӧ�ĳݵı��(8��)
	int cmin_mid;//����cmin_mid,�洢��Ш����е���Сֵ��Ӧ�ĵ���ı��(1��)
	int cmax_mid;//����cmax_mid,�洢��Ш����е����ֵ��Ӧ�ĵ���ı��(1��)

	vector<vector<double> >cs;//��ά����cs;�洢8������ĳݲ���(��Ш����ȥ��϶��С)(ÿ��79��)
	vector<double>cs_min;//һά����cs_min;�洢8�������Ӧ�ĳݲ������Сֵ(8��)
	vector<double>cs_max;//һά����cs_max;�洢8�������Ӧ�ĳݲ�������ֵ(8��)
	vector<int>csmin_gid;//һά����csmin_gid;�洢8������ݲ�����Сֵ��Ӧ�ĳݱ��(8��)
	vector<int>csmax_gid;//һά����csmax_gid;�洢8������ݲ������ֵ��Ӧ�ĳݱ��(8��)
	vector<double>csmean; //һά����csmean; �洢8�������Ӧ�ĳݲ����ƽ��ֵ(8��)
	int max_csmid;//һά����max_csid;�洢һ�г��ݲ��������ݲ������ֵ��Ӧ�ĳݱ��(1��)
	int min_csmid;//һά����min_csid;�洢һ�г��ݲ�����С�ĵ�����(1��)
	double min_cs;//һά����min_cs;�洢һ�г��ݲ�����Сֵ(1��)
	double max_cs;//һά����max_cs;�洢һ�г��ݲ������ֵ(1��)
	double mean_cs;//һ�г�����ƽ��ֵ(1��)

	vector<double>qmean3_13;//һά����qmean3_13,�洢ÿ�������3������϶����13������϶(��11������϶)��ƽ��ֵ(8��)
	vector<double>qmean68_78;//һά����qmean68_78,�洢ÿ�������68������϶����78������϶(��11������϶)��ƽ��ֵ(8��)
	vector<double>max_qmean;//һά����max_qmean,�洢ǰ11������϶ƽ��ֵ���11������϶ƽ��ֵ�Ľϴ���(8��)
	vector<double>min_qmean;//һά����min_qmean,�洢ǰ11������϶ƽ��ֵ���11������϶ƽ��ֵ�Ľ�С��(8��)

	vector<double>t_max, t_min, t_mean;//һά����t_max, t_min, t_mean;�ֱ�洢ÿ������¶ȵ����ֵ(8��)����Сֵ(8��)��ƽ��ֵ(8��)
	double max_temp, min_temp, mean_temp;//����max_temp, min_temp, mean_temp;�ֱ���һ�г��ĵ���¶ȵ����ֵ(1��)����Сֵ(1��)��ƽ��ֵ(1��)
	vector<int>tmax_id, t_min_id;//һά����t_max, t_min;�ֱ�洢ÿ������¶ȵ����ֵ�ı��(8��)����Сֵ�ı��(8��)
	int t_maxid, t_minid;//�洢ÿ�г�����¶����Сֵ�ĵ����ţ�1����

	vector<vector<double> >wd;//��ά����wd,ÿ�д洢һ��������¶�ֵ

	vector<double>mg3min_qx;//һά����mg3min_qx,�洢�������ݵ���϶��Сֵ(8��)
	int qmin_3id;//����qmin_3id,�洢8������е������ݵ���϶��Сֵ��Ӧ�ĵ�����(1��)

	vector<double>mg78min_qx;//һά����mg78min_qx,�洢��78���ݵ���϶��Сֵ(8��)
	int qmin_78id;//����qmin_78id,�洢8������е�78���ݵ���϶��Сֵ��Ӧ�ĵ�����(1��)

	vector<double>motor_speed;//һά����motor_speed,���8��������ٶ�
	vector<double> pilot_value; //һά�������������߶�
	double pilot_min;

private:
	int motor_num;
	vector<double> L1;   //2·���⴫�����ź�
	vector<double> T1;     //1·�¶��ź� 
	double C1[4];
	bool Pflag;//��־λ
};

