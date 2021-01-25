#include "My_Project.h"
#include "My_USART.h"
#include "crc.h"
u16 crc_data=0;
/****************************************************************************
* 名    称： SendBeginCMD()
* 功    能： 发送帧头命令
* 入口参数： 无
* 出口参数： 无
****************************************************************************/
void SendBeginCMD()
{
 //SendChar(0xEE);
  sendData(0xEE);
 crc_data=0xFFFF;//开始计算CRC16//修正于2018/06/01
}

/****************************************************************************
* 名    称： SendEndCmd()
* 功    能： 发送帧尾命令
* 入口参数： 无
* 出口参数： 无
****************************************************************************/
void SendEndCmd()
{
/*
 SendChar(0xFF);
 SendChar(0xFC);
 SendChar(0xFF);
 SendChar(0xFF);
*/
sendData(crc_data>>8);//发送CRC16校验
sendData(crc_data&0xff);//发送CRC16校验
sendData(0xFF);
sendData(0xFC);
sendData(0xFF);
sendData(0xFF);
}
/*****************************************************************
* 名    称： SendChar()
* 功    能： 发送1个字节
* 入口参数： t  发送的字节
* 出口参数： 无
 *****************************************************************/
//修正于//2018/06/01(为了加入CRC校验，故将该函数移动至Man_DriverScreen.c中)
void  SendChar(unsigned char t)
{
  AddCRC16(&t,1,&crc_data);//对每一个字节进行CRC16校验
  ScicRegs.SCITXBUF=t&0xFF;
  while(!ScicRegs.SCICTL2.bit.TXRDY);
}
/*****************************************************************
* 名    称： SendData()
* 功    能： 发送1个字节
* 入口参数： temp  发送的字节
* 出口参数： 无
 *****************************************************************/
//修正于//2018/06/01(添加的函数为CRC校验)
void sendData(unsigned char temp)
{
  ScicRegs.SCITXBUF=temp&0xFF;
  while(!ScicRegs.SCICTL2.bit.TXRDY);
}
/****************************************************************************
* 名    称： SetHandShake()
* 功    能： 握手
* 入口参数： 无
* 出口参数： 无
****************************************************************************/
void SetHandShake(void)
{
  SendBeginCMD();
  SendChar(0x00);
  SendEndCmd();
}

/****************************************************************************
* 名    称： SetFcolor()
* 功    能： 设置前景色
* 入口参数： fcolor  前景色RGB参数
* 出口参数： 无
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
* 名    称： SetBcolor()
* 功    能： 设置背景色
* 入口参数： bcolor  背景色RGB参数
* 出口参数： 无
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
* 名    称： ColorPicker()
* 功    能： 取屏幕点设置前景色/背景色
* 入口参数： mode 取当前显示屏幕(X,Y)处的颜色作为前景/背景色
             x       以点为单位的X轴坐标值
             y       以点为单位的Y轴坐标值
* 出口参数： 无
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
* 名    称： GUI_CleanScreen()
* 功    能： 清屏
* 入口参数： 无
* 出口参数： 无
****************************************************************************/
void GUI_CleanScreen()
{
  SendBeginCMD();
  SendChar(0x01);
  SendEndCmd();
}

/****************************************************************************
* 名    称： SetTextSpace()
* 功    能： 设置文字行列间距
* 入口参数： x_w     以点为单位的行间距，取值范围00~1F
             y_w     以点为单位的列间距，取值范围00~1F
* 出口参数： 无
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
* 名    称： DisText_Region()
* 功    能： 限定文本显示范围
* 入口参数： enable   限制文本使能
             length   文本框限定的长度
	     width    文本框限定的高度
* 出口参数： 无
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
* 名    称： SetFilterColor()
* 功    能： 设置过滤颜色
* 入口参数： fillcolor_dwon   滤除颜色的下限值
             fillcolor_up     滤除颜色的上限值
* 出口参数： 无
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
* 名    称： DisText()
* 功    能： 文本显示
* 入口参数： x         以点为单位的X轴坐标值
             y         以点为单位的Y轴坐标值
   	         back      背景色使能
	         font      字库编码
             strings   写入的字符串
* 出口参数： 无
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
* 名    称： DisData()
* 功    能： 数据显示
* 入口参数： x       以点为单位的X轴坐标值
             y       以点为单位的Y轴坐标值
   	         back    背景色使能
	         font    字库编码
	         sdata   写入的数据
* 出口参数： 无
****************************************************************************/
void DisData(unsigned int x, unsigned int y,unsigned char back,unsigned char sdata)
{
  SendChar(0xEE);
  SendChar(0x20);
  SendChar((x>>8)&0xff);
  SendChar(x&0xff);
  SendChar((y>>8)&0xff);
  SendChar(y&0xff);
  SendChar(back);   //背景色使能
  SendChar(0x06);   //选择字体
  SendChar(sdata);
  SendEndCmd();
}

