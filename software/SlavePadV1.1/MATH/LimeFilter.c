#include "LimeFilter.h"


float middleFilter_Cal(middleFilter_t* fil, float in_data)
{
	float sum = 0;
	float temp[MIDDLE_DATA_MAX];
	float change;
	uint8_t i = 0,j = 0;
	for(i=0; i<MIDDLE_DATA_MAX - 1; i++)          //��������λ
	{
		fil->data[i]=fil->data[i+1];
	}
	fil->data[MIDDLE_DATA_MAX - 1] = in_data;     //����������
	for(i=0; i<MIDDLE_DATA_MAX; i++)              //��������
		temp[i] = fil->data[i];
	for(i=1; i<MIDDLE_DATA_MAX; i++)              //ð�ݷ�����
		for(j=0; j<MIDDLE_DATA_MAX-i; j++)
		{
			if(temp[j] > temp[j+1])                   //����˳��
			{
				change = temp[j];
				temp[j] = temp[j+1];
				temp[j+1] = change;
			}
		}
	for(i=1; i<MIDDLE_DATA_MAX-1; i++)            //��ͷȥβ���
		sum = sum + temp[i];
    fil->result = sum/(MIDDLE_DATA_MAX - 2);    //�����м�ƽ��ֵ
	return(fil->result);
}


void LowPassFilter_Init(LowPassFilter_t * initPin,float k)
{
  if(k > 1 || k < 0)k = 0.5;
  initPin->k_History = k;
  initPin->input = 0;
  initPin->result = 0;
}
float LowPassFilter_Cal(LowPassFilter_t * FilterPin, float input)
{
  FilterPin->input = input;
  FilterPin->result = FilterPin->k_History * (FilterPin->result) + 
                      (1 - FilterPin->k_History) * FilterPin->input;
  return FilterPin->result;
}

