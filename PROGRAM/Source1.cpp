void g4142Fuction(HWND hWnd,int *sign, nc_data *dataChild,nc_data asChild[],int *num,double *Start_X, double *Start_Y,int *c, nc_data *csChild)
{
	
	

	int Sign1,Sign2,Sign3,Sign4;                        //���߲���״̬��־
    double Y0,X0,Y1,X1,R,d0,d1,r0y,r0x_abs,r0x,r0z_abs,r1y,r1y_abs,r1x,r1x_abs;
    double S1x,S1y,S2x,S2y,S3x,S3y,S4x,S4y,K1,K2,Sx,Sy,Sx1,Sy1,Sx2,Sy2;
    double J_one,I_one,J_two,I_two,KBy0,KBx0,KBy1,KBx1,Oy,Ox,K_arc,a_arc,b_arc,c_arc;
    double Kline,r_arc;
	char chTemp1[101];
	int decimal, tsign;
	
	
	
	R=dataChild->D;
		
	//R=(tapepm->Hworkthick - tapepm->Hreference)*tan(dataChild->T/1000);
	//R2=tapepm->Hreference*tan(dataChild->T/1000);
		


	if(*c != 0)//����������      
            {
				
				   
                if(dataChild->G_pathmode == 1 && csChild->G_pathmode ==1)//ֱ�߽�ֱ��
                {
					
                 
					if(csChild->G_plane == 18)
                    {
                        X0 = csChild->Z;
                        Y0 = csChild->X;
                        X1 = dataChild->Z;
                        Y1 = dataChild->X;
                    }
                    else if(csChild->G_plane == 17)
                    {
                        X0 = csChild->X;
                        Y0 = csChild->Y;
                        X1 = dataChild->X;
                        Y1 = dataChild->Y;
                    }
                    else if(csChild->G_plane == 19)
                    {
                        X0 = csChild->Y;
                        Y0 = csChild->Z;
                        X1 = dataChild->Y;
                        Y1 = dataChild->Z;

												
                    }
                    if(csChild->G_coormode == 91)        //G90�������Ҫ����......................;
                    {
                        d0 = sqrt(X0*X0 +Y0*Y0);    
                        d1 = sqrt(X1*X1 +Y1*Y1);  
						
                        if(csChild->G_compensate == 41)
                        {
                            r0y = R*X0/d0;
                            r0x = (-R)*Y0/d0;

                            r1y = R*X1/d1;
                            r1x = (-R)*Y1/d1;                           
                        }
                        else if(csChild->G_compensate == 42)
                        {
                            r0y = (-R)*X0/d0;
                            r0x = R*Y0/d0;

                            r1y = (-R)*X1/d1;
                            r1x = R*Y1/d1;
						
						
                        }
                    }
					
					    //MessageBox(hWnd,_fcvt( r0y, 7, &decimal, &tsign ),NULL,NULL); 
						
                //�жϽ�������:����,����,�쳤
                        if( ((r0y*r1y + r0x*r1x) >= 0) && (((r1y-r0y)*Y0 + (r1x-r0x)*X0) >= 0) )//�쳤��||������
                        {                   //�쳤�ͺ������ͳ���һ��,���Ժϲ�Ϊһ�γ���***          
               // �쳤�Ե���
							
                            if(X0 == 0)
                            {
                            S1x= r0x;   S1y=r0y; //ǰһ��G��������Ӧ�ĵ�����������,���ڱ�ʾ��һ��ֱ��;
                            S2x= X0+X1+r1x; S2y=Y0+Y1+r1y;K2=Y1/X1;//��һG���յ��Ӧ�ĵ�����������,ȷ���ڶ�����;
                            
                            Sx = S1x;
                            Sy = K2*(Sx - S2x) + S2y;//�쳤�ߵĽ�������;                    
                            }
                            else if(X1 == 0)
                            {
                            S1x= r0x;   S1y=r0y;K1=Y0/X0; 
                            S2x= X0+X1+r1x; S2y=Y0+Y1+r1y;
                            
                            Sx = S2x;
                            Sy = K1*(Sx - S1x) + S1y;//�쳤�ߵĽ�������;                    
                            }
                            else if((Y1*X0) == (Y0*X1))
                            {
                            S1x= r0x;    S1y=r0y; 
                                                        S2x= X0+X1+r1x;    S2y=Y0+Y1+r1y;
                            Sx = X0+r0x;
                            Sy = Y0+r0y;
                            }   
                            else
                            {
                            S1x= r0x;   S1y=r0y; K1=Y0/X0;//��һ���߶���㴦�İ뾶ʸ����������;
                            S2x= X0+X1+r1x; S2y=Y0+Y1+r1y; K2=Y1/X1;//�ڶ������յ㴦�İ뾶ʸ����������;
                        
                            Sx = (S2y - S1y - K2*S2x + K1*S1x)/(K1 - K2);
                            Sy = K1*(Sx - S1x) + S1y;
                            }//�쳤�ߵĽ�������;
                            

                            asChild[*num] = *csChild;     //��ֵ,����
                            if(csChild->G_plane == 18)
                            {
                                asChild[*num].Z = Sx - (*Start_X);
                                asChild[*num].X = Sy - (*Start_Y);
                            }
                            else if(csChild->G_plane == 17)
                            {
                                asChild[*num].X = Sx - (*Start_X);
                                asChild[*num].Y= Sy - (*Start_Y);
                            }
                            else if(csChild->G_plane == 19)
                            {
                                asChild[*num].Y= Sx - (*Start_X);
                                asChild[*num].Z = Sy - (*Start_Y);
                            }
               

                           *num=*num+1;

                            *Start_X = Sx - X0;  
                            *Start_Y = Sy - Y0;//ת�ӵ������ڶ���G���������������,Ϊ��һ�ε�����׼��;

							
						
                        }//�쳤
                
                        else if( ((r1y-r0y)*Y0 + (r1x-r0x)*X0) < 0 )//������
                        {
                       //�����Ե���
							

                            if(X0 == 0)
                            {
                            S1x= r0x;   S1y=r0y; 
                            S2x= X0+X1+r1x; S2y=Y0+Y1+r1y;K2=Y1/X1;
                            
                            Sx = S1x;
                            Sy = K2*(Sx - S2x) + S2y;//�쳤�ߵĽ�������;                    
                            }
                            else if(X1 == 0)
                            {
                            S1x= r0x;   S1y=r0y;K1=Y0/X0; 
                            S2x= X0+X1+r1x; S2y=Y0+Y1+r1y;
                            
                            Sx = S2x;
                            Sy = K1*(Sx - S1x) + S1y;//�쳤�ߵĽ�������;                    
                            }
                            else
                            {
                            S1x= r0x;   S1y=r0y; K1=Y0/X0;//��һ���߶���㴦�İ뾶ʸ����������;
                            S2x= X0+X1+r1x; S2y=Y0+Y1+r1y; K2=Y1/X1;//�ڶ������յ㴦�İ뾶ʸ����������;
                        
                            Sx = (S2y - S1y - K2*S2x + K1*S1x)/(K1 - K2);
                            Sy = K1*(Sx - S1x) + S1y;
                            }//�쳤�ߵĽ�������;
                            

                            asChild[*num]= *csChild;     //��ֵ,����
							
                          
							if(csChild->G_plane == 18)
                            {
                                asChild[*num].Z = Sx - (*Start_X);
                                asChild[*num].X = Sy - (*Start_Y);
                            }
                            else if(csChild->G_plane == 17)
                            {
                                asChild[*num].X = Sx - (*Start_X);
                                asChild[*num].Y= Sy - (*Start_Y);
                            }
                            else if(csChild->G_plane == 19)
                            {
                                asChild[*num].Y= Sx - (*Start_X);
                                asChild[*num].Z = Sy - (*Start_Y);
                            }
                    
                            *num= *num+1;

                            *Start_X = Sx - X0;  
                            *Start_Y = Sy - Y0;//ת�ӵ������ڶ���G���������������,Ϊ��һ�ε�����׼��;
							
                            
                        }//������

                    else if( (((r1x-r0x)*X0 +(r1y-r0y)*Y0) >= 0) && ((r0y*r1y + r0x*r1x) < 0) )//������
                        {
                // �����Ե���
							
                            S1x= X0 + r0x;  S1y=Y0 + r0y; //��һ���߶��յ㴦�İ뾶ʸ����������;
                            S2x= X0 + r1x;  S2y=Y0 + r1y;//�ڶ�������㴦�İ뾶ʸ����������;
                                        //S1,S2���������;
                            if(Y0 > 0) Sign1 = 1;
                            else if(Y0 == 0) Sign1 = 0;
                            else Sign1 = -1;
                                
                            if(X0 > 0) Sign2 = 1;
                            else if(X0 == 0) Sign2 = 0;
                            else Sign2 = -1;

                            if(-Y1 > 0) Sign3 = 1;
                            else if(-Y1 == 0) Sign3 = 0;
                            else Sign3 = -1;

                            if(-X1 > 0) Sign4 = 1;
                            else if(-X1 == 0) Sign4 = 0;
                            else Sign4 = -1;
                            
                            if(r0y <0) r0x_abs = -r0y;
                            else r0x_abs = r0y;

                            if(r0x <0) r0z_abs = -r0x;
                            else r0z_abs = r0x;

                            if(r1y <0) r1y_abs = -r1y;
                            else r1y_abs = r1y;

                            if(r1x <0) r1x_abs = -r1x;
                            else r1x_abs = r1x;
                            
   
                            S3y = S1y + Sign1*r0z_abs;
                            S3x = S1x + Sign2*r0x_abs;  

                            S4y = S2y + Sign3*r1x_abs;
                            S4x = S2x + Sign4*r1y_abs;  
   

                            asChild[*num]= *csChild;     //��ֵ,����
                            if(csChild->G_plane == 18)
                            {
                                asChild[*num].Z = S3x - (*Start_X);
                                asChild[*num].X = S3y - (*Start_Y);
                            }
                            else if(csChild->G_plane == 17)
                            {
                                asChild[*num].X = S3x - (*Start_X);
                                asChild[*num].Y= S3y - (*Start_Y);
                            }
                            else if(csChild->G_plane == 19)
                            {
                                asChild[*num].Y= S3x - (*Start_X);
                                asChild[*num].Z = S3y - (*Start_Y);
                            }
                    
                           *num=*num+1;
                            
                            asChild[*num]= *csChild;     //���������
                            if(csChild->G_plane == 18)
                            {
                                asChild[*num].Z = S4x - S3x;
                                asChild[*num].X = S4y - S3y;
                                asChild[*num].row_id=*num+1;
                            }
                            else if(csChild->G_plane == 17)
                            {
                                asChild[*num].X = S4x - S3x;
                                asChild[*num].Y= S4y - S3y;
                                asChild[*num].row_id=*num+1;
                            }
                            else if(csChild->G_plane == 19)
                            {
                                asChild[*num].Y= S4x - S3x;
                                asChild[*num].Z = S4y - S3y;
                                asChild[*num].row_id=*num+1;
                            }
                   
                           *num=*num+1;

                            *Start_X = S4x - X0; 
                            *Start_Y = S4y - Y0;//ת�ӵ������ڶ���G���������������,Ϊ��һ�ε�����׼��;
                        }//����
                       
						 	
						
                        if(dataChild->G_compensate == 40)       //ȡ������;
                            {
                       //ȡ������
								
							
									
                                asChild[*num]= *dataChild;     //��ֵ,����
								
								//asChild[*num]= *csChild;     //��ֵ,����
                                
								 if(csChild->G_plane == 18)
                                {
									asChild[*num].Z = X1 - (*Start_X);
                                    asChild[*num].X = Y1 - (*Start_Y);
                                }
                                else if(csChild->G_plane == 17)
                                {
                                    asChild[*num].X = X1 - (*Start_X);
                                    asChild[*num].Y= Y1 - (*Start_Y);
                                }
                                else if(csChild->G_plane == 19)
                                {
									
								
                                    asChild[*num].Y= X1 - (*Start_X);
                                    asChild[*num].Z = Y1 - (*Start_Y);
									
                                }
								
                                 *num=*num+1;
                                *sign = 0;//��������
                                *c = 0;//Ϊ��һ�ε�������ʼ��.
                                *Start_X = 0.;   
                                *Start_Y = 0.;
                          }//ȡ������;            
                }//ֱ�߽�ֱ��;


                else if(dataChild->G_pathmode == 1 && (csChild->G_pathmode ==3 || csChild->G_pathmode ==2))//Բ����ֱ��
                {
//********************************************************************************
                    if(csChild->G_plane == 18)
                    {
                        X0 = csChild->Z;
                        Y0 = csChild->X;
                        I_one=csChild->K;
                        J_one=csChild->I;

                        X1 = dataChild->Z;
                        Y1 = dataChild->X;
                    }
                    else if(csChild->G_plane == 17)
                    {
                        X0 = csChild->X;
                        Y0 = csChild->Y;
						I_one=csChild->I;
                        J_one=csChild->J;

                        X1 = dataChild->X;
                        Y1 = dataChild->Y;
                    }
                    else if(csChild->G_plane == 19)
                    {
                        X0 = csChild->Y;
                        Y0 = csChild->Z;
                        I_one=csChild->J;
                        J_one=csChild->K;

                        X1 = dataChild->Y;
                        Y1 = dataChild->Z;
                    }
                    if(csChild->G_coormode == 91)        //G90�������Ҫ����......................;
                    {
                        d0 = sqrt((Y0-J_one)*(Y0-J_one)+(X0-I_one)*(X0-I_one)); 
                        d1 = sqrt(X1*X1 +Y1*Y1);    
                        
        if((csChild->G_compensate == 41 && csChild->G_pathmode == 2)||(csChild->G_compensate == 42 && csChild->G_pathmode == 3))
                        {
                            r0y = R*(Y0-J_one)/d0;
                            r0x = R*(X0-I_one)/d0;
                        }
        else if((csChild->G_compensate == 42 && csChild->G_pathmode == 2)||(csChild->G_compensate == 41 && csChild->G_pathmode == 3))
                        {
                            r0y = (-R)*(Y0-J_one)/d0;
                            r0x = (-R)*(X0-I_one)/d0;
                        }

                        if(dataChild->G_compensate == 41)
                        {
                            r1y = R*X1/d1;
                            r1x = (-R)*Y1/d1;                               
                        }       
                        else if(dataChild->G_compensate == 42)
                        {
                            r1y = (-R)*X1/d1;
                            r1x = R*Y1/d1;
                        }//ȷ��r0y,r0x,r1y,r1x����;

                        if(csChild->G_pathmode == 2)
                        {
                            KBy0= -(X0-I_one);
                            KBx0=Y0-J_one;
                        }
                        else if(csChild->G_pathmode == 3)
                        {
                            KBy0=X0-I_one;
                            KBx0= -(Y0-J_one);
                        }// ȷ��KB����;
                    }//G91
                //�жϽ�������:����,����,�쳤
                        if( ((r0y*r1y + r0x*r1x) >= 0) && (((r1y-r0y)*KBy0 + (r1x-r0x)*KBx0) >= 0) )//�쳤��    
                             {
                      //�쳤�Ե���
                            if(KBx0 == 0 && X1 != 0)
                            {
                            S1x= X0+r0x;    S1y=Y0+r0y; 
                            S2x= X0+r1x;    S2y=Y0+r1y;K2=Y1/X1;
                            
                            Sx = S1x;
                            Sy = K2*(Sx - S2x) + S2y;//ֱ��б�����޴�ʱ;                    
                            }
                            else if(KBx0 != 0 && X1 == 0)
                            {
                            S1x= X0+r0x;    S1y=Y0+r0y;K1=KBy0/KBx0; 
                            S2x= X0+r1x;    S2y=Y0+r1y;
                            
                            Sx = S2x;
                            Sy = K1*(Sx - S1x) + S1y;//ֱ��б�����޴�ʱ;                    
                            }
                            else if((KBy0*X1) == (Y1*KBx0))
                            {
                            
                            Sx = X0+r0x;
                            Sy = Y0+r0y;            
                            }//ֱ��б�����ʱ;
                            else
                            {
                            S1x= X0+r0x;    S1y=Y0+r0y;K1=KBy0/KBx0; 
                            S2x= X0+r1x;    S2y=Y0+r1y;K2=Y1/X1;
                            
                            Sx = (S2y - S1y - K2*S2x + K1*S1x)/(K1 - K2);
                            Sy = K1*(Sx - S1x) + S1y;
                            }//�쳤�ߵĽ�������;                    
                        

                            asChild[*num] = *csChild;     //��ֵ,����
                            if(csChild->G_plane == 18)
                            {
                                asChild[*num].Z = (X0+r0x)-(*Start_X);
                                asChild[*num].X = (Y0+r0y)-(*Start_Y);
                                asChild[*num].K = I_one-(*Start_X);
                                asChild[*num].I = J_one-(*Start_Y);
                            }
                            else if(csChild->G_plane == 17)
                            {
                                asChild[*num].X = (X0+r0x)-(*Start_X);
                                asChild[*num].Y= (Y0+r0y)-(*Start_Y);
                                asChild[*num]. I = I_one-(*Start_X);
                                                                asChild[*num].J = J_one-(*Start_Y);
                            }
                            else if(csChild->G_plane == 19)
                            {
                                asChild[*num].Y= (X0+r0x)-(*Start_X);
                                asChild[*num].Z = (Y0+r0y)-(*Start_Y);
                                asChild[*num].J = I_one-(*Start_X);
                                                                asChild[*num].K = J_one-(*Start_Y);
                            }

                             *num=*num+1;//��һ��Բ��;

                            asChild[*num]= *dataChild;     //��ֵ,����
                            if(csChild->G_plane == 18)
                            {
                                asChild[*num].Z = Sx-(X0+r0x);
                                asChild[*num].X = Sy-(Y0+r0y);
                            }
                            else if(csChild->G_plane == 17)
                            {
                                asChild[*num].X = Sx-(X0+r0x);
                                asChild[*num].Y= Sy-(Y0+r0y);
                            }
                            else if(csChild->G_plane == 19)
                            {
                                asChild[*num].Y= Sx-(X0+r0x);
                                asChild[*num].Z = Sy-(Y0+r0y);
                            }
                             *num=*num+1;//Բ�����ߵ��쳤����;

                            *Start_X = Sx - X0;  
                            *Start_Y = Sy - Y0;//ת�ӵ������ڶ���G���������������,Ϊ��һ�ε�����׼��;
                            
                        }//�쳤��;

                        else if( (((r1y-r0y)*KBy0 + (r1x-r0x)*KBx0) < 0)||(((r1y-r0y)*KBy0 + (r1x-r0x)*KBx0)==0&&csChild->G_compensate == 41 && csChild->G_pathmode == 2)||(((r1y-r0y)*KBy0 + (r1x-r0x)*KBx0)==0&&csChild->G_compensate == 42 && csChild->G_pathmode== 3)) //������
                        {
                       //�����Ե���
                            S1x=X0+r1x; S1y=Y0+r1y;
                            S2x=X0+X1+r1x;  S2y=Y0+Y1+r1y;//��Բ���ཻ��ֱ�ߵ�������;

                            Ox=I_one;   Oy=J_one;//Բ������;
                            r_arc=sqrt( (X0+r0x-I_one)*(X0+r0x-I_one) + (Y0+r0y-J_one)*(Y0+r0y-J_one) );//Բ���뾶

                            if(S1x == S2x)
                            {
                            Sx1=S1x ;   Sy1=Oy + sqrt(r_arc*r_arc-(S1x-Ox)*(S1x-Ox));
                            Sx2=S1x ;   Sy2=Oy - sqrt(r_arc*r_arc-(S1x-Ox)*(S1x-Ox));
                            }
                            else if(S1y == S2y)
                            {
                            Sx1=Ox + sqrt(r_arc*r_arc-(S1y-Oy)*(S1y-Oy));   Sy1=S1y;
                            Sx2=Ox - sqrt(r_arc*r_arc-(S1y-Oy)*(S1y-Oy));   Sy2=S1y;
                            }
                            else
                            {
                            K_arc=(S2x-S1x)/(S2y-S1y);
                            a_arc=1+K_arc*K_arc;
                            b_arc=2*K_arc*(S2x-Ox-K_arc*S2y-Oy/K_arc);
                            c_arc=(S2x-Ox-K_arc*S2y)*(S2y-Ox-K_arc*S2y)-r_arc*r_arc+Oy*Oy;
                            
                        Sy1=( -b_arc+sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);   Sx1=S2x + K_arc*(Sy1-S2y);
                        Sy2=( -b_arc-sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);   Sx2=S2x + K_arc*(Sy2-S2y);
                            }//�쳤�ߵĽ�������;
                            
    //if( ((S1x*X1+S1y*Y1)>=0 && ((Sx2-Sx1)*X1+(Sy2-Sy1)*Y1)<=0) || ((S1x*X1+S1y*Y1)<0 && ((Sx2-Sx1)*X1+(Sy2-Sy1)*Y1)>0) )
    if( ((r0x*X1+r0y*Y1)>=0 && ((Sx2-Sx1)*X1+(Sy2-Sy1)*Y1)<=0) || ((r0x*X1+r0y*Y1)<0 && ((Sx2-Sx1)*X1+(Sy2-Sy1)*Y1)>0) )
                            {
                            Sy=Sy1;
                            Sx=Sx1;
                            }
    //else if( ((S1x*X1+S1y*Y1)>=0 && ((Sx2-Sx1)*X1+(Sy2-Sy1)*Y1)>0) || ((S1x*X1+S1y*Y1)<0 && ((Sx2-Sx1)*X1+(Sy2-Sy1)*Y1)<=0) )
    else if( ((r0x*X1+r0y*Y1)>=0 && ((Sx2-Sx1)*X1+(Sy2-Sy1)*Y1)>0) || ((r0x*X1+r0y*Y1)<0 && ((Sx2-Sx1)*X1+(Sy2-Sy1)*Y1)<=0) )
                            {
                            Sy=Sy2;
                            Sx=Sx2;
                            }
                            asChild[*num]= *csChild;     //��ֵ,����
                            if(csChild->G_plane == 18)
                            {
                                asChild[*num].Z = Sx - (*Start_X);
                                asChild[*num].X = Sy - (*Start_Y);
                                asChild[*num].K = I_one-(*Start_X);
                                asChild[*num].I = J_one-(*Start_Y);
                            }
                            else if(csChild->G_plane == 17)
                            {
                                asChild[*num].X = Sx - (*Start_X);
                                asChild[*num].Y= Sy - (*Start_Y);
                                asChild[*num].I = I_one-(*Start_X);
                                                                asChild[*num].J = J_one-(*Start_Y);
                            }
                            else if(csChild->G_plane == 19)
                            {
                                asChild[*num].Y= Sx - (*Start_X);
                                asChild[*num].Z = Sy - (*Start_Y);
                                asChild[*num].J = I_one-(*Start_X);
                                                                asChild[*num].K = J_one-(*Start_Y);
                            }
                             *num=*num+1;

                            *Start_X = Sx - X0;  
                            *Start_Y = Sy - Y0;//ת�ӵ������ڶ���G���������������,Ϊ��һ�ε�����׼��;
                            
                        }//������
                

                    else if( ((((r1x-r0x)*KBx0 +(r1y-r0y)*KBy0) >0)&&((r0y*r1y + r0x*r1x) < 0))||(((r1y-r0y)*KBy0 + (r1x-r0x)*KBx0)==0&&csChild->G_compensate == 42 && csChild->G_pathmode ==2)||(((r1y-r0y)*KBy0 + (r1x-r0x)*KBx0)==0&&csChild->G_compensate == 41 && csChild->G_pathmode ==3) )//������
                        {    
                            S1x= X0 + r0x;  S1y=Y0 + r0y; //��һ���߶��յ㴦�İ뾶ʸ����������;
                            S2x= X0 + r1x;  S2y=Y0 + r1y;//�ڶ�������㴦�İ뾶ʸ����������;
                                        //S1,S2���������;

                            if(csChild->G_pathmode ==3)
                            {
                                if((X0-I_one) > 0) Sign1 = 1;
                                else if((X0-I_one) == 0) Sign1 = 0;
                                else Sign1 = -1;

                                if((J_one-Y0) > 0) Sign2 = 1;
                                else if((J_one-Y0) == 0) Sign2 = 0;
                                else Sign2 = -1;
                            }
                            else if(csChild->G_pathmode ==2)
                            {
                                if((I_one-X0) > 0) Sign1 = 1;
                                else if((I_one-X0) == 0) Sign1 = 0;
                                else Sign1 = -1;

                                if((Y0-J_one) > 0) Sign2 = 1;
                                else if((Y0-J_one) == 0) Sign2 = 0;
                                else Sign2 = -1;
                            }
                            if((-Y1) > 0) Sign3 = 1;
                            else if((-Y1) == 0) Sign3 = 0;
                            else Sign3 = -1;

                            if(-X1 > 0) Sign4 = 1;
                            else if(-X1 == 0) Sign4 = 0;
                            else Sign4 = -1;
                            
                            if(r0y <0) r0x_abs = -r0y;
                            else r0x_abs = r0y;

                            if(r0x <0) r0z_abs = -r0x;
                            else r0z_abs = r0x;

                            if(r1y <0) r1y_abs = -r1y;
                            else r1y_abs = r1y;

                            if(r1x <0) r1x_abs = -r1x;
                            else r1x_abs = r1x;
                            
    
                            S3y = S1y + Sign1*r0z_abs;
                            S3x = S1x + Sign2*r0x_abs;  

                            S4y = S2y + Sign3*r1x_abs;
                            S4x = S2x + Sign4*r1y_abs;  
    
                            asChild[*num]= *csChild;     //��ֵ,����,Բ������
                            if(csChild->G_plane == 18)
                            {
                                asChild[*num].Z = S1x - (*Start_X);
                                asChild[*num].X = S1y - (*Start_Y);
                                asChild[*num].K = I_one-(*Start_X);
                                asChild[*num].I = J_one-(*Start_Y);
                            }
                            else if(csChild->G_plane == 17)
                            {
                                asChild[*num].X = S1x - (*Start_X);
                                asChild[*num].Y= S1y - (*Start_Y);
                                asChild[*num].I = I_one-(*Start_X);
                                                                asChild[*num].J = J_one-(*Start_Y);
                            }
                            else if(csChild->G_plane == 19)
                            {
                                asChild[*num].Y= S1x - (*Start_X);
                                asChild[*num].Z = S1y - (*Start_Y);
                                asChild[*num].J = I_one-(*Start_X);
                                                                asChild[*num].K = J_one-(*Start_Y);
                            }
                             *num=*num+1;

                            asChild[*num]= *dataChild;     //��ֵ,����,�쳤��ֱ��S1S3����;
                            if(csChild->G_plane == 18)
                            {
                                asChild[*num].Z = S3x - S1x;
                                asChild[*num].X = S3y - S1y;
                                asChild[*num].row_id=*num+1;
                            }
                            else if(csChild->G_plane == 17)
                            {
                                asChild[*num].X = S3x - S1x;
                                asChild[*num].Y= S3y - S1y;
                                asChild[*num].row_id=*num+1;
                            }
                            else if(csChild->G_plane == 19)
                            {
                                asChild[*num].Y= S3x - S1x;
                                asChild[*num].Z = S3y - S1y;
                                asChild[*num].row_id=*num+1;
                            }
                            *num=*num+1;
                            
                            asChild[*num]= *dataChild;     //�������S3S4����;
                            if(csChild->G_plane == 18)
                            {
                                asChild[*num].Z = S4x - S3x;
                                asChild[*num].X = S4y - S3y;
                                asChild[*num].row_id=*num+1;
                            }
                            else if(csChild->G_plane == 17)
                            {
                                asChild[*num].X = S4x - S3x;
                                asChild[*num].Y= S4y - S3y;
                                asChild[*num].row_id=*num+1;
                            }
                            else if(csChild->G_plane == 19)
                            {
                                asChild[*num].Y= S4x - S3x;
                                asChild[*num].Z = S4y - S3y;
                                asChild[*num].row_id=*num+1;
                            }
                             *num=*num+1;

                            *Start_X = S4x - X0; 
                            *Start_Y = S4y - Y0;//ת�ӵ������ڶ���G���������������,Ϊ��һ�ε�����׼��;
                        }//����         
                            if(dataChild->G_compensate == 40)       //ȡ������;
                            {
                                asChild[*num]= *dataChild;     //��ֵ,����
                                if(csChild->G_plane == 18)
                                {
                                    asChild[*num].Z = X1 - (*Start_X);
                                    asChild[*num].X = Y1 - (*Start_Y);
                                }
                                else if(csChild->G_plane == 17)
                                {
                                    asChild[*num].X = X1 - (*Start_X);
                                    asChild[*num].Y= Y1 - (*Start_Y);
                                }
                                else if(csChild->G_plane == 19)
                                {
                                    asChild[*num].Y= X1 - (*Start_X);
                                    asChild[*num].Z = Y1 - (*Start_Y);
                                }
                   //����ȡ�� 
                                 *num=*num+1;
                                *sign = 0;//��������
                                *c = 0;//Ϊ��һ�ε�������ʼ��.
                                *Start_X = 0.;   
                                *Start_Y = 0.;
                            }//ȡ������ 
                }//Բ����ֱ��

            
                else if((dataChild->G_pathmode == 2 || dataChild->G_pathmode == 3) && csChild->G_pathmode ==1)//ֱ�߽�Բ��
                {
                    if(csChild->G_plane == 18)
                    {
                        X0 = csChild->Z;
                        Y0 = csChild->X;

                        X1 = dataChild->Z;
                        Y1 = dataChild->X;
                        I_two=dataChild->K;
                        J_two=dataChild->I;
                    }
                    else if(csChild->G_plane == 17)
                    {
                        X0 = csChild->X;
                        Y0 = csChild->Y;

                        X1 = dataChild->X;
                        Y1 = dataChild->Y;
                        I_two=dataChild->I;
                        J_two=dataChild->J;
                    }
                    else if(csChild->G_plane == 19)
                    {
                        X0 = csChild->Y;
                        Y0 = csChild->Z;

                        X1 = dataChild->Y;
                        Y1 = dataChild->Z;
                        I_two=dataChild->J;
                        J_two=dataChild->K;
                    }
                    if(csChild->G_coormode == 91)        //G90�������Ҫ����......................;
                    {
                        d0 = sqrt(X0*X0 +Y0*Y0);    
                        d1 = sqrt((Y1-J_two)*(Y1-J_two)+(X1-I_two)*(X1-I_two)); 
                        
                        if(csChild->G_compensate == 41)
                        {
                            r0y = R*X0/d0;
                            r0x = (-R)*Y0/d0;                               
                        }       
                        else if(csChild->G_compensate == 42)
                        {
                            r0y = (-R)*X0/d0;
                            r0x = R*Y0/d0;
                        }

                if(dataChild->G_compensate == 41 && dataChild->G_pathmode == 2)
                        {
                            r1y = R*(-J_two)/d1;
                            r1x = R*(-I_two)/d1;
                        }
                else if(dataChild->G_compensate == 42 && dataChild->G_pathmode == 3)
                        {
                            r1y = R*(-J_two)/d1;
                                                        r1x = R*(-I_two)/d1;
                                                }
                else if(dataChild->G_compensate == 42 && dataChild->G_pathmode == 2)
                        {
                            r1y = R*J_two/d1;
                            r1x = R*I_two/d1;

                        }
                else if(dataChild->G_compensate == 41 && dataChild->G_pathmode == 3)
                        {
                            r1y = R*J_two/d1;
                                                        r1x = R*I_two/d1;
                        }
                        //ȷ��r0y,r0x,r1y,r1x����;

                        if(dataChild->G_pathmode == 2)
                        {
                            KBy1= I_two;
                            KBx1=-J_two;
                        }
                        else if(dataChild->G_pathmode == 3)
                        {
                            KBy1=-I_two;
                            KBx1= J_two;
                        }// ȷ��KB����,Բ����㴦������;
                    }//G91
                //�жϽ�������:����,����,�쳤
                        if( ((r0y*r1y + r0x*r1x) >= 0) && (((r1y-r0y)*Y0 + (r1x-r0x)*X0) >= 0) )//�쳤��    
                            {
                     //�쳤�Ե���
                            if(KBx1 == 0 && X0 != 0)
                            {
                            S1x= X0+r0x;    S1y=Y0+r0y; K1=Y0/X0;
                            S2x= X0+r1x;    S2y=Y0+r1y;
                            
                            Sx = S2x;
                            Sy = K1*(Sx - S1x) + S1y;//ֱ��б�����޴�ʱ;    
                            }
                            else if(KBx1 != 0 && X0 == 0)
                            {
                            S1x= X0+r0x;    S1y=Y0+r0y; 
                            S2x= X0+r1x;    S2y=Y0+r1y;K2=KBy1/KBx1;
                            
                            Sx = S1x;
                            Sy = K2*(Sx - S2x) + S2y;//ֱ��б�����޴�ʱ;                    
                            }
                            else if((KBy1*X0) == (Y0*KBx1))
                            {
                            S1x= X0+r0x;    S1y=Y0+r0y; 
                                                        S2x= X0+r1x;    S2y=Y0+r1y;
                            Sx = X0+r0x;
                            Sy = Y0+r0y;            
                            }//ֱ��б�����ʱ;
                            else
                            {
                            S1x= X0+r0x;    S1y=Y0+r0y;K1=Y0/X0; 
                            S2x= X0+r1x;    S2y=Y0+r1y;K2=KBy1/KBx1;
                            
                            Sx = (S2y - S1y - K2*S2x + K1*S1x)/(K1 - K2);
                            Sy = K1*(Sx - S1x) + S1y;
                            }//�쳤�ߵĽ�������;                    
                        
                            asChild[*num]= *csChild;     //��ֵ,������һ��ֱ��
                            if(csChild->G_plane == 18)
                            {
                                asChild[*num].Z = Sx - (*Start_X);
                                asChild[*num].X = Sy - (*Start_Y);
                            }
                            else if(csChild->G_plane == 17)
                            {
                                asChild[*num].X = Sx - (*Start_X);
                                asChild[*num].Y= Sy - (*Start_Y);
                            }
                            else if(csChild->G_plane == 19)
                            {
                                asChild[*num].Y= Sx - (*Start_X);
                                asChild[*num].Z = Sy - (*Start_Y);
                            }
                             *num=*num+1;


                            asChild[*num]= *csChild;     //��ֵ,����Բ�����ֵ��쳤����S-S2;
                            if(csChild->G_plane == 18)
                            {
                                asChild[*num].Z = S2x - Sx;
                                asChild[*num].X = S2y - Sy;
                            }
                            else if(csChild->G_plane == 17)
                            {
                                asChild[*num].X = S2x - Sx;
                                asChild[*num].Y= S2y - Sy;
                            }
                            else if(csChild->G_plane == 19)
                            {
                                asChild[*num].Y= S2x - Sx;
                                asChild[*num].Z = S2y - Sy;
                            
                            }
                             *num=*num+1;//��һ��Բ��;

                            *Start_X = S2x - X0; 
                            *Start_Y = S2y - Y0;//ת�ӵ������ڶ���G���������������,Ϊ��һ�ε�����׼��;
                        }//�쳤��;

                        else if((((r1y-r0y)*Y0 + (r1x-r0x)*X0) < 0)||((((r1y-r0y)*Y0 + (r1x-r0x)*X0)== 0)&&dataChild->G_compensate == 41 && dataChild->G_pathmode == 2)||((((r1y-r0y)*Y0 + (r1x-r0x)*X0)== 0)&&dataChild->G_compensate == 42 && dataChild->G_pathmode == 3)) //������
                        {
                            S1x=r0x;    S1y=r0y;
                            S2x=X0+r0x; S2y=Y0+r0y;//��Բ���ཻ��ֱ�ߵ�������;

                            Ox=X0+I_two;    Oy=Y0+J_two;//Բ������;
                            r_arc=sqrt( (X0+r1x-Ox)*(X0+r1x-Ox) + (Y0+r1y-Oy)*(Y0+r1y-Oy) );//Բ���뾶

                            if(S1x == S2x)
                            {
                            Sx1=S1x ;   Sy1=Oy + sqrt(r_arc*r_arc-(S1x-Ox)*(S1x-Ox));
                            Sx2=S1x ;   Sy2=Oy - sqrt(r_arc*r_arc-(S1x-Ox)*(S1x-Ox));
                            }
                            else if(S1y == S2y)
                            {
                            Sx1=Ox + sqrt(r_arc*r_arc-(S1y-Oy)*(S1y-Oy));   Sy1=S1y;
                            Sx2=Ox - sqrt(r_arc*r_arc-(S1y-Oy)*(S1y-Oy));   Sy2=S1y;
                            }
                            else
                            {
                            K_arc=(S2x-S1x)/(S2y-S1y);
                            a_arc=1+K_arc*K_arc;
                            b_arc=2*K_arc*(S2x-Ox-K_arc*S2y-Oy/K_arc);
                            c_arc=(S2x-Ox-K_arc*S2y)*(S2y-Ox-K_arc*S2y)-r_arc*r_arc+Oy*Oy;
                            
                        Sy1=( -b_arc+sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);   Sx1=S2x + K_arc*(Sy1-S2y);
                        Sy2=( -b_arc-sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);   Sx2=S2x + K_arc*(Sy2-S2y);
                            }//�쳤�ߵĽ�������;
                            
    //if( ((S2x*X0+S2y*Y0)>=0 && ((Sx2-Sx1)*X0+(Sy2-Sy1)*Y0)<=0) || ((S2x*X0+S2y*Y0)<0 && ((Sx2-Sx1)*X0+(Sy2-Sy1)*Y0)>0) )
    if( ((r1x*X0+r1y*Y0)>=0 && ((Sx2-Sx1)*X0+(Sy2-Sy1)*Y0)<=0) || ((r1x*X0+r1y*Y0)<0 && ((Sx2-Sx1)*X0+(Sy2-Sy1)*Y0)>0) )
                            {
                            Sy=Sy1;
                            Sx=Sx1;
                            }
    //else if( ((S2x*X0+S2y*Y0)>=0 && ((Sx2-Sx1)*X0+(Sy2-Sy1)*Y0)>0) || ((S2x*X0+S2y*Y0)<0 && ((Sx2-Sx1)*X0+(Sy2-Sy1)*Y0)<=0) )
    else if( ((r1x*X0+r1y*Y0)>=0 && ((Sx2-Sx1)*X0+(Sy2-Sy1)*Y0)>0) || ((r1x*X0+r1y*Y0)<0 && ((Sx2-Sx1)*X0+(Sy2-Sy1)*Y0)<=0) )
                            {
                            Sy=Sy2;
                            Sx=Sx2;
                            }
                            asChild[*num]= *csChild;     //��ֵ,����
                            if(csChild->G_plane == 18)
                            {
                                asChild[*num].Z = Sx - (*Start_X);
                                asChild[*num].X = Sy - (*Start_Y);
                            }
                            else if(csChild->G_plane == 17)
                            {
                                asChild[*num].X = Sx - (*Start_X);
                                asChild[*num].Y= Sy - (*Start_Y);
                            }
                            else if(csChild->G_plane == 19)
                            {
                                asChild[*num].Y= Sx - (*Start_X);
                                asChild[*num].Z = Sy - (*Start_Y);
                            }
                            *num=*num+1;

                            *Start_X = Sx - X0;  
                            *Start_Y = Sy - Y0;//ת�ӵ������ڶ���G���������������,Ϊ��һ�ε�����׼��;
                        }//������

                    else if(((((r1x-r0x)*X0 +(r1y-r0y)*Y0) > 0) && ((r0y*r1y + r0x*r1x) < 0))||((((r1x-r0x)*X0 +(r1y-r0y)*Y0)== 0)&&dataChild->G_compensate == 41 && dataChild->G_pathmode == 3)||((((r1x-r0x)*X0 +(r1y-r0y)*Y0)== 0)&&dataChild->G_compensate == 42 && dataChild->G_pathmode == 2))//������
                        {
                                S1x= X0 + r0x;  S1y=Y0 + r0y; //��һ���߶��յ㴦�İ뾶ʸ����������;
                            S2x= X0 + r1x;  S2y=Y0 + r1y;//�ڶ�������㴦�İ뾶ʸ����������;
                                        //S1,S2���������;
                            if(Y0 > 0) Sign1 = 1;
                            else if(Y0 == 0) Sign1 = 0;
                            else Sign1 = -1;
                                
                            if(X0 > 0) Sign2 = 1;
                            else if(X0 == 0) Sign2 = 0;
                            else Sign2 = -1;

                            if(dataChild->G_pathmode ==3)
                            {
                                if(I_two > 0) Sign3 = 1;
                                else if(I_two == 0) Sign3 = 0;
                                else Sign3 = -1;

                                if((-J_two) > 0) Sign4 = 1;
                                else if((-J_two) == 0) Sign4 = 0;
                                else Sign4 = -1;
                            }
                            else if(dataChild->G_pathmode ==2)
                            {
                                if((-I_two) > 0) Sign3 = 1;
                                else if((-I_two) == 0) Sign3 = 0;
                                else Sign3 = -1;

                                if(J_two > 0) Sign4 = 1;
                                else if(J_two == 0) Sign4 = 0;
                                else Sign4 = -1;
                            }
                            
                            if(r0y <0) r0x_abs = -r0y;
                            else r0x_abs = r0y;

                        if(r0x <0) r0z_abs = -r0x;
                        else r0z_abs = r0x;

                        if(r1y <0) r1y_abs = -r1y;
                        else r1y_abs = r1y;

                        if(r1x <0) r1x_abs = -r1x;
                        else r1x_abs = r1x;

                        S3y = S1y + Sign1*r0z_abs;
                        S3x = S1x + Sign2*r0x_abs;  

                        S4y = S2y + Sign3*r1x_abs;
                        S4x = S2x + Sign4*r1y_abs; 
                        
                        asChild[*num]= *csChild;     //��ֵ,����,��һ��ֱ��
                        if(csChild->G_plane == 18)
                        {
                            asChild[*num].Z = S3x - (*Start_X);
                            asChild[*num].X = S3y - (*Start_Y);
                        }
                        else if(csChild->G_plane == 17)
                        {
                            asChild[*num].X = S3x - (*Start_X);
                            asChild[*num].Y= S3y - (*Start_Y);
                        }
                        else if(csChild->G_plane == 19)
                        {
                            asChild[*num].Y= S3x - (*Start_X);
                            asChild[*num].Z = S3y - (*Start_Y);
                        }
                        *num=*num+1;

                        asChild[*num] = *csChild;     //��ֵ,����,�����ֱ��S3S4����;
                        if(csChild->G_plane == 18)
                        {
                            asChild[*num].Z = S4x - S3x;
                            asChild[*num].X = S4y - S3y;
                            asChild[*num].row_id=*num+1;
                        }
                        else if(csChild->G_plane == 17)
                        {
                            asChild[*num].X = S4x - S3x;
                            asChild[*num].Y= S4y - S3y;
                            asChild[*num].row_id=*num+1;
                        }
                        else if(csChild->G_plane == 19)
                        {
                            asChild[*num].Y= S4x - S3x;
                            asChild[*num].Z = S4y - S3y;
                            asChild[*num].row_id=*num+1;
                        }
                       *num=*num+1;
                        
                        asChild[*num]= *csChild;     //�������S4S2����;
                        if(csChild->G_plane == 18)
                        {
                            asChild[*num].Z = S2x - S4x;
                            asChild[*num].X = S2y - S4y;
                            asChild[*num].row_id=*num+1;
                        }
                        else if(csChild->G_plane == 17)
                        {
                            asChild[*num].X = S2x - S4x;
                            asChild[*num].Y= S2y - S4y;
                            asChild[*num].row_id=*num+1;
                        }
                        else if(csChild->G_plane == 19)
                        {
                            asChild[*num].Y= S2x - S4x;
                            asChild[*num].Z = S2y - S4y;
                            asChild[*num].row_id=*num+1;
                        }
                        *num=*num+1;

                        *Start_X = S2x - X0; 
                        *Start_Y = S2y - Y0;//ת�ӵ������ڶ���G���������������,Ϊ��һ�ε�����׼��;
                    }//����         
                        if(dataChild->G_compensate == 40)       //ȡ������;
                        {
                            asChild[*num]= *dataChild;     //��ֵ,����
                            if(csChild->G_plane == 18)
                            {
                                asChild[*num].Z = X1 - (*Start_X);
                                asChild[*num].X = Y1 - (*Start_Y);
                            }
                            else if(csChild->G_plane == 17)
                            {
                                asChild[*num].X = X1 - (*Start_X);
                                asChild[*num].Y= Y1 - (*Start_Y);
                            }
                            else if(csChild->G_plane == 19)
                            {
                                asChild[*num].Y= X1 - (*Start_X);
                                asChild[*num].Z = Y1 - (*Start_Y);
                            }
                      //����ȡ��
                             *num=*num+1;
                            *sign = 0;//��������
                            *c = 0;//Ϊ��һ�ε�������ʼ��.
                            *Start_X = 0.;   
                            *Start_Y = 0.;
                        }
            }//ֱ�߽�Բ��
else if((dataChild->G_pathmode==2 || dataChild->G_pathmode==3)&& (csChild->G_pathmode==2 || csChild->G_pathmode==3))//Բ����Բ��
            {
                if(csChild->G_plane == 18)
                {
                    X0 = csChild->Z;
                    Y0 = csChild->X;
                    I_one=csChild->K;
                    J_one=csChild->I;

                    X1 = dataChild->Z;
                    Y1 = dataChild->X;
                    I_two=dataChild->K;
                    J_two=dataChild->I;
                }
                else if(csChild->G_plane == 17)
                {
                    X0 = csChild->X;
                    Y0 = csChild->Y;
                    I_one=csChild->I;
                    J_one=csChild->J;

                    X1 = dataChild->X;
                    Y1 = dataChild->Y;
                    I_two=dataChild->I;
                    J_two=dataChild->J;
                }
                else if(csChild->G_plane == 19)
                {
                    X0 = csChild->Y;
                    Y0 = csChild->Z;
                    I_one=csChild->J;
                    J_one=csChild->K;

                    X1 = dataChild->Y;
                    Y1 = dataChild->Z;
                    I_two=dataChild->J;
                    J_two=dataChild->K;
                }
                if(csChild->G_coormode == 91)        //G90�������Ҫ����......................;
                {
                    d0 = sqrt((Y0-J_one)*(Y0-J_one)+(X0-I_one)*(X0-I_one)); 
                    d1 = sqrt((Y1-J_two)*(Y1-J_two)+(X1-I_two)*(X1-I_two)); 
                    
    if((csChild->G_compensate == 41 && csChild->G_pathmode == 2)||(csChild->G_compensate == 42 && csChild->G_pathmode == 3))
                    {
                        r0y = R*(Y0-J_one)/d0;
                        r0x = R*(X0-I_one)/d0;
                    }
    else if((csChild->G_compensate == 42 && csChild->G_pathmode == 2)||(csChild->G_compensate == 41 && csChild->G_pathmode == 3))
                    {
                        r0y = (-R)*(Y0-J_one)/d0;
                        r0x = (-R)*(X0-I_one)/d0;
                    }

    if((dataChild->G_compensate == 41 && dataChild->G_pathmode == 2)||(dataChild->G_compensate == 42 && dataChild->G_pathmode == 3))
                    {
                        r1y = R*(-J_two)/d1;
                        r1x = R*(-I_two)/d1;
                    }
    else if((dataChild->G_compensate == 42 && dataChild->G_pathmode == 2)||(dataChild->G_compensate == 41 && dataChild->G_pathmode == 3))
                    {
                        r1y = R*J_two/d1;
                        r1x = R*I_two/d1;
                    }
                    //ȷ��r0y,r0x,r1y,r1x����;

                    if(csChild->G_pathmode == 2)
                    {
                        KBy0= -(X0-I_one);
                        KBx0=Y0-J_one;
                    }
                    else if(csChild->G_pathmode == 3)
                    {
                        KBy0=X0-I_one;
                        KBx0= -(Y0-J_one);
                    }// ȷ��KB����;
                    if(dataChild->G_pathmode == 2)
                    {
                        KBy1= I_two;
                        KBx1=-J_two;
                    }
                    else if(dataChild->G_pathmode == 3)
                    {
                        KBy1=-I_two;
                        KBx1= J_two;
                    }// ȷ��KB����,Բ����㴦������;
                }//G91
            
            //�жϽ�������:����,����,�쳤
                    if( ((r0y*r1y + r0x*r1x) >= 0) && (((r1y-r0y)*KBy0 + (r1x-r0x)*KBx0) >= 0) )//�쳤��    
                    {
                        if(KBx0 == 0 && KBx1 != 0)
                        {
                        S1x= X0+r0x;    S1y=Y0+r0y; 
                        S2x= X0+r1x;    S2y=Y0+r1y;K2=KBy1/KBx1;
                        
                        Sx = S1x;
                        Sy = K2*(Sx - S2x) + S2y;//ֱ��б�����޴�ʱ;                    
                        }
                        else if(KBx0 != 0 && KBx1 == 0)
                        {
                        S1x= X0+r0x;    S1y=Y0+r0y;K1=KBy0/KBx0; 
                        S2x= X0+r1x;    S2y=Y0+r1y;
                        
                        Sx = S2x;
                        Sy = K1*(Sx - S1x) + S1y;//ֱ��б�����޴�ʱ;                    
                        }
                        else if((KBy0*KBx1) == (KBy1*KBx0))
                        {
                        Sx = X0+r0x;
                        Sy = Y0+r0y;            
                        }//ֱ��б�����ʱ;
                        else
                        {
                        S1x= X0+r0x;    S1y=Y0+r0y;K1=KBy0/KBx0; 
                        S2x= X0+r1x;    S2y=Y0+r1y;K2=KBy1/KBx1;
                        
                        Sx = (S2y - S1y - K2*S2x + K1*S1x)/(K1 - K2);
                        Sy = K1*(Sx - S1x) + S1y;
                        }//�쳤�ߵĽ�������;                    
                    

                        asChild[*num]= *csChild;     //��ֵ,����
                        if(csChild->G_plane == 18)
                        {
                            asChild[*num].Z = (X0+r0x)-(*Start_X);
                            asChild[*num].X = (Y0+r0y)-(*Start_Y);
                            asChild[*num].K = I_one-(*Start_X);
                            asChild[*num].I = J_one-(*Start_Y);
                        }
                        else if(csChild->G_plane == 17)
                        {
                            asChild[*num].X = (X0+r0x)-(*Start_X);
                            asChild[*num].Y= (Y0+r0y)-(*Start_Y);
                            asChild[*num].I = I_one-(*Start_X);
                            asChild[*num].J = J_one-(*Start_Y);
                        }
                        else if(csChild->G_plane == 19)
                        {
                            asChild[*num].Y= (X0+r0x)-(*Start_X);
                            asChild[*num].Z = (Y0+r0y)-(*Start_Y);
                            asChild[*num].J = I_one-(*Start_X);
                            asChild[*num].K = J_one-(*Start_Y);
                        }
                         *num=*num+1;//��һ��Բ��;

                        //��ֵ,����,��һ��Բ�����쳤ֱ��;
                        asChild[*num].G_pathmode=1;
                        asChild[*num].G_coormode=91;
                        if(csChild->G_plane == 18)
                        {
                            asChild[*num].Z = Sx-(X0+r0x);
                            asChild[*num].X = Sy-(Y0+r0y);
                        }
                        else if(csChild->G_plane == 17)
                        {
                            asChild[*num].X = Sx-(X0+r0x);
                            asChild[*num].Y= Sy-(Y0+r0y);
                        }
                        else if(csChild->G_plane == 19)
                        {
                            asChild[*num].Y= Sx-(X0+r0x);
                            asChild[*num].Z = Sy-(Y0+r0y);
                        }
                         *num=*num+1;//��һ��Բ�����쳤ֱ�߲���;

                        asChild[*num].G_pathmode=1;
                        asChild[*num].G_coormode=91;
                        if(csChild->G_plane == 18)
                        {
                            asChild[*num].Z = (X0+r1x)-Sx;
                            asChild[*num].X = (Y0+r1y)-Sy;
                        }
                        else if(csChild->G_plane == 17)
                        {
                            asChild[*num].X = (X0+r1x)-Sx;
                            asChild[*num].Y= (Y0+r1y)-Sy;
                        }
                        else if(csChild->G_plane == 19)
                        {
                            asChild[*num].Y= (X0+r1x)-Sx;
                            asChild[*num].Z = (Y0+r1y)-Sy;
                        }
                        *num=*num+1;//�ڶ���Բ�����ߵ��쳤����;
                        *Start_X = r1x;  
                        *Start_Y = r1y;//ת�ӵ������ڶ���G���������������,Ϊ��һ�ε�����׼��;
                        
                    }//�쳤��;

                    else if(( ((r1y-r0y)*KBy0 + (r1x-r0x)*KBx0)< 0) ||((((r1y-r0y)*KBy0 + (r1x-r0x)*KBx0)== 0)&&csChild->G_compensate == 41 && csChild->G_pathmode == 2)||((((r1y-r0y)*KBy0 + (r1x-r0x)*KBx0)== 0)&&csChild->G_compensate == 42 && csChild->G_pathmode == 3)) //������
                    {
               //�����Ե���
                        S1x=X0; S1y=Y0;
                    
                        Ox=I_one;   Oy=J_one;//Բ������;
                        r_arc=sqrt( (X0+r0x-I_one)*(X0+r0x-I_one) + (Y0+r0y-J_one)*(Y0+r0y-J_one) );//Բ���뾶
                        
                        if(X0+I_two-Ox==0)
                        {
                        S2x = I_one;
                        S2y = S1y;
                        }
                        else if(Y0+J_two-Oy==0)
                        {
                        S2x = S1x; 
                        S2y = J_one;
                        }
                        else
                        {
                        Kline=(Y0+J_two - Oy)/(X0+I_two - Ox);//����Բ�ĵ�ֱ��б��;
                        S2x=(Kline/(Kline*Kline+1))*(Y0-Oy+X0/Kline+Kline*Ox);//����Բ�ĵ�ֱ��,
                                        //�Ե�һ��Բ��Բ������Ϊ������,����
                                        //������Բ�������ߵ� ����;
                        S2y=Kline*(S2x-Ox)+Oy;
                        }
                        if(S1x == S2x)
                        {
                        Sx1=S1x ;   Sy1=Oy + sqrt(r_arc*r_arc-(S1x-Ox)*(S1x-Ox));
                        Sx2=S1x ;   Sy2=Oy - sqrt(r_arc*r_arc-(S1x-Ox)*(S1x-Ox));
                        }
                        else if(S1y == S2y)
                        {
                        Sx1=Ox + sqrt(r_arc*r_arc-(S1y-Oy)*(S1y-Oy));   Sy1=S1y;
                        Sx2=Ox - sqrt(r_arc*r_arc-(S1y-Oy)*(S1y-Oy));   Sy2=S1y;
                        }
                        else
                        {
                        K_arc=(S2x-S1x)/(S2y-S1y);
                        a_arc=1+K_arc*K_arc;
                        b_arc=2*K_arc*(S2x-Ox-K_arc*S2y-Oy/K_arc);
                        c_arc=(S2x-Ox-K_arc*S2y)*(S2y-Ox-K_arc*S2y)-r_arc*r_arc+Oy*Oy;
                        
                    Sy1=( -b_arc+sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);   Sx1=S2x + K_arc*(Sy1-S2y);
                    Sy2=( -b_arc-sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);   Sx2=S2x + K_arc*(Sy2-S2y);
                        }
                        
    if( ((r1x*KBx0+r1y*KBy0)>=0 && ((Sx2-Sx1)*KBx0+(Sy2-Sy1)*KBy0)<=0) || ((r1x*KBx0+r1y*KBy0)<0 && ((Sx2-Sx1)*KBx0+(Sy2-Sy1)*KBy0)>0) )
                            {
                            Sy=Sy2;
                            Sx=Sx2;
                            }
    else if(((r1x*KBx0+r1y*KBy0)>=0 && ((Sx2-Sx1)*KBx0+(Sy2-Sy1)*KBy0)>0)||((r1x*KBx0+r1y*KBy0)<0 && ((Sx2-Sx1)*KBx0+(Sy2-Sy1)*KBy0)<=0) )
                            {
                            Sy=Sy1;
                            Sx=Sx1;
                            }
                            asChild[*num]= *csChild;     //��ֵ,����
                            if(csChild->G_plane == 18)
                            {
                                asChild[*num].Z = Sx - (*Start_X);
                                asChild[*num].X = Sy - (*Start_Y);
                                asChild[*num].K = I_one-(*Start_X);
                                asChild[*num].I = J_one-(*Start_Y);
                            }
                            else if(csChild->G_plane == 17)
                            {
                                asChild[*num].X = Sx - (*Start_X);
                                asChild[*num].Y= Sy - (*Start_Y);
                                asChild[*num].I = I_one-(*Start_X);
                                                                asChild[*num].J = J_one-(*Start_Y);
                            }
                            else if(csChild->G_plane == 19)
                            {
                                asChild[*num].Y= Sx - (*Start_X);
                                asChild[*num].Z = Sy - (*Start_Y);
                                asChild[*num].J = I_one-(*Start_X);
                                                                asChild[*num].K = J_one-(*Start_Y);
                            }
                            *num=*num+1;;

                            *Start_X = Sx - X0;  
                            *Start_Y = Sy - Y0;//ת�ӵ������ڶ���G���������������,Ϊ��һ�ε�����׼��;
                            
                        }//������
                
                    else if(( (((r1x-r0x)*KBx0 +(r1y-r0y)*KBy0) > 0) && ((r0y*r1y + r0x*r1x) < 0) )||((((r1y-r0y)*KBy0 + (r1x-r0x)*KBx0)== 0)&&csChild->G_compensate == 42 && csChild->G_pathmode == 2)||((((r1y-r0y)*KBy0 + (r1x-r0x)*KBx0)== 0)&&csChild->G_compensate == 41 && csChild->G_pathmode == 3))//������
                        {
						  //�����Ե���
                            S1x= X0 + r0x;  S1y=Y0 + r0y; //��һ���߶��յ㴦�İ뾶ʸ����������;
                            S2x= X0 + r1x;  S2y=Y0 + r1y;//�ڶ�������㴦�İ뾶ʸ����������;
                                        //S1,S2���������;

                            if(csChild->G_pathmode ==3)
                            {
                                if((X0-I_one) > 0) Sign1 = 1;
                                else if((X0-I_one) == 0) Sign1 = 0;
                                else Sign1 = -1;

                                if((J_one-Y0) > 0) Sign2 = 1;
                                else if((J_one-Y0) == 0) Sign2 = 0;
                                else Sign2 = -1;
                            }
                            else if(csChild->G_pathmode ==2)
                            {
                                if((I_one-X0) > 0) Sign1 = 1;
                                else if((I_one-X0) == 0) Sign1 = 0;
                                else Sign1 = -1;

                                if((Y0-J_one) > 0) Sign2 = 1;
                                else if((Y0-J_one) == 0) Sign2 = 0;
                                else Sign2 = -1;
                            }
                            if(dataChild->G_pathmode ==3)
                            {
                                if(I_two > 0) Sign3 = 1;
                                else if(I_two == 0) Sign3 = 0;
                                else Sign3 = -1;

                                if((-J_two) > 0) Sign4 = 1;
                                else if((-J_two) == 0) Sign4 = 0;
                                else Sign4 = -1;
                            }
                            else if(dataChild->G_pathmode ==2)
                            {
                                if((-I_two) > 0) Sign3 = 1;
                                else if((-I_two) == 0) Sign3 = 0;
                                else Sign3 = -1;

                                if(J_two > 0) Sign4 = 1;
                                else if(J_two == 0) Sign4 = 0;
                                else Sign4 = -1;
                            }
                            if(r0y <0) r0x_abs = -r0y;
                            else r0x_abs = r0y;

                            if(r0x <0) r0z_abs = -r0x;
                            else r0z_abs = r0x;

                            if(r1y <0) r1y_abs = -r1y;
                            else r1y_abs = r1y;

                            if(r1x <0) r1x_abs = -r1x;
                            else r1x_abs = r1x;
                            
    
                            S3y = S1y + Sign1*r0z_abs;
                            S3x = S1x + Sign2*r0x_abs;  

                            S4y = S2y + Sign3*r1x_abs;
                            S4x = S2x + Sign4*r1y_abs;  

                            asChild[*num]= *csChild;     //��ֵ,����,Բ������
                            if(csChild->G_plane == 18)
                            {
                                asChild[*num].Z = S1x - (*Start_X);
                                asChild[*num].X = S1y - (*Start_Y);
                                asChild[*num].K = I_one-(*Start_X);
                                asChild[*num].I = J_one-(*Start_Y);
                            }
                            else if(csChild->G_plane == 17)
                            {
                                asChild[*num].X = S1x - (*Start_X);
                                asChild[*num].Y= S1y - (*Start_Y);
                                asChild[*num].I = I_one-(*Start_X);
                                                                asChild[*num].J = J_one-(*Start_Y);
                            }
                            else if(csChild->G_plane == 19)
                            {
                                asChild[*num].Y= S1x - (*Start_X);
                                asChild[*num].Z = S1y - (*Start_Y);
                                asChild[*num].J = I_one-(*Start_X);
                                asChild[*num].K = J_one-(*Start_Y);
                            }
                             *num=*num+1;

                            asChild[*num].G_pathmode=1;
                            asChild[*num].G_coormode=91;
                            if(csChild->G_plane == 18)
                            {
                                asChild[*num].Z = S3x - S1x;
                                asChild[*num].X = S3y - S1y;
                               asChild[*num].row_id=*num+1;
                            }
                            else if(csChild->G_plane == 17)
                            {
                                asChild[*num].X = S3x - S1x;
                                asChild[*num].Y= S3y - S1y;
                                asChild[*num].row_id=*num+1;
                            }
                            else if(csChild->G_plane == 19)
                            {
                                asChild[*num].Y= S3x - S1x;
                                asChild[*num].Z = S3y - S1y;
                                asChild[*num].row_id=*num+1;
                            }
                            *num=*num+1;
                            
                            asChild[*num].G_pathmode=1;
                            asChild[*num].G_coormode=91;
                            if(csChild->G_plane == 18)
                            {
                                asChild[*num].Z = S4x - S3x;
                                asChild[*num].X = S4y - S3y;
                                asChild[*num].row_id=*num+1;
                            }
                            else if(csChild->G_plane == 17)
                            {
                                asChild[*num].X = S4x - S3x;
                                asChild[*num].Y= S4y - S3y;
                                asChild[*num].row_id=*num+1;
                            }
                            else if(csChild->G_plane == 19)
                            {
                                asChild[*num].Y= S4x - S3x;
                                asChild[*num].Z = S4y - S3y;
                                asChild[*num].row_id=*num+1;
                            }
                             *num=*num+1;


                            asChild[*num].G_pathmode=1;
                            asChild[*num].G_coormode=91;
                            if(csChild->G_plane == 18)
                            {
                                asChild[*num].Z = S2x - S4x;
                                asChild[*num].X = S2y - S4y;
                                asChild[*num].row_id=*num+1;
                            }
                            else if(csChild->G_plane == 17)
                            {
                                asChild[*num].X = S2x - S4x;
                                asChild[*num].Y= S2y - S4y;
                                asChild[*num].row_id=*num+1;
                            }
                            else if(csChild->G_plane == 19)
                            {
                                asChild[*num].Y= S2x - S4x;
                                asChild[*num].Z = S2y - S4y;
                                asChild[*num].row_id=*num+1;
                            }
                            *num=*num+1;

                            *Start_X = r1x;  
                            *Start_Y = r1y;//ת�ӵ������ڶ���G���������������,Ϊ��һ�ε�����׼��;
                        }//����
            
                            if(dataChild->G_compensate == 40)       //ȡ������;
                            {
                                asChild[*num]= *dataChild;     //��ֵ,����
                                if(csChild->G_plane == 18)
                                {
                                    asChild[*num].Z = X1 - (*Start_X);
                                    asChild[*num].X = Y1 - (*Start_Y);
                                }
                                else if(csChild->G_plane == 17)
                                {
                                    asChild[*num].X = X1 - (*Start_X);
                                    asChild[*num].Y= Y1 - (*Start_Y);
                                }
                                else if(csChild->G_plane == 19)
                                {
                                    asChild[*num].Y= X1 - (*Start_X);
                                    asChild[*num].Z = Y1 - (*Start_Y);
									  
                                }
								//����ȡ�� 
                                *num=*num+1;
                                *sign = 0;//��������
                                *c = 0;//Ϊ��һ�ε�������ʼ��.
                                *Start_X = 0.;   
                                *Start_Y = 0.;
                            }//ȡ������  */
                }//Բ����Բ��*/

            }//���������� if(c != 0)
			if(dataChild->G_compensate != 40)
			{
				
				*csChild = *dataChild;
				*c = 1;
				
						
			}

}
