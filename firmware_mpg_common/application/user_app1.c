/**********************************************************************************************************************
File: user_app.c                                                                

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app as a template:
 1. Copy both user_app.c and user_app.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "user_app" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "UserApp1" with "YourNewTaskName"
 6. Use ctrl-h to find and replace all instances of "USER_APP" with "YOUR_NEW_TASK_NAME"
 7. Add a call to YourNewTaskNameInitialize() in the init section of main
 8. Add a call to YourNewTaskNameRunActiveState() in the Super Loop section of main
 9. Update yournewtaskname.h per the instructions at the top of yournewtaskname.h
10. Delete this text (between the dashed lines) and update the Description below to describe your task
----------------------------------------------------------------------------------------------------------------------

Description:
This is a user_app.c file template 

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
All Global variable names shall start with "G_"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserApp1Flags;                       /* Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern u32 G_u32AntApiCurrentDataTimeStamp;                       /* From ant_api.c */
extern AntApplicationMessageType G_eAntApiCurrentMessageClass;    /* From ant_api.c */
extern u8 G_au8AntApiCurrentMessageBytes[ANT_APPLICATION_MESSAGE_BYTES];  /* From ant_api.c */
extern AntExtendedDataType G_sAntApiCurrentMessageExtData;        /* From ant_api.c */

extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */



/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_" and be declared as static.
***********************************************************************************************************************/
static u32 UserApp1_u32DataMsgCount = 0;             /* Counts the number of ANT_DATA packets received */          /* Counts the number of ANT_TICK packets received */

static fnCode_type UserApp1_StateMachine;            /* The state machine function pointer */
static u32 UserApp1_u32Timeout;                      /* Timeout counter used across states */

static s8 SignalStrength=0;
static bool bOk=TRUE;
static AntAssignChannelInfoType sAntSetupData;
static bool bTime = FALSE;
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
    u8 au8WelcomeMessage[] = "HIDE AND GO SEEK!";
    u8 au8Instructions[] = "PRESS B1 TO CHOOSE YOUR ROLE!";
    LedOff(WHITE);
    LedOff(PURPLE);
    LedOff(BLUE);
    LedOff(CYAN);
    LedOff(GREEN);
    LedOff(YELLOW);
    LedOff(ORANGE);
    LedOff(RED);
#ifdef EIE1
  LCDCommand(LCD_CLEAR_CMD);
  LCDMessage(LINE1_START_ADDR,au8WelcomeMessage); 
  LCDMessage(LINE2_START_ADDR, au8Instructions); 
  UserApp1_StateMachine=ChooseStatus;
#endif /* EIE1 */
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
/* Wait for the ANT channel assignment to finish */
static void UserApp1SM_WaitChannelAssign(void)
{
  /* Check if the channel assignment is complete */
  if(AntRadioStatusChannel(ANT_CHANNEL_USERAPP) == ANT_CONFIGURED)
  {
    UserApp1_StateMachine = UserApp1SM_Idle;
  }
  
  /* Monitor for timeout */
      
} /* end UserApp1SM_WaitChannelAssign() */
  /*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for a message to be queued */
static void UserApp1SM_Idle(void)
{
    /* Queue open channel and change LED0 from yellow to blinking green to indicate channel is opening */
    AntOpenChannelNumber(ANT_CHANNEL_USERAPP);
    PWMAudioOn(BUZZER1);
    UserApp1_u32Timeout = G_u32SystemTime1ms;
    UserApp1_StateMachine = UserApp1SM_WaitChannelOpen;
} /* end UserApp1SM_Idle() */
     

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for channel to open */
static void UserApp1SM_WaitChannelOpen(void)
{
  /* Monitor the channel status to check if channel is opened */
  if(AntRadioStatusChannel(ANT_CHANNEL_USERAPP) == ANT_OPEN)
  {
     if(bOk==0)
     {
        LCDCommand(LCD_CLEAR_CMD);
        LCDMessage(LINE1_START_ADDR,"Seeker");
        UserApp1_StateMachine = UserApp1SM_OpenSlaveChannel;
     }
     else
     {
        LCDCommand(LCD_CLEAR_CMD);
        LCDMessage(LINE1_START_ADDR,"HIDE!");
        UserApp1_StateMachine = UserApp1SM_OpenMasterChannel;
     }
  }
} /* end UserApp1SM_WaitChannelOpen() */

