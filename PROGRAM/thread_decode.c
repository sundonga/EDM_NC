#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include <minigui/newfiledlg.h>

#include "thread_decode.h"
#include "auto.h"
#include "edit.h"
#include "thread_statusdisp.h"
#include "usermain.h"


extern volatile int *User_SpeedChkFlag_Shm;  //用户层是否进行速度检测标志位的共享内存地址;
extern  volatile int *User_Decode_Flag;//用户层译码标志位
extern volatile int *User_Counter_Num;//
extern volatile AlarmStruct User_AlarmMessage;

extern HWND hAAuto,hStatus, hARtgrap, hManual;
extern Coordata data_r;   //Relative coor
extern int drawll;

FILE *file1;//********************
static double width=0, length=0, mw=0, ml=0, poi_x=0,poi_y=0;
extern HDC PCD2;

inline double Gcode2d(char* s, const char* gcode); /*将字符串s中字符串gcode后面的实型数字返回, gcode必须是一个字符!*/
inline int Gcode2i(char* s, const char* gcode); /*将字符串s中字符串gcode后面的整形数字返回, gcode必须是一个字符!*/
int stringnum(char *p, const char* s); /*返回字符串p中含有字符串s的个数, s必须是一个字符*/
int row_n = 90;


/*译码主函数*/
void decode(void)
{
    /*取出优先级*/
    pthread_attr_t attr;
    struct sched_param param;

    pthread_getattr_np (pthread_self(), &attr);
    pthread_attr_getschedparam(&attr, &param);

    NEWFILEDLGDATA myWinFileData;
    int choise=0, fd;
    int reallength = -1;

    char ptext[50000];      
    int num, i=0, j, d_c = 0, c = 0;
    int fd2, fd3, n1;

    nc_data data[100],cs[100],as[120];
    char *phead;        //程序每一行字符串的头指针
    char *delim ="\n";      //用回车符分割程序
    memset(ptext, '0', sizeof(ptext));
    
    int sign,Sign1,Sign2,Sign3,Sign4;                        //刀具补偿状态标志
    double X0,Z0,X1,Z1,R=0.05,d0,d1,r0x,r0x_abs,r0z,r0z_abs,r1x,r1x_abs,r1z,r1z_abs;
    double S1z,S1x,S2z,S2x,S3z,S3x,S4z,S4x,K1,K2,Sz,Sx,Sz1,Sx1,Sz2,Sx2,Start_Z=0.,Start_X=0.;
    double I_one,K_one,I_two,K_two,KBx0,KBz0,KBx1,KBz1,Ox,Oz,K_arc,a_arc,b_arc,c_arc;
    double Kline,r_arc;

    myWinFileData.IsSave = FALSE;   
    choise = ShowOpenDialog (HWND_DESKTOP, 200, 80, 350, 350, &myWinFileData);

    if(choise == IDOK)
    {
        fd = open(myWinFileData.filefullname, O_RDONLY);

                if (fd <= 0)
                {
                        User_AlarmMessage.num=12;
                        User_AlarmMessage.level=1;
                        User_AlarmMessage.message="Can not open machining file!";
                        MessageBox (HWND_DESKTOP, "文件打开失败","Program", MB_OK | MB_ICONSTOP);
                        return ;
                }
    }
    while (1)
    {

        if(*User_Reset_Flag==1){
              close(fd);
              row_n = 90;
              pthread_cancel(id_decode);//撤销线程
        }

        pthread_mutex_lock(&mutex);
        while( *User_Decode_Flag == 2) 
            pthread_cond_wait(&cv,&mutex);
        pthread_mutex_unlock(&mutex);   

        num = 0;
        i = 0;

        while(num <= row_n)
        {
            reallength = read(fd, ptext+i, 1 );
                    
            if(reallength == 0)
            {
            close(fd);
            break;
            }
            
            if(ptext[i] == '\n') num ++ ;
            i++;
        }
        printf("*******************%d\n",num);

    /*if( !ProgramOpen(ptext, HWND_DESKTOP)) { return; }
    else  { EnableWindow(hManual, 0);
        Vopenclose( 0 ); }*/
        
        ptext[i] = 0; //最后位置 ' \0 '                 //读入程序内容到ptext中
        printf("%s\n",ptext);
        //num=stringnum(ptext, "\n");   //程序中回车符的个数(行数)
        //if(num<=0) return;
    
        int a=0;            //i为data下标, i+1为行数

        for(j=0; phead=(0==j ? strtok(ptext, delim) : strtok(NULL, delim)); j++)
        {
            //行号
            data[j].row_id=j+1;
            //路径插补模式
            if(strstr(phead,"G00")) data[j].G_pathmode=0;
                else if(strstr(phead,"G01")) data[j].G_pathmode=1; 
                else if(strstr(phead,"G02")) data[j].G_pathmode=2; 
                else if(strstr(phead,"G03")) data[j].G_pathmode=3;
                else data[j].G_pathmode = 0;    //(0==j ? 1 : data[j-1].G_pathmode);
            //坐标模式
            if(strstr(phead,"G90")) data[j].G_coormode=90; 
                else if(strstr(phead,"91")) data[j].G_coormode=91;
                else data[j].G_coormode = 0;        // (0==j ? 91 : data[j-1].G_coormode);
            //坐标平面  
            if(strstr(phead,"G17")) data[j].G_plane=17;
            else
            if(strstr(phead,"G18")) data[j].G_plane=18;
            else 
            if(strstr(phead,"G19")) data[j].G_plane=19;
            else data[j].G_plane = 0;
            //刀具补偿
            if(strstr(phead,"G40")) data[j].G_compensate=40;
                else if(strstr(phead,"G41")) {data[j].G_compensate=41; sign=1;}
                else if(strstr(phead,"G42")) {data[j].G_compensate=42; sign=1;}
                else data[j].G_compensate = 0;      // (0==j ? 40 : data[j-1].G_compensate);
            //是否是设定坐标原点
            if(strstr(phead,"G92")) data[j].G_origin=1;
            else data[j].G_origin = 0;
            //坐标值
            if(strstr(phead,"X")) data[j].X=Gcode2d(phead,"X");
                else data[j].X = 0;     //(0==j ? 0 : data[j-1].X);
            if(strstr(phead,"Y")) data[j].Y=Gcode2d(phead,"Y");
                else data[j].Y = 0;     (0==j ? 0 : data[j-1].Y);
            if(strstr(phead,"Z")) data[j].Z=Gcode2d(phead,"Z");
                else data[j].Z = 0;     //(0==j ? 0 : data[j-1].Z);
            if(strstr(phead,"B")) data[j].B=Gcode2d(phead,"B");
                else data[j].B = 0;     //(0==j ? 0 : data[j-1].B);
            if(strstr(phead,"C")) data[j].C=Gcode2d(phead,"C");
                else data[j].C = 0;     //(0==j ? 0 : data[j-1].C);
            if(strstr(phead,"I")) data[j].I=Gcode2d(phead,"I");
                else data[j].I = 0;
            if(strstr(phead,"J")) data[j].J=Gcode2d(phead,"J");
                else data[j].J = 0;
            if(strstr(phead,"K")) data[j].K=Gcode2d(phead,"K");
                else data[j].K = 0;
            if(strstr(phead,"R")) data[j].R=Gcode2d(phead,"R");
                else data[j].R = 0;
            //M代码
            if(strstr(phead,"M")) data[j].M=Gcode2i(phead,"M");
                else data[j].M=0;
            //S代码
            if(strstr(phead,"S")) data[j].S=Gcode2i(phead,"S");
            else data[j].S = 0;
            //D代码
            if(strstr(phead,"D")) data[j].D=Gcode2i(phead,"D");
            else data[j].D = 0;
        
                
                  printf("cde daxiao wei %d\n",c);

        if(sign == 1)//进行刀补
        {
            if(c != 0)//建立刀补后      
            {
          printf("建立刀补\n");
                if(data[j].G_pathmode == 1 && cs[c-1].G_pathmode ==1)//直线接直线
                {printf("A \n");
                    if(cs[c-1].G_plane == 18)
                    {
                        Z0 = cs[c-1].Z;
                        X0 = cs[c-1].X;
                        Z1 = data[j].Z;
                        X1 = data[j].X;
                    }
                    else if(cs[c-1].G_plane == 17)
                    {
                        Z0 = cs[c-1].X;
                        X0 = cs[c-1].Y;
                        Z1 = data[j].X;
                        X1 = data[j].Y;
                    }
                    else if(cs[c-1].G_plane == 19)
                    {
                        Z0 = cs[c-1].Y;
                        X0 = cs[c-1].Z;
                        Z1 = data[j].Y;
                        X1 = data[j].Z;
                    }
                    if(cs[c-1].G_coormode == 91)        //G90的情况需要补充......................;
                    {
                        d0 = sqrt(Z0*Z0 +X0*X0);    
                        d1 = sqrt(Z1*Z1 +X1*X1);    
                        
                        if(cs[c-1].G_compensate == 41)
                        {
                            r0x = R*Z0/d0;
                            r0z = (-R)*X0/d0;

                            r1x = R*Z1/d1;
                            r1z = (-R)*X1/d1;                       	
                        }
                        else if(cs[c-1].G_compensate == 42)
                        {
                            r0x = (-R)*Z0/d0;
                            r0z = R*X0/d0;

                            r1x = (-R)*Z1/d1;
                            r1z = R*X1/d1;
                        }
                    }
          //printf("ok\n");
                //判断交接类型:缩短,插入,伸长
                        if( ((r0x*r1x + r0z*r1z) >= 0) && (((r1x-r0x)*X0 + (r1z-r0z)*Z0) >= 0) )//伸长型||缩短型
                        {                   //伸长型和缩短型程序一样,可以合并为一段程序***          
          printf("B 伸长性刀补\n"); 
                            if(Z0 == 0)
                            {
                            S1z= r0z;   S1x=r0x; //前一段G代码起点对应的刀具中心坐标,用于表示第一条直线;
                            S2z= Z0+Z1+r1z; S2x=X0+X1+r1x;K2=X1/Z1;//后一G码终点对应的刀具中心坐标,确定第二条线;
                            
                            Sz = S1z;
                            Sx = K2*(Sz - S2z) + S2x;//伸长线的交点坐标;                    
                            }
                            else if(Z1 == 0)
                            {
                            S1z= r0z;   S1x=r0x;K1=X0/Z0; 
                            S2z= Z0+Z1+r1z; S2x=X0+X1+r1x;
                            
                            Sz = S2z;
                            Sx = K1*(Sz - S1z) + S1x;//伸长线的交点坐标;                    
                            }
                            else if((X1*Z0) == (X0*Z1))
                            {
                            S1z= r0z;    S1x=r0x; 
                                                        S2z= Z0+Z1+r1z;    S2x=X0+X1+r1x;
                            Sz = Z0+r0z;
                            Sx = X0+r0x;
                            }   
                            else
                            {
                            S1z= r0z;   S1x=r0x; K1=X0/Z0;//第一段线段起点处的半径矢量顶点坐标;
                            S2z= Z0+Z1+r1z; S2x=X0+X1+r1x; K2=X1/Z1;//第二段线终点处的半径矢量顶点坐标;
                        
                            Sz = (S2x - S1x - K2*S2z + K1*S1z)/(K1 - K2);
                            Sx = K1*(Sz - S1z) + S1x;
                            }//伸长线的交点坐标;
                            

                            as[a]= cs[c-1];     //赋值,修正
                            if(cs[c-1].G_plane == 18)
                            {
                                as[a].Z = Sz - Start_Z;
                                as[a].X = Sx - Start_X;
                            }
                            else if(cs[c-1].G_plane == 17)
                            {
                                as[a].X = Sz - Start_Z;
                                as[a].Y = Sx - Start_X;
                            }
                            else if(cs[c-1].G_plane == 19)
                            {
                                as[a].Y = Sz - Start_Z;
                                as[a].Z = Sx - Start_X;
                            }
                    printf("伸长 as[%d] Y:%f Z:%f\n",a,as[a].Y,as[a].Z);
                            a++;

                            Start_Z = Sz - Z0;  
                            Start_X = Sx - X0;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备;
                        }//伸长
                
                        else if( ((r1x-r0x)*X0 + (r1z-r0z)*Z0) < 0 )//缩短型
                        {
          printf("C 缩短性刀补\n");
                            if(Z0 == 0)
                            {
                            S1z= r0z;   S1x=r0x; 
                            S2z= Z0+Z1+r1z; S2x=X0+X1+r1x;K2=X1/Z1;
                            
                            Sz = S1z;
                            Sx = K2*(Sz - S2z) + S2x;//伸长线的交点坐标;                    
                            }
                            else if(Z1 == 0)
                            {
                            S1z= r0z;   S1x=r0x;K1=X0/Z0; 
                            S2z= Z0+Z1+r1z; S2x=X0+X1+r1x;
                            
                            Sz = S2z;
                            Sx = K1*(Sz - S1z) + S1x;//伸长线的交点坐标;                    
                            }
                            else
                            {
                            S1z= r0z;   S1x=r0x; K1=X0/Z0;//第一段线段起点处的半径矢量顶点坐标;
                            S2z= Z0+Z1+r1z; S2x=X0+X1+r1x; K2=X1/Z1;//第二段线终点处的半径矢量顶点坐标;
                        
                            Sz = (S2x - S1x - K2*S2z + K1*S1z)/(K1 - K2);
                            Sx = K1*(Sz - S1z) + S1x;
                            }//伸长线的交点坐标;
                            

                            as[a]= cs[c-1];     //赋值,修正
                            if(cs[c-1].G_plane == 18)
                            {
                                as[a].Z = Sz - Start_Z;
                                as[a].X = Sx - Start_X;
                            }
                            else if(cs[c-1].G_plane == 17)
                            {
                                as[a].X = Sz - Start_Z;
                                as[a].Y = Sx - Start_X;
                            }
                            else if(cs[c-1].G_plane == 19)
                            {
                                as[a].Y = Sz - Start_Z;
                                as[a].Z = Sx - Start_X;
                            }
                    printf("缩短  as[%d] Y:%f Z:%f\n",a,as[a].Y,as[a].Z);
                            a++;

                            Start_Z = Sz - Z0;  
                            Start_X = Sx - X0;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备;
                            
                        }//缩短型

                    else if( (((r1z-r0z)*Z0 +(r1x-r0x)*X0) >= 0) && ((r0x*r1x + r0z*r1z) < 0) )//插入型
                        {
          printf("D 插入性刀补\n");
                            S1z= Z0 + r0z;  S1x=X0 + r0x; //第一段线段终点处的半径矢量顶点坐标;
                            S2z= Z0 + r1z;  S2x=X0 + r1x;//第二段线起点处的半径矢量顶点坐标;
                                        //S1,S2两点的坐标;
                            if(X0 > 0) Sign1 = 1;
                            else if(X0 == 0) Sign1 = 0;
                            else Sign1 = -1;
                            	
                            if(Z0 > 0) Sign2 = 1;
                            else if(Z0 == 0) Sign2 = 0;
                            else Sign2 = -1;

                            if(-X1 > 0) Sign3 = 1;
                            else if(-X1 == 0) Sign3 = 0;
                            else Sign3 = -1;

                            if(-Z1 > 0) Sign4 = 1;
                            else if(-Z1 == 0) Sign4 = 0;
                            else Sign4 = -1;
                            
                            if(r0x <0) r0x_abs = -r0x;
                            else r0x_abs = r0x;

                            if(r0z <0) r0z_abs = -r0z;
                            else r0z_abs = r0z;

                            if(r1x <0) r1x_abs = -r1x;
                            else r1x_abs = r1x;

                            if(r1z <0) r1z_abs = -r1z;
                            else r1z_abs = r1z;
                            
    printf("Sig1=%d,%d,%d,%d,r0x=%f,r0z=%f,r1x=%f,r1z=%f\n",Sign1,Sign2,Sign3,Sign4,r0x_abs,r0z_abs,r1x_abs,r1z_abs);

                            S3x = S1x + Sign1*r0z_abs;
                            S3z = S1z + Sign2*r0x_abs;  

                            S4x = S2x + Sign3*r1z_abs;
                            S4z = S2z + Sign4*r1x_abs;  
    printf("Start_Z=%f,Start_X=%f\n",Start_Z,Start_X);
    printf("s3x=%f,s3z=%f,s4x=%f,s4z=%f\n",S3x,S3z,S4x,S4z);

                            as[a]= cs[c-1];     //赋值,修正
                            if(cs[c-1].G_plane == 18)
                            {
                                as[a].Z = S3z - Start_Z;
                                as[a].X = S3x - Start_X;
                            }
                            else if(cs[c-1].G_plane == 17)
                            {
                                as[a].X = S3z - Start_Z;
                                as[a].Y = S3x - Start_X;
                            }
                            else if(cs[c-1].G_plane == 19)
                            {
                                as[a].Y = S3z - Start_Z;
                                as[a].Z = S3x - Start_X;
                            }
                    printf("插入第一 as[%d] Y:%f Z:%f\n",a,as[a].Y,as[a].Z);
                            a++;
                            
                            as[a]= cs[c-1];     //插入段修正
                            if(cs[c-1].G_plane == 18)
                            {
                                as[a].Z = S4z - S3z;
                                as[a].X = S4x - S3x;
                                as[a].row_id=a+1;
                            }
                            else if(cs[c-1].G_plane == 17)
                            {
                                as[a].X = S4z - S3z;
                                as[a].Y = S4x - S3x;
                                as[a].row_id=a+1;
                            }
                            else if(cs[c-1].G_plane == 19)
                            {
                                as[a].Y = S4z - S3z;
                                as[a].Z = S4x - S3x;
                                as[a].row_id=a+1;
                            }
                    printf("插入第二 as[%d] Y:%f Z:%f\n",a,as[a].Y,as[a].Z);
                            a++;

                            Start_Z = S4z - Z0; 
                            Start_X = S4x - X0;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备;
                        }//插入
                            
                        if(data[i].G_compensate == 40)       //取消刀补;
                            {
          printf("quxiao刀补2\n");
                                as[a]= data[j];     //赋值,修正
                                if(cs[c-1].G_plane == 18)
                                {
                                    as[a].Z = Z1 - Start_Z;
                                    as[a].X = X1 - Start_X;
                                }
                                else if(cs[c-1].G_plane == 17)
                                {
                                    as[a].X = Z1 - Start_Z;
                                    as[a].Y = X1 - Start_X;
                                }
                                else if(cs[c-1].G_plane == 19)
                                {
                                    as[a].Y = Z1 - Start_Z;
                                    as[a].Z = X1 - Start_X;
                                }
                    printf("插入取消 as[%d] Y:%f Z:%f\n",a,as[a].Y,as[a].Z);
                                a++;
                                sign = 0;//跳出补偿
                                c = 0;//为下一次刀补作初始化.
                                Start_Z = 0.;   
                                Start_X = 0.;
                            }//取消刀补;            
                }//直线接直线;


                else if(data[j].G_pathmode == 1 && (cs[c-1].G_pathmode ==3 || cs[c-1].G_pathmode ==2))//圆弧接直线
                {
//********************************************************************************
                    if(cs[c-1].G_plane == 18)
                    {
                        Z0 = cs[c-1].Z;
                        X0 = cs[c-1].X;
                        K_one=cs[c-1].K;
                        I_one=cs[c-1].I;

                        Z1 = data[j].Z;
                        X1 = data[j].X;
                    }
                    else if(cs[c-1].G_plane == 17)
                    {
                        Z0 = cs[c-1].X;
                        X0 = cs[c-1].Y;
                        K_one=cs[c-1].I;
                        I_one=cs[c-1].J;

                        Z1 = data[j].X;
                        X1 = data[j].Y;
                    }
                    else if(cs[c-1].G_plane == 19)
                    {
                        Z0 = cs[c-1].Y;
                        X0 = cs[c-1].Z;
                        K_one=cs[c-1].J;
                        I_one=cs[c-1].K;

                        Z1 = data[j].Y;
                        X1 = data[j].Z;
                    }
                    if(cs[c-1].G_coormode == 91)        //G90的情况需要补充......................;
                    {
                        d0 = sqrt((X0-I_one)*(X0-I_one)+(Z0-K_one)*(Z0-K_one)); 
                        d1 = sqrt(Z1*Z1 +X1*X1);    
                        
        if((cs[c-1].G_compensate == 41 && cs[c-1].G_pathmode == 2)||(cs[c-1].G_compensate == 42 && cs[c-1].G_pathmode == 3))
                        {
                            r0x = R*(X0-I_one)/d0;
                            r0z = R*(Z0-K_one)/d0;
                        }
        else if((cs[c-1].G_compensate == 42 && cs[c-1].G_pathmode == 2)||(cs[c-1].G_compensate == 41 && cs[c-1].G_pathmode == 3))
                        {
                            r0x = (-R)*(X0-I_one)/d0;
                            r0z = (-R)*(Z0-K_one)/d0;
                        }

                        if(data[j].G_compensate == 41)
                        {
                            r1x = R*Z1/d1;
                            r1z = (-R)*X1/d1;                           	
                        }       
                        else if(data[j].G_compensate == 42)
                        {
                            r1x = (-R)*Z1/d1;
                            r1z = R*X1/d1;
                        }//确定r0x,r0z,r1x,r1z参量;

                        if(cs[c-1].G_pathmode == 2)
                        {
                            KBx0= -(Z0-K_one);
                            KBz0=X0-I_one;
                        }
                        else if(cs[c-1].G_pathmode == 3)
                        {
                            KBx0=Z0-K_one;
                            KBz0= -(X0-I_one);
                        }// 确定KB参量;
                    }//G91
                //判断交接类型:缩短,插入,伸长
                        if( ((r0x*r1x + r0z*r1z) >= 0) && (((r1x-r0x)*KBx0 + (r1z-r0z)*KBz0) >= 0) )//伸长型    
                            {
    printf("伸长性刀补\n");
                            if(KBz0 == 0 && Z1 != 0)
                            {
                            S1z= Z0+r0z;    S1x=X0+r0x; 
                            S2z= Z0+r1z;    S2x=X0+r1x;K2=X1/Z1;
                            
                            Sz = S1z;
                            Sx = K2*(Sz - S2z) + S2x;//直线斜率无限大时;                    
                            }
                            else if(KBz0 != 0 && Z1 == 0)
                            {
                            S1z= Z0+r0z;    S1x=X0+r0x;K1=KBx0/KBz0; 
                            S2z= Z0+r1z;    S2x=X0+r1x;
                            
                            Sz = S2z;
                            Sx = K1*(Sz - S1z) + S1x;//直线斜率无限大时;                    
                            }
                            else if((KBx0*Z1) == (X1*KBz0))
                            {
                            
                            Sz = Z0+r0z;
                            Sx = X0+r0x;            
                            }//直线斜率相等时;
                            else
                            {
                            S1z= Z0+r0z;    S1x=X0+r0x;K1=KBx0/KBz0; 
                            S2z= Z0+r1z;    S2x=X0+r1x;K2=X1/Z1;
                            
                            Sz = (S2x - S1x - K2*S2z + K1*S1z)/(K1 - K2);
                            Sx = K1*(Sz - S1z) + S1x;
                            }//伸长线的交点坐标;                    
                        

                            as[a]= cs[c-1];     //赋值,修正
                            if(cs[c-1].G_plane == 18)
                            {
                                as[a].Z = (Z0+r0z)-Start_Z;
                                as[a].X = (X0+r0x)-Start_X;
                                as[a].K = K_one-Start_Z;
                                as[a].I = I_one-Start_X;
                            }
                            else if(cs[c-1].G_plane == 17)
                            {
                                as[a].X = (Z0+r0z)-Start_Z;
                                as[a].Y = (X0+r0x)-Start_X;
                                as[a].I = K_one-Start_Z;
                                                                as[a].J = I_one-Start_X;
                            }
                            else if(cs[c-1].G_plane == 19)
                            {
                                as[a].Y = (Z0+r0z)-Start_Z;
                                as[a].Z = (X0+r0x)-Start_X;
                                as[a].J = K_one-Start_Z;
                                                                as[a].K = I_one-Start_X;
                            }
                            a++;//第一段圆弧;

                            as[a]= data[i];     //赋值,修正
                            if(cs[c-1].G_plane == 18)
                            {
                                as[a].Z = Sz-(Z0+r0z);
                                as[a].X = Sx-(X0+r0x);
                            }
                            else if(cs[c-1].G_plane == 17)
                            {
                                as[a].X = Sz-(Z0+r0z);
                                as[a].Y = Sx-(X0+r0x);
                            }
                            else if(cs[c-1].G_plane == 19)
                            {
                                as[a].Y = Sz-(Z0+r0z);
                                as[a].Z = Sx-(X0+r0x);
                            }
                            a++;//圆弧切线的伸长部分;

                            Start_Z = Sz - Z0;  
                            Start_X = Sx - X0;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备;
                            
                        }//伸长型;

                        else if( (((r1x-r0x)*KBx0 + (r1z-r0z)*KBz0) < 0)||(((r1x-r0x)*KBx0 + (r1z-r0z)*KBz0)==0&&cs[c-1].G_compensate == 41 && cs[c-1].G_pathmode == 2)||(((r1x-r0x)*KBx0 + (r1z-r0z)*KBz0)==0&&cs[c-1].G_compensate == 42 && cs[c-1].G_pathmode== 3)) //缩短型 
                        {
          printf("缩短性刀补\n");
                            S1z=Z0+r1z; S1x=X0+r1x;
                            S2z=Z0+Z1+r1z;  S2x=X0+X1+r1x;//与圆弧相交的直线的两个点;

                            Oz=K_one;   Ox=I_one;//圆心坐标;
                            r_arc=sqrt( (Z0+r0z-K_one)*(Z0+r0z-K_one) + (X0+r0x-I_one)*(X0+r0x-I_one) );//圆弧半径

                            if(S1z == S2z)
                            {
                            Sz1=S1z ;   Sx1=Ox + sqrt(r_arc*r_arc-(S1z-Oz)*(S1z-Oz));
                            Sz2=S1z ;   Sx2=Ox - sqrt(r_arc*r_arc-(S1z-Oz)*(S1z-Oz));
                            }
                            else if(S1x == S2x)
                            {
                            Sz1=Oz + sqrt(r_arc*r_arc-(S1x-Ox)*(S1x-Ox));   Sx1=S1x;
                            Sz2=Oz - sqrt(r_arc*r_arc-(S1x-Ox)*(S1x-Ox));   Sx2=S1x;
                            }
                            else
                            {
                            K_arc=(S2z-S1z)/(S2x-S1x);
                            a_arc=1+K_arc*K_arc;
                            b_arc=2*K_arc*(S2z-Oz-K_arc*S2x-Ox/K_arc);
                            c_arc=(S2z-Oz-K_arc*S2x)*(S2x-Oz-K_arc*S2x)-r_arc*r_arc+Ox*Ox;
                            
                        Sx1=( -b_arc+sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);   Sz1=S2z + K_arc*(Sx1-S2x);
                        Sx2=( -b_arc-sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);   Sz2=S2z + K_arc*(Sx2-S2x);
                            }//伸长线的交点坐标;
                            
    //if( ((S1z*Z1+S1x*X1)>=0 && ((Sz2-Sz1)*Z1+(Sx2-Sx1)*X1)<=0) || ((S1z*Z1+S1x*X1)<0 && ((Sz2-Sz1)*Z1+(Sx2-Sx1)*X1)>0) )
    if( ((r0z*Z1+r0x*X1)>=0 && ((Sz2-Sz1)*Z1+(Sx2-Sx1)*X1)<=0) || ((r0z*Z1+r0x*X1)<0 && ((Sz2-Sz1)*Z1+(Sx2-Sx1)*X1)>0) )
                            {
                            Sx=Sx1;
                            Sz=Sz1;
                            }
    //else if( ((S1z*Z1+S1x*X1)>=0 && ((Sz2-Sz1)*Z1+(Sx2-Sx1)*X1)>0) || ((S1z*Z1+S1x*X1)<0 && ((Sz2-Sz1)*Z1+(Sx2-Sx1)*X1)<=0) )
    else if( ((r0z*Z1+r0x*X1)>=0 && ((Sz2-Sz1)*Z1+(Sx2-Sx1)*X1)>0) || ((r0z*Z1+r0x*X1)<0 && ((Sz2-Sz1)*Z1+(Sx2-Sx1)*X1)<=0) )
                            {
                            Sx=Sx2;
                            Sz=Sz2;
                            }
                            as[a]= cs[c-1];     //赋值,修正
                            if(cs[c-1].G_plane == 18)
                            {
                                as[a].Z = Sz - Start_Z;
                                as[a].X = Sx - Start_X;
                                as[a].K = K_one-Start_Z;
                                as[a].I = I_one-Start_X;
                            }
                            else if(cs[c-1].G_plane == 17)
                            {
                                as[a].X = Sz - Start_Z;
                                as[a].Y = Sx - Start_X;
                                as[a].I = K_one-Start_Z;
                                                                as[a].J = I_one-Start_X;
                            }
                            else if(cs[c-1].G_plane == 19)
                            {
                                as[a].Y = Sz - Start_Z;
                                as[a].Z = Sx - Start_X;
                                as[a].J = K_one-Start_Z;
                                                                as[a].K = I_one-Start_X;
                            }
                            a++;

                            Start_Z = Sz - Z0;  
                            Start_X = Sx - X0;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备;
                            
                        }//缩短型
                

                    else if( ((((r1z-r0z)*KBz0 +(r1x-r0x)*KBx0) >0)&&((r0x*r1x + r0z*r1z) < 0))||(((r1x-r0x)*KBx0 + (r1z-r0z)*KBz0)==0&&cs[c-1].G_compensate == 42 && cs[c-1].G_pathmode ==2)||(((r1x-r0x)*KBx0 + (r1z-r0z)*KBz0)==0&&cs[c-1].G_compensate == 41 && cs[c-1].G_pathmode ==3) )//插入型
                        {printf("插入性刀补\n");        
                            S1z= Z0 + r0z;  S1x=X0 + r0x; //第一段线段终点处的半径矢量顶点坐标;
                            S2z= Z0 + r1z;  S2x=X0 + r1x;//第二段线起点处的半径矢量顶点坐标;
                                        //S1,S2两点的坐标;

                            if(cs[c-1].G_pathmode ==3)
                            {
                                if((Z0-K_one) > 0) Sign1 = 1;
                                else if((Z0-K_one) == 0) Sign1 = 0;
                                else Sign1 = -1;

                                if((I_one-X0) > 0) Sign2 = 1;
                                else if((I_one-X0) == 0) Sign2 = 0;
                                else Sign2 = -1;
                            }
                            else if(cs[c-1].G_pathmode ==2)
                            {
                                if((K_one-Z0) > 0) Sign1 = 1;
                                else if((K_one-Z0) == 0) Sign1 = 0;
                                else Sign1 = -1;

                                if((X0-I_one) > 0) Sign2 = 1;
                                else if((X0-I_one) == 0) Sign2 = 0;
                                else Sign2 = -1;
                            }
                            if((-X1) > 0) Sign3 = 1;
                            else if((-X1) == 0) Sign3 = 0;
                            else Sign3 = -1;

                            if(-Z1 > 0) Sign4 = 1;
                            else if(-Z1 == 0) Sign4 = 0;
                            else Sign4 = -1;
                            
                            if(r0x <0) r0x_abs = -r0x;
                            else r0x_abs = r0x;

                            if(r0z <0) r0z_abs = -r0z;
                            else r0z_abs = r0z;

                            if(r1x <0) r1x_abs = -r1x;
                            else r1x_abs = r1x;

                            if(r1z <0) r1z_abs = -r1z;
                            else r1z_abs = r1z;
                            
    printf("Sig1=%d,%d,%d,%d,r0x=%f,r0z=%f,r1x=%f,r1z=%f\n",Sign1,Sign2,Sign3,Sign4,r0x_abs,r0z_abs,r1x_abs,r1z_abs);

                            S3x = S1x + Sign1*r0z_abs;
                            S3z = S1z + Sign2*r0x_abs;  

                            S4x = S2x + Sign3*r1z_abs;
                            S4z = S2z + Sign4*r1x_abs;  
    printf("Start_Z=%f,Start_X=%f\n",Start_Z,Start_X);
    printf("s3x=%f,s3z=%f,s4x=%f,s4z=%f\n",S3x,S3z,S4x,S4z);

                            as[a]= cs[c-1];     //赋值,修正,圆弧部分
                            if(cs[c-1].G_plane == 18)
                            {
                                as[a].Z = S1z - Start_Z;
                                as[a].X = S1x - Start_X;
                                as[a].K = K_one-Start_Z;
                                as[a].I = I_one-Start_X;
                            }
                            else if(cs[c-1].G_plane == 17)
                            {
                                as[a].X = S1z - Start_Z;
                                as[a].Y = S1x - Start_X;
                                as[a].I = K_one-Start_Z;
                                                                as[a].J = I_one-Start_X;
                            }
                            else if(cs[c-1].G_plane == 19)
                            {
                                as[a].Y = S1z - Start_Z;
                                as[a].Z = S1x - Start_X;
                                as[a].J = K_one-Start_Z;
                                                                as[a].K = I_one-Start_X;
                            }
                            a++;

                            as[a]= data[j];     //赋值,修正,伸长的直线S1S3部分;
                            if(cs[c-1].G_plane == 18)
                            {
                                as[a].Z = S3z - S1z;
                                as[a].X = S3x - S1x;
                                as[a].row_id=a+1;
                            }
                            else if(cs[c-1].G_plane == 17)
                            {
                                as[a].X = S3z - S1z;
                                as[a].Y = S3x - S1x;
                                as[a].row_id=a+1;
                            }
                            else if(cs[c-1].G_plane == 19)
                            {
                                as[a].Y = S3z - S1z;
                                as[a].Z = S3x - S1x;
                                as[a].row_id=a+1;
                            }
                            a++;
                            
                            as[a]= data[j];     //插入段修S3S4部分;
                            if(cs[c-1].G_plane == 18)
                            {
                                as[a].Z = S4z - S3z;
                                as[a].X = S4x - S3x;
                                as[a].row_id=a+1;
                            }
                            else if(cs[c-1].G_plane == 17)
                            {
                                as[a].X = S4z - S3z;
                                as[a].Y = S4x - S3x;
                                as[a].row_id=a+1;
                            }
                            else if(cs[c-1].G_plane == 19)
                            {
                                as[a].Y = S4z - S3z;
                                as[a].Z = S4x - S3x;
                                as[a].row_id=a+1;
                            }
                            a++;

                            Start_Z = S4z - Z0; 
                            Start_X = S4x - X0;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备;
                        }//插入         
                            if(data[j].G_compensate == 40)       //取消刀补;
                            {
                                as[a]= data[j];     //赋值,修正
                                if(cs[c-1].G_plane == 18)
                                {
                                    as[a].Z = Z1 - Start_Z;
                                    as[a].X = X1 - Start_X;
                                }
                                else if(cs[c-1].G_plane == 17)
                                {
                                    as[a].X = Z1 - Start_Z;
                                    as[a].Y = X1 - Start_X;
                                }
                                else if(cs[c-1].G_plane == 19)
                                {
                                    as[a].Y = Z1 - Start_Z;
                                    as[a].Z = X1 - Start_X;
                                }
                    printf("缩短取消 as[%d] Y:%f Z:%f\n",a,as[a].Y,as[a].Z);
                                a++;
                                sign = 0;//跳出补偿
                                c = 0;//为下一次刀补作初始化.
                                Start_Z = 0.;   
                                Start_X = 0.;
                            }//取消刀补 
                }//圆弧接直线

            
                else if((data[j].G_pathmode == 2 || data[j].G_pathmode == 3) && cs[c-1].G_pathmode ==1)//直线接圆弧
                {
                    if(cs[c-1].G_plane == 18)
                    {
                        Z0 = cs[c-1].Z;
                        X0 = cs[c-1].X;

                        Z1 = data[j].Z;
                        X1 = data[j].X;
                        K_two=data[j].K;
                        I_two=data[j].I;
                    }
                    else if(cs[c-1].G_plane == 17)
                    {
                        Z0 = cs[c-1].X;
                        X0 = cs[c-1].Y;

                        Z1 = data[j].X;
                        X1 = data[j].Y;
                        K_two=data[j].I;
                        I_two=data[j].J;
                    }
                    else if(cs[c-1].G_plane == 19)
                    {
                        Z0 = cs[c-1].Y;
                        X0 = cs[c-1].Z;

                        Z1 = data[j].Y;
                        X1 = data[j].Z;
                        K_two=data[j].J;
                        I_two=data[j].K;
                    }
                    if(cs[c-1].G_coormode == 91)        //G90的情况需要补充......................;
                    {
                        d0 = sqrt(Z0*Z0 +X0*X0);    
                        d1 = sqrt((X1-I_two)*(X1-I_two)+(Z1-K_two)*(Z1-K_two)); 
                        
                        if(cs[c-1].G_compensate == 41)
                        {
                            r0x = R*Z0/d0;
                            r0z = (-R)*X0/d0;                           	
                        }       
                        else if(cs[c-1].G_compensate == 42)
                        {
                            r0x = (-R)*Z0/d0;
                            r0z = R*X0/d0;
                        }

                if(data[j].G_compensate == 41 && data[j].G_pathmode == 2)
                        {
                            r1x = R*(-I_two)/d1;
                            r1z = R*(-K_two)/d1;
                        }
                else if(data[j].G_compensate == 42 && data[j].G_pathmode == 3)
                        {
                            r1x = R*(-I_two)/d1;
                                                        r1z = R*(-K_two)/d1;
                                                }
                else if(data[j].G_compensate == 42 && data[j].G_pathmode == 2)
                        {
                            r1x = R*I_two/d1;
                            r1z = R*K_two/d1;

                        }
                else if(data[j].G_compensate == 41 && data[j].G_pathmode == 3)
                        {
                            r1x = R*I_two/d1;
                                                        r1z = R*K_two/d1;
                        }
                        //确定r0x,r0z,r1x,r1z参量;

                        if(data[j].G_pathmode == 2)
                        {
                            KBx1= K_two;
                            KBz1=-I_two;
                        }
                        else if(data[j].G_pathmode == 3)
                        {
                            KBx1=-K_two;
                            KBz1= I_two;
                        }// 确定KB参量,圆弧起点处的切线;
                    }//G91
                //判断交接类型:缩短,插入,伸长
                        if( ((r0x*r1x + r0z*r1z) >= 0) && (((r1x-r0x)*X0 + (r1z-r0z)*Z0) >= 0) )//伸长型    
                            {
     printf("伸长性刀补\n");
                            if(KBz1 == 0 && Z0 != 0)
                            {
                            S1z= Z0+r0z;    S1x=X0+r0x; K1=X0/Z0;
                            S2z= Z0+r1z;    S2x=X0+r1x;
                            
                            Sz = S2z;
                            Sx = K1*(Sz - S1z) + S1x;//直线斜率无限大时;    
                            }
                            else if(KBz1 != 0 && Z0 == 0)
                            {
                            S1z= Z0+r0z;    S1x=X0+r0x; 
                            S2z= Z0+r1z;    S2x=X0+r1x;K2=KBx1/KBz1;
                            
                            Sz = S1z;
                            Sx = K2*(Sz - S2z) + S2x;//直线斜率无限大时;                    
                            }
                            else if((KBx1*Z0) == (X0*KBz1))
                            {
                            S1z= Z0+r0z;    S1x=X0+r0x; 
                                                        S2z= Z0+r1z;    S2x=X0+r1x;
                            Sz = Z0+r0z;
                            Sx = X0+r0x;            
                            }//直线斜率相等时;
                            else
                            {
                            S1z= Z0+r0z;    S1x=X0+r0x;K1=X0/Z0; 
                            S2z= Z0+r1z;    S2x=X0+r1x;K2=KBx1/KBz1;
                            
                            Sz = (S2x - S1x - K2*S2z + K1*S1z)/(K1 - K2);
                            Sx = K1*(Sz - S1z) + S1x;
                            }//伸长线的交点坐标;                    
                        
                            as[a]= cs[c-1];     //赋值,修正第一段直线
                            if(cs[c-1].G_plane == 18)
                            {
                                as[a].Z = Sz - Start_Z;
                                as[a].X = Sx - Start_X;
                            }
                            else if(cs[c-1].G_plane == 17)
                            {
                                as[a].X = Sz - Start_Z;
                                as[a].Y = Sx - Start_X;
                            }
                            else if(cs[c-1].G_plane == 19)
                            {
                                as[a].Y = Sz - Start_Z;
                                as[a].Z = Sx - Start_X;
                            }
                            a++;


                            as[a]= cs[c-1];     //赋值,修正圆弧部分的伸长部分S-S2;
                            if(cs[c-1].G_plane == 18)
                            {
                                as[a].Z = S2z - Sz;
                                as[a].X = S2x - Sx;
                            }
                            else if(cs[c-1].G_plane == 17)
                            {
                                as[a].X = S2z - Sz;
                                as[a].Y = S2x - Sx;
                            }
                            else if(cs[c-1].G_plane == 19)
                            {
                                as[a].Y = S2z - Sz;
                                as[a].Z = S2x - Sx;
                            printf("honghua S2x:%f,Sx:%f",S2x,Sx);
                            }
                            a++;//第一段圆弧;

                            Start_Z = S2z - Z0; 
                            Start_X = S2x - X0;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备;
                        }//伸长型;

                        else if((((r1x-r0x)*X0 + (r1z-r0z)*Z0) < 0)||((((r1x-r0x)*X0 + (r1z-r0z)*Z0)== 0)&&data[j].G_compensate == 41 && data[j].G_pathmode == 2)||((((r1x-r0x)*X0 + (r1z-r0z)*Z0)== 0)&&data[j].G_compensate == 42 && data[j].G_pathmode == 3)) //缩短型   
                        {
                            S1z=r0z;    S1x=r0x;
                            S2z=Z0+r0z; S2x=X0+r0x;//与圆弧相交的直线的两个点;

                            Oz=Z0+K_two;    Ox=X0+I_two;//圆心坐标;
                            r_arc=sqrt( (Z0+r1z-Oz)*(Z0+r1z-Oz) + (X0+r1x-Ox)*(X0+r1x-Ox) );//圆弧半径

                            if(S1z == S2z)
                            {
                            Sz1=S1z ;   Sx1=Ox + sqrt(r_arc*r_arc-(S1z-Oz)*(S1z-Oz));
                            Sz2=S1z ;   Sx2=Ox - sqrt(r_arc*r_arc-(S1z-Oz)*(S1z-Oz));
                            }
                            else if(S1x == S2x)
                            {
                            Sz1=Oz + sqrt(r_arc*r_arc-(S1x-Ox)*(S1x-Ox));   Sx1=S1x;
                            Sz2=Oz - sqrt(r_arc*r_arc-(S1x-Ox)*(S1x-Ox));   Sx2=S1x;
                            }
                            else
                            {
                            K_arc=(S2z-S1z)/(S2x-S1x);
                            a_arc=1+K_arc*K_arc;
                            b_arc=2*K_arc*(S2z-Oz-K_arc*S2x-Ox/K_arc);
                            c_arc=(S2z-Oz-K_arc*S2x)*(S2x-Oz-K_arc*S2x)-r_arc*r_arc+Ox*Ox;
                            
                        Sx1=( -b_arc+sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);   Sz1=S2z + K_arc*(Sx1-S2x);
                        Sx2=( -b_arc-sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);   Sz2=S2z + K_arc*(Sx2-S2x);
                            }//伸长线的交点坐标;
                            
    //if( ((S2z*Z0+S2x*X0)>=0 && ((Sz2-Sz1)*Z0+(Sx2-Sx1)*X0)<=0) || ((S2z*Z0+S2x*X0)<0 && ((Sz2-Sz1)*Z0+(Sx2-Sx1)*X0)>0) )
    if( ((r1z*Z0+r1x*X0)>=0 && ((Sz2-Sz1)*Z0+(Sx2-Sx1)*X0)<=0) || ((r1z*Z0+r1x*X0)<0 && ((Sz2-Sz1)*Z0+(Sx2-Sx1)*X0)>0) )
                            {
                            Sx=Sx1;
                            Sz=Sz1;
                            }
    //else if( ((S2z*Z0+S2x*X0)>=0 && ((Sz2-Sz1)*Z0+(Sx2-Sx1)*X0)>0) || ((S2z*Z0+S2x*X0)<0 && ((Sz2-Sz1)*Z0+(Sx2-Sx1)*X0)<=0) )
    else if( ((r1z*Z0+r1x*X0)>=0 && ((Sz2-Sz1)*Z0+(Sx2-Sx1)*X0)>0) || ((r1z*Z0+r1x*X0)<0 && ((Sz2-Sz1)*Z0+(Sx2-Sx1)*X0)<=0) )
                            {
                            Sx=Sx2;
                            Sz=Sz2;
                            }
                            as[a]= cs[c-1];     //赋值,修正
                            if(cs[c-1].G_plane == 18)
                            {
                                as[a].Z = Sz - Start_Z;
                                as[a].X = Sx - Start_X;
                            }
                            else if(cs[c-1].G_plane == 17)
                            {
                                as[a].X = Sz - Start_Z;
                                as[a].Y = Sx - Start_X;
                            }
                            else if(cs[c-1].G_plane == 19)
                            {
                                as[a].Y = Sz - Start_Z;
                                as[a].Z = Sx - Start_X;
                            }
                            a++;

                            Start_Z = Sz - Z0;  
                            Start_X = Sx - X0;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备;
                        }//缩短型

                    else if(((((r1z-r0z)*Z0 +(r1x-r0x)*X0) > 0) && ((r0x*r1x + r0z*r1z) < 0))||((((r1z-r0z)*Z0 +(r1x-r0x)*X0)== 0)&&data[j].G_compensate == 41 && data[j].G_pathmode == 3)||((((r1z-r0z)*Z0 +(r1x-r0x)*X0)== 0)&&data[j].G_compensate == 42 && data[j].G_pathmode == 2))//插入型
                        {
                                S1z= Z0 + r0z;  S1x=X0 + r0x; //第一段线段终点处的半径矢量顶点坐标;
                            S2z= Z0 + r1z;  S2x=X0 + r1x;//第二段线起点处的半径矢量顶点坐标;
                                        //S1,S2两点的坐标;
                            if(X0 > 0) Sign1 = 1;
                            else if(X0 == 0) Sign1 = 0;
                            else Sign1 = -1;
                            	
                            if(Z0 > 0) Sign2 = 1;
                            else if(Z0 == 0) Sign2 = 0;
                            else Sign2 = -1;

                            if(data[j].G_pathmode ==3)
                            {
                                if(K_two > 0) Sign3 = 1;
                                else if(K_two == 0) Sign3 = 0;
                                else Sign3 = -1;

                                if((-I_two) > 0) Sign4 = 1;
                                else if((-I_two) == 0) Sign4 = 0;
                                else Sign4 = -1;
                            }
                            else if(data[j].G_pathmode ==2)
                            {
                                if((-K_two) > 0) Sign3 = 1;
                                else if((-K_two) == 0) Sign3 = 0;
                                else Sign3 = -1;

                                if(I_two > 0) Sign4 = 1;
                                else if(I_two == 0) Sign4 = 0;
                                else Sign4 = -1;
                            }
                            
                            if(r0x <0) r0x_abs = -r0x;
                            else r0x_abs = r0x;

                        if(r0z <0) r0z_abs = -r0z;
                        else r0z_abs = r0z;

                        if(r1x <0) r1x_abs = -r1x;
                        else r1x_abs = r1x;

                        if(r1z <0) r1z_abs = -r1z;
                        else r1z_abs = r1z;

                        S3x = S1x + Sign1*r0z_abs;
                        S3z = S1z + Sign2*r0x_abs;  

                        S4x = S2x + Sign3*r1z_abs;
                        S4z = S2z + Sign4*r1x_abs; 
                        
                        as[a]= cs[c-1];     //赋值,修正,第一段直线
                        if(cs[c-1].G_plane == 18)
                        {
                            as[a].Z = S3z - Start_Z;
                            as[a].X = S3x - Start_X;
                        }
                        else if(cs[c-1].G_plane == 17)
                        {
                            as[a].X = S3z - Start_Z;
                            as[a].Y = S3x - Start_X;
                        }
                        else if(cs[c-1].G_plane == 19)
                        {
                            as[a].Y = S3z - Start_Z;
                            as[a].Z = S3x - Start_X;
                        }
                        a++;

                        as[a]= cs[c-1];     //赋值,修正,插入的直线S3S4部分;
                        if(cs[c-1].G_plane == 18)
                        {
                            as[a].Z = S4z - S3z;
                            as[a].X = S4x - S3x;
                            as[a].row_id=a+1;
                        }
                        else if(cs[c-1].G_plane == 17)
                        {
                            as[a].X = S4z - S3z;
                            as[a].Y = S4x - S3x;
                            as[a].row_id=a+1;
                        }
                        else if(cs[c-1].G_plane == 19)
                        {
                            as[a].Y = S4z - S3z;
                            as[a].Z = S4x - S3x;
                            as[a].row_id=a+1;
                        }
                        a++;
                        
                        as[a]= cs[c-1];     //插入段修S4S2部分;
                        if(cs[c-1].G_plane == 18)
                        {
                            as[a].Z = S2z - S4z;
                            as[a].X = S2x - S4x;
                            as[a].row_id=a+1;
                        }
                        else if(cs[c-1].G_plane == 17)
                        {
                            as[a].X = S2z - S4z;
                            as[a].Y = S2x - S4x;
                            as[a].row_id=a+1;
                        }
                        else if(cs[c-1].G_plane == 19)
                        {
                            as[a].Y = S2z - S4z;
                            as[a].Z = S2x - S4x;
                            as[a].row_id=a+1;
                        }
                        a++;

                        Start_Z = S2z - Z0; 
                        Start_X = S2x - X0;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备;
                    }//插入         
                        if(data[j].G_compensate == 40)       //取消刀补;
                        {
                            as[a]= data[j];     //赋值,修正
                            if(cs[c-1].G_plane == 18)
                            {
                                as[a].Z = Z1 - Start_Z;
                                as[a].X = X1 - Start_X;
                            }
                            else if(cs[c-1].G_plane == 17)
                            {
                                as[a].X = Z1 - Start_Z;
                                as[a].Y = X1 - Start_X;
                            }
                            else if(cs[c-1].G_plane == 19)
                            {
                                as[a].Y = Z1 - Start_Z;
                                as[a].Z = X1 - Start_X;
                            }
                printf("缩短取消 as[%d] Y:%f Z:%f\n",a,as[a].Y,as[a].Z);
                            a++;
                            sign = 0;//跳出补偿
                            c = 0;//为下一次刀补作初始化.
                            Start_Z = 0.;   
                            Start_X = 0.;
                        }
            }//直线接圆弧
else if((data[j].G_pathmode==2 || data[j].G_pathmode==3)&& (cs[c-1].G_pathmode==2 || cs[c-1].G_pathmode==3))//圆弧接圆弧
            {
                if(cs[c-1].G_plane == 18)
                {
                    Z0 = cs[c-1].Z;
                    X0 = cs[c-1].X;
                    K_one=cs[c-1].K;
                    I_one=cs[c-1].I;

                    Z1 = data[j].Z;
                    X1 = data[j].X;
                    K_two=data[j].K;
                    I_two=data[j].I;
                }
                else if(cs[c-1].G_plane == 17)
                {
                    Z0 = cs[c-1].X;
                    X0 = cs[c-1].Y;
                    K_one=cs[c-1].I;
                    I_one=cs[c-1].J;

                    Z1 = data[j].X;
                    X1 = data[j].Y;
                    K_two=data[j].I;
                    I_two=data[j].J;
                }
                else if(cs[c-1].G_plane == 19)
                {
                    Z0 = cs[c-1].Y;
                    X0 = cs[c-1].Z;
                    K_one=cs[c-1].J;
                    I_one=cs[c-1].K;

                    Z1 = data[j].Y;
                    X1 = data[j].Z;
                    K_two=data[j].J;
                    I_two=data[j].K;
                }
                if(cs[c-1].G_coormode == 91)        //G90的情况需要补充......................;
                {
                    d0 = sqrt((X0-I_one)*(X0-I_one)+(Z0-K_one)*(Z0-K_one)); 
                    d1 = sqrt((X1-I_two)*(X1-I_two)+(Z1-K_two)*(Z1-K_two)); 
                    
    if((cs[c-1].G_compensate == 41 && cs[c-1].G_pathmode == 2)||(cs[c-1].G_compensate == 42 && cs[c-1].G_pathmode == 3))
                    {
                        r0x = R*(X0-I_one)/d0;
                        r0z = R*(Z0-K_one)/d0;
                    }
    else if((cs[c-1].G_compensate == 42 && cs[c-1].G_pathmode == 2)||(cs[c-1].G_compensate == 41 && cs[c-1].G_pathmode == 3))
                    {
                        r0x = (-R)*(X0-I_one)/d0;
                        r0z = (-R)*(Z0-K_one)/d0;
                    }

    if((data[j].G_compensate == 41 && data[j].G_pathmode == 2)||(data[j].G_compensate == 42 && data[j].G_pathmode == 3))
                    {
                        r1x = R*(-I_two)/d1;
                        r1z = R*(-K_two)/d1;
                    }
    else if((data[j].G_compensate == 42 && data[j].G_pathmode == 2)||(data[j].G_compensate == 41 && data[j].G_pathmode == 3))
                    {
                        r1x = R*I_two/d1;
                        r1z = R*K_two/d1;
                    }
                    //确定r0x,r0z,r1x,r1z参量;

                    if(cs[c-1].G_pathmode == 2)
                    {
                        KBx0= -(Z0-K_one);
                        KBz0=X0-I_one;
                    }
                    else if(cs[c-1].G_pathmode == 3)
                    {
                        KBx0=Z0-K_one;
                        KBz0= -(X0-I_one);
                    }// 确定KB参量;
                    if(data[j].G_pathmode == 2)
                    {
                        KBx1= K_two;
                        KBz1=-I_two;
                    }
                    else if(data[j].G_pathmode == 3)
                    {
                        KBx1=-K_two;
                        KBz1= I_two;
                    }// 确定KB参量,圆弧起点处的切线;
                }//G91
            
            //判断交接类型:缩短,插入,伸长
                    if( ((r0x*r1x + r0z*r1z) >= 0) && (((r1x-r0x)*KBx0 + (r1z-r0z)*KBz0) >= 0) )//伸长型    
                    {
                        if(KBz0 == 0 && KBz1 != 0)
                        {
                        S1z= Z0+r0z;    S1x=X0+r0x; 
                        S2z= Z0+r1z;    S2x=X0+r1x;K2=KBx1/KBz1;
                        
                        Sz = S1z;
                        Sx = K2*(Sz - S2z) + S2x;//直线斜率无限大时;                    
                        }
                        else if(KBz0 != 0 && KBz1 == 0)
                        {
                        S1z= Z0+r0z;    S1x=X0+r0x;K1=KBx0/KBz0; 
                        S2z= Z0+r1z;    S2x=X0+r1x;
                        
                        Sz = S2z;
                        Sx = K1*(Sz - S1z) + S1x;//直线斜率无限大时;                    
                        }
                        else if((KBx0*KBz1) == (KBx1*KBz0))
                        {
                        Sz = Z0+r0z;
                        Sx = X0+r0x;            
                        }//直线斜率相等时;
                        else
                        {
                        S1z= Z0+r0z;    S1x=X0+r0x;K1=KBx0/KBz0; 
                        S2z= Z0+r1z;    S2x=X0+r1x;K2=KBx1/KBz1;
                        
                        Sz = (S2x - S1x - K2*S2z + K1*S1z)/(K1 - K2);
                        Sx = K1*(Sz - S1z) + S1x;
                        }//伸长线的交点坐标;                    
                    

                        as[a]= cs[c-1];     //赋值,修正
                        if(cs[c-1].G_plane == 18)
                        {
                            as[a].Z = (Z0+r0z)-Start_Z;
                            as[a].X = (X0+r0x)-Start_X;
                            as[a].K = K_one-Start_Z;
                            as[a].I = I_one-Start_X;
                        }
                        else if(cs[c-1].G_plane == 17)
                        {
                            as[a].X = (Z0+r0z)-Start_Z;
                            as[a].Y = (X0+r0x)-Start_X;
                            as[a].I = K_one-Start_Z;
                            as[a].J = I_one-Start_X;
                        }
                        else if(cs[c-1].G_plane == 19)
                        {
                            as[a].Y = (Z0+r0z)-Start_Z;
                            as[a].Z = (X0+r0x)-Start_X;
                            as[a].J = K_one-Start_Z;
                            as[a].K = I_one-Start_X;
                        }
                        a++;//第一段圆弧;

                        //赋值,修正,第一段圆弧的伸长直线;
                        as[a].G_pathmode=1;
                        as[a].G_coormode=91;
                        if(cs[c-1].G_plane == 18)
                        {
                            as[a].Z = Sz-(Z0+r0z);
                            as[a].X = Sx-(X0+r0x);
                        }
                        else if(cs[c-1].G_plane == 17)
                        {
                            as[a].X = Sz-(Z0+r0z);
                            as[a].Y = Sx-(X0+r0x);
                        }
                        else if(cs[c-1].G_plane == 19)
                        {
                            as[a].Y = Sz-(Z0+r0z);
                            as[a].Z = Sx-(X0+r0x);
                        }
                        a++;//第一段圆弧的伸长直线部分;

                        as[a].G_pathmode=1;
                        as[a].G_coormode=91;
                        if(cs[c-1].G_plane == 18)
                        {
                            as[a].Z = (Z0+r1z)-Sz;
                            as[a].X = (X0+r1x)-Sx;
                        }
                        else if(cs[c-1].G_plane == 17)
                        {
                            as[a].X = (Z0+r1z)-Sz;
                            as[a].Y = (X0+r1x)-Sx;
                        }
                        else if(cs[c-1].G_plane == 19)
                        {
                            as[a].Y = (Z0+r1z)-Sz;
                            as[a].Z = (X0+r1x)-Sx;
                        }
                        a++;//第二段圆弧切线的伸长部分;
                        Start_Z = r1z;  
                        Start_X = r1x;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备;
                        
                    }//伸长型;

                    else if(( ((r1x-r0x)*KBx0 + (r1z-r0z)*KBz0)< 0) ||((((r1x-r0x)*KBx0 + (r1z-r0z)*KBz0)== 0)&&cs[c-1].G_compensate == 41 && cs[c-1].G_pathmode == 2)||((((r1x-r0x)*KBx0 + (r1z-r0z)*KBz0)== 0)&&cs[c-1].G_compensate == 42 && cs[c-1].G_pathmode == 3)) //缩短型  
                    {
                printf("缩短性刀补\n");
                        S1z=Z0; S1x=X0;
                    
                        Oz=K_one;   Ox=I_one;//圆心坐标;
                        r_arc=sqrt( (Z0+r0z-K_one)*(Z0+r0z-K_one) + (X0+r0x-I_one)*(X0+r0x-I_one) );//圆弧半径
                        
                        if(Z0+K_two-Oz==0)
                        {
                        S2z = K_one;
                        S2x = S1x;
                        }
                        else if(X0+I_two-Ox==0)
                        {
                        S2z = S1z; 
                        S2x = I_one;
                        }
                        else
                        {
                        Kline=(X0+I_two - Ox)/(Z0+K_two - Oz);//过两圆心的直线斜率;
                        S2z=(Kline/(Kline*Kline+1))*(X0-Ox+Z0/Kline+Kline*Oz);//过两圆心的直线,
                                        //以第一个圆的圆心坐标为点坐标,计算
                                        //出与两圆交点连线的 交点;
                        S2x=Kline*(S2z-Oz)+Ox;
                        }
                        if(S1z == S2z)
                        {
                        Sz1=S1z ;   Sx1=Ox + sqrt(r_arc*r_arc-(S1z-Oz)*(S1z-Oz));
                        Sz2=S1z ;   Sx2=Ox - sqrt(r_arc*r_arc-(S1z-Oz)*(S1z-Oz));
                        }
                        else if(S1x == S2x)
                        {
                        Sz1=Oz + sqrt(r_arc*r_arc-(S1x-Ox)*(S1x-Ox));   Sx1=S1x;
                        Sz2=Oz - sqrt(r_arc*r_arc-(S1x-Ox)*(S1x-Ox));   Sx2=S1x;
                        }
                        else
                        {
                        K_arc=(S2z-S1z)/(S2x-S1x);
                        a_arc=1+K_arc*K_arc;
                        b_arc=2*K_arc*(S2z-Oz-K_arc*S2x-Ox/K_arc);
                        c_arc=(S2z-Oz-K_arc*S2x)*(S2x-Oz-K_arc*S2x)-r_arc*r_arc+Ox*Ox;
                        
                    Sx1=( -b_arc+sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);   Sz1=S2z + K_arc*(Sx1-S2x);
                    Sx2=( -b_arc-sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);   Sz2=S2z + K_arc*(Sx2-S2x);
                        }
                        
    if( ((r1z*KBz0+r1x*KBx0)>=0 && ((Sz2-Sz1)*KBz0+(Sx2-Sx1)*KBx0)<=0) || ((r1z*KBz0+r1x*KBx0)<0 && ((Sz2-Sz1)*KBz0+(Sx2-Sx1)*KBx0)>0) )
                            {
                            Sx=Sx2;
                            Sz=Sz2;
                            }
    else if(((r1z*KBz0+r1x*KBx0)>=0 && ((Sz2-Sz1)*KBz0+(Sx2-Sx1)*KBx0)>0)||((r1z*KBz0+r1x*KBx0)<0 && ((Sz2-Sz1)*KBz0+(Sx2-Sx1)*KBx0)<=0) )
                            {
                            Sx=Sx1;
                            Sz=Sz1;
                            }
                            as[a]= cs[c-1];     //赋值,修正
                            if(cs[c-1].G_plane == 18)
                            {
                                as[a].Z = Sz - Start_Z;
                                as[a].X = Sx - Start_X;
                                as[a].K = K_one-Start_Z;
                                as[a].I = I_one-Start_X;
                            }
                            else if(cs[c-1].G_plane == 17)
                            {
                                as[a].X = Sz - Start_Z;
                                as[a].Y = Sx - Start_X;
                                as[a].I = K_one-Start_Z;
                                                                as[a].J = I_one-Start_X;
                            }
                            else if(cs[c-1].G_plane == 19)
                            {
                                as[a].Y = Sz - Start_Z;
                                as[a].Z = Sx - Start_X;
                                as[a].J = K_one-Start_Z;
                                                                as[a].K = I_one-Start_X;
                            }
                            a++;

                            Start_Z = Sz - Z0;  
                            Start_X = Sx - X0;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备;
                            
                        }//缩短型
                
                    else if(( (((r1z-r0z)*KBz0 +(r1x-r0x)*KBx0) > 0) && ((r0x*r1x + r0z*r1z) < 0) )||((((r1x-r0x)*KBx0 + (r1z-r0z)*KBz0)== 0)&&cs[c-1].G_compensate == 42 && cs[c-1].G_pathmode == 2)||((((r1x-r0x)*KBx0 + (r1z-r0z)*KBz0)== 0)&&cs[c-1].G_compensate == 41 && cs[c-1].G_pathmode == 3))//插入型
                        {
          printf("插入性刀补\n");
                            S1z= Z0 + r0z;  S1x=X0 + r0x; //第一段线段终点处的半径矢量顶点坐标;
                            S2z= Z0 + r1z;  S2x=X0 + r1x;//第二段线起点处的半径矢量顶点坐标;
                                        //S1,S2两点的坐标;

                            if(cs[c-1].G_pathmode ==3)
                            {
                                if((Z0-K_one) > 0) Sign1 = 1;
                                else if((Z0-K_one) == 0) Sign1 = 0;
                                else Sign1 = -1;

                                if((I_one-X0) > 0) Sign2 = 1;
                                else if((I_one-X0) == 0) Sign2 = 0;
                                else Sign2 = -1;
                            }
                            else if(cs[c-1].G_pathmode ==2)
                            {
                                if((K_one-Z0) > 0) Sign1 = 1;
                                else if((K_one-Z0) == 0) Sign1 = 0;
                                else Sign1 = -1;

                                if((X0-I_one) > 0) Sign2 = 1;
                                else if((X0-I_one) == 0) Sign2 = 0;
                                else Sign2 = -1;
                            }
                            if(data[j].G_pathmode ==3)
                            {
                                if(K_two > 0) Sign3 = 1;
                                else if(K_two == 0) Sign3 = 0;
                                else Sign3 = -1;

                                if((-I_two) > 0) Sign4 = 1;
                                else if((-I_two) == 0) Sign4 = 0;
                                else Sign4 = -1;
                            }
                            else if(data[j].G_pathmode ==2)
                            {
                                if((-K_two) > 0) Sign3 = 1;
                                else if((-K_two) == 0) Sign3 = 0;
                                else Sign3 = -1;

                                if(I_two > 0) Sign4 = 1;
                                else if(I_two == 0) Sign4 = 0;
                                else Sign4 = -1;
                            }
                            if(r0x <0) r0x_abs = -r0x;
                            else r0x_abs = r0x;

                            if(r0z <0) r0z_abs = -r0z;
                            else r0z_abs = r0z;

                            if(r1x <0) r1x_abs = -r1x;
                            else r1x_abs = r1x;

                            if(r1z <0) r1z_abs = -r1z;
                            else r1z_abs = r1z;
                            
    //printf("Sig1=%d,%d,%d,%d,r0x=%f,r0z=%f,r1x=%f,r1z=%f\n",Sign1,Sign2,Sign3,Sign4,r0x_abs,r0z_abs,r1x_abs,r1z_abs);

                            S3x = S1x + Sign1*r0z_abs;
                            S3z = S1z + Sign2*r0x_abs;  

                            S4x = S2x + Sign3*r1z_abs;
                            S4z = S2z + Sign4*r1x_abs;  

                            as[a]= cs[c-1];     //赋值,修正,圆弧部分
                            if(cs[c-1].G_plane == 18)
                            {
                                as[a].Z = S1z - Start_Z;
                                as[a].X = S1x - Start_X;
                                as[a].K = K_one-Start_Z;
                                as[a].I = I_one-Start_X;
                            }
                            else if(cs[c-1].G_plane == 17)
                            {
                                as[a].X = S1z - Start_Z;
                                as[a].Y = S1x - Start_X;
                                as[a].I = K_one-Start_Z;
                                                                as[a].J = I_one-Start_X;
                            }
                            else if(cs[c-1].G_plane == 19)
                            {
                                as[a].Y = S1z - Start_Z;
                                as[a].Z = S1x - Start_X;
                                as[a].J = K_one-Start_Z;
                                                                as[a].K = I_one-Start_X;
                            }
                            a++;

                            as[a].G_pathmode=1;
                            as[a].G_coormode=91;
                            if(cs[c-1].G_plane == 18)
                            {
                                as[a].Z = S3z - S1z;
                                as[a].X = S3x - S1x;
                                as[a].row_id=a+1;
                            }
                            else if(cs[c-1].G_plane == 17)
                            {
                                as[a].X = S3z - S1z;
                                as[a].Y = S3x - S1x;
                                as[a].row_id=a+1;
                            }
                            else if(cs[c-1].G_plane == 19)
                            {
                                as[a].Y = S3z - S1z;
                                as[a].Z = S3x - S1x;
                                as[a].row_id=a+1;
                            }
                            a++;
                            
                            as[a].G_pathmode=1;
                            as[a].G_coormode=91;
                            if(cs[c-1].G_plane == 18)
                            {
                                as[a].Z = S4z - S3z;
                                as[a].X = S4x - S3x;
                                as[a].row_id=a+1;
                            }
                            else if(cs[c-1].G_plane == 17)
                            {
                                as[a].X = S4z - S3z;
                                as[a].Y = S4x - S3x;
                                as[a].row_id=a+1;
                            }
                            else if(cs[c-1].G_plane == 19)
                            {
                                as[a].Y = S4z - S3z;
                                as[a].Z = S4x - S3x;
                                as[a].row_id=a+1;
                            }
                            a++;


                            as[a].G_pathmode=1;
                            as[a].G_coormode=91;
                            if(cs[c-1].G_plane == 18)
                            {
                                as[a].Z = S2z - S4z;
                                as[a].X = S2x - S4x;
                                as[a].row_id=a+1;
                            }
                            else if(cs[c-1].G_plane == 17)
                            {
                                as[a].X = S2z - S4z;
                                as[a].Y = S2x - S4x;
                                as[a].row_id=a+1;
                            }
                            else if(cs[c-1].G_plane == 19)
                            {
                                as[a].Y = S2z - S4z;
                                as[a].Z = S2x - S4x;
                                as[a].row_id=a+1;
                            }
                            a++;

                            Start_Z = r1z;  
                            Start_X = r1x;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备;
                        }//插入
            
                            if(data[j].G_compensate == 40)       //取消刀补;
                            {
                                as[a]= data[j];     //赋值,修正
                                if(cs[c-1].G_plane == 18)
                                {
                                    as[a].Z = Z1 - Start_Z;
                                    as[a].X = X1 - Start_X;
                                }
                                else if(cs[c-1].G_plane == 17)
                                {
                                    as[a].X = Z1 - Start_Z;
                                    as[a].Y = X1 - Start_X;
                                }
                                else if(cs[c-1].G_plane == 19)
                                {
                                    as[a].Y = Z1 - Start_Z;
                                    as[a].Z = X1 - Start_X;
                                }
                    printf("缩短取消 as[%d] Y:%f Z:%f\n",a,as[a].Y,as[a].Z);
                                a++;
                                sign = 0;//跳出补偿
                                c = 0;//为下一次刀补作初始化.
                                Start_Z = 0.;   
                                Start_X = 0.;
                            }//取消刀补  */
                }//圆弧接圆弧*/

            }//建立刀补后
         if(data[j].G_compensate != 40)
            {
                cs[c]=data[j];
                //printf("第一段代码\n");
                c++;
            }
        }//进行刀补

        else if(data[j].M != 30)
        {
            as[a]=data[j]; 
            printf("tiao chu bu chang\n");
                a++;
        }   //不需要刀补功能时;
    }       //for
    


    if( (fd2 = open("/dev/rtf2", O_WRONLY | O_NONBLOCK)) < 0)
    {
         User_AlarmMessage.num=12;
         User_AlarmMessage.level=1;
         User_AlarmMessage.message="Can not open FIFO2 in user area!";
         printf("Error opening /dev/rtf2\n");
         return ;
    }
    
//***********************
    file1 = fopen("data.f", "wa");
    if (file1 == NULL)
    {
          printf("文件打开失败\n");
          return ;
    }
//***************************
for(j=0;j<num;i++){printf("%d\t%d\t%f%f%f%f%f\t%f%f%f%f\t%d\t%d\t%d\n%d\t%d\t%d\t%d\n",data[j].row_id,data[j].G_pathmode,data[j].X,data[j].Y,data[j].Z,data[j].B,data[j].C,data[j].I,data[j].J,data[j].K,data[j].R,data[j].M,data[j].S,data[j].D,data[j].G_origin,data[j].G_coormode,data[j].G_plane,data[j].G_compensate);}

/*写FIFO-2译码数据*/
    for(j=0; j<a; j++) 
    {
    
        printf("%d\t%d\t%f%f%f%f%f\t%f%f%f%f\t%d\t%d\t%d\n%d\t%d\t%d\t%d\n",as[j].row_id,as[j].G_pathmode,as[j].X,as[j].Y,as[j].Z,as[j].B,as[j].C,as[j].I,as[j].J,as[j].K,as[j].R,as[j].M,as[j].S,as[j].D,as[j].G_origin,as[j].G_coormode,as[j].G_plane,as[j].G_compensate);
        n1 = write(fd2, &as[j], sizeof(nc_data));
        if( n1 < sizeof(nc_data) ) {
            User_AlarmMessage.num=13;
            User_AlarmMessage.level=1;
            User_AlarmMessage.message="Write FIFO2 Error in user area!";
            printf(" write /dev/rtf2 makes an error!\n"); 
            return;
        }


    fprintf(file1,"%f\t%f\n",as[j].Y,as[j].Z);//

    }
    fclose(file1);//
    close(fd2);
    
    if((fd3 = open("/dev/rtf3", O_WRONLY)) < 0){
         User_AlarmMessage.num=14;
         User_AlarmMessage.level=1;
         User_AlarmMessage.message="Open FIFO3 Error in user area!";
         printf("Error opening /dev/rtf3\n");
         return ;
    }
    write(fd3, 1, sizeof(int)); //写入任意数,触发内核插补线程
    * User_SpeedChkFlag_Shm = 1;//开始速度检测;
    close(fd3);

    memset(ptext, '0', sizeof(ptext));
     *User_Decode_Flag = 2;

    if(reallength == 0) {
        *User_Decode_Flag = 3;
        return; }
    }

    /*坐标处理-"绝对坐标"*/
    /*HWND h1=GetDlgItem (hAAuto, IDC_RTGRAP);
    float WC ,LC;

    m_point point[a+2];     //每行产生一个"绝对"坐标点 

    point[0].X=0;
        point[0].Y=0;

    for(i=0;i<=a;i++)
    {
        double  u1,u2;
        u1=as[i].Z;
        u2=-u1;
        as[i].Z=u2;     //原坐标Y轴乡下,现变坐标向上
        point[i+1].X = point[i].X + as[i].Y*1000;
        point[i+1].Y = point[i].Y + as[i].Z*1000;
    }*/
    
    /*求显示图形的长宽*/
    /*double m , n, a1, b;
    m=n=0;
    a1=b=0;
    for(i=0;i<=(a+1);i++)
    {
        //Y轴
        if(point[i].X >= 0 )
        {
            if(m < point[i].X)
            m = point[i].X;
        }
        else
        {
            if(n>point[i].X)
            n=point[i].X;   
        }
        //Z轴
        if(point[i].Y >=0)
        {
            if(a1<point[i].Y)
            a1 = point[i].Y;
        }
        else
        {
            if(b>point[i].Y)
            b = point[i].Y;
        }
    }
    width=(m -n);
    length=(a1 - b);*/
    //printf("%f %f\n",width,length);
    /*画图准备*/
    /*RECT r1;
    //HDC PCD1= GetDC(h1);
    int angle1, angle2, sx, sy,sr;
    double sm, R1;
    
    GetClientRect(hARtgrap,&r1);
        
    WC=(float)((r1.right-r1.left)/(width*1.2));
    LC=(float)((r1.bottom-r1.top)/(length*1.2));

    if(width>=length)
    {
        mw=((m+n)/2)*WC;
        ml=((a1+b)/2)*WC;
        for(i=0;i<=(a+1);i++)
            {
            point[i].X=point[i].X*WC+(r1.right*0.5-mw);
        point[i].Y=point[i].Y*WC+(r1.bottom*0.5-ml);        //因为窗口坐标Y轴向下,此行代码是为了把显示图像移至窗口中央
            }
    }
    else
    {
        mw=((m+n)/2)*LC;
        ml=((a1+b)/2)*LC;
        for(i=0;i<=(a+1);i++)
                {
                point[i].X=point[i].X*LC+(r1.right*0.5-mw);
                point[i].Y=point[i].Y*LC+(r1.bottom*0.5-ml);
                }
    }
    
    SetPenWidth(PCD2, 20);
    SetPenType(PCD2, PT_SOLID);
    SetPenColor(PCD2, PIXEL_green);
    SetPenJoinStyle(PCD2, PT_JOIN_ROUND);
    SetRasterOperation(PCD2,ROP_XOR);*/
/*下载加载图标*/
/*  HICON myicon1;
    myicon1=LoadIconFromFile(PCD1,"/home/minigui/minigui-res-1.6/icon/w95mbx01.ico",0);
    if(myicon1==0)
    printf("load icon file fail\n");*/

/*  for(i=0;i<=a;i++)
    {   
    
        if (as[i].G_pathmode==1)
        {
        MoveTo(PCD2,(int) point[i].X, (int) point[i].Y);
        LineTo(PCD2,(int) point[i+1].X,(int)point[i+1].Y);
        //DrawIcon(PCD1,(int)(point[i+1].X-3),(int)(point[i+1].Y-3),6,6,myicon1);
        //printf("%f %f\n",point[i+1].X,point[i+1].Y);
        }

        else if(as[i].G_pathmode==2)
            {
            R1=sqrt(as[i].J*as[i].J + as[i].K*as[i].K);
            sm=as[i].Y*as[i].Y + as[i].Z*as[i].Z;
            //printf("%f %f\n",R,sm);
            
            double  m1,m2;
            m1=as[i].K;
            m2=-m1;
            as[i].K=m2;
    
            if(as[i].J>0&&as[i].K==0)       //data[i].K和data[i].Z都是经过取反的,计算时注意
            {
                angle2=180;
                if(as[i].Y*as[i].Z<=0)
                angle1=(int)(180-2*180*asin(sqrt(sm)/(2*R1))/3.14);
                else
                angle1=(int)(2*180*asin(sqrt(sm)/(2*R1))/3.14-180);
            }
            if(as[i].J==0&&as[i].K<0)
            {
            //  printf("ok?\n");
                angle2=-90;
                if(as[i].Y*as[i].Z>=0)
                                angle1=(int)(270-2*180*asin(sqrt(sm)/(2*R1))/3.14);
                                else
                                angle1=(int)(2*180*asin(sqrt(sm)/(2*R1))/3.14-90);

            }
            if(as[i].J<0&&as[i].K==0)
            {
                angle2=0;
                if(as[i].Y*as[i].Z<=0)
                                angle1=(int)(-2*180*asin(sqrt(sm)/(2*R1))/3.14);
                                else
                                angle1=(int)(2*180*asin(sqrt(sm)/(2*R1))/3.14);

            }
            if(as[i].J==0&&as[i].K>0)
            {
                angle2=90;
                if(as[i].Y*as[i].Z>=0)
                                angle1=(int)(-2*180*asin(sqrt(sm)/(2*R1))/3.14+90);
                                else
                                angle1=(int)(90+2*180*asin(sqrt(sm)/(2*R1))/3.14);

            }*/
            /*if(as[i].J>0&&as[i].K<0)          //复杂不规则圆弧需要前处理来生成G代码
            {
                angle2=180*atan(-as[i].K/as[i].J)/3.14+180;
                if(as[i].Z<=as[i].K/as[i].J*as[i].Y)
                angle1=180-(2*180*asin(sqrt(sm)/(2*R1))/3.14+180*atan(as[i].K/as[i].J)/3.14);
                else
                angle1=-(180-2*180*asin(sqrt(sm)/(2*R1))/3.14+180*atan(as[i].K/as[i].J)/3.14);
            }
            if(as[i].J>0&&as[i].K>0)
            {
                angle2=180-180*atan(as[i].K/as[i].J)/3.14;
                if(as[i].Z<=as[i].K/as[i].J*as[i].Y)
                angle1=-2*180*asin(sqrt(sm)/(2*R1))/3.14+90+180*atan(as[i].K/as[i].J)/3.14;
                else
                angle1=-180+2*180*asin(sqrt(sm)/(2*R1))/3.14-180*atan(as[i].K/as[i].J)/3.14;
            }
            if(as[i].J<0&&as[i].K<0)
            {
                angle2=-180*atan(as[i].K/as[i].J)/3.14;
                if(data[i].Z>=as[i].K/as[i].J*as[i].Y)
                angle1=-2*180*asin(sqrt(sm)/(2*R1))/3.14-180*atan(as[i].K/as[i].J)/3.14;
                else
                angle1=2*180*asin(sqrt(sm)/(2*R1))/3.14-180*atan(as[i].K/as[i].J)/3.14;
            }
            if(as[i].J<0&&as[i].K>0)
            {
                angle2=-180*atan(as[i].K/as[i].J)/3.14;
                if(as[i].Z>=as[i].K/as[i].J*as[i].Y)
                angle1=-2*180*asin(sqrt(sm)/(2*R1))/3.14-180*atan(as[i].K/as[i].J)/3.14;
                else
                angle1=2*180*asin(sqrt(sm)/(2*R1))/3.14-180*atan(as[i].K/as[i].J)/3.14;
            }
*/
        /*  if(width>=length)
            {
            sx=(int)(point[i].X+as[i].J*1000*WC);
            sy=(int)(point[i].Y+as[i].K*1000*WC);
            sr=(int)(R1*1000*WC);
            }else
            {
            sx=(int)(point[i].X+as[i].J*1000*LC);
                        sy=(int)(point[i].Y+as[i].K*1000*LC);
                        sr=(int)(R1*1000*LC);

            }
            CircleArc(PCD2,sx,sy,sr,angle1*64,angle2*64);
            printf("%d %d %d %d %d\n",sx,sy,sr,angle1,angle2);
            }
        else if(as[i].G_pathmode==3)
            {
            R1=sqrt(as[i].J*as[i].J + as[i].K*as[i].K);
                        sm=as[i].Y*as[i].Y + as[i].Z*as[i].Z;
                        //printf("%f %f\n",R,sm);
                                                                                                                                               
                        double  m1,m2;
                        m1=as[i].K;
                        m2=-m1;
                        as[i].K=m2;
                                                                                                                                               
                        if(as[i].J>0&&as[i].K==0)           //data[i].K和data[i].Z都是经过取反的,计算时注意
                        {
                                angle1=180;
                                if(as[i].Y*as[i].Z<=0)
                                angle2=(int)(180-2*180*asin(sqrt(sm)/(2*R1))/3.14);
                                else
                                angle2=(int)(2*180*asin(sqrt(sm)/(2*R1))/3.14-180);
                        }
                        if(as[i].J==0&&as[i].K<0)
                        {
                        //      printf("ok?\n");
                                angle1=-90;
                                if(as[i].Y*as[i].Z>=0)
                                angle2=(int)(270-2*180*asin(sqrt(sm)/(2*R1))/3.14);
                                else
                                angle2=(int)(2*180*asin(sqrt(sm)/(2*R1))/3.14-90);
                                                                                                                                               
                        }
            if(as[i].J<0&&as[i].K==0)
                        {
                                angle1=0;
                                if(as[i].Y*as[i].Z<=0)
                                angle2=(int)(-2*180*asin(sqrt(sm)/(2*R1))/3.14);
                                else
                                angle2=(int)(2*180*asin(sqrt(sm)/(2*R1))/3.14);
                                                                                                                                               
                        }
                        if(as[i].J==0&&as[i].K>0)
                        {
                                angle1=90;
                                if(as[i].Y*as[i].Z>=0)
                                angle2=(int)(-2*180*asin(sqrt(sm)/(2*R1))/3.14+90);
                                else
                                angle2=(int)(90+2*180*asin(sqrt(sm)/(2*R1))/3.14);
                                                                                                                                               
                        }*/
        /*
            if(as[i].J>0&&as[i].K<0)                  //复杂不规则圆弧需要前处理来生成G代码
                        {
                                angle1=180*atan(-as[i].K/as[i].J)/3.14+180;
                                if(as[i].Z<=as[i].K/as[i].J*as[i].Y)
                                angle2=180-(2*180*asin(sqrt(sm)/(2*R1))/3.14+180*atan(as[i].K/as[i].J)/3.14);
                                else
                                angle2=-(180-2*180*asin(sqrt(sm)/(2*R1))/3.14+180*atan(as[i].K/as[i].J)/3.14);
                        }
                        if(as[i].J>0&&as[i].K>0)
                        {
                                angle1=180-180*atan(as[i].K/as[i].J)/3.14;
                                if(as[i].Z<=as[i].K/as[i].J*as[i].Y)
                                angle2=-2*180*asin(sqrt(sm)/(2*R1))/3.14+90+180*atan(as[i].K/as[i].J)/3.14;
                                else
                                angle2=-180+2*180*asin(sqrt(sm)/(2*R1))/3.14-180*atan(as[i].K/as[i].J)/3.14;
                        }
                        if(as[i].J<0&&as[i].K<0)
                        {
                                angle1=-180*atan(as[i].K/as[i].J)/3.14;
                                if(as[i].Z>=as[i].K/as[i].J*as[i].Y)
                                angle2=-2*180*asin(sqrt(sm)/(2*R1))/3.14-180*atan(as[i].K/as[i].J)/3.14;
                                else
                                angle2=2*180*asin(sqrt(sm)/(2*R1))/3.14-180*atan(as[i].K/as[i].J)/3.14;
                        }
                        if(as[i].J<0&&as[i].K>0)
                        {
                                angle1=-180*atan(as[i].K/as[i].J)/3.14;
                                if(as[i].Z>=as[i].K/as[i].J*as[i].Y)
                                angle2=-2*180*asin(sqrt(sm)/(2*R1))/3.14-180*atan(as[i].K/as[i].J)/3.14;
                                else
                                angle2=2*180*asin(sqrt(sm)/(2*R1))/3.14-180*atan(as[i].K/as[i].J)/3.14;
                        }
    */
    /*       if(width>=length)
                        {
                        sx=(int)(point[i].X+as[i].J*1000*WC);
                        sy=(int)(point[i].Y+as[i].K*1000*WC);
                        sr=(int)(R1*1000*WC);
                        }else
                        {
                        sx=(int)(point[i].X+as[i].J*1000*LC);
                        sy=(int)(point[i].Y+as[i].K*1000*LC);
                        sr=(int)(R1*1000*LC);
                                                                                                                                               
                        }
                        CircleArc(PCD2,sx,sy,sr,angle1*64,angle2*64);
                        printf("%d %d %d %d %d\n",sx,sy,sr,angle1,angle2);
            
            }
    }
    
//  ReleaseDC(PCD2);
    drawll = 1;*/


}

