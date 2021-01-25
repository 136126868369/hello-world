/*
 * Spi.c
 *
 *  Created on: 2015-11-30
 *      Author: rd49
 */
#include "DSP28335_Spi.h"
#include "ScreenApi.h"
//#define SPISIMOA	GpioDataRegs.GPBDAT.bit.GPIO54
//#define SPISOMIA	GpioDataRegs.GPBDAT.bit.GPIO55
//#define SPICLKA	GpioDataRegs.GPBDAT.bit.GPIO56
#define SPITEA		GpioDataRegs.GPBDAT.bit.GPIO57
//#define SPIWAIT	delay_1ms(50)

u8 SpiSendReadOneByte(u8 byte)
{
	u16 t;
	t=0;
	while(SpiaRegs.SPISTS.bit.BUFFULL_FLAG == 1)//�ȴ�����׼������
	{
		if(t == 2000)
		{
			t = 0;
			break;
		}
		t++;
	}
	SpiaRegs.SPITXBUF = byte<<8;
	while(SpiaRegs.SPISTS.bit.INT_FLAG == 0)
	{
		if(t == 2000)
		{
			t = 0;
			break;
		}
		t++;
	}
	return SpiaRegs.SPIRXBUF & 0x00ff;
}
/***************************************************
 * ���ܣ���������
 * ������ָ�������ָ�룬����u8������5
 * ����ֵ��NONE
 */
void SpiSendCmd(u8* cmd)
{
	u16 i;
	u8 sent_cmd[CMD_LEN];
	SPITEA = 0;
	sent_cmd[CMD_LEN-1] = 0;
	for(i=0;i<CMD_LEN-1;i++)
	{
		sent_cmd[i]  = cmd[i];
		sent_cmd[CMD_LEN-1] += cmd[i];//У��λ����
	}

	for(i=0;i<CMD_LEN;i++)
	{
		SpiSendReadOneByte(sent_cmd[i]);
	}
	SPITEA = 1;
}

/***************************************************
 * ���ܣ���������
 * ������ָ�������ָ�룬����u8������5
 * ����ֵ��0->��ȡ�ɹ�	1->��ȡʧ�ܣ�У�����
 */
