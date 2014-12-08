#include <stdio.h>
#include <math.h>
typedef struct 
{
	int G_pathmode;		//路径插补模式G00(G0)=0 G01(G1)=1 G02(G2)=2 G03(G3)=3
	double X , Y;	//坐标
	double I, J;		//圆坐标数据
}  nc_data;
nc_data cs[]={ {1,0,1,0,0},{1,-1,0,0,0},{1,0,1,0,0}, {2,2,0,1,-1},{2,0,-2,0,-1},{1,0,1,0,0},{1,-0.9,0,0,0},{1,0,-0.5,0,0} },cs1[100],cs2[100];
nc_data cs0[]= {{0,0,0,0,0},{0,0,0,0,0}};
//经过刀具补偿之后的数据以及轨迹处理后上表面的轨迹


struct mihua
{
	double X,Y;
} ; //点的坐标
//全局变量声明
double Xtop,Ytop,Xbtm,Ybtm;//直线的一些参数
double Xtopstart,Ytopstart,Xtopend, Ytopend,Xbtmstart,Ybtmstart,Xbtmend,Ybtmend,Rtoptop,Rbtmbtm,deltl;//圆弧的一些参数
int top,btm;//用来记录密化了多少段
struct mihua m1[2000],m2[2000],tmp1[2000],tmp2[2000];//记录每段线段的密化点
int d=0;//整个完整图形的密化点数


//一些函数的声明
//单条直线的密化函数
void line(double X,double Y,double deltl,struct mihua tmp[],int *a )
{
	double len;
	int m;
	len=sqrt(X*X + Y*Y);
	for(m=0;len>=deltl;m++)
	{
		tmp[m].X=deltl*X/sqrt(X*X + Y*Y);
	    tmp[m].Y=deltl*Y/sqrt(X*X + Y*Y);
		len=len-deltl;
	}
	tmp[m].X=X-m*deltl*X/sqrt(X*X + Y*Y);
	tmp[m].Y=Y-m*deltl*Y/sqrt(X*X + Y*Y);
	*a = m;
}
// 上下对应直线的密化函数
void lineline(double Xtop,double Ytop,double Xbottom,double Ybottom,struct mihua tmp1[],struct mihua tmp2[],int *c1,int *c2)//直线对直线的处理方法
{
	int c; 
	int m;// 记录密化点数
	double deltl2,deltl1;
	struct mihua tmp[2000];
	if( (Xtop*Xtop + Ytop*Ytop) >= (Xbottom*Xbottom + Ybottom*Ybottom) )
	{
		deltl2=0.04;
		line(Xbottom,Ybottom,deltl2,tmp,&c);//将下表面的密化点存进tmp[]中，并记录点的个数于c中。
		for(m=0;m<=c;m++)
		{
		tmp2[m]=tmp[m];
		}
		*c2 = c;
		deltl1=deltl2*sqrt( (Xtop*Xtop + Ytop*Ytop)/(Xbottom*Xbottom + Ybottom*Ybottom) );
		line(Xtop,Ytop,deltl1,tmp,&c);
		for(m=0;m<=c;m++)
		{
		tmp1[m]=tmp[m];
		}
		*c1 = c;
	}
	else
	{
		deltl1=0.04;
		line(Xtop,Ytop,deltl1,tmp,&c);
		for(m=0;m<=c;m++)
		{
		tmp1[m]=tmp[m];
		}
		*c1 = c;
		deltl2=deltl1*sqrt( (Xbottom*Xbottom + Ybottom*Ybottom)/(Xtop*Xtop + Ytop*Ytop) );
		line(Xbottom,Ybottom,deltl2,tmp,&c);
		for(m=0;m<=c;m++)
		{
		tmp2[m]=tmp[m];
		}
		*c2 = c;
	}
}
// 单个圆弧的密化函数
void arc(double X1,double Y1,double X2,double Y2,double R, double deltl,struct mihua arc[],int *a ) //将一点圆弧密化（不用分象限吧）
{
	int i;//循环变量
	double m,n,L,deltx,delty;
	m=deltl*sqrt(4*R*R-deltl*deltl)/(2*R*R);
	n=deltl*deltl/(2*R*R);
	L=sqrt( (X2-X1)*(X2-X1) + (Y2-Y1)*(Y2-Y1) );//剩余线段的长度
 	for(i=0;L>deltl;i++)
	{
		deltx=m*Y1-n*X1;
		delty=-(m*X1+n*Y1);
		arc[i].X=deltx;
		arc[i].Y=delty;
		X1+=deltx;
		Y1+=delty;
		L=sqrt((X2-X1)*(X2-X1) + (Y2-Y1)*(Y2-Y1) );
	}
	arc[i].X=X2-X1;
	arc[i].Y=Y2-Y1;
	*a = i;
}

void arc1(double X1,double Y1,double X2,double Y2,double R, double deltl,struct mihua arc[],int *a ) //将一点圆弧密化（不用分象限吧）
{
	int i;//循环变量
	double m,n,L,deltx,delty;
	m=deltl*sqrt(4*R*R-deltl*deltl)/(2*R*R);
	n=deltl*deltl/(2*R*R);
	L=sqrt( (X2-X1)*(X2-X1) + (Y2-Y1)*(Y2-Y1) );//剩余线段的长度
 	for(i=0;L>deltl;i++)
	{
		deltx=-(m*Y1+n*X1);
		delty= (m*X1-n*Y1);
		arc[i].X=deltx;
		arc[i].Y=delty;
		X1+=deltx;
		Y1+=delty;
		L=sqrt((X2-X1)*(X2-X1) + (Y2-Y1)*(Y2-Y1) );
	}
	arc[i].X=X2-X1;
	arc[i].Y=Y2-Y1;
	*a = i;
}

// 上下对应圆弧的密化函数
void arcarc(double Xtopstart,double Ytopstart,double Xtopend,double Ytopend,
			 double Xbtmstart,double Ybtmstart,double Xbtmend,double Ybtmend,
			 struct mihua tmp1[],struct mihua tmp2[],int *c1,int *c2
			 )
{
    struct mihua tmp[2000];
	int c;//记录密化的个数
	int m;//循环变量
	double  Rtop,Rbtm,deltl1,deltl2;  //上下圆弧的半径、内接弦线长、弧长
    Rtop = sqrt(Xtopstart*Xtopstart + Ytopstart*Ytopstart);
    Rbtm = sqrt(Xbtmstart*Xbtmstart + Ybtmstart*Ybtmstart);
	if(Rtop >= Rbtm)
	{
		deltl2 = sqrt(8*Rbtm*0.001);
		arc(Xbtmstart,Ybtmstart,Xbtmend,Ybtmend,Rbtm,deltl2,tmp,&c);
		for(m=0;m<=c;m++)
		{
		tmp2[m]=tmp[m];
		}
		*c2 = c;
        
		//求两段圆弧的长度，相比可求出长圆弧的步长
		deltl1 = deltl2 * Rtop*asin(sqrt((Xtopend-Xtopstart)*(Xtopend-Xtopstart)+(Ytopend-Ytopstart)*(Ytopend-Ytopstart))/(2*Rtop))/(Rbtm*asin(sqrt((Xbtmend-Xbtmstart)*(Xbtmend-Xbtmstart)+(Ybtmend-Ybtmstart)*(Ybtmend-Ybtmstart))/(2*Rbtm)));
        arc(Xtopstart,Ytopstart,Xtopend,Ytopend,Rtop,deltl1,tmp,&c);
		for(m=0;m<=c;m++)
		{
		tmp1[m]=tmp[m];
		}
		*c1 = c;
	}
	else
	{
		deltl1 = sqrt(8*Rtop*0.001);
		arc(Xtopstart,Ytopstart,Xtopend,Ytopend,Rtop,deltl1,tmp,&c);
		for(m=0;m<=c;m++)
		{
		tmp1[m]=tmp[m];
		}
		*c1 = c;
        
		//求两段圆弧的长度，相比可求出长圆弧的步长
		deltl2 = deltl1 / (Rtop*asin(sqrt((Xtopend-Xtopstart)*(Xtopend-Xtopstart)+(Ytopend-Ytopstart)*(Ytopend-Ytopstart))/(2*Rtop))/(Rbtm*asin(sqrt((Xbtmend-Xbtmstart)*(Xbtmend-Xbtmstart)+(Ybtmend-Ybtmstart)*(Ybtmend-Ybtmstart))/(2*Rbtm))));
		arc(Xbtmstart,Ybtmstart,Xbtmend,Ybtmend,Rbtm,deltl2,tmp,&c);
		for(m=0;m<=c;m++)
		{
		tmp2[m]=tmp[m];
		}
		*c2 = c;
	}
}
//函数声明结束



