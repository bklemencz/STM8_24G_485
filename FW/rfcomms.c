#include "rfcomms.h"
#include "lt8900.h"
#include "serialcomms.h"
#include "eeprom.h"
#include "main.h"

///																			RF DATA STRUCTURE
///// 														7			6			5			4			3			2			1			0
////										byte0		|									NodeID												|
////                    byte1   |                Sequence                       |
////										byte2 	|  MsgType  |ARQ | ARS |         Length         |
////										byte3   |				          ItemID							          |
///											byte4.. |								DATA x													|

///											sequence: counter - increased every message
///											ARQ: 			Sender wants an ACK message
///											ARS:			Flags ACK message (3bytes, NODEID, Sequence of the original message, ARQ-clear, ARS-set, Msgtype - reply)
///											msgtype: 	00 - request,
///																01 - reply,
///																10 - periodical mess,
///																11-  Alarm mess
///											ItemID: 	Node Function ID
///											Length:   Data bytes count + 1



Group_Status_t Group_Command[4];
RF_Params_t RF_Status;
uint8_t RF_Command_LastSeq;

uint8_t RF_NODEID;                    //Local NODE ID
uint8_t RF_RX_MI_LastCounter;         //Last MI Remote pocket number
uint8_t RF_RX_MI_PrevCommand;         //Mi Remote Previous Command
bool RF_RX_MI_FirstPress;             //First new command - maybe used for key hold timer

uint8_t RF_RX_Buffer[17];     //Buffer to store incomming 2.4G messages
uint8_t RF_TX_Buffer[17];
uint8_t RF_TX_Counter;

extern uint8_t Main_State;
extern uint16_t Main_Config;
extern uint8_t Conf_LED_Max_Brightness;
extern uint16_t Conf_LED_TurnOff_Delay_S;
extern uint8_t Conf_LED_NightMode_Brightness;
extern uint8_t Conf_LED_Fade_Delay;

void RF_SetNodeID(uint8_t Node)
{
  RF_NODEID = Node;
}

uint8_t RF_GetNodeID(void)
{
  return RF_NODEID;
}

void RF_Send_Motion(uint8_t Group)
{
  uint8_t i;
  RF_TX_Buffer[0] = RF_NODEID;
  RF_TX_Counter++;
  RF_TX_Buffer[1] = RF_TX_Counter;
  RF_TX_Buffer[2] = 0xE2;             // Contol Byte showing alarm message
  RF_TX_Buffer[3] = 64;               // Motion event code
  RF_TX_Buffer[4] = Group - 100;      // ID of motion sensor
  for (i=0; i<RF_TX_REPEAT; i++) LT8900_sendPacket(RF_TX_Buffer, 5);
  LT8900_startListening();
}

int8_t RF_RX_Handle()
{
  bool ValidMI = FALSE;


  if (LT8900_available_R())
    {
      int8_t packetSize;
      packetSize = LT8900_read(RF_RX_Buffer, 16);
      if (packetSize > 0)
      {
				
        ValidMI = RF_RX_MIRemote(RF_RX_Buffer,packetSize);
        if (!ValidMI) RF_RX_Command(RF_RX_Buffer,packetSize);
      }
      if (packetSize != 0) 
      {
          LT8900_startListening();      // LT8900 Clear FIFOS, Rx Enable
      }
      return packetSize;
    }


}

void RF_RX_Check_LT8900(void)
{
	if ((LT8900_readRegister(R_CHANNEL)%32) != DEFAULT_CHANNEL)
	{
		LT8900_softReset();
    LT8900_InitRegisters();
		LT8900_setChannel(DEFAULT_CHANNEL);                                                         // Set recieve channel
		LT8900_startListening();
	}
}

/// Checks for Valid Stored remote address, or learns a New one
bool RF_RX_MI_ChkAddress(uint16_t MI_Address, uint8_t MI_Command)
{

  if (MI_Address == RF_Status.MI_Remote_Address) return TRUE;
  else if ((RF_Status.Learn_Enabled)  && (MI_Command == 0x15))
  {
    RF_Status.Learn_Counter++;
    if (RF_Status.Learn_Counter >= 20)          //HOLD all on for 5s
    {
      RF_Status.MI_Remote_Address = MI_Address;
      EEPROM_Program2Byte(47, RF_Status.MI_Remote_Address);
      return TRUE;
    } else return FALSE;
  }

  return FALSE;
}