/****************************************************************************
* 名    称： DisCursor()
* 功    能： 光标显示
* 入口参数： enable		光标使能
             x        以点为单位的X轴坐标值
             y        以点为单位的Y轴坐标值
             length   光标长度
	         width    光标宽度
* 出口参数： 无
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
* 名    称： DisFull_Image()
* 功    能： 全屏整幅图显示
* 入口参数： image_id   图片ID编号
             masken     屏蔽色使能
* 出口参数： 无
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
* 名    称： DisArea_Image()
* 功    能： 区域图片显示
* 入口参数： x          以点为单位的X轴坐标值
             y          以点为单位的Y轴坐标值
             image_id   图片ID编号
             masken     屏蔽色使能
* 出口参数： 无
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
* 名    称： DisCut_Image()
* 功    能： 图片剪切
* 入口参数： x          以点为单位的X轴坐标值
             y          以点为单位的Y轴坐标值
             image_id   图片编号
             image_x    图片内部X坐标
             image_y    图片内部Y坐标
             image_l    剪切长度
             image_w    剪切宽度
             masken     屏蔽色使能
* 出口参数： 无
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
* 名    称： DisFlashImgae()
* 功    能： 动画显示
* 入口参数： x               以点为单位的X轴坐标值
             y               以点为单位的Y轴坐标值
             flashimage_id   GIF动画ID编号
    	     enable          关闭/开启动作播放
	         playnum         动画播放次数 0:一直播放，1~FF:播放次数
* 出口参数： 无
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
* 名    称： GUI_Dot()
* 功    能： 画点
* 入口参数： x       以点为单位的X轴坐标值
             y       以点为单位的Y轴坐标值
* 出口参数： 无
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
* 名    称： GUI_Line()
* 功    能： 画线
* 入口参数： x0      以点为单位的X轴坐标值
             y0      以点为单位的Y轴坐标值
             x1      以点为单位的X轴坐标值
             y1      以点为单位的Y轴坐标值
* 出口参数： 无
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
* 名    称： GUI_ConDots()
* 功    能： 绘制折线,将指定的坐标点自动连接起来
* 入口参数： mode     0:用前景色描绘 1：用背景色描绘
	         pDot     指向点的指针
			 num      数据的个数
* 出口参数： 无
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
* 名    称： GUI_Circle()
* 功    能： 画空心圆
* 入口参数： x               以点为单位的X轴坐标值
             y               以点为单位的Y轴坐标值
             r                空心圆的半径
* 出口参数： 无
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
* 名    称： GUI_CircleFill()
* 功    能： 画实心圆
* 入口参数： x               以点为单位的X轴坐标值
             y               以点为单位的Y轴坐标值
             r                实心圆的半径
* 出口参数： 无
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
* 名    称： GUI_Arc()
* 功    能： 画圆弧
* 入口参数： x               以点为单位的X轴坐标值
             y               以点为单位的Y轴坐标值
             r               圆的半径
 	         sa              起始角度
	         ea              结束角度
* 出口参数： 无
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
* 名    称： GUI_Rectangle()
* 功    能： 画空心矩形
* 入口参数： x0      以点为单位的空心矩形左上角X坐标值
             y0      以点为单位的空心矩形左上角Y坐标值
             x1      以点为单位的空心矩形右下角X坐标值
             y1      以点为单位的空心矩形右下角Y坐标值
* 出口参数： 无
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
* 名    称： RectangleFill()
* 功    能： 画实心矩形
* 入口参数： x0      以点为单位的实心矩形左上角X坐标值
             y0      以点为单位的实心矩形左上角Y坐标值
             x1      以点为单位的实心矩形右下角X坐标值
             y1      以点为单位的实心矩形右下角Y坐标值
* 出口参数： 无
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
* 名    称： GUI_AreaInycolor()
* 功    能： 屏幕区域反色
* 入口参数： x0      以点为单位的实心矩形左上角X坐标值
             y0      以点为单位的实心矩形左上角Y坐标值
             x1      以点为单位的实心矩形右下角X坐标值
             y1      以点为单位的实心矩形右下角Y坐标值
* 出口参数： 无
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
* 名    称： GUI_Ellipse()
* 功    能： 画空心椭圆
* 入口参数： x0      以点为单位的空心椭圆最左端X坐标值
             y0      以点为单位的空心椭圆最上端Y坐标值
             x1      以点为单位的空心椭圆最右端X坐标值
             y1      以点为单位的空心椭圆最下端Y坐标值
* 出口参数： 无
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
* 名    称： GUI_EllipseFill()
* 功    能： 画实心椭圆
* 入口参数： x0      以点为单位的实心椭圆最左端X坐标值
             y0      以点为单位的实心椭圆最上端Y坐标值
             x1      以点为单位的实心椭圆最右端X坐标值
             y1      以点为单位的实心椭圆最下端Y坐标值
* 出口参数： 无
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
* 名    称： GUI_PolygonFill()
* 功    能： 多边形填充
* 入口参数： x               以点为单位的X轴坐标值
             y               以点为单位的Y轴坐标值
             color            填充颜色
* 出口参数： 无
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
* 名    称： SetBackLight()
* 功    能： 背光调节
* 入口参数： light_level
* 出口参数： 无
****************************************************************************/
void SetBackLight(u8 light_level)
{
  SendBeginCMD();
  SendChar(0x60);
  SendChar(light_level);
  SendEndCmd();
}
/****************************************************************************
* 名    称： SetBuzzer()
* 功    能： 蜂鸣器控制
* 入口参数： time
* 出口参数： 无
****************************************************************************/
void SetBuzzer(u8 time)
{
  SendBeginCMD();
  SendChar(0x61);
  SendChar(time);
  SendEndCmd();
}
/****************************************************************************
* 名    称： SetCommBps()
* 功    能： 设置通讯波特率
* 入口参数： Baud
* 出口参数： 无
****************************************************************************/
void SetCommBps(u8 baudset)
{
  SendBeginCMD();
  SendChar(0xA0);
  SendChar(baudset);
  SendEndCmd();
}
/****************************************************************************
* 名    称： SetTouchScreen(u8 cmd)
* 功    能： 触摸屏控制
* 入口参数： cmd   Bit0:1/0 触摸开/闭; Bit1 : 1/0 蜂鸣器开/闭; Bit2: 上传方式
* 出口参数： 无
****************************************************************************/
void SetTouchScreen(u8 cmd)
{
  SendBeginCMD();
  SendChar(0x70);
  SendChar(cmd);
  SendEndCmd();
}
/****************************************************************************
* 名    称： SetTouchScreen_Adj()
* 功    能： 触摸屏校准，校准完毕后下发结束信息
* 入口参数： 无
* 出口参数： 无
****************************************************************************/
void SetTouchScreen_Adj()
{
  SendBeginCMD();
  SendChar(0x72);
  SendEndCmd();
}
/****************************************************************************
* 名    称： TestTouchScreen()
* 功    能： 触摸屏体验
* 入口参数： 无
* 出口参数： 无
****************************************************************************/
void TestTouchScreen()
{
  SendBeginCMD();
  SendChar(0x73);
  SendEndCmd();
}

