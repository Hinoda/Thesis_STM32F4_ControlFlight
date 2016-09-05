#include "project.h"
#include "math.h"
PID_Index Roll_PID;
PID_Index Pitch_PID;
PID_Index Yaw_PID;
PID_Index Alt_PID;
PID_Index Roll1_PID;
PID_Index Pitch1_PID;
PID_Index Yaw1_PID;
PID_Index Alt1_PID;
PID_Index Latitude;
PID_Index Longitude ;
PID_Index Speed;

PID_Index Press;
void PID_Init(void)
{
	Roll_PID.e[0]=Roll_PID.e[1]=Roll_PID.e[2]=0;
	Roll_PID.Kp=3;
	Roll_PID.Ki=0;
	Roll_PID.Kd=0;
	Pitch_PID.e[0]=Pitch_PID.e[1]=Pitch_PID.e[2]=0;
	Pitch_PID.Kp=3;
	Pitch_PID.Ki=0;
	Pitch_PID.Kd=0;
	Yaw_PID.e[0]=Yaw_PID.e[1]=Yaw_PID.e[2]=0;
	Yaw_PID.Kp=3;
	Yaw_PID.Ki=0;
	Yaw_PID.Kd=0;
	Alt_PID.e[0]=Alt_PID.e[1]=Alt_PID.e[2]=0;
	Alt_PID.Kp=5;
	Alt_PID.Ki=0;
	Alt_PID.Kd=0;
}
void Sampling_VTG(uint8_t* VTG , int lenght)
{
	int i=0;
	int comma=0;
	int dot =0 ;
	int fp=0,ep = 0;
	float vantoc=0;
	for (i=0;i<lenght;i++)
	{
			if (VTG[i]==',')
			{
				comma++;
				if(comma==7)
					fp=i;
				if(comma==8)
				{
					ep=i;
					break;
				}
			}
	 }
	for(i=fp+1;i<ep;++i)
	{
		if(VTG[i]=='.')
		{
			dot=i;
			break;
		}
	}
	for (i=fp+1;i<dot;++i)
		vantoc+=(VTG[i]-0x30)*pow(10,(dot-i-1));
	for (i=dot+1;i<ep;++i)
		vantoc+=(VTG[i]-0x30)*pow(10,(dot-i));
	Speed.Current=vantoc;
}
void Sampling_GGA(uint8_t* GGA , int lenght)
{
	int i=0;
	int comma=0;
	int dot =0 ;
	int fp=0,fp_latitude=0,fp_longitude=0;
	int ep = 0,ep_latitude=0,ep_longitude=0;
	float docao =0,vido=0,kinhdo=0;
	// tim [ fp -12.5 ep ]
	for (i=0;i<lenght;i++)
	{
			if (*(GGA+i)==',')
			{
				comma++;
				if(comma==2)
					fp_latitude=i;
				if(comma==3)
					ep_latitude=i;
				if(comma==4)
					fp_longitude=i;
				if(comma==5)
					ep_longitude=i;
				if (comma==9)
					fp = i;
				if (comma==10)
				{
					ep=i;
					break;
				}
			}
	}
	if(ep_latitude!=(fp_latitude+1))
	{
		vido= (GGA[fp_latitude+1]-0x30)*10+(GGA[fp_latitude+2]-0x30)+(GGA[fp_latitude+3]-0x30)*0.1+(GGA[fp_latitude+4]-0x30)*0.01+(GGA[fp_latitude+6]-0x30)*0.001;
		vido+=(GGA[fp_latitude+7]-0x30)*0.0001+(GGA[fp_latitude+8]-0x30)*0.00001+(GGA[fp_latitude+9]-0x30)*0.000001;
		// 		for(i=0;i<4;i++)
// 		{
// 			vido+=(GGA[fp_latitude+6+i]-0x30)* pow(10,-i-3);
// 		}
	}
	else vido=0;
	Latitude.Current =vido;
	if(ep_longitude!=(fp_longitude+1))
	{
		kinhdo= (GGA[fp_longitude+1]-0x30)*100+(GGA[fp_longitude+2]-0x30)*10+(GGA[fp_longitude+3]-0x30)+(GGA[fp_longitude+4]-0x30)*0.1+(GGA[fp_longitude+5]-0x30)*0.01;
		for(i=0;i<4;i++)
		{
			kinhdo+=(GGA[fp_longitude+7+i]-0x30)* pow(10,-i-3);
		}
	}
	else kinhdo=0;
	Longitude.Current =kinhdo;
	// neu co thong tin do cao thi lam 
if(state_alt==1)
{
	if (ep!=(fp+1))
	{
	// tim vi tri dau cham
	for (i=fp+1;i<ep;++i)
	{
		if (*(GGA+i)=='.')
		{
			dot=i;
			break;
		}
	}
	if (*(GGA+fp+1)=='-')
	{
		for (i=fp+2;i<dot;++i)
				docao+=(*(GGA+i)-0x30)*pow(10,(dot-i-1));
		for (i=dot+1;i<ep;++i)
				docao+=(*(GGA+i)-0x30)*pow(10,(dot-i));
		docao= -docao;
	}
	else 
	{
		for (i=fp+1;i<dot;++i)
				docao+=(*(GGA+i)-0x30)*pow(10,(dot-i-1));
		for (i=dot+1;i<ep;++i)
				docao+=(*(GGA+i)-0x30)*pow(10,(dot-i));
	}
 }
 else docao=0;
 if (docao >8 )
 {
	Alt_PID.Current = docao;
	Alt_PID.enable =1 ;
 }
 }
}
	