//******************************************************************************************************************
/*将字符串s中的字符gcode后面的实型数返回, gcode必须是一个字符!*/
double Gcode2d(char* s, const char* gcode)
{
    double dvalue;
    int i;
    char *pos=strstr(s, gcode);
    char temp[25]="";
    for(i=1; isdigit(*(pos+i)) || (*(pos+i))==46 || (*(pos+i))==45; i++) //数字或小数点或负号
      temp[i-1]=*(pos+i);
    dvalue=atof(temp);
    return dvalue;
}
/*将字符串s中的字符gcode后面的整型数返回, gcode必须是一个字符!*/
int Gcode2i(char* s, const char* gcode)
{
    int ivalue;
    int i;
    char *pos=strstr(s, gcode);
    char temp[10]="";
    for(i=1; isdigit(*(pos+i)); i++ ) //只有正整数
      temp[i-1]=*(pos+i);
    ivalue=atoi(temp);
    return ivalue;
}
/*返回字符串p中含有字符串s的个数, s必须只是一个字符*/
int stringnum(char *p, const char* s)
{
    int num=0;
    char* pos;
    if(pos=strstr(p, s)) num++;
    else return 0;
    while (pos=strstr(pos+1, s)) {
        num++;
    }
    return num;
}

//*****************************************************************************************************************888

