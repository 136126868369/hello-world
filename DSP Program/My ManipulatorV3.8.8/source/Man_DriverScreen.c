#include "My_Project.h"
#include "My_USART.h"
#include "crc.h"
u16 crc_data=0;
/****************************************************************************
* ��    �ƣ� SendBeginCMD()
* ��    �ܣ� ����֡ͷ����
* ��ڲ����� ��
* ���ڲ����� ��
****************************************************************************/
void SendBeginCMD()
{
 //SendChar(0xEE);
  sendData(0xEE);
 crc_data=0xFFFF;//��ʼ����CRC16//������2018/06/01
}

/****************************************************************************
* ��    �ƣ� SendEndCmd()
* ��    �ܣ� ����֡β����
* ��ڲ����� ��
* ���ڲ����� ��
****************************************************************************/
void SendEndCmd()
{
/*
 SendChar(0xFF);
 SendChar(0xFC);
 SendChar(0xFF);
 SendChar(0xFF);
*/
sendData(crc_data>>8);//����CRC16У��
sendData(crc_data&0xff);//����CRC16У��
sendData(0xFF);
sendData(0xFC);
sendData(0xFF);
sendData(0xFF);
}
/*****************************************************************
* ��    �ƣ� SendChar()
* ��    �ܣ� ����1���ֽ�
* ��ڲ����� t  ���͵��ֽ�
* ���ڲ����� ��
 *****************************************************************/
//������//2018/06/01(Ϊ�˼���CRCУ�飬�ʽ��ú����ƶ���Man_DriverScreen.c��)
void  SendChar(unsigned char t)
{
  AddCRC16(&t,1,&crc_data);//��ÿһ���ֽڽ���CRC16У��
  ScicRegs.SCITXBUF=t&0xFF;
  while(!ScicRegs.SCICTL2.bit.TXRDY);
}
/*****************************************************************
* ��    �ƣ� SendData()
* ��    �ܣ� ����1���ֽ�
* ��ڲ����� temp  ���͵��ֽ�
* ���ڲ����� ��
 *****************************************************************/
//������//2018/06/01(��ӵĺ���ΪCRCУ��)
void sendData(unsigned char temp)
{
  ScicRegs.SCITXBUF=temp&0xFF;
  while(!ScicRegs.SCICTL2.bit.TXRDY);
}
/****************************************************************************
* ��    �ƣ� SetHandShake()
* ��    �ܣ� ����
* ��ڲ����� ��
* ���ڲ����� ��
****************************************************************************/
void SetHandShake(void)
{
  SendBeginCMD();
  SendChar(0x00);
  SendEndCmd();
}

/****************************************************************************
* ��    �ƣ� SetFcolor()
* ��    �ܣ� ����ǰ��ɫ
* ��ڲ����� fcolor  ǰ��ɫRGB����
* ���ڲ����� ��
****************************************************************************/
void SetFcolor(u16 fcolor)
{
  SendBeginCMD();
  SendChar(0x41);
  SendChar((fcolor>>8)&0xff);
  SendChar(fcolor&0xff);
  SendEndCmd();
}

/****************************************************************************
* ��    �ƣ� SetBcolor()
* ��    �ܣ� ���ñ���ɫ
* ��ڲ����� bcolor  ����ɫRGB����
* ���ڲ����� ��
****************************************************************************/
void SetBcolor(u16 bcolor)
{
  SendBeginCMD();
  SendChar(0x42);
  SendChar((bcolor>>8)&0xff);
  SendChar(bcolor&0xff);
  SendEndCmd();
}

/****************************************************************************
* ��    �ƣ� ColorPicker()
* ��    �ܣ� ȡ��Ļ������ǰ��ɫ/����ɫ
* ��ڲ����� mode ȡ��ǰ��ʾ��Ļ(X,Y)������ɫ��Ϊǰ��/����ɫ
             x       �Ե�Ϊ��λ��X������ֵ
             y       �Ե�Ϊ��λ��Y������ֵ
* ���ڲ����� ��
****************************************************************************/
void ColorPicker(u8 mode, u16 x,u16 y)
{
  SendBeginCMD();
  SendChar(0xa3);
  SendChar(mode);
  SendChar((x>>8)&0xff);
  SendChar(x&0xff);
  SendChar((y>>8)&0xff);
  SendChar(y&0xff);
  SendEndCmd();
}
/****************************************************************************
* ��    �ƣ� GUI_CleanScreen()
* ��    �ܣ� ����
* ��ڲ����� ��
* ���ڲ����� ��
****************************************************************************/
void GUI_CleanScreen()
{
  SendBeginCMD();
  SendChar(0x01);
  SendEndCmd();
}

