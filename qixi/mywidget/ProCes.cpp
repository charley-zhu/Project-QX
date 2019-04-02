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
	//2018 7/30 原始数据size判断
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
	
	int moutain_num = 0;//数据突出部分，按理来说是电机数
	vector<vector<double>> value_save_speed;//计算速度
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
	}//有待改进，分电机应
	int motor_num = moutain_num;
	for (int m = 0; m != 8; m++)
	{
		value_save_speed.at(m).assign(value_save.at(m).begin(), value_save.at(m).end());
	}

	//***********************提前预处理，去除明显的噪声点*****************************(这里上下行的阈值选取不一致）
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
	//**************************寻找每个电机齿的起始位置*******************************
	vector<vector<double>> motor_start_cz; //存放电机差值
	motor_start_cz.resize(8);
	vector<double>start_point;//存放初始端的最后一个气隙点
	vector<int>start_point_index;//存放初始端的开始位置索引
	vector<double>end_point;//存放末端的最后一个气隙点
	vector<int>end_point_index;//存放末端的起始点的索引值
	vector<vector<double>> value_save_start;

	value_save_start.assign(value_save.begin(), value_save.end()); //备份
	for (int m = 0; m != 8; m++) //去除每个电机的前40和后40个点（参数可调）
	{
		value_save_start.at(m).erase(value_save_start.at(m).begin(), value_save_start.at(m).begin() + 50);
		value_save_start.at(m).erase(value_save_start.at(m).end() - 51, value_save_start.at(m).end());
	}

	for (int m = 0; m != 8; m++) //计算差值
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

	for (int m = 0; m != 8; m++)  //差值参数选择0.3
	{
		for (auto it = motor_start_cz.at(m).begin(); it != motor_start_cz.at(m).end(); it++)
		{
			int num = it - motor_start_cz.at(m).begin();
			if (*it > 0.3)
				value_point_s.at(m).push_back(num + 1);
		}
	}

	for (int m = 0; m != 8; m++) //分类
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

	vector<vector<double>> value_pjz_s; //计算平均值
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
	vector<vector<double>> test1_index; //计算槽楔的索引值
	test1_index.resize(8);
	for (int m = 0; m != 8; m++)
	{
		for (auto it = test1.at(m).begin(); it != test1.at(m).end() - 1; it++)
		{
			if (*(it + 1) - *it == 0)
				test1_index.at(m).push_back(*(it + 1));
		}
	}

	for (int m = 0; m != 8; m++) //计算初始位置
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

	for (int m = 0; m != 8; m++) //计算终止位置
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
	//****************************************开始段****************************************开始段1-40，每2个比较一次
	vector<vector<double>> start_cx_index; //存放从开始位置开始的槽楔索引
	start_cx_index.resize(8);
	for (int m = 0; m != 8; m++)
	{
		double temp = start_point.at(m) + 0.5; // 取一个衡量值
		for (auto it = value_save.at(m).begin() + start_point_index.at(m) + 1; it != value_save.at(m).begin() + start_point_index.at(m) + 500; it++)
		{
			if (*it > temp) //如果大于这个衡量值
			{
				int index = it - value_save.at(m).begin();
				start_cx_index.at(m).push_back(index);//把索引值存入start_cx_index中
			}
		}
	}
	vector<vector<double>> cx_index_cz;//槽楔索引值的差值
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
	vector<vector<double>> un1index; //第一段非1索引
	un1index.resize(8);
	vector<vector<double>> cx_index_s;//第一段槽楔索引
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
	//*****************开始段的循环*********************
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

			double temp = start_point_2 + 0.5;// 取一个衡量值
			for (auto it = value_save.at(m).begin() + start_point_index_2 + 1; it != value_save.at(m).begin() + start_point_index_2 + 500; it++)
			{
				if (*it > temp) //如果大于这个衡量值
				{
					int index = it - value_save.at(m).begin();
					start_cx_index.at(m).push_back(index);//把索引值存入start_cx_index中
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
					if (*it > temp) //如果大于这个衡量值
					{
						int index = it - value_save.at(m).begin();
						start_cx_index.at(m).push_back(index);//把索引值存入start_cx_index中
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
						if (*it > temp) //如果大于这个衡量值
						{
							int index = it - value_save.at(m).begin();
							start_cx_index.at(m).push_back(index);//把索引值存入start_cx_index中
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
							if (*it > temp) //如果大于这个衡量值
							{
								int index = it - value_save.at(m).begin();
								start_cx_index.at(m).push_back(index);//把索引值存入start_cx_index中
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

	//*********************************从结束段开始*****************************
	for (int m = 0; m != 8; m++)
	{
		reverse(value_save.at(m).begin(), value_save.at(m).end());  //颠倒回来
	}

	vector<vector<double>> end_cx_index; //存放从开始位置开始的槽楔索引
	end_cx_index.resize(8);
	for (int m = 0; m != 8; m++)
	{
		double temp = end_point.at(m) + 0.5; // 取一个衡量值
		for (auto it = value_save.at(m).begin() + end_point_index.at(m) + 1; it != value_save.at(m).begin() + end_point_index.at(m) + 500; it++)
		{
			if (*it > temp) //如果大于这个衡量值
			{
				int index = it - value_save.at(m).begin();
				end_cx_index.at(m).push_back(index);//把索引值存入start_cx_index中
			}
		}
	}
	vector<vector<double>> cx_index_cz_2;//槽楔索引值的差值
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
	vector<vector<double>> un1index_2; //第一段非1索引
	un1index_2.resize(8);
	vector<vector<double>> cx_index_e;//第一段槽楔索引
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
	//*****************结束段的循环*********************
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

			double temp = end_point_2 + 0.5;// 取一个衡量值
			for (auto it = value_save.at(m).begin() + end_point_index_2 + 1; it != value_save.at(m).begin() + end_point_index_2 + 500; it++)
			{
				if (*it > temp) //如果大于这个衡量值
				{
					int index = it - value_save.at(m).begin();
					end_cx_index.at(m).push_back(index);//把索引值存入start_cx_index中
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
					if (*it > temp) //如果大于这个衡量值
					{
						int index = it - value_save.at(m).begin();
						end_cx_index.at(m).push_back(index);//把索引值存入start_cx_index中
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
						if (*it > temp) //如果大于这个衡量值
						{
							int index = it - value_save.at(m).begin();
							end_cx_index.at(m).push_back(index);//把索引值存入start_cx_index中
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
							if (*it > temp) //如果大于这个衡量值
							{
								int index = it - value_save.at(m).begin();
								end_cx_index.at(m).push_back(index);//把索引值存入start_cx_index中
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
		reverse(value_save.at(m).begin(), value_save.at(m).end());  //颠倒回来
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
		reverse(cx_index_e.at(m).begin(), cx_index_e.at(m).end());  //颠倒回来
	}

	//******************************************************8存数据**********************************
	vector<vector<vector<double>>>mg_qx(motor_num, vector<vector<double>>(80));//气隙
	vector<vector<vector<double>>>mg_cx(motor_num, vector<vector<double>>(79));//槽楔

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

	//**************************************速度计算**********************
	vector<long>collect_points;
	for (int jj = 0; jj != motor_num; jj++)
	{
		collect_points.push_back(value_save_speed.at(jj).size());
	}
	const double collect_freq = 20000;
	const double motor_leng = 2.52;
	for (int jj = 0; jj != motor_num; jj++)
	{
		motor_speed.push_back((motor_leng / (collect_points.at(jj))) * collect_freq * 3.6);//每个电机的速度(Km/h) = 18144 /(每个电机的点数-1)
	}

	//***********************************************************数据预处理*******************************************************//
	//vector<vector<vector<double>>>mg_qx_cz(motor_num, vector<vector<double>>(80));//气隙的差值
	//vector<vector<vector<double>>>mg_cx_cz(motor_num, vector<vector<double>>(79));//槽楔的差值
	//vector<vector<vector<double>>>qx_point(motor_num, vector<vector<double>>(80)); //每一个类的节点
	//vector<vector<vector<double>>>cx_point(motor_num, vector<vector<double>>(80)); //每一个类的节点
	//vector<vector<vector<double>>>qx_point_num(motor_num, vector<vector<double>>(80));//每一类的数据量
	//vector<vector<vector<double>>>cx_point_num(motor_num, vector<vector<double>>(80));//每一类的数据量
	//vector<vector<vector<double>>>mg_qx_2(motor_num, vector<vector<double>>(80));//存放最大数据量的一类
	//vector<vector<vector<double>>>mg_qx_mid(motor_num, vector<vector<double>>(80));//存放数据量最大一类的平均值；
	//vector<vector<vector<double>>>mg_cx_2(motor_num, vector<vector<double>>(80));//存放最大数据量的一类
	//vector<vector<vector<double>>>mg_cx_mid(motor_num, vector<vector<double>>(80));//存放数据量最大一类的平均值；


	//for (int m = 0; m <= 7; m++)  //计算差值
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
	//		for (auto it = mg_qx_cz.at(m).at(n).begin(); it != mg_qx_cz.at(m).at(n).end(); it++) //如果差值超过0.1，就把索引值存入分类节点
	//		{
	//			int num = it - mg_qx_cz.at(m).at(n).begin();
	//			if (*it > 0.1)
	//				qx_point.at(m).at(n).push_back(num + 1);
	//		}
	//		if (qx_point.at(m).at(n).size()) //判断是否存在分类节点
	//		{
	//			if (qx_point.at(m).at(n).size() > 1) //分类，每一类的数目存入容器
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
	//			auto max_qx_it = max_element(qx_point_num.at(m).at(n).begin(), qx_point_num.at(m).at(n).end());//寻找数据量最大的一类
	//			for (auto it = qx_point_num.at(m).at(n).begin(); it <= max_qx_it; it++) // 计算数据量最大的一类之前有多少数据
	//			{
	//				sum_2 = sum_2 + *it;
	//			}
	//			mg_qx_2.at(m).at(n).assign(mg_qx.at(m).at(n).begin() + sum_2 - *max_qx_it, mg_qx.at(m).at(n).begin() + sum_2); //把数据量最大的一类交换
	//			for (auto it = mg_qx_2.at(m).at(n).begin(); it != mg_qx_2.at(m).at(n).end(); it++) //计算数据量最大的一类的和
	//			{
	//				double a = *it;
	//				sum_3 = sum_3 + a;
	//			}
	//			mg_qx_mid.at(m).at(n).push_back(sum_3 / mg_qx_2.at(m).at(n).size()); //把平均值存入容器
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
	////槽楔
	//for (int m = 0; m <= 7; m++)
	//{
	//	for (int n = 0; n <= 78; n++)
	//	{
	//		for (auto it = mg_cx_cz.at(m).at(n).begin(); it != mg_cx_cz.at(m).at(n).end(); it++) //如果差值超过0.1，就把索引值存入分类节点
	//		{
	//			int num = it - mg_cx_cz.at(m).at(n).begin();
	//			if (*it > 0.1)
	//				cx_point.at(m).at(n).push_back(num + 1);
	//		}
	//		if (cx_point.at(m).at(n).size()) //判断是否存在分类节点
	//		{
	//			if (cx_point.at(m).at(n).size() > 1) //分类，每一类的数目存入容器
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
	//			auto max_cx_it = max_element(cx_point_num.at(m).at(n).begin(), cx_point_num.at(m).at(n).end());//寻找数据量最大的一类
	//			for (auto it = cx_point_num.at(m).at(n).begin(); it <= max_cx_it; it++) // 计算数据量最大的一类之前有多少数据
	//			{
	//				sum_2 = sum_2 + *it;
	//			}
	//			mg_cx_2.at(m).at(n).assign(mg_cx.at(m).at(n).begin() + sum_2 - *max_cx_it, mg_cx.at(m).at(n).begin() + sum_2); //把数据量最大的一类交换
	//			for (auto it = mg_cx_2.at(m).at(n).begin(); it != mg_cx_2.at(m).at(n).end(); it++) //计算数据量最大的一类的和
	//			{
	//				double a = *it;
	//				sum_3 = sum_3 + a;
	//			}
	//			mg_cx_mid.at(m).at(n).push_back(sum_3 / mg_cx_2.at(m).at(n).size()); //把平均值存入容器
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
	//********************************************************************计算中值**************************************************
	vector<vector<vector<double>>>qx_2(motor_num, vector<vector<double>>(80));//气隙的备份
	vector<vector<vector<double>>>cx_2(motor_num, vector<vector<double>>(80));//气隙的备份
	vector<vector<double>>qx_mid(8, vector<double>());//气隙中值
	vector<vector<double>>cx_mid(8, vector<double>());//槽楔中值
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
	/****************************************************************************需要的数据****************************************************************************/
	mgmin_qx.resize(8);
	mgmax_cx.resize(8);
	cs.resize(8);
	
	for (int jj = 0; jj != motor_num; jj++)
	{
		//每个齿的气隙最大值,最小值,平均值
		if (jj % 2)
		{
			for (int kk = 79; kk >= 0; kk--)
			{
				auto iter19 = min_element(begin(mg_qx.at(jj).at(kk)), end(mg_qx.at(jj).at(kk)));
				mgmin_qx.at(jj).push_back(*iter19);//二维容器mgmin_qx,每行存储一个电机的气隙最小值(每行80个)(某一次出现的最小值)
			}
			//每个齿的槽楔最大值,最小值,平均值
			for (int kk = 78; kk >= 0; kk--)
			{
				auto iter21 = max_element(begin(mg_cx.at(jj).at(kk)), end(mg_cx.at(jj).at(kk)));
				mgmax_cx.at(jj).push_back(*iter21);//二维容器mgmax_cx,每行存储一个电机的槽楔最大值(每行79个)(某一次出现的最大值)
			}
			for (int kk = 78; kk >= 0; kk--)
			{
				double num = mgmax_cx.at(jj).at(78 - kk) - mgmin_qx.at(jj).at(78 - kk);
				if (kk == 78 && num <= 1.3)
					num = num + 2;
				cs.at(jj).push_back(num );//二维容器cs,存储每个电机每个齿的槽深
			}
			double iter44 = accumulate(cs.at(jj).begin(), cs.at(jj).end(), 0.0);
			csmean.push_back(iter44 / (cs.at(jj).size()));//每个电机槽深平均值(8个)
		}
		else
		{
			for (int kk = 0; kk <= 79; kk++)
			{
				auto iter19 = min_element(begin(mg_qx.at(jj).at(kk)), end(mg_qx.at(jj).at(kk)));
				mgmin_qx.at(jj).push_back(*iter19);//二维容器mgmin_qx,每行存储一个电机的气隙最小值(每行80个)(某一次出现的最小值)
			}
			//每个齿的槽楔最大值,最小值,平均值
			for (int kk = 0; kk <= 78; kk++)
			{
				auto iter21 = max_element(begin(mg_cx.at(jj).at(kk)), end(mg_cx.at(jj).at(kk)));
				mgmax_cx.at(jj).push_back(*iter21);//二维容器mgmax_cx,每行存储一个电机的槽楔最大值(每行79个)(某一次出现的最大值)
			}

			for (int kk = 0; kk != 79; kk++)
			{
				double num = mgmax_cx.at(jj).at(kk) - mgmin_qx.at(jj).at(kk);
				if (kk == 0 && num <= 1.3)
					num = num + 2;
				cs.at(jj).push_back(num );//二维容器cs,存储每个电机每个齿的槽深
			}
			double iter44 = accumulate(cs.at(jj).begin(), cs.at(jj).end(), 0.0);
			csmean.push_back(iter44 / (cs.at(jj).size()));//每个电机槽深平均值(8个)
		}
	}

	for (int jj = 0; jj != motor_num; jj++)
	{
		//电机的气隙最大值,最小值,平均值
		auto iter24 = max_element(mgmin_qx.at(jj).begin(), mgmin_qx.at(jj).end());
		qxmax.push_back(*iter24);//一维容器qxmax,存储所有电机气隙的最大值(8个)(某一次出现的最大值)
		qmax_gid.push_back(iter24 - mgmin_qx.at(jj).begin());//存储8个电机气隙最大值对应的齿的编号(8个)
		auto iter25 = min_element(mgmin_qx.at(jj).begin(), mgmin_qx.at(jj).end());
		qxmin.push_back(*iter25);//一维容器qxmin,存储所有电机气隙的最小值(8个)(某一次出现的最小值)
		qmin_gid.push_back(iter25 - mgmin_qx.at(jj).begin());//存储8个电机气隙最小值对应的齿的编号(8个)
		double iter26 = accumulate(mgmin_qx.at(jj).begin(), mgmin_qx.at(jj).end(), 0.0);
		qxmean.push_back(iter26 / mgmin_qx.at(jj).size());//一维容器qxmean,存储所有电机气隙的平均值(8个)

		//电机的槽楔最大值,最小值,平均值
		auto iter29 = max_element(mgmax_cx.at(jj).begin(), mgmax_cx.at(jj).end());
		cxmax.push_back(*iter29);//一维容器cxmax,存储所有电机槽楔的最大值(8个)(某一次出现的最大值)
		cmax_gid.push_back(iter29 - mgmax_cx.at(jj).begin());//存储8个电机槽楔最大值对应的齿的编号(8个)
		auto iter30 = min_element(mgmax_cx.at(jj).begin(), mgmax_cx.at(jj).end());
		cxmin.push_back(*iter30);//一维容器cxmin,存储所有电机槽楔的最小值(8个)(某一次出现的最小值)
		cmin_gid.push_back(iter30 - mgmax_cx.at(jj).begin());//存储8个电机槽楔最小值对应的齿的编号(8个)
		double iter31 = accumulate(mgmax_cx.at(jj).begin(), mgmax_cx.at(jj).end(), 0.0);
		cxmean.push_back(iter31 / mgmax_cx.at(jj).size());//一维容器cxmean,存储所有电机槽楔的平均值(8个)
	}

	auto iter34 = max_element(begin(qxmin), end(qxmin));
	max_qx = *iter34;//变量max_qx,存储一列车的气隙最大值(1个)(某一次出现的最大值)
	qmax_mid = iter34 - begin(qxmin);//变量qmax_mid,存储气隙最大值对应的电机的编号(1个)
	auto iter35 = min_element(begin(qxmin), end(qxmin));
	min_qx = *iter35;//变量min_qx,存储一列车的气隙最小值(1个)(某一次出现的最小值)
	qmin_mid = iter35 - begin(qxmin);//变量qmin_mid,存储气隙最小值对应的电机的编号(1个)
	double iter36 = accumulate(begin(qxmean), end(qxmean), 0.0);
	mean_qx = (iter36 / (qxmean.size()));//变量mean_qx,存储一列车的气隙平均值(1个)

	auto iter37 = max_element(begin(cxmax), end(cxmax));
	max_cx = *iter37;//变量max_cx,存储一列车的槽楔最大值(1个)(某一次出现的最大值)
	cmax_mid = iter37 - begin(cxmax);//变量cmax_mid,存储槽楔最大值对应的电机的编号(1个)
	auto iter38 = min_element(begin(cxmax), end(cxmax));
	min_cx = *iter38;//变量min_cx,存储一列车的槽楔最小值(1个)(某一次出现的最小值)
	cmin_mid = iter38 - begin(cxmax);//变量cmin_mid,存储槽楔最小值对应的电机的编号(1个)
	double iter39 = accumulate(begin(cxmean), end(cxmean), 0.0);
	mean_cx = (iter39 / (cxmean.size()));//变量mean_cx,存储一列车的槽楔平均值(1个)

	for (int jj = 0; jj != motor_num; jj++)
	{
		mg3min_qx.push_back(mgmin_qx.at(jj).at(2));//一维容器mg3min_qx,存储第3个齿的气隙最小值(8个)
		mg78min_qx.push_back(mgmin_qx.at(jj).at(77));//一维容器mg78min_qx,存储第78个齿的气隙最小值(8个)
	}

	auto iter42 = min_element(begin(mg3min_qx), end(mg3min_qx));
	qmin_3id = iter42 - begin(mg3min_qx);//变量qmin_3id,存储8个电机中第3个齿的气隙最小值对应的电机编号(1个)

	auto iter43 = min_element(begin(mg78min_qx), end(mg78min_qx));
	qmin_78id = iter43 - begin(mg78min_qx);//变量qmin_78id,存储8个电机中第78个齿的气隙最小值对应的电机编号(1个)


	for (int jj = 0; jj != motor_num; jj++)
	{
		auto iter45 = min_element(cs.at(jj).begin(), cs.at(jj).end());
		cs_min.push_back(*iter45);//一维容器cs_min;存储8个电机对应的齿槽深的最小值(8个)
		csmin_gid.push_back(iter45 - cs.at(jj).begin());//一维容器csmin_gid;存储8个电机齿槽深最小值对应的齿编号(8个)
		auto iter46 = max_element(cs.at(jj).begin(), cs.at(jj).end());
		cs_max.push_back(*iter46);//一维容器cs_max;存储8个电机对应的齿槽深的最大值(8个)
		csmax_gid.push_back(iter46 - cs.at(jj).begin());//一维容器csmax_gid;存储8个电机齿槽深最大值对应的齿编号(8个)
	}

	auto iter47 = min_element(begin(cs_min), end(cs_min));
	min_cs = *iter47;//一维容器min_cs;存储一列车齿槽深最小值(1个)
	min_csmid = iter47 - begin(cs_min);//一维容器min_csid;存储一列车齿槽深最小的电机编号(1个)

	auto iter48 = max_element(begin(cs_max), end(cs_max));
	max_cs = *iter48;//一维容器max_cs;存储一列车齿槽深最大值(1个)
	max_csmid = iter48 - begin(cs_max);//一维容器max_csid;存储一列车齿槽深最大的电机编号(1个)

	double iter49 = accumulate(csmean.begin(), csmean.end(), 0.0);
	mean_cs = iter49 / (csmean.size());//一列车槽深平均值(1个)

	vector<vector<double> >qx3_13(motor_num);
	vector<vector<double> >qx68_78(motor_num);
	for (int jj = 0; jj != motor_num; jj++)
	{
		for (int kk = 2; kk <= 12; kk++)
		{
			qx3_13.at(jj).push_back(mgmin_qx.at(jj).at(kk));
		}
		double iter50 = accumulate(qx3_13.at(jj).begin(), qx3_13.at(jj).end(), 0.0);
		qmean3_13.push_back(iter50 / 11);//一维容器qmean3_13,存储每个电机第3个齿气隙到第13个齿气隙(共11个齿气隙)的平均值(8个)

		for (int kk = 67; kk <= 77; kk++)
		{
			qx68_78.at(jj).push_back(mgmin_qx.at(jj).at(kk));
		}
		double iter51 = accumulate(qx68_78.at(jj).begin(), qx68_78.at(jj).end(), 0.0);
		qmean68_78.push_back(iter51 / 11);//一维容器qmean68_78,存储每个电机第68个齿气隙到第78个齿气隙(共11个齿气隙)的平均值(8个)

		if (qmean3_13.at(jj) > qmean68_78.at(jj))
		{
			max_qmean.push_back(qmean3_13.at(jj));//一维容器max_qmean,存储前11个齿气隙平均值与后11个齿气隙平均值的较大者(8个)
			min_qmean.push_back(qmean68_78.at(jj));//一维容器min_qmean,存储前11个齿气隙平均值与后11个齿气隙平均值的较小者(8个)
		}
		else
		{
			max_qmean.push_back(qmean68_78.at(jj));//一维容器max_qmean,存储前11个齿气隙平均值与后11个齿气隙平均值的较大者(8个)
			min_qmean.push_back(qmean3_13.at(jj));//一维容器min_qmean,存储前11个齿气隙平均值与后11个齿气隙平均值的较小者(8个)
		}
	}

	//*******************排障器计算****************************
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
	/*******************************************温度处理************************************************/

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
	/**********************************************************温度需要的数据********************************************************/
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