static void UserApp1SM_OpenMasterChannel(void)
{
    static u8 au8TestMessage[] = {6, 6, 6, 2, 0xA5, 0, 0, 0};
    static u8 au8Restart[30]="PRESS BUTTON0 TO RESTART!";
    static bool bRestart = FALSE;

    
    if( AntReadAppMessageBuffer() )
    {
     /* New message from ANT task: check what it is */
        if(G_eAntApiCurrentMessageClass == ANT_DATA)
        {
          if(G_au8AntApiCurrentMessageBytes[0] == 'F')
          {
            LCDCommand(LCD_CLEAR_CMD);
            LCDMessage(LINE1_START_ADDR,"YOU FOUND ME!");
            LCDMessage(LINE2_START_ADDR,&au8Restart[0]);
            bRestart=TRUE;
          }
        }
        else if(G_eAntApiCurrentMessageClass == ANT_TICK)
        {
          AntQueueBroadcastMessage(ANT_CHANNEL_USERAPP, au8TestMessage);
        }

        if( bRestart && WasButtonPressed(BUTTON0) )
        {
          ButtonAcknowledge(BUTTON0);
          bRestart = FALSE;
          AntCloseChannelNumber(ANT_CHANNEL_USERAPP);
          AntUnassignChannelNumber(ANT_CHANNEL_USERAPP);
          UserApp1_StateMachine = UserApp1Initialize;
        }
    } /* end AntReadData() */
}
static void UserApp1SM_OpenSlaveChannel(void)
{
  static u8 au8Feedback[]="FIND YOU!";
  static u8 au8Data[2];
  static s8 CurrentSignal = 0;
  static bool bRestart=FALSE;
  
  if( AntReadAppMessageBuffer() )
  {
     /* New data message: check what it is */
    if(G_eAntApiCurrentMessageClass == ANT_DATA)
    {
      UserApp1_u32DataMsgCount++;
      SignalStrength=-1*G_sAntApiCurrentMessageExtData.s8RSSI;
      
      au8Data[0]=SignalStrength/10+0x30;
      au8Data[1]=SignalStrength%10+0x30;
      
      LCDMessage(LINE1_START_ADDR+12, "RSSI:"); 
      LCDMessage(LINE1_START_ADDR+18, au8Data);
      
      if(CurrentSignal != (SignalStrength-40)/6 )
      {
          CurrentSignal = (SignalStrength-40)/6;
          switch(CurrentSignal)
          {
              case 0:
                LedOn(WHITE);
                LedOn(PURPLE);
                LedOn(BLUE);
                LedOn(CYAN);
                LedOn(GREEN);
                LedOn(YELLOW);
                LedOn(ORANGE);
                LedOn(RED);
                PWMAudioSetFrequency(BUZZER1,500);
                AntQueueAcknowledgedMessage(ANT_CHANNEL_USERAPP,au8Feedback);
                LCDMessage(LINE1_START_ADDR,"I FIND YOU!");
                bRestart=TRUE;
                LCDMessage(LINE2_START_ADDR,"PRESS B0 TO START");
                break;
              case 1:
                LedOn(WHITE);
                LedOn(PURPLE);
                LedOn(BLUE);
                LedOn(CYAN);
                LedOn(GREEN);
                LedOn(YELLOW);
                LedOn(ORANGE);
                LedOff(RED);
                PWMAudioSetFrequency(BUZZER1,400);
                break;
              case 2:
                LedOn(WHITE);
                LedOn(PURPLE);
                LedOn(BLUE);
                LedOn(CYAN);
                LedOn(GREEN);
                LedOn(YELLOW);
                LedOff(ORANGE);
                LedOff(RED);
                PWMAudioSetFrequency(BUZZER1,300);
                break;
              case 3:        
                LedOn(WHITE);
                LedOn(PURPLE);
                LedOn(BLUE);
                LedOn(CYAN);
                LedOn(GREEN);
                LedOff(YELLOW);
                LedOff(ORANGE);
                LedOff(RED);
                PWMAudioSetFrequency(BUZZER1,200);
                break;
              case 4:
                LedOn(WHITE);
                LedOn(PURPLE);
                LedOn(BLUE);
                LedOn(CYAN);
                LedOff(GREEN);
                LedOff(YELLOW);
                LedOff(ORANGE);
                LedOff(RED);
                PWMAudioSetFrequency(BUZZER1,100);
                break;
              case 5:
                LedOn(WHITE);
                LedOn(PURPLE);
                LedOn(BLUE);
                LedOff(CYAN);
                LedOff(GREEN);
                LedOff(YELLOW);
                LedOff(ORANGE);
                LedOff(RED);
                PWMAudioSetFrequency(BUZZER1,0);
                break;
              case 6:
                LedOn(WHITE);
                LedOn(PURPLE);
                LedOff(BLUE);
                LedOff(CYAN);
                LedOff(GREEN);
                LedOff(YELLOW);
                LedOff(ORANGE);
                LedOff(RED);
                break;
              case 7:
                LedOn(WHITE);
                LedOff(PURPLE);
                LedOff(BLUE);
                LedOff(CYAN);
                LedOff(GREEN);
                LedOff(YELLOW);
                LedOff(ORANGE);
                LedOff(RED);
                break;
              default:
                break;
              }/*end of switch*/
      }
      if(WasButtonPressed(BUTTON3))
      {
        ButtonAcknowledge(BUTTON3);
        PWMAudioOff(BUZZER1);
      }
      if(WasButtonPressed(BUTTON2))
      {
        ButtonAcknowledge(BUTTON2);
        PWMAudioOn(BUZZER1);
      }
      
      if(WasButtonPressed(BUTTON0)&&bRestart)
      {
        ButtonAcknowledge(BUTTON0);
        AntCloseChannelNumber(ANT_CHANNEL_USERAPP);
        AntUnassignChannelNumber(ANT_CHANNEL_USERAPP);
        bRestart=FALSE;
        UserApp1_StateMachine = UserApp1Initialize;
      }

    } /* end if(G_eAntApiCurrentMessageClass == ANT_DATA) */
  }
} /* end UserApp1SM_ChannelOpen() */

