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


extern volatile int *User_SpeedChkFlag_Shm;  //�û����Ƿ�����ٶȼ���־λ�Ĺ����ڴ��ַ;
extern  volatile int *User_Decode_Flag;//�û��������־λ
extern volatile int *User_Counter_Num;//
extern volatile AlarmStruct User_AlarmMessage;

extern HWND hAAuto,hStatus, hARtgrap, hManual;
extern Coordata data_r;   //Relative coor
extern int drawll;

FILE *file1;//********************
static double width=0, length=0, mw=0, ml=0, poi_x=0,poi_y=0;
extern HDC PCD2;

inline double Gcode2d(char* s, const char* gcode); /*���ַ���s���ַ���gcode�����ʵ�����ַ���, gcode������һ���ַ�!*/
inline int Gcode2i(char* s, const char* gcode); /*���ַ���s���ַ���gcode������������ַ���, gcode������һ���ַ�!*/
int stringnum(char *p, const char* s); /*�����ַ���p�к����ַ���s�ĸ���, s������һ���ַ�*/
int row_n = 90;


/*����������*/
void decode(void)
{
    /*ȡ�����ȼ�*/
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
    char *phead;        //����ÿһ���ַ�����ͷָ��
    char *delim ="\n";      //�ûس����ָ����
    memset(ptext, '0', sizeof(ptext));
    
    int sign,Sign1,Sign2,Sign3,Sign4;                        //���߲���״̬��־
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
                        MessageBox (HWND_DESKTOP, "�ļ���ʧ��","Program", MB_OK | MB_ICONSTOP);
                        return ;
                }
    }
    while (1)
    {

        if(*User_Reset_Flag==1){
              close(fd);
              row_n = 90;
              pthread_cancel(id_decode);//�����߳�
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
        
        ptext[i] = 0; //���λ�� ' \0 '                 //����������ݵ�ptext��
        printf("%s\n",ptext);
        //num=stringnum(ptext, "\n");   //�����лس����ĸ���(����)
        //if(num<=0) return;
    
        int a=0;            //iΪdata�±�, i+1Ϊ����

        for(j=0; phead=(0==j ? strtok(ptext, delim) : strtok(NULL, delim)); j++)
        {
            //�к�
            data[j].row_id=j+1;
            //·���岹ģʽ
            if(strstr(phead,"G00")) data[j].G_pathmode=0;
                else if(strstr(phead,"G01")) data[j].G_pathmode=1; 
                else if(strstr(phead,"G02")) data[j].G_pathmode=2; 
                else if(strstr(phead,"G03")) data[j].G_pathmode=3;
                else data[j].G_pathmode = 0;    //(0==j ? 1 : data[j-1].G_pathmode);
            //����ģʽ
            if(strstr(phead,"G90")) data[j].G_coormode=90; 
                else if(strstr(phead,"91")) data[j].G_coormode=91;
                else data[j].G_coormode = 0;        // (0==j ? 91 : data[j-1].G_coormode);
            //����ƽ��  
            if(strstr(phead,"G17")) data[j].G_plane=17;
            else
            if(strstr(phead,"G18")) data[j].G_plane=18;
            else 
            if(strstr(phead,"G19")) data[j].G_plane=19;
            else data[j].G_plane = 0;
            //���߲���
            if(strstr(phead,"G40")) data[j].G_compensate=40;
                else if(strstr(phead,"G41")) {data[j].G_compensate=41; sign=1;}
                else if(strstr(phead,"G42")) {data[j].G_compensate=42; sign=1;}
                else data[j].G_compensate = 0;      // (0==j ? 40 : data[j-1].G_compensate);
            //�Ƿ����趨����ԭ��
            if(strstr(phead,"G92")) data[j].G_origin=1;
            else data[j].G_origin = 0;
            //����ֵ
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
            //M����
            if(strstr(phead,"M")) data[j].M=Gcode2i(phead,"M");
                else data[j].M=0;
            //S����
            if(strstr(phead,"S")) data[j].S=Gcode2i(phead,"S");
            else data[j].S = 0;
            //D����
            if(strstr(phead,"D")) data[j].D=Gcode2i(phead,"D");
            else data[j].D = 0;
        
                
                  printf("cde daxiao wei %d\n",c);

        if(sign == 1)//���е���
        {
            if(c != 0)//����������      
            {
          printf("��������\n");
                if(data[j].G_pathmode == 1 && cs[c-1].G_pathmode ==1)//ֱ�߽�ֱ��
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
                    if(cs[c-1].G_coormode == 91)        //G90�������Ҫ����......................;
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
                //�жϽ�������:����,����,�쳤
                        if( ((r0x*r1x + r0z*r1z) >= 0) && (((r1x-r0x)*X0 + (r1z-r0z)*Z0) >= 0) )//�쳤��||������
                        {                   //�쳤�ͺ������ͳ���һ��,���Ժϲ�Ϊһ�γ���***          
          printf("B �쳤�Ե���\n"); 
                            if(Z0 == 0)
                            {
                            S1z= r0z;   S1x=r0x; //ǰһ��G��������Ӧ�ĵ�����������,���ڱ�ʾ��һ��ֱ��;
                            S2z= Z0+Z1+r1z; S2x=X0+X1+r1x;K2=X1/Z1;//��һG���յ��Ӧ�ĵ�����������,ȷ���ڶ�����;
                            
                            Sz = S1z;
                            Sx = K2*(Sz - S2z) + S2x;//�쳤�ߵĽ�������;                    
                            }
                            else if(Z1 == 0)
                            {
                            S1z= r0z;   S1x=r0x;K1=X0/Z0; 
                            S2z= Z0+Z1+r1z; S2x=X0+X1+r1x;
                            
                            Sz = S2z;
                            Sx = K1*(Sz - S1z) + S1x;//�쳤�ߵĽ�������;                    
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
                            S1z= r0z;   S1x=r0x; K1=X0/Z0;//��һ���߶���㴦�İ뾶ʸ����������;
                            S2z= Z0+Z1+r1z; S2x=X0+X1+r1x; K2=X1/Z1;//�ڶ������յ㴦�İ뾶ʸ����������;
                        
                            Sz = (S2x - S1x - K2*S2z + K1*S1z)/(K1 - K2);
                            Sx = K1*(Sz - S1z) + S1x;
                            }//�쳤�ߵĽ�������;
                            

                            as[a]= cs[c-1];     //��ֵ,����
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
                    printf("�쳤 as[%d] Y:%f Z:%f\n",a,as[a].Y,as[a].Z);
                            a++;

                            Start_Z = Sz - Z0;  
                            Start_X = Sx - X0;//ת�ӵ������ڶ���G���������������,Ϊ��һ�ε�����׼��;
                        }//�쳤
                
                        else if( ((r1x-r0x)*X0 + (r1z-r0z)*Z0) < 0 )//������
                        {
          printf("C �����Ե���\n");
                            if(Z0 == 0)
                            {
                            S1z= r0z;   S1x=r0x; 
                            S2z= Z0+Z1+r1z; S2x=X0+X1+r1x;K2=X1/Z1;
                            
                            Sz = S1z;
                            Sx = K2*(Sz - S2z) + S2x;//�쳤�ߵĽ�������;                    
                            }
                            else if(Z1 == 0)
                            {
                            S1z= r0z;   S1x=r0x;K1=X0/Z0; 
                            S2z= Z0+Z1+r1z; S2x=X0+X1+r1x;
                            
                            Sz = S2z;
                            Sx = K1*(Sz - S1z) + S1x;//�쳤�ߵĽ�������;                    
                            }
                            else
                            {
                            S1z= r0z;   S1x=r0x; K1=X0/Z0;//��һ���߶���㴦�İ뾶ʸ����������;
                            S2z= Z0+Z1+r1z; S2x=X0+X1+r1x; K2=X1/Z1;//�ڶ������յ㴦�İ뾶ʸ����������;
                        
                            Sz = (S2x - S1x - K2*S2z + K1*S1z)/(K1 - K2);
                            Sx = K1*(Sz - S1z) + S1x;
                            }//�쳤�ߵĽ�������;
                            

                            as[a]= cs[c-1];     //��ֵ,����
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
                    printf("����  as[%d] Y:%f Z:%f\n",a,as[a].Y,as[a].Z);
                            a++;

                            Start_Z = Sz - Z0;  
                            Start_X = Sx - X0;//ת�ӵ������ڶ���G���������������,Ϊ��һ�ε�����׼��;
                            
                        }//������

                    else if( (((r1z-r0z)*Z0 +(r1x-r0x)*X0) >= 0) && ((r0x*r1x + r0z*r1z) < 0) )//������
                        {
          printf("D �����Ե���\n");
                            S1z= Z0 + r0z;  S1x=X0 + r0x; //��һ���߶��յ㴦�İ뾶ʸ����������;
                            S2z= Z0 + r1z;  S2x=X0 + r1x;//�ڶ�������㴦�İ뾶ʸ����������;
                                        //S1,S2���������;
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

                            as[a]= cs[c-1];     //��ֵ,����
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
                    printf("�����һ as[%d] Y:%f Z:%f\n",a,as[a].Y,as[a].Z);
                            a++;
                            
                            as[a]= cs[c-1];     //���������
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
                    printf("����ڶ� as[%d] Y:%f Z:%f\n",a,as[a].Y,as[a].Z);
                            a++;

                            Start_Z = S4z - Z0; 
                            Start_X = S4x - X0;//ת�ӵ������ڶ���G���������������,Ϊ��һ�ε�����׼��;
                        }//����
                            
                        if(data[i].G_compensate == 40)       //ȡ������;
                            {
          printf("quxiao����2\n");
                                as[a]= data[j];     //��ֵ,����
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
                    printf("����ȡ�� as[%d] Y:%f Z:%f\n",a,as[a].Y,as[a].Z);
                                a++;
                                sign = 0;//��������
                                c = 0;//Ϊ��һ�ε�������ʼ��.
                                Start_Z = 0.;   
                                Start_X = 0.;
                            }//ȡ������;            
                }//ֱ�߽�ֱ��;


                else if(data[j].G_pathmode == 1 && (cs[c-1].G_pathmode ==3 || cs[c-1].G_pathmode ==2))//Բ����ֱ��
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
                    if(cs[c-1].G_coormode == 91)        //G90�������Ҫ����......................;
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
                        }//ȷ��r0x,r0z,r1x,r1z����;

                        if(cs[c-1].G_pathmode == 2)
                        {
                            KBx0= -(Z0-K_one);
                            KBz0=X0-I_one;
                        }
                        else if(cs[c-1].G_pathmode == 3)
                        {
                            KBx0=Z0-K_one;
                            KBz0= -(X0-I_one);
                        }// ȷ��KB����;
                    }//G91
                //�жϽ�������:����,����,�쳤
                        if( ((r0x*r1x + r0z*r1z) >= 0) && (((r1x-r0x)*KBx0 + (r1z-r0z)*KBz0) >= 0) )//�쳤��    
                            {
    printf("�쳤�Ե���\n");
                            if(KBz0 == 0 && Z1 != 0)
                            {
                            S1z= Z0+r0z;    S1x=X0+r0x; 
                            S2z= Z0+r1z;    S2x=X0+r1x;K2=X1/Z1;
                            
                            Sz = S1z;
                            Sx = K2*(Sz - S2z) + S2x;//ֱ��б�����޴�ʱ;                    
                            }
                            else if(KBz0 != 0 && Z1 == 0)
                            {
                            S1z= Z0+r0z;    S1x=X0+r0x;K1=KBx0/KBz0; 
                            S2z= Z0+r1z;    S2x=X0+r1x;
                            
                            Sz = S2z;
                            Sx = K1*(Sz - S1z) + S1x;//ֱ��б�����޴�ʱ;                    
                            }
                            else if((KBx0*Z1) == (X1*KBz0))
                            {
                            
                            Sz = Z0+r0z;
                            Sx = X0+r0x;            
                            }//ֱ��б�����ʱ;
                            else
                            {
                            S1z= Z0+r0z;    S1x=X0+r0x;K1=KBx0/KBz0; 
                            S2z= Z0+r1z;    S2x=X0+r1x;K2=X1/Z1;
                            
                            Sz = (S2x - S1x - K2*S2z + K1*S1z)/(K1 - K2);
                            Sx = K1*(Sz - S1z) + S1x;
                            }//�쳤�ߵĽ�������;                    
                        

                            as[a]= cs[c-1];     //��ֵ,����
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
                            a++;//��һ��Բ��;

                            as[a]= data[i];     //��ֵ,����
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
                            a++;//Բ�����ߵ��쳤����;

                            Start_Z = Sz - Z0;  
                            Start_X = Sx - X0;//ת�ӵ������ڶ���G���������������,Ϊ��һ�ε�����׼��;
                            
                        }//�쳤��;

                        else if( (((r1x-r0x)*KBx0 + (r1z-r0z)*KBz0) < 0)||(((r1x-r0x)*KBx0 + (r1z-r0z)*KBz0)==0&&cs[c-1].G_compensate == 41 && cs[c-1].G_pathmode == 2)||(((r1x-r0x)*KBx0 + (r1z-r0z)*KBz0)==0&&cs[c-1].G_compensate == 42 && cs[c-1].G_pathmode== 3)) //������ 
                        {
          printf("�����Ե���\n");
                            S1z=Z0+r1z; S1x=X0+r1x;
                            S2z=Z0+Z1+r1z;  S2x=X0+X1+r1x;//��Բ���ཻ��ֱ�ߵ�������;

                            Oz=K_one;   Ox=I_one;//Բ������;
                            r_arc=sqrt( (Z0+r0z-K_one)*(Z0+r0z-K_one) + (X0+r0x-I_one)*(X0+r0x-I_one) );//Բ���뾶

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
                            }//�쳤�ߵĽ�������;
                            
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
                            as[a]= cs[c-1];     //��ֵ,����
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
                            Start_X = Sx - X0;//ת�ӵ������ڶ���G���������������,Ϊ��һ�ε�����׼��;
                            
                        }//������
                

                    else if( ((((r1z-r0z)*KBz0 +(r1x-r0x)*KBx0) >0)&&((r0x*r1x + r0z*r1z) < 0))||(((r1x-r0x)*KBx0 + (r1z-r0z)*KBz0)==0&&cs[c-1].G_compensate == 42 && cs[c-1].G_pathmode ==2)||(((r1x-r0x)*KBx0 + (r1z-r0z)*KBz0)==0&&cs[c-1].G_compensate == 41 && cs[c-1].G_pathmode ==3) )//������
                        {printf("�����Ե���\n");        
                            S1z= Z0 + r0z;  S1x=X0 + r0x; //��һ���߶��յ㴦�İ뾶ʸ����������;
                            S2z= Z0 + r1z;  S2x=X0 + r1x;//�ڶ�������㴦�İ뾶ʸ����������;
                                        //S1,S2���������;

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

                            as[a]= cs[c-1];     //��ֵ,����,Բ������
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

                            as[a]= data[j];     //��ֵ,����,�쳤��ֱ��S1S3����;
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
                            
                            as[a]= data[j];     //�������S3S4����;
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
                            Start_X = S4x - X0;//ת�ӵ������ڶ���G���������������,Ϊ��һ�ε�����׼��;
                        }//����         
                            if(data[j].G_compensate == 40)       //ȡ������;
                            {
                                as[a]= data[j];     //��ֵ,����
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
                    printf("����ȡ�� as[%d] Y:%f Z:%f\n",a,as[a].Y,as[a].Z);
                                a++;
                                sign = 0;//��������
                                c = 0;//Ϊ��һ�ε�������ʼ��.
                                Start_Z = 0.;   
                                Start_X = 0.;
                            }//ȡ������ 
                }//Բ����ֱ��

            
                else if((data[j].G_pathmode == 2 || data[j].G_pathmode == 3) && cs[c-1].G_pathmode ==1)//ֱ�߽�Բ��
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
                    if(cs[c-1].G_coormode == 91)        //G90�������Ҫ����......................;
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
                        //ȷ��r0x,r0z,r1x,r1z����;

                        if(data[j].G_pathmode == 2)
                        {
                            KBx1= K_two;
                            KBz1=-I_two;
                        }
                        else if(data[j].G_pathmode == 3)
                        {
                            KBx1=-K_two;
                            KBz1= I_two;
                        }// ȷ��KB����,Բ����㴦������;
                    }//G91
                //�жϽ�������:����,����,�쳤
                        if( ((r0x*r1x + r0z*r1z) >= 0) && (((r1x-r0x)*X0 + (r1z-r0z)*Z0) >= 0) )//�쳤��    
                            {
     printf("�쳤�Ե���\n");
                            if(KBz1 == 0 && Z0 != 0)
                            {
                            S1z= Z0+r0z;    S1x=X0+r0x; K1=X0/Z0;
                            S2z= Z0+r1z;    S2x=X0+r1x;
                            
                            Sz = S2z;
                            Sx = K1*(Sz - S1z) + S1x;//ֱ��б�����޴�ʱ;    
                            }
                            else if(KBz1 != 0 && Z0 == 0)
                            {
                            S1z= Z0+r0z;    S1x=X0+r0x; 
                            S2z= Z0+r1z;    S2x=X0+r1x;K2=KBx1/KBz1;
                            
                            Sz = S1z;
                            Sx = K2*(Sz - S2z) + S2x;//ֱ��б�����޴�ʱ;                    
                            }
                            else if((KBx1*Z0) == (X0*KBz1))
                            {
                            S1z= Z0+r0z;    S1x=X0+r0x; 
                                                        S2z= Z0+r1z;    S2x=X0+r1x;
                            Sz = Z0+r0z;
                            Sx = X0+r0x;            
                            }//ֱ��б�����ʱ;
                            else
                            {
                            S1z= Z0+r0z;    S1x=X0+r0x;K1=X0/Z0; 
                            S2z= Z0+r1z;    S2x=X0+r1x;K2=KBx1/KBz1;
                            
                            Sz = (S2x - S1x - K2*S2z + K1*S1z)/(K1 - K2);
                            Sx = K1*(Sz - S1z) + S1x;
                            }//�쳤�ߵĽ�������;                    
                        
                            as[a]= cs[c-1];     //��ֵ,������һ��ֱ��
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


                            as[a]= cs[c-1];     //��ֵ,����Բ�����ֵ��쳤����S-S2;
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
                            a++;//��һ��Բ��;

                            Start_Z = S2z - Z0; 
                            Start_X = S2x - X0;//ת�ӵ������ڶ���G���������������,Ϊ��һ�ε�����׼��;
                        }//�쳤��;

                        else if((((r1x-r0x)*X0 + (r1z-r0z)*Z0) < 0)||((((r1x-r0x)*X0 + (r1z-r0z)*Z0)== 0)&&data[j].G_compensate == 41 && data[j].G_pathmode == 2)||((((r1x-r0x)*X0 + (r1z-r0z)*Z0)== 0)&&data[j].G_compensate == 42 && data[j].G_pathmode == 3)) //������   
                        {
                            S1z=r0z;    S1x=r0x;
                            S2z=Z0+r0z; S2x=X0+r0x;//��Բ���ཻ��ֱ�ߵ�������;

                            Oz=Z0+K_two;    Ox=X0+I_two;//Բ������;
                            r_arc=sqrt( (Z0+r1z-Oz)*(Z0+r1z-Oz) + (X0+r1x-Ox)*(X0+r1x-Ox) );//Բ���뾶

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
                            }//�쳤�ߵĽ�������;
                            
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
                            as[a]= cs[c-1];     //��ֵ,����
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
                            Start_X = Sx - X0;//ת�ӵ������ڶ���G���������������,Ϊ��һ�ε�����׼��;
                        }//������

                    else if(((((r1z-r0z)*Z0 +(r1x-r0x)*X0) > 0) && ((r0x*r1x + r0z*r1z) < 0))||((((r1z-r0z)*Z0 +(r1x-r0x)*X0)== 0)&&data[j].G_compensate == 41 && data[j].G_pathmode == 3)||((((r1z-r0z)*Z0 +(r1x-r0x)*X0)== 0)&&data[j].G_compensate == 42 && data[j].G_pathmode == 2))//������
                        {
                                S1z= Z0 + r0z;  S1x=X0 + r0x; //��һ���߶��յ㴦�İ뾶ʸ����������;
                            S2z= Z0 + r1z;  S2x=X0 + r1x;//�ڶ�������㴦�İ뾶ʸ����������;
                                        //S1,S2���������;
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
                        
                        as[a]= cs[c-1];     //��ֵ,����,��һ��ֱ��
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

                        as[a]= cs[c-1];     //��ֵ,����,�����ֱ��S3S4����;
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
                        
                        as[a]= cs[c-1];     //�������S4S2����;
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
                        Start_X = S2x - X0;//ת�ӵ������ڶ���G���������������,Ϊ��һ�ε�����׼��;
                    }//����         
                        if(data[j].G_compensate == 40)       //ȡ������;
                        {
                            as[a]= data[j];     //��ֵ,����
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
                printf("����ȡ�� as[%d] Y:%f Z:%f\n",a,as[a].Y,as[a].Z);
                            a++;
                            sign = 0;//��������
                            c = 0;//Ϊ��һ�ε�������ʼ��.
                            Start_Z = 0.;   
                            Start_X = 0.;
                        }
            }//ֱ�߽�Բ��
