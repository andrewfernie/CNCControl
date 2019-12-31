/* in all my programmes i marked all global variables with a g_, all local with l_  and all parameter from functions with p_ */
/* this variable is used in the thread functions for this example */
  uint8_t g_thread_counter = 0; 

/* *********************************** */
/* Thread A  
/* *********************************** */
  void simpleThread_setup(Thread_A)
  {
    // reset counter value
    g_thread_counter = 0;
    // output
    Serial.println(F("Thread A: setup"));   
  }  

  boolean simpleThread_loop(Thread_A)
  {
    // output counter value
    Serial.print(F("Thread A Counter: "));
    Serial.println(g_thread_counter);
    
    // increment counter    
    g_thread_counter++;
    
    // check whether the counter has reached the value of 5
    if(g_thread_counter > 5) {
      // start thread B with restart, or with start and a reset     
      simpleThread_restart(Thread_B);
    }    
  }
/* *********************************** */
/* Thread B  
/* *********************************** */   
  void simpleThread_setup(Thread_B)
  {      
    // output
    Serial.println(F("Thread B: setup"));
    g_thread_counter = 0;    
  }
 
  void simpleThread_stable(Thread_B)
  {
    // Hier können z.B: Ausgänge zurückgesetzt werden
    //
    // Falls der Thread eine Alarmfunktion darstellt, kann der 
    // Alarm, beim stoppen auch zurückgesetzt werden
    //
    // Oder falls ein Blinklicht realisiert wird, kann hier beim 
    // Ausschalten das Blinklicht stabild auf aus gesetzt werden
    Serial.println(F("Thread B: stable state reached"));  
  } 

  boolean simpleThread_loop(Thread_B)
  {
    // output  
    Serial.print(F("Thread B Counter: "));
    Serial.println(g_thread_counter);
    
    // increment counter  
    g_thread_counter++;
    
    // check whether the counter has reached the value of 3
    if(g_thread_counter > 3) {      
      // output
      Serial.println(F("Thread B: stopped"));
      // reset counter for new count in thread A
      g_thread_counter = 0; 
   
      // stopt this thread with in Stable state
      simpleThread_stopStable(Thread_B);   
    }   
  }




   



