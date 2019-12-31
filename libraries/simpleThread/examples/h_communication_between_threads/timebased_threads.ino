/* in all my programmes i marked all global variables with a g_, all local with l_  and all parameter from functions with p_ */
/* this variable is used in the thread functions for this example */
  uint8_t g_thread_counter = 0; 

/* *********************************** */
/* Thread A  
/* *********************************** */
/* This thread is called once per second. When the thread counter reached the value 5, thead B is starting */
/* thread A setup */
  void simpleThread_setup(Thread_A)
  {
    // reset counter value
    g_thread_counter = 0;
    // output
    Serial.println(F("Thread A: setup"));   
  }
/* thread A loop */
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
/* This thread is called all 50 ms. When the thread counter reached the value 3, this thread stopps itself */
/* thread B setup */
  void simpleThread_setup(Thread_B)
  {
    // reset counter für eine neue Zählung
    g_thread_counter = 0;
    // output
    Serial.println(F("Thread B: setup"));    
  }
/* thread B loop */
  boolean simpleThread_loop(Thread_B)
  {
    // output  
    Serial.print(F("Thread B Counter: "));
    Serial.println(g_thread_counter);
    
    // increment counter  
    g_thread_counter++;
    
    // check whether the counter has reached the value of 3
    if(g_thread_counter > 3) {
      // stopt this thread
      simpleThread_stop(Thread_B);
      // output
      Serial.println(F("Thread B: stopped"));
      // reset counter for new count in thread A
      g_thread_counter = 0;    
    }
  }


/* *********************************** */
/* Thread C  
/* *********************************** */
/* This thread has a dynamc thread time. Every call the loop time is halved until the time is < 2 ms, then it begins again */
/* thread C setup */
  void simpleThread_setup(Thread_C)
  {
    // init loop time
    simpleThread_dynamic_setDefaultTime(Thread_C);
    // output
    Serial.println(F("Thread C: started"));        
  }
/* thread C loop */  
  boolean simpleThread_loop(Thread_C)
  {
    // read the current loop time
    unsigned long time = simpleThread_dynamic_getLoopTime(Thread_C);
    
    // output current looop time
    Serial.print(F("Thread C: dyn time "));  
    Serial.println(time);
    
    // check end condition
    if(time < 2) {
      // start this thread with setup new (restart)
      simpleThread_restart(Thread_C);
      // start thread D 
      simpleThread_event_start(Thread_D);      
    } else {
      // halved the loop time
      time /= 2;
      // set the new loop time
      simpleThread_dynamic_setLoopTime(Thread_C, time);      
    }  
  }
  
/* *********************************** */
/* Thread X  
/* *********************************** */
  void simpleThread_setup(Thread_X)
  {          
  }
  boolean simpleThread_loop(Thread_X)
  {
    if(simpleThread_signal_get(signal_from_thread_y_to_x) == true) 
    {    
      Serial.println(F("Thread X: get Signal Y"));
      simpleThread_signal_reset(signal_from_thread_y_to_x);
    }
    simpleThread_signal_set(signal_from_thread_x_to_y);
    simpleThread_event_start(Thread_Y);    
  }
  



   



