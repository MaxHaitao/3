
//#include <conio.h>
#include <cmath>
#include <string>

#pragma once



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
