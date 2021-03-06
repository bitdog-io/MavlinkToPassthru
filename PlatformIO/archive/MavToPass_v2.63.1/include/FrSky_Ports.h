//=================================================================================================  
//================================================================================================= 
//
//                                    F . P O R T   C L A S S
//
//================================================================================================= 
//=================================================================================================

#if defined frBuiltin   // FrSky port support

   #define fpStatusLed    99        // not used in this application
   #define InvertfpLed    false
    


  //    forward declaration of main and utility functions

  uint32_t  Abs(int32_t);
  float     RadToDeg (float );
  uint16_t  prep_number(int32_t, uint8_t, uint8_t);
  uint32_t  bit32Extract(uint32_t,uint8_t, uint8_t); 
  void      bit32Pack(uint32_t, uint8_t, uint8_t);
  uint32_t  createMask(uint8_t, uint8_t);
  int16_t   Add360(int16_t, int16_t);
  float     wrap_360(int16_t);
  int8_t    PWM_To_63(uint16_t);
  void      nbdelay(uint32_t);
  void      DisplayPrintln(String);
  void      Printbyte(byte, bool);
  void      PrintbyteNon(byte);
  void      PrintbyteOut(byte);
  void      PrintbyteIn(byte);
  void      PrintMavLiteUplink();
  void      PrintFrPeriod(bool);
  void      PrintLoopPeriod();
  uint8_t   PX4FlightModeNum(uint8_t, uint8_t);
  void      Mavlink_Param_Request_Read(int16_t, char *);
  void      Mavlink_Param_Set();
  void      Mavlink_Command_Long();
  void      SP_Byte_To_SD(byte);
  String    MavSeverity(uint8_t);



  class     FrSkyPort 
  { 
    // Data members 
  public: 
    
  private:
    static const uint8_t timeout_secs = 5;
    byte      chr = 0;
    
    bool      serGood = false;
    bool      frGood = false;
    bool      frPrev = false; 
    bool      pwmGood = false; 
    bool      pwmPrev = false;
    uint32_t  frGood_millis = 0;   
    uint32_t  serGood_millis = 0;  
    uint32_t  pwmGood_millis = 0;       

    bool      parseGood = false;
    uint32_t  goodFrames = 0;
    uint32_t  badFrames = 0;
    uint32_t  packetloss_millis = 0;
    uint8_t   fpLedState = LOW; 
    uint32_t  fpLed_millis = 0;

             
    #define   frameSize  32
    byte      fpbuf[frameSize];

    static const uint8_t max_ch = 18;         // 16 + 2 digi ch
    int16_t   pwm_ch[max_ch];                 // PWM Channels
    uint8_t   pwm_rssi = 0;
  
    int16_t   crcin = 0;                      // CRC of inbound frsky frame   
    int16_t   crcout = 0;   
    uint16_t  lth = 0;

    uint8_t   fp_lth = 0;
    uint8_t   fp_type = 0;

    volatile uint8_t *uartC3;
    
    enum PortMode { rx , tx };
    PortMode mode, modeNow;

    bool      printcrcout = false;
    byte      nb, lb;                      // Nextbyte, Lastbyt of ByteStuff pair     
  
    uint8_t   time_slot_max = 16;              
    uint32_t  time_slot = 1;
    float     a, az, c, dis, dLat, dLon;
    uint8_t   sv_count = 0;

    uint8_t prevbyt=0;

    bool       frInvert = false;
    uint32_t   frBaud = 0;

    static const uint16_t  fr_max = 32;           // frame buffer
    byte            frbuf[fr_max];
    uint16_t        fr_idx = 0;
    uint8_t         p_fr = 0;   
    
    //=================================================================================
    //                      S  C  H  E  D  U  L  E  R

    // Give the F.Port table more space when status_text messages sent three times
    #if defined Send_status_Text_3_Times
      static const uint16_t sb_rows = 256;  // possible unsent sensor ids at any moment 
    #else 
     static const uint16_t sb_rows = 128;  
    #endif 
    uint16_t        sb_row = 0;
  
    typedef union {
    struct {
       uint32_t       passthru;       // 4B  passthru payload
       uint16_t       packing;        // 2B   
         };
       mtf_payload_t  mavlite;        // 6B  mavlite payload
      }
      sb_payload_t;                          

    // Scheduler identity for both Mavlink and MavLite
    typedef struct  {
      uint16_t      msg_id;          // 2B - note Mavlink needs 2 byte msg_id
      uint8_t       sub_id;          // 1B
      uint32_t      millis;          // 4B - mS since boot
      uint8_t       mt_idx;          // 1B - index if it is a MavLite frame class
      uint8_t       inuse;           // 1B
      sb_payload_t  payload;         // 6B
    } sb_t;

    sb_t          sb[sb_rows];                    // Scheduler buffer table


    uint32_t        sb_buf_full_gear = 0;         // downlink
    uint32_t        mt_buf_full_gear = 0;        // downlink
    uint32_t        mt20_buf_full_gear = 0;      // uplink

    uint32_t        sr_pt_payload;                // 4B
    uint16_t        sb_unsent;                    // how many sb rows in use  
    uint16_t        mt_unsent;                    // how many mt rows in use  

    // Member function declarations
  public:    
    void          initialise();
    void          HandleTraffic();
    void          PushMessage(uint16_t msg_id, uint8_t sub_id); 
    void          ReportFrPortOnlineStatus();      
    uint16_t      MatchWaitingParamRequests(char * paramid);   
            
  private:  
    void          FPort_Read_And_Write(uint8_t *buf);  
    void          SPort_Read_And_Write(uint8_t *buf);       
    
    void          setMode(PortMode mode);
    byte          ReadByte();
    byte          SafeRead();
    void          WriteByte(byte b);   
    void          SafeWrite(byte b, bool isPayload);
    void          ReportOnlineStatus(); 
    bool          ParseFrame(uint8_t *buf, uint16_t lth);
    void          Uplink_Frame();
    uint8_t       crcGet(uint8_t *buf, uint8_t lth);
    bool          crcGood(uint8_t *buf, uint8_t lth);                
    bool          BytesToPWM(uint8_t *buf, int16_t *ch);
    void          crcStepOut(uint8_t b);  
    void          crcStep(int16_t *mycrc, uint8_t b); 
    void          crcEnd(int16_t *mycrc);          
    void          WriteCrc(); 
    void          Print_PWM_Channels(int16_t *ch, uint16_t max_ch); 
    void          PrintBuffer(uint8_t *buf, uint8_t lth);   
    void          PrintFPortBuffer(uint16_t lth);
    void          CheckForTimeout();   
    void          ServiceStatusLed();
    void          BlinkFpLed(uint32_t period);

    void          InjectFrame();  
    uint16_t      PopNextFrame();  
    msg_class_t   msg_class_now(uint8_t msg_id);
    void          Send_Frsky_Frame(uint16_t idx);
    bool          DecodeAndDownlinkMavLite();  
    void          ChunkMavLitePayload(uint8_t msg_id, uint8_t tlth);        // downlink 
    bool          UnchunkMavLitePayload();                  // uplink 
    void          SendPassthruPacket(uint8_t sensor_id, uint16_t msg_id);
    void          SendMavlitePacket(uint8_t sensor_id, uint16_t msg_id);
    uint16_t      FirstEmptyMt20Row();
    void          PushToEmptyRow(uint16_t msg_id, uint8_t sub_id); 
    void          Push_Param_Val_016(uint16_t msg_id);   
    void          Push_Lat_800(uint16_t msg_id);  
    void          Push_Lon_800(uint16_t msg_id);
    void          Push_Text_Chunks_5000(uint16_t msg_id);
    void          Push_AP_status_5001(uint16_t msg_id);  
    void          Push_GPS_status_5002(uint16_t msg_id);
    void          Push_Bat1_5003(uint16_t msg_id);
    void          Push_Home_5004(uint16_t msg_id);
    void          Push_VelYaw_5005(uint16_t msg_id);
    void          Push_Atti_5006(uint16_t msg_id);   
    void          Push_Parameters_5007(uint16_t msg_id, uint8_t sub_id);
    void          Push_Bat2_5008(uint16_t msg_id);  
    void          Push_WayPoint_5009(uint16_t msg_id);
    void          Push_Servo_Raw_50F1(uint16_t msg_id);    
    void          Push_VFR_Hud_50F2(uint16_t msg_id);    
    void          Push_Wind_Estimate_50F3(uint16_t msg_id);
    void          Push_Rssi_F101(uint16_t msg_id);                                                            
    void          PrintPayload(msg_class_t msg_class); 
    void          PrintMavLiteUplink();
    void          Printbyte(byte b, char delimiter);    

      
         
}; // end of class

    // External member functions
    //===================================================================

    void FrSkyPort::initialise()  {  

    for (int i=0 ; i < sb_rows ; i++) {  // initialise F.Port table
        sb[i].msg_id = 0;
        sb[i].sub_id = 0;
        sb[i].millis = 0;
        sb[i].inuse = 0;
    }
    
    for (int i = 0 ; i < mt20_rows ; i++) {  // initialise MavLite uplink table
      mt20[i].inuse = 0;
    }     
    
    if (set.frport == f_port) {
      frBaud = 115200; 
      frInvert = false;   // F.Port is normally idle high (not inverted)
      Debug.printf("F.Port at %d baud is NOT INVERTED", frBaud);     
    } else 
      if (set.frport == s_port) {
        frBaud = 57600;
        #if defined Uninverted_FrSky_Receiver_Pad // note S.Port is normally idle low (inverted)
          frInvert = false;  
          Debug.printf("S.Port at %d baud is NOT INVERTED", frBaud); 
        #else
          frInvert = true;  
          Debug.printf("S.Port at %d baud is INVERTED", frBaud);  
        #endif  
      }
    
    #if (defined ESP32) || (defined ESP8266) // ESP only
      int8_t frRx;
      int8_t frTx;

      frRx = fr_rxPin;
      frTx = fr_txPin;

      #if ( (defined ESP8266) || ( (defined ESP32) && (defined ESP32_SoftwareSerial)) )
  
          if (set.trmode == ground) {
            Debug.printf(" and is 1-wire simplex on tx pin = %d\n", frTx);
          } else { 
            Debug.printf(" on pins rx = %d and tx = %d\n", frRx, frTx);
          }  

          nbdelay(100);
          frSerial.begin(frBaud, SWSERIAL_8N1, frRx, frTx, frInvert);     // SoftwareSerial
          nbdelay(100);
          Debug.println("Using SoftwareSerial for F.Port");

      #else  // HardwareSerial
          frSerial.begin(frBaud, SERIAL_8N1, frRx, frTx, frInvert); 
          if (set.trmode == ground)  {                        
            Debug.printf("and is 1-wire simplex on tx pin = %d\n", frTx);
          } else  
          if ((set.trmode == air) || (set.trmode == relay)) {
            Debug.printf(" on pins rx = %d and tx = %d\n", frRx, frTx);  
            Debug.println("A 2-wire to 1-wire converter or diode is required");
          }    
      #endif
    #endif
    
    #if (defined TEENSY3X) 
      frSerial.begin(frBaud); // Teensy 3.x    tx pin hard wired
     #if (frPort_Serial == 1)
      // Manipulate UART registers for F.Port working
       uartC3   = &UART0_C3;  // UART0 is Serial1
       if (frInvert) {        // For S.Port not F.Port
         UART0_C3 = 0x10;       // Invert Serial1 Tx levels
         UART0_S2 = 0x10;       // Invert Serial1 Rx levels;       
       }
       #if (defined Teensy_One_Wire)  // default
         UART0_C1 = 0xA0;       // Switch Serial1 to single wire (half-duplex) mode  
         Debug.printf(" and 1-wire half-duplex on Teensy3.x pin %d \n", fr_txPin);          
       #else
         Debug.printf(" and 2-wire full-duplex on Teensy3.x pins rx=%d  tx=%x\n", fr_rxPin, fr_txPin);  
       #endif
        /*
        UART0_C3 |= 0x20;    // Switch F.Port into send mode
        UART0_C3 ^= 0x20;    // Switch F.Port into receive mode
        */
        
     #else
       uartC3   = &UART2_C3;  // UART2 is Serial3
       if (frInvert) {      
         UART2_C3 = 0x10;       // Invert Serial1 Tx levels
         UART2_S2 = 0x10;       // Invert Serial1 Rx levels;  
       }
       #if (defined Teensy_One_Wire)
         UART2_C1 = 0xA0;       // Switch Serial1 to single wire mode
         Debug.printf(" and 1-wire half-duplex on Teensy3.x pin %d \n", fr_txPin);         
       #else
         Debug.printf(" and 2-wire full-duplex on Teensy3.x pins rx=%d  tx=%x\n", fr_rxPin, fr_txPin);  
       #endif
          

     #endif
      
    #endif 
      
    } // end of member function
    

    //===================================================================
    void FrSkyPort::HandleTraffic() {  

      if (set.trmode == ground) {   // no fr port reading necessary
        if(mavGood  && ((millis() - blind_inject_millis) > 18)) {  
          FrSkyPort::InjectFrame();                    // Blind inject frame into Taranis et al 
          blind_inject_millis=millis();
        }
        return; // go  around
      }  

      FrSkyPort::CheckForTimeout();

      if (set.frport == f_port) {
   
        #if defined TEENSY3X 
          if(mavGood && ((millis() - sp_millis) > 1)) {   // very necessary for Teensy 3.x
            sp_millis=millis();
            FPort_Read_And_Write(&FrSkyPort::frbuf[0]);
          }
          
        #else   // ESP32 and ESP8266
          FPort_Read_And_Write(&FrSkyPort::frbuf[0]);
        #endif   

      } else  
      if (set.frport == s_port) {  
        if(mavGood && ((millis() - sp_millis) > 2)) {   // very necessary for S.Port !! especially for Teensy 3.x
          sp_millis=millis();
          SPort_Read_And_Write(&FrSkyPort::frbuf[0]);
        }
      }  
    }   
    //===================================================================
    
    void FrSkyPort::FPort_Read_And_Write(uint8_t *buf) {
      FrSkyPort::setMode(rx);
      lth=frSerial.available();  
      if (lth < 10) {
        if (lth > 0) {
          //Debug.printf("lth=%d\n", lth); 
        }
        return;       // prevent 'wait-on-read' blocking
      }
          
      #if defined Report_Packetloss
        uint32_t period = (Report_Packetloss * 60000);
        if (millis() - FrSkyPort::packetloss_millis > period) {
          FrSkyPort::packetloss_millis = millis();
          float packetloss = float(float(FrSkyPort::badFrames * 100) / float(FrSkyPort::goodFrames + FrSkyPort::badFrames));
          Debug.printf("goodFrames:%d   badFrames:%d   frame loss:%.3f%%\n", FrSkyPort::goodFrames, FrSkyPort::badFrames, packetloss);
        }
      #endif
  
      #if defined Debug_FrPort_Serial_Loop
        while(true) {            // loop here forever - debugging only
          FrSkyPort::chr = FrSkyPort::ReadByte();  
        }
      #endif  
      
      while (!(FrSkyPort::chr==0x7E)) {     // find first 0x7E, should be start, but could be previous stop
        FrSkyPort::chr = FrSkyPort::ReadByte();
      }

      FrSkyPort::fpbuf[0] = FrSkyPort::chr;
      FrSkyPort::chr = FrSkyPort::ReadByte();      // could be start 0x7E or len 0x19 or 0x08

      while (FrSkyPort::chr == 0x7E) {         // if start 0x7E, then the first one was a stop so ignore it
       
        *buf = FrSkyPort::chr;
        FrSkyPort::chr = FrSkyPort::ReadByte();
      }

     // Debug.printf("\nStart char=%X\n", FrSkyPort::fpbuf[0]); 
  
      *(buf+1) = FrSkyPort::chr;                   // should be len 0x19 or 0x08
  
      *(buf+2) = FrSkyPort::ReadByte();             // frame_type  
  
      fp_lth = *(buf+1);
      fp_type = *(buf+2);

      if ((fp_lth == 25) || (fp_lth == 8)) {
        FrSkyPort::frGood = true;            
        FrSkyPort::frGood_millis = millis(); 
        inject_delay = micros();        
        switch(fp_type){

          case 0x00:      // Control Frame (RC Channels)
            FrSkyPort::parseGood = FrSkyPort::ParseFrame(buf, fp_lth); 
            if (FrSkyPort::parseGood) {
              #if defined Derive_PWM
                FrSkyPort::pwmGood = FrSkyPort::BytesToPWM(buf+3, &FrSkyPort::pwm_ch[0]);
                if (FrSkyPort::pwmGood) {
                  FrSkyPort::pwmGood_millis = millis();
                }
              #endif  

              #if defined Support_SBUS_Out 
              
              #endif    
            }  
            break;
        
          case 0x01:      // Downlink Master (their) Frame                  
            FrSkyPort::parseGood = FrSkyPort::ParseFrame(buf, fp_lth); 
            if (FrSkyPort::parseGood) {    
              //byte fp_prim = fpbuf[3];                 // for info - prim should be 0x10           
              FrSkyPort::InjectFrame();                  // Inject our uplink slave packet right now! (<=3mS) 
            }
            break;
        
          case 0x81:      // Uplink Slave Frame
            FrSkyPort::parseGood = FrSkyPort::ParseFrame(buf, fp_lth); 
            if (FrSkyPort::parseGood) {
           
              // do something
            }      
            break;  
        
          default: 
         //   Debug.printf("Unknown frame type = %X\n", fp_type);  
            break;  
            
         }
      } else {
       // Debug.printf("Bad FPort frame length = %X\n", fp_lth);  
      }
       
      FrSkyPort::chr = FrSkyPort::ReadByte(); // should be stop 0x7E  
     //  Debug.printf("\nStop char=%X\n", FrSkyPort::chr); 
    } 
    
    //===================================================================
    
    void FrSkyPort::SPort_Read_And_Write(uint8_t *buf) {
      
      FrSkyPort::setMode(rx);  

      uint8_t byt = 0;

      while (frSerial.available())   {  // Receive sensor IDs from X receiver

        byt =  FrSkyPort::ReadByte();        // no bytestuff
        if (fr_idx > fr_max -1) {
         // Debug.println("S.Port from X Receiver read buff overflow ignored");
          fr_idx--;
        }

        if ((prevbyt == 0x7E) && (byt == 0x1B)) {   // Real-Time DIY downlink slot found, slot ours in, and send right now 
           FrSkyPort::frGood = true;            
           FrSkyPort::frGood_millis = millis();         
           inject_delay = micros();       
           FrSkyPort::InjectFrame();                  // Interleave a packet right now!    
           return;  
        }

        if (byt == 0x7E) {
          if ((*(buf+1) == 0x0D) && (*(buf+2) == 0x30)) { // MavLite downlink match found

            DecodeAndDownlinkMavLite();        

          }      
          fr_idx = 0;
        }   
        *(buf + fr_idx) = byt;
        fr_idx++;
        prevbyt=byt;
      } 
      
    } 
    
    //===================================================================   
 
    bool FrSkyPort::ParseFrame(uint8_t *buf, uint16_t lth) {
      int i = 0;
      for (i = 3 ; i < lth+2 ; i++) {      // payload after start byte[0], lth[1] and type[2], 
        chr = FrSkyPort::SafeRead(); 
        *(buf+i) = chr;
      }
      
       chr = FrSkyPort::SafeRead();           // this is the crc byte
       *(buf+i) = chr;

       // FrSkyPort::crcEnd(&crcin);        
       
      #if (defined Debug_FPort_Buffer) 
        FrSkyPort::PrintBuffer(buf, lth);
      #endif  

      bool mycrcGood =  FrSkyPort::crcGood(buf+1, lth+1);  // include the lth byte but obviously not the crc byte
      if (mycrcGood) {
        FrSkyPort::goodFrames++;
      } else {
        badFrames++;
      }
      #if defined Debug_CRC
        Debug.printf("mycrcGood=%d\n\n", mycrcGood);  
      #endif  
      return mycrcGood;   
   
    }     
    //===================================================================
    
    void FrSkyPort::Uplink_Frame() {

    }   
    //===================================================================
      
    void FrSkyPort::setMode(PortMode mode) {   
    
    #if (defined TEENSY3X) 
      if(mode == tx && modeNow !=tx) {
        *uartC3 |= 0x20;                 // Switch FrPort into send mode
        modeNow=mode;
        #if defined Debug_FrPort_Switching
          Debug.print("tx");
        #endif
      }
      else if(mode == rx && modeNow != rx) {   
        *uartC3 ^= 0x20;                 // Switch FrPort into receive mode
        modeNow=mode;
        #if defined Debug_FrPort_Switching
          Debug.print("rx");
        #endif
      }
    #endif

/*
    #if (defined ESP8266) || (defined ESP32) 
        if(mode == tx && modeNow !=tx) { 
          modeNow=mode;
          #if (defined ESP_Onewire) && (defined ESP32_SoftwareSerial)        
          frSerial.enableTx(true);  // Switch F.Port into send mode
          #endif
          #if defined Debug_FrPort_Switching
            Debug.print("tx");
          #endif
        }   else 
        if(mode == rx && modeNow != rx) {   
          modeNow=mode; 
          #if (defined ESP_Onewire) && (defined ESP32_SoftwareSerial)                  
          frSerial.enableTx(false);  // disable interrupts on tx pin     
          #endif
          #if defined Debug_FrPort_Switching
            Debug.print("rx");
          #endif
        } 
    #endif
*/
    
    }  // end of member function    
    
    //===================================================================

    byte FrSkyPort::ReadByte() {
    byte b;
      FrSkyPort::setMode(rx);
      if (lth == 0) {
        while (lth==0) {
          FrSkyPort::CheckForTimeout();
          lth=frSerial.available();
        }
     //    Debug.printf("\nlen=%3d\n",len); 
      } 

      // Data is available
      FrSkyPort::serGood = true;            // We have a good serial connection!
      FrSkyPort::serGood_millis = millis();
      
      b = frSerial.read();
      lth--;
      
      #if (defined Debug_FrPort_Stream)  
        FrSkyPort::Printbyte(b, '<');
      #endif 
      delay(0); // yield to rtos for wifi & bt to get a sniff      
      return b;
    }

    //===================================================================

    byte FrSkyPort::SafeRead() {
      byte b;  

      FrSkyPort::setMode(rx);

      b = FrSkyPort::ReadByte();     
      
      //  if 0x7D is received it should be omitted, and the next byte should 
      //  be XOR or ADD with 0x20
      //  0x5D => 0x7D, 0x5E => 0x7E
    
      if ((b == 0x7D) || (b == 0x7E)){
        b = FrSkyPort::ReadByte();
        b ^= 0x20;
      }
      #if (defined Debug_FrPort_Stream)  
        FrSkyPort::Printbyte(b, '|');
      #endif 
      delay(0); // yield to rtos for wifi & bt to get a sniff 
      return b;
    } // end of member function
    //===================================================================

    void FrSkyPort::SafeWrite(byte b, bool isPayload) {
    #if (not defined inhibit_FPort)
      FrSkyPort::setMode(tx);

      //  B Y T E   S T U F F   
      //  Byte in frame has value 0x7E is changed into 2 bytes: 0x7D 0x5E
      //  Byte in frame has value 0x7D is changed into 2 bytes: 0x7D 0x5D
      if (isPayload) {
        if (b == 0x7E) {
          FrSkyPort::WriteByte(0x7D);           
          FrSkyPort::WriteByte(0x5E);    
        } else if (b == 0x7D) {
          FrSkyPort::WriteByte(0x7D);                   
          FrSkyPort::WriteByte(0x5D);          
        } else {   
        FrSkyPort::WriteByte(b);       
        }
      } else {
        FrSkyPort::WriteByte(b);         
      }
     if (isPayload) {  // Add crcout
       FrSkyPort::crcStepOut(b);
     }
      delay(0); // yield to rtos for wifi & bt to get a sniff    
    #endif      
    }
    //===========================
    void FrSkyPort::WriteByte(byte b) {
      frSerial.write(b); 
      if  (set.sport_sd == spsd_on) SP_Byte_To_SD(b);          // optionally to SD
      #if (defined Debug_FrPort_Stream) 
        FrSkyPort::Printbyte(b, '>');
      #endif         
    }

    //===================================================================

    void FrSkyPort::crcStepOut(uint8_t b) {
       crcout += b;          // add in new byte
       crcout += crcout >> 8;   // add in high byte overflow if any
       crcout &= 0xff;  // mask all but low byte, constrain to 8 bits
       if (printcrcout) Debug.printf("AddIn %3d %2X\tcrcout_now=%3d %2X\n", b, b, crcout, crcout);
    }   
    //=======================================================================  
    
    void FrSkyPort::crcStep(int16_t *mycrc, uint8_t b) {
       *mycrc += b;             // add in new byte
       *mycrc += *mycrc >> 8;   // add in high byte carry if any
       *mycrc &= 0xff;          // mask all but low byte, constrain to 8 bits

      #if defined Debug_CRC
         Debug.printf("CRC Step: b=%3X %3d\  crc=%3X %3d\n", b, b, *mycrc, *mycrc);
       #endif
    }
    //=========================================================================
    
    void FrSkyPort::crcEnd(int16_t *mycrc)  {
      *mycrc = 0xFF - *mycrc;                  // final 2s complement
      #if defined Debug_CRC
        Debug.printf("crcEnd=%3X %3d\n", *mycrc, *mycrc );
      #endif  
    } 
    //=======================================================================   
       
    uint8_t FrSkyPort::crcGet(uint8_t *buf, uint8_t lth)  {

      int16_t mycrc = 0;
      for (int i = 0; i < lth; i++) {
        crcStep(&mycrc, *buf++);
      }
      FrSkyPort::crcEnd(&mycrc);
      return mycrc;
    }
    //=======================================================================  
    bool FrSkyPort::crcGood(uint8_t *buf, uint8_t lth)  {
      
      uint8_t mycrc = FrSkyPort::crcGet(buf, lth);   
      uint8_t fpcrc = *(buf+lth);
      #if defined Debug_CRC    
        Debug.printf("mycrc=%3X %3d  fpcrc=%3X\ %3d\n", mycrc, mycrc, fpcrc, fpcrc );
      #endif
    return (mycrc == fpcrc);

   }  
     //=======================================================================    
         
    bool FrSkyPort::BytesToPWM(uint8_t *buf, int16_t *ch) {  // PWM Channels

      //uint8_t fpcrc = *(buf+lth);  // may be useful

     *ch  = ((*buf|*(buf+1)<< 8) & 0x07FF);
     *(ch+1)  = ((*(buf+1)>>3|*(buf+2)<<5) & 0x07FF);
     *(ch+2)  = ((*(buf+2)>>6|*(buf+3)<<2|*(buf+4)<<10) & 0x07FF);
     *(ch+3)  = ((*(buf+4)>>1|*(buf+5)<<7) & 0x07FF);
     *(ch+4)  = ((*(buf+5)>>4|*(buf+6)<<4) & 0x07FF);
     *(ch+5)  = ((*(buf+6)>>7|*(buf+7)<<1|*(buf+8)<<9) & 0x07FF);
     *(ch+6)  = ((*(buf+8)>>2|*(buf+9)<<6) & 0x07FF);
     *(ch+7)  = ((*(buf+9)>>5|*(buf+10)<<3) & 0x07FF); 
     *(ch+8)  = ((*(buf+11)|*(buf+12)<< 8) & 0x07FF);
     *(ch+9)  = ((*(buf+12)>>3|*(buf+13)<<5) & 0x07FF);
     *(ch+10) = ((*(buf+13)>>6|*(buf+14)<<2|*(buf+15)<<10) & 0x07FF);
     *(ch+11) = ((*(buf+15)>>1|*(buf+16)<<7) & 0x07FF);
     *(ch+12) = ((*(buf+16)>>4|*(buf+17)<<4) & 0x07FF);
     *(ch+13) = ((*(buf+17)>>7|*(buf+18)<<1|*(buf+19)<<9) & 0x07FF);
     *(ch+14) = ((*(buf+19)>>2|*(buf+20)<<6) & 0x07FF);
     *(ch+15) = ((*(buf+20)>>5|*(buf+21)<<3) & 0x07FF);

     // remap values to regular uS range
     for (int i = 0 ; i < max_ch-2 ; i++) {
       if (*(ch+i) > 0) {
         *(ch+i) = map(*(ch+i), 172, 1811, 982, 2006);     
       }
     }

      // flags digi ch 1
      if (*(buf+22) & (1<<0)) {
       ch[16] = 1;
      }
      else{
       ch[16] = 0;
      }
      // flags digi ch 2
      if (*(buf+22) & (1<<1)) {
       ch[17] = 1;
      }
      else{
       ch[17] = 0;
     }

     FrSkyPort::pwm_rssi = *(buf+23);

     #if defined Debug_PWM_Channels 
        FrSkyPort::Print_PWM_Channels(&FrSkyPort::pwm_ch[0], max_ch);
     #endif   

     if (*ch > 0) {
      return true;
     } else {
      return false;
     }
    }  
     
    //===================================================================      
    void  FrSkyPort::Print_PWM_Channels(int16_t *ch, uint16_t max_ch) {
      for (int i = 0 ; i < max_ch ; i++ ) {
        Debug.printf("%2d:%4d  ", i+1, *(ch+i)); 
      }
      Debug.printf("  rssi:%d\n", FrSkyPort::pwm_rssi );
    }
    //===================================================================      

   void FrSkyPort::PrintBuffer(uint8_t *buf, uint8_t length) {
   // Debug.printf("length=%d\t", length);
    for (int i = 0 ; i < length ; i++) {  
      if (*(buf+i) <= 0xf) Debug.print("0");
        Debug.print(*(buf+i),HEX);
        Debug.write(" ");
      }
      Debug.println();
  }
   
    //===================================================================      

    void FrSkyPort::PrintFPortBuffer(uint16_t lth) {
      for (int i = 0 ; i < lth+3 ; i++) {   // start_byte[0] + lth[1] + [ payload (from type[2]) ] + CRC
        if (fpbuf[i] <= 0xf) Debug.print("0");
        Debug.print(fpbuf[i],HEX);
        Debug.write(" ");
      }
      Debug.println();
    }  

    //===================================================================
    void FrSkyPort::CheckForTimeout() {
      
     if ((millis() - serGood_millis) > (FrSkyPort::timeout_secs * 1000)) {
       FrSkyPort::serGood = false;
       FrSkyPort::frGood = false; 
       FrSkyPort::pwmGood = false;           
     }    

     if ((millis() - frGood_millis) > (FrSkyPort::timeout_secs * 1000)) {
       FrSkyPort::frGood = false;
       FrSkyPort::pwmGood = false;  
     }
     
     if ((millis() - pwmGood_millis) > (FrSkyPort::timeout_secs * 1000)) {
       FrSkyPort::pwmGood = false;
     }

     
     FrSkyPort::ServiceStatusLed(); 
     FrSkyPort::ReportOnlineStatus();
    }   
    //===================================================================     

    void FrSkyPort::ServiceStatusLed() {
      if (fpStatusLed == 99) return;
      
      if (FrSkyPort::pwmGood) {
          if (InvertfpLed) {
           FrSkyPort::fpLedState = LOW;
          } else {
           FrSkyPort::fpLedState = HIGH;
          }
          digitalWrite(fpStatusLed, fpLedState); 
      }
        else {
          if (serGood) {
            FrSkyPort::BlinkFpLed(500);
          }
        }
      digitalWrite(fpStatusLed, fpLedState); 
    }
    //===================================================================   
    void FrSkyPort::BlinkFpLed(uint32_t period) {
         if (millis() - FrSkyPort::fpLed_millis >= period) {    // blink period
            FrSkyPort::fpLed_millis = millis();
            if (FrSkyPort::fpLedState == LOW) {
              FrSkyPort::fpLedState = HIGH; }   
            else {
              FrSkyPort::fpLedState = LOW;  } 
          }
    }  
    //===================================================================  
    
    void FrSkyPort::ReportOnlineStatus() {
  
       if (FrSkyPort::frGood != FrSkyPort::frPrev) {  // report on change of status
         FrSkyPort::frPrev = FrSkyPort::frGood;
         if (frGood) {
           Debug.println("FrSport read good!");
           DisplayPrintln("FrSport read ok");         
         } else {
          Debug.println("FrSport read timeout!");
          DisplayPrintln("FrSport timeout");         
         }
       }
       
       if (FrSkyPort::pwmGood != FrSkyPort::pwmPrev) {  // report on change of status
         FrSkyPort::pwmPrev = FrSkyPort::pwmGood;
         if (pwmGood) {
           Debug.println("RC PWM good");
           DisplayPrintln("RC PWM good");         
         } else {
          Debug.println("RC PWM timeout");
          DisplayPrintln("RC PWM timeout");         
         }
       }
    } 
    
    //=================================================================================================
    //=================================================================================================
    //====================================   U P L I N K      =========================================  


    void FrSkyPort::InjectFrame() {  
 
      uint16_t nxt = PopNextFrame();
      

      if (nxt != 0xffff) {   // not empty
        FrSkyPort::Send_Frsky_Frame(nxt);  
      }
     }
    
    //===================================================================  
 
    uint16_t FrSkyPort::PopNextFrame() {

      uint32_t sb_now = millis();
      int16_t sb_age;
      int16_t sb_tier_age;
      int16_t sb_oldest_tier1 = 0; 
      int16_t sb_oldest_tier2 = 0; 
      int16_t sb_oldest       = 0;     
      uint16_t idx_tier1      = 0;                 // row with oldest sensor data
      uint16_t idx_tier2      = 0; 
      uint16_t idx            = 0; 

      // 2 tier scheduling. Tier 1 gets priority, tier2 (0x5000) only sent when tier 1 empty 
  
      // find the row with oldest sensor data = idx 
      sb_unsent = 0;  // how many rows in-use

      uint16_t i = 0;
      while (i < sb_rows) {  
    
        if (sb[i].inuse) {
          sb_unsent++;   
      
          sb_age = (sb_now - sb[i].millis); 
          sb_tier_age = sb_age - sb[i].sub_id;  

          if (sb[i].msg_id == 0x5000) {
            if (sb_tier_age >= sb_oldest_tier2) {
              sb_oldest_tier2 = sb_tier_age;
              idx_tier2 = i;
            }
          } else {
          if (sb_tier_age >= sb_oldest_tier1) {
            sb_oldest_tier1 = sb_tier_age;
            idx_tier1 = i;
            }   
          }
        } 
      i++;    
      } 
    
      if (sb_oldest_tier1 == 0) {            // if there are no tier 1 sensor entries
        if (sb_oldest_tier2 > 0) {           // but there are tier 2 entries
          idx = idx_tier2;                   // send tier 2 instead
          sb_oldest = sb_oldest_tier2;
        }
      } else {
        idx = idx_tier1;                    // if there are tier1 entries send them
       sb_oldest = sb_oldest_tier1;
      }
  
      //Debug.println(sb_unsent);           // limited detriment :)  

      if (sb_oldest == 0)  return 0xffff;  // flag the scheduler table as empty

      if ((FrSkyPort::msg_class_now(sb[idx].msg_id)) == passthru) {
          pt_payload = sb[idx].payload.passthru; 
      } else
      if ((FrSkyPort::msg_class_now(sb[idx].msg_id)) == mavlite) {
          mtf_payload = sb[idx].payload.mavlite; 
      }

      #if (defined Frs_Debug_Scheduler) || (defined MavLite_Debug_Scheduler)
        uint16_t msgid_filter;
        #if (defined MavLite_Debug_Scheduler)
          msgid_filter = 0x100;
        #else
          msgid_filter = 0xffff; // just high value
        #endif

        if (sb[idx].msg_id < msgid_filter) {
          Debug.print(sb_unsent); 
          Debug.printf("\tPop  row= %3d", idx );
          Debug.print("  msg_id=0x");  Debug.print(sb[idx].msg_id, HEX);
          if (sb[idx].msg_id < 0x1000) Debug.print(" ");
          Debug.printf("  sub_id= %2d", sb[idx].sub_id); 
      
          pb_rx=false;
          FrSkyPort::PrintPayload(FrSkyPort::msg_class_now(sb[idx].msg_id));
          Debug.printf("  age=%3d mS \n" , sb_oldest_tier1 );        
        }
      #endif

      return idx;  // return the index of the oldest frame
     }
    //===================================================================   

    msg_class_t  FrSkyPort::msg_class_now(uint8_t msg_id) {
      if ((msg_id >= 20) && (msg_id <=77)) {
        return mavlite; 
      } else {
        return passthru;
      }
     }
    //===================================================================    
    void FrSkyPort::Send_Frsky_Frame(uint16_t idx) {
      FrSkyPort::setMode(tx); 
      #if defined Frs_Debug_Period
        PrintFrPeriod(0);   
      #endif    
      
      sp_msg_id = sb[idx].msg_id;   // global msg_id for sending debug

      if (sb[idx].msg_id == 0xF101) {
        #if (defined Frs_Debug_Rssi)
          PrintFrPeriod(0);    
          Debug.println(" 0xF101 sent");
        #endif
    
        if (set.trmode != relay) {   
          FrSkyPort::SafeWrite(0x7E, false);      // not payload, don't stuff or add to crcout       
          if (set.frport == f_port) {
            FrSkyPort::SafeWrite(0x08, true);     // lth       
            FrSkyPort::SafeWrite(0x81, true);     // type 
          } else  
          if (set.frport == s_port) {
            FrSkyPort::SafeWrite(0x1B, false);    // our sport diy sensor       
          }   
        }
      }

      #if defined Frs_Debug_Scheduler
        Debug.printf("Injecting frame idx=%d ", idx);
        FrSkyPort::PrintPayload(FrSkyPort::msg_class_now(sb[idx].msg_id)); 
        Debug.println();
      #elif defined Debug_Mavlite_Scheduler
      if (sb[idx].msg_id < 0x100) {  
        Debug.printf("Injecting frame idx=%d ", idx);
        FrSkyPort::PrintPayload(FrSkyPort::msg_class_now(sb[idx].msg_id)); 
        Debug.println();
      }  
      #endif

      msg_class = FrSkyPort::msg_class_now(sb[idx].msg_id);
      if (msg_class == passthru) {
        FrSkyPort::SendPassthruPacket(0x1B, sb[idx].msg_id);   // pt_payload given value in PopNextFrame() above
        pt_payload = 0;                                        // clear the payload field 
      } else
  
      if (msg_class == mavlite) {
        FrSkyPort::SendMavlitePacket(0x1B, sb[idx].msg_id);   // mtf_payload given value in PopNextFrame() above
        mtf_payload = {};                                     // clear payload struct
      }
  
      sb[idx].inuse = 0;                                      // 0=free for use, 1=occupied - for passthru and mavlite
   
     }

    //=========================  D O W N L I N K   T O   F C  ========================
 
    bool FrSkyPort::DecodeAndDownlinkMavLite() {
    #if defined Support_MavLite  
      mt_seq = frbuf[3];
      if (mt_seq == 0) {
        mt_paylth = frbuf[4];
        mt_msg_id = frbuf[5];
        mt_idx = 0;
        }

      if (FrSkyPort::UnchunkMavLitePayload()) {  // when fully expanded
 
        switch (mt_msg_id) {                // Decode uplink mavlite messages according to msg-id
          
          case 20:                          //  #20 or 0x14   Mavlink_Param_Request_Read - 
            {
              for (int i = 0; i <16; i++) {
                ap22_param_id[i] = mt_payload[i];
              }

              #if defined Debug_Mavlite
                Debug.printf("MavLite #20 Param_Request_Read :%s:\n", ap22_param_id);  
              #endif  

              Mavlink_Param_Request_Read(-1, ap22_param_id); // Request Param Read using param_id, not index  

              // store in table of param requests awaiting reply
              mt20_row = FrSkyPort::FirstEmptyMt20Row();
              //   Debug.printf("mt20_row=%d\n", mt20_row);
              if (mt20_row == 0xffff) return false;  // mt20 table overflowed, ignore this message :(
              strncpy(mt20[mt20_row].param_id, ap22_param_id, 16);
              mt20[mt20_row].millis = millis(); 
              mt20[mt20_row].inuse = 1;

              #if (defined Debug_Mavlite_SPort)
                PrintMavLiteUplink();
              #endif                
              return true;           
            }
          case 23:                          // #23 or 0x17  PARAM_SET - Uplink
            {
              // expanded payload to value 
              byte *b = (byte *)&ap23_param_value;  // cast value to bytes
              b[0] = mt_payload[0];
              b[1] = mt_payload[1];
              b[2] = mt_payload[2];
              b[3] = mt_payload[3];

              for (int i = 0; i <16; i++) {
                ap23_param_id[i] = mt_payload[i+4];
              }

              Mavlink_Param_Set();
              
              #if defined Debug_Mavlite
                 Debug.printf("MavLite #23 Param_Set. Parameter-id= %s  Value = %.5f\n", ap23_param_id, ap23_param_value);  
              #endif  
              return true; 
            }            

          case 76:                          // #76 or 0x4c  COMMAND_LONG
            {
              ap76_command = (mt_payload[1] << 8) + mt_payload[0];          
              ap76_confirm = mt_payload[2];
              uint8_t par_cnt = (mt_payload[3] & 0xE0) >> 5;
              uint8_t conf_cnt = mt_payload[3] & 7;   // unused
              float val = 0;
              byte *b = (byte *)&val;  // cast value to bytes
              for ( int i = 1; i <= par_cnt ; i++) {
                uint8_t j = i * 4;
                b[0] = mt_payload[j+0];
                b[1] = mt_payload[j+1];
                b[2] = mt_payload[j+2];
                b[3] = mt_payload[j+3];
                ap76_param[i-1] = val;
              }

              Mavlink_Command_Long();  // #76

              #if defined Debug_Mavlite
               Debug.printf("MavLite #76 Command_Long: Command=%d  confirm=%d param_count=%d ", ap76_command, ap76_confirm, par_cnt);  
               for ( int i = 0; i < par_cnt ; i++) {
                  Debug.printf("param%d=%.5f", i+1,  ap76_param[i]); 
               }
               Debug.println();
              #endif  

              return true;      
            }                  
          default:
            #if defined Debug_Mavlite
              Debug.printf("Mavlite Uplink: Unknown Message ID #%d ignored\n", mt_msg_id);
              PrintMavLiteUplink();
            #endif
            return false;   
        } // end of switch
      }  
    #endif 
    return false;        
    }

    //===================================================================  
    #if defined Support_MavLite
    
    bool FrSkyPort::UnchunkMavLitePayload() {              
      if (mt_seq == 0) {            // first chunk 
        for (int i = 6 ; i <= 8 ; i++, mt_idx++) {
      //    Debug.printf("XX i=%d  mt_idx=%d frbuf[i]=%x \n",i, mt_idx, frbuf[i]);
          mt_payload[mt_idx] = frbuf[i];
          if (mt_idx >= mt_paylth) {
            return true;
          }        
        }
        return false;
      } else {           // rest of the chunks
        for (int i = 4 ; i <= 8 ; i++, mt_idx++) {
     //     Debug.printf("YY i=%d  mt_idx=%d frbuf[i]=%x \n",i, mt_idx, frbuf[i]);
          mt_payload[mt_idx] = frbuf[i];
          if (mt_idx >= mt_paylth) {
            mt_payload[mt_idx] = 0x00; // terminate string      
            return true; 
          }                
         }
       return false;
      }
     return false;
    }
    #endif
    //============================  U P L I N K   T O   G C S  ================================ 

    void FrSkyPort::SendPassthruPacket(uint8_t sensor_id, uint16_t msg_id) {
      uint8_t *bytes;

      #if defined Debug_Inject_Delay
        Debug.printf("Inject_delay %duS\n", (micros() - inject_delay));
      #endif
  
      if (set.trmode == ground) {               // Only if ground mode send these bytes, else XSR sends them
        FrSkyPort::SafeWrite(0x7E, false);       // START/STOP don't stuff or add into crcout
        FrSkyPort::SafeWrite(sensor_id, false);  // alias == instance 
      }

      if (set.frport == f_port) {
        FrSkyPort::SafeWrite(0x7E, false);        // START - don't stuff or add into crcout
        FrSkyPort::SafeWrite(0x08, true );        //  Passthru - lth
        FrSkyPort::SafeWrite(0x81, true );        //  Passthru - type  
      }
            
      FrSkyPort::SafeWrite(0x10, true );        //  Passthru - prime

      bytes = (uint8_t*)&msg_id;               // cast to bytes

      FrSkyPort::SafeWrite(bytes[0], true);
      FrSkyPort::SafeWrite(bytes[1], true);
  
      #if (defined Frs_Debug_Payload) 
        PrintFrPeriod(0);
        Debug.print("\tDataFrame. ID "); 
        Printbyte(bytes[0], 0);
        Debug.print(" "); 
        Printbyte(bytes[1], 0);
      #endif
  
      bytes = (uint8_t*)&pt_payload;       // cast to bytes

      FrSkyPort::SafeWrite(bytes[0], true);
      FrSkyPort::SafeWrite(bytes[1], true);
      FrSkyPort::SafeWrite(bytes[2], true);
      FrSkyPort::SafeWrite(bytes[3], true);
  
      #if (defined Frs_Debug_Payload) 
        Debug.print(" Payload (send order) "); 
        Printbyte(bytes[0], 0);
        Debug.print(" "); 
        Printbyte(bytes[1], 0);
        Debug.print(" "); 
        Printbyte(bytes[2], 0);
        Debug.print(" "); 
        Printbyte(bytes[3], 0);  
        Debug.print(" crcout= "); 
        Printbyte(0xFF-crcout, 0);
        Debug.println("/"); 
      #endif 

      FrSkyPort::WriteCrc(); 
      
      if (set.frport == f_port) {    
        FrSkyPort::SafeWrite(0x7E, false);        // STOP - don't stuff or add into crcout               
      }
}
    //===================================================================  
    void FrSkyPort::SendMavlitePacket(uint8_t sensor_id, uint16_t msg_id) {
      uint8_t *bytes;
  
      if (set.trmode == ground) {         // Only if ground mode send these bytes, else XSR sends them
        FrSkyPort::SafeWrite(0x7E, false);       //  START/STOP don't stuff or add into crcout
        FrSkyPort::SafeWrite(sensor_id, false);  // alias == instance 
      }
  
      FrSkyPort::SafeWrite(0x32, false );        //  MavLite frame id

      bytes = (uint8_t*)&mtf_payload;      // cast to bytes

      FrSkyPort::SafeWrite(bytes[0], false);
      FrSkyPort::SafeWrite(bytes[1], false);
      FrSkyPort::SafeWrite(bytes[2], false);
      FrSkyPort::SafeWrite(bytes[3], false);
      FrSkyPort::SafeWrite(bytes[4], false); 
      FrSkyPort::SafeWrite(bytes[5], false);

  
      #if (defined Frs_Debug_Payload) 
        Debug.print("Payload (send order) "); 
        Printbyte(bytes[0], 0);
        Debug.print(" "); 
        Printbyte(bytes[1], 0);
        Debug.print(" "); 
        Printbyte(bytes[2], 0);
        Debug.print(" "); 
        Printbyte(bytes[3], 0);  
        Printbyte(bytes[4], 0);   
        Printbyte(bytes[5], 0);  
        Debug.println("/"); 
      #endif
  
    }
    //===================================================================   
    void FrSkyPort::WriteCrc() {
      uint8_t byte;
      byte = 0xFF-crcout;

      FrSkyPort::SafeWrite(byte, false);
 
      // Printbyte(byte, 1);
      // Debug.println("");
      crcout = 0;          // crcout reset
    }
    //===================================================================  

    uint16_t FrSkyPort::FirstEmptyMt20Row() {
      uint16_t i = 0;
      while (mt20[i].inuse == 1) {   // find empty mt20 uplink row
      //  Debug.printf("i=%d  mt20[i].inuse=%d\n", i, mt20[i].inuse);

        if (millis() - mt20[i].millis > 5000) { // expire the row if no reply from FC in 5 seconds ? bad param_id
          mt20[i].inuse = 0;
          Debug.printf("param_id %s not received back from FC. Timed out.\n", mt20[i].param_id);
        }
        i++; 
        if ( i >= mt20_rows-1) {
          mt20_buf_full_gear++;
          if ( (mt20_buf_full_gear == 0) || (mt20_buf_full_gear%1000 == 0)) {
            Debug.println("mt20 uplink buffer full");  // Report every so often
          }
          return 0xffff;
        }
      }
      return i;
    }    
    //=================================================================== 
    void FrSkyPort::PushMessage(uint16_t msg_id, uint8_t sub_id) {    // Downlink
      switch(msg_id) {
        case 0x16:                   // msg_id 0x16 MavLite PARAM_VALUE ( #22 )
          FrSkyPort::Push_Param_Val_016(msg_id);
          break; 
      
        case 0x800:                  // msg_id 0x800 Lat & Lon
          if (sub_id == 0) {
            FrSkyPort::Push_Lat_800(msg_id);
          }
          if (sub_id == 1) {
            FrSkyPort::Push_Lon_800(msg_id);
          }
          break;            
        case 0x5000:                 // msg_id 0x5000 Status Text            
            FrSkyPort::Push_Text_Chunks_5000(msg_id);
            break;
        
        case 0x5001:                // msg_id 0x5001 AP Status
          FrSkyPort::Push_AP_status_5001(msg_id);
          break; 

        case 0x5002:                // msg_id 0x5002 GPS Status
          FrSkyPort::Push_GPS_status_5002(msg_id);
          break; 
          
        case 0x5003:                //msg_id 0x5003 Batt 1
          FrSkyPort::Push_Bat1_5003(msg_id);
          break; 
                    
        case 0x5004:                // msg_id 0x5004 Home
          FrSkyPort::Push_Home_5004(msg_id);
          break; 

        case 0x5005:                // msg_id 0x5005 Velocity and yaw
          FrSkyPort::Push_VelYaw_5005(msg_id);
          break; 

        case 0x5006:                // msg_id 0x5006 Attitude and range
          FrSkyPort::Push_Atti_5006(msg_id);
          break; 
      
        case 0x5007:                // msg_id 0x5007 Parameters 
          FrSkyPort::Push_Parameters_5007(msg_id, sub_id);
          break; 
      
        case 0x5008:                // msg_id 0x5008 Batt 2
          FrSkyPort::Push_Bat2_5008(msg_id);
          break; 

        case 0x5009:                // msg_id 0x5009 Waypoints/Missions 
          FrSkyPort::Push_WayPoint_5009(msg_id);
          break;       

        case 0x50F1:                // msg_id 0x50F1 Servo_Raw            
          FrSkyPort::Push_Servo_Raw_50F1(msg_id);
          break;      

        case 0x50F2:                // msg_id 0x50F2 VFR HUD          
          FrSkyPort::Push_VFR_Hud_50F2(msg_id);
          break;    

        case 0x50F3:                // msg_id 0x50F3 Wind Estimate      
       //   FrSkyPort::Push_Wind_Estimate_50F3(msg_id);  // not presently implemented
          break; 
        case 0xF101:                // msg_id 0xF101 RSSI      
          FrSkyPort::Push_Rssi_F101(msg_id);      
          break;       
        default:
          Debug.print("Warning, msg_id "); Debug.print(msg_id, HEX); Debug.println(" unknown");
          break;       
      }            
    }
    //=================================================================== 
    void FrSkyPort::PushToEmptyRow(uint16_t msg_id, uint8_t sub_id) {  // Downlink
      sb_row = 0;
      while (sb[sb_row].inuse) {   // find empty F.Port row 
        sb_row++; 
        if (sb_row >= sb_rows-1) {
          if ( (sb_buf_full_gear == 0) || (sb_buf_full_gear%4000 == 0)) {
            Debug.println("FrPort scheduler buffer full. Check FrPort downlink");  // Report every so often
          }
          sb_buf_full_gear++;
          return;     
        }
      }
      sb_unsent++;

      // The push
      sb[sb_row].millis = millis();
      sb[sb_row].inuse = 1;                // 0=free for use, 1=occupied
      sb[sb_row].msg_id = msg_id;
      sb[sb_row].sub_id = sub_id;
  
      if(FrSkyPort::msg_class_now(msg_id) == passthru) {
          sb[sb_row].payload.passthru = pt_payload;  
      } else
      if(FrSkyPort::msg_class_now(msg_id)== mavlite) {
          sb[sb_row].payload.mavlite = mtf_payload;  
      }
  
      #if (defined Frs_Debug_Scheduler) || (defined MavLite_Debug_Scheduler) 
  
        uint16_t msgid_filter;  
        #if (defined MavLite_Debug_Scheduler)
          msgid_filter = 0x100;
        #else
          msgid_filter = 0xffff; // high values
        #endif
    
        if (msg_id < msgid_filter) {
          Debug.print(sb_unsent); 
          Debug.printf("\tPush row= %3d", sb_row );
          Debug.print("  msg_id=0x"); Debug.print(msg_id, HEX);
          if (msg_id < 0x1000) Debug.print(" ");
          Debug.printf("  sub_id= %2d", sub_id);
          pb_rx=false;
          FrSkyPort::PrintPayload(FrSkyPort::msg_class_now(msg_id));
          Debug.println();      
        }
      #endif
    }
    //=================================================================== 
 
    void FrSkyPort::Push_Param_Val_016(uint16_t msg_id) {   //  0x16 MavLite PARAM_VALUE ( #22 ) Downlink
    #if defined Support_MavLite  
      //printcrcout = 1; //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
 
      mt_msg_id = msg_id;                                // 0x16;  
      mt_paylth = 4 +strlen(ap22_param_id);              // payload = value + param = 17

      // value to expanded payload
      byte *b = (byte *)&ap22_param_value;  // cast value to bytes
      mt_payload[0] = b[0];
      mt_payload[1] = b[1];
      mt_payload[2] = b[2];
      mt_payload[3] = b[3];

      for (int i=0; i<16 ; i++) {
        mt_payload[i+4] = ap22_param_id[i];
      }

      mt_totlth = 2 + mt_paylth;                         // msg_id + lth + payload
      
      Debug.printf("Mavlite #22 downlink lth=%d\n", mt_totlth);    
      
      #if (defined Debug_Mavlite)
          Debug.printf("PushMavLiteParamValue16 msg_id=0x%X lth=%d param_value=%.3f param_id=%s \n"
            , mt_msg_id, mt_paylth, ap22_param_value, ap22_param_id );           
          Debug.print("mliteMsg=");
          for (int i = 0 ; i < mt_totlth ; i++) {
            if ( (i == 3) || ( (i > 3) && (!((i+2)%5)) ) ) Debug.print("|");
            PrintbyteNon(mt_payload[i]);
          }  
          Debug.print("|\t|");
          for (int i = 0 ; i < mt_totlth ; i++) {
            if ((mt_payload[i] >31) && (mt_payload[i]<127)) Debug.write(mt_payload[i]);
          }
          Debug.println("|");
        #endif
      
      FrSkyPort::ChunkMavLitePayload(msg_id, mt_paylth);
      
    #endif
    }

    //===================================================================     
    #if defined Support_MavLite  // Downlink
    
    void FrSkyPort::ChunkMavLitePayload(uint8_t msg_id, uint8_t tlth) {
      mt_chunk = 0; 
      mt_idx = 0;   

      while (mt_idx <= tlth) {    
      //  Debug.printf("mt_idx=%d  tlth=%d   mt_chunk=%d\n", mt_idx, tlth, mt_chunk     ); 
        if (mt_chunk == 0) {

          //======= message to frame (chunk)
          // payload starting position  msgid, lth - then 20B payload, then crcout
    
          // first chunk 
          mtf_payload.seq = mt_chunk;      FrSkyPort::crcStepOut(mtf_payload.lth);
          mtf_payload.lth = mt_paylth;     FrSkyPort::crcStepOut(mtf_payload.lth);     // idx = 1          
          mtf_payload.msg_id = msg_id;     FrSkyPort::crcStepOut(mtf_payload.msg_id);  // idx = 0

         //______________________________________________________
         for (int i = 3 ; i < 6 ; i++) {  
            mtf_payload.raw[i] = mt_payload[mt_idx];  
            FrSkyPort::crcStepOut(mtf_payload.raw[i]); 
            #if defined Debug_Mavlite
              Debug.printf("i=%d mt_idx=%d mtf_payload.raw[i]=0x%X\t|", i, mt_idx, mtf_payload.raw[i]); 
              if ((mtf_payload.raw[i] > 31) && (mtf_payload.raw[i] < 127)) 
                Debug.write(mtf_payload.raw[i]);
                else Debug.print(" ");
              Debug.println("|");  
            #endif
        
            mt_idx++;      
          } 
      
          //_______________________________________________________
          // rest of the frames / chunks
        } else {           
          mtf_payload.seq = mt_chunk;  FrSkyPort::crcStepOut(mtf_payload.seq); 
          //_______________________________________________________
          for (int i = 1 ; i < 6 ; i++) { 
          
            if (mt_idx >= tlth) {  // break out of the for loop
              mtf_payload.raw[i] = crcout;     // no 1s complement? and append the crcout
      //       Debug.printf("mt_idx=%d  mt_payload.raw[mt_idx]=0x%X\n", mt_idx, mt_payload.raw[mt_idx] );
              mt_idx++;  // gets me out of the while loop
              break; // short chunk  - break out of for loop
            }

            mtf_payload.raw[i] = mt_payload[mt_idx];
            FrSkyPort::crcStepOut(mtf_payload.raw[i]); 
            #if (defined Debug_Mavlite)
              Debug.printf("i=%d mt_idx=%d mtf_payload.raw[i]=0x%X\t|", i, mt_idx, mtf_payload.raw[i]); 
                if ((mtf_payload.raw[i] > 31) && (mtf_payload.raw[i] < 127)) 
                Debug.write(mtf_payload.raw[i]);
                else Debug.print(" ");
              Debug.println("|");
            #endif
             
            mtf_payload.raw[i] = mt_payload[mt_idx];   
            mt_idx++;                                    
          }  // end of for
          //_________________________________________________________ 
        }   // end of else - chunks > 0

        FrSkyPort::PushToEmptyRow(msg_id, 0);  

        #if defined Frs_Debug_All || defined Debug_Mavlite
          Debug.printf("MavLite downlink chunk 0x%x: ", msg_id);
          Debug.printf(" seq=%d  ", mtf_payload.seq);
          pb_rx = false;
          FrSkyPort::PrintPayload(mavlite); 
          Debug.println();
        #endif
      
        mt_chunk++;
        mtf_payload = {};  // clear payload struct
      } // end of while loop

      crcout = 0;
       printcrcout = 0;
 
    }
    #endif   
    //===================================================================   
    void FrSkyPort::Push_Lat_800(uint16_t msg_id) {       // 0x800
      pt_gps_status = ap24_fixtype < 3 ? ap24_fixtype : 3;                   //  0 - 3 
      if (pt_gps_status < 3) return;
      if ((px4_flight_stack) || (pitlab_flight_stack)) {
        pt_lat = Abs(ap24_lat) / 100 * 6;  // ap_lat * 60 / 1000
        if (ap24_lat<0) 
          ms2bits = 1;
        else ms2bits = 0;    
      } else {
        pt_lat = Abs(ap33_lat) / 100 * 6;  // ap_lat * 60 / 1000
        if (ap33_lat<0) 
          ms2bits = 1;
        else ms2bits = 0;
      }
      pt_payload = 0;
      bit32Pack(pt_lat, 0, 30);
      bit32Pack(ms2bits, 30, 2);
          
      #if defined Frs_Debug_All || defined Frs_Debug_LatLon
        PrintFrPeriod(0); 
        Debug.print("Passthru out LatLon 0x800: ");
        Debug.print(" ap33_lat="); Debug.print((float)ap33_lat / 1E7, 7); 
        Debug.print(" pt_lat="); Debug.print(pt_lat);  
        Debug.print(" pt_payload="); Debug.print(pt_payload); Debug.print(" ");
       FrSkyPort::PrintPayload(passthru);
        int32_t r_lat = (bit32Extract(pt_payload,0,30) * 100 / 6);
        Debug.print(" lat unpacked="); Debug.println(r_lat );    
      #endif

     FrSkyPort::PushToEmptyRow(msg_id, 0);        
    }
    //===================================================================  
    void FrSkyPort::Push_Lon_800(uint16_t msg_id) {      // 0x800
      pt_gps_status = ap24_fixtype < 3 ? ap24_fixtype : 3;                   //  0 - 3 
      if (pt_gps_status < 3) return;
      if ((px4_flight_stack) || (pitlab_flight_stack)) {
        pt_lon = Abs(ap24_lon) / 100 * 6;  // ap_lon * 60 / 1000
        if (ap24_lon<0) {
          ms2bits = 3;
        }
        else {
          ms2bits = 2;    
        }
      } else {
        pt_lon = Abs(ap33_lon) / 100 * 6;  // ap_lon * 60 / 1000
        if (ap33_lon<0) { 
          ms2bits = 3;
        }
        else {
          ms2bits = 2;
        }
      }
      pt_payload = 0;
      bit32Pack(pt_lon, 0, 30);
      bit32Pack(ms2bits, 30, 2);
          
      #if defined Frs_Debug_All || defined Frs_Debug_LatLon
       PrintFrPeriod(0); 
        Debug.print("Passthru out LatLon 0x800: ");  
        Debug.print(" ap33_lon="); Debug.print((float)ap33_lon / 1E7, 7);     
        Debug.print(" pt_lon="); Debug.print(pt_lon); 
        Debug.print(" pt_payload="); Debug.print(pt_payload); Debug.print(" ");
        FrSkyPort::PrintPayload(passthru);
        int32_t r_lon = (bit32Extract(pt_payload,0,30) * 100 / 6);
        Debug.print(" lon unpacked="); Debug.println(r_lon );  
      #endif

       FrSkyPort::PushToEmptyRow(msg_id, 1); 
    }
    //===================================================================  
    void FrSkyPort::Push_Text_Chunks_5000(uint16_t msg_id) {

      // status text  char[50] no null,  ap-text char[60]

      for (int i=0; i<50 ; i++) {       // Get text len
        if (ap_text[i]==0) {            // end of text
          len=i;
          break;
        }
      }
  
      ap_text[len+1]=0x00;
      ap_text[len+2]=0x00;  // mark the end of text chunk +
      ap_text[len+3]=0x00;
      ap_text[len+4]=0x00;
          
      ap_txtlth = len;
  
      // look for simple-mode status change messages       
      if (strcmp (ap_text,"SIMPLE mode on") == 0)
        ap_simple = true;
      else if (strcmp (ap_text,"SIMPLE mode off") == 0)
        ap_simple = false;

      pt_severity = ap_severity;
      pt_txtlth = ap_txtlth;
      memcpy(pt_text, ap_text, pt_txtlth+4);   // plus rest of last chunk at least
      pt_simple = ap_simple;

      #if defined Frs_Debug_All || defined Frs_Debug_StatusText
        PrintFrPeriod(0); 
        Debug.print("Passthru out AP_Text 0x5000: ");  
        Debug.print(" pt_severity="); Debug.print(pt_severity);
        Debug.print(" "); Debug.print(MavSeverity(pt_severity)); 
        Debug.print(" Text= ");  Debug.print(" |"); Debug.print(pt_text); Debug.println("| ");
      #endif

      pt_chunk_idx = 0;

      while (pt_chunk_idx <= (pt_txtlth)) {                 // send multiple 4 byte (32b) chunks
    
        pt_chunk_num = (pt_chunk_idx / 4) + 1;
    
        pt_chunk[0] = pt_text[pt_chunk_idx];
        pt_chunk[1] = pt_text[pt_chunk_idx+1];
        pt_chunk[2] = pt_text[pt_chunk_idx+2];
        pt_chunk[3] = pt_text[pt_chunk_idx+3];
    
        pt_payload = 0;
        bit32Pack(pt_chunk[0], 24, 7);
        bit32Pack(pt_chunk[1], 16, 7);
        bit32Pack(pt_chunk[2], 8, 7);    
        bit32Pack(pt_chunk[3], 0, 7);  
    
        #if defined Frs_Debug_All || defined Frs_Debug_StatusText
          PrintFrPeriod(0); 
          Debug.print(" pt_chunk_num="); Debug.print(pt_chunk_num); 
          Debug.print(" pt_txtlth="); Debug.print(pt_txtlth); 
          Debug.print(" pt_chunk_idx="); Debug.print(pt_chunk_idx); 
          Debug.print(" "); 
          strncpy(pt_chunk_print,pt_chunk, 4);
          pt_chunk_print[4] = 0x00;
          Debug.print(" |"); Debug.print(pt_chunk_print); Debug.print("| ");
          Debug.print(" pt_payload="); Debug.print(pt_payload); Debug.print(" ");
          FrSkyPort::PrintPayload(passthru);
          Debug.println();
        #endif  

        if (pt_chunk_idx+4 > (pt_txtlth)) {

          bit32Pack((pt_severity & 0x1), 7, 1);            // ls bit of severity
          bit32Pack(((pt_severity & 0x2) >> 1), 15, 1);    // mid bit of severity
          bit32Pack(((pt_severity & 0x4) >> 2) , 23, 1);   // ms bit of severity                
          bit32Pack(0, 31, 1);     // filler
      
          #if defined Frs_Debug_All || defined Frs_Debug_StatusText
            PrintFrPeriod(0); 
            Debug.print(" pt_chunk_num="); Debug.print(pt_chunk_num); 
            Debug.print(" pt_severity="); Debug.print(pt_severity);
            Debug.print(" "); Debug.print(MavSeverity(pt_severity)); 
            bool lsb = (pt_severity & 0x1);
            bool sb = (pt_severity & 0x2) >> 1;
            bool msb = (pt_severity & 0x4) >> 2;
            Debug.print(" ls bit="); Debug.print(lsb); 
            Debug.print(" mid bit="); Debug.print(sb); 
            Debug.print(" ms bit="); Debug.print(msb); 
            Debug.print(" pt_payload="); Debug.print(pt_payload); Debug.print(" ");
            FrSkyPort::PrintPayload(passthru);
            Debug.println(); Debug.println();
         #endif 
         }

       FrSkyPort::PushToEmptyRow(msg_id, pt_chunk_num); 

       #if defined Send_status_Text_3_Times 
        FrSkyPort::PushToEmptyRow(msg_id, pt_chunk_num); 
        FrSkyPort::PushToEmptyRow(msg_id, pt_chunk_num); 
       #endif 
    
       pt_chunk_idx +=4;
     }
  
      pt_chunk_idx = 0;
   
    }

    //===================================================================   
    void FrSkyPort::Push_AP_status_5001(uint16_t msg_id) {
      if (ap_type == 6) return;      // If GCS heartbeat ignore it  -  yaapu  - ejs also handled at #0 read
      pt_payload = 0;
     // pt_simple = ap_simple;         // Derived from "ALR SIMPLE mode on/off" text messages
      pt_simple = 0;                   // stops repeated 'simple mode enabled' and flight mode messages
      pt_armed = ap_base_mode >> 7;  
      pt_land_complete = pt_armed;
  
      if (px4_flight_stack) {
        pt_flight_mode = PX4FlightModeNum(px4_main_mode, px4_sub_mode);
      } else 
      if (pitlab_flight_stack) {
        pt_flight_mode = ap_custom_mode;
      } else { //  APM Flight Stack
        pt_flight_mode = ap_custom_mode + 1; // AP_CONTROL_MODE_LIMIT - ls 5 bits
      }
  
      pt_imu_temp = ap26_temp;
    
      bit32Pack(pt_flight_mode, 0, 5);      // Flight mode   0-32 - 5 bits
      bit32Pack(pt_simple ,5, 2);           // Simple/super simple mode flags
      bit32Pack(pt_land_complete ,7, 1);    // Landed flag
      bit32Pack(pt_armed ,8, 1);            // Armed
      bit32Pack(pt_bat_fs ,9, 1);           // Battery failsafe flag
      bit32Pack(pt_ekf_fs ,10, 2);          // EKF failsafe flag
      bit32Pack(px4_flight_stack ,12, 1);   // px4_flight_stack flag
      bit32Pack(pt_imu_temp, 26, 6);        // imu temperature in cdegC

      #if defined Frs_Debug_All || defined Frs_Debug_APStatus
        PrintFrPeriod(0); 
        Debug.print("Passthru out AP_status 0x5001: ");   
        Debug.print(" pt_flight_mode="); Debug.print(pt_flight_mode);
        Debug.print(" pt_simple="); Debug.print(pt_simple);
        Debug.print(" pt_land_complete="); Debug.print(pt_land_complete);
        Debug.print(" pt_armed="); Debug.print(pt_armed);
        Debug.print(" pt_bat_fs="); Debug.print(pt_bat_fs);
        Debug.print(" pt_ekf_fs="); Debug.print(pt_ekf_fs);
        Debug.print(" px4_flight_stack="); Debug.print(px4_flight_stack);
        Debug.print(" pt_imu_temp="); Debug.print(pt_imu_temp);
        Debug.print(" pt_payload="); Debug.print(pt_payload); Debug.print(" ");
        FrSkyPort::PrintPayload(passthru);
        Debug.println();
      #endif

      FrSkyPort::PushToEmptyRow(msg_id, 0);       

    }
    //===================================================================   
    void FrSkyPort::Push_GPS_status_5002(uint16_t msg_id) {
      pt_payload = 0;
      if (ap24_sat_visible > 15)
        pt_numsats = 15;
      else
        pt_numsats = ap24_sat_visible;
  
      bit32Pack(pt_numsats ,0, 4); 
          
      pt_gps_status = ap24_fixtype < 3 ? ap24_fixtype : 3;                   //  0 - 3
      pt_gps_adv_status = ap24_fixtype > 3 ? ap24_fixtype - 3 : 0;           //  4 - 8 -> 0 - 3   
          
      pt_amsl = ap24_amsl / 100;  // dm
      pt_hdop = ap24_eph /10;
          
      bit32Pack(pt_gps_status ,4, 2);       // part a, 3 bits
      bit32Pack(pt_gps_adv_status ,14, 2);  // part b, 3 bits
          
      #if defined Frs_Debug_All || defined Frs_Debug_GPS_status
        PrintFrPeriod(0); 
        Debug.print("Passthru out GPS Status 0x5002: ");   
        Debug.print(" pt_numsats="); Debug.print(pt_numsats);
        Debug.print(" pt_gps_status="); Debug.print(pt_gps_status);
        Debug.print(" pt_gps_adv_status="); Debug.print(pt_gps_adv_status);
        Debug.print(" pt_amsl="); Debug.print(pt_amsl);
        Debug.print(" pt_hdop="); Debug.print(pt_hdop);
      #endif
          
      pt_amsl = prep_number(pt_amsl,2,2);                       // Must include exponent and mantissa    
      pt_hdop = prep_number(pt_hdop,2,1);
          
      #if defined Frs_Debug_All || defined Frs_Debug_GPS_status
        Debug.print(" After prep: pt_amsl="); Debug.print(pt_amsl);
        Debug.print(" pt_hdop="); Debug.print(pt_hdop); 
        Debug.print(" pt_payload="); Debug.print(pt_payload); Debug.print(" ");
        FrSkyPort::PrintPayload(passthru);
        Debug.println(); 
      #endif     
              
      bit32Pack(pt_hdop ,6, 8);
      bit32Pack(pt_amsl ,22, 9);
      bit32Pack(0, 31,0);  // 1=negative 

      FrSkyPort::PushToEmptyRow(msg_id, 0);  
    }
    //===================================================================   
    void FrSkyPort::Push_Bat1_5003(uint16_t msg_id) {   //  Into F.Port table from #1 SYS_status only
      pt_payload = 0;
      pt_bat1_volts = ap_voltage_battery1 / 100;         // Were mV, now dV  - V * 10
      pt_bat1_amps = ap_current_battery1 ;               // Remain       dA  - A * 10   
  
      // pt_bat1_mAh is populated at #147 depending on battery id.  Into F.Port table from #1 SYS_status only.
      //pt_bat1_mAh = Total_mAh1();  // If record type #147 is not sent and good
  
      #if defined Frs_Debug_All || defined Debug_Batteries
        PrintFrPeriod(0); 
        Debug.print("Passthru out Bat1 0x5003: ");   
        Debug.print(" pt_bat1_volts="); Debug.print(pt_bat1_volts);
        Debug.print(" pt_bat1_amps="); Debug.print(pt_bat1_amps);
        Debug.print(" pt_bat1_mAh="); Debug.print(pt_bat1_mAh);
        Debug.print(" pt_payload="); Debug.print(pt_payload); Debug.print(" ");
        FrSkyPort::PrintPayload(passthru);
        Debug.println();               
      #endif
          
      bit32Pack(pt_bat1_volts ,0, 9);
      pt_bat1_amps = prep_number(roundf(pt_bat1_amps * 0.1F),2,1);          
      bit32Pack(pt_bat1_amps,9, 8);
      bit32Pack(pt_bat1_mAh,17, 15);

      FrSkyPort::PushToEmptyRow(msg_id, 0);  
                       
    }
    //===================================================================
       
    void FrSkyPort::Push_Home_5004(uint16_t msg_id) {
      pt_payload = 0;
    
      lon1=hom.lon/180*PI;  // degrees to radians
      lat1=hom.lat/180*PI;
      lon2=cur.lon/180*PI;
      lat2=cur.lat/180*PI;

      //Calculate azimuth bearing of craft from home
      a=atan2(sin(lon2-lon1)*cos(lat2), cos(lat1)*sin(lat2)-sin(lat1)*cos(lat2)*cos(lon2-lon1));
      az=a*180/PI;  // radians to degrees
      if (az<0) az=360+az;

      pt_home_angle = Add360(az, -180);                           // Is now the angle from the craft to home in degrees
  
      pt_home_arrow = pt_home_angle * 0.3333;                     // Units of 3 degrees

      // Calculate the distance from home to craft
      dLat = (lat2-lat1);
      dLon = (lon2-lon1);
      a = sin(dLat/2) * sin(dLat/2) + sin(dLon/2) * sin(dLon/2) * cos(lat1) * cos(lat2); 
      c = 2* asin(sqrt(a));    // proportion of Earth's radius
      dis = 6371000 * c;       // radius of the Earth is 6371km

      if (homGood)
        pt_home_dist = (int)dis;
      else
        pt_home_dist = 0;

        pt_home_alt = ap33_alt_ag / 100;    // mm->dm
        
      #if defined Frs_Debug_All || defined Frs_Debug_Home
        PrintFrPeriod(0); 
        Debug.print("Passthru out Home 0x5004: ");         
        Debug.print("pt_home_dist=");  Debug.print(pt_home_dist);
        Debug.print(" pt_home_alt=");  Debug.print(pt_home_alt);
        Debug.print(" az=");  Debug.print(az);
        Debug.print(" pt_home_angle="); Debug.print(pt_home_angle);  
        Debug.print(" pt_home_arrow="); Debug.print(pt_home_arrow);         // units of 3 deg  
        Debug.print(" pt_payload="); Debug.print(pt_payload); Debug.print(" ");
        FrSkyPort::PrintPayload(passthru);
        Debug.println();      
      #endif
      pt_home_dist = prep_number(roundf(pt_home_dist), 3, 2);
      bit32Pack(pt_home_dist ,0, 12);
      pt_home_alt = prep_number(roundf(pt_home_alt), 3, 2);
      bit32Pack(pt_home_alt ,12, 12);
      if (pt_home_alt < 0)
        bit32Pack(1,24, 1);
      else  
        bit32Pack(0,24, 1);
      bit32Pack(pt_home_arrow,25, 7);

      FrSkyPort::PushToEmptyRow(msg_id, 0);  

    }

    //===================================================================   
    
    void FrSkyPort::Push_VelYaw_5005(uint16_t msg_id) {
      pt_payload = 0;
  
      pt_vy = ap74_climb * 10;   // from #74   m/s to dm/s;
      pt_vx = ap74_grd_spd * 10;  // from #74  m/s to dm/s

      //pt_yaw = (float)ap33_gps_hdg / 10;  // (degrees*100) -> (degrees*10)
      pt_yaw = ap74_hdg * 10;              // degrees -> (degrees*10)
  
      #if defined Frs_Debug_All || defined Frs_Debug_VelYaw
        PrintFrPeriod(0); 
        Debug.print("Passthru out VelYaw 0x5005:");  
        Debug.print(" pt_vy=");  Debug.print(pt_vy);       
        Debug.print(" pt_vx=");  Debug.print(pt_vx);
        Debug.print(" pt_yaw="); Debug.print(pt_yaw);
     
      #endif
      if (pt_vy<0)
        bit32Pack(1, 8, 1);
      else
        bit32Pack(0, 8, 1);
      pt_vy = prep_number(roundf(pt_vy), 2, 1);  // Vertical velocity
      bit32Pack(pt_vy, 0, 8);   

      pt_vx = prep_number(roundf(pt_vx), 2, 1);  // Horizontal velocity
      bit32Pack(pt_vx, 9, 8);    
      pt_yaw = pt_yaw * 0.5f;                   // Unit = 0.2 deg
      bit32Pack(pt_yaw ,17, 11);  

     #if defined Frs_Debug_All || defined Frs_Debug_VelYaw
       Debug.print(" After prep:"); \
       Debug.print(" pt_vy=");  Debug.print((int)pt_vy);          
       Debug.print(" pt_vx=");  Debug.print((int)pt_vx);  
       Debug.print(" pt_yaw="); Debug.print((int)pt_yaw);  
       Debug.print(" pt_payload="); Debug.print(pt_payload); Debug.print(" ");
       FrSkyPort::PrintPayload(passthru);
       Debug.println();                 
     #endif

     FrSkyPort::PushToEmptyRow(msg_id, 0);  
    
    }
    //=================================================================== 
      
    void FrSkyPort::Push_Atti_5006(uint16_t msg_id) {
      pt_payload = 0;
  
      pt_roll = (ap_roll * 5) + 900;             //  -- pt_roll units = [0,1800] ==> [-180,180]
      pt_pitch = (ap_pitch * 5) + 450;           //  -- pt_pitch units = [0,900] ==> [-90,90]
      pt_range = roundf(ap_range*100);   
      bit32Pack(pt_roll, 0, 11);
      bit32Pack(pt_pitch, 11, 10); 
      bit32Pack(prep_number(pt_range,3,1), 21, 11);
      #if defined Frs_Debug_All || defined Frs_Debug_AttiRange
        PrintFrPeriod(0); 
        Debug.print("Passthru out Attitude 0x5006: ");         
        Debug.print("pt_roll=");  Debug.print(pt_roll);
        Debug.print(" pt_pitch=");  Debug.print(pt_pitch);
        Debug.print(" pt_range="); Debug.print(pt_range);
        Debug.print(" Payload="); Debug.println(pt_payload);  
      #endif

      FrSkyPort::PushToEmptyRow(msg_id, 0);   
     
    }
    //===================================================================   
    
    void FrSkyPort::Push_Parameters_5007(uint16_t msg_id, uint8_t sub_id) {
    
      switch(sub_id) {
        case 1:                                    // Frame type
          pt_param_id = 1;
          pt_frame_type = ap_type;
      
          pt_payload = 0;
          bit32Pack(pt_frame_type, 0, 24);
          bit32Pack(pt_param_id, 24, 4);

          #if defined Frs_Debug_All || defined Frs_Debug_Params
            PrintFrPeriod(0);  
            Debug.print("Passthru out Params 0x5007: ");   
            Debug.print(" pt_param_id="); Debug.print(pt_param_id);
            Debug.print(" pt_frame_type="); Debug.print(pt_frame_type);  
            Debug.print(" pt_payload="); Debug.print(pt_payload);  Debug.print(" "); 
            FrSkyPort::PrintPayload(passthru);
            Debug.println();                
          #endif
      
          FrSkyPort::PushToEmptyRow(msg_id, sub_id);
          break;    
       
        case 4:    // Battery pack 1 capacity
          pt_param_id = sub_id;
          #if (Battery_mAh_Source == 2)    // Local
            pt_bat1_capacity = bat1_capacity;
          #elif  (Battery_mAh_Source == 1) //  FC
            pt_bat1_capacity = ap_bat1_capacity;
          #endif 

          pt_payload = 0;
          bit32Pack(pt_bat1_capacity, 0, 24);
          bit32Pack(pt_param_id, 24, 4);

          #if defined Frs_Debug_All || defined Frs_Debug_Params || defined Debug_Batteries
            PrintFrPeriod(0);       
            Debug.print("Passthru out Params 0x5007: ");   
            Debug.print(" pt_param_id="); Debug.print(pt_param_id);
            Debug.print(" pt_bat1_capacity="); Debug.print(pt_bat1_capacity);  
            Debug.print(" pt_payload="); Debug.print(pt_payload);  Debug.print(" "); 
            FrSkyPort::PrintPayload(passthru);
            Debug.println();                   
          #endif

          FrSkyPort::PushToEmptyRow(msg_id, sub_id); 
          break; 
      
        case 5:                 // Battery pack 2 capacity
          pt_param_id = sub_id;
          #if (Battery_mAh_Source == 2)    // Local
            pt_bat2_capacity = bat2_capacity;
          #elif  (Battery_mAh_Source == 1) //  FC
            pt_bat2_capacity = ap_bat2_capacity;
          #endif  

          pt_payload = 0;
          bit32Pack(pt_bat2_capacity, 0, 24);
          bit32Pack(pt_param_id, 24, 4);
      
          #if defined Frs_Debug_All || defined Frs_Debug_Params || defined Debug_Batteries
            PrintFrPeriod(0);  
            Debug.print("Passthru out Params 0x5007: ");   
            Debug.print(" pt_param_id="); Debug.print(pt_param_id);
            Debug.print(" pt_bat2_capacity="); Debug.print(pt_bat2_capacity); 
            Debug.print(" pt_payload="); Debug.print(pt_payload);  Debug.print(" "); 
            FrSkyPort::PrintPayload(passthru);
            Debug.println();           
          #endif
      
          FrSkyPort::PushToEmptyRow(msg_id, sub_id);
          break; 
    
         case 6:               // Number of waypoints in mission                       
          pt_param_id = sub_id;
          pt_mission_count = ap_mission_count;

          pt_payload = 0;
          bit32Pack(pt_mission_count, 0, 24);
          bit32Pack(pt_param_id, 24, 4);

          FrSkyPort::PushToEmptyRow(msg_id, sub_id);        
      
          #if defined Frs_Debug_All || defined Frs_Debug_Params || defined Debug_Batteries
            PrintFrPeriod(0); 
            Debug.print("Passthru out Params 0x5007: ");   
            Debug.print(" pt_param_id="); Debug.print(pt_param_id);
            Debug.print(" pt_mission_count="); Debug.println(pt_mission_count);           
          #endif
 
          pt_paramsSent = true;          // get this done early on and then regularly thereafter

          break;
      }    
    }
    //===================================================================   
    
    void FrSkyPort::Push_Bat2_5008(uint16_t msg_id) {
       pt_payload = 0;
   
       pt_bat2_volts = ap_voltage_battery2 / 100;         // Were mV, now dV  - V * 10
       pt_bat2_amps = ap_current_battery2 ;               // Remain       dA  - A * 10   
   
      // pt_bat2_mAh is populated at #147 depending on battery id
      //pt_bat2_mAh = Total_mAh2();  // If record type #147 is not sent and good
  
      #if defined Frs_Debug_All || defined Debug_Batteries
        PrintFrPeriod(0);  
        Debug.print("Passthru out Bat2 0x5008: ");   
        Debug.print(" pt_bat2_volts="); Debug.print(pt_bat2_volts);
        Debug.print(" pt_bat2_amps="); Debug.print(pt_bat2_amps);
        Debug.print(" pt_bat2_mAh="); Debug.print(pt_bat2_mAh);
        Debug.print(" pt_payload="); Debug.print(pt_payload);  Debug.print(" "); 
        FrSkyPort::PrintPayload(passthru);
        Debug.println();                  
      #endif        
          
      bit32Pack(pt_bat2_volts ,0, 9);
      pt_bat2_amps = prep_number(roundf(pt_bat2_amps * 0.1F),2,1);          
      bit32Pack(pt_bat2_amps,9, 8);
      bit32Pack(pt_bat2_mAh,17, 15);      

      FrSkyPort::PushToEmptyRow(msg_id, 1);           
    }


    //===================================================================   

    void FrSkyPort::Push_WayPoint_5009(uint16_t msg_id) {
      pt_payload = 0;
  
      pt_ms_seq = ap_ms_seq;                                      // Current WP seq number, wp[0] = wp1, from regular #42
  
      pt_ms_dist = ap_wp_dist;                                        // Distance to next WP  

      pt_ms_xtrack = ap_xtrack_error;                                 // Cross track error in metres from #62
      pt_ms_target_bearing = ap_target_bearing;                       // Direction of next WP
      pt_ms_cog = ap24_cog * 0.01;                                      // COG in degrees from #24
      int32_t angle = (int32_t)wrap_360(pt_ms_target_bearing - pt_ms_cog);
      int32_t arrowStep = 360 / 8; 
      pt_ms_offset = ((angle + (arrowStep/2)) / arrowStep) % 8;       // Next WP bearing offset from COG

      /*
   
    0 - up
    1 - up-right
    2 - right
    3 - down-right
    4 - down
    5 - down - left
    6 - left
    7 - up - left
 
       */
      #if defined Frs_Debug_All || defined Frs_Debug_Mission
        PrintFrPeriod(0);  
        Debug.print("Passthru out RC 0x5009: ");   
        Debug.print(" pt_ms_seq="); Debug.print(pt_ms_seq);
        Debug.print(" pt_ms_dist="); Debug.print(pt_ms_dist);
        Debug.print(" pt_ms_xtrack="); Debug.print(pt_ms_xtrack, 3);
        Debug.print(" pt_ms_target_bearing="); Debug.print(pt_ms_target_bearing, 0);
        Debug.print(" pt_ms_cog="); Debug.print(pt_ms_cog, 0);  
        Debug.print(" pt_ms_offset="); Debug.print(pt_ms_offset);
        Debug.print(" pt_payload="); Debug.print(pt_payload);  Debug.print(" "); 
        FrSkyPort::PrintPayload(passthru);         
        Debug.println();      
      #endif

      bit32Pack(pt_ms_seq, 0, 10);    //  WP number

      pt_ms_dist = prep_number(roundf(pt_ms_dist), 3, 2);       //  number, digits, power
      bit32Pack(pt_ms_dist, 10, 12);    

      pt_ms_xtrack = prep_number(roundf(pt_ms_xtrack), 1, 1);  
      bit32Pack(pt_ms_xtrack, 22, 6); 

      bit32Pack(pt_ms_offset, 29, 3);  

      FrSkyPort::PushToEmptyRow(msg_id, 1);  
        
    }

    //===================================================================  
    
    void FrSkyPort::Push_Servo_Raw_50F1(uint16_t msg_id) {
    uint8_t sv_chcnt = 8;
      pt_payload = 0;
  
      if (sv_count+4 > sv_chcnt) { // 4 channels at a time
        sv_count = 0;
        return;
      } 

      uint8_t  chunk = sv_count / 4; 

      pt_sv[1] = PWM_To_63(ap65_chan_raw[sv_count]);     // PWM 1000 to 2000 -> 6bit 0 to 63
      pt_sv[2] = PWM_To_63(ap65_chan_raw[sv_count+1]);    
      pt_sv[3] = PWM_To_63(ap65_chan_raw[sv_count+2]); 
      pt_sv[4] = PWM_To_63(ap65_chan_raw[sv_count+3]); 

      bit32Pack(chunk, 0, 4);                // chunk number, 0 = chans 1-4, 1=chans 5-8, 2 = chans 9-12, 3 = chans 13 -16 .....
      bit32Pack(Abs(pt_sv[1]) ,4, 6);        // fragment 1 
      if (pt_sv[1] < 0)
        bit32Pack(1, 10, 1);                 // neg
      else 
        bit32Pack(0, 10, 1);                 // pos          
      bit32Pack(Abs(pt_sv[2]), 11, 6);      // fragment 2 
      if (pt_sv[2] < 0) 
        bit32Pack(1, 17, 1);                 // neg
      else 
        bit32Pack(0, 17, 1);                 // pos   
      bit32Pack(Abs(pt_sv[3]), 18, 6);       // fragment 3
      if (pt_sv[3] < 0)
        bit32Pack(1, 24, 1);                 // neg
      else 
        bit32Pack(0, 24, 1);                 // pos      
      bit32Pack(Abs(pt_sv[4]), 25, 6);       // fragment 4 
      if (pt_sv[4] < 0)
        bit32Pack(1, 31, 1);                 // neg
      else 
        bit32Pack(0, 31, 1);                 // pos  
        
      uint8_t sv_num = sv_count % 4;

      FrSkyPort::PushToEmptyRow(msg_id, sv_num + 1);  

      #if defined Frs_Debug_All || defined Frs_Debug_Servo
        PrintFrPeriod(0);  
        Debug.print("Passthru out Servo_Raw 0x50F1: ");  
        Debug.print(" sv_chcnt="); Debug.print(sv_chcnt); 
        Debug.print(" sv_count="); Debug.print(sv_count); 
        Debug.print(" chunk="); Debug.print(chunk);
        Debug.print(" pt_sv1="); Debug.print(pt_sv[1]);
        Debug.print(" pt_sv2="); Debug.print(pt_sv[2]);
        Debug.print(" pt_sv3="); Debug.print(pt_sv[3]);   
        Debug.print(" pt_sv4="); Debug.print(pt_sv[4]); 
        Debug.print(" pt_payload="); Debug.print(pt_payload);  Debug.print(" "); 
        FrSkyPort::PrintPayload(passthru);
        Debug.println();             
      #endif

      sv_count += 4; 
    }
    //===================================================================   
    
    void FrSkyPort::Push_VFR_Hud_50F2(uint16_t msg_id) {
      pt_payload = 0;
  
      pt_air_spd = ap74_air_spd * 10;      // from #74  m/s to dm/s
      pt_throt = ap74_throt;               // 0 - 100%
      pt_bar_alt = ap74_amsl * 10;      // m to dm

      #if defined Frs_Debug_All || defined Frs_Debug_Hud
        PrintFrPeriod(0);  
        Debug.print("Passthru out Hud 0x50F2: ");   
        Debug.print(" pt_air_spd="); Debug.print(pt_air_spd);
        Debug.print(" pt_throt=");   Debug.print(pt_throt);
        Debug.print(" pt_bar_alt="); Debug.print(pt_bar_alt);
        Debug.print(" pt_payload="); Debug.print(pt_payload);  Debug.print(" "); 
        FrSkyPort::PrintPayload(passthru);
        Debug.println();             
      #endif
  
      pt_air_spd = prep_number(roundf(pt_air_spd), 2, 1);  
      bit32Pack(pt_air_spd, 0, 8);    

      bit32Pack(pt_throt, 8, 7);

      pt_bar_alt =  prep_number(roundf(pt_bar_alt), 3, 2);
      bit32Pack(pt_bar_alt, 15, 12);
      if (pt_bar_alt < 0)
        bit32Pack(1, 27, 1);  
      else
       bit32Pack(0, 27, 1); 
    
      FrSkyPort::PushToEmptyRow(msg_id, 1); 
        
    }
    //===================================================================  
    void FrSkyPort::Push_Wind_Estimate_50F3(uint16_t msg_id) {
      pt_payload = 0;
    }
    //===================================================================
               
    void FrSkyPort::Push_Rssi_F101(uint16_t msg_id) {          // msg_id 0xF101 RSSI tell LUA script in Taranis we are connected
      pt_payload = 0;
  
      if (rssiGood)
        pt_rssi = ap_rssi;            // always %
      else
        pt_rssi = 254;     // We may have a connection but don't yet know how strong. Prevents spurious "Telemetry lost" announcement
        
      #ifdef RSSI_Override   // dummy rssi override for debugging
        pt_rssi = RSSI_Override;
      #endif

      if(pt_rssi < 1){    // Patch from hasi123 
        pt_rssi = 69;
      }

      bit32Pack(pt_rssi ,0, 32);

      #if defined Frs_Debug_All || defined Frs_Debug_Rssi
        PrintFrPeriod(0);    
        Debug.print("Passthru out Rssi 0x5F101: ");   
        Debug.print(" pt_rssi="); Debug.print(pt_rssi);
        Debug.print(" pt_payload="); Debug.print(pt_payload);  Debug.print(" "); 
        FrSkyPort::PrintPayload(passthru);
        Debug.println();             
      #endif

      FrSkyPort::PushToEmptyRow(msg_id, 1); 
    }
    //=================================================================================================  

    void FrSkyPort::Printbyte(byte b, char delimiter) {
      if (b == 0x7E)  {//             || (b == 0x10)  || (b == 0x32)) {
        Debug.println();
      } 
      if (b<=0xf) Debug.print("0");
      Debug.print(b,HEX);
      Debug.write(delimiter);
    }
         
    //===================================================================  
    
    uint16_t FrSkyPort::MatchWaitingParamRequests(char * paramid) {
      for (int i = 0 ; i < mt20_rows ; i++ ) {  // try to find match on waiting param requests
    //    Debug.printf("MatchWaiting. i=%d  mt20_rows=%d  inuse=%d param_id=%s paramid=%s \n", i, mt20_rows, mt20[i].inuse, mt20[i].param_id,  ap22_param_id);

        bool paramsequal = (strcmp (mt20[i].param_id, paramid) == 0);
        if ( (mt20[i].inuse) && paramsequal ) {
          return i;             
         }
      }
      return 0xffff;  // this mean no match  
    }  
    //===================================================================  

    void FrSkyPort::PrintPayload(msg_class_t msg_class)  {
      uint8_t *bytes = 0;
      uint8_t sz = 0;
      if (msg_class == passthru) {
        Debug.print(" passthru payload ");
        bytes = (uint8_t*)&pt_payload;         // cast to bytes
        sz = 4;   
      } else
      if (msg_class == mavlite) {
        Debug.print(" MavLite payload ");   
        bytes = (uint8_t*)&mtf_payload;           // cast to bytes
        sz = 6;
      } 
   
      for (int i = 0 ; i < sz ; i++) {
        Printbyte(bytes[i], 0);     
      }
      Debug.print("\t");
      for (int i = 0 ; i <sz ; i++) {
         if ((bytes[i] > 31) && (bytes[i] < 127)) Debug.write(bytes[i]);  
      }
      Debug.print("\t");
    } 
    //===================================================================  

    void FrSkyPort::PrintMavLiteUplink() {
  
        if (frbuf[3] == 0) Debug.println();  // seq == 0       
        PrintbyteNon(frbuf[0]);  // 0x7E
        Debug.print(" ");
        PrintbyteNon(frbuf[1]);  // 0x0D
        Debug.print(" ");
        PrintbyteNon(frbuf[2]);  // 0x30 
        Debug.print("\t");
        PrintbyteNon(frbuf[3]);  // seq
        Debug.print(" ");       
        for (int i = 4 ; i <= 8 ; i++ ) {
          PrintbyteNon(frbuf[i]);
        }
        Debug.print(" | ");
        PrintbyteNon(frbuf[9]);  // ?

        Debug.print("\t");
        
        PrintbyteNon(frbuf[3]);  // seq

        if (frbuf[3] == 0)  {    // if seq == 0
          Debug.print("  ");
          PrintbyteNon(frbuf[4]);   // length
          PrintbyteNon(frbuf[5]);   // msg_id 
          Debug.print(" [");
          for (int i = 6 ; i <= 8 ; i++ ) {  
            if ((frbuf[i] > 31) && (frbuf[i] < 127)) Debug.write(frbuf[i]);   // print ascii
          }
        } else {                  // seq > 0
          Debug.print(" [");
          for (int i = 4 ; i <= 8 ; i++ ) {  
           if ((frbuf[i] > 31) && (frbuf[i] < 127)) Debug.write(frbuf[i]);   // print ascii
          }       
        }
        Debug.print("] ");
        PrintbyteNon(frbuf[9]);  // ?
        Debug.println();
    }
    //===================================================================  
 
    
#endif  // end of FrSky port support


      
           