void autodraw()
{
    if (drawll >= 1)
    {
        HWND hCtrl;
        float datar_x, datar_y, WC, LC;
        double  poix,poiy;
        RECT r1;
    
        GetClientRect(hARtgrap ,&r1);

        poix = datar_x*1000;
        poiy = -datar_y*1000;
    
        WC=(float)((r1.right-r1.left)/(width*1.2));
        LC=(float)((r1.bottom-r1.top)/(length*1.2));

        if(width>=length)
        {
            if(drawll == 0)
            {
            poi_x = poi_x*WC+(r1.right*0.5-mw);
            poi_y = poi_y*WC+(r1.bottom*0.5-ml);
            }
            poix = poix*WC+(r1.right*0.5-mw);
            poiy = poiy*WC+(r1.bottom*0.5-ml);      //因为窗口坐标Y轴向下,此行代码是为了把显示图像移至窗口中央
        }
        else
        {
            if(drawll == 0)
            {
            poi_x = poi_x*LC+(r1.right*0.5-mw);
                    poi_y = poi_y*LC+(r1.bottom*0.5-ml);
            }
                    poix = poix*LC+(r1.right*0.5-mw);
                    poiy = poiy*LC+(r1.bottom*0.5-ml);
        }
    
        SetPenWidth(PCD2, 20);
        SetPenType(PCD2, PT_SOLID);
        SetPenColor(PCD2, PIXEL_green);
        SetPenJoinStyle(PCD2, PT_JOIN_ROUND);
        SetRasterOperation(PCD2,ROP_XOR);

        MoveTo(PCD2,(int) poi_x, (int) poi_y);
        LineTo(PCD2,(int) poix,(int)poiy);

        poi_x = poix;
        poi_y = poiy;

    }
}