/****************************************************************************
* 名    称： WriteLayer()
* 功    能： 写图层操作
* 入口参数： layer         写入的图层数
* 出口参数： 无
****************************************************************************/
void WriteLayer(u8 layer)
{
   SendBeginCMD();
   SendChar(0xa1);
   SendChar(layer);
   SendEndCmd();
}
/****************************************************************************
* 名    称： WriteLayer()
* 功    能： 切换图层显示
* 入口参数： layer         切换显示的图层
* 出口参数： 无
****************************************************************************/
void DisplyLayer(u8 layer)
{
   SendBeginCMD();
   SendChar(0xa2);
   SendChar(layer);
   SendEndCmd();
}


 /****************************************************************************
* 名    称： SetScreen()
* 功    能： 切换画面
* 入口参数： screen_id，目标画面ID
* 出口参数： 无
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
* 名    称： GetScreen()
* 功    能： 异步获取当前画面ID
* 入口参数： 无
* 出口参数： 无
****************************************************************************/
void GetScreen()
{
	SendBeginCMD();
	SendChar(0xb1);
	SendChar(0x01);
	SendEndCmd();
}

/****************************************************************************
* 名    称： DisGifFrame()
* 功    能： 显示某一图标帧
* 入口参数： screen_id，控件所在的画面ID
* 入口参数： control_id，目标控件ID
* 入口参数： icon_imgae_id，图标帧id,从0开始
* 出口参数： 无
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
* 名    称： SetControlValue()
* 功    能： 设置控件的数值
* 入口参数： screen_id，控件所在的画面ID
* 入口参数： control_id，目标控件ID
* 入口参数： param，设置参数
* 入口参数： param_len，参数长度
* 出口参数： 无
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
* 名    称： SetButtonValue()
* 功    能： 设置按钮控件的状态值
* 入口参数： screen_id，控件所在的画面ID
* 入口参数： control_id，目标控件ID
* 入口参数： value，按钮状态-0弹起，1按下
* 出口参数： 无
****************************************************************************/
void SetButtonValue(u16 screen_id,u16 control_id,u8 value)
{
	SetControlValue(screen_id,control_id,&value,1);
}

