/* *********************************** */
/* Thread D  
/* *********************************** */
  void simpleThread_setup(Thread_D)
  {
    Serial.println(F("Thread D setup"));    
  }
  boolean simpleThread_loop(Thread_D)
  {
    Serial.println(F("Thread D is called"));
    Serial.println(F("Thread C is stopped from D"));  
    simpleThread_stop(Thread_C);
    simpleThread_call(Thread_E);      
  }
  
/* *********************************** */
/* Thread E  
/* *********************************** */
  void simpleThread_setup(Thread_E)
  {
    Serial.println(F("Thread E setup"));       
  }
  boolean simpleThread_loop(Thread_E)
  {
    simpleThread_restart(Thread_C);
    Serial.println(F("Thread E called"));
    Serial.println(F("Thread C is restarted from E"));    
  }
  
  
/* *********************************** */
/* Thread Y  
/* *********************************** */
  void simpleThread_setup(Thread_Y)
  {
    Serial.println(F("Thread E setup"));       
  }
  boolean simpleThread_loop(Thread_Y)
  {
    if(simpleThread_signal_get(signal_from_thread_x_to_y) == true) 
    {    
      Serial.println(F("Thread Y: Signal X get"));
      simpleThread_signal_reset(signal_from_thread_x_to_y);
      simpleThread_signal_set(signal_from_thread_y_to_x);
    }  
  }