static void ChooseStatus(void)
{
    static u32 u32TimeCounter=0;
    u8 au8WelcomeMessage2[] = "Hide!";
    u8 au8Instructions2[] = "PRESS B0 TO START!";
    u8 au8WelcomeMessage1[] = "Seeker!";
    u8 au8Instructions1[] = "PRESS B0 TO START!";
    
    
    if(WasButtonPressed(BUTTON1))
    {
        ButtonAcknowledge(BUTTON1);
        bOk=!bOk;

        if(bOk==0)
        {
            sAntSetupData.AntChannel          = ANT_CHANNEL_USERAPP;
            sAntSetupData.AntChannelType      = CHANNEL_TYPE_SLAVE;
            sAntSetupData.AntChannelPeriodLo  = ANT_CHANNEL_PERIOD_LO_USERAPP;
            sAntSetupData.AntChannelPeriodHi  = ANT_CHANNEL_PERIOD_HI_USERAPP;

            sAntSetupData.AntDeviceIdLo       = ANT_DEVICEID_LO_USERAPP;
            sAntSetupData.AntDeviceIdHi       = ANT_DEVICEID_HI_USERAPP;
            sAntSetupData.AntDeviceType       = ANT_DEVICE_TYPE_USERAPP;
            sAntSetupData.AntTransmissionType = ANT_TRANSMISSION_TYPE_USERAPP;
            sAntSetupData.AntFrequency        = ANT_FREQUENCY_USERAPP;
            sAntSetupData.AntTxPower          = ANT_TX_POWER_USERAPP;

            sAntSetupData.AntNetwork = ANT_NETWORK_DEFAULT;
            for(u8 i = 0; i < ANT_NETWORK_NUMBER_BYTES; i++)
            {
                sAntSetupData.AntNetworkKey[i] = ANT_DEFAULT_NETWORK_KEY;
            }
            
            LCDCommand(LCD_CLEAR_CMD);
            LCDMessage(LINE1_START_ADDR, au8WelcomeMessage1); 
            LCDMessage(LINE2_START_ADDR, au8Instructions1);
            
        }
        else
        {
            sAntSetupData.AntChannel          = ANT_CHANNEL_USERAPP;
            sAntSetupData.AntChannelType      = CHANNEL_TYPE_MASTER;
            sAntSetupData.AntChannelPeriodLo  = ANT_CHANNEL_PERIOD_LO_USERAPP;
            sAntSetupData.AntChannelPeriodHi  = ANT_CHANNEL_PERIOD_HI_USERAPP;

            sAntSetupData.AntDeviceIdLo       = ANT_DEVICEID_LO_USERAPP;
            sAntSetupData.AntDeviceIdHi       = ANT_DEVICEID_HI_USERAPP;
            sAntSetupData.AntDeviceType       = ANT_DEVICE_TYPE_USERAPP;
            sAntSetupData.AntTransmissionType = ANT_TRANSMISSION_TYPE_USERAPP;
            sAntSetupData.AntFrequency        = ANT_FREQUENCY_USERAPP;
            sAntSetupData.AntTxPower          = ANT_TX_POWER_USERAPP;

            sAntSetupData.AntNetwork = ANT_NETWORK_DEFAULT;
            for(u8 i = 0; i < ANT_NETWORK_NUMBER_BYTES; i++)
            {
                sAntSetupData.AntNetworkKey[i] = ANT_DEFAULT_NETWORK_KEY;
            }
            
            LCDCommand(LCD_CLEAR_CMD);
            LCDMessage(LINE1_START_ADDR, au8WelcomeMessage2); 
            LCDMessage(LINE2_START_ADDR, au8Instructions2);
        }
    }
    
    if(WasButtonPressed(BUTTON0))
    {   
        LCDCommand(LCD_CLEAR_CMD);
        ButtonAcknowledge(BUTTON0);
        bTime = TRUE;
    }

    if(bTime&&!bOk)
    {
        u32TimeCounter++;
        
        switch (u32TimeCounter)
        {
            case 1:
                LCDMessage(LINE1_START_ADDR,"Time Last :10");
                break;
            case 1000:
                LCDMessage(LINE1_START_ADDR, "Time Last :9 ");
                break;
            case 2000:
                LCDMessage(LINE1_START_ADDR, "Time Last :8 ");
                break;
            case 3000:
                LCDMessage(LINE1_START_ADDR, "Time Last :7 ");
                break;
            case 4000:
                LCDMessage(LINE1_START_ADDR, "Time Last :6 ");
                break;
            case 5000:
                LCDMessage(LINE1_START_ADDR, "Time Last :5 ");
                break;
            case 6000:
                LCDMessage(LINE1_START_ADDR, "Time Last :4 ");
                break;
            case 7000:
                LCDMessage(LINE1_START_ADDR, "Time Last :3 ");
                break;
            case 8000:
                LCDMessage(LINE1_START_ADDR, "Time Last :2 ");
                break;
            case 9000:
                LCDMessage(LINE1_START_ADDR, "Time Last :1 ");
                break;
            default:
                break;
        }
    }
        
        if((u32TimeCounter>=9000||bOk)&&bTime)
        {
            if( AntAssignChannel(&sAntSetupData) )
            {
                u32TimeCounter = 0;
                bTime=FALSE;
                UserApp1_StateMachine = UserApp1SM_WaitChannelAssign;
            }
        }
}
/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
