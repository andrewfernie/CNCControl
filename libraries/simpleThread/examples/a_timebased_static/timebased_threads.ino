/* in all my programmes i marked all global variables with a g_, all local with l_  and all parameter from functions with p_ */
/* this variable is used in the thread functions for this example */
  uint8_t g_thread_counter = 0; 

/* 
 ************************************* 
 * Thread A  
 ************************************* 
 */
  void simpleThread_setup(Loop_1)
  {
    // reset counter value
    g_thread_counter = 0;
    // output
    Serial.println(F("Loop 1: setup"));   
  }  

  boolean simpleThread_loop(Loop_1)
  {
    // output counter value
    Serial.print(F("Loop 1 Counter: "));
    Serial.println(g_thread_counter);
    
    // increment counter    
    g_thread_counter++;
    
    // check whether the counter has reached the value of 5
    if(g_thread_counter > 5) {
      // start thread B with restart, or with start and a reset     
      simpleThread_restart(Loop_2);
    }
    return false;    
  }
/* 
 ************************************* 
 * Thread B  
 ************************************* 
 */ 

  void simpleThread_setup(Loop_2)
  {      
    // output
    Serial.println(F("Loop 2: setup"));
    g_thread_counter = 0;    
  } 

  boolean simpleThread_loop(Loop_2)
  {
    // output  
    Serial.print(F("Loop 2 Counter: "));
    Serial.println(g_thread_counter);
    
    // increment counter  
    g_thread_counter++;
    
    // check whether the counter has reached the value of 3
    if(g_thread_counter > 3) {      
      // output
      Serial.println(F("Loop 2: stopped"));
      // reset counter for new count in thread A
      g_thread_counter = 0; 
   
      // stopt this thread
      simpleThread_stop(Loop_2);   
    } 
    return false;  
  }


 void simpleThread_setup(Loop_3)
 {     
 }
 void simpleThread_stable(Loop_3)
 { 
   Serial.println("Loop 3 is stopped stable");   
 }
 boolean simpleThread_loop(Loop_3)
 {     
 }
 

 

 



   