/****************************************************************************
* ��    �ƣ� SetTextSpace()
* ��    �ܣ� �����������м��
* ��ڲ����� x_w     �Ե�Ϊ��λ���м�࣬ȡֵ��Χ00~1F
             y_w     �Ե�Ϊ��λ���м�࣬ȡֵ��Χ00~1F
* ���ڲ����� ��
****************************************************************************/
void SetTextSpace(u8 x_w, u8 y_w)
{
  SendBeginCMD();
  SendChar(0x43);
  SendChar(x_w);
  SendChar(y_w);
  SendEndCmd();
}
/****************************************************************************
* ��    �ƣ� DisText_Region()
* ��    �ܣ� �޶��ı���ʾ��Χ
* ��ڲ����� enable   �����ı�ʹ��
             length   �ı����޶��ĳ���
	     width    �ı����޶��ĸ߶�
* ���ڲ����� ��
****************************************************************************/
void SetFont_Region(u8 enable,u16 length,u16 width )
{
  SendBeginCMD();
  SendChar(0x45);
  SendChar(enable);
  SendChar((length>>8)&0xff);
  SendChar(length&0xff);
  SendChar((width>>8)&0xff);
  SendChar(width&0xff);
  SendEndCmd();
}
/****************************************************************************
* ��    �ƣ� SetFilterColor()
* ��    �ܣ� ���ù�����ɫ
* ��ڲ����� fillcolor_dwon   �˳���ɫ������ֵ
             fillcolor_up     �˳���ɫ������ֵ
* ���ڲ����� ��
****************************************************************************/
void SetFilterColor(u16 fillcolor_dwon, u16 fillcolor_up)
{
  SendBeginCMD();
  SendChar(0x44);
  SendChar((fillcolor_dwon>>8)&0xff);
  SendChar(fillcolor_dwon&0xff);
  SendChar((fillcolor_up>>8)&0xff);
  SendChar(fillcolor_up&0xff);
  SendEndCmd();
}
/****************************************************************************
* ��    �ƣ� DisText()
* ��    �ܣ� �ı���ʾ
* ��ڲ����� x         �Ե�Ϊ��λ��X������ֵ
             y         �Ե�Ϊ��λ��Y������ֵ
   	         back      ����ɫʹ��
	         font      �ֿ����
             strings   д����ַ���
* ���ڲ����� ��
****************************************************************************/
void DisText(u16 x, u16 y,u8 back,u8 font,u8 *strings )
{
  SendBeginCMD();
  SendChar(0x20);
  SendChar((x>>8)&0xff);
  SendChar(x&0xff);
  SendChar((y>>8)&0xff);
  SendChar(y&0xff);
  SendChar(back);
  SendChar(font);
  SendStrings(strings);
  SendEndCmd();
}

// void DisText(unsigned int x, unsigned int y,unsigned char back,unsigned char font,unsigned char longht,unsigned int *strings)
// {
//   SendChar(0xEE);
//   SendChar(0x20);
//   SendChar(x>>8);
//   SendChar(x&0xff);
//   SendChar(y>>8);
//   SendChar(y&0xff);
//   SendChar(back);
//   SendChar(font);
//   SendStrings(strings,longht);
//   SendEndCmd();
// }

/****************************************************************************
* ��    �ƣ� DisData()
* ��    �ܣ� ������ʾ
* ��ڲ����� x       �Ե�Ϊ��λ��X������ֵ
             y       �Ե�Ϊ��λ��Y������ֵ
   	         back    ����ɫʹ��
	         font    �ֿ����
	         sdata   д�������
* ���ڲ����� ��
****************************************************************************/
void DisData(unsigned int x, unsigned int y,unsigned char back,unsigned char sdata)
{
  SendChar(0xEE);
  SendChar(0x20);
  SendChar((x>>8)&0xff);
  SendChar(x&0xff);
  SendChar((y>>8)&0xff);
  SendChar(y&0xff);
  SendChar(back);   //����ɫʹ��
  SendChar(0x06);   //ѡ������
  SendChar(sdata);
  SendEndCmd();
}

/****************************************************************************
* ��    �ƣ� DisCursor()
* ��    �ܣ� �����ʾ
* ��ڲ����� enable		���ʹ��
             x        �Ե�Ϊ��λ��X������ֵ
             y        �Ե�Ϊ��λ��Y������ֵ
             length   ��곤��
	         width    �����
* ���ڲ����� ��
****************************************************************************/
void DisCursor(u8 enable,u16 x, u16 y,u8 length,u8 width )
{
  SendBeginCMD();
  SendChar(0x21);
  SendChar(enable);
  SendChar((x>>8)&0xff);
  SendChar(x&0xff);
  SendChar((y>>8)&0xff);
  SendChar(y&0xff);
  SendChar(length&0xff);
  SendChar(width&0xff);
  SendEndCmd();
}

