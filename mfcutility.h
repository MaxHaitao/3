
//#include <conio.h>
#include <cmath>
#include <string>

#pragma once



//****************************************
// Method:    GetMaxMin
// Function :  ѡ���㷨��ָ������ΪDateType���������ҳ�������Сֵ
// Returns:   void
// Parameter: [IN] SortType Array   �����ҵ�����
// Parameter: [IN] DWORD ArraySize    ����Ĵ�С
// Parameter: [OUT] SortType max   �����е����ֵ
// Parameter: [OUT] SortType min	�����е���Сֵ
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
