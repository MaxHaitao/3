#include <cmath>
#pragma  once


//****************************************
// Method:    GetMaxMin
// Function :  选择算法从指定类型为DateType的数组中找出最大和最小值
// Returns:   void
// Parameter: [IN] SortType Array   待查找的数组
// Parameter: [IN] DWORD ArraySize    数组的大小
// Parameter: [OUT] SortType max   数组中的最大值
// Parameter: [OUT] SortType min	数组中的最小值
//****************************************
template<typename DateType>
inline void GetMaxMin(DateType* Array , DWORD ArraySize , DateType& max , DateType& min)
{
	DateType tmax = static_cast<DateType>(-99999999);
	DateType tmin = static_cast<DateType>(99999999);

	for(DWORD i = 0;i < ArraySize;i++)
	{
		if(Array[i] < tmin)
		{
			tmin = Array[i];
		}
		if(Array[i] > tmax)
		{
			tmax = Array[i];
		}
	}
	max = tmax;
	min = tmin;
}

//计算光敏传感器数值
inline int ComputeLight(WORD x)
{
	//light = (int)(x*3.3*913/4/8192);
	return (int)(x*3.3*913/4/8192);
}

//计算电流传感器数值
inline float ComputeCurrent(WORD x,WORD max,WORD zeo)
{
	return (float)((max-zeo)?(((float)10)/(max-zeo)*(x-zeo)):(float)x*10/8192);
}

//计算电压传感器数值
inline float ComputeVoltage(WORD x,WORD max,WORD zeo,WORD min)
{
	if(x>=zeo)
		return (float)((max-zeo)?(((float)10)/(max-zeo)*(x-zeo)):(float)x*10/4096);
	else
		return (float)((zeo-min)?(((float)10)/(zeo-min)*(x-min)-10):(float)x*10/4096-10);
}

BYTE MyGetSum(BYTE *,BYTE );
void MsgErrStr(TCHAR *s,int Err=NULL);
