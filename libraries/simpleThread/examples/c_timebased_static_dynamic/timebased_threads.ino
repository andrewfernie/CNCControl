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
   
      // stopt this thread
      simpleThread_stop(Thread_B);   
    }   
  }

/* *********************************** */
/* Thread C  
/* *********************************** */
  void simpleThread_setup(Thread_C)
  {    
    // output
    Serial.println(F("Thread C: started"));        
  }
 
  boolean simpleThread_loop(Thread_C)
  {
    // Auslesen der aktuellen Loop Time
    unsigned long time = simpleThread_dynamic_getLoopTime(Thread_C);
    
    // Ausgabe der Zeit
    Serial.print(F("Thread C: "));  
    Serial.print(time);
    Serial.println("ms befor Thread is called again");
    
    // Wenn Loop Time kleiner als 2 ms Thread zurücksetzen
    if(time < 2) {
      // Thread Zeit auf Default Wert zurücksetzen
      simpleThread_dynamic_setDefaultTime(Thread_C);      
    } 
    else 
    {   
      // halbiert die aktuelle Loop Time und setzt eine neue         
      simpleThread_dynamic_setLoopTime(Thread_C, (time/2));
    }          
  }




   