/****************************************************************************
* ��    �ƣ� DisFull_Image()
* ��    �ܣ� ȫ������ͼ��ʾ
* ��ڲ����� image_id   ͼƬID���
             masken     ����ɫʹ��
* ���ڲ����� ��
****************************************************************************/
void DisFull_Image(u16 image_id,u8 masken)
{
  SendBeginCMD();
  SendChar(0x31);
  SendChar((image_id>>8)&0xff);
  SendChar(image_id&0xff);
  SendChar(masken);
  SendEndCmd();
}

/****************************************************************************
* ��    �ƣ� DisArea_Image()
* ��    �ܣ� ����ͼƬ��ʾ
* ��ڲ����� x          �Ե�Ϊ��λ��X������ֵ
             y          �Ե�Ϊ��λ��Y������ֵ
             image_id   ͼƬID���
             masken     ����ɫʹ��
* ���ڲ����� ��
****************************************************************************/
void DisArea_Image(u16 x,u16 y,u16 image_id,u8 masken)
{
  SendBeginCMD();
  SendChar(0x32);
  SendChar((x>>8)&0xff);
  SendChar(x&0xff);
  SendChar((y>>8)&0xff);
  SendChar(y&0xff);
  SendChar((image_id>>8)&0xff);
  SendChar(image_id&0xff);
  SendChar(masken);
  SendEndCmd();
}

/****************************************************************************
* ��    �ƣ� DisCut_Image()
* ��    �ܣ� ͼƬ����
* ��ڲ����� x          �Ե�Ϊ��λ��X������ֵ
             y          �Ե�Ϊ��λ��Y������ֵ
             image_id   ͼƬ���
             image_x    ͼƬ�ڲ�X����
             image_y    ͼƬ�ڲ�Y����
             image_l    ���г���
             image_w    ���п��
             masken     ����ɫʹ��
* ���ڲ����� ��
****************************************************************************/
void DisCut_Image(u16 x,u16 y,u16 image_id,u16 image_x,u16 image_y,
                   u16 image_l, u16 image_w,u8 masken)
{
  SendBeginCMD();
  SendChar(0x33);
  SendChar((x>>8)&0xff);
  SendChar(x&0xff);
  SendChar((y>>8)&0xff);
  SendChar(y&0xff);
  SendChar((image_id>>8)&0xff);
  SendChar(image_id&0xff);
  SendChar((image_x>>8)&0xff);
  SendChar(image_x&0xff);
  SendChar((image_y>>8)&0xff);
  SendChar(image_y&0xff);
  SendChar((image_l>>8)&0xff);
  SendChar(image_l&0xff);
  SendChar((image_w>>8)&0xff);
  SendChar(image_w&0xff);
  SendChar(masken);
  SendEndCmd();
}
/****************************************************************************
* ��    �ƣ� DisFlashImgae()
* ��    �ܣ� ������ʾ
* ��ڲ����� x               �Ե�Ϊ��λ��X������ֵ
             y               �Ե�Ϊ��λ��Y������ֵ
             flashimage_id   GIF����ID���
    	     enable          �ر�/������������
	         playnum         �������Ŵ��� 0:һֱ���ţ�1~FF:���Ŵ���
* ���ڲ����� ��
****************************************************************************/
void DisFlashImage(u16 x,u16 y,u16 flashimage_id,u8 enable,u8 playnum)
{
  SendBeginCMD();
  SendChar(0x80);
  SendChar((x>>8)&0xff);
  SendChar(x&0xff);
  SendChar((y>>8)&0xff);
  SendChar(y&0xff);
  SendChar((flashimage_id>>8)&0xff);
  SendChar(flashimage_id&0xff);
  SendChar(enable);
  SendChar(playnum);
  SendEndCmd();
}

/****************************************************************************
* ��    �ƣ� GUI_Dot()
* ��    �ܣ� ����
* ��ڲ����� x       �Ե�Ϊ��λ��X������ֵ
             y       �Ե�Ϊ��λ��Y������ֵ
* ���ڲ����� ��
****************************************************************************/
/*void GUI_Dot(u16 x,u16 y)
{
  SendBeginCMD();
  SendChar(0x50);
  SendChar((x>>8)&0xff);
  SendChar(x&0xff);
  SendChar((y>>8)&0xff);
  SendChar(y&0xff);
  SendEndCmd();
}*/

