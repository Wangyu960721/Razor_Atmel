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
{
  		LedOff(WHITE);
		LedOff(RED);
		LedOff(YELLOW);
		LedOff(ORANGE);
		LedOff(PURPLE);
		LedOff(CYAN);
		LedOff(BLUE);
		LedOff(GREEN);
		LCDCommand(LCD_CLEAR_CMD);


  /* If good initialization, set state 
		to Idle */
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


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for ??? */
static void UserApp1SM_Idle(void)
{	
  	static u8 i=0;
  	static u8 u8i;
	static u8 u8Ok=0;
  	static u8 u8DebugNo=0;
	static u8 u8TimeCounter=0;
	static u8 u8TimeCounter1=0;
	static u8 u8Frequency=2;
	static u8 UserApp_CursorPosition=LINE1_END_ADDR;

	static u8 au8InputBuffer[255]={0,0};
	static u8 au8Array1[20]={0,0};
	static u8 au8Array2[20]={0,0};
	static LedNumberType eLedNo=0;
	

	u8TimeCounter++;
	
	if(WasButtonPressed(BUTTON2))
	{
	  	u8Ok=1;
		ButtonAcknowledge(BUTTON2);
		u8DebugNo=DebugScanf(au8InputBuffer);
	}
	if(u8Ok==1)
	{
		if(u8DebugNo>40)
		{
			if(u8TimeCounter==200)
			{
				
				
				if((UserApp_CursorPosition+1)==LINE1_START_ADDR)
				{
					LCDMessage(LINE1_START_ADDR,&au8InputBuffer[++i]);
					LCDClearChars(LINE2_START_ADDR ,20);
					if(i==u8DebugNo)
					{
						i=0;
					}
				}
				else
				{
					LCDMessage(UserApp_CursorPosition, au8InputBuffer);
					LCDClearChars(LINE2_START_ADDR ,20);
					UserApp_CursorPosition--;
				}
				
				
				LedPWM(eLedNo,LED_PWM_60);
				PWMAudioSetFrequency(BUZZER1,u8Frequency);
				PWMAudioOn(BUZZER1);
				eLedNo++;
				u8Frequency*=2;
				
				if(eLedNo==9)
				{
					eLedNo=0;
					LedOff(WHITE);
					LedOff(RED);
					LedOff(YELLOW);
					LedOff(ORANGE);
					LedOff(PURPLE);
					LedOff(CYAN);
					LedOff(BLUE);
					LedOff(GREEN);
				}
				
				if(u8Frequency==5120)
				{
					u8Frequency=10;
				}
				
				u8TimeCounter=0;
			}
			
		}
		else
		{			
			for(u8i=0;u8i<20;u8i++)
			{
				au8Array1[u8i]=au8InputBuffer[u8i];
			}
			
			for(u8i=20;u8i<40;u8i++)
			{
				au8Array2[u8i-20]=au8InputBuffer[u8i];
			}		
			
			
			
			u8TimeCounter1++;
			
			if(u8TimeCounter1==100)
			{
			  	LCDMessage(LINE1_START_ADDR,au8Array1);
				LCDMessage(LINE2_START_ADDR,au8Array2);
				u8TimeCounter1=0;
			}
		
		}
	
			if(u8TimeCounter==200)
			{

				LedPWM(eLedNo,LED_PWM_30);
				PWMAudioSetFrequency(BUZZER1,u8Frequency);
				PWMAudioOn(BUZZER1);
				eLedNo++;
				u8Frequency*=2;
				
				
				if(eLedNo==9)
				{
					eLedNo=0;
					LedOff(WHITE);
					LedOff(RED);
					LedOff(YELLOW);
					LedOff(ORANGE);
					LedOff(PURPLE);
					LedOff(CYAN);
					LedOff(BLUE);
					LedOff(GREEN);
								
				}
				if(u8Frequency==5120)
				{
					u8Frequency=10;
				}
				
				u8TimeCounter=0;
			}
	}

	
	if(WasButtonPressed(BUTTON0))
	{
	  	ButtonAcknowledge(BUTTON0);
		LedOff(WHITE);
		LedOff(RED);
		LedOff(YELLOW);
		LedOff(ORANGE);
		LedOff(PURPLE);
		LedOff(CYAN);
		LedOff(BLUE);
		LedOff(GREEN);
	}
	if(WasButtonPressed(BUTTON1))
	{
	  	ButtonAcknowledge(BUTTON1);
		DebugPrintf(au8InputBuffer);
	}
	
} /* end UserApp1SM_Idle() */
    

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
} /* end UserApp1SM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