void Sampling_RPY(uint8_t* IMU , int lenght)
{
	int i = 0 ; 
	float roll=0,pitch=0,yaw=0,press=0;
	// cap nhat roll
	for (i=4;i<8;i++)
		roll+= (*(IMU+i)-0x30)*pow(10,(7-i));
	roll=roll*0.1;
	if (*(IMU+3)=='-')
		roll=-roll;
	Roll_PID.Current=roll;
	Roll_PID.enable=1;
	//cap nhat pitch
	for (i=10;i<14;++i)
	pitch+= (*(IMU+i)-0x30)*pow(10,(13-i));
	pitch=pitch*0.1;
	if (*(IMU+9)=='-')
		pitch=-pitch;
	Pitch_PID.Current=pitch;
	Pitch_PID.enable =1 ;
	//cap nhat Yaw
	for (i=16;i<20;++i)
	yaw+= (*(IMU+i)-0x30)*pow(10,(19-i));
	yaw=yaw*0.1;
	if (*(IMU+15)=='-')
		yaw=-yaw;
	Yaw_PID.Current=yaw;
	Yaw_PID.enable =1 ;	
// 	if(state_press==1)
// 	{	
	for (i=76;i<80;++i)
	press+= (*(IMU+i)-0x30)*pow(10,(79-i));
	press+=100000;
	press=1- pow((press/101325),(1/5.25578));
	press=(press*pow(10,5))/2.25577;
	Press.Current=press;
	//chuyen doi press sang do cao
	if(state_press==1)
	{
	Alt_PID.Current = Press.Current;
	Alt_PID.enable =1 ;
	}
}