/****************************************************************************
* ��    �ƣ� GUI_Line()
* ��    �ܣ� ����
* ��ڲ����� x0      �Ե�Ϊ��λ��X������ֵ
             y0      �Ե�Ϊ��λ��Y������ֵ
             x1      �Ե�Ϊ��λ��X������ֵ
             y1      �Ե�Ϊ��λ��Y������ֵ
* ���ڲ����� ��
****************************************************************************/
void GUI_Line(u16 x0, u16 y0, u16 x1, u16 y1)
{
  SendBeginCMD();
  SendChar(0x51);
  SendChar((x0>>8)&0xff);
  SendChar(x0&0xff);
  SendChar((y0>>8)&0xff);
  SendChar(y0&0xff);
  SendChar((x1>>8)&0xff);
  SendChar(x1&0xff);
  SendChar((y1>>8)&0xff);
  SendChar(y1&0xff);
  SendEndCmd();
}

/****************************************************************************
* ��    �ƣ� GUI_ConDots()
* ��    �ܣ� ��������,��ָ����������Զ���������
* ��ڲ����� mode     0:��ǰ��ɫ��� 1���ñ���ɫ���
	         pDot     ָ����ָ��
			 num      ���ݵĸ���
* ���ڲ����� ��
****************************************************************************/
/*void GUI_ConDots(u8 mode,u8 *pDot,u16 num)
{
 u16 i;
  SendBeginCMD();
  SendChar(0x63);
  SendChar(mode);
  for(i=0;i<num;i++)
  {
   SendChar((*pDot>>8)&0xff);
   SendChar(*pDot++&0xff);
  }
  SendEndCmd();
}*/
/****************************************************************************
* ��    �ƣ� GUI_Circle()
* ��    �ܣ� ������Բ
* ��ڲ����� x               �Ե�Ϊ��λ��X������ֵ
             y               �Ե�Ϊ��λ��Y������ֵ
             r                ����Բ�İ뾶
* ���ڲ����� ��
****************************************************************************/
/*void GUI_Circle(u16 x, u16 y, u16 r)
{
  SendBeginCMD();
  SendChar(0x52);
  SendChar((x>>8)&0xff);
  SendChar(x&0xff);
  SendChar((y>>8)&0xff);
  SendChar(y&0xff);
  SendChar((r>>8)&0xff);
  SendChar(r&0xff);
  SendEndCmd();
}*/
/****************************************************************************
* ��    �ƣ� GUI_CircleFill()
* ��    �ܣ� ��ʵ��Բ
* ��ڲ����� x               �Ե�Ϊ��λ��X������ֵ
             y               �Ե�Ϊ��λ��Y������ֵ
             r                ʵ��Բ�İ뾶
* ���ڲ����� ��
****************************************************************************/
/*void GUI_CircleFill(u16 x, u16 y, u16 r)
{
  SendBeginCMD();
  SendChar(0x53);
  SendChar((x>>8)&0xff);
  SendChar(x&0xff);
  SendChar((y>>8)&0xff);
  SendChar(y&0xff);
  SendChar((r>>8)&0xff);
  SendChar(r&0xff);
  SendEndCmd();
}*/
/****************************************************************************
* ��    �ƣ� GUI_Arc()
* ��    �ܣ� ��Բ��
* ��ڲ����� x               �Ե�Ϊ��λ��X������ֵ
             y               �Ե�Ϊ��λ��Y������ֵ
             r               Բ�İ뾶
 	         sa              ��ʼ�Ƕ�
	         ea              �����Ƕ�
* ���ڲ����� ��
****************************************************************************/
/*void GUI_Arc(u16 x,u16 y, u16 r,u16 sa, u16 ea)
{
  SendBeginCMD();
  SendChar(0x67);
  SendChar((x>>8)&0xff);
  SendChar(x&0xff);
  SendChar((y>>8)&0xff);
  SendChar(y&0xff);
  SendChar((r>>8)&0xff);
  SendChar(r&0xff);
  SendChar((sa>>8)&0xff);
  SendChar(sa&0xff);
  SendChar((ea>>8)&0xff);
  SendChar(ea&0xff);
  SendEndCmd();
}*/
/****************************************************************************
* ��    �ƣ� GUI_Rectangle()
* ��    �ܣ� �����ľ���
* ��ڲ����� x0      �Ե�Ϊ��λ�Ŀ��ľ������Ͻ�X����ֵ
             y0      �Ե�Ϊ��λ�Ŀ��ľ������Ͻ�Y����ֵ
             x1      �Ե�Ϊ��λ�Ŀ��ľ������½�X����ֵ
             y1      �Ե�Ϊ��λ�Ŀ��ľ������½�Y����ֵ
* ���ڲ����� ��
****************************************************************************/
void GUI_Rectangle(u16 x0, u16 y0, u16 x1,u16 y1 )
{
  SendBeginCMD();
  SendChar(0x54);
  SendChar((x0>>8)&0xff);
  SendChar(x0&0xff);
  SendChar((y0>>8)&0xff);
  SendChar(y0&0xff);
  SendChar((x1>>8)&0xff);
  SendChar(x1&0xff);
  SendChar((y1>>8)&0xff);
  SendChar(y1&0xff);
  SendEndCmd();
}
/****************************************************************************
* ��    �ƣ� RectangleFill()
* ��    �ܣ� ��ʵ�ľ���
* ��ڲ����� x0      �Ե�Ϊ��λ��ʵ�ľ������Ͻ�X����ֵ
             y0      �Ե�Ϊ��λ��ʵ�ľ������Ͻ�Y����ֵ
             x1      �Ե�Ϊ��λ��ʵ�ľ������½�X����ֵ
             y1      �Ե�Ϊ��λ��ʵ�ľ������½�Y����ֵ
* ���ڲ����� ��
****************************************************************************/
void GUI_RectangleFill(u16 x0, u16 y0, u16 x1,u16 y1 )
{
  SendBeginCMD();
  SendChar(0x55);
  SendChar((x0>>8)&0xff);
  SendChar(x0&0xff);
  SendChar((y0>>8)&0xff);
  SendChar(y0&0xff);
  SendChar((x1>>8)&0xff);
  SendChar(x1&0xff);
  SendChar((y1>>8)&0xff);
  SendChar(y1&0xff);
  SendEndCmd();
}
/****************************************************************************
* ��    �ƣ� GUI_AreaInycolor()
* ��    �ܣ� ��Ļ����ɫ
* ��ڲ����� x0      �Ե�Ϊ��λ��ʵ�ľ������Ͻ�X����ֵ
             y0      �Ե�Ϊ��λ��ʵ�ľ������Ͻ�Y����ֵ
             x1      �Ե�Ϊ��λ��ʵ�ľ������½�X����ֵ
             y1      �Ե�Ϊ��λ��ʵ�ľ������½�Y����ֵ
* ���ڲ����� ��
****************************************************************************/
/*void GUI_AreaInycolor(u16 x0, u16 y0, u16 x1,u16 y1 )
{
  SendBeginCMD();
  SendChar(0x65);
  SendChar((x0>>8)&0xff);
  SendChar(x0&0xff);
  SendChar((y0>>8)&0xff);
  SendChar(y0&0xff);
  SendChar((x1>>8)&0xff);
  SendChar(x1&0xff);
  SendChar((y1>>8)&0xff);
  SendChar(y1&0xff);
  SendEndCmd();
}*/
/****************************************************************************
* ��    �ƣ� GUI_Ellipse()
* ��    �ܣ� ��������Բ
* ��ڲ����� x0      �Ե�Ϊ��λ�Ŀ�����Բ�����X����ֵ
             y0      �Ե�Ϊ��λ�Ŀ�����Բ���϶�Y����ֵ
             x1      �Ե�Ϊ��λ�Ŀ�����Բ���Ҷ�X����ֵ
             y1      �Ե�Ϊ��λ�Ŀ�����Բ���¶�Y����ֵ
* ���ڲ����� ��
****************************************************************************/
/*void GUI_Ellipse (u16 x0, u16 y0, u16 x1,u16 y1 )
{
  SendBeginCMD();
  SendChar(0x56);
  SendChar((x0>>8)&0xff);
  SendChar(x0&0xff);
  SendChar((y0>>8)&0xff);
  SendChar(y0&0xff);
  SendChar((x1>>8)&0xff);
  SendChar(x1&0xff);
  SendChar((y1>>8)&0xff);
  SendChar(y1&0xff);
  SendEndCmd();
}*/
/****************************************************************************
* ��    �ƣ� GUI_EllipseFill()
* ��    �ܣ� ��ʵ����Բ
* ��ڲ����� x0      �Ե�Ϊ��λ��ʵ����Բ�����X����ֵ
             y0      �Ե�Ϊ��λ��ʵ����Բ���϶�Y����ֵ
             x1      �Ե�Ϊ��λ��ʵ����Բ���Ҷ�X����ֵ
             y1      �Ե�Ϊ��λ��ʵ����Բ���¶�Y����ֵ
* ���ڲ����� ��
****************************************************************************/
/*void GUI_EllipseFill (u16 x0, u16 y0, u16 x1,u16 y1 )
{
  SendBeginCMD();
  SendChar(0x57);
  SendChar((x0>>8)&0xff);
  SendChar(x0&0xff);
  SendChar((y0>>8)&0xff);
  SendChar(y0&0xff);
  SendChar((x1>>8)&0xff);
  SendChar(x1&0xff);
  SendChar((y1>>8)&0xff);
  SendChar(y1&0xff);
  SendEndCmd();
}*/
/****************************************************************************
* ��    �ƣ� GUI_PolygonFill()
* ��    �ܣ� ��������
* ��ڲ����� x               �Ե�Ϊ��λ��X������ֵ
             y               �Ե�Ϊ��λ��Y������ֵ
             color            �����ɫ
* ���ڲ����� ��
****************************************************************************/
void GUI_PolygonFill(u16 x, u16 y, u16 color)
{
  SendBeginCMD();
  SendChar(0x64);
  SendChar((x>>8)&0xff);
  SendChar(x&0xff);
  SendChar((y>>8)&0xff);
  SendChar(y&0xff);
  SendChar((color>>8)&0xff);
  SendChar(color&0xff);
  SendEndCmd();
}
/****************************************************************************
* ��    �ƣ� SetBackLight()
* ��    �ܣ� �������
* ��ڲ����� light_level
* ���ڲ����� ��
****************************************************************************/
void SetBackLight(u8 light_level)
{
  SendBeginCMD();
  SendChar(0x60);
  SendChar(light_level);
  SendEndCmd();
}
/****************************************************************************
* ��    �ƣ� SetBuzzer()
* ��    �ܣ� ����������
* ��ڲ����� time
* ���ڲ����� ��
****************************************************************************/
void SetBuzzer(u8 time)
{
  SendBeginCMD();
  SendChar(0x61);
  SendChar(time);
  SendEndCmd();
}
/****************************************************************************
* ��    �ƣ� SetCommBps()
* ��    �ܣ� ����ͨѶ������
* ��ڲ����� Baud
* ���ڲ����� ��
****************************************************************************/
void SetCommBps(u8 baudset)
{
  SendBeginCMD();
  SendChar(0xA0);
  SendChar(baudset);
  SendEndCmd();
}
/****************************************************************************
* ��    �ƣ� SetTouchScreen(u8 cmd)
* ��    �ܣ� ����������
* ��ڲ����� cmd   Bit0:1/0 ������/��; Bit1 : 1/0 ��������/��; Bit2: �ϴ���ʽ
* ���ڲ����� ��
****************************************************************************/
void SetTouchScreen(u8 cmd)
{
  SendBeginCMD();
  SendChar(0x70);
  SendChar(cmd);
  SendEndCmd();
}
/****************************************************************************
* ��    �ƣ� SetTouchScreen_Adj()
* ��    �ܣ� ������У׼��У׼��Ϻ��·�������Ϣ
* ��ڲ����� ��
* ���ڲ����� ��
****************************************************************************/
void SetTouchScreen_Adj()
{
  SendBeginCMD();
  SendChar(0x72);
  SendEndCmd();
}
/****************************************************************************
* ��    �ƣ� TestTouchScreen()
* ��    �ܣ� ����������
* ��ڲ����� ��
* ���ڲ����� ��
****************************************************************************/
void TestTouchScreen()
{
  SendBeginCMD();
  SendChar(0x73);
  SendEndCmd();
}