/****************************************************************************
* 名    称： SetTextValue()
* 功    能： 设置文本控件的显示文字
* 入口参数： screen_id，控件所在的画面ID
* 入口参数： control_id，目标控件ID
* 入口参数： str，显示文字
* 出口参数： 无
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
* 名    称： SetProgressValue()
* 功    能： 设置进度条控件的数值
* 入口参数： screen_id，控件所在的画面ID
* 入口参数： control_id，目标控件ID
* 入口参数： value-进度条的进度位置
* 出口参数： 无
****************************************************************************/
void SetProgressValue(u16 screen_id,u16 control_id,u32 value)
{
    SetProg_Meter_Slider_Value(screen_id,control_id,value);
}

/****************************************************************************
* 名    称： SetMeterValue()
* 功    能： 设置仪表控件的数值
* 入口参数： screen_id，控件所在的画面ID
* 入口参数： control_id，目标控件ID
* 入口参数： value-仪表指针位置
* 出口参数： 无
****************************************************************************/
void SetMeterValue(u16 screen_id,u16 control_id,u32 value)
{
	SetProg_Meter_Slider_Value(screen_id,control_id,value);
}

/****************************************************************************
* 名    称： SetSliderValue()
* 功    能： 设置滑动条控件的数值
* 入口参数： screen_id，控件所在的画面ID
* 入口参数： control_id，目标控件ID
* 入口参数： value-滑动条的滑块位置
* 出口参数： 无
****************************************************************************/
void SetSliderValue(u16 screen_id,u16 control_id,u32 value)
{
	SetProg_Meter_Slider_Value(screen_id,control_id,value);
 }


/****************************************************************************
* 名    称： GetControlValue()
* 功    能： 异步获取控件数值
* 入口参数： screen_id，控件所在的画面ID
* 入口参数： control_id，目标控件ID
* 出口参数： 无
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
* 名    称： SetCursor()
* 功    能： 设置光标焦点
* 入口参数： screen_id，控件所在的画面ID
* 入口参数： control_id，目标控件ID
* 入口参数： enable，使能
* 出口参数： 无
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
* 名    称： Getscreen()
* 功    能： 获取当前页面编号
* 入口参数： 无
* 出口参数： 无
****************************************************************************/
void Getscreen()
{
	SendBeginCMD();
	SendChar(0xb1);
	SendChar(0x01);
	SendEndCmd();
}
/****************************************************************************
* 名    称： Changescreen()
* 功    能： 更改页面
* 入口参数： 目标界面编号
* 出口参数： 无
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
* 名    称： DisableEnableControlId()
* 功    能： 关闭使能控件
* 入口参数： 当前界面编号，控件编号，使能位
* 出口参数： 无
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
* 名    称： ResetTime()
* 功    能： 更新RTC时间
* 入口参数： 秒，分，时，日，星期，月份，年份
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
* 名    称： ReadTime()
* 功    能： 读取RTC时间
****************************************************************************/
void ReadTime(void)
{
	SendBeginCMD();
	SendChar(0x82);
	SendEndCmd();
}
/****************************************************************************
* 名    称： SetRTCMode()
* 功    能： 设置RTC显示模式
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