/*******************************************************************************
Function name: Call_Roll_PID
Decription: 
Input: v_set
Output: None

//Formula
a0=Kp+(Ki*T)/2+Kd/T;
a1=-Kp+(Ki*T)/2-(2*Kd)/T;
a2=Kd/T;
uk = u(k - 1)+ a0 * e(k) + a1 * e(k - 1) + a2 * e(k - 2)
*******************************************************************************/
void Call_Roll_PID(float Roll_set)
{
	static float Roll_set_previous=200; 
	if(Roll_PID.enable)
		{
			Roll_PID.enable=0;
			if ((Roll_set_previous != Roll_set)||(Update_heso_Roll==1))
			{
				Roll_set_previous=Roll_set;
				Update_heso_Roll=0;
				// Tinh cac he so dat
				Roll_PID.a0=(Roll_PID.Kp +Roll_PID.Ki*0.02*0.5+Roll_PID.Kd*50);	//a0=(Roll_PID.Kp +Roll_PID.Ki*0.01*0.5+Roll_PID.Kd/0.01)
				Roll_PID.a1=(-Roll_PID.Kp+Roll_PID.Ki*0.02*0.5-2*Roll_PID.Kd*50);//a1=(-Roll_PID.Kp+Roll_PID.Ki*0.01*0.5-2*Roll_PID.Kd/0.01);
				Roll_PID.a2=Roll_PID.Kd*50;//a2=Roll_PID.Kd/0.01

				// Reset lai cac gia tri error
				Roll_PID.e[0]=Roll_PID.e[1]=Roll_PID.e[2]=0;
				Roll_PID.Pid_Result=0;
			}
			Roll_PID.e[2]=Roll_set-Roll_PID.Current;
			if (trituyetdoi(Roll_PID.e[2])>180)
			Roll_PID.e[2]=(Roll_PID.e[2]-360);

			Roll_PID.Pid_Result_Temp=Roll_PID.Pid_Result;
			Roll_PID.Pid_Result=Roll_PID.Pid_Result_Temp+Roll_PID.a0*Roll_PID.e[2]+Roll_PID.a1*Roll_PID.e[1]+Roll_PID.a2*Roll_PID.e[0];
			//Roll_PID.Pid_Result = Roll_PID.Kp*Roll_PID.e[2];
			// gioi han		
			if(Roll_PID.Pid_Result>Max_Xung) Roll_PID.Pid_Result=Max_Xung;
			if(Roll_PID.Pid_Result<-Max_Xung) Roll_PID.Pid_Result=-Max_Xung;
					
			Roll_PID.e[0]=Roll_PID.e[1];
			Roll_PID.e[1]=Roll_PID.e[2];

			// Dat gia tri vao PWM	
			 Gent_Pwm_Roll(Roll_PID.Pid_Result);
			}
	else return;
	}

	/*******************************************************************************
Function name: Call_Pitch_PID
Decription: 
Input: 
Output: None
*******************************************************************************/
void Call_Pitch_PID(float Pitch_set)
{
	static float Pitch_set_previous=200; 
	if(Pitch_PID.enable)
	{
		Pitch_PID.enable=0;
		if ((Pitch_set_previous != Pitch_set)||(Update_heso_Pitch==1))
			{
					Pitch_set_previous=Pitch_set;
					Update_heso_Pitch=0;
					// Tinh cac he so dat
					Pitch_PID.a0=(Pitch_PID.Kp +Pitch_PID.Ki*0.02*0.5+Pitch_PID.Kd*50);	
					Pitch_PID.a1=(-Pitch_PID.Kp+Pitch_PID.Ki*0.02*0.5-2*Pitch_PID.Kd*50);
					Pitch_PID.a2=Pitch_PID.Kd*50;
					// Reset lai cac gia tri error
					Pitch_PID.e[0]=Pitch_PID.e[1]=Pitch_PID.e[2]=0;
					Pitch_PID.Pid_Result=0;
			}
		Pitch_PID.e[2]=Pitch_set-Pitch_PID.Current;
		if (trituyetdoi(Pitch_PID.e[2])>180)
		Pitch_PID.e[2]=(Pitch_PID.e[2]-360);
		
		Pitch_PID.Pid_Result_Temp=Pitch_PID.Pid_Result;
		Pitch_PID.Pid_Result=Pitch_PID.Pid_Result_Temp+Pitch_PID.a0*Pitch_PID.e[2]+Pitch_PID.a1*Pitch_PID.e[1]	+Pitch_PID.a2*Pitch_PID.e[0];
				
		if(Pitch_PID.Pid_Result>Max_Xung) Pitch_PID.Pid_Result=Max_Xung;
		if(Pitch_PID.Pid_Result<-Max_Xung) Pitch_PID.Pid_Result=-Max_Xung;
				
		Pitch_PID.e[0]=Pitch_PID.e[1];
		Pitch_PID.e[1]=Pitch_PID.e[2];
		// Dat gia tri vao PWM	
		Gent_Pwm_Pitch(-Pitch_PID.Pid_Result);
	}
	else return;
}
/*******************************************************************************
Function name: Call_Alt_PID
Decription: Ha
Input: p_set
Output: None
*******************************************************************************/
void Call_Alt_PID(float Alt_set)
{
		static float Alt_set_previous=-1; 
		if(Alt_PID.enable)
		{
			Alt_PID.enable=0;
			if(state_alt==1)
			{
						if ((Alt_set_previous != Alt_set)||(Update_heso_Alt==1))
						{
							Alt_set_previous=Alt_set;
							Update_heso_Alt=0;		
							// Tinh cac he so dat
							Alt_PID.a0=(Alt_PID.Kp +Alt_PID.Ki*0.1*0.5+Alt_PID.Kd*10);	
							Alt_PID.a1=(-Alt_PID.Kp+Alt_PID.Ki*0.1*0.5-2*Alt_PID.Kd*10);
							Alt_PID.a2=Alt_PID.Kd*10;
			
							// Reset lai cac gia tri error
							Alt_PID.e[0]=Alt_PID.e[1]=Alt_PID.e[2]=0;
							Alt_PID.Pid_Result=0;
						}
						Alt_PID.e[2]=Alt_set-Alt_PID.Current;
						if (trituyetdoi(Pitch_PID.e[2])>180)
						Pitch_PID.e[2]=(Pitch_PID.e[2]-360);
					
						Alt_PID.Pid_Result_Temp=Alt_PID.Pid_Result;
						Alt_PID.Pid_Result=Alt_PID.Pid_Result_Temp+Alt_PID.a0*Alt_PID.e[2]+Alt_PID.a1*Alt_PID.e[1]	+Alt_PID.a2*Alt_PID.e[0];
								
						if(Alt_PID.Pid_Result>Max_Xung) Alt_PID.Pid_Result=Max_Xung;
						if(Alt_PID.Pid_Result<-Max_Xung) Alt_PID.Pid_Result=-Max_Xung;
								
						Alt_PID.e[0]=Alt_PID.e[1];
						Alt_PID.e[1]=Alt_PID.e[2];
					
						// Dat gia tri vao PWM	
						Gent_Pwm_Alt(Alt_PID.Pid_Result);
						}
			if(state_press==1)
			{
						if ((Alt_set_previous != Alt_set)||(Update_heso_Press==1))
						{
								Alt_set_previous=Alt_set;
								Update_heso_Press=0;		
								// Tinh cac he so dat
								Alt_PID.a0=(Press.Kp +Press.Ki*0.1*0.5+Press.Kd*10);	
								Alt_PID.a1=(-Press.Kp+Press.Ki*0.1*0.5-2*Press.Kd*10);
								Alt_PID.a2=Press.Kd*10;
					
								// Reset lai cac gia tri error
								Alt_PID.e[0]=Alt_PID.e[1]=Alt_PID.e[2]=0;
								Alt_PID.Pid_Result=0;
						}
						Alt_PID.e[2]=Alt_set-Press.Current;
						if (trituyetdoi(Pitch_PID.e[2])>180)
						Pitch_PID.e[2]=(Pitch_PID.e[2]-360);
					
						Alt_PID.Pid_Result_Temp=Alt_PID.Pid_Result;
						Alt_PID.Pid_Result=Alt_PID.Pid_Result_Temp+Alt_PID.a0*Alt_PID.e[2]+Alt_PID.a1*Alt_PID.e[1]	+Alt_PID.a2*Alt_PID.e[0];
								
						if(Alt_PID.Pid_Result>Max_Xung) Alt_PID.Pid_Result=Max_Xung;
						if(Alt_PID.Pid_Result<-Max_Xung) Alt_PID.Pid_Result=-Max_Xung;
								
						Alt_PID.e[0]=Alt_PID.e[1];
						Alt_PID.e[1]=Alt_PID.e[2];
					
						// Dat gia tri vao PWM	
						Gent_Pwm_Alt(10+Alt_PID.Pid_Result);
						}
		}
		else return;
}