/****************************************************************************
* ��    �ƣ� WriteLayer()
* ��    �ܣ� дͼ�����
* ��ڲ����� layer         д���ͼ����
* ���ڲ����� ��
****************************************************************************/
void WriteLayer(u8 layer)
{
   SendBeginCMD();
   SendChar(0xa1);
   SendChar(layer);
   SendEndCmd();
}
/****************************************************************************
* ��    �ƣ� WriteLayer()
* ��    �ܣ� �л�ͼ����ʾ
* ��ڲ����� layer         �л���ʾ��ͼ��
* ���ڲ����� ��
****************************************************************************/
void DisplyLayer(u8 layer)
{
   SendBeginCMD();
   SendChar(0xa2);
   SendChar(layer);
   SendEndCmd();
}


 /****************************************************************************
* ��    �ƣ� SetScreen()
* ��    �ܣ� �л�����
* ��ڲ����� screen_id��Ŀ�껭��ID
* ���ڲ����� ��
****************************************************************************/
void SetScreen(u16 screen_id)
{
	SendBeginCMD();
	SendChar(0xb1);
	SendChar(0x00);
	SendChar(screen_id>>8);
	SendChar(screen_id&0xff);
	SendEndCmd();
}

/****************************************************************************
* ��    �ƣ� GetScreen()
* ��    �ܣ� �첽��ȡ��ǰ����ID
* ��ڲ����� ��
* ���ڲ����� ��
****************************************************************************/
void GetScreen()
{
	SendBeginCMD();
	SendChar(0xb1);
	SendChar(0x01);
	SendEndCmd();
}

