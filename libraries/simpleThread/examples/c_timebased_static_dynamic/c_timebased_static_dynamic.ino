/* ******************************************************************** */
/* Example: static and dynamic timebased threads
/* ******************************************************************** */
/* Description: (german)
 *
 * Eine Mischung aus Beispiel a und b, soll nur darstellen, das auch 
 * unterscheidliche Threads angelegt werden können
 */
#include <simpleThread.h>
   
/* 
 * einen Thread anlegen 
 *
 * werden hier mehr Threads angelegt als in wirklich kein existieren kommt eine Fehlermeldung 
 * die z.B. so aussehen könnte
 * a_timebased_static.ino: In function 'void setup()':
 * a_timebased_static:68: error: 'simpleThread_setupInit_3' was not declared in this scope
 *
 * wenn zu wenig threads angegeben werden, werden die threads mit der größten Priorität nicht
 * gestartet
 *
 */   
  #define _sT_cnt         _sT_cnt_3
 
  simpleThread_init(_sT_cnt);  
/* 
 * neue Threads anlegen
 * - Parameter für "timebased_static" und "timebased dynamic" 
 *    Param 1: Priorität, darf nicht doppelt mit den anderen Threads vergeben werden  
 *    Param 2: Zeiteinheit  
 *            _sT_millis 
 *            _sT_micros
 *    Param 3: Zeit nach der der Thread wieder aufgerufen wird
 *    Param 4: Initialisierung des Thread:
 *            _sT_start:     thread wird direkt gestartet 
 *            _sT_stop:      thread wird zuanfang nicht gestartet
 *            _sT_stable:    thread wird in einem stabilen zustand geladen
 *    Param 5: Thread Bezeichnung, mit dieser müssen auch die Thread Funktionen erstellt werden
 *
 * - Parameter für "eventbased"
 *    Param 1: Priorität, darf nicht doppelt mit den anderen Threads vergeben werden
 *    Param 2: Thread Bezeichnung, mit dieser müssen auch die Thread Funktionen erstellt werden
 *
 * Die Priorität der Threads muss nicht der Reihenfolge nach festgelegt werden
 *
 */
  simpleThread_new_timebased_static (_sT_P1  , _sT_millis, 1000    , _sT_start  , Thread_A); 
  simpleThread_new_timebased_static (_sT_P2  , _sT_millis, 50      , _sT_start  , Thread_B); 
  simpleThread_new_timebased_dynamic (_sT_P3  , _sT_millis, 5535    , _sT_start  , Thread_C); 


void setup() 
{ 
  /* serial communication for this example */
  Serial.begin(9600); 
  
  /* Threads initialisieren */
  simpleThread_initSetup(_sT_cnt);
}

void loop() {
  /* calls the threads */ 
  simpleThread_run(_sT_priority);
}