u8 SpiReceiveCmd(u8* cmd)
{
	u16 i;
	u8 crc8=0,cmd_rx[CMD_LEN];
	SPITEA = 0;
	for(i=0;i<CMD_LEN;i++)
	{
		cmd_rx[i] = SpiSendReadOneByte(0xff);
	}
	for(i=0;i<CMD_LEN-1;i++)
	{
		cmd[i] = cmd_rx[i];
		crc8 += cmd_rx[i];
	}
	SPITEA = 1;
	if((crc8&0xff) == cmd_rx[CMD_LEN-1])//crc8&0xff����Ҫ��&0xff
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

/***************************************************
 * ���ܣ���������
 * ������len:���ݳ���	data:ָ�����ݵ�ָ��
 * 		 mode:0->8bit����	1->16bit����
 * ����ֵ��0->���ճɹ� 	1->����ʧ�ܣ�У�����
 */
u8 SpiReceiveData(u32 len,u8* data,u8 mode)
{
	u32 i,rx_crc32=0,cul_crc32=0;
	u8 tmp[4],tmp_data=0;
	if(len == 0) return 0;
	SPITEA = 0;
	for(i=0;i<len;i++)//��������
	{
		data[i] = SpiSendReadOneByte(0xff);
		if(mode)
		{
			tmp_data = SpiSendReadOneByte(0xff);
			data[i] |= (u16)tmp_data<<8;
			cul_crc32 += tmp_data;
		}
		cul_crc32 += data[i]&0x00ff;
	}
	delay_1ms(1);//��Ϊdsp�ٶȱ�stm32�죬����Ҫ��һ�²��ܿ�ʼ�������ݣ�����
	for(i=0;i<4;i++)//����У��λ��4λ
	{
		tmp[i] = SpiSendReadOneByte(0xff);
		rx_crc32  |= (u32)tmp[i]<<(i*8);
	}
	SPITEA =1;
	if(cul_crc32 == rx_crc32)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

/***************************************************
 * ���ܣ���������
 * ������len:���ݳ���	data:ָ�����ݵ�ָ��
 * 		 mode:0->8bit����	1->16bit����
 * ����ֵ��NONE
 */
void SpiSendData(u32 len,u8* data,u8 mode)
{
	u32 i,crc32=0;
	u8 tmp;
	if(len == 0) return ;
	SPITEA = 0;
	for(i=0;i<len;i++)
	{
		SpiSendReadOneByte(data[i]&0x00ff);
		if(mode)
		{
			SpiSendReadOneByte(data[i]>>8);
			crc32 += data[i]>>8;
		}
		crc32 += data[i]&0x00ff;
	}

	for(i=0;i<4;i++)
	{
		tmp = (crc32>>(i*8))&0x000000FF;
		SpiSendReadOneByte(tmp);
	}
	SPITEA = 1;
}
/***************************************************
 * ���ܣ�ͨ��SPI������������
 * ������len:���ݳ���	data:ָ�����ݵ�ָ��
 * 	   mode:0->8bit����	(modeֵΪ0)
 * ����ֵ��0->���ճɹ� 	1->����ʧ�ܣ�У�����
 * ������//2018/06/01
 * len==8
 */
u8 SpiReceiveNetData(u8 len,u8* data,u8 mode)
{
	u16 i,rx_crc=0,cul_crc=0;
	if(len == 0) return 0;
	SPITEA = 0;
	for(i=0;i<len;i++)//��������
	{
		data[i] = SpiSendReadOneByte(0xff);

		cul_crc += data[i]&0x00ff;
	}
	cul_crc = cul_crc-data[0]-data[len-2]-data[len-1];
	rx_crc  = data[len-2];
	delay_1ms(1);//��Ϊdsp�ٶȱ�stm32�죬����Ҫ��һ�²��ܿ�ʼ�������ݣ��������
	SPITEA =1;
	if(cul_crc == rx_crc)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}
/***************************************************
 * ���ܣ�ͨ��SPI������������
 * ������len:���ݳ���	data:ָ�����ݵ�ָ��
 * 	mode:0->8bit����	(modeֵΪ0)
 * ����ֵ��NONE
 * ������//2018/06/01
 * len==5
 */
void SpiSendNetdata(u8 len,u8* data,u8 mode)
{
	 u16 crc_data=0;
	 u8 i,tmp;
	SPITEA = 0;
	SpiSendReadOneByte(0x88);
	for(i=0;i<len;i++)
	{
		SpiSendReadOneByte(data[i]&0x00ff);
		crc_data += data[i]&0x00ff;
	}
	crc_data = crc_data&0x00ff;
	tmp = (u8)crc_data;
	SpiSendReadOneByte(tmp);
	SpiSendReadOneByte(0x66);
	SPITEA = 1;
}
/***************************************************
	 * ���ܣ�����һ֡����
	 * ������dataָ��֡���ݵ�ָ��
	 * ����ֵ�����ؽ�������֡�������Լ��Ƿ���յ�����֡
	 * ������//2018/06/01
	 */
u8 JudgeReceiveFrame(u8* data)
{
	u8 value=0;
	value=CompareFrameData(data,NoQuery_data);
	if(value==1) return 1;//�����״̬����֡����λ��û�з����������STM32�����򷵻�ֵ1

	value=CompareFrameData(data,Pause_data);
	if(value==1) return 2;//�������ͣ����֡���򷵻�ֵ2

	value=CompareFrameData(data,Continue_data);
	if(value==1) return 3;//����Ǽ�����������֡���򷵻�ֵ3

	value=CompareFrameData(data,Origin_data);
	if(value==1) return 4;//����Ƿ���ԭ������֡���򷵻�ֵ4

	value=CompareFrameData(data,Stop_data);
	if(value==1) return 5;//�����ֹͣ��������֡���򷵻�ֵ5
    return 0;
}
/***************************************************
	 * ���ܣ�����һ֡����
	 * ������dataָ��֡���ݵ�ָ��
	 * ����ֵ�����ؽ�������֡�������Լ��Ƿ���յ�����֡
	 * ������//2018/06/01
	 */
u8 JudgeReceiveData(u8* data)
{
	u8 value=0;
	value=CompareFrameData(data,NoQuery_data);
	if(value==1) return 1;//�����״̬����֡����λ��û�з����������STM32�����򷵻�ֵ1

	value=CompareFrameData(data,Start_data);
	if(value==1) return 2;//�������������֡���򷵻�ֵ2

	value=CompareFrameData(data,Origin_data);
	if(value==1) return 3;//�������������֡���򷵻�ֵ3



	return 0;
}
/***************************************************
	 * ���ܣ�����һ֡����
	 * ������dataָ��֡���ݵ�ָ��
	 * ����ֵ�����ؽ�������֡�������Լ��Ƿ���յ�����֡
	 * ������//2018/06/01
	 */
u8 CompareFrameData(u8* data,u8* frame_data)
{
	u8 i=0,rec_data[5]={0,0,0,0,0};
	u16 data_sum=0,framedata_sum=0;
	for(i=0;i<5;i++)
	{
		rec_data[i]=data[i+1];
	}
	for(i=0;i<5;i++)
	{
		data_sum=data_sum+rec_data[i];
		framedata_sum=framedata_sum+frame_data[i];
	}
	if(data_sum==framedata_sum)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/***************************************************
 * ���ܣ�ͨ��SPI�����������
 * ������num:�������
 * ����ֵ��NONE
 * ������//2018/06/01
 */
void SendComponentNum(u16 num)
{
	u8 data[8]={0,0,0};
	u8 i=0;
	u16 crc_data=0;
	SPITEA = 0;
	data[0]=0x88;
	data[1]=0x06;
	data[2]=0x00;
	data[3]=num%256;
	data[4]=num/256;
	data[5]=0x00;
	for(i=1;i<6;i++)
	{
		crc_data=crc_data+data[i];
	}
	data[6]=crc_data%256;
	data[7]=0x66;
	for(i=0;i<8;i++)
	{
		SpiSendReadOneByte(data[i]&0x00ff);
	}
	SPITEA = 1;
}
/***************************************************
 * ���ܣ�ͨ��SPI�����������
 * ������num:�������
 * ����ֵ��NONE
 * ������//2018/06/01
 */
void SendManipulatorPositionValue(u16 num)
{
	u8 data[8]={0,0,0};
	u8 i=0;
	u16 crc_data=0;
	SPITEA = 0;
	data[0]=0x88;
	data[1]=0x11;
	data[2]=0x00;
	data[3]=num%256;
	data[4]=num/256;
	data[5]=0x00;
	for(i=1;i<6;i++)
	{
		crc_data=crc_data+data[i];
	}
	data[6]=crc_data%256;
	data[7]=0x66;
	for(i=0;i<8;i++)
	{
		SpiSendReadOneByte(data[i]&0x00ff);
	}
	SPITEA = 1;
}
/***************************************************
 * ���ܣ�ͨ��SPI���ʹ���������ĸ���
 * ������num:�������
 * ����ֵ��NONE
 * ������//2018/06/01
 */
void SendProgramNum(u8 program_num)
{
	u8 data[8]={0,0,0};
	u8 i=0;
	u16 crc_data=0;
	SPITEA = 0;
	data[0]=0x88;
	data[1]=0x0D;
	data[2]=0x00;
	data[3]=program_num;
	data[4]=0x00;
	data[5]=0x00;
	for(i=1;i<6;i++)
	{
		crc_data=crc_data+data[i];
	}
	data[6]=crc_data%256;
	data[7]=0x66;
	for(i=0;i<8;i++)
	{
		SpiSendReadOneByte(data[i]&0x00ff);
	}
	SPITEA = 1;
}
/***************************************************
 * ���ܣ�ͨ��SPI���ʹ��������������
 * ������program_name:��������
 * ����ֵ��NONE
 * ������//2018/06/01
 */
void SendProgramName(u8* program_name)
{
	u8 data[25]={0,0,0};
	u8 i=0;
	u16 crc_data=0;
	SPITEA = 0;
	data[0]=0x88;
	data[1]=0x0E;
	data[2]=0x00;
	crc_data=data[1]+data[2];
    for(i=0;i<20;i++)
    {
    	data[i+3]=program_name[i];
    	crc_data=crc_data+program_name[i];
    }
	data[23]=crc_data%256;
	data[24]=0x66;
	for(i=0;i<25;i++)
	{
		SpiSendReadOneByte(data[i]&0x00ff);
	}
	SPITEA = 1;
}
/***************************************************
 * ���ܣ����ڲ�ѯ�Ƿ�����λ������
 * ������i:�������
 * ����ֵ��NONE
 * ������//2018/06/01
 */
u8 Cycle_Query_Uppermachine(u16* program_select,u8* program_check)
{
	u8 temp_data=0,program_check1=0;
	u16 software_select=0;
	temp_data=Warningup_Querydown(&software_select,&program_check1);
	*program_select=software_select;
	*program_check=program_check1;
	return temp_data;
}
/***************************************************
 * ���ܣ������ϴ�����ѯ�Ƿ�����λ������
 * ������num:�������
 * ����ֵ��NONE
 * ������//2018/06/01
 */
u8 Warningup_Querydown(u16* software_select,u8* program_check1)
{
	u8 Re_Data[8]={0,0,0,0};
	u8 Frame_value=0,i=0;
	//���ͱ���֡�������ݱ��������з���
	warning_solve();
	delay_1ms(3);
	//������λ����������
	SpiReceiveNetData(8,Re_Data,0);
	if(Re_Data[1]!=0x0C)
	{
	//��������֡��������֡������
		Frame_value=JudgeReceiveData(Re_Data);
	}
	else
	{
		//�����ڽ���һ֡���ݣ�ƾ����һ֡���ݣ�
		//�ж���λ���Ƿ���������������򲻽��к�������
		Frame_value=4;
	}


	SetScreen(7);//�л�ҳ��
	switch(Frame_value)
	{
	//��λ��û�з�������֡��STM32
		case 1:
			//*software_select=Re_Data[3]+Re_Data[4]*255;
			*software_select=Re_Data[1]-5;
				return 1;
	//��λ��������������֡��STM32
		case 2:
				SpiSendNetdata(5,Start_Ok,0);//��STM32���ͻ�е���Ѿ�����������
			    return 2;
		case 3:
				SpiSendNetdata(5,origin_OK,0);//��STM32���ͻ�е���Ѿ�����ԭ�������
				return 3;
		case 4:
				*software_select=Re_Data[3];
				*program_check1=Re_Data[4];
				//SpiSendNetdata(5,Re_Data,0);//��STM32���ͻ�е���Ѿ�����������
				for(i=0;i<8;i++)
				{
					SpiSendReadOneByte(Re_Data[i]&0x00ff);
				}
				return 4;

		default:break;
	}
	return 0;
}
/***************************************************
 * ���ܣ������ϴ�����ѯ�Ƿ�����λ������
 * ������num:�������
 * ����ֵ��NONE
 * ������//2018/06/01
 */
void warning_solve(void)
{
	unsigned int err=0;
	err = (ReadErrInfo()&(~0x40));//����ǰ����/������Ϣ�����ε���ȫ����
	if(err)
	{
		if(err&0x02)
		{
			SpiSendNetdata(5,Server_Warn,0);//��STM32���ͻ�е���ŷ�����������

		}
		else if(err&0x01)
		{
			SpiSendNetdata(5,ServerClose_Warn,0);//��STM32���ͻ�е��û�п����ŷ�������
		}
		else if(err&0x04)
		{
			SpiSendNetdata(5,OriginLost_Warn,0);//��STM32���ͻ�е��ԭ�㶪ʧ������
		}
		else if(err&0x380)
		{
			SpiSendNetdata(5,Brake_Warn,0);//��STM32���ͻ�е��ɲ����������
		}
		else if(err&0x30)
		{
			SpiSendNetdata(5,Axis_Warn,0);//��STM32���ͻ�е�ֵ��ᱨ��������
		}
		else if(err&0x08)
		{
			SpiSendNetdata(5,Emergency_Warn,0);//��STM32���ͻ�е�ֽ���ֹͣ������
		}
	}
	else
	{
		SpiSendNetdata(5,Not_Warn,0);//��STM32����ϵͳ״̬����������

	}
}
//===========================================================================
// No more.
//===========================================================================