/****************************************************************************
* ��    �ƣ� DisGifFrame()
* ��    �ܣ� ��ʾĳһͼ��֡
* ��ڲ����� screen_id���ؼ����ڵĻ���ID
* ��ڲ����� control_id��Ŀ��ؼ�ID
* ��ڲ����� icon_imgae_id��ͼ��֡id,��0��ʼ
* ���ڲ����� ��
****************************************************************************/
void DisGifFrame(u16 screen_id,u16 control_id,u8 icon_imgae_id)
{
	SendBeginCMD();
	SendChar(0xb1);
	SendChar(0x23);
	SendChar((screen_id>>8));//&0xff
	SendChar(screen_id&0xff);
	SendChar((control_id>>8));
	SendChar((control_id&0xff));
	SendChar(icon_imgae_id);
	SendEndCmd();
}

/****************************************************************************
* ��    �ƣ� SetControlValue()
* ��    �ܣ� ���ÿؼ�����ֵ
* ��ڲ����� screen_id���ؼ����ڵĻ���ID
* ��ڲ����� control_id��Ŀ��ؼ�ID
* ��ڲ����� param�����ò���
* ��ڲ����� param_len����������
* ���ڲ����� ��
****************************************************************************/
void SetControlValue(u16 screen_id,u16 control_id,u8 *param,u16 param_len)
{
	u16 i = 0;
	SendBeginCMD();
	SendChar(0xb1);
	SendChar(0x10);
	SendChar((screen_id>>8));//&0xff
	SendChar(screen_id&0xff);
	SendChar((control_id>>8));
	SendChar((control_id&0xff));
	for(i=0;i<param_len;++i)
	{
	  SendChar(param[i]);
	}
	SendEndCmd();
}

