/**********************************************************************************************************************
File: user_app1.c                                                                

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app1 as a template:
 1. Copy both user_app1.c and user_app1.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "user_app1" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "UserApp1" with "YourNewTaskName"
 6. Use ctrl-h to find and replace all instances of "USER_APP1" with "YOUR_NEW_TASK_NAME"
 7. Add a call to YourNewTaskNameInitialize() in the init section of main
 8. Add a call to YourNewTaskNameRunActiveState() in the Super Loop section of main
 9. Update yournewtaskname.h per the instructions at the top of yournewtaskname.h
10. Delete this text (between the dashed lines) and update the Description below to describe your task
----------------------------------------------------------------------------------------------------------------------

Description:
This is a user_app1.c file template 

------------------------------------------------------------------------------------------------------------------------
API:

Public functions:


Protected System functions:
void UserApp1Initialize(void)
Runs required initialzation for the task.  Should only be called once in main init section.

void UserApp1RunActiveState(void)
Runs current task state.  Should only be called once in main loop.


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_UserApp1"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserApp1Flags;                       /* Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */

extern u8 G_au8DebugScanfBuffer[];  /* From debug.c */
extern u8 G_u8DebugScanfCharCount;  /* From debug.c */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp1_StateMachine;            /* The state machine function pointer */
//static u32 UserApp1_u32Timeout;                      /* Timeout counter used across states */


/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions                                                                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Protected functions                                                                                                */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
Function: UserApp1Initialize

Description:
Initializes the State Machine and its variables.

Requires:
  -

Promises:
  - 
