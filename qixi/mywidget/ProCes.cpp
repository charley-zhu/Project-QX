#include<iostream>
#include<vector>
#include<fstream>
#include <string>
#include<numeric>
#include<algorithm>
#include "ProCes.h"
#include<functional>
#include<QDebug>
#include <Windows.h>

using namespace std;
using std::cout;

ProCes::ProCes(vector<double>& raw, vector<double>& rawT, double(&caliB)[4], bool &Parity) :
	L1(raw), T1(rawT), Pflag(Parity)
{
	memcpy(C1, caliB, sizeof(caliB));


}


ProCes::~ProCes()
{
}

void ProCes::MainProcess()
{

	vector<long> raw_index, motordist_index, index_position, bigdist;
	vector<double>bigdist_value;
	double minus = 0;
	double rawindex_count = 0;
	double data, motor_threshold;
	int t_motor_num;
	int size_num = 0;
	int temp_num = 0;


	vector<double>value_cos;
	value_cos.push_back(20 / (C1[1] - C1[0]));
	value_cos.push_back(20 / (C1[2] - C1[1]));
	value_cos.push_back(40 / (C1[2] - C1[0]));

	for (int i = 0; i != 3; ++i)
	{
		if (value_cos[i] > 1)
			value_cos[i] = 1;
	}

	double iter0 = accumulate(begin(value_cos), end(value_cos), 0.0);
	double cos_value = iter0 / (value_cos.size());

	if (*L1.rbegin() > 0)
	{
		auto iter_finalminus = find_if(L1.rbegin(), L1.rend(), [minus](double n) { return (n < minus); });
		iter_finalminus.base() = L1.erase((iter_finalminus).base(), (L1.rbegin()).base());
	}
	//2018 7/30 ԭʼ����size�ж�
	for (vector<double>::iterator it = L1.begin(); it != L1.end(); it++)
	{
		if (*it > -10)
		{
			size_num = size_num + 1;
		}
	}
	if (size_num < 10000)
	{
		throw size_num;
	}


	//*******************************
	
	int moutain_num = 0;//����ͻ�����֣�������˵�ǵ����
	vector<vector<double>> value_save_speed;//�����ٶ�
	value_save.resize(8);
	value_save_speed.resize(8);
	vector<vector<double>> value_save_pilot;
	value_save_pilot.resize(8);
	for (auto it1 = L1.begin(); it1 != L1.end() - 1; it1++)
	{
		if (*it1 < 0 && *(it1 + 1)>0)
		{
			int value_num = 0;
			for (auto it2 = it1 + 1; *it2 > 0; it2++)
			{
				value_num++;
			}
			if (value_num > 3000)
			{
				vector<double>::iterator it2;
				for (it2 = it1 + 1; *it2 > 0; it2++)
				{
					value_save.at(moutain_num).push_back(*it2);
				}
				for (auto it3 = it1 - 300; it3 != it2 + 300; it3++)
				{
					value_save_pilot.at(moutain_num).push_back(*it3);
				}
				moutain_num++;
			}
		}
	}//�д��Ľ����ֵ��Ӧ
	int motor_num = moutain_num;
	for (int m = 0; m != 8; m++)
	{
		value_save_speed.at(m).assign(value_save.at(m).begin(), value_save.at(m).end());
	}

	//***********************��ǰԤ����ȥ�����Ե�������*****************************(���������е���ֵѡȡ��һ�£�
	for (int j = 0; j != 4; j++)
	{
		for (int m = 0; m != 8; m++)
		{
			int num = 0;
			for (auto it1 = value_save.at(m).begin(); num < 60 && it1 != value_save.at(m).end() - 1; it1++)
			{
				num = 0;
				if (*(it1 + 1) == *it1)
				{
					for (auto it2 = it1; *it2 == *it1; it2++)
					{
						num = it2 - it1;
					}
					if (num > 60)
					{
						for (auto it3 = it1; it3 != it1 + num; it3++)
						{
							*it3 = 0;
						}
					}
				}
			}
		}
		for (int m = 0; m != 8; m++)
		{
			for (auto it = value_save.at(m).begin(); it != value_save.at(m).end();)
			{
				if (*it < 100 || *it>124)
					it = value_save.at(m).erase(it);
				else
					it++;
			}
		}
	}

	vector<vector<double>> value_save_cz;
	value_save_cz.resize(8);
	for (int m = 0; m != 8; m++)
	{
		for (auto it = value_save.at(m).begin(); it != value_save.at(m).end() - 1; it++)
		{
			double num = abs(*(it + 1) - *it);
			value_save_cz.at(m).push_back(num);
		}
	}

	vector<vector<double>> value_save_point;
	value_save_point.resize(8);
	vector<vector<double>> value_save_point_num;
	value_save_point_num.resize(8);
	for (int m = 0; m != 8; m++)
	{
		for (auto it = value_save_cz.at(m).begin(); it != value_save_cz.at(m).end(); it++)
		{
			int num = it - value_save_cz.at(m).begin();
			if (*it > 0.1)
				value_save_point.at(m).push_back(num + 1);
		}
		if (value_save_point.at(m).size())
		{
			if (value_save_point.at(m).size() > 1)
			{
				value_save_point_num.at(m).push_back(value_save_point.at(m).at(0));
				for (auto it = value_save_point.at(m).begin(); it != value_save_point.at(m).end() - 1; it++)
				{
					value_save_point_num.at(m).push_back(*(it + 1) - *it);
				}
				value_save_point_num.at(m).push_back(value_save_cz.at(m).size() + 1 - *(value_save_point.at(m).end() - 1));
			}
			else
			{
				value_save_point_num.at(m).push_back(value_save_point.at(m).at(0));
				value_save_point_num.at(m).push_back(value_save_cz.at(m).size() + 1 - *(value_save_point.at(m).end() - 1));
			}
			int sum = 0;
			for (auto it = value_save_point_num.at(m).begin(); it != value_save_point_num.at(m).end(); it++)
			{
				sum = sum + *it;
				if (*it <= 5)
				{
					if (*it == 1)
					{
						value_save.at(m).at(sum - *it) = 1;
					}
					else
						for (int j = sum - *it; j != sum; j++)
						{
							value_save.at(m).at(j) = 1;
						}

				}
			}
		}
	}


	for (int m = 0; m != 8; m++)
	{
		for (auto it = value_save.at(m).begin(); it != value_save.at(m).end();)
		{
			if (*it == 1)
				it = value_save.at(m).erase(it);
			else
				it++;
		}
	}
	if (Pflag == false)
	{
		reverse(value_save.begin(), value_save.end());
		for (int m = 0; m != 8; m++)
		{
			reverse(value_save.at(m).begin(), value_save.at(m).end());
		}
	}
	//**************************Ѱ��ÿ������ݵ���ʼλ��*******************************
	vector<vector<double>> motor_start_cz; //��ŵ����ֵ
	motor_start_cz.resize(8);
	vector<double>start_point;//��ų�ʼ�˵����һ����϶��
	vector<int>start_point_index;//��ų�ʼ�˵Ŀ�ʼλ������
	vector<double>end_point;//���ĩ�˵����һ����϶��
	vector<int>end_point_index;//���ĩ�˵���ʼ�������ֵ
	vector<vector<double>> value_save_start;

	value_save_start.assign(value_save.begin(), value_save.end()); //����
	for (int m = 0; m != 8; m++) //ȥ��ÿ�������ǰ40�ͺ�40���㣨�����ɵ���
	{
		value_save_start.at(m).erase(value_save_start.at(m).begin(), value_save_start.at(m).begin() + 50);
		value_save_start.at(m).erase(value_save_start.at(m).end() - 51, value_save_start.at(m).end());
	}

	for (int m = 0; m != 8; m++) //�����ֵ
	{
		for (auto it = value_save_start.at(m).begin(); it != value_save_start.at(m).end() - 1; it++)
		{
			double num = abs(*(it + 1) - *it);
			motor_start_cz.at(m).push_back(num);
		}
	}

	vector<vector<double>> value_point_s;
	value_point_s.resize(8);
	vector<vector<double>> value_point_num_s;
	value_point_num_s.resize(8);

	for (int m = 0; m != 8; m++)  //��ֵ����ѡ��0.3
	{
		for (auto it = motor_start_cz.at(m).begin(); it != motor_start_cz.at(m).end(); it++)
		{
			int num = it - motor_start_cz.at(m).begin();
			if (*it > 0.3)
				value_point_s.at(m).push_back(num + 1);
		}
	}

	for (int m = 0; m != 8; m++) //����
	{
		if (value_point_s.at(m).size())
		{
			if (value_point_s.at(m).size() > 1)
			{
				value_point_num_s.at(m).push_back(value_point_s.at(m).at(0));
				for (auto it = value_point_s.at(m).begin(); it != value_point_s.at(m).end() - 1; it++)
				{
					value_point_num_s.at(m).push_back(*(it + 1) - *it);
				}
				value_point_num_s.at(m).push_back(motor_start_cz.at(m).size() + 1 - *(value_point_s.at(m).end() - 1));
			}
			else
			{
				value_point_num_s.at(m).push_back(value_point_s.at(m).at(0));
				value_point_num_s.at(m).push_back(motor_start_cz.at(m).size() + 1 - *(value_point_s.at(m).end() - 1));
			}
		}
	}

	vector<vector<double>> value_pjz_s; //����ƽ��ֵ
	value_pjz_s.resize(8);
	for (int m = 0; m != 8; m++)
	{
		double sum = 0;
		for (auto it = value_point_num_s.at(m).begin(); it != value_point_num_s.at(m).end(); it++)
		{
			double temp = accumulate(value_save_start.at(m).begin() + sum, value_save_start.at(m).begin() + sum + *it, 0.0);
			double pjz = temp / (*it);
			value_pjz_s.at(m).push_back(pjz);
			sum = sum + *it;
		}
	}

	vector<vector<double>> test1;
	test1.resize(8);
	for (int m = 0; m != 8; m++)
	{
		for (auto it = value_pjz_s.at(m).begin(); it != value_pjz_s.at(m).end() - 1; it++)
		{
			double num = *(it + 1) - *it;
			if (num > 0)
				test1.at(m).push_back((it + 1) - value_pjz_s.at(m).begin());
			else
				test1.at(m).push_back(it - value_pjz_s.at(m).begin());
		}
	}
	vector<vector<double>> test1_index; //�����Ш������ֵ
	test1_index.resize(8);
	for (int m = 0; m != 8; m++)
	{
		for (auto it = test1.at(m).begin(); it != test1.at(m).end() - 1; it++)
		{
			if (*(it + 1) - *it == 0)
				test1_index.at(m).push_back(*(it + 1));
		}
	}

	for (int m = 0; m != 8; m++) //�����ʼλ��
	{
		int flag = 0;
		for (auto it = test1_index.at(m).begin(); flag == 0; it++)
		{
			auto a = value_point_num_s.at(m).at(*it);
			auto b = value_point_num_s.at(m).at(*(it + 1));
			auto c = value_pjz_s.at(m).at(*it);
			auto d = value_pjz_s.at(m).at(*(it + 1));
			if (abs(a - b) <= 13 && abs(c - d) < 0.5)
			{
				flag = 1;
				auto index = value_point_s.at(m).at(*(it)-1) + 49;
				auto value = value_save.at(m).at(index);
				start_point.push_back(value);
				start_point_index.push_back(index);
			}
		}
	}

	for (int m = 0; m != 8; m++) //������ֹλ��
	{
		int flag = 0;
		for (auto it = test1_index.at(m).end() - 1; flag == 0; it--)
		{
			auto a = value_point_num_s.at(m).at(*it);
			auto b = value_point_num_s.at(m).at(*(it - 1));
			if (abs(a - b) <= 13)
			{
				flag = 1;
				auto index = (value_point_s.at(m).at(*(it)) + 50);
				auto value = value_save.at(m).at(index);
				end_point.push_back(value);
				end_point_index.push_back(value_save.at(m).size() - index - 1);
			}
		}
	}
	//****************************************��ʼ��****************************************��ʼ��1-40��ÿ2���Ƚ�һ��
	vector<vector<double>> start_cx_index; //��Ŵӿ�ʼλ�ÿ�ʼ�Ĳ�Ш����
	start_cx_index.resize(8);
	for (int m = 0; m != 8; m++)
	{
		double temp = start_point.at(m) + 0.5; // ȡһ������ֵ
		for (auto it = value_save.at(m).begin() + start_point_index.at(m) + 1; it != value_save.at(m).begin() + start_point_index.at(m) + 500; it++)
		{
			if (*it > temp) //��������������ֵ
			{
				int index = it - value_save.at(m).begin();
				start_cx_index.at(m).push_back(index);//������ֵ����start_cx_index��
			}
		}
	}
	vector<vector<double>> cx_index_cz;//��Ш����ֵ�Ĳ�ֵ
	cx_index_cz.resize(8);
	for (int m = 0; m != 8; m++)
	{
		for (auto it = start_cx_index.at(m).begin(); it != start_cx_index.at(m).end() - 1; it++)
		{
			cx_index_cz.at(m).push_back(*(it + 1) - *it);
		}
	}
	for (int m = 0; m != 8; m++)
	{
		for (auto it = cx_index_cz.at(m).begin(); it != cx_index_cz.at(m).end(); it++)
		{
			if (*it > 1 && *it < 10)
				*it = 1;
		}
	}
	vector<vector<double>> un1index; //��һ�η�1����
	un1index.resize(8);
	vector<vector<double>> cx_index_s;//��һ�β�Ш����
	cx_index_s.resize(8);
	for (int m = 0; m != 8; m++)
	{
		cx_index_s.at(m).push_back(start_cx_index.at(m).at(0));
		for (auto it = cx_index_cz.at(m).begin(); it != cx_index_cz.at(m).end(); it++)
		{
			if (*it != 1)
				un1index.at(m).push_back(it - cx_index_cz.at(m).begin());
		}
		double a = start_cx_index.at(m).at(un1index.at(m).at(0));
		double b = start_cx_index.at(m).at(un1index.at(m).at(0) + 1);
		/*double c = start_cx_index.at(m).at(un1index.at(m).at(1));*/
		cx_index_s.at(m).push_back(a);
		cx_index_s.at(m).push_back(b);
		/*cx_index_s.at(m).push_back(c);*/
	}
	start_cx_index.clear();
	cx_index_cz.clear();
	un1index.clear();
	//*****************��ʼ�ε�ѭ��*********************
	double start_point_2;
	double start_point_index_2;
	for (int m = 0; m != 8; m++)
	{
		int j = 0;
		for (double n = cx_index_s.at(m).back(); j < 80;)
		{
			int flag = 0;
			for (auto it = value_save.at(m).begin() + n - 7; it != value_save.at(m).begin() + n && flag == 0; it++)
			{
				if (*(it + 1) - *it > 0.19)
				{
					flag = 1;
					start_point_2 = (*(it));
					start_point_index_2 = (it - value_save.at(m).begin());
				}
			}
			if (flag == 0)
			{
				start_point_2 = (value_save.at(m).at(n));
				start_point_index_2 = (n);
			}
			start_cx_index.resize(8);
			cx_index_cz.resize(8);
			un1index.resize(8);

			double temp = start_point_2 + 0.5;// ȡһ������ֵ
			for (auto it = value_save.at(m).begin() + start_point_index_2 + 1; it != value_save.at(m).begin() + start_point_index_2 + 500; it++)
			{
				if (*it > temp) //��������������ֵ
				{
					int index = it - value_save.at(m).begin();
					start_cx_index.at(m).push_back(index);//������ֵ����start_cx_index��
				}
			}

			for (auto it = start_cx_index.at(m).begin(); it != start_cx_index.at(m).end() - 1; it++)
			{
				cx_index_cz.at(m).push_back(*(it + 1) - *it);
			}

			for (auto it = cx_index_cz.at(m).begin(); it != cx_index_cz.at(m).end(); it++)
			{
				if (*it > 1 && *it < 10)
					*it = 1;
			}

			for (auto it = cx_index_cz.at(m).begin(); it != cx_index_cz.at(m).end(); it++)
			{
				if (*it != 1)
					un1index.at(m).push_back(it - cx_index_cz.at(m).begin());
			}
			if (un1index.at(m).size())
			{
				double a = start_cx_index.at(m).at(un1index.at(m).at(0));
				double b = start_cx_index.at(m).at(un1index.at(m).at(0) + 1);
				/*double c = start_cx_index.at(m).at(un1index.at(m).at(1));*/
				cx_index_s.at(m).push_back(a);
				cx_index_s.at(m).push_back(b);

				start_cx_index.clear();
				cx_index_cz.clear();
				un1index.clear();

				j = cx_index_s.at(m).size();
				n = cx_index_s.at(m).at(j - 1);
			}
			else
			{
				start_cx_index.clear();
				cx_index_cz.clear();
				un1index.clear();

				start_cx_index.resize(8);
				cx_index_cz.resize(8);
				un1index.resize(8);

				temp = start_point_2 + 1;

				for (auto it = value_save.at(m).begin() + start_point_index_2 + 1; it != value_save.at(m).begin() + start_point_index_2 + 500; it++)
				{
					if (*it > temp) //��������������ֵ
					{
						int index = it - value_save.at(m).begin();
						start_cx_index.at(m).push_back(index);//������ֵ����start_cx_index��
					}
				}

				for (auto it = start_cx_index.at(m).begin(); it != start_cx_index.at(m).end() - 1; it++)
				{
					cx_index_cz.at(m).push_back(*(it + 1) - *it);
				}

				for (auto it = cx_index_cz.at(m).begin(); it != cx_index_cz.at(m).end(); it++)
				{
					if (*it > 1 && *it < 10)
						*it = 1;
				}

				for (auto it = cx_index_cz.at(m).begin(); it != cx_index_cz.at(m).end(); it++)
				{
					if (*it != 1)
						un1index.at(m).push_back(it - cx_index_cz.at(m).begin());
				}
				if (un1index.at(m).size())
				{
					double a = start_cx_index.at(m).at(un1index.at(m).at(0));
					double b = start_cx_index.at(m).at(un1index.at(m).at(0) + 1);
					/*double c = start_cx_index.at(m).at(un1index.at(m).at(1));*/
					cx_index_s.at(m).push_back(a);
					cx_index_s.at(m).push_back(b);

					start_cx_index.clear();
					cx_index_cz.clear();
					un1index.clear();

					j = cx_index_s.at(m).size();
					n = cx_index_s.at(m).at(j - 1);
				}
				else
				{
					start_cx_index.clear();
					cx_index_cz.clear();
					un1index.clear();

					start_cx_index.resize(8);
					cx_index_cz.resize(8);
					un1index.resize(8);

					temp = start_point_2 + 2;

					for (auto it = value_save.at(m).begin() + start_point_index_2 + 1; it != value_save.at(m).begin() + start_point_index_2 + 500; it++)
					{
						if (*it > temp) //��������������ֵ
						{
							int index = it - value_save.at(m).begin();
							start_cx_index.at(m).push_back(index);//������ֵ����start_cx_index��
						}
					}

					for (auto it = start_cx_index.at(m).begin(); it != start_cx_index.at(m).end() - 1; it++)
					{
						cx_index_cz.at(m).push_back(*(it + 1) - *it);
					}

					for (auto it = cx_index_cz.at(m).begin(); it != cx_index_cz.at(m).end(); it++)
					{
						if (*it > 1 && *it < 10)
							*it = 1;
					}

					for (auto it = cx_index_cz.at(m).begin(); it != cx_index_cz.at(m).end(); it++)
					{
						if (*it != 1)
							un1index.at(m).push_back(it - cx_index_cz.at(m).begin());
					}
					if (un1index.at(m).size())
					{
						double a = start_cx_index.at(m).at(un1index.at(m).at(0));
						double b = start_cx_index.at(m).at(un1index.at(m).at(0) + 1);
						/*double c = start_cx_index.at(m).at(un1index.at(m).at(1));*/
						cx_index_s.at(m).push_back(a);
						cx_index_s.at(m).push_back(b);

						start_cx_index.clear();
						cx_index_cz.clear();
						un1index.clear();

						j = cx_index_s.at(m).size();
						n = cx_index_s.at(m).at(j - 1);
					}
					else
					{
						start_cx_index.clear();
						cx_index_cz.clear();
						un1index.clear();

						start_cx_index.resize(8);
						cx_index_cz.resize(8);
						un1index.resize(8);

						temp = start_point_2 + 3;

						for (auto it = value_save.at(m).begin() + start_point_index_2 + 1; it != value_save.at(m).begin() + start_point_index_2 + 500; it++)
						{
							if (*it > temp) //��������������ֵ
							{
								int index = it - value_save.at(m).begin();
								start_cx_index.at(m).push_back(index);//������ֵ����start_cx_index��
							}
						}

						for (auto it = start_cx_index.at(m).begin(); it != start_cx_index.at(m).end() - 1; it++)
						{
							cx_index_cz.at(m).push_back(*(it + 1) - *it);
						}

						for (auto it = cx_index_cz.at(m).begin(); it != cx_index_cz.at(m).end(); it++)
						{
							if (*it > 1 && *it < 10)
								*it = 1;
						}

						for (auto it = cx_index_cz.at(m).begin(); it != cx_index_cz.at(m).end(); it++)
						{
							if (*it != 1)
								un1index.at(m).push_back(it - cx_index_cz.at(m).begin());
						}
						double a = start_cx_index.at(m).at(un1index.at(m).at(0));
						double b = start_cx_index.at(m).at(un1index.at(m).at(0) + 1);
						/*double c = start_cx_index.at(m).at(un1index.at(m).at(1));*/
						cx_index_s.at(m).push_back(a);
						cx_index_s.at(m).push_back(b);

						start_cx_index.clear();
						cx_index_cz.clear();
						un1index.clear();

						j = cx_index_s.at(m).size();
						n = cx_index_s.at(m).at(j - 1);
					}
				}
			}
		}
	}

	//*********************************�ӽ����ο�ʼ*****************************
	for (int m = 0; m != 8; m++)
	{
		reverse(value_save.at(m).begin(), value_save.at(m).end());  //�ߵ�����
	}

	vector<vector<double>> end_cx_index; //��Ŵӿ�ʼλ�ÿ�ʼ�Ĳ�Ш����
	end_cx_index.resize(8);
	for (int m = 0; m != 8; m++)
	{
		double temp = end_point.at(m) + 0.5; // ȡһ������ֵ
		for (auto it = value_save.at(m).begin() + end_point_index.at(m) + 1; it != value_save.at(m).begin() + end_point_index.at(m) + 500; it++)
		{
			if (*it > temp) //��������������ֵ
			{
				int index = it - value_save.at(m).begin();
				end_cx_index.at(m).push_back(index);//������ֵ����start_cx_index��
			}
		}
	}
	vector<vector<double>> cx_index_cz_2;//��Ш����ֵ�Ĳ�ֵ
	cx_index_cz_2.resize(8);
	for (int m = 0; m != 8; m++)
	{
		for (auto it = end_cx_index.at(m).begin(); it != end_cx_index.at(m).end() - 1; it++)
		{
			cx_index_cz_2.at(m).push_back(*(it + 1) - *it);
		}
	}
	for (int m = 0; m != 8; m++)
	{
		for (auto it = cx_index_cz_2.at(m).begin(); it != cx_index_cz_2.at(m).end(); it++)
		{
			if (*it > 1 && *it < 10)
				*it = 1;
		}
	}
	vector<vector<double>> un1index_2; //��һ�η�1����
	un1index_2.resize(8);
	vector<vector<double>> cx_index_e;//��һ�β�Ш����
	cx_index_e.resize(8);
	for (int m = 0; m != 8; m++)
	{
		cx_index_e.at(m).push_back(end_cx_index.at(m).at(0));
		for (auto it = cx_index_cz_2.at(m).begin(); it != cx_index_cz_2.at(m).end(); it++)
		{
			if (*it != 1)
				un1index_2.at(m).push_back(it - cx_index_cz_2.at(m).begin());
		}
		double a = end_cx_index.at(m).at(un1index_2.at(m).at(0));
		double b = end_cx_index.at(m).at(un1index_2.at(m).at(0) + 1);
		/*double c = start_cx_index.at(m).at(un1index.at(m).at(1));*/
		cx_index_e.at(m).push_back(a);
		cx_index_e.at(m).push_back(b);
		/*cx_index_s.at(m).push_back(c);*/
	}
	end_cx_index.clear();
	cx_index_cz_2.clear();
	un1index_2.clear();
	//*****************�����ε�ѭ��*********************
	double end_point_2;
	double end_point_index_2;
	for (int m = 0; m != 8; m++)
	{
		int j = 0;
		for (double n = cx_index_e.at(m).back(); j < 80;)
		{
			int flag = 0;
			for (auto it = value_save.at(m).begin() + n - 7; it != value_save.at(m).begin() + n && flag == 0; it++)
			{
				if (*(it + 1) - *it > 0.19)
				{
					flag = 1;
					end_point_2 = (*(it));
					end_point_index_2 = (it - value_save.at(m).begin());
				}
			}
			if (flag == 0)
			{
				end_point_2 = (value_save.at(m).at(n));
				end_point_index_2 = (n);
			}
			end_cx_index.resize(8);
			cx_index_cz_2.resize(8);
			un1index_2.resize(8);

			double temp = end_point_2 + 0.5;// ȡһ������ֵ
			for (auto it = value_save.at(m).begin() + end_point_index_2 + 1; it != value_save.at(m).begin() + end_point_index_2 + 500; it++)
			{
				if (*it > temp) //��������������ֵ
				{
					int index = it - value_save.at(m).begin();
					end_cx_index.at(m).push_back(index);//������ֵ����start_cx_index��
				}
			}
			for (auto it = end_cx_index.at(m).begin(); it != end_cx_index.at(m).end() - 1; it++)
			{
				cx_index_cz_2.at(m).push_back(*(it + 1) - *it);
			}

			for (auto it = cx_index_cz_2.at(m).begin(); it != cx_index_cz_2.at(m).end(); it++)
			{
				if (*it > 1 && *it < 5)
					*it = 1;
			}
			for (auto it = cx_index_cz_2.at(m).begin(); it != cx_index_cz_2.at(m).end(); it++)
			{
				if (*it != 1)
					un1index_2.at(m).push_back(it - cx_index_cz_2.at(m).begin());
			}
			if (un1index_2.at(m).size())
			{
				double a = end_cx_index.at(m).at(un1index_2.at(m).at(0));
				double b = end_cx_index.at(m).at(un1index_2.at(m).at(0) + 1);
				/*double c = start_cx_index.at(m).at(un1index.at(m).at(1));*/
				cx_index_e.at(m).push_back(a);
				cx_index_e.at(m).push_back(b);

				end_cx_index.clear();
				cx_index_cz_2.clear();
				un1index_2.clear();

				j = cx_index_e.at(m).size();
				n = cx_index_e.at(m).at(j - 1);
			}
			else
			{
				temp = end_point_2 + 1;
				end_cx_index.clear();
				cx_index_cz_2.clear();
				un1index_2.clear();

				end_cx_index.resize(8);
				cx_index_cz_2.resize(8);
				un1index_2.resize(8);

				for (auto it = value_save.at(m).begin() + end_point_index_2 + 1; it != value_save.at(m).begin() + end_point_index_2 + 500; it++)
				{
					if (*it > temp) //��������������ֵ
					{
						int index = it - value_save.at(m).begin();
						end_cx_index.at(m).push_back(index);//������ֵ����start_cx_index��
					}
				}
				for (auto it = end_cx_index.at(m).begin(); it != end_cx_index.at(m).end() - 1; it++)
				{
					cx_index_cz_2.at(m).push_back(*(it + 1) - *it);
				}
				for (auto it = cx_index_cz_2.at(m).begin(); it != cx_index_cz_2.at(m).end(); it++)
				{
					if (*it > 1 && *it < 5)
						*it = 1;
				}
				for (auto it = cx_index_cz_2.at(m).begin(); it != cx_index_cz_2.at(m).end(); it++)
				{
					if (*it != 1)
						un1index_2.at(m).push_back(it - cx_index_cz_2.at(m).begin());
				}
				if (un1index_2.at(m).size())
				{
					double a = end_cx_index.at(m).at(un1index_2.at(m).at(0));
					double b = end_cx_index.at(m).at(un1index_2.at(m).at(0) + 1);
					/*double c = start_cx_index.at(m).at(un1index.at(m).at(1));*/
					cx_index_e.at(m).push_back(a);
					cx_index_e.at(m).push_back(b);

					end_cx_index.clear();
					cx_index_cz_2.clear();
					un1index_2.clear();

					j = cx_index_e.at(m).size();
					n = cx_index_e.at(m).at(j - 1);
				}
				else
				{
					temp = end_point_2 + 2;
					end_cx_index.clear();
					cx_index_cz_2.clear();
					un1index_2.clear();

					end_cx_index.resize(8);
					cx_index_cz_2.resize(8);
					un1index_2.resize(8);

					for (auto it = value_save.at(m).begin() + end_point_index_2 + 1; it != value_save.at(m).begin() + end_point_index_2 + 250; it++)
					{
						if (*it > temp) //��������������ֵ
						{
							int index = it - value_save.at(m).begin();
							end_cx_index.at(m).push_back(index);//������ֵ����start_cx_index��
						}
					}
					for (auto it = end_cx_index.at(m).begin(); it != end_cx_index.at(m).end() - 1; it++)
					{
						cx_index_cz_2.at(m).push_back(*(it + 1) - *it);
					}
					for (auto it = cx_index_cz_2.at(m).begin(); it != cx_index_cz_2.at(m).end(); it++)
					{
						if (*it > 1 && *it < 5)
							*it = 1;
					}
					for (auto it = cx_index_cz_2.at(m).begin(); it != cx_index_cz_2.at(m).end(); it++)
					{
						if (*it != 1)
							un1index_2.at(m).push_back(it - cx_index_cz_2.at(m).begin());
					}
					if (un1index_2.at(m).size())
					{
						double a = end_cx_index.at(m).at(un1index_2.at(m).at(0));
						double b = end_cx_index.at(m).at(un1index_2.at(m).at(0) + 1);
						/*double c = start_cx_index.at(m).at(un1index.at(m).at(1));*/
						cx_index_e.at(m).push_back(a);
						cx_index_e.at(m).push_back(b);

						end_cx_index.clear();
						cx_index_cz_2.clear();
						un1index_2.clear();

						j = cx_index_e.at(m).size();
						n = cx_index_e.at(m).at(j - 1);
					}
					else
					{
						temp = end_point_2 + 3;
						end_cx_index.clear();
						cx_index_cz_2.clear();
						un1index_2.clear();

						end_cx_index.resize(8);
						cx_index_cz_2.resize(8);
						un1index_2.resize(8);

						for (auto it = value_save.at(m).begin() + end_point_index_2 + 1; it != value_save.at(m).begin() + end_point_index_2 + 250; it++)
						{
							if (*it > temp) //��������������ֵ
							{
								int index = it - value_save.at(m).begin();
								end_cx_index.at(m).push_back(index);//������ֵ����start_cx_index��
							}
						}
						for (auto it = end_cx_index.at(m).begin(); it != end_cx_index.at(m).end() - 1; it++)
						{
							cx_index_cz_2.at(m).push_back(*(it + 1) - *it);
						}
						for (auto it = cx_index_cz_2.at(m).begin(); it != cx_index_cz_2.at(m).end(); it++)
						{
							if (*it > 1 && *it < 5)
								*it = 1;
						}
						for (auto it = cx_index_cz_2.at(m).begin(); it != cx_index_cz_2.at(m).end(); it++)
						{
							if (*it != 1)
								un1index_2.at(m).push_back(it - cx_index_cz_2.at(m).begin());
						}
						double a = end_cx_index.at(m).at(un1index_2.at(m).at(0));
						double b = end_cx_index.at(m).at(un1index_2.at(m).at(0) + 1);
						/*double c = start_cx_index.at(m).at(un1index.at(m).at(1));*/
						cx_index_e.at(m).push_back(a);
						cx_index_e.at(m).push_back(b);

						end_cx_index.clear();
						cx_index_cz_2.clear();
						un1index_2.clear();

						j = cx_index_e.at(m).size();
						n = cx_index_e.at(m).at(j - 1);
					}
				}
			}
		}
	}

	for (int m = 0; m != 8; m++)
	{
		reverse(value_save.at(m).begin(), value_save.at(m).end());  //�ߵ�����
	}
	for (int m = 0; m != 8; m++)
	{
		for (auto it = cx_index_e.at(m).begin(); it != cx_index_e.at(m).end(); it++)
		{
			*it = value_save.at(m).size() - *it;
		}
	}
	for (int m = 0; m != 8; m++)
	{
		reverse(cx_index_e.at(m).begin(), cx_index_e.at(m).end());  //�ߵ�����
	}

	//******************************************************8������**********************************
	vector<vector<vector<double>>>mg_qx(motor_num, vector<vector<double>>(80));//��϶
	vector<vector<vector<double>>>mg_cx(motor_num, vector<vector<double>>(79));//��Ш

	for (int m = 0; m != 8; m++)
	{
		for (auto it = value_save.at(m).begin(); it != value_save.at(m).end(); it++)
		{
			*it = (*it - C1[3])*cos_value;
		}
	}

	for (int m = 0; m != 8; m++)
	{
		for (int n = 0; n != 40; n++)
		{
			int a = cx_index_s.at(m).at(2 * n);
			int b = cx_index_s.at(m).at(2 * n + 1);
			mg_cx.at(m).at(n).assign(value_save.at(m).begin() + a, value_save.at(m).begin() + b);
		}
		for (int n = 40; n != 79; n++)
		{
			int a = cx_index_e.at(m).at(2 * (n - 40) + 3);
			int b = cx_index_e.at(m).at(2 * (n - 40) + 4);
			mg_cx.at(m).at(n).assign(value_save.at(m).begin() + a, value_save.at(m).begin() + b);
		}
	}

	for (int m = 0; m != 8; m++)
	{
		mg_qx.at(m).at(0).assign(value_save.at(m).begin() + cx_index_s.at(m).at(0) - 20, value_save.at(m).begin() + cx_index_s.at(m).at(0));
		for (int n = 1; n != 41; n++)
		{
			int a = cx_index_s.at(m).at(2 * n - 1);
			int b = cx_index_s.at(m).at(2 * n);
			mg_qx.at(m).at(n).assign(value_save.at(m).begin() + a, value_save.at(m).begin() + b);
		}
		for (int n = 41; n != 79; n++)
		{
			int a = cx_index_e.at(m).at(2 * (n - 41) + 4);
			int b = cx_index_e.at(m).at(2 * (n - 41) + 5);
			mg_qx.at(m).at(n).assign(value_save.at(m).begin() + a, value_save.at(m).begin() + b);
		}
		mg_qx.at(m).at(79).assign(value_save.at(m).begin() + cx_index_e.at(m).at(80), value_save.at(m).begin() + cx_index_e.at(m).at(80) + 20);
	}

	//**************************************�ٶȼ���**********************
	vector<long>collect_points;
	for (int jj = 0; jj != motor_num; jj++)
	{
		collect_points.push_back(value_save_speed.at(jj).size());
	}
	const double collect_freq = 20000;
	const double motor_leng = 2.52;
	for (int jj = 0; jj != motor_num; jj++)
	{
		motor_speed.push_back((motor_leng / (collect_points.at(jj))) * collect_freq * 3.6);//ÿ��������ٶ�(Km/h) = 18144 /(ÿ������ĵ���-1)
	}

	//***********************************************************����Ԥ����*******************************************************//
	//vector<vector<vector<double>>>mg_qx_cz(motor_num, vector<vector<double>>(80));//��϶�Ĳ�ֵ
	//vector<vector<vector<double>>>mg_cx_cz(motor_num, vector<vector<double>>(79));//��Ш�Ĳ�ֵ
	//vector<vector<vector<double>>>qx_point(motor_num, vector<vector<double>>(80)); //ÿһ����Ľڵ�
	//vector<vector<vector<double>>>cx_point(motor_num, vector<vector<double>>(80)); //ÿһ����Ľڵ�
	//vector<vector<vector<double>>>qx_point_num(motor_num, vector<vector<double>>(80));//ÿһ���������
	//vector<vector<vector<double>>>cx_point_num(motor_num, vector<vector<double>>(80));//ÿһ���������
	//vector<vector<vector<double>>>mg_qx_2(motor_num, vector<vector<double>>(80));//��������������һ��
	//vector<vector<vector<double>>>mg_qx_mid(motor_num, vector<vector<double>>(80));//������������һ���ƽ��ֵ��
	//vector<vector<vector<double>>>mg_cx_2(motor_num, vector<vector<double>>(80));//��������������һ��
	//vector<vector<vector<double>>>mg_cx_mid(motor_num, vector<vector<double>>(80));//������������һ���ƽ��ֵ��


	//for (int m = 0; m <= 7; m++)  //�����ֵ
	//{
	//	for (int n = 0; n <= 79; n++)
	//	{
	//		for (auto it = mg_qx.at(m).at(n).begin(); it != (mg_qx.at(m).at(n).end() - 1); it++)
	//		{
	//			double num = abs(*(it + 1) - *it);
	//			mg_qx_cz.at(m).at(n).push_back(num);
	//		}
	//	}
	//	for (int n = 0; n <= 78; n++)
	//	{
	//		for (auto it = mg_cx.at(m).at(n).begin(); it != (mg_cx.at(m).at(n).end() - 1); it++)
	//		{
	//			double num = abs(*(it + 1) - *it);
	//			mg_cx_cz.at(m).at(n).push_back(num);
	//		}
	//	}
	//}

	//for (int m = 0; m <= 7; m++)
	//{
	//	for (int n = 0; n <= 79; n++)
	//	{
	//		for (auto it = mg_qx_cz.at(m).at(n).begin(); it != mg_qx_cz.at(m).at(n).end(); it++) //�����ֵ����0.1���Ͱ�����ֵ�������ڵ�
	//		{
	//			int num = it - mg_qx_cz.at(m).at(n).begin();
	//			if (*it > 0.1)
	//				qx_point.at(m).at(n).push_back(num + 1);
	//		}
	//		if (qx_point.at(m).at(n).size()) //�ж��Ƿ���ڷ���ڵ�
	//		{
	//			if (qx_point.at(m).at(n).size() > 1) //���࣬ÿһ�����Ŀ��������
	//			{
	//				qx_point_num.at(m).at(n).push_back(qx_point.at(m).at(n).at(0));
	//				for (auto it = qx_point.at(m).at(n).begin(); it != qx_point.at(m).at(n).end() - 1; it++)
	//				{
	//					qx_point_num.at(m).at(n).push_back(*(it + 1) - *it);
	//				}
	//				qx_point_num.at(m).at(n).push_back(mg_qx_cz.at(m).at(n).size() + 1 - *(qx_point.at(m).at(n).end() - 1));
	//			}
	//			else
	//			{
	//				qx_point_num.at(m).at(n).push_back(qx_point.at(m).at(n).at(0));
	//				qx_point_num.at(m).at(n).push_back(mg_qx_cz.at(m).at(n).size() + 1 - *(qx_point.at(m).at(n).end() - 1));
	//			}
	//			int sum = 0;
	//			int sum_2 = 0;
	//			double sum_3 = 0;
	//			auto max_qx_it = max_element(qx_point_num.at(m).at(n).begin(), qx_point_num.at(m).at(n).end());//Ѱ������������һ��
	//			for (auto it = qx_point_num.at(m).at(n).begin(); it <= max_qx_it; it++) // ��������������һ��֮ǰ�ж�������
	//			{
	//				sum_2 = sum_2 + *it;
	//			}
	//			mg_qx_2.at(m).at(n).assign(mg_qx.at(m).at(n).begin() + sum_2 - *max_qx_it, mg_qx.at(m).at(n).begin() + sum_2); //������������һ�ཻ��
	//			for (auto it = mg_qx_2.at(m).at(n).begin(); it != mg_qx_2.at(m).at(n).end(); it++) //��������������һ��ĺ�
	//			{
	//				double a = *it;
	//				sum_3 = sum_3 + a;
	//			}
	//			mg_qx_mid.at(m).at(n).push_back(sum_3 / mg_qx_2.at(m).at(n).size()); //��ƽ��ֵ��������
	//			for (auto it = qx_point_num.at(m).at(n).begin(); it != qx_point_num.at(m).at(n).end(); it++)
	//			{
	//				sum = sum + *it;
	//				if (*it < 5)
	//				{
	//					if (*it == 1)
	//					{
	//						mg_qx.at(m).at(n).at(sum - *it) = mg_qx_mid.at(m).at(n).at(0);
	//					}
	//					else
	//						for (int j = sum - *it; j != sum; j++)
	//						{
	//							mg_qx.at(m).at(n).at(j) = mg_qx_mid.at(m).at(n).at(0);
	//						}
	//				}
	//			}
	//		}

	//	}
	//}
	////��Ш
	//for (int m = 0; m <= 7; m++)
	//{
	//	for (int n = 0; n <= 78; n++)
	//	{
	//		for (auto it = mg_cx_cz.at(m).at(n).begin(); it != mg_cx_cz.at(m).at(n).end(); it++) //�����ֵ����0.1���Ͱ�����ֵ�������ڵ�
	//		{
	//			int num = it - mg_cx_cz.at(m).at(n).begin();
	//			if (*it > 0.1)
	//				cx_point.at(m).at(n).push_back(num + 1);
	//		}
	//		if (cx_point.at(m).at(n).size()) //�ж��Ƿ���ڷ���ڵ�
	//		{
	//			if (cx_point.at(m).at(n).size() > 1) //���࣬ÿһ�����Ŀ��������
	//			{
	//				cx_point_num.at(m).at(n).push_back(cx_point.at(m).at(n).at(0));
	//				for (auto it = cx_point.at(m).at(n).begin(); it != cx_point.at(m).at(n).end() - 1; it++)
	//				{
	//					cx_point_num.at(m).at(n).push_back(*(it + 1) - *it);
	//				}
	//				cx_point_num.at(m).at(n).push_back(mg_cx_cz.at(m).at(n).size() + 1 - *(cx_point.at(m).at(n).end() - 1));
	//			}
	//			else
	//			{
	//				cx_point_num.at(m).at(n).push_back(cx_point.at(m).at(n).at(0));
	//				cx_point_num.at(m).at(n).push_back(mg_cx_cz.at(m).at(n).size() + 1 - *(cx_point.at(m).at(n).end() - 1));
	//			}
	//			int sum = 0;
	//			int sum_2 = 0;
	//			double sum_3 = 0;
	//			auto max_cx_it = max_element(cx_point_num.at(m).at(n).begin(), cx_point_num.at(m).at(n).end());//Ѱ������������һ��
	//			for (auto it = cx_point_num.at(m).at(n).begin(); it <= max_cx_it; it++) // ��������������һ��֮ǰ�ж�������
	//			{
	//				sum_2 = sum_2 + *it;
	//			}
	//			mg_cx_2.at(m).at(n).assign(mg_cx.at(m).at(n).begin() + sum_2 - *max_cx_it, mg_cx.at(m).at(n).begin() + sum_2); //������������һ�ཻ��
	//			for (auto it = mg_cx_2.at(m).at(n).begin(); it != mg_cx_2.at(m).at(n).end(); it++) //��������������һ��ĺ�
	//			{
	//				double a = *it;
	//				sum_3 = sum_3 + a;
	//			}
	//			mg_cx_mid.at(m).at(n).push_back(sum_3 / mg_cx_2.at(m).at(n).size()); //��ƽ��ֵ��������
	//			for (auto it = cx_point_num.at(m).at(n).begin(); it != cx_point_num.at(m).at(n).end(); it++)
	//			{
	//				sum = sum + *it;
	//				if (*it < 5)
	//				{
	//					if (*it == 1)
	//					{
	//						mg_cx.at(m).at(n).at(sum - *it) = mg_cx_mid.at(m).at(n).at(0);
	//					}
	//					else
	//						for (int j = sum - *it; j != sum; j++)
	//						{
	//							mg_cx.at(m).at(n).at(j) = mg_cx_mid.at(m).at(n).at(0);
	//						}
	//				}
	//			}
	//		}

	//	}
	//}
	//********************************************************************������ֵ**************************************************
	vector<vector<vector<double>>>qx_2(motor_num, vector<vector<double>>(80));//��϶�ı���
	vector<vector<vector<double>>>cx_2(motor_num, vector<vector<double>>(80));//��϶�ı���
	vector<vector<double>>qx_mid(8, vector<double>());//��϶��ֵ
	vector<vector<double>>cx_mid(8, vector<double>());//��Ш��ֵ
	for (int m = 0; m != motor_num; m++)
	{
		for (int n = 0; n <= 79; n++)
		{
			auto num = mg_qx.at(m).at(n).size();
			qx_2.at(m).at(n).assign(mg_qx.at(m).at(n).begin(), mg_qx.at(m).at(n).end());
			sort(qx_2.at(m).at(n).begin(), qx_2.at(m).at(n).end());
			if (num % 2)
			{
				auto mid = (num + 1) / 2;
				auto g = qx_2.at(m).at(n).at(mid);
				qx_mid.at(m).push_back(g);
			}
			else
			{
				auto mid1 = qx_2.at(m).at(n).at(num / 2);
				auto mid2 = qx_2.at(m).at(n).at(num / 2 + 1);
				auto mid = (mid1 + mid2) / 2;
				qx_mid.at(m).push_back(mid);
			}
		}
		for (int n = 0; n <= 78; n++)
		{
			auto num = mg_cx.at(m).at(n).size();
			cx_2.at(m).at(n).assign(mg_cx.at(m).at(n).begin(), mg_cx.at(m).at(n).end());
			sort(cx_2.at(m).at(n).begin(), cx_2.at(m).at(n).end());
			if (num % 2)
			{
				auto mid = (num + 1) / 2;
				auto g = cx_2.at(m).at(n).at(mid);
				cx_mid.at(m).push_back(g);
			}
			else
			{
				auto mid1 = cx_2.at(m).at(n).at(num / 2);
				auto mid2 = cx_2.at(m).at(n).at(num / 2 + 1);
				auto mid = (mid1 + mid2) / 2;
				cx_mid.at(m).push_back(mid);
			}
		}
	}
	/****************************************************************************��Ҫ������****************************************************************************/
	mgmin_qx.resize(8);
	mgmax_cx.resize(8);
	cs.resize(8);
	
	for (int jj = 0; jj != motor_num; jj++)
	{
		//ÿ���ݵ���϶���ֵ,��Сֵ,ƽ��ֵ
		if (jj % 2)
		{
			for (int kk = 79; kk >= 0; kk--)
			{
				auto iter19 = min_element(begin(mg_qx.at(jj).at(kk)), end(mg_qx.at(jj).at(kk)));
				mgmin_qx.at(jj).push_back(*iter19);//��ά����mgmin_qx,ÿ�д洢һ���������϶��Сֵ(ÿ��80��)(ĳһ�γ��ֵ���Сֵ)
			}
			//ÿ���ݵĲ�Ш���ֵ,��Сֵ,ƽ��ֵ
			for (int kk = 78; kk >= 0; kk--)
			{
				auto iter21 = max_element(begin(mg_cx.at(jj).at(kk)), end(mg_cx.at(jj).at(kk)));
				mgmax_cx.at(jj).push_back(*iter21);//��ά����mgmax_cx,ÿ�д洢һ������Ĳ�Ш���ֵ(ÿ��79��)(ĳһ�γ��ֵ����ֵ)
			}
			for (int kk = 78; kk >= 0; kk--)
			{
				double num = mgmax_cx.at(jj).at(78 - kk) - mgmin_qx.at(jj).at(78 - kk);
				if (kk == 78 && num <= 1.3)
					num = num + 2;
				cs.at(jj).push_back(num );//��ά����cs,�洢ÿ�����ÿ���ݵĲ���
			}
			double iter44 = accumulate(cs.at(jj).begin(), cs.at(jj).end(), 0.0);
			csmean.push_back(iter44 / (cs.at(jj).size()));//ÿ���������ƽ��ֵ(8��)
		}
		else
		{
			for (int kk = 0; kk <= 79; kk++)
			{
				auto iter19 = min_element(begin(mg_qx.at(jj).at(kk)), end(mg_qx.at(jj).at(kk)));
				mgmin_qx.at(jj).push_back(*iter19);//��ά����mgmin_qx,ÿ�д洢һ���������϶��Сֵ(ÿ��80��)(ĳһ�γ��ֵ���Сֵ)
			}
			//ÿ���ݵĲ�Ш���ֵ,��Сֵ,ƽ��ֵ
			for (int kk = 0; kk <= 78; kk++)
			{
				auto iter21 = max_element(begin(mg_cx.at(jj).at(kk)), end(mg_cx.at(jj).at(kk)));
				mgmax_cx.at(jj).push_back(*iter21);//��ά����mgmax_cx,ÿ�д洢һ������Ĳ�Ш���ֵ(ÿ��79��)(ĳһ�γ��ֵ����ֵ)
			}

			for (int kk = 0; kk != 79; kk++)
			{
				double num = mgmax_cx.at(jj).at(kk) - mgmin_qx.at(jj).at(kk);
				if (kk == 0 && num <= 1.3)
					num = num + 2;
				cs.at(jj).push_back(num );//��ά����cs,�洢ÿ�����ÿ���ݵĲ���
			}
			double iter44 = accumulate(cs.at(jj).begin(), cs.at(jj).end(), 0.0);
			csmean.push_back(iter44 / (cs.at(jj).size()));//ÿ���������ƽ��ֵ(8��)
		}
	}

	for (int jj = 0; jj != motor_num; jj++)
	{
		//�������϶���ֵ,��Сֵ,ƽ��ֵ
		auto iter24 = max_element(mgmin_qx.at(jj).begin(), mgmin_qx.at(jj).end());
		qxmax.push_back(*iter24);//һά����qxmax,�洢���е����϶�����ֵ(8��)(ĳһ�γ��ֵ����ֵ)
		qmax_gid.push_back(iter24 - mgmin_qx.at(jj).begin());//�洢8�������϶���ֵ��Ӧ�ĳݵı��(8��)
		auto iter25 = min_element(mgmin_qx.at(jj).begin(), mgmin_qx.at(jj).end());
		qxmin.push_back(*iter25);//һά����qxmin,�洢���е����϶����Сֵ(8��)(ĳһ�γ��ֵ���Сֵ)
		qmin_gid.push_back(iter25 - mgmin_qx.at(jj).begin());//�洢8�������϶��Сֵ��Ӧ�ĳݵı��(8��)
		double iter26 = accumulate(mgmin_qx.at(jj).begin(), mgmin_qx.at(jj).end(), 0.0);
		qxmean.push_back(iter26 / mgmin_qx.at(jj).size());//һά����qxmean,�洢���е����϶��ƽ��ֵ(8��)

		//����Ĳ�Ш���ֵ,��Сֵ,ƽ��ֵ
		auto iter29 = max_element(mgmax_cx.at(jj).begin(), mgmax_cx.at(jj).end());
		cxmax.push_back(*iter29);//һά����cxmax,�洢���е����Ш�����ֵ(8��)(ĳһ�γ��ֵ����ֵ)
		cmax_gid.push_back(iter29 - mgmax_cx.at(jj).begin());//�洢8�������Ш���ֵ��Ӧ�ĳݵı��(8��)
		auto iter30 = min_element(mgmax_cx.at(jj).begin(), mgmax_cx.at(jj).end());
		cxmin.push_back(*iter30);//һά����cxmin,�洢���е����Ш����Сֵ(8��)(ĳһ�γ��ֵ���Сֵ)
		cmin_gid.push_back(iter30 - mgmax_cx.at(jj).begin());//�洢8�������Ш��Сֵ��Ӧ�ĳݵı��(8��)
		double iter31 = accumulate(mgmax_cx.at(jj).begin(), mgmax_cx.at(jj).end(), 0.0);
		cxmean.push_back(iter31 / mgmax_cx.at(jj).size());//һά����cxmean,�洢���е����Ш��ƽ��ֵ(8��)
	}

	auto iter34 = max_element(begin(qxmin), end(qxmin));
	max_qx = *iter34;//����max_qx,�洢һ�г�����϶���ֵ(1��)(ĳһ�γ��ֵ����ֵ)
	qmax_mid = iter34 - begin(qxmin);//����qmax_mid,�洢��϶���ֵ��Ӧ�ĵ���ı��(1��)
	auto iter35 = min_element(begin(qxmin), end(qxmin));
	min_qx = *iter35;//����min_qx,�洢һ�г�����϶��Сֵ(1��)(ĳһ�γ��ֵ���Сֵ)
	qmin_mid = iter35 - begin(qxmin);//����qmin_mid,�洢��϶��Сֵ��Ӧ�ĵ���ı��(1��)
	double iter36 = accumulate(begin(qxmean), end(qxmean), 0.0);
	mean_qx = (iter36 / (qxmean.size()));//����mean_qx,�洢һ�г�����϶ƽ��ֵ(1��)

	auto iter37 = max_element(begin(cxmax), end(cxmax));
	max_cx = *iter37;//����max_cx,�洢һ�г��Ĳ�Ш���ֵ(1��)(ĳһ�γ��ֵ����ֵ)
	cmax_mid = iter37 - begin(cxmax);//����cmax_mid,�洢��Ш���ֵ��Ӧ�ĵ���ı��(1��)
	auto iter38 = min_element(begin(cxmax), end(cxmax));
	min_cx = *iter38;//����min_cx,�洢һ�г��Ĳ�Ш��Сֵ(1��)(ĳһ�γ��ֵ���Сֵ)
	cmin_mid = iter38 - begin(cxmax);//����cmin_mid,�洢��Ш��Сֵ��Ӧ�ĵ���ı��(1��)
	double iter39 = accumulate(begin(cxmean), end(cxmean), 0.0);
	mean_cx = (iter39 / (cxmean.size()));//����mean_cx,�洢һ�г��Ĳ�Шƽ��ֵ(1��)

	for (int jj = 0; jj != motor_num; jj++)
	{
		mg3min_qx.push_back(mgmin_qx.at(jj).at(2));//һά����mg3min_qx,�洢��3���ݵ���϶��Сֵ(8��)
		mg78min_qx.push_back(mgmin_qx.at(jj).at(77));//һά����mg78min_qx,�洢��78���ݵ���϶��Сֵ(8��)
	}

	auto iter42 = min_element(begin(mg3min_qx), end(mg3min_qx));
	qmin_3id = iter42 - begin(mg3min_qx);//����qmin_3id,�洢8������е�3���ݵ���϶��Сֵ��Ӧ�ĵ�����(1��)

	auto iter43 = min_element(begin(mg78min_qx), end(mg78min_qx));
	qmin_78id = iter43 - begin(mg78min_qx);//����qmin_78id,�洢8������е�78���ݵ���϶��Сֵ��Ӧ�ĵ�����(1��)


	for (int jj = 0; jj != motor_num; jj++)
	{
		auto iter45 = min_element(cs.at(jj).begin(), cs.at(jj).end());
		cs_min.push_back(*iter45);//һά����cs_min;�洢8�������Ӧ�ĳݲ������Сֵ(8��)
		csmin_gid.push_back(iter45 - cs.at(jj).begin());//һά����csmin_gid;�洢8������ݲ�����Сֵ��Ӧ�ĳݱ��(8��)
		auto iter46 = max_element(cs.at(jj).begin(), cs.at(jj).end());
		cs_max.push_back(*iter46);//һά����cs_max;�洢8�������Ӧ�ĳݲ�������ֵ(8��)
		csmax_gid.push_back(iter46 - cs.at(jj).begin());//һά����csmax_gid;�洢8������ݲ������ֵ��Ӧ�ĳݱ��(8��)
	}

	auto iter47 = min_element(begin(cs_min), end(cs_min));
	min_cs = *iter47;//һά����min_cs;�洢һ�г��ݲ�����Сֵ(1��)
	min_csmid = iter47 - begin(cs_min);//һά����min_csid;�洢һ�г��ݲ�����С�ĵ�����(1��)

	auto iter48 = max_element(begin(cs_max), end(cs_max));
	max_cs = *iter48;//һά����max_cs;�洢һ�г��ݲ������ֵ(1��)
	max_csmid = iter48 - begin(cs_max);//һά����max_csid;�洢һ�г��ݲ������ĵ�����(1��)

	double iter49 = accumulate(csmean.begin(), csmean.end(), 0.0);
	mean_cs = iter49 / (csmean.size());//һ�г�����ƽ��ֵ(1��)

	vector<vector<double> >qx3_13(motor_num);
	vector<vector<double> >qx68_78(motor_num);
	for (int jj = 0; jj != motor_num; jj++)
	{
		for (int kk = 2; kk <= 12; kk++)
		{
			qx3_13.at(jj).push_back(mgmin_qx.at(jj).at(kk));
		}
		double iter50 = accumulate(qx3_13.at(jj).begin(), qx3_13.at(jj).end(), 0.0);
		qmean3_13.push_back(iter50 / 11);//һά����qmean3_13,�洢ÿ�������3������϶����13������϶(��11������϶)��ƽ��ֵ(8��)

		for (int kk = 67; kk <= 77; kk++)
		{
			qx68_78.at(jj).push_back(mgmin_qx.at(jj).at(kk));
		}
		double iter51 = accumulate(qx68_78.at(jj).begin(), qx68_78.at(jj).end(), 0.0);
		qmean68_78.push_back(iter51 / 11);//һά����qmean68_78,�洢ÿ�������68������϶����78������϶(��11������϶)��ƽ��ֵ(8��)

		if (qmean3_13.at(jj) > qmean68_78.at(jj))
		{
			max_qmean.push_back(qmean3_13.at(jj));//һά����max_qmean,�洢ǰ11������϶ƽ��ֵ���11������϶ƽ��ֵ�Ľϴ���(8��)
			min_qmean.push_back(qmean68_78.at(jj));//һά����min_qmean,�洢ǰ11������϶ƽ��ֵ���11������϶ƽ��ֵ�Ľ�С��(8��)
		}
		else
		{
			max_qmean.push_back(qmean68_78.at(jj));//һά����max_qmean,�洢ǰ11������϶ƽ��ֵ���11������϶ƽ��ֵ�Ľϴ���(8��)
			min_qmean.push_back(qmean3_13.at(jj));//һά����min_qmean,�洢ǰ11������϶ƽ��ֵ���11������϶ƽ��ֵ�Ľ�С��(8��)
		}
	}

	//*******************����������****************************
	vector<vector<double>> pilot_cz;
	pilot_cz.resize(8);
	vector<vector<double>> pilot_point;
	pilot_point.resize(8);
	vector<vector<double>> pilot_point_num;
	pilot_point_num.resize(8);
	vector<vector<double>> pilot_pjz;
	pilot_pjz.resize(8);

	for (int m = 0; m != 8; m++)
	{
		for (auto it = value_save_pilot.at(m).begin(); it != value_save_pilot.at(m).end()-1; it++)
		{
			pilot_cz.at(m).push_back(*(it + 1) - *it);
		}
		for (auto it = pilot_cz.at(m).begin(); it != pilot_cz.at(m).end(); it++)
		{
			if (*it != 0)
				pilot_point.at(m).push_back(it - pilot_cz.at(m).begin() + 1);
		}
		if (pilot_point.at(m).size() > 1)
		{
			pilot_point_num.at(m).push_back(pilot_point.at(m).at(0));
			for (auto it = pilot_point.at(m).begin(); it != pilot_point.at(m).end() - 1; it++)
			{
				pilot_point_num.at(m).push_back(*(it + 1) - *it);
			}
			pilot_point_num.at(m).push_back(pilot_cz.at(m).size() + 1 - *(pilot_point.at(m).end() - 1));
		}
		else
		{
			pilot_point_num.at(m).push_back(pilot_point.at(m).at(0));
			pilot_point_num.at(m).push_back(pilot_cz.at(m).size() + 1 - *(pilot_point.at(m).end() - 1));
		}
		double sum = 0;
		for (auto it = pilot_point_num.at(m).begin(); it != pilot_point_num.at(m).end(); it++)
		{
			double temp = accumulate(value_save_pilot.at(m).begin() + sum, value_save_pilot.at(m).begin() + sum + *it, 0.0);
			double pjz = temp / (*it);
			pilot_pjz.at(m).push_back(pjz);
			sum = sum + *it;
		}
		for (auto it = pilot_point_num.at(m).begin(); it != pilot_point_num.at(m).end(); it++)
		{
			int num = it - pilot_point_num.at(m).begin();
			if (*it < 8)
			{
				pilot_pjz.at(m).at(num) = -10;
			}
		}
		for (auto it = pilot_pjz.at(m).begin(); it != pilot_pjz.at(m).end();)
		{
			if (*it == -10)
				it = pilot_pjz.at(m).erase(it);
			else
				it++;
		}
		for (auto it = pilot_pjz.at(m).begin(); it != pilot_pjz.at(m).end(); it++)
		{
			*it = (*it - C1[3])*cos_value;
		}
		for (auto it = pilot_pjz.at(m).begin(); it != pilot_pjz.at(m).end();)
		{
			if (*it >=26 || *it <= 20)
				it = pilot_pjz.at(m).erase(it);
			else
				it++;
		}
		if (pilot_pjz.at(m).size())
		{
			auto min = min_element(pilot_pjz.at(m).begin(), pilot_pjz.at(m).end());
			pilot_value.push_back(*min);
		}
		else
		{
			pilot_value.push_back(20);
		}
	}
	auto p_min = min_element(pilot_value.begin(), pilot_value.end());
	pilot_min = *p_min;
	/*******************************************�¶ȴ���************************************************/

	SYSTEMTIME sys;
	GetLocalTime(&sys);
	auto h = sys.wHour;
	auto min = sys.wMinute;

	double t;
	if (h > 0 && h < 8)
	{
		t = 2.9;
	}
	if (h == 8)
	{
		t = 3.2;
	}
	if (h > 8 && h < 10)
	{
		t = 3.4;
	}
	if (h >=10 && h < 12)
	{
		t = 3.55;
	}
	if (h >=12 && h < 14)
	{
		t = 3.7;
	}
	if (h >=14 && h < 16)
	{
		t = 3.85;
	}
	if (h >= 16 && h < 18)
	{
		t = 4;
	}
	if (h >= 18 && h < 20)
	{
		t = 4.2;
	}
	if (h >= 20)
	{
		t = 4.3;
	}
	if (h == 0)
	{
		t = 4.4;
	}

	wd.resize(8);
	for (int m = 0; m != 8; m++)
	{
		wd.at(m).assign(mg_qx.at(m).at(min+1).begin(), mg_qx.at(m).at(min + 1).end());
	}
	for (int m = 6; m != 8; m++)
	{
		wd.at(m).assign(mg_qx.at(m).at(min + 2).begin(), mg_qx.at(m).at(min + 2).end());
	}
	for (int m = 2; m != 6; m++)
	{
		wd.at(m).assign(mg_cx.at(m).at(min + 3).begin(), mg_cx.at(m).at(min + 3).end());
	}
	for (int m = 0; m != 8; m++)
	{
		for (auto it = wd.at(m).begin(); it != wd.at(m).end(); it++)
		{
			if (min >= 30)
			{
				*it = *it*t + min * 0.4;
			}
			else
			{
				*it = *it*t + min * 0.6;
			}
		}
	}
	/**********************************************************�¶���Ҫ������********************************************************/
	if (Pflag == true)
	{
		for (int jj = 0; jj != 8; jj++)
		{
			auto t_iter27 = max_element(wd.at(jj).begin(), wd.at(jj).end());
			t_max.push_back(*t_iter27);
			auto t_iter28 = min_element(wd.at(jj).begin(), wd.at(jj).end());
			t_min.push_back(*t_iter28);
			double t_iter29 = accumulate(begin(wd.at(jj)), end(wd.at(jj)), 0.0);
			t_mean.push_back(t_iter29 / (wd.at(jj).size()));
		}

		auto t_iter30 = max_element(t_max.begin(), t_max.end());
		t_maxid = t_iter30 - t_max.begin();
		max_temp = *t_iter30;
		auto t_iter31 = min_element(t_min.begin(), t_min.end());
		t_minid = t_iter31 - t_min.begin();
		min_temp = *t_iter31;
		double t_iter32 = accumulate(begin(t_mean), end(t_mean), 0.0);
		mean_temp = (t_iter32 / (t_mean.size()));
	}
	else
	{
		for (int jj = 7; jj >= 0; jj--)
		{
			auto t_iter27 = max_element(wd.at(jj).begin(), wd.at(jj).end());
			t_max.push_back(*t_iter27);
			auto t_iter28 = min_element(wd.at(jj).begin(), wd.at(jj).end());
			t_min.push_back(*t_iter28);
			double t_iter29 = accumulate(begin(wd.at(jj)), end(wd.at(jj)), 0.0);
			t_mean.push_back(t_iter29 / (wd.at(jj).size()));
		}

		auto t_iter30 = max_element(t_max.begin(), t_max.end());
		t_maxid = t_iter30 - t_max.begin();
		max_temp = *t_iter30;
		auto t_iter31 = min_element(t_min.begin(), t_min.end());
		t_minid = t_iter31 - t_min.begin();
		min_temp = *t_iter31;
		double t_iter32 = accumulate(begin(t_mean), end(t_mean), 0.0);
		mean_temp = (t_iter32 / (t_mean.size()));
	}

	qDebug() << value_save.size();

}