/****************************************************************************
* ��    �ƣ� SetButtonValue()
* ��    �ܣ� ���ð�ť�ؼ���״ֵ̬
* ��ڲ����� screen_id���ؼ����ڵĻ���ID
* ��ڲ����� control_id��Ŀ��ؼ�ID
* ��ڲ����� value����ť״̬-0����1����
* ���ڲ����� ��
****************************************************************************/
void SetButtonValue(u16 screen_id,u16 control_id,u8 value)
{
	SetControlValue(screen_id,control_id,&value,1);
}

/****************************************************************************
* ��    �ƣ� SetTextValue()
* ��    �ܣ� �����ı��ؼ�����ʾ����
* ��ڲ����� screen_id���ؼ����ڵĻ���ID
* ��ڲ����� control_id��Ŀ��ؼ�ID
* ��ڲ����� str����ʾ����
* ���ڲ����� ��
****************************************************************************/
void SetTextValue(u16 screen_id,u16 control_id,char *str)
{
	u16 len = 0;
	char *p;
	p= str;
	while(*p++) ++len;
	SetControlValue(screen_id,control_id,(u8*)str,len);
}

static void SetProg_Meter_Slider_Value(u16 screen_id,u16 control_id,u32 value)
{
	//u16 i = 0;
	SendBeginCMD();
	SendChar(0xb1);
	SendChar(0x10);
	SendChar(screen_id>>8);
	SendChar(screen_id&0xff);
	SendChar((control_id>>8));
	SendChar((control_id&0xff));
	SendChar((value>>24)&0xff);
	SendChar((value>>16)&0xff);
	SendChar((value>>8)&0xff);
	SendChar((value&0xff));
	SendEndCmd();
}

/****************************************************************************
* ��    �ƣ� SetProgressValue()
* ��    �ܣ� ���ý������ؼ�����ֵ
* ��ڲ����� screen_id���ؼ����ڵĻ���ID
* ��ڲ����� control_id��Ŀ��ؼ�ID
* ��ڲ����� value-�������Ľ���λ��
* ���ڲ����� ��
****************************************************************************/
void SetProgressValue(u16 screen_id,u16 control_id,u32 value)
{
    SetProg_Meter_Slider_Value(screen_id,control_id,value);
}

/****************************************************************************
* ��    �ƣ� SetMeterValue()
* ��    �ܣ� �����Ǳ�ؼ�����ֵ
* ��ڲ����� screen_id���ؼ����ڵĻ���ID
* ��ڲ����� control_id��Ŀ��ؼ�ID
* ��ڲ����� value-�Ǳ�ָ��λ��
* ���ڲ����� ��
****************************************************************************/
void SetMeterValue(u16 screen_id,u16 control_id,u32 value)
{
	SetProg_Meter_Slider_Value(screen_id,control_id,value);
}

/****************************************************************************
* ��    �ƣ� SetSliderValue()
* ��    �ܣ� ���û������ؼ�����ֵ
* ��ڲ����� screen_id���ؼ����ڵĻ���ID
* ��ڲ����� control_id��Ŀ��ؼ�ID
* ��ڲ����� value-�������Ļ���λ��
* ���ڲ����� ��
****************************************************************************/
void SetSliderValue(u16 screen_id,u16 control_id,u32 value)
{
	SetProg_Meter_Slider_Value(screen_id,control_id,value);
 }