*/
void UserApp1Initialize(void)
		{       LedOff(WHITE);
				LedOff(RED);
				LedOff(PURPLE);
				LedOff(BLUE);
				LedOff(ORANGE);
				LedOff(CYAN);
				LedOff(GREEN);
				LedOff(YELLOW);
				u8 u8String[] = "A string to print that returns cursor to start of next line.\n\r";
				u8 u8String2[] = "Here's a number: ";
				u8 u8String3[] = " < The 'cursor' was here after the number.";
				u32 u32Number = 1234567;

				DebugPrintf(u8String);
				DebugPrintf(u8String2);
				DebugPrintNumber(u32Number);
				DebugPrintf(u8String3);
				DebugLineFeed();
				DebugPrintf(u8String3);
				DebugLineFeed();

  /* If good initialization, set state to Idle */
  if( 1 )
  {
    UserApp1_StateMachine = UserApp1SM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp1_StateMachine = UserApp1SM_Error;
  }

} /* end UserApp1Initialize() */

  
/*----------------------------------------------------------------------------------------------------------------------
Function UserApp1RunActiveState()

Description:
Selects and runs one iteration of the current state in the state machine.
All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
  - State machine function pointer points at current state

Promises:
  - Calls the function to pointed by the state machine function pointer
*/
void UserApp1RunActiveState(void)
{
  UserApp1_StateMachine();

} /* end UserApp1RunActiveState */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/
static bool CheckCMDString(u8*pu8CMDToCheck)
		{
		  	u8 u8Ok=0;
			bool bOk=FALSE;
			u8 u8OnTime=0;
			u8 u8OffTime=0;
			u8 u8Array1No=0;
			u8 u8Array2No=0;
			u8 au8CompareArray1[4];
			u8 au8CompareArray2[4];
			static u8 au8LedName[16]={'R','B','Y','C','W','P','O','G','r','b','y','c','w','p','o','g'};
			u8 *pu8Parser;
			pu8Parser=pu8CMDToCheck;
			for(u8 i=0;i<sizeof(au8LedName);i++)
			{
				if(*pu8Parser==au8LedName[i])
				{
					pu8Parser++;
				}
			}
			if(*pu8Parser=='-')
			{
			  	pu8Parser++;
				for(;*pu8Parser!='-';pu8Parser++)
				{
				  	au8CompareArray1[u8Array1No++]=*pu8Parser;
				  	if(*pu8Parser>='0'&&*pu8Parser<='9')
					{
						u8OnTime++;
						if(u8OnTime>4)
						{
							return FALSE;
						}
					}
				}
				pu8Parser++;
				for(;*pu8Parser!=0x0D;pu8Parser++)
				{
					au8CompareArray2[u8Array2No++]=*pu8Parser;
					if(*pu8Parser>='0'&&*pu8Parser<='9')
					{
						u8OffTime++;
						if(u8OffTime>4)
						{
							return FALSE;
						}
					}
				}
				if(u8OnTime>u8OffTime)
				{
					LedBlink(RED,63);
					bOk=TRUE;
					return FALSE;
				}
				if(u8OnTime==u8OffTime)
				{
					for(u8 u8Num3=0;u8Num3<u8OnTime;u8Num3++)
					{
						if(au8CompareArray1[u8Num3]>au8CompareArray2[u8Num3])
						{
							LedBlink(RED,63);
							bOk=TRUE;
							return FALSE;
						}
					}
				}
			}
			else
			{
				return FALSE;
			}
			
			if(bOk==FALSE)
			{
				LedBlink(WHITE,63);
				return TRUE;
			}
	
		}


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for ??? */
/* Wait for a message to be queued */
static void UserApp1SM_Idle(void)
{
    static u8 au8InputBuffer[]={0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	static u8 u8CompareWord1=0;
	static u8 u8CompareWord2=0;
	static u8 u8CompareWord3=0;
	static u8 u8Ok=0;
	static u8 i;
	static bool bOk=FALSE;
	static u8 u8Num1=0;
	static u8 u8Num2=0;
	static u8 u8Num3=0;
	static u8 au8ComapreArray2[]={0,0,0,0,0,0,0,0,0,0,0,0,0};
	static u8 au8ComapreArray3[]={0,0,0,0,0,0,0,0,0,0,0,0,0};
	
	 
	if(WasButtonPressed(BUTTON0))
	{	
		ButtonAcknowledge(BUTTON0);	   	
		DebugScanf(au8InputBuffer);
	
		if((au8InputBuffer[0]>='a'&&au8InputBuffer[0]<='z')||(au8InputBuffer[0]>='A'&&au8InputBuffer[0]<='Z'))
		{
			if(au8InputBuffer[1]=='-')
			{
			  	for(u8CompareWord1=2;au8InputBuffer[u8CompareWord1]!='-';u8CompareWord1++)
				{
				  	au8ComapreArray2[u8CompareWord1-2]=au8InputBuffer[u8CompareWord1];
				}
					
					for(u8CompareWord2=0;u8CompareWord2<(u8CompareWord1-2);u8CompareWord2++)//compare with each number
					{
						if(au8ComapreArray2[u8CompareWord2]>='0'&&u8CompareWord2<='9')
						{
						  	u8Ok++;
						}
					}
				
				if(u8Ok==u8CompareWord1-2)//each word is number
				{
					if(au8InputBuffer[u8CompareWord1]=='-')
					{
						for(u8CompareWord3=(u8CompareWord1+1);au8InputBuffer[u8CompareWord3]!=' ';u8CompareWord3++)//u8CompareWord1+1 is the first subscript of the second number
						{
						  	 
							 au8ComapreArray3[u8CompareWord3-(u8CompareWord1+1)]=au8InputBuffer[u8CompareWord3];//put the second number into another array
						}
						
						for(i=0;i<13;i++)
						{
							if(au8ComapreArray3[i]!='\0')
							{
								u8Num1++;
							}
							
							if(au8ComapreArray2[i]!='\0')
							{
								u8Num2++;
							}
						}
						
						if(u8Num1>u8Num2)
						{
							LedBlink(RED,63);
							bOk=TRUE;
						}
						if(u8Num1=u8Num2)
						{
						  	for(u8Num3=0;u8Num3<u8Num1;u8Num3++)
							{
								if(au8ComapreArray3[u8Num3]>au8ComapreArray2[u8Num3])
								{
									LedBlink(RED,63);
									bOk=TRUE;
								}
							}
						}

					}
				}
					
	
			}
		}
		
		if(bOk==FALSE)
		{
			LedBlink(WHITE,63);
		}
		
	}
	
	if(WasButtonPressed(BUTTON1))
	{
		ButtonAcknowledge(BUTTON1);
		LedOff(WHITE);
		LedOff(RED);
		DebugLineFeed();
		bOk=FALSE;
		u8Ok=0;
		for(u8 u8n=0;u8n<14;u8n++)
		{
			au8ComapreArray3[u8n]=0;
			au8InputBuffer[u8n]=0;
			au8ComapreArray2[u8n]=0;
		}
	}
	

}/* end UserApp1SM_Idle() */

   


/*  static u8 u8KeyValue=0
  static u8 au8PassWord[4]="123";
  static u8 i=0;
  static u8 au8InPut[4]="000";
  
    LedOn(RED);
  if(i<3)
  {
    if(WasButtonPressed(BUTTON0))
    {
      u8KeyValue=1;
      ButtonAcknowledge(BUTTON0);
      au8InPut[i]=1;
      i++;
    }

    if(IsButtonPressed(BUTTON1))
    {
      u8KeyValue=2;
      ButtonAcknowledge(BUTTON1);
      au8InPut[i]=2;
      i++;
    }
  
    if(IsButtonPressed(BUTTON2))
    {
      u8KeyValue=3;
      ButtonAcknowledge(BUTTON2);
      au8InPut[i]=3;
      i++;
    }
  }
  if(WasButtonPressed(BUTTON3))
  {
    ButtonAcknowledge(BUTTON3);
    if(strcmp(au8PassWord,au8InPut)==0)
    {
      LedBlink(GREEN,LED_8HZ);
    }
    else
    {
      LedOff(RED);
      LedBlink(RED,LED_8HZ);
    }
  }
  
}*/
  
  /*{static u16 u16Counter=0;
    static LedRateType eLedDutyLevel=LED_PWM_0;
    static LedNumberType eLedNo=WHITE;
    static bool bOk=0;
    
    u16Counter++;
	
    if(u16Counter==1)//1ms
	{
          u16Counter=0;

          LedPWM(eLedNo,eLedDutyLevel);

          if(eLedNo==8)//eight leds
            {
              eLedNo=0;
            }
          
          if(bOk<20)//up
          {
            eLedDutyLevel++;
            bOk++;
          }
          else//down
          {
            if(bOk>=20&&bOk<=39)
            {
              eLedDutyLevel--;
              bOk++;
              if(bOk==40)
               {
                 bOk=0;
               }
              if(eLedDutyLevel==LED_PWM_0)
               {
                  LedPWM(eLedNo,eLedDutyLevel);
                  eLedNo++;
               }            

            }
          }//end else
	}//end if
  }//end UserApp1SM_Idle*/


  
  /*if(IsButtonPressed(BUTTON1))
  {
    u8ButtonPressedNum=1;
  }
  
  if(IsButtonPressed(BUTTON2))
  {
    u8ButtonPressedNum=2;
  }
  
  if(IsButtonPressed(BUTTON3))
  {
    u8ButtonPressedNum=3;
  }
    
  switch(u8ButtonPressedNum)
  { 
      case 0:
          LedOn(WHITE);
          u8ButtonPressedNum=0;
          break;
      case 1:
          LedOn(RED);
          u8ButtonPressedNum=0;
          break;
      case 2:
          LedOn(PURPLE);
          u8ButtonPressedNum=0;
          break;
      case 3:
          LedOff(WHITE);
          LedOff(RED);
          LedOff(PURPLE);
          u8ButtonPressedNum=0;
          break;
      default:
          break;
  }*/
          
//end usser app1
/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