main()
{
    
	int N=7; //数组的元素个数
	//double H;//参考面距工件上表面的距离
	//float a;//锥度角
	double R=0.2;//相当于刀具补偿的刀具半径
	double X0,Y0,X1,Y1,d0,d1,r0x,r0y,r1x,r1y;
	double S1x,S1y,S2x,S2y,S3x,S3y,K1,K2,Sx,Sy,Sx1,Sy1,Sx2,Sy2,Start_X=0,Start_Y=0,Start_X2=0,Start_Y2=0;
	double I_one,J_one,I_two,J_two,KBx0,KBy0,KBx1,KBy1,Ox,Oy,K_arc,a_arc,b_arc,c_arc,r_arc,Kline;
	int j=0,i=0,m;
	//R=H*tan(a);//上下表面轮廓在xy平面上的投影之间的距离

    

	if(cs[j].G_pathmode == 1 && cs[j+1].G_pathmode ==1)//直线接直线
	{
		printf("1\n");
		X0 = cs[j].X;
		Y0 = cs[j].Y;
		X1 = cs[j+1].X; 
		Y1 = cs[j+1].Y;
		//开始求偏移矢量(右偏)
		d0 = sqrt(X0*X0 +Y0*Y0);	
		d1 = sqrt(X1*X1 +Y1*Y1);
		r0x = 0;
		r0y = 0;
		r1x = R*Y1/d1;
		r1y = (-R)*X1/d1;
		//判断交接类型
		if( ( (r1x-r0x)*X0 + (r1y-r0y)*Y0 )<0 )//缩短型
		{
			//求上表面对应的点
			if(X0 == 0)
			{
				S1x=X0 + r0x;S1y=Y0 + r0y;
				S2x=X0 + r1x;S2y=Y0 + r1y;K2=Y1/X1;
				Sx =S1x;
				Sy =K2*(Sx - S2x) + S2y;
			}
			else if(X1 ==0)
			{
				S1x=X0 + r0x;S1y=Y0 + r0y;K1=Y0/X0;
				S2x=X0 + r1x;S2y=Y0 + r1y;
				Sx =S2x;
				Sy =K1*(Sx - S1x) + S1y;
			}
			else
			{
				S1x=X0 + r0x;S1y=Y0 + r0y;K1=Y0/X0;
				S2x=X0 + r1x;S2y=Y0 + r1y;K2=Y1/X1;
				Sx = (S2y - S1y - K2*S2x + K1*S1x)/(K1 - K2);
				Sy = K1*(Sx - S1x) + S1y;
			}
			//转换坐标
			cs1[i]= cs[j];
			cs1[i].X = Sx - Start_X;
			cs1[i].Y = Sy - Start_Y;
			cs2[i]= cs[j];
			i++;
			Start_X = Sx - X0;	
			Start_Y = Sy - Y0;
		}
		else //伸长型
		{
			//求上表面对应的点
			if(X0 == 0)
			{
				S1x=X0 + r0x;S1y=Y0 + r0y;
				S2x=X0 + r1x;S2y=Y0 + r1y;K2=Y1/X1;
				Sx =S1x;
				Sy =K2*(Sx - S2x) + S2y;
			}
			else if(X1 ==0)
			{
				S1x=X0 + r0x;S1y=Y0 + r0y;K1=Y0/X0;
				S2x=X0 + r1x;S2y=Y0 + r1y;
				Sx =S2x;
				Sy =K1*(Sx - S1x) + S1y;
			}
			else
			{
				S1x=X0 + r0x;S1y=Y0 + r0y;K1=Y0/X0;
				S2x=X0 + r1x;S2y=Y0 + r1y;K2=Y1/X1;
				Sx = (S2y - S1y - K2*S2x + K1*S1x)/(K1 - K2);
				Sy = K1*(Sx - S1x) + S1y;
			}
			//转换坐标
			cs1[i]= cs[j];
			cs1[i].X = Sx - Start_X;
			cs1[i].Y = Sy - Start_Y;
			cs2[i]= cs[j];
			i++;
			Start_X = Sx - X0;	
			Start_Y = Sy - Y0;
		}
	}//直线接直线结束

	else if(cs[j].G_pathmode == 1 && (cs[j+1].G_pathmode ==3 || cs[j+1].G_pathmode ==2))//直线接圆弧
	{
			printf("3\n");
		X0 = cs[j].X;
		Y0 = cs[j].Y;

		I_two=cs[j+1].I;
		J_two=cs[j+1].J;

		X1 = cs[j+1].X;
		Y1 = cs[j+1].Y;

		d0 = sqrt(X0*X0 + Y0*Y0);
		d1 = sqrt((X1-I_two)*(X1-I_two)+(Y1-J_two)*(Y1-J_two));	

		r0x = 0;
		r0y = 0;		
		if(cs[j+1].G_pathmode == 2)//圆弧顺时针
		{
		    r1x = (-R)*(-I_two)/d1;
			r1y = (-R)*(-J_two)/d1;
		}
		else if (cs[j+1].G_pathmode == 3)//圆弧逆时针
		{
			r1x = R*(-I_two)/d1;
			r1y = R*(-J_two)/d1;
		}

	
		//确定KB，即第二段圆弧起点的切线斜率
		if(cs[j+1].G_pathmode == 2)
		{
			KBx1 = -J_two;
			KBy1 =  I_two;
		}
		else if(cs[j+1].G_pathmode == 3)
		{
			KBx1 =  J_two;
			KBy1 = -I_two;
		}// 确定KB参量结束;

		//判断交接类型，求拐点对应的坐标，并储存
		if( (X0*(r1x-r0x)+Y0*(r1y-r0y))>0  )//伸长型
		{
			printf("3.1\n");
			if(KBx1 == 0 && X0 != 0)
			{
			S1x= X0+r0x;	S1y=Y0+r0y;  K1=Y0/X0;
			S2x= X0+r1x;	S2y=Y0+r1y; 
							
			Sx = S2x;
			Sy = K1*(Sx - S1x) + S1y;//直线斜率无限大时;					
			}
			else if(KBx1 != 0 && X0 == 0)
			{
			S1x= X0+r0x;	S1y=Y0+r0y;   
			S2x= X0+r1x;	S2y=Y0+r1y;	 K2=KBy1/KBx1;
							
			Sx = S1x;
			Sy = K2*(Sx - S2x) + S2y;//直线斜率无限大时;					
			}
			
			else
			{
				S1x= X0+r0x;	S1y=Y0+r0y;  K1=Y0/X0;
				S2x= X0+r1x;	S2y=Y0+r1y;	 K2=KBy1/KBx1;
			
							
			Sx = (S2y - S1y - K2*S2x + K1*S1x)/(K1 - K2);
			Sy = K1*(Sx - S1x) + S1y;
			}//伸长线的交点坐标;
			//转换坐标
			cs1[i]= cs[j];
			cs1[i].X = Sx - Start_X;
			cs1[i].Y = Sy - Start_Y;//第一段直线的终点
			cs2[i]= cs[j];//下表面对应点
			i++;

			cs1[i].X = S2x - Sx;
			cs1[i].Y = S2y - Sy;//圆弧切线的伸长部分
			cs1[i].G_pathmode = 1;
			cs2[i]= cs0[0];//下表面对应点
			i++;

			Start_X = S2x - X0;	
			Start_Y = S2y - Y0;//转接点相对于第二段G代码起点的相对坐标,为求下一个拐点的相对坐标做准备;
		}//伸长型结束
		else if ( (r0x == r1x) && (r0y == r1y) )//相切型
			{
			printf("3.2\n");
			Sx = X0+r0x;
			Sy = Y0+r0y;
			cs1[i]= cs[j];
            cs1[i].X = Sx - Start_X;
			cs1[i].Y = Sy - Start_Y;//第一段直线的终点
			cs2[i]= cs[j];//下表面对应点
			i++;
			printf("1233\n");
			Start_X = Sx - X0;	
			Start_Y = Sy - Y0;//转接点相对于第二段G代码起点的相对坐标,为求下一个拐点的相对坐标做准备;
			}//相切型结束;


		else if ( (X0*(r1x-r0x)+Y0*(r1y-r0y) )<0||(cs[j+1].G_pathmode==3 && r0x == -(r1x) && r0y == -(r1y) ) )// 缩短型
		{
			printf("3.3\n");
			S1x=X0+r0x;	S1y=Y0+r0y;
			S3x=r0x;	S3y=r0y;//与圆弧相交的直线的两个点;

			Ox=X0+I_two;	Oy=Y0+J_two;//圆心坐标;
			r_arc=sqrt( (X0+r1x-Ox)*(X0+r1x-Ox) + (Y0+r1y-Oy)*(Y0+r1y-Oy) );//圆弧半径
			if(S1x == S3x)
			{
			Sx1=S1x ;	Sy1=Oy + sqrt(r_arc*r_arc-(S1x-Ox)*(S1x-Ox));
			Sx2=S1x ;	Sy2=Oy - sqrt(r_arc*r_arc-(S1x-Ox)*(S1x-Ox));
			}
			else if(S1y == S3y)
			{
			Sx1=Ox + sqrt(r_arc*r_arc-(S1y-Oy)*(S1y-Oy));	Sy1=S1y;
			Sx2=Ox - sqrt(r_arc*r_arc-(S1y-Oy)*(S1y-Oy));	Sy2=S1y;
			}
			else
			{
			K_arc=(S3y-S1y)/(S3x-S1x);
			a_arc=1+K_arc*K_arc;
			b_arc=2*K_arc*(S1y-Oy-K_arc*S1x-Ox/K_arc);
			c_arc=(S1y-Oy-K_arc*S1x)*(S1y-Oy-K_arc*S1x)-r_arc*r_arc+Ox*Ox;
							
			Sx1=( -b_arc+sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);	Sy1=S1y + K_arc*(Sx1-S1x);
			Sx2=( -b_arc-sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);	Sy2=S1y + K_arc*(Sx2-S1x);
			}//交点坐标;
			//判断是两个交点中的哪个
			if( ((r1x*X0+r1y*Y0)>=0 && ((Sx2-Sx1)*X0+(Sy2-Sy1)*Y0)<=0) || ((r1x*X1+r1y*Y1)<0 && ((Sx2-Sx1)*X0+(Sy2-Sy1)*Y0)>0) )
							{
							Sx=Sx2;
							Sy=Sy2;
							}
			else if( ((r1x*X0+r1y*Y0)>=0 && ((Sx2-Sx1)*X0+(Sy2-Sy1)*Y0)>0) || ((r1x*X0+r1y*Y0)<0 && ((Sx2-Sx1)*X0+(Sy2-Sy1)*Y0)<=0) )
							{
							Sx=Sx1;
							Sy=Sy1;
							}



			//坐标转换
			cs1[i]= cs[j];



			cs1[i].X = Sx - Start_X;
			cs1[i].Y = Sy - Start_Y;//第一段直线的终点

			cs2[i]= cs[j];//下表面对应点
			i++;
			
			Start_X = Sx - X0;	
			Start_Y = Sy - Y0;//转接点相对于第二段G代码起点的相对坐标,为求下一个拐点的相对坐标做准备;
		}//缩短型结束

		else if( r0x == -(r1x) && r0y == -(r1y) && cs[j+1].G_pathmode==2)// 不相交型
		{
			printf("3.4\n");
			S1x= X0+r0x;	S1y=Y0+r0y; 
			S2x= X0+r1x;	S2y=Y0+r1y;
			//存入插入的圆弧+转换坐标
			cs1[i]= cs[j];
			cs1[i].X = S1x - Start_X;
			cs1[i].Y = S1y - Start_Y;//第一段直线的终点
			cs2[i]= cs[j];//下表面对应点
			i++;

			cs1[i].X = S2x - S1x;
			cs1[i].Y = S2y - S1y;//插入圆弧线的终点坐标
			cs1[i].I = -(r0x);
			cs1[i].J = -(r0y);//插入圆弧的圆心坐标
			if(cs[j+1].G_pathmode == 2) 
				cs1[i].G_pathmode = 3;
			else if(cs[j+1].G_pathmode == 3)
				cs1[i].G_pathmode = 2;
			cs2[i]= cs0[0];//下表面对应点
			i++;

			Start_X = r1x;	
			Start_Y = r1y;//转接点相对于第二段G代码起点的相对坐标,为求下一个拐点的相对坐标做准备;
		}//不相交型结束
	}//直线接圆弧结束




//进刀之后的情况
for(j=1;j<N-1;j++)
{
	if(cs[j].G_pathmode == 1 && cs[j+1].G_pathmode ==1)//直线接直线
	{
		printf("1\n");
		X0 = cs[j].X;
		Y0 = cs[j].Y;
		X1 = cs[j+1].X; 
		Y1 = cs[j+1].Y;
		//开始求偏移矢量(右偏)
		d0 = sqrt(X0*X0 +Y0*Y0);	
		d1 = sqrt(X1*X1 +Y1*Y1);
		r0x = R*Y0/d0;
		r0y = (-R)*X0/d0;
		r1x = R*Y1/d1;
		r1y = (-R)*X1/d1;
		//判断交接类型
		if( ( (r1x-r0x)*X0 + (r1y-r0y)*Y0 )<0 )//缩短型
		{
			//求上表面对应的点
			if(X0 == 0)
			{
				S1x=X0 + r0x;S1y=Y0 + r0y;
				S2x=X0 + r1x;S2y=Y0 + r1y;K2=Y1/X1;
				Sx =S1x;
				Sy =K2*(Sx - S2x) + S2y;
			}
			else if(X1 ==0)
			{
				S1x=X0 + r0x;S1y=Y0 + r0y;K1=Y0/X0;
				S2x=X0 + r1x;S2y=Y0 + r1y;
				Sx =S2x;
				Sy =K1*(Sx - S1x) + S1y;
			}
			else
			{
				S1x=X0 + r0x;S1y=Y0 + r0y;K1=Y0/X0;
				S2x=X0 + r1x;S2y=Y0 + r1y;K2=Y1/X1;
				Sx = (S2y - S1y - K2*S2x + K1*S1x)/(K1 - K2);
				Sy = K1*(Sx - S1x) + S1y;
			}
			//转换坐标
			/*cs1[i]= cs[j];
			cs1[i].X = Sx - Start_X;
			cs1[i].Y = Sy - Start_Y;
			cs2[i]= cs[j];
			i++;
			Start_X = Sx - X0;	
			Start_Y = Sy - Y0;*/
			cs1[i]= cs[j];
			cs1[i].X = Sx - Start_X;
			cs1[i].Y = Sy - Start_Y;
			
			
			cs2[i]= cs[j];
			cs2[i].X= -r0x + Sx - Start_X2;
			cs2[i].Y= -r0y + Sy - Start_Y2;
			
			i++;
			//圆弧过渡
			cs1[i]=cs0[0];
			cs2[i].G_pathmode = 2;
			cs2[i].X= r0x - r1x;
			cs2[i].Y= r0y - r1y;
			cs2[i].I=  r0x;
			cs2[i].J=  r0y;
			i++;

			Start_X = Sx - X0;	
			Start_Y = Sy - Y0;
			Start_X2 = -r1x + Start_X;	
			Start_Y2 = -r1y + Start_Y;




		}
		else //伸长型
		{
			//求上表面对应的点
			if(X0 == 0)
			{
				S1x=X0 + r0x;S1y=Y0 + r0y;
				S2x=X0 + r1x;S2y=Y0 + r1y;K2=Y1/X1;
				Sx =S1x;
				Sy =K2*(Sx - S2x) + S2y;
			}
			else if(X1 ==0)
			{
				S1x=X0 + r0x;S1y=Y0 + r0y;K1=Y0/X0;
				S2x=X0 + r1x;S2y=Y0 + r1y;
				Sx =S2x;
				Sy =K1*(Sx - S1x) + S1y;
			}
			else
			{
				S1x=X0 + r0x;S1y=Y0 + r0y;K1=Y0/X0;
				S2x=X0 + r1x;S2y=Y0 + r1y;K2=Y1/X1;
				Sx = (S2y - S1y - K2*S2x + K1*S1x)/(K1 - K2);
				Sy = K1*(Sx - S1x) + S1y;
			}
			//转换坐标
			cs1[i]= cs[j];
			cs1[i].X = S1x - Start_X;
			cs1[i].Y = S1y - Start_Y;
			cs2[i]= cs[j];
			cs2[i].X = cs[j].X - Start_X2;
			cs2[i].Y = cs[j].Y - Start_Y2;
			i++;

			//圆弧过渡
			
			cs1[i].G_pathmode = 3;
			cs1[i].X= r1x - r0x;
			cs1[i].Y= r1y - r0y;
			cs1[i].I=  -r0x;
			cs1[i].J=  -r0y;

			cs2[i]=cs0[0];
			i++;

			Start_X = S2x - X0;	
			Start_Y = S2y - Y0;
			Start_X2 = 0;	
			Start_Y2 = 0;

		}
	}//直线接直线结束
else if(cs[j+1].G_pathmode == 1 && (cs[j].G_pathmode ==3 || cs[j].G_pathmode ==2))//圆弧接直线
{
	printf("2\n");

		X0 = cs[j].X;
		Y0 = cs[j].Y;
		I_one=cs[j].I;
		J_one=cs[j].J;

		X1 = cs[j+1].X;
		Y1 = cs[j+1].Y;

		d0 = sqrt((X0-I_one)*(X0-I_one)+(Y0-J_one)*(Y0-J_one));	
		d1 = sqrt(X1*X1 +Y1*Y1);
		printf("%f,%f\n",d0,d1);
		if(cs[j].G_pathmode == 2)//圆弧顺时针
		{
		    r0x = (-R)*(X0-I_one)/d0;
			r0y = (-R)*(Y0-J_one)/d0;
			printf("%f,%f\n",r0x,r0y);
		}
		else if (cs[j].G_pathmode == 3)//圆弧逆时针
		{
			r0x = (R)*(X0-I_one)/d0;
			r0y = (R)*(Y0-J_one)/d0;
		}

		r1x = R*Y1/d1;
		r1y = (-R)*X1/d1;
		printf("%f,%f\n",r1x,r1y);
		//确定KB，即圆弧终点的切线斜率
		if(cs[j].G_pathmode == 2)
		{
			KBx0 = Y0-J_one;
			KBy0 = -(X0-I_one);
		}
		else if(cs[j].G_pathmode == 3)
		{
			KBx0 = -(Y0-J_one);
			KBy0 =  (X0-I_one);
		}// 确定KB参量结束;
		//判断交接类型，求拐点对应的坐标，并储存
		if( (KBx0*(r1x-r0x)+KBy0*(r1y-r0y))>0 )//伸长型
		{

			printf("2.1\n");
			if(KBx0 == 0 && X1 != 0)
			{
			S1x= X0+r0x;	S1y=Y0+r0y; 
			S2x= X0+r1x;	S2y=Y0+r1y; K2=Y1/X1;
							
			Sx = S1x;
			Sy = K2*(Sx - S2x) + S2y;//直线斜率无限大时;					
			}
			else if(KBx0 != 0 && X1 == 0)
			{
			S1x= X0+r0x;	S1y=Y0+r0y;  K1=KBy0/KBx0; 
			S2x= X0+r1x;	S2y=Y0+r1y;
							
			Sx = S2x;
			Sy = K1*(Sx - S1x) + S1y;//直线斜率无限大时;					
			}
			
			else
			{
			S1x= X0+r0x;	S1y=Y0+r0y;  K1=KBy0/KBx0; 
			S2x= X0+r1x;	S2y=Y0+r1y; K2=Y1/X1;
							
			Sx = (S2y - S1y - K2*S2x + K1*S1x)/(K1 - K2);
			Sy = K1*(Sx - S1x) + S1y;
			}//伸长线的交点坐标;
			//转换坐标
			/*cs1[i]= cs[j];
			cs1[i].X = S1x - Start_X;
			cs1[i].Y = S1y - Start_Y;//第一段圆弧的终点

			cs1[i].I = I_one - Start_X;
			cs1[i].J = J_one - Start_Y;//圆弧的圆心坐标

			cs2[i]= cs[j];//下表面对应点
			i++;

			cs1[i].X = Sx - S1x;
			cs1[i].Y = Sy - S1y;//圆弧切线的伸长部分
			cs1[i].G_pathmode = 1;

			cs2[i]= cs0[0];//下表面对应点,多家一个零点，只是为了和上表面元素个数可以对应。
			i++;

			Start_X = Sx - X0;	
			Start_Y = Sy - Y0;//转接点相对于第二段G代码起点的相对坐标,为求下一个拐点的相对坐标做准备;
			*/
			cs1[i]= cs[j];
			cs1[i].X = S1x - Start_X;
			cs1[i].Y = S1y - Start_Y;//第一段圆弧的终点

			cs1[i].I = I_one - Start_X;
			cs1[i].J = J_one - Start_Y;//圆弧的圆心坐标

			cs2[i]= cs[j];
			cs2[i].X = cs[j].X - Start_X2;
			cs2[i].Y = cs[j].Y - Start_Y2;
			cs2[i].I= cs[j].I - Start_X2;
			cs2[i].J= cs[j].J - Start_Y2;
			i++;

		
			cs1[i].G_pathmode = 3;
			cs1[i].X = r1x-r0x;
			cs1[i].Y = r1y-r0y;
			cs1[i].I = -r0x;
			cs1[i].J = -r0y;
			cs2[i]= cs0[0];//下表面对应点
			i++;
			Start_X = S2x - X0;	
			Start_Y = S2y - Y0;//转接点相对于第二段G代码起点的相对坐标,为求下一个拐点的相对坐标做准备;
			Start_X2 = 0;	
			Start_Y2 = 0;


		  }
			//伸长型结束
		else if  ( r0x == r1x && r0y == r1y )// 相切型
			{
			printf("2.2\n");
			Sx = X0+r0x;
			Sy = Y0+r0y;
			cs1[i]= cs[j];
            cs1[i].X = Sx - Start_X;
			cs1[i].Y = Sy - Start_Y;//第一段圆弧的终点

			cs1[i].I = I_one - Start_X;
			cs1[i].J = J_one - Start_Y;//圆弧的圆心坐标

			cs2[i]= cs[j];//下表面对应点
			i++;
			
			Start_X = Sx - X0;	
			Start_Y = Sy - Y0;//转接点相对于第二段G代码起点的相对坐标,为求下一个拐点的相对坐标做准备;
			}//相切型结束;
		else if ( (KBx0*(r1x-r0x)+KBy0*(r1y-r0y))<0 || (r0x == -(r1x) && r0y == -(r1y)&&cs[j].G_pathmode == 3))// 缩短型
		{
			printf("2.3\n");
			S2x=X0+r1x;	S2y=Y0+r1y;
			S3x=X0+X1+r1x;	S3y=Y0+Y1+r1y;//与圆弧相交的直线的两个点;
			printf("%f,%f,%f,%f\n",S2x,S2y,S3x,S3y);

			Ox=I_one;	Oy=J_one;//圆心坐标;
			r_arc=sqrt( (X0+r0x-I_one)*(X0+r0x-I_one) + (Y0+r0y-J_one)*(Y0+r0y-J_one) );//圆弧半径
			printf("%f\n",r_arc);
			if(S2x == S3x)
			{
			Sx1=S2x ;	Sy1=Oy + sqrt(r_arc*r_arc-(S2x-Ox)*(S2x-Ox));
			Sx2=S2x ;	Sy2=Oy - sqrt(r_arc*r_arc-(S2x-Ox)*(S2x-Ox));
			}
			else if(S2y == S3y)
			{
			Sx1=Ox + sqrt(r_arc*r_arc-(S2y-Oy)*(S2y-Oy));	Sy1=S2y;
			Sx2=Ox - sqrt(r_arc*r_arc-(S2y-Oy)*(S2y-Oy));	Sy2=S2y;
			}
			else
			{
			K_arc=(S3y-S2y)/(S3x-S2x);
			a_arc=1+K_arc*K_arc;
			b_arc=2*K_arc*(S2y-Oy-K_arc*S2x-Ox/K_arc);
			c_arc=(S2y-Oy-K_arc*S2x)*(S2y-Oy-K_arc*S2x)-r_arc*r_arc+Ox*Ox;
							
			Sx1=( -b_arc+sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);	Sy1=S2y + K_arc*(Sx1-S2x);
			Sx2=( -b_arc-sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);	Sy2=S2y + K_arc*(Sx2-S2x);
			}//交点坐标;
			//判断是两个交点中的哪个
			if( ((r0x*X1+r0y*Y1)>=0 && ((Sx2-Sx1)*X1+(Sy2-Sy1)*Y1)<=0) || ((r0x*X1+r0y*Y1)<0 && ((Sx2-Sx1)*X1+(Sy2-Sy1)*Y1)>0) )
							{
							Sx=Sx2;
							Sy=Sy2;
							}
			else if( ((r0x*X1+r0y*Y1)>=0 && ((Sx2-Sx1)*X1+(Sy2-Sy1)*Y1)>0) || ((r0x*X1+r0y*Y1)<0 && ((Sx2-Sx1)*X1+(Sy2-Sy1)*Y1)<=0) )
							{
							Sx=Sx1;
							Sy=Sy1;
							}



			//坐标转换
			/*cs1[i]= cs[j];



			cs1[i].X = Sx - Start_X;
			cs1[i].Y = Sy - Start_Y;//第一段圆弧的终点

			cs1[i].I = I_one - Start_X;
			cs1[i].J = J_one - Start_Y;//圆弧的圆心坐标

			cs2[i]= cs[j];//下表面对应点
			i++;
			*/
			cs1[i]= cs[j];
			cs1[i].X = Sx - Start_X;
			cs1[i].Y = Sy - Start_Y;
			cs1[i].I = I_one - Start_X;
			cs1[i].J = J_one - Start_Y;//圆弧的圆心坐标
			cs2[i]= cs[j];
			cs2[i].X= -r0x + Sx - Start_X2;
			cs2[i].Y= -r0y + Sy - Start_Y2; 
			cs2[i].I= cs[j].I - Start_X2;
			cs2[i].J= cs[j].J - Start_Y2;
			i++;
			//圆弧过渡
			cs1[i]=cs0[0];
			cs2[i].G_pathmode=2;
			cs2[i].X= r0x - r1x;
			cs2[i].Y= r0y - r1y;
			cs2[i].I=  r0x;
			cs2[i].J=  r0y;
			i++;

			

			Start_X = Sx - X0;	
			Start_Y = Sy - Y0;// 注意：转接点的求法仍然是相对于参考面上原来的点。因为后一条线的端点是一这个点为坐标原点的。转接点相对于第二段G代码起点的相对坐标,为求下一个拐点的相对坐标做准备;
			Start_X2 = -r1x + Start_X;	
			Start_Y2 = -r1y + Start_Y;
		}//缩短型结束

		else if( r0x == -(r1x) && r0y == -(r1y) && cs[j].G_pathmode == 2 )// 不相交型
		{
			printf("2.4\n");
			S1x= X0+r0x;	S1y=Y0+r0y; 
			S2x= X0+r1x;	S2y=Y0+r1y;
			//存入插入的圆弧+转换坐标
			cs1[i]= cs[j];
			cs1[i].X = S1x - Start_X;
			cs1[i].Y = S1y - Start_Y;//第一段圆弧的终点

			cs1[i].I = I_one - Start_X;
			cs1[i].J = J_one - Start_Y;//圆弧的圆心坐标

			cs2[i]= cs[j];//下表面对应点
			i++;
			cs1[i].X = S2x - S1x;
			cs1[i].Y = S2y - S1y;//插入圆弧线的终点坐标
			cs1[i].I = -(r0x);
			cs1[i].J = -(r0y);//插入圆弧的圆心坐标
			if(cs[j].G_pathmode == 2) 
				cs1[i].G_pathmode = 3;
			else if(cs[j].G_pathmode == 3)
				cs1[i].G_pathmode = 2;

			cs2[i]= cs0[0];//下表面对应点
			i++;

			Start_X = r1x;	
			Start_Y = r1y;//转接点相对于第二段G代码起点的相对坐标,为求下一个拐点的相对坐标做准备;
		}//不相交型结束
	}//圆弧接直线结束
	
		else if(cs[j].G_pathmode == 1 && (cs[j+1].G_pathmode ==3 || cs[j+1].G_pathmode ==2))//直线接圆弧
	{
			printf("3\n");
		X0 = cs[j].X;
		Y0 = cs[j].Y;

		I_two=cs[j+1].I;
		J_two=cs[j+1].J;

		X1 = cs[j+1].X;
		Y1 = cs[j+1].Y;

		d0 = sqrt(X0*X0 + Y0*Y0);
		d1 = sqrt((X1-I_two)*(X1-I_two)+(Y1-J_two)*(Y1-J_two));	

		r0x = R*Y0/d0;
		r0y = (-R)*X0/d0;		
		if(cs[j+1].G_pathmode == 2)//圆弧顺时针
		{
		    r1x = (-R)*(-I_two)/d1;
			r1y = (-R)*(-J_two)/d1;
		}
		else if (cs[j+1].G_pathmode == 3)//圆弧逆时针
		{
			r1x = R*(-I_two)/d1;
			r1y = R*(-J_two)/d1;
		}

	
		//确定KB，即第二段圆弧起点的切线斜率
		if(cs[j+1].G_pathmode == 2)
		{
			KBx1 = -J_two;
			KBy1 =  I_two;
		}
		else if(cs[j+1].G_pathmode == 3)
		{
			KBx1 =  J_two;
			KBy1 = -I_two;
		}// 确定KB参量结束;

		//判断交接类型，求拐点对应的坐标，并储存
		if( (X0*(r1x-r0x)+Y0*(r1y-r0y))>0  )//伸长型
		{
			printf("3.1\n");
			if(KBx1 == 0 && X0 != 0)
			{
			S1x= X0+r0x;	S1y=Y0+r0y;  K1=Y0/X0;
			S2x= X0+r1x;	S2y=Y0+r1y; 
							
			Sx = S2x;
			Sy = K1*(Sx - S1x) + S1y;//直线斜率无限大时;					
			}
			else if(KBx1 != 0 && X0 == 0)
			{
			S1x= X0+r0x;	S1y=Y0+r0y;   
			S2x= X0+r1x;	S2y=Y0+r1y;	 K2=KBy1/KBx1;
							
			Sx = S1x;
			Sy = K2*(Sx - S2x) + S2y;//直线斜率无限大时;					
			}
			
			else
			{
				S1x= X0+r0x;	S1y=Y0+r0y;  K1=Y0/X0;
				S2x= X0+r1x;	S2y=Y0+r1y;	 K2=KBy1/KBx1;
			
							
			Sx = (S2y - S1y - K2*S2x + K1*S1x)/(K1 - K2);
			Sy = K1*(Sx - S1x) + S1y;
			}//伸长线的交点坐标;
			//转换坐标
			/*cs1[i]= cs[j];
			cs1[i].X = Sx - Start_X;
			cs1[i].Y = Sy - Start_Y;//第一段直线的终点
			cs2[i]= cs[j];//下表面对应点
			i++;

			cs1[i].X = S2x - Sx;
			cs1[i].Y = S2y - Sy;//圆弧切线的伸长部分
			cs1[i].G_pathmode = 1;
			cs2[i]= cs0[0];//下表面对应点
			i++;*/
            cs1[i]= cs[j];
			cs1[i].X = S1x - Start_X;
			cs1[i].Y = S1y - Start_Y;
			cs2[i]= cs[j];
			cs2[i].X = cs[j].X - Start_X2;
			cs2[i].Y = cs[j].Y - Start_Y2;
			i++;

			//圆弧过渡
			
			cs1[i].G_pathmode = 3;
			cs1[i].X= r1x - r0x;
			cs1[i].Y= r1y - r0y;
			cs1[i].I=  -r0x;
			cs1[i].J=  -r0y;

			cs2[i]=cs0[0];
			i++;


			Start_X = S2x - X0;	
			Start_Y = S2y - Y0;//转接点相对于第二段G代码起点的相对坐标,为求下一个拐点的相对坐标做准备;
			Start_X2 = 0;	
			Start_Y2 = 0;
		}//伸长型结束
		else if ( (r0x == r1x) && (r0y == r1y) )//相切型
			{
			printf("3.2\n");
			Sx = X0+r0x;
			Sy = Y0+r0y;
			cs1[i]= cs[j];
            cs1[i].X = Sx - Start_X;
			cs1[i].Y = Sy - Start_Y;//第一段直线的终点
			cs2[i]= cs[j];//下表面对应点
			i++;
			printf("1233\n");
			Start_X = Sx - X0;	
			Start_Y = Sy - Y0;//转接点相对于第二段G代码起点的相对坐标,为求下一个拐点的相对坐标做准备;
			}//相切型结束;


		else if ( (X0*(r1x-r0x)+Y0*(r1y-r0y) )<0||(cs[j+1].G_pathmode==3 && r0x == -(r1x) && r0y == -(r1y) ) )// 缩短型
		{
			printf("3.3\n");
			S1x=X0+r0x;	S1y=Y0+r0y;
			S3x=r0x;	S3y=r0y;//与圆弧相交的直线的两个点;

			Ox=X0+I_two;	Oy=Y0+J_two;//圆心坐标;
			r_arc=sqrt( (X0+r1x-Ox)*(X0+r1x-Ox) + (Y0+r1y-Oy)*(Y0+r1y-Oy) );//圆弧半径
			if(S1x == S3x)
			{
			Sx1=S1x ;	Sy1=Oy + sqrt(r_arc*r_arc-(S1x-Ox)*(S1x-Ox));
			Sx2=S1x ;	Sy2=Oy - sqrt(r_arc*r_arc-(S1x-Ox)*(S1x-Ox));
			}
			else if(S1y == S3y)
			{
			Sx1=Ox + sqrt(r_arc*r_arc-(S1y-Oy)*(S1y-Oy));	Sy1=S1y;
			Sx2=Ox - sqrt(r_arc*r_arc-(S1y-Oy)*(S1y-Oy));	Sy2=S1y;
			}
			else
			{
			K_arc=(S3y-S1y)/(S3x-S1x);
			a_arc=1+K_arc*K_arc;
			b_arc=2*K_arc*(S1y-Oy-K_arc*S1x-Ox/K_arc);
			c_arc=(S1y-Oy-K_arc*S1x)*(S1y-Oy-K_arc*S1x)-r_arc*r_arc+Ox*Ox;
							
			Sx1=( -b_arc+sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);	Sy1=S1y + K_arc*(Sx1-S1x);
			Sx2=( -b_arc-sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);	Sy2=S1y + K_arc*(Sx2-S1x);
			}//交点坐标;
			//判断是两个交点中的哪个
			if( ((r1x*X0+r1y*Y0)>=0 && ((Sx2-Sx1)*X0+(Sy2-Sy1)*Y0)<=0) || ((r1x*X1+r1y*Y1)<0 && ((Sx2-Sx1)*X0+(Sy2-Sy1)*Y0)>0) )
							{
							Sx=Sx2;
							Sy=Sy2;
							}
			else if( ((r1x*X0+r1y*Y0)>=0 && ((Sx2-Sx1)*X0+(Sy2-Sy1)*Y0)>0) || ((r1x*X0+r1y*Y0)<0 && ((Sx2-Sx1)*X0+(Sy2-Sy1)*Y0)<=0) )
							{
							Sx=Sx1;
							Sy=Sy1;
							}



			//坐标转换
			/*cs1[i]= cs[j];



			cs1[i].X = Sx - Start_X;
			cs1[i].Y = Sy - Start_Y;//第一段直线的终点

			cs2[i]= cs[j];//下表面对应点
			i++;*/
			cs1[i]= cs[j];
			cs1[i].X = Sx - Start_X;
			cs1[i].Y = Sy - Start_Y;
			cs2[i]= cs[j];
			cs2[i].X= -r0x + Sx - Start_X2;
			cs2[i].Y= -r0y + Sy - Start_Y2;
			i++;
			//圆弧过渡
			cs1[i]=cs0[0];
			cs2[i].G_pathmode = 2;
			cs2[i].X= r0x - r1x;
			cs2[i].Y= r0y - r1y;
			cs2[i].I=  r0x;
			cs2[i].J=  r0y;
			i++;
			
			Start_X = Sx - X0;	
			Start_Y = Sy - Y0;//转接点相对于第二段G代码起点的相对坐标,为求下一个拐点的相对坐标做准备;
			Start_X2 = -r1x + Start_X;	
			Start_Y2 = -r1y + Start_Y;
		}//缩短型结束

		else if( r0x == -(r1x) && r0y == -(r1y) && cs[j+1].G_pathmode==2)// 不相交型
		{
			printf("3.4\n");
			S1x= X0+r0x;	S1y=Y0+r0y; 
			S2x= X0+r1x;	S2y=Y0+r1y;
			//存入插入的圆弧+转换坐标
			cs1[i]= cs[j];
			cs1[i].X = S1x - Start_X;
			cs1[i].Y = S1y - Start_Y;//第一段直线的终点
			cs2[i]= cs[j];//下表面对应点
			i++;

			cs1[i].X = S2x - S1x;
			cs1[i].Y = S2y - S1y;//插入圆弧线的终点坐标
			cs1[i].I = -(r0x);
			cs1[i].J = -(r0y);//插入圆弧的圆心坐标
			if(cs[j+1].G_pathmode == 2) 
				cs1[i].G_pathmode = 3;
			else if(cs[j+1].G_pathmode == 3)
				cs1[i].G_pathmode = 2;
			cs2[i]= cs0[0];//下表面对应点
			i++;

			Start_X = r1x;	
			Start_Y = r1y;//转接点相对于第二段G代码起点的相对坐标,为求下一个拐点的相对坐标做准备;
		}//不相交型结束
	}//直线接圆弧结束



	else if((cs[j].G_pathmode==2 || cs[j].G_pathmode==3)&& (cs[j+1].G_pathmode==2 || cs[j+1].G_pathmode==3))//圆弧接圆弧
	{
		printf("4\n");
		X0 = cs[j].X;
		Y0 = cs[j].Y;
		I_one=cs[j].I;
		J_one=cs[j].J;

		X1 = cs[j+1].X;
		Y1 = cs[j+1].Y;
		I_two=cs[j+1].I;
		J_two=cs[j+1].J;

		d0 = sqrt((X0-I_one)*(X0-I_one)+(Y0-J_one)*(Y0-J_one));	
		d1 = sqrt((X1-I_two)*(X1-I_two)+(Y1-J_two)*(Y1-J_two));

		if(cs[j].G_pathmode == 2)//圆弧顺时针
		{
		    r0x = (-R)*(X0-I_one)/d0;
			r0y = (-R)*(Y0-J_one)/d0;
		}
		else if (cs[j].G_pathmode == 3)//圆弧逆时针
		{
			r0x = (R)*(X0-I_one)/d0;
			r0y = (R)*(Y0-J_one)/d0;
		}


		if(cs[j+1].G_pathmode == 2)//圆弧顺时针
		{
		    r1x = (-R)*(-I_two)/d1;
			r1y = (-R)*(-J_two)/d1;
		}
		else if (cs[j+1].G_pathmode == 3)//圆弧逆时针
		{
			r1x = R*(-I_two)/d1;
			r1y = R*(-J_two)/d1;
		}
		//确定KB1，KB2
		if(cs[j].G_pathmode == 2)
		{
			KBx0 = Y0-J_one;
			KBy0 = -(X0-I_one);
		}
		else if(cs[j].G_pathmode == 3)
		{
			KBx0 = -(Y0-J_one);
			KBy0 =  (X0-I_one);
		}

		if(cs[j+1].G_pathmode == 2)
		{
			KBx1 = -J_two;
			KBy1 =  I_two;
		}
		else if(cs[j+1].G_pathmode == 3)
		{
			KBx1 =  J_two;
			KBy1 = -I_two;
		}// 确定KB参量结束;
		if( (KBx0*(r1x-r0x)+KBy0*(r1y-r0y))>0  )//伸长型
		{

			printf("4.1\n");
			if(KBx0 == 0 && KBx1 != 0)
			{
			S1x= X0+r0x;	S1y=Y0+r0y; 
			S2x= X0+r1x;	S2y=Y0+r1y; K2=KBy1/KBx1;
							
			Sx = S1x;
			Sy = K2*(Sx - S2x) + S2y;//直线斜率无限大时;					
			}
			else if(KBx0 != 0 && KBx1 == 0)
			{
			S1x= X0+r0x;	S1y=Y0+r0y;  K1=KBy0/KBx0; 
			S2x= X0+r1x;	S2y=Y0+r1y;
							
			Sx = S2x;
			Sy = K1*(Sx - S1x) + S1y;//直线斜率无限大时;					
			}
			else if((KBy0*KBx1) == (KBy1*KBx0))
			{
			S1x= X0+r0x;	S1y=Y0+r0y;  
			S2x= X0+r1x;	S2y=Y0+r1y;
							
			Sx = X0+r0x;
			Sy = Y0+r0y;
			
			}//直线斜率相等时;
			else
			{
			S1x= X0+r0x;	S1y=Y0+r0y;  K1=KBy0/KBx0; 
			S2x= X0+r1x;	S2y=Y0+r1y;  K2=KBy1/KBx1;
							
			Sx = (S2y - S1y - K2*S2x + K1*S1x)/(K1 - K2);
			Sy = K1*(Sx - S1x) + S1y;
			}//伸长线的交点坐标;
			//转换坐标
			/*cs1[i]= cs[j];
			cs1[i].X = S1x - Start_X;
			cs1[i].Y = S1y - Start_Y;//第一段圆弧的终点

			cs1[i].I = I_one - Start_X;
			cs1[i].J = J_one - Start_Y;//圆弧的圆心坐标

			cs2[i]= cs[j];//下表面对应点
			i++;
			cs1[i].X = Sx - S1x;
			cs1[i].Y = Sy - S1y;//圆弧切线的伸长部分
			cs1[i].G_pathmode = 1;

			cs2[i]= cs0[0];//下表面对应点
			i++;
			
			cs1[i].X = S2x - Sx;
			cs1[i].Y = S2y - Sy;//第二段圆弧切线的伸长部分
			cs1[i].G_pathmode = 1;

			cs2[i]= cs0[0];//下表面对应点
			i++;*/

			cs1[i]= cs[j];
			cs1[i].X = S1x - Start_X;
			cs1[i].Y = S1y - Start_Y;//第一段圆弧的终点

			cs1[i].I = I_one - Start_X;
			cs1[i].J = J_one - Start_Y;//圆弧的圆心坐标

			cs2[i]= cs[j];//下表面对应点
			cs2[i].X = cs[j].X - Start_X2;
			cs2[i].Y = cs[j].Y - Start_Y2;
			cs2[i].I= cs[j].I - Start_X2;
			cs2[i].J= cs[j].J - Start_Y2;
			i++;

		
			cs1[i].G_pathmode = 3;
			cs1[i].X = r1x-r0x;
			cs1[i].Y = r1y-r0y;
			cs1[i].I = -r0x;
			cs1[i].J = -r0y;
			cs2[i]= cs0[0];//下表面对应点
			i++;


			Start_X = S2x - X0;	
			Start_Y = S2y - Y0;//转接点相对于第二段G代码起点的相对坐标,为求下一个拐点的相对坐标做准备;
			Start_X2 = 0;	
			Start_Y2 = 0;
		}
			//伸长型结束
		else if  ( r0x == r1x && r0y == r1y )// 相切型
			{
			printf("4.4\n");
			Sx = X0+r0x;
			Sy = Y0+r0y;
			cs1[i]= cs[j];
            cs1[i].X = Sx - Start_X;
			cs1[i].Y = Sy - Start_Y;//第一段圆弧的终点

			cs1[i].I = I_one - Start_X;
			cs1[i].J = J_one - Start_Y;//圆弧的圆心坐标

			cs2[i]= cs[j];//下表面对应点
			i++;
			
			Start_X = Sx - X0;	
			Start_Y = Sy - Y0;//转接点相对于第二段G代码起点的相对坐标,为求下一个拐点的相对坐标做准备;
			}//相切型结束;

		else if ( (KBx0*(r1x-r0x)+KBy0*(r1y-r0y))<0 || (r0x == -(r1x) && r0y == -(r1y)&&cs[j].G_pathmode == 3))// 缩短型
		{
			printf("4.3\n");
			S1x=X0;S1y=Y0;//两圆相交线上的点
			

			Ox=I_one;	Oy=J_one;//圆心坐标;
			r_arc=sqrt( (X0+r0x-I_one)*(X0+r0x-I_one) + (Y0+r0y-J_one)*(Y0+r0y-J_one) );//圆弧半径
			if(X0+I_two-Ox==0)
			{
				S2x=I_one;
				S2y=S1y;
			}
			else if(Y0+J_two-Oy==0)
			{
				S2x=S1x;
				S2y=J_one;
			}
			else
			{
				Kline=(Y0+J_two-Oy)/(X0+I_two-Ox);
				S2y=Kline*(X0-Ox+Oy/Kline+Kline*Y0)/(1+Kline*Kline);
				S2x=(S2y+Oy+Kline*Ox)/Kline;
			}

			if(S1x == S2x)
			{
			Sx1=S1x ;	Sy1=Oy + sqrt(r_arc*r_arc-(S2x-Ox)*(S2x-Ox));
			Sx2=S1x ;	Sy2=Oy - sqrt(r_arc*r_arc-(S2x-Ox)*(S2x-Ox));
			printf("%f,%f\n",Sx1,Sy1);
			}
			else if(S1y == S2y)
			{
			Sx1=Ox + sqrt(r_arc*r_arc-(S2y-Oy)*(S2y-Oy));	Sy1=S2y;
			Sx2=Ox - sqrt(r_arc*r_arc-(S2y-Oy)*(S2y-Oy));	Sy2=S2y;
			}
			else
			{
			K_arc=(S2y-S1y)/(S2x-S1x);
			a_arc=1+K_arc*K_arc;
			b_arc=2*K_arc*(S1y-Oy-K_arc*S1x-Ox/K_arc);
			c_arc=(S1y-Oy-K_arc*S1x)*(S1y-Oy-K_arc*S1x)-r_arc*r_arc+Ox*Ox;
							
			Sx1=( -b_arc+sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);	Sy1=S1y + K_arc*(Sx1-S1x);
			Sx2=( -b_arc-sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);	Sy2=S1y + K_arc*(Sx2-S1x);
			}//交点坐标;



			//判断是两个交点中的哪个
			if( ((r0x*(S2x-S1x)+r1y*(S2y-S1y))>=0 && ((Sx2-Sx1)*(S2x-S1x)+(Sy2-Sy1)*(S2y-S1y))<=0) || ((r1x*(S2x-S1x)+r1y*(S2y-S1y))>0 && ((Sx2-Sx1)*(S2x-S1x)+(Sy2-Sy1)*(S2y-S1y))>0 ))
							{
							Sx=Sx1;
							Sy=Sy1;
							}
			else if( ((r0x*(S2x-S1x)+r1y*(S2y-S1y))>=0 && ((Sx2-Sx1)*(S2x-S1x)+(Sy2-Sy1)*(S2y-S1y))>0) || ((r1x*(S2x-S1x)+r1y*(S2y-S1y))>0 && ((Sx2-Sx1)*(S2x-S1x)+(Sy2-Sy1)*(S2y-S1y))<=0 ))
							{
							Sx=Sx2;
							Sy=Sy2;
							}



			//坐标转换
			/*cs1[i]= cs[j];



			cs1[i].X = Sx - Start_X;
			cs1[i].Y = Sy - Start_Y;//第一段圆弧的终点

			cs1[i].I = I_one - Start_X;
			cs1[i].J = J_one - Start_Y;//圆弧的圆心坐标

			cs2[i]= cs[j];//下表面对应点
			i++;*/

			cs1[i]= cs[j];
			cs1[i].X = Sx - Start_X;
			cs1[i].Y = Sy - Start_Y;
			cs1[i].I = I_one - Start_X;
			cs1[i].J = J_one - Start_Y;//圆弧的圆心坐标
			cs2[i]= cs[j];
			cs2[i].X= -r0x + Sx  - Start_X2;
			cs2[i].Y= -r0y + Sy  - Start_Y2;
			cs2[i].I= cs[j].I - Start_X2;
			cs2[i].J= cs[j].J - Start_Y2;
			i++;
			//圆弧过渡
			cs1[i]=cs0[0];
			cs2[i].G_pathmode=2;
			cs2[i].X= r0x - r1x;
			cs2[i].Y= r0y - r1y;
			cs2[i].I=  r0x;
			cs2[i].J=  r0y;
			i++;
			
			Start_X = Sx - X0;	
			Start_Y = Sy - Y0;//转接点相对于第二段G代码起点的相对坐标,为求下一个拐点的相对坐标做准备;
			Start_X2 = -r1x + Start_X;	
			Start_Y2 = -r1y + Start_Y;
		}//缩短型结束

		else if( r0x == -(r1x) && r0y == -(r1y)&& cs[j].G_pathmode == 2 )// 不相交型
		{
			printf("4.4\n");
			S1x= X0+r0x;	S1y=Y0+r0y; 
			S2x= X0+r1x;	S2y=Y0+r1y;
			//存入插入的圆弧+转换坐标
			cs1[i]= cs[j];
			cs1[i].X = S1x - Start_X;
			cs1[i].Y = S1y - Start_Y;//第一段圆弧的终点

			cs1[i].I = I_one - Start_X;
			cs1[i].J = J_one - Start_Y;//圆弧的圆心坐标

			cs2[i]= cs[j];//下表面对应点
			i++;
			cs1[i].X = S2x - S1x;
			cs1[i].Y = S2y - S1y;//插入圆弧线的终点坐标
			cs1[i].I = -(r0x);
			cs1[i].J = -(r0y);//插入圆弧的圆心坐标
			if(cs[j].G_pathmode == 2) 
				cs1[i].G_pathmode = 3;
			else if(cs[j].G_pathmode == 3)
				cs1[i].G_pathmode = 2;

			cs2[i]= cs0[0];//下表面对应点
			i++;

			Start_X = r1x;	
			Start_Y = r1y;//转接点相对于第二段G代码起点的相对坐标,为求下一个拐点的相对坐标做准备;
		}//不相交型结束
	}//圆弧接圆弧结束			


}//此时j=N-1，


//求开始退刀的那点的上下表面坐标


if(cs[j].G_pathmode == 1 && cs[j+1].G_pathmode ==1)//直线接直线
	{
		printf("1\n");
		X0 = cs[j].X;
		Y0 = cs[j].Y;
		X1 = cs[j+1].X; 
		Y1 = cs[j+1].Y;
		//开始求偏移矢量(右偏)
		d0 = sqrt(X0*X0 +Y0*Y0);	
		d1 = sqrt(X1*X1 +Y1*Y1);
		r0x = R*Y0/d0;
		r0y = (-R)*X0/d0;
		r1x = 0;
		r1y = 0;
		//判断交接类型
		if( ( (r1x-r0x)*X0 + (r1y-r0y)*Y0 )<0 )//缩短型
		{
			//求上表面对应的点
			if(X0 == 0)
			{
				S1x=X0 + r0x;S1y=Y0 + r0y;
				S2x=X0 + r1x;S2y=Y0 + r1y;K2=Y1/X1;
				Sx =S1x;
				Sy =K2*(Sx - S2x) + S2y;
			}
			else if(X1 ==0)
			{
				S1x=X0 + r0x;S1y=Y0 + r0y;K1=Y0/X0;
				S2x=X0 + r1x;S2y=Y0 + r1y;
				Sx =S2x;
				Sy =K1*(Sx - S1x) + S1y;
			}
			else
			{
				S1x=X0 + r0x;S1y=Y0 + r0y;K1=Y0/X0;
				S2x=X0 + r1x;S2y=Y0 + r1y;K2=Y1/X1;
				Sx = (S2y - S1y - K2*S2x + K1*S1x)/(K1 - K2);
				Sy = K1*(Sx - S1x) + S1y;
			}
			//转换坐标
			cs1[i]= cs[j];
			cs1[i].X = Sx - Start_X;
			cs1[i].Y = Sy - Start_Y;
			cs2[i]= cs[j];
			cs2[i].X = cs[j].X - Start_X2;
			cs2[i].Y = cs[j].Y - Start_Y2;
			i++;
			Start_X = Sx - X0;	
			Start_Y = Sy - Y0;
		}
		else //伸长型
		{
			//求上表面对应的点
			if(X0 == 0)
			{
				S1x=X0 + r0x;S1y=Y0 + r0y;
				S2x=X0 + r1x;S2y=Y0 + r1y;K2=Y1/X1;
				Sx =S1x;
				Sy =K2*(Sx - S2x) + S2y;
			}
			else if(X1 ==0)
			{
				S1x=X0 + r0x;S1y=Y0 + r0y;K1=Y0/X0;
				S2x=X0 + r1x;S2y=Y0 + r1y;
				Sx =S2x;
				Sy =K1*(Sx - S1x) + S1y;
			}
			else
			{
				S1x=X0 + r0x;S1y=Y0 + r0y;K1=Y0/X0;
				S2x=X0 + r1x;S2y=Y0 + r1y;K2=Y1/X1;
				Sx = (S2y - S1y - K2*S2x + K1*S1x)/(K1 - K2);
				Sy = K1*(Sx - S1x) + S1y;
			}
			//转换坐标
			cs1[i]= cs[j];
			cs1[i].X = Sx - Start_X;
			cs1[i].Y = Sy - Start_Y;
			cs2[i]= cs[j];
			cs2[i].X = cs[j].X - Start_X2;
			cs2[i].Y = cs[j].Y - Start_Y2;
			i++;
			Start_X = Sx - X0;	
			Start_Y = Sy - Y0;
		}
	}//直线接直线结束

else if(cs[j+1].G_pathmode == 1 && (cs[j].G_pathmode ==3 || cs[j].G_pathmode ==2))//圆弧接直线
{
	printf("2\n");

		X0 = cs[j].X;
		Y0 = cs[j].Y;
		I_one=cs[j].I;
		J_one=cs[j].J;

		X1 = cs[j+1].X;
		Y1 = cs[j+1].Y;

		d0 = sqrt((X0-I_one)*(X0-I_one)+(Y0-J_one)*(Y0-J_one));	
		d1 = sqrt(X1*X1 +Y1*Y1);
		printf("%f,%f\n",d0,d1);
		if(cs[j].G_pathmode == 2)//圆弧顺时针
		{
		    r0x = (-R)*(X0-I_one)/d0;
			r0y = (-R)*(Y0-J_one)/d0;
			printf("%f,%f\n",r0x,r0y);
		}
		else if (cs[j].G_pathmode == 3)//圆弧逆时针
		{
			r0x = (R)*(X0-I_one)/d0;
			r0y = (R)*(Y0-J_one)/d0;
		}

		r1x = 0;
		r1y = 0;
		
		//确定KB，即圆弧终点的切线斜率
		if(cs[j].G_pathmode == 2)
		{
			KBx0 = Y0-J_one;
			KBy0 = -(X0-I_one);
		}
		else if(cs[j].G_pathmode == 3)
		{
			KBx0 = -(Y0-J_one);
			KBy0 =  (X0-I_one);
		}// 确定KB参量结束;
		//判断交接类型，求拐点对应的坐标，并储存
		if( (KBx0*(r1x-r0x)+KBy0*(r1y-r0y))>0 )//伸长型
		{

			printf("2.1\n");
			if(KBx0 == 0 && X1 != 0)
			{
			S1x= X0+r0x;	S1y=Y0+r0y; 
			S2x= X0+r1x;	S2y=Y0+r1y; K2=Y1/X1;
							
			Sx = S1x;
			Sy = K2*(Sx - S2x) + S2y;//直线斜率无限大时;					
			}
			else if(KBx0 != 0 && X1 == 0)
			{
			S1x= X0+r0x;	S1y=Y0+r0y;  K1=KBy0/KBx0; 
			S2x= X0+r1x;	S2y=Y0+r1y;
							
			Sx = S2x;
			Sy = K1*(Sx - S1x) + S1y;//直线斜率无限大时;					
			}
			
			else
			{
			S1x= X0+r0x;	S1y=Y0+r0y;  K1=KBy0/KBx0; 
			S2x= X0+r1x;	S2y=Y0+r1y; K2=Y1/X1;
							
			Sx = (S2y - S1y - K2*S2x + K1*S1x)/(K1 - K2);
			Sy = K1*(Sx - S1x) + S1y;
			}//伸长线的交点坐标;
			//转换坐标
			cs1[i]= cs[j];
			cs1[i].X = S1x - Start_X;
			cs1[i].Y = S1y - Start_Y;//第一段圆弧的终点

			cs1[i].I = I_one - Start_X;
			cs1[i].J = J_one - Start_Y;//圆弧的圆心坐标

			cs2[i]= cs[j];//下表面对应点
			cs2[i].X = cs[j].X - Start_X2;
			cs2[i].Y = cs[j].Y - Start_Y2;
			
			i++;

			cs1[i].X = Sx - S1x;
			cs1[i].Y = Sy - S1y;//圆弧切线的伸长部分
			cs1[i].G_pathmode = 1;

			cs2[i]= cs0[0];//下表面对应点,多家一个零点，只是为了和上表面元素个数可以对应。
			i++;

			Start_X = Sx - X0;	
			Start_Y = Sy - Y0;//转接点相对于第二段G代码起点的相对坐标,为求下一个拐点的相对坐标做准备;
		}
			//伸长型结束
		else if  ( r0x == r1x && r0y == r1y )// 相切型
			{
			printf("2.2\n");
			Sx = X0+r0x;
			Sy = Y0+r0y;
			cs1[i]= cs[j];
            cs1[i].X = Sx - Start_X;
			cs1[i].Y = Sy - Start_Y;//第一段圆弧的终点

			cs1[i].I = I_one - Start_X;
			cs1[i].J = J_one - Start_Y;//圆弧的圆心坐标

			cs2[i]= cs[j];//下表面对应点
			i++;
			
			Start_X = Sx - X0;	
			Start_Y = Sy - Y0;//转接点相对于第二段G代码起点的相对坐标,为求下一个拐点的相对坐标做准备;
			}//相切型结束;
		else if ( (KBx0*(r1x-r0x)+KBy0*(r1y-r0y))<0 || (r0x == -(r1x) && r0y == -(r1y)&&cs[j].G_pathmode == 3))// 缩短型
		{
			printf("2.3\n");
			S2x=X0+r1x;	S2y=Y0+r1y;
			S3x=X0+X1+r1x;	S3y=Y0+Y1+r1y;//与圆弧相交的直线的两个点;
			printf("%f,%f,%f,%f\n",S2x,S2y,S3x,S3y);

			Ox=I_one;	Oy=J_one;//圆心坐标;
			r_arc=sqrt( (X0+r0x-I_one)*(X0+r0x-I_one) + (Y0+r0y-J_one)*(Y0+r0y-J_one) );//圆弧半径
			printf("%f\n",r_arc);
			if(S2x == S3x)
			{
			Sx1=S2x ;	Sy1=Oy + sqrt(r_arc*r_arc-(S2x-Ox)*(S2x-Ox));
			Sx2=S2x ;	Sy2=Oy - sqrt(r_arc*r_arc-(S2x-Ox)*(S2x-Ox));
			}
			else if(S2y == S3y)
			{
			Sx1=Ox + sqrt(r_arc*r_arc-(S2y-Oy)*(S2y-Oy));	Sy1=S2y;
			Sx2=Ox - sqrt(r_arc*r_arc-(S2y-Oy)*(S2y-Oy));	Sy2=S2y;
			}
			else
			{
			K_arc=(S3y-S2y)/(S3x-S2x);
			a_arc=1+K_arc*K_arc;
			b_arc=2*K_arc*(S2y-Oy-K_arc*S2x-Ox/K_arc);
			c_arc=(S2y-Oy-K_arc*S2x)*(S2y-Oy-K_arc*S2x)-r_arc*r_arc+Ox*Ox;
							
			Sx1=( -b_arc+sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);	Sy1=S2y + K_arc*(Sx1-S2x);
			Sx2=( -b_arc-sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);	Sy2=S2y + K_arc*(Sx2-S2x);
			}//交点坐标;
			//判断是两个交点中的哪个
			if( ((r0x*X1+r0y*Y1)>=0 && ((Sx2-Sx1)*X1+(Sy2-Sy1)*Y1)<=0) || ((r0x*X1+r0y*Y1)<0 && ((Sx2-Sx1)*X1+(Sy2-Sy1)*Y1)>0) )
							{
							Sx=Sx2;
							Sy=Sy2;
							}
			else if( ((r0x*X1+r0y*Y1)>=0 && ((Sx2-Sx1)*X1+(Sy2-Sy1)*Y1)>0) || ((r0x*X1+r0y*Y1)<0 && ((Sx2-Sx1)*X1+(Sy2-Sy1)*Y1)<=0) )
							{
							Sx=Sx1;
							Sy=Sy1;
							}



			//坐标转换
			cs1[i]= cs[j];



			cs1[i].X = Sx - Start_X;
			cs1[i].Y = Sy - Start_Y;//第一段圆弧的终点

			cs1[i].I = I_one - Start_X;
			cs1[i].J = J_one - Start_Y;//圆弧的圆心坐标

			cs2[i]= cs[j];//下表面对应点
			cs2[i].X= cs[j].X - Start_X2;
			cs2[i].Y= cs[j].Y - Start_Y2;
			cs2[i].I= cs[j].I - Start_X2;
			cs2[i].J= cs[j].J - Start_Y2;
			i++;
			
			Start_X = Sx - X0;	
			Start_Y = Sy - Y0;//转接点相对于第二段G代码起点的相对坐标,为求下一个拐点的相对坐标做准备;
		}//缩短型结束

		else if( r0x == -(r1x) && r0y == -(r1y) && cs[j].G_pathmode == 2 )// 不相交型
		{
			printf("2.4\n");
			S1x= X0+r0x;	S1y=Y0+r0y; 
			S2x= X0+r1x;	S2y=Y0+r1y;
			//存入插入的圆弧+转换坐标
			cs1[i]= cs[j];
			cs1[i].X = S1x - Start_X;
			cs1[i].Y = S1y - Start_Y;//第一段圆弧的终点

			cs1[i].I = I_one - Start_X;
			cs1[i].J = J_one - Start_Y;//圆弧的圆心坐标

			cs2[i]= cs[j];//下表面对应点

			i++;
			cs1[i].X = S2x - S1x;
			cs1[i].Y = S2y - S1y;//插入圆弧线的终点坐标
			cs1[i].I = -(r0x);
			cs1[i].J = -(r0y);//插入圆弧的圆心坐标
			if(cs[j].G_pathmode == 2) 
				cs1[i].G_pathmode = 3;
			else if(cs[j].G_pathmode == 3)
				cs1[i].G_pathmode = 2;

			cs2[i]= cs0[0];//下表面对应点
			i++;

			Start_X = r1x;	
			Start_Y = r1y;//转接点相对于第二段G代码起点的相对坐标,为求下一个拐点的相对坐标做准备;
		}//不相交型结束
	}//圆弧接直线结束



//补充退刀的那一段

		cs1[i]= cs[j+1]; // 最后一段是退刀
		cs1[i].X = cs[j+1].X - Start_X;
		cs1[i].Y = cs[j+1].Y - Start_Y;

		cs2[i]= cs[j+1];//下表面对应点  最终电极丝变垂直。


		
		
		
//开始轨迹密化

for(j=0;j<i+1;j++)
{
if (cs1[j].G_pathmode == 1 && cs2[j].G_pathmode == 1)//直线对直线
{
	printf("lineline\n");
	Xtop = 	cs1[j].X;
	Ytop = 	cs1[j].Y;
    Xbtm = 	cs2[j].X;
	Ybtm = 	cs2[j].Y;
	lineline(Xtop,Ytop,Xbtm,Ybtm,tmp1,tmp2,&top,&btm); //直线对直线的函数，功能是将上下两表面的对应线段密化；
	// 此时，tmp1、tmp2中存有密化的数据，top，btm 为密化的点数。
	for(m=0;m<=(top>btm?top:btm);d++,m++)
	{
		m1[d].X=tmp1[m].X;
		m1[d].Y=tmp1[m].Y;
		m2[d].X=tmp2[m].X;
		m2[d].Y=tmp2[m].Y;
	}

}

else if((cs1[j].G_pathmode == 2 &&  cs2[j].G_pathmode == 2)|| (cs2[j].G_pathmode == 3&& cs2[j].G_pathmode == 3))//圆弧对圆弧
{
	printf("arcarc\n");
	//输入上下圆弧的起点和终点数据
	Xtopstart=-cs1[j].I;
	Ytopstart=-cs1[j].J;
	Xtopend=cs1[j].X-cs1[j].I;
	Ytopend=cs1[j].Y-cs1[j].J;
	Xbtmstart=-cs2[j].I;
	Ybtmstart=-cs2[j].J;
	Xbtmend=cs2[j].X-cs2[j].I;
	Ybtmend=cs2[j].Y-cs2[j].J;

	arcarc( Xtopstart,Ytopstart, Xtopend, Ytopend,
		    Xbtmstart,Ybtmstart, Xbtmend, Ybtmend,
			tmp1,tmp2,&top,&btm
		  );//调用函数
	
	for(m=0;m<=(top>btm?top:btm);d++,m++)//结构体不输入值是就是零吗？
	{
		m1[d].X=tmp1[m].X;
		m1[d].Y=tmp1[m].Y;
		m2[d].X=tmp2[m].X;
		m2[d].Y=tmp2[m].Y;
	}
	
}
else if(cs1[j].G_pathmode == 1 && cs2[j].G_pathmode == 0)//直线对点
{
	printf("linepoint\n");
	Xtop = 	cs1[j].X;
	Ytop = 	cs1[j].Y;
	line(Xtop,Ytop,0.02,tmp1,&top);
	for(m=0;m<=top;d++,m++)
	{
		m1[d].X=tmp1[m].X;
		m1[d].Y=tmp1[m].Y;
		m2[d].X=0;
		m2[d].Y=0;
	}
}
else if( (cs1[j].G_pathmode == 3)&& cs2[j].G_pathmode == 0)//圆弧对点
{
	printf("arcpoint\n"); 
	Xtopstart=-cs1[j].I;
	Ytopstart=-cs1[j].J;
	Xtopend=cs1[j].X-cs1[j].I;
	Ytopend=cs1[j].Y-cs1[j].J;
	Rtoptop = sqrt(Xtopstart*Xtopstart + Ytopstart*Ytopstart);
	deltl = sqrt(8*Rtoptop*0.001);
	arc1(Xtopstart,Ytopstart,Xtopend,Ytopend,Rtoptop,deltl,tmp1,&top);
	for(m=0;m<=top;d++,m++)
	{
		m1[d].X=tmp1[m].X;
		m1[d].Y=tmp1[m].Y;
		m2[d].X=0;
		m2[d].Y=0;
	}
}

else if(cs1[j].G_pathmode == 0 &&  cs2[j].G_pathmode == 2 )//点对圆弧
{
	printf("pointarc\n");
	Xbtmstart=-cs2[j].I;
	Ybtmstart=-cs2[j].J;
	Xbtmend=cs2[j].X-cs2[j].I;
	Ybtmend=cs2[j].Y-cs2[j].J;
	Rbtmbtm = sqrt(Xbtmstart*Xbtmstart + Ybtmstart*Ybtmstart);
	deltl = sqrt(8*Rbtmbtm*0.0002);
	arc(Xbtmstart,Ybtmstart,Xbtmend,Ybtmend,Rbtmbtm,deltl,tmp2,&btm);
	for(m=0;m<=btm;d++,m++)
	{
		m2[d].X=tmp2[m].X;
		m2[d].Y=tmp2[m].Y;
		m1[d].X=0;
		m1[d].Y=0;
	}


}

}


//密化完毕。数据存在 m1[]、m2[]中。 



struct touying
{
	double X,Y,U,V;
} ; 
struct touying ty[2000];
struct touying tydata[2000];
int k;
double h1=1,h2=1,l=3;//分别为下表面距离UV的距离、工件的高度、上下丝架面的距离。
double x1[2000],y1[2000],x2[2000],y2[2000];
x1[0]=0;
y1[0]=0;
x2[0]=0;
y2[0]=0;
tydata[0].X = (h1+h2)*m2[0].X / h2 - h1*m1[0].X / h2;
tydata[0].Y = (h1+h2)*m2[0].Y / h2 - h1*m1[0].Y / h2;
tydata[0].U = l*(m1[0].X - m2[0].X)/h2 + tydata[0].X;
tydata[0].V = l*(m1[0].Y - m2[0].Y)/h2 + tydata[0].Y;	

ty[0].X = (h1+h2)*m2[0].X / h2 - h1*m1[0].X / h2;
ty[0].Y = (h1+h2)*m2[0].Y / h2 - h1*m1[0].Y / h2;
ty[0].U = l*(m1[0].X - m2[0].X)/h2 + ty[0].X;
ty[0].V = l*(m1[0].Y - m2[0].Y)/h2 + ty[0].Y;	
	x1[1] = m1[0].X ;
	y1[1] = m1[0].Y ;
	x2[1] = m2[0].X ;
	y2[1] = m2[0].Y ;
for(k=1;k<d;k++)
{

	ty[k].X = (h1+h2)*(m2[k].X + x2[k])/ h2 - h1*(m1[k].X +	x1[k])/h2;
	ty[k].Y = (h1+h2)*(m2[k].Y + y2[k]) / h2 - h1*(m1[k].Y + y1[k])/h2;
	ty[k].U = l*((m1[k].X +	x1[k]) - (m2[k].X + x2[k]))/h2 + ty[k].X;
	ty[k].V = l*((m1[k].Y + y1[k]) - (m2[k].Y + y2[k]))/h2 + ty[k].Y;	

	tydata[k].X= ty[k].X - ty[k-1].X;
	tydata[k].Y= ty[k].Y - ty[k-1].Y;
	tydata[k].U= ty[k].U - ty[k-1].U;
	tydata[k].V= ty[k].V - ty[k-1].V;
	
	x1[k+1] = m1[k].X + x1[k];
	y1[k+1] = m1[k].Y +	y1[k];
	x2[k+1] = m2[k].X +	x2[k];
	y2[k+1] = m2[k].Y +	y2[k];
}

for(m=0;m<k;m++)
	{
		
		printf("%f,%f,%f,%f\n",tydata[m].X,tydata[m].Y,tydata[m].U,tydata[m].V);
	}
	printf("%d\n",k);
}



/*for(m=0;m<=i;m++)
{
	printf("%d,%f,%f,%f,%f\n",cs1[m].G_pathmode,cs1[m].X,cs1[m].Y,cs1[m].I,cs1[m].J);
}
	printf("\n");
for(m=0;m<=i;m++)
	printf("%d,%f,%f,%f,%f\n",cs2[m].G_pathmode,cs2[m].X,cs2[m].Y,cs2[m].I,cs2[m].J);

for(m=0;m<d;m++)
	{
		
		printf("%f,%f,%f,%f\n",m1[m].X,m1[m].Y,m2[m].X,m2[m].Y);
	}
	printf("%d\n",d);

}*/





