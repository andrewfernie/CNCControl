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


   