/****************************************************************************
* ��    �ƣ� GetControlValue()
* ��    �ܣ� �첽��ȡ�ؼ���ֵ
* ��ڲ����� screen_id���ؼ����ڵĻ���ID
* ��ڲ����� control_id��Ŀ��ؼ�ID
* ���ڲ����� ��
****************************************************************************/
void GetControlValue(u16 screen_id,u16 control_id)
{
	SendBeginCMD();
	SendChar(0xb1);
	SendChar(0x11);
	SendChar(screen_id>>8);
	SendChar(screen_id&0xff);
	SendChar((control_id>>8));
	SendChar((control_id&0xff));
	SendEndCmd();
}
/****************************************************************************
* ��    �ƣ� SetCursor()
* ��    �ܣ� ���ù�꽹��
* ��ڲ����� screen_id���ؼ����ڵĻ���ID
* ��ڲ����� control_id��Ŀ��ؼ�ID
* ��ڲ����� enable��ʹ��
* ���ڲ����� ��
****************************************************************************/
void SetCursor(u16 screen_id,u16 control_id,u8 enable)
{
	SendBeginCMD();
	SendChar(0xb1);
	SendChar(0x02);
	SendChar(screen_id>>8);
	SendChar(screen_id&0xff);
	SendChar((control_id>>8));
	SendChar((control_id&0xff));
	SendChar(enable);
	SendEndCmd();
}
/****************************************************************************
* ��    �ƣ� Getscreen()
* ��    �ܣ� ��ȡ��ǰҳ����
* ��ڲ����� ��
* ���ڲ����� ��
****************************************************************************/
void Getscreen()
{
	SendBeginCMD();
	SendChar(0xb1);
	SendChar(0x01);
	SendEndCmd();
}
/****************************************************************************
* ��    �ƣ� Changescreen()
* ��    �ܣ� ����ҳ��
* ��ڲ����� Ŀ�������
* ���ڲ����� ��
****************************************************************************/
void Changescreen(u8 screen_id)
{
	SendBeginCMD();
	SendChar(0xb1);
	SendChar(0x00);
	SendChar(screen_id>>8);
	SendChar(screen_id&0xff);
	SendEndCmd();
}

/****************************************************************************
* ��    �ƣ� DisableEnableControlId()
* ��    �ܣ� �ر�ʹ�ܿؼ�
* ��ڲ����� ��ǰ�����ţ��ؼ���ţ�ʹ��λ
* ���ڲ����� ��
****************************************************************************/
void DisableEnableControlId(u8 screen_id,u8 control_id,u8 enable)
{
	SendBeginCMD();
	SendChar(0xb1);
	SendChar(0x03);
	SendChar(screen_id>>8);
	SendChar(screen_id&0xff);
	SendChar(control_id>>8);
	SendChar(control_id&0xff);
	SendChar(enable);
	SendEndCmd();
}

/****************************************************************************
* ��    �ƣ� ResetTime()
* ��    �ܣ� ����RTCʱ��
* ��ڲ����� �룬�֣�ʱ���գ����ڣ��·ݣ����
****************************************************************************/
void ResetTime(u8 Sec,u8 Min,u8 Hour,u8 Week,u8 Day,u8 Mon,u8 Year)
{
	SendBeginCMD();
	SendChar(0x81);
	SendChar(Sec);
	SendChar(Min);
	SendChar(Hour);
	SendChar(Day);
	SendChar(Week);
	SendChar(Mon);
	SendChar(Year);
	SendEndCmd();
}

/****************************************************************************
* ��    �ƣ� ReadTime()
* ��    �ܣ� ��ȡRTCʱ��
****************************************************************************/
void ReadTime(void)
{
	SendBeginCMD();
	SendChar(0x82);
	SendEndCmd();
}
/****************************************************************************
* ��    �ƣ� SetRTCMode()
* ��    �ܣ� ����RTC��ʾģʽ
****************************************************************************/
void SetRTCMode(u8 cmd,u8 DisMode,u8 TextMode,u16 Color,u16 Xpoint,u16 Ypoint)
{
	SendBeginCMD();
	SendChar(0x85);
	SendChar(cmd);
	SendChar(DisMode);
	SendChar(TextMode);
	SendChar(Color>>8);
	SendChar(Color&0xff);
	SendChar(Xpoint>>8);
	SendChar(Xpoint&0xff);
	SendChar(Ypoint>>8);
	SendChar(Ypoint&0xff);
	SendEndCmd();
}