else if((data[j].G_pathmode==2 || data[j].G_pathmode==3)&& (cs[c-1].G_pathmode==2 || cs[c-1].G_pathmode==3))//Բ����Բ��
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
                if(cs[c-1].G_coormode == 91)        //G90�������Ҫ����......................;
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
                    //ȷ��r0x,r0z,r1x,r1z����;

                    if(cs[c-1].G_pathmode == 2)
                    {
                        KBx0= -(Z0-K_one);
                        KBz0=X0-I_one;
                    }
                    else if(cs[c-1].G_pathmode == 3)
                    {
                        KBx0=Z0-K_one;
                        KBz0= -(X0-I_one);
                    }// ȷ��KB����;
                    if(data[j].G_pathmode == 2)
                    {
                        KBx1= K_two;
                        KBz1=-I_two;
                    }
                    else if(data[j].G_pathmode == 3)
                    {
                        KBx1=-K_two;
                        KBz1= I_two;
                    }// ȷ��KB����,Բ����㴦������;
                }//G91
            
            //�жϽ�������:����,����,�쳤
                    if( ((r0x*r1x + r0z*r1z) >= 0) && (((r1x-r0x)*KBx0 + (r1z-r0z)*KBz0) >= 0) )//�쳤��    
                    {
                        if(KBz0 == 0 && KBz1 != 0)
                        {
                        S1z= Z0+r0z;    S1x=X0+r0x; 
                        S2z= Z0+r1z;    S2x=X0+r1x;K2=KBx1/KBz1;
                        
                        Sz = S1z;
                        Sx = K2*(Sz - S2z) + S2x;//ֱ��б�����޴�ʱ;                    
                        }
                        else if(KBz0 != 0 && KBz1 == 0)
                        {
                        S1z= Z0+r0z;    S1x=X0+r0x;K1=KBx0/KBz0; 
                        S2z= Z0+r1z;    S2x=X0+r1x;
                        
                        Sz = S2z;
                        Sx = K1*(Sz - S1z) + S1x;//ֱ��б�����޴�ʱ;                    
                        }
                        else if((KBx0*KBz1) == (KBx1*KBz0))
                        {
                        Sz = Z0+r0z;
                        Sx = X0+r0x;            
                        }//ֱ��б�����ʱ;
                        else
                        {
                        S1z= Z0+r0z;    S1x=X0+r0x;K1=KBx0/KBz0; 
                        S2z= Z0+r1z;    S2x=X0+r1x;K2=KBx1/KBz1;
                        
                        Sz = (S2x - S1x - K2*S2z + K1*S1z)/(K1 - K2);
                        Sx = K1*(Sz - S1z) + S1x;
                        }//�쳤�ߵĽ�������;                    
                    

                        as[a]= cs[c-1];     //��ֵ,����
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
                        a++;//��һ��Բ��;

                        //��ֵ,����,��һ��Բ�����쳤ֱ��;
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
                        a++;//��һ��Բ�����쳤ֱ�߲���;

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
                        a++;//�ڶ���Բ�����ߵ��쳤����;
                        Start_Z = r1z;  
                        Start_X = r1x;//ת�ӵ������ڶ���G���������������,Ϊ��һ�ε�����׼��;
                        
                    }//�쳤��;

                    else if(( ((r1x-r0x)*KBx0 + (r1z-r0z)*KBz0)< 0) ||((((r1x-r0x)*KBx0 + (r1z-r0z)*KBz0)== 0)&&cs[c-1].G_compensate == 41 && cs[c-1].G_pathmode == 2)||((((r1x-r0x)*KBx0 + (r1z-r0z)*KBz0)== 0)&&cs[c-1].G_compensate == 42 && cs[c-1].G_pathmode == 3)) //������  
                    {
                printf("�����Ե���\n");
                        S1z=Z0; S1x=X0;
                    
                        Oz=K_one;   Ox=I_one;//Բ������;
                        r_arc=sqrt( (Z0+r0z-K_one)*(Z0+r0z-K_one) + (X0+r0x-I_one)*(X0+r0x-I_one) );//Բ���뾶
                        
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
                        Kline=(X0+I_two - Ox)/(Z0+K_two - Oz);//����Բ�ĵ�ֱ��б��;
                        S2z=(Kline/(Kline*Kline+1))*(X0-Ox+Z0/Kline+Kline*Oz);//����Բ�ĵ�ֱ��,
                                        //�Ե�һ��Բ��Բ������Ϊ������,����
                                        //������Բ�������ߵ� ����;
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
                            as[a]= cs[c-1];     //��ֵ,����
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
                            Start_X = Sx - X0;//ת�ӵ������ڶ���G���������������,Ϊ��һ�ε�����׼��;
                            
                        }//������
                
                    else if(( (((r1z-r0z)*KBz0 +(r1x-r0x)*KBx0) > 0) && ((r0x*r1x + r0z*r1z) < 0) )||((((r1x-r0x)*KBx0 + (r1z-r0z)*KBz0)== 0)&&cs[c-1].G_compensate == 42 && cs[c-1].G_pathmode == 2)||((((r1x-r0x)*KBx0 + (r1z-r0z)*KBz0)== 0)&&cs[c-1].G_compensate == 41 && cs[c-1].G_pathmode == 3))//������
                        {
          printf("�����Ե���\n");
                            S1z= Z0 + r0z;  S1x=X0 + r0x; //��һ���߶��յ㴦�İ뾶ʸ����������;
                            S2z= Z0 + r1z;  S2x=X0 + r1x;//�ڶ�������㴦�İ뾶ʸ����������;
                                        //S1,S2���������;

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

                            as[a]= cs[c-1];     //��ֵ,����,Բ������
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
                            Start_X = r1x;//ת�ӵ������ڶ���G���������������,Ϊ��һ�ε�����׼��;
                        }//����
            
                            if(data[j].G_compensate == 40)       //ȡ������;
                            {
                                as[a]= data[j];     //��ֵ,����
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
                    printf("����ȡ�� as[%d] Y:%f Z:%f\n",a,as[a].Y,as[a].Z);
                                a++;
                                sign = 0;//��������
                                c = 0;//Ϊ��һ�ε�������ʼ��.
                                Start_Z = 0.;   
                                Start_X = 0.;
                            }//ȡ������  */
                }//Բ����Բ��*/

            }//����������
         if(data[j].G_compensate != 40)
            {
                cs[c]=data[j];
                //printf("��һ�δ���\n");
                c++;
            }
        }//���е���

        else if(data[j].M != 30)
        {
            as[a]=data[j]; 
            printf("tiao chu bu chang\n");
                a++;
        }   //����Ҫ��������ʱ;
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
          printf("�ļ���ʧ��\n");
          return ;
    }