/*******************************************************************************
Function name: Call_Yaw_PID
Decription: Ham PID vi tri dong co theo gia tri xung encoder
Input: p_set
Output: None
*******************************************************************************/
void Call_Yaw_PID(float Yaw_set)
{
	static float Yaw_set_previous=200; 
	if(Yaw_PID.enable)
	{
		Yaw_PID.enable=0;
		if ((Yaw_set_previous != Yaw_set)||(Update_heso_Yaw==1))
		{
				Yaw_set_previous=Yaw_set;
				Update_heso_Yaw=0;		
				// Tinh cac he so dat
				Yaw_PID.a0=(Yaw_PID.Kp +Yaw_PID.Ki*0.02*0.5+Yaw_PID.Kd*50);	
				Yaw_PID.a1=(-Yaw_PID.Kp+Yaw_PID.Ki*0.02*0.5-2*Yaw_PID.Kd*50);
				Yaw_PID.a2=Yaw_PID.Kd*50;


				// Reset lai cac gia tri error
				Yaw_PID.e[0]=Yaw_PID.e[1]=Yaw_PID.e[2]=0;
				Yaw_PID.Pid_Result=0;
		}
		Yaw_PID.e[2]=Yaw_set-Yaw_PID.Current;
		// XU LY CODE QUA MUC 180
		if (trituyetdoi(Yaw_PID.e[2])>=180)
		Yaw_PID.e[2]=(Yaw_PID.e[2]-360);
		// PID
		Yaw_PID.Pid_Result_Temp=Yaw_PID.Pid_Result;
		Yaw_PID.Pid_Result=Yaw_PID.Pid_Result_Temp+Yaw_PID.a0*Yaw_PID.e[2]+Yaw_PID.a1*Yaw_PID.e[1]	+Yaw_PID.a2*Yaw_PID.e[0];
				
		if(Yaw_PID.Pid_Result>Max_Xung) Yaw_PID.Pid_Result=Max_Xung;
		if(Yaw_PID.Pid_Result<-Max_Xung) Yaw_PID.Pid_Result=-Max_Xung;
				
		Yaw_PID.e[0]=Yaw_PID.e[1];
		Yaw_PID.e[1]=Yaw_PID.e[2];

		// Dat gia tri vao PWM	
		Gent_Pwm_Yaw(Yaw_PID.Pid_Result);
	}
	else return;
}
/************************************************************************************************/
//TIM4->CCR3 = Pwm;//channel 3
//TIM4->CCR2 = Pwm;//channel 2
void Gent_Pwm_Roll(float Roll)
{
	int Pwm ;
 	Pwm =(int)((1+(Roll+45)/90)*40000/8);
	TIM4->CCR1 = Pwm;
}
void Gent_Pwm_Pitch(float Pitch)
{
	int Pwm ;
 	Pwm =(int)((1+(Pitch+45)/90)*40000/8);
	TIM4->CCR2 = Pwm;
}
void Gent_Pwm_Yaw(float Yaw)
{
	int Pwm ;
 	Pwm =(int)((1+(Yaw+45)/90)*40000/8);
	TIM4->CCR4 = Pwm;
}
void Gent_Pwm_Alt(float Alt)
{
	int Pwm ;
 	Pwm =(int)((1+(Alt+45)/90)*40000/8);
	TIM4->CCR3 = Pwm;
}

float trituyetdoi(float a)
{
	if (a<0) 
		return -a ;
	else
		return a;
}