bool RF_RX_MIRemote(uint8_t *RX_Buffer, int8_t Lenght)
{

  uint8_t i;
  bool Valid;
  uint16_t Rem_Address;
  uint8_t Counter;
  uint8_t Group;
  bool  KeyHold;
  uint8_t Command;

  // Valid MI Command
  if ((Lenght == 7) && (RX_Buffer[0]==0x5A))
  {

    Valid = TRUE;
    if (RF_RX_MI_LastCounter != RX_Buffer[5])
    {
      Rem_Address = RX_Buffer[1] * 256 + RX_Buffer[2];
      Counter = RX_Buffer[5];
      RF_RX_MI_LastCounter = Counter;
      Group = RX_Buffer[3];
      if (RX_Buffer[4]/16) KeyHold = TRUE;
                      else KeyHold = FALSE;
      Command = RX_Buffer[4] % 16;                //AllON-0x05, AllOFF-0x09,
                                                  //UP-0x0C, DOWN-0x04,
                                                  //LEFT-0x0E, RIGHT-0x0F,
                                                  //GR1ON-0x08, GR1OFF-0x0B,
                                                  //GR2ON-0x0D, GR2OFF-0x03,
                                                  //GR3ON-0x07, GR3OFF-0x0A,
                                                  //GR4ON-0x02, GR4OFF-0x06,
      if (RF_RX_MI_ChkAddress(Rem_Address,RX_Buffer[4]))
      {
        // MAIN ON KEY, PRESSING IT TURNS ON ALL LIGHT TO THE LAST BRIGHTNESS, HOLDING IT TURNS ON ALL IN NIGHT MODE

        if (Command == 0x05)
        {
          if (!KeyHold) {
            if (RF_RX_MI_PrevCommand!=Command)
            {
              RF_RX_MI_PrevCommand = Command;
              RF_RX_MI_FirstPress = TRUE;
            } else RF_RX_MI_FirstPress = FALSE;
            for (i=0;i<4;i++)                     //All on to last PWM
            {
              Group_Command[i].Light_Enabled = TRUE;
              Group_Command[i].MotionEnabled = FALSE;
              Group_Command[i].NightMode = FALSE;
            }
            Serial_Send_PWM_NightEN(255,0);       // All night off
            //Serial_Send_PWM_MotionEN(255,0);      // All motion off
            _delay_ms(RS485_MESSAGE_DELAY);
            Serial_Send_PWM_Onoff(255,1);         // All On

          } else                                  // ALL at night mode
          {
            for (i=0;i<4;i++)                     //All on to last PWM
            {
              Group_Command[i].NightMode = TRUE;
              Group_Command[i].Light_Enabled = TRUE;
              RF_RX_MI_FirstPress = FALSE;
            }
            Serial_Send_PWM_NightEN(255,1);
          } //nightmode all
        } // All on command

        // MAIN OFF KEY, PRESSING IT SETS MOTION ACTIVATED MODE FOR ALL,, HOLDING TURNS OFF ALL LIGHTS.
        else if (Command == 0x09)
        {
          if (!KeyHold) {
            if (RF_RX_MI_PrevCommand!=Command)
            {
              RF_RX_MI_PrevCommand = Command;
              RF_RX_MI_FirstPress = TRUE;
            } else RF_RX_MI_FirstPress = FALSE;
            for (i=0;i<4;i++)                     //All motion mode
            {
              Group_Command[i].MotionEnabled = TRUE;
            }
            Serial_Send_PWM_MotionEN(255,1);
            
          } else                                  // ALL off
          {
            for (i=0;i<4;i++)
            {

              Group_Command[i].MotionEnabled = FALSE;
              Group_Command[i].Light_Enabled = FALSE;
              RF_RX_MI_FirstPress = FALSE;
            }
            Serial_Send_PWM_MotionEN(255,0);
            Serial_Send_PWM_Onoff(255,0);
          } //all off
        } // All off command

        ////////////////////////////////////////
        ///// GROUP 1 ///////////////////////
        ///////////////////////////////////////

        // GROUP1 ON KEY, PRESSING IT TURNS ON GROUP1 TO THE LAST BRIGHTNESS, HOLDING IT TURNS ON GROUP1 IN NIGHT MODE
        else if (Command == 0x08)
        {
          if (!KeyHold) {
              //GR1 on to last PWM
              if (RF_RX_MI_PrevCommand!=Command)
              {
                RF_RX_MI_PrevCommand = Command;
                RF_RX_MI_FirstPress = TRUE;
              } 
              else RF_RX_MI_FirstPress = FALSE;
              
              Group_Command[0].Light_Enabled = TRUE;
              Group_Command[0].MotionEnabled = FALSE;
              Group_Command[0].NightMode = FALSE;
              
              Serial_Send_PWM_Onoff(101,TRUE);
          } else                                  // GR1 at night mode
          {
              Group_Command[0].NightMode = TRUE;
              Group_Command[0].Light_Enabled = TRUE;
              RF_RX_MI_FirstPress = FALSE;
              
              Serial_Send_PWM_NightEN(101,TRUE);
          } //nightmode GR1
        } // GR1 on command

        // GR1 OFF KEY, PRESSING IT TURNS OFF GR1 LIGHTS, HOLDING SETS MOTION ACTIVATED MODE FOR GR1.
        else if (Command == 0x0B)               //GR1 Motion
        {
          if (!KeyHold) {
            if (RF_RX_MI_PrevCommand!=Command)
            {
              RF_RX_MI_PrevCommand = Command;
              RF_RX_MI_FirstPress = TRUE;
            } else RF_RX_MI_FirstPress = FALSE;

              Group_Command[0].MotionEnabled = TRUE;

              Serial_Send_PWM_MotionEN(101,TRUE);
              
              
          } else                                  // GR1 off
          {
              Group_Command[0].MotionEnabled = FALSE;
              Group_Command[0].Light_Enabled = FALSE;
              
              RF_RX_MI_FirstPress = FALSE;
              Serial_Send_PWM_Onoff(101,FALSE);
          } //GR1 motion
        } // GR1 off command


        ////////////////////////////////////////
        ///// GROUP 2 ///////////////////////
        ///////////////////////////////////////

        // GROUP2 ON KEY, PRESSING IT TURNS ON GROUP2 TO THE LAST BRIGHTNESS, HOLDING IT TURNS ON GROUP2 IN NIGHT MODE
        else if (Command == 0x0D)
        {
          if (!KeyHold) {
            if (RF_RX_MI_PrevCommand!=Command)
            {
              RF_RX_MI_PrevCommand = Command;
              RF_RX_MI_FirstPress = TRUE;
            }
            else RF_RX_MI_FirstPress = FALSE;
              //GR1 on to last PWM
              Group_Command[1].Light_Enabled = TRUE;
              Group_Command[1].MotionEnabled = FALSE;
              Group_Command[1].NightMode = FALSE;

              Serial_Send_PWM_Onoff(102,TRUE);
          } else                                  // GR at night mode
          {
              Group_Command[1].NightMode = TRUE;
              Group_Command[1].Light_Enabled = TRUE;

              RF_RX_MI_FirstPress = FALSE;
              Serial_Send_PWM_NightEN(102,TRUE);
          } //nightmode GR
        } // GR on command

        // GR OFF KEY, PRESSING IT TURNS OFF GR LIGHTS, HOLDING SETS MOTION ACTIVATED MODE FOR GR.
        else if (Command == 0x03)
        {
          if (!KeyHold) {
            if (RF_RX_MI_PrevCommand!=Command)
            {
              RF_RX_MI_PrevCommand = Command;
              RF_RX_MI_FirstPress = TRUE;
            } 
            else RF_RX_MI_FirstPress = FALSE;
             
               Group_Command[1].MotionEnabled = TRUE;

              Serial_Send_PWM_MotionEN(102,TRUE);
          } else                                  // GR at motion mode
          {
              Group_Command[1].MotionEnabled = FALSE;
              Group_Command[1].Light_Enabled = FALSE;
              
              RF_RX_MI_FirstPress = FALSE;
              Serial_Send_PWM_Onoff(102,FALSE);
          } //GR motion
        } // GR off command


        ////////////////////////////////////////
        ///// GROUP 3 ///////////////////////
        ///////////////////////////////////////

        // GROUP ON KEY, PRESSING IT TURNS ON GROUP TO THE LAST BRIGHTNESS, HOLDING IT TURNS ON GROUP IN NIGHT MODE
        else if (Command == 0x07)
        {
          if (!KeyHold) {
            if (RF_RX_MI_PrevCommand!=Command)
            {
              RF_RX_MI_PrevCommand = Command;
              RF_RX_MI_FirstPress = TRUE;
            } 
            else RF_RX_MI_FirstPress = FALSE;
              //GR on to last PWM
            
              Group_Command[2].Light_Enabled = TRUE;
              Group_Command[2].MotionEnabled = FALSE;
              Group_Command[2].NightMode = FALSE;
              
              Serial_Send_PWM_Onoff(103,TRUE);
          } else                                  // GR at night mode
          {
              Group_Command[2].NightMode = TRUE;
              Group_Command[2].Light_Enabled = TRUE;

              RF_RX_MI_FirstPress = FALSE;
              
              Serial_Send_PWM_NightEN(103,TRUE);
          } //nightmode GR
        } // GR on command

        // GR OFF KEY, PRESSING IT TURNS OFF GR LIGHTS, HOLDING SETS MOTION ACTIVATED MODE FOR GR.
        else if (Command == 0x0A)
        {
          if (!KeyHold) {
            if (RF_RX_MI_PrevCommand!=Command)
            {
              RF_RX_MI_PrevCommand = Command;
              RF_RX_MI_FirstPress = TRUE;
            } 
            else RF_RX_MI_FirstPress = FALSE;

            Group_Command[2].MotionEnabled = TRUE;
            
            Serial_Send_PWM_MotionEN(103,TRUE);
            
          } else                                  // GR at motion mode
          {
              Group_Command[2].MotionEnabled = FALSE;
              Group_Command[2].Light_Enabled = FALSE;

              RF_RX_MI_FirstPress = FALSE;
              Serial_Send_PWM_Onoff(103,FALSE);
          } //GR motion
        } // GR off command

        ////////////////////////////////////////
        ///// GROUP 4 ///////////////////////
        ///////////////////////////////////////

        // GROUP ON KEY, PRESSING IT TURNS ON GROUP TO THE LAST BRIGHTNESS, HOLDING IT TURNS ON GROUP IN NIGHT MODE
        else if (Command == 0x02)
        {
          if (!KeyHold) {
            if (RF_RX_MI_PrevCommand!=Command)
            {
              RF_RX_MI_PrevCommand = Command;
              RF_RX_MI_FirstPress = TRUE;
            } 
            else RF_RX_MI_FirstPress = FALSE;
              //GR on to last PWM
              Group_Command[3].Light_Enabled = TRUE;
              Group_Command[3].MotionEnabled = FALSE;
              Group_Command[3].NightMode = FALSE;

              //if (!Shutter_Enabled[3]) 
              Serial_Send_PWM_Onoff(104,TRUE);
              //else Serial_Send_Shutter_Start(4);

          } else                                  // GR at night mode
          {
              Group_Command[3].NightMode = TRUE;
              Group_Command[3].Light_Enabled = TRUE;
              RF_RX_MI_FirstPress = FALSE;
              //if (!Shutter_Enabled[3]) 
              Serial_Send_PWM_NightEN(104,TRUE);
              //else Serial_Send_Shutter_GoUp(4);
          } //nightmode GR
        } // GR on command

        // GR OFF KEY, PRESSING IT TURNS OFF GR LIGHTS, HOLDING SETS MOTION ACTIVATED MODE FOR GR.
        else if (Command == 0x06)
        {
          if (!KeyHold) {
            if (RF_RX_MI_PrevCommand!=Command)
            {
              RF_RX_MI_PrevCommand = Command;
              RF_RX_MI_FirstPress = TRUE;
            } 
            else RF_RX_MI_FirstPress = FALSE;
            
            Group_Command[3].MotionEnabled = TRUE;
            
            Serial_Send_PWM_MotionEN(104,TRUE);

          } else                                  // GR at motion mode
          {
              Group_Command[3].MotionEnabled = FALSE;
              Group_Command[3].Light_Enabled = FALSE;

              RF_RX_MI_FirstPress = FALSE;
              Serial_Send_PWM_Onoff(104,FALSE);
          } //GR motion
        } // GR off command

        ////////////////////////////////////////
        ///// UP KEY ///////////////////////
        ///////////////////////////////////////

        else if (Command == 0x0C)
        {
          RF_RX_MI_FirstPress = FALSE;
          if (!KeyHold)                     // single press
          {
            if (Group == 0)                 //global command
            {
              for (i=0;i<4;i++)
              {
                if (Group_Command[i].Light_Enabled == TRUE)
                {
                  Group_Command[i].CW_Bright++;
                  Group_Command[i].CC_Bright++;
                  if (Group_Command[i].CW_Bright > Conf_LED_Max_Brightness) Group_Command[i].CW_Bright = Conf_LED_Max_Brightness;
                  if (Group_Command[i].CC_Bright > Conf_LED_Max_Brightness) Group_Command[i].CC_Bright = Conf_LED_Max_Brightness;

                  Serial_Send_PWM_Bright((101+i),Group_Command[i].CW_Bright,Group_Command[i].CC_Bright); _delay_ms(RS485_MESSAGE_DELAY);
                }
              }
            } else                          //group command
            {
              if (Group_Command[Group-1].Light_Enabled == TRUE)
              {
                Group_Command[Group-1].CW_Bright++;
                Group_Command[Group-1].CC_Bright++;
                if (Group_Command[Group-1].CW_Bright > Conf_LED_Max_Brightness) Group_Command[Group-1].CW_Bright = Conf_LED_Max_Brightness;
                if (Group_Command[Group-1].CC_Bright > Conf_LED_Max_Brightness) Group_Command[Group-1].CC_Bright = Conf_LED_Max_Brightness;
                Serial_Send_PWM_Bright((100+Group),Group_Command[Group-1].CW_Bright,Group_Command[Group-1].CC_Bright);
              }
            }
          }
          else                              //key hold
          {
            if (Group == 0)                   //global command
            {
              for (i=0;i<4;i++)
              {
                if (Group_Command[i].Light_Enabled == TRUE)
                {
                  Group_Command[i].CW_Bright += RF_MILIGHT_HOLD_STEP;
                  Group_Command[i].CC_Bright += RF_MILIGHT_HOLD_STEP;
                  if (Group_Command[i].CW_Bright > Conf_LED_Max_Brightness) Group_Command[i].CW_Bright = Conf_LED_Max_Brightness;
                  if (Group_Command[i].CC_Bright > Conf_LED_Max_Brightness) Group_Command[i].CC_Bright = Conf_LED_Max_Brightness;
                  Serial_Send_PWM_Bright((101+i),Group_Command[i].CW_Bright,Group_Command[i].CC_Bright); _delay_ms(RS485_MESSAGE_DELAY);
                }
              }
            } else                        // group command
            {
              if (Group_Command[Group-1].Light_Enabled == TRUE)
              {
                Group_Command[Group-1].CW_Bright += RF_MILIGHT_HOLD_STEP;
                Group_Command[Group-1].CC_Bright += RF_MILIGHT_HOLD_STEP;
                if (Group_Command[Group-1].CW_Bright > Conf_LED_Max_Brightness) Group_Command[Group-1].CW_Bright = Conf_LED_Max_Brightness;
                if (Group_Command[Group-1].CC_Bright > Conf_LED_Max_Brightness) Group_Command[Group-1].CC_Bright = Conf_LED_Max_Brightness;
                Serial_Send_PWM_Bright((100+Group),Group_Command[Group-1].CW_Bright,Group_Command[Group-1].CC_Bright);
              }
            }       //End of single group
          }         // End of Keyhold
          for (i=0;i<4;i++)                 //limit maximum
          {
            if (Group_Command[i].CW_Bright > Conf_LED_Max_Brightness) Group_Command[i].CW_Bright = Conf_LED_Max_Brightness;
            if (Group_Command[i].CC_Bright > Conf_LED_Max_Brightness) Group_Command[i].CC_Bright = Conf_LED_Max_Brightness;
          }
          Save_Targets();
        }           //end of command

        ////////////////////////////////////////
        ///// DOWN KEY ///////////////////////
        ///////////////////////////////////////

        else if (Command == 0x04)
        {
          RF_RX_MI_FirstPress = FALSE;
          if (!KeyHold)                     // single press
          {
            if (Group == 0)                 //global command
            {
              for (i=0;i<4;i++)
              {
                if (Group_Command[i].Light_Enabled == TRUE)
                {
                  if (Group_Command[i].CW_Bright > 2)   Group_Command[i].CW_Bright--;
                  if (Group_Command[i].CC_Bright > 2)   Group_Command[i].CC_Bright--;
                  Serial_Send_PWM_Bright((101+i),Group_Command[i].CC_Bright,Group_Command[i].CC_Bright); _delay_ms(RS485_MESSAGE_DELAY);
                }
              }
            } else                          //group command
            {
              if (Group_Command[Group-1].Light_Enabled == TRUE)
              {
                if (Group_Command[Group-1].CW_Bright > 2)   Group_Command[Group-1].CW_Bright--;
                if (Group_Command[Group-1].CC_Bright > 2)   Group_Command[Group-1].CC_Bright--;
                Serial_Send_PWM_Bright((100+Group),Group_Command[Group-1].CW_Bright,Group_Command[Group-1].CC_Bright);
              }
            }
          }
          else                              //key hold
          {
            if (Group == 0)                   //global command
            {
              for (i=0;i<4;i++)
              {
                if (Group_Command[i].Light_Enabled == TRUE)
                {
                  if (Group_Command[i].CW_Bright > (2+RF_MILIGHT_HOLD_STEP))   Group_Command[i].CW_Bright -= RF_MILIGHT_HOLD_STEP;
                  if (Group_Command[i].CC_Bright > (2+RF_MILIGHT_HOLD_STEP))   Group_Command[i].CC_Bright -= RF_MILIGHT_HOLD_STEP;                  
                  Serial_Send_PWM_Bright((101+i),Group_Command[i].CW_Bright,Group_Command[i].CC_Bright); _delay_ms(RS485_MESSAGE_DELAY);
                }
              }
            } else                        // group command
            {
              if (Group_Command[Group-1].Light_Enabled == TRUE)
              {
                if (Group_Command[Group-1].CW_Bright > (2+RF_MILIGHT_HOLD_STEP))  Group_Command[Group-1].CW_Bright -= RF_MILIGHT_HOLD_STEP;
                if (Group_Command[Group-1].CC_Bright > (2+RF_MILIGHT_HOLD_STEP))  Group_Command[Group-1].CC_Bright -= RF_MILIGHT_HOLD_STEP;
                Serial_Send_PWM_Bright((100+Group),Group_Command[Group-1].CW_Bright,Group_Command[Group-1].CC_Bright);

              }
            }       //End of single group
          }         // End of Keyhold
          Save_Targets();
        }           //end of command

        ////////////////////////////////////////
        ///// RIGHT KEY ///////////////////////
        ///////////////////////////////////////
        else if (Command == 0x0E)
        {
          RF_RX_MI_FirstPress = FALSE;
          if (!KeyHold)                     // single press
          {
            if (Group == 0)                 //global command
            {
              for (i=0;i<4;i++)
              {
                if (Group_Command[i].Light_Enabled == TRUE)
                {
                  if (Group_Command[i].CW_Bright > 2) Group_Command[i].CW_Bright--;
                  if (Group_Command[i].CC_Bright < Conf_LED_Max_Brightness) Group_Command[i].CC_Bright++;
                  Serial_Send_PWM_Bright((101+i),Group_Command[i].CW_Bright,Group_Command[i].CC_Bright); _delay_ms(RS485_MESSAGE_DELAY);
                }
              }
            } else                          //group command
            {
              if (Group_Command[Group-1].Light_Enabled == TRUE)
              {
                if (Group_Command[Group-1].CW_Bright > 2) Group_Command[Group-1].CW_Bright--;
                if (Group_Command[Group-1].CC_Bright < Conf_LED_Max_Brightness) Group_Command[Group-1].CC_Bright++;
                Serial_Send_PWM_Bright((100+Group),Group_Command[Group-1].CW_Bright,Group_Command[Group-1].CC_Bright);
              }
            }
          }
          else                              //key hold
          {
            if (Group == 0)                   //global command
            {
              for (i=0;i<4;i++)
              {
                if (Group_Command[i].Light_Enabled == TRUE)
                {
                  if (Group_Command[i].CW_Bright > (2+RF_MILIGHT_HOLD_STEP)) Group_Command[i].CW_Bright -= RF_MILIGHT_HOLD_STEP;
                  Group_Command[i].CC_Bright += RF_MILIGHT_HOLD_STEP;
                  if (Group_Command[i].CC_Bright > Conf_LED_Max_Brightness) Group_Command[i].CC_Bright = Conf_LED_Max_Brightness;
                  Serial_Send_PWM_Bright((101+i),Group_Command[i].CW_Bright,Group_Command[i].CC_Bright); _delay_ms(RS485_MESSAGE_DELAY);

                }
              }
            } else                        // group command
            {
              if (Group_Command[Group-1].Light_Enabled == TRUE)
              {
                if (Group_Command[Group-1].CW_Bright > (2+RF_MILIGHT_HOLD_STEP))  Group_Command[Group-1].CW_Bright -= RF_MILIGHT_HOLD_STEP;
                Group_Command[Group-1].CC_Bright += RF_MILIGHT_HOLD_STEP;
                if (Group_Command[Group-1].CC_Bright > Conf_LED_Max_Brightness) Group_Command[Group-1].CC_Bright = Conf_LED_Max_Brightness;
                Serial_Send_PWM_Bright((100+Group),Group_Command[Group-1].CW_Bright,Group_Command[Group-1].CC_Bright);
              }
            }       //End of single group
          }         // End of Keyhold
          for (i=0;i<4;i++)                 //limit maximum
          {
            if (Group_Command[i].CW_Bright > Conf_LED_Max_Brightness) Group_Command[i].CW_Bright = Conf_LED_Max_Brightness;
            if (Group_Command[i].CC_Bright > Conf_LED_Max_Brightness) Group_Command[i].CC_Bright = Conf_LED_Max_Brightness;

          }
          Save_Targets();
        }           //end of command

        ////////////////////////////////////////
        ///// LEFT KEY ///////////////////////
        ///////////////////////////////////////
        
        else if (Command == 0x0F)
        {
          RF_RX_MI_FirstPress = FALSE;
          if (!KeyHold)                     // single press
          {
            if (Group == 0)                 //global command
            {
              for (i=0;i<4;i++)
              {
                if (Group_Command[i].Light_Enabled == TRUE)
                {
                  Group_Command[i].CW_Bright++;
                  if (Group_Command[i].CW_Bright > Conf_LED_Max_Brightness) Group_Command[i].CW_Bright = Conf_LED_Max_Brightness;
                  if (Group_Command[i].CC_Bright > 2) Group_Command[i].CC_Bright--;
                  Serial_Send_PWM_Bright((101+i),Group_Command[i].CW_Bright,Group_Command[i].CC_Bright); _delay_ms(RS485_MESSAGE_DELAY);
                }
              }
            } else                          //group command
            {
              if (Group_Command[Group-1].Light_Enabled == TRUE)
              {
                Group_Command[Group-1].CW_Bright++;
                if (Group_Command[Group-1].CW_Bright > Conf_LED_Max_Brightness) Group_Command[Group-1].CW_Bright = Conf_LED_Max_Brightness;
                if (Group_Command[Group-1].CW_Bright > 2) Group_Command[Group-1].CC_Bright--;
                Serial_Send_PWM_Bright((100+Group),Group_Command[Group-1].CW_Bright,Group_Command[Group-1].CC_Bright);
              }
            }
          }
          else                              //key hold
          {
            if (Group == 0)                   //global command
            {
              for (i=0;i<4;i++)
              {
                if (Group_Command[i].Light_Enabled == TRUE)
                {
                  Group_Command[i].CW_Bright += RF_MILIGHT_HOLD_STEP;
                  if (Group_Command[i].CW_Bright > Conf_LED_Max_Brightness) Group_Command[i].CW_Bright = Conf_LED_Max_Brightness;
                  if (Group_Command[i].CC_Bright > (2+RF_MILIGHT_HOLD_STEP))  Group_Command[i].CC_Bright -= RF_MILIGHT_HOLD_STEP;
                  Serial_Send_PWM_Bright((101+i),Group_Command[i].CW_Bright,Group_Command[i].CC_Bright); _delay_ms(RS485_MESSAGE_DELAY);
                }
              }
            } else                        // group command
            {
              if (Group_Command[Group-1].Light_Enabled == TRUE)
              {
                Group_Command[Group-1].CW_Bright += RF_MILIGHT_HOLD_STEP;
                if (Group_Command[Group-1].CW_Bright > Conf_LED_Max_Brightness) Group_Command[Group-1].CW_Bright = Conf_LED_Max_Brightness;
                if (Group_Command[Group-1].CC_Bright > (2+RF_MILIGHT_HOLD_STEP))  Group_Command[Group-1].CC_Bright -= RF_MILIGHT_HOLD_STEP;
                Serial_Send_PWM_Bright((100+Group),Group_Command[Group-1].CW_Bright,Group_Command[Group-1].CC_Bright);
              }
            }       //End of single group
          }         // End of Keyhold
          for (i=0;i<4;i++)                 //limit maximum
          {
            if (Group_Command[i].CW_Bright > Conf_LED_Max_Brightness) Group_Command[i].CW_Bright = Conf_LED_Max_Brightness;
            if (Group_Command[i].CC_Bright > Conf_LED_Max_Brightness) Group_Command[i].CC_Bright = Conf_LED_Max_Brightness;
          }
          Save_Targets();
        }           //end of command
        
      } // End of valid address
    }   // End of new MI command
  }     //End of valid MI command
  else Valid = FALSE;
  return Valid;
}