//***************************
for(j=0;j<num;i++){printf("%d\t%d\t%f%f%f%f%f\t%f%f%f%f\t%d\t%d\t%d\n%d\t%d\t%d\t%d\n",data[j].row_id,data[j].G_pathmode,data[j].X,data[j].Y,data[j].Z,data[j].B,data[j].C,data[j].I,data[j].J,data[j].K,data[j].R,data[j].M,data[j].S,data[j].D,data[j].G_origin,data[j].G_coormode,data[j].G_plane,data[j].G_compensate);}

/*дFIFO-2��������*/
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
    write(fd3, 1, sizeof(int)); //д��������,�����ں˲岹�߳�
    * User_SpeedChkFlag_Shm = 1;//��ʼ�ٶȼ��;
    close(fd3);

    memset(ptext, '0', sizeof(ptext));
     *User_Decode_Flag = 2;

    if(reallength == 0) {
        *User_Decode_Flag = 3;
        return; }
    }

    /*���괦��-"��������"*/
    /*HWND h1=GetDlgItem (hAAuto, IDC_RTGRAP);
    float WC ,LC;

    m_point point[a+2];     //ÿ�в���һ��"����"����� 

    point[0].X=0;
        point[0].Y=0;

    for(i=0;i<=a;i++)
    {
        double  u1,u2;
        u1=as[i].Z;
        u2=-u1;
        as[i].Z=u2;     //ԭ����Y������,�ֱ���������
        point[i+1].X = point[i].X + as[i].Y*1000;
        point[i+1].Y = point[i].Y + as[i].Z*1000;
    }*/
    
    /*����ʾͼ�εĳ���*/
    /*double m , n, a1, b;
    m=n=0;
    a1=b=0;
    for(i=0;i<=(a+1);i++)
    {
        //Y��
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
        //Z��
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
    /*��ͼ׼��*/
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
        point[i].Y=point[i].Y*WC+(r1.bottom*0.5-ml);        //��Ϊ��������Y������,���д�����Ϊ�˰���ʾͼ��������������
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
/*���ؼ���ͼ��*/
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
    
            if(as[i].J>0&&as[i].K==0)       //data[i].K��data[i].Z���Ǿ���ȡ����,����ʱע��
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
            /*if(as[i].J>0&&as[i].K<0)          //���Ӳ�����Բ����Ҫǰ����������G����
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
                                                                                                                                               
                        if(as[i].J>0&&as[i].K==0)           //data[i].K��data[i].Z���Ǿ���ȡ����,����ʱע��
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
            if(as[i].J>0&&as[i].K<0)                  //���Ӳ�����Բ����Ҫǰ����������G����
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
/*���ַ���s�е��ַ�gcode�����ʵ��������, gcode������һ���ַ�!*/
double Gcode2d(char* s, const char* gcode)
{
    double dvalue;
    int i;
    char *pos=strstr(s, gcode);
    char temp[25]="";
    for(i=1; isdigit(*(pos+i)) || (*(pos+i))==46 || (*(pos+i))==45; i++) //���ֻ�С����򸺺�
      temp[i-1]=*(pos+i);
    dvalue=atof(temp);
    return dvalue;
}
/*���ַ���s�е��ַ�gcode���������������, gcode������һ���ַ�!*/
int Gcode2i(char* s, const char* gcode)
{
    int ivalue;
    int i;
    char *pos=strstr(s, gcode);
    char temp[10]="";
    for(i=1; isdigit(*(pos+i)); i++ ) //ֻ��������
      temp[i-1]=*(pos+i);
    ivalue=atoi(temp);
    return ivalue;
}
/*�����ַ���p�к����ַ���s�ĸ���, s����ֻ��һ���ַ�*/
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
            poiy = poiy*WC+(r1.bottom*0.5-ml);      //��Ϊ��������Y������,���д�����Ϊ�˰���ʾͼ��������������
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

