#include "fun.h"
char show_message[20];
int state=0;
int freA15=0;
int freA7=0;
int first_enter=0;
int hour=0,min=0,sec=0;
char wrong_show[20];
char kongbai[20];
char  hour1[5],min1[5],sec1[5];
int lock_state=0;  //0表示解锁， 1 表示锁上
int DR=80,DS=1,FS=100,FR=2000;
int bstate[4];
int bstate_last[4]={1,1,1,1};
int CF=0,CD=0,DF=0,XF=0;
int wrong_state=0; //判断异常状态
int selece_state_first=0;
int judge_state_first=0;
int DR1=80,DS1=1,FS1=100,FR1=2000;
int select_state=0;
double n;
double Va,Vb;
void light_show(int num,int mode){
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,1);
	if (mode ==1){  //点亮
			HAL_GPIO_WritePin(GPIOC,0x0080<<num,0);
	}else if (mode==0){
			HAL_GPIO_WritePin(GPIOC,0x0080<<num,1);
	}
HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,0);
}
double get_value2(){   //R38
HAL_ADC_Start(&hadc1);
double value = HAL_ADC_GetValue(&hadc1);
	
return value*3.3/4096.0;

}

double get_value1(){//R37
	
HAL_ADC_Start(&hadc2);
double value = HAL_ADC_GetValue(&hadc2);
	
	return value*3.3/4096.0;

}


int change_judge(){
  if (DR<10||FR<1000){
	return 1;
	}
	else if (DS>DR||FS>FR){
	return 1;
	}else {
	return 0;
	}
}
void change_ccr(){
Va=3.3*DS/(DR-10);
TIM3->CCR2= 10+(get_value1()/Va)*DS;
}
void change_fre(){
Vb=3.3*FS/(FR-1000);
freA7=1000+(get_value2()/Vb)*FS;
	
TIM3->PSC=(800000/freA7)-1;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){

if (htim==&htim4){
sec++;

}

}
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim){
if (htim==&htim8){
freA15 = 1000000/TIM8->CNT;
TIM8->CNT=0;
}

}

