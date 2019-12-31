/* ******************************************************************** 
 * Example: static timebased threads
 * ******************************************************************** 
 * Description: (german)
 *
 * In diesem Beispiel werden drei zeitbasierte statische Threads angelegt. 
 * Sie besitzen eine feste Zeit. Loop_1 wird jede Sekunde aufgerufen,
 * Loop_2 alle 50 Microsekunden und Loop_3 wird in einem stabilen Zustand gesetzt.
 * 
 * Zu beginn wird nur Loop_1 gestartet. In dieser Schleife wird ein Zähler bis
 * zum Wert 5 hochgezählt. Dann wird Loop_2 aus Loop_1 gestartet.
 *
 * Loop_2 arbeitet mit dem gleichen Zähler. Im Setup von Loop_2 wird der 
 * Zähler zurückgesetzt, anschließend zählt Loop_2 bis 3 und 
 * stoppt sich selbst. Dies geschiet theoretisch in 150 micro Sekunden. Die serielle
 * Schnittstelle bremst hier den Schleifendurchlauf, sodass die Ausführungszeit
 * in real größer ist. Wenn Loop 2 durchgelaufen ist, stoppt sich der Thread selbst. 
 *
 * Wenn die Ausführungszeit von Loop_2 größer als die von Loop_1 wäre, würde Loop_1 
 * die Schleife stören. Dies ist in diesem Beispiel aber nicht der fall.
 *  
 * Damit Threads sich gegenseitig nicht stören muss die Priorität geschickt gewählt 
 * werden
 *
 * In diesem Beispiel wäre es z.B besser Loop_2 mit _sT_P2 zu belegen anstatt mit _sT_P3
 */
#include <simpleThread.h>
  
/* 
 * drei Threads anlegen 
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
  simpleThread_new_timebased_static  (_sT_P2  , _sT_millis, 1000    , _sT_start  , Loop_1); 
  simpleThread_new_timebased_static  (_sT_P3  , _sT_micros, 50      , _sT_stop   , Loop_2);
  simpleThread_new_timebased_static  (_sT_P1  , _sT_millis, 50      , _sT_stable , Loop_3);
  
 


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
