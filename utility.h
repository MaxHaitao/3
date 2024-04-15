#include <cmath>
#pragma  once


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

//���������������ֵ
inline int ComputeLight(WORD x)
{
	//light = (int)(x*3.3*913/4/8192);
	return (int)(x*3.3*913/4/8192);
}

//���������������ֵ
inline float ComputeCurrent(WORD x,WORD max,WORD zeo)
{
	return (float)((max-zeo)?(((float)10)/(max-zeo)*(x-zeo)):(float)x*10/8192);
}

//�����ѹ��������ֵ
inline float ComputeVoltage(WORD x,WORD max,WORD zeo,WORD min)
{
	if(x>=zeo)
		return (float)((max-zeo)?(((float)10)/(max-zeo)*(x-zeo)):(float)x*10/4096);
	else
		return (float)((zeo-min)?(((float)10)/(zeo-min)*(x-min)-10):(float)x*10/4096-10);
}

BYTE MyGetSum(BYTE *,BYTE );
void MsgErrStr(TCHAR *s,int Err=NULL);