void wrong_judge(){
if ((freA15-freA7>1000||freA7-freA15>1000)&&wrong_state==0){
   CF=80000000/((TIM3->PSC+1)*(TIM3->ARR+1));
	 CD=TIM3->CCR2;
	 DF=freA15;
	if (freA15-freA7<0){
	XF=-(freA15-freA7);
	}else if (freA15-freA7>0){
	XF=(freA15-freA7);
	}
	 

	 light_show(3,1);
	 first_enter=1;
	 sprintf(wrong_show,"   %sH%sM%sS   ",hour1,min1,sec1);
	 HAL_Delay(30);
	 wrong_state=1;
   
 }else if ((freA15-freA7<1000&&freA15-freA7>0)||(freA7-freA15<1000&&freA7-freA15>0)){
	 if (first_enter==0){
	 	sprintf(wrong_show,"                 ");
	 }

	light_show(3,0);
 wrong_state=0;
 }
}
void oled_show(){
	if (lock_state==0){
	 change_ccr();
   change_fre();
	}
	 if (sec>=60){
		min++;
	 sec=0;
	 }
	 if (min>=60){
		hour++;
	 min=0;
	 }if (hour<10){
	 sprintf(hour1,"0%d",hour);
	 }else if (hour>10){
	  sprintf(hour1,"%d",hour);
	 }
	 
	 if (min<10){
	 sprintf(min1,"0%d",min);
	 }else if (min>10){
		 
	  sprintf(min1,"%d",min);
	 }
	 
	 if (sec<10){
	 sprintf(sec1,"0%d",sec);
	 }else if (sec>10){
		
	  sprintf(sec1,"%d",sec);
	 }
	
	if (lock_state==1) {
	light_show(2,1);
	}else if (lock_state==0) {
	light_show(2,0);
	}
	if (state%3==0){
		wrong_judge();
		selece_state_first=0;
				if (judge_state_first==0){
					if (change_judge()==1){
							DS=DS1;
              DR=DR1;
              FS=FS1;
              FR=FR1;	
					
					}
					
				judge_state_first=1;
					
				}
		
		light_show(1,1);
	
		
		light_show(4,0);
		light_show(5,0);
		light_show(6,0);
		light_show(7,0);
		light_show(8,0);
		
	 sprintf(show_message,"       PWM  ");
	 LCD_DisplayStringLine(Line1,(u8 *)show_message);
		
   sprintf(show_message,"   CF=%dHz    ",80000000/((TIM3->PSC+1)*(TIM3->ARR+1)));
	 LCD_DisplayStringLine(Line3,(u8 *)show_message);

   sprintf(show_message,"   CD=%d%%    ", TIM3->CCR2);
	 LCD_DisplayStringLine(Line4,(u8 *)show_message);
	 
	 sprintf(show_message,"   DF=%dHz    ", freA15);
	 LCD_DisplayStringLine(Line5,(u8 *)show_message);
	 if (lock_state==0){
	 sprintf(show_message,"   ST=UNLOCK   ");
	 LCD_DisplayStringLine(Line6,(u8 *)show_message);
	 }else if (lock_state==1){
	 sprintf(show_message,"   ST=LOCK   ");
	 LCD_DisplayStringLine(Line6,(u8 *)show_message);
	 }

	 
	 sprintf(show_message,"   %sH%sM%sS   ",hour1,min1,sec1);
	 LCD_DisplayStringLine(Line7,(u8 *)show_message);
	  

	}else if (state%3==1){
		wrong_judge();
		light_show(1,0);
		judge_state_first=0;
		selece_state_first=0;
	sprintf(show_message,"       RECD  ");
	 LCD_DisplayStringLine(Line1,(u8 *)show_message);	
	 sprintf(show_message,"   CF=%dHz   ",CF);
	 LCD_DisplayStringLine(Line3,(u8 *)show_message);
	 sprintf(show_message,"   CD=%d%%    ",CD);
	 LCD_DisplayStringLine(Line4,(u8 *)show_message);
	 sprintf(show_message,"   DF=%dHz    ",DF);
	 LCD_DisplayStringLine(Line5,(u8 *)show_message);
	 sprintf(show_message,"   XF=%dHz    ", XF);
	 LCD_DisplayStringLine(Line6,(u8 *)show_message);
		
	 LCD_DisplayStringLine(Line7,(u8 *)wrong_show);
		
	}else if (state%3==2){
		wrong_judge();
		light_show(1,0);
	  if (selece_state_first==0){
		select_state=0;
		DS1=DS;
    DR1=DR;
    FS1=FS;
    FR1=FR;			
		judge_state_first=0;
		selece_state_first=1;
		}
	 sprintf(show_message,"       PARA  ");
	 LCD_DisplayStringLine(Line1,(u8 *)show_message);
		
   sprintf(show_message,"   DS=%d%%    ",DS);
	 LCD_DisplayStringLine(Line3,(u8 *)show_message);
	 sprintf(show_message,"   DR=%d%%    ",DR);
	 LCD_DisplayStringLine(Line4,(u8 *)show_message);
	 sprintf(show_message,"   FS=%dHz    ",FS);
	 LCD_DisplayStringLine(Line5,(u8 *)show_message);
	 sprintf(show_message,"   FR=%dHz    ", FR);
	 LCD_DisplayStringLine(Line6,(u8 *)show_message);
	sprintf(kongbai,"                         ");
	 LCD_DisplayStringLine(Line7,(u8 *)kongbai);
	}


}
void key_scan(){

bstate[0]= HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0);
bstate[1]= HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1);
bstate[2]= HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2);
bstate[3]= HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0);
	
	
	if (bstate[0]==0&&bstate_last[0]==1){
	state++;
	}else if (bstate[1]==0&&bstate_last[1]==1&&state%3==2){
	select_state++;
	}else if (bstate[1]==0&&bstate_last[1]==1&&state%3==0){   //按下
	TIM2->CNT=0;
	}else if (bstate[1]==1&&bstate_last[1]==0&&state%3==0){   //松手
	 if (TIM2->CNT>20000){
		 
		hour=0;
		 min=0;
		 sec=0;
	 }else if (TIM2->CNT<20000){
		 if (lock_state==1){
		 lock_state=0;
		 }else if (lock_state==0){
		 lock_state=1;
		 }
	 }
	}else if (bstate[2]==0&&bstate_last[2]==1&&state%3==2){
	if (select_state%4==0){
	  DS++;
	 
	  }	else if (select_state%4==1){
	  DR=DR+10;
	 
	  }	else if (select_state%4==2){
	  FS=FS+100;
	 
	  }	else if (select_state%4==3){
    FR=FR+1000;
	 
	  }
	}else if (bstate[3]==0&&bstate_last[3]==1&&state%3==2){
	if (select_state%4==0){
	  DS--;
	 
	  }	else if (select_state%4==1){
	  DR=DR-10;
	 
	  }	else if (select_state%4==2){
	  FS=FS-100;
	 
	  }	else if (select_state%4==3){
    FR=FR-1000;
	 
	  }
	}
	
	
	bstate_last[0]=bstate[0];
	bstate_last[1]=bstate[1];
	bstate_last[2]=bstate[2];
	bstate_last[3]=bstate[3];
}