/// Parse RF Commands from Local System
void RF_RX_Command(uint8_t *RX_Buffer, int8_t Length)
{
  uint8_t Item;
  uint8_t Size;
  uint8_t send485[16],i;

    if ((RX_Buffer[0] == RF_NODEID) || (RX_Buffer[0] == 255))
    {
        if (RX_Buffer[1] == RF_Command_LastSeq) return;
        RF_Command_LastSeq = RX_Buffer[1];
        Item =  RX_Buffer[3];
        Size =  RX_Buffer[2]%16;
        switch (Item)
        {
        case 1:
          Main_Config = RX_Buffer[4] * 256 + RX_Buffer[5];
          //RFChannel = RX_Buffer[6];
          RF_SetNodeID(RX_Buffer[7]);
          EE_Store_Config();
          break;
        //case 10:
        //  RF_PeriodicTime = RX_Buffer[4] * 1000;
        //  EE_Store_Config();
        //  break;
        case 11:          //set max brightness
          Conf_LED_Max_Brightness = RX_Buffer[4];
          EE_Store_Config();
          Serial_Send_PWM_MaxBright(255,RX_Buffer[4]);
          break;
        case 12:
          Conf_LED_NightMode_Brightness = RX_Buffer[4];
          EE_Store_Config();
          Serial_Send_PWM_NightBright(255,RX_Buffer[4]);
          break;
        case 13:
          Conf_LED_TurnOff_Delay_S = RX_Buffer[4] * 10;
          EE_Store_Config();
          Serial_Send_PWM_OnTime(255,RX_Buffer[4]);
          break;
        case 14:          //Fade Time
          Conf_LED_Fade_Delay = RX_Buffer[4];
          EE_Store_Config();
          Serial_Send_PWM_FadeTime(255,RX_Buffer[4]);
          break;
        case 15:          //Warm light
          Serial_Send_PWM_Bright(255,RX_Buffer[4],1);
          // TODO same to internal enabled channels.
          break;
        case 16:          //Cold light
          Serial_Send_PWM_Bright(255,1,RX_Buffer[4]);
          break;
        case 17:          //Neutral light
          Serial_Send_PWM_Bright(255,RX_Buffer[4],RX_Buffer[4]);
          break;
        case 18:          //Individual lamp control by ID
          Serial_Send_PWM_Bright(RX_Buffer[4],RX_Buffer[5],RX_Buffer[6]);
          break;
        case 19:          //External motion detection event
          Serial_Send_PWM_MotionDet(255);
          break;
        case 101:
          Serial_Send_Shutter_Start(RX_Buffer[4]);
          break;
        case 102:
          Serial_Send_Shutter_GoUp(RX_Buffer[4]);
          break;
        case 103:
          Serial_Send_Shutter_GoDown(RX_Buffer[4]);
          break;
        case 104:
          Serial_Send_Shutter_Position(RX_Buffer[4],RX_Buffer[5]);
          break;
        case 250:
          for (i = 0; i < Size; i++)
          {
            send485[i] = RX_Buffer[4 + i];
          }
          Serial_Send_Raw(Size, send485, RS485_CHAR_DELAY);
          break;
        default:
          break;
        }
    }
}
