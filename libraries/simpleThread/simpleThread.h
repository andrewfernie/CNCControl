/* ******************************************************************** 
 * Simple Thread Macros
 * ********************************************************************
 * This file contains only macros which creats threads
 *
 * Autor:		Nils Feldkämper
 * Email:		nilsfeld@gmail.com
 * License:		all Free
 * Last edit:	17.05.2013
 

 *
 * The following functions are supported 
 * 
 *  
 * simpleThread_start(thread_name)
 * simpleThread_startAll()
 * simpleThread_startGoup(group_name)
   
 * simpleThread_stop(thread_name)
 * simpleThread_stopAll()
 * simpleThread_stopGroup(group_name) 
 
 * simpleThread_reset(thread_name) 
 * simpleThread_resetAll(thread_name)
 * simpleThread_resetGroup(group_name)
 
 * simpleThread_restart(thread_name)
 * simpleThread_restartAll()
 * simpleThread_restartGroup(group_name)

 * simpleThread_isRun(thread_name)

 * mode: eventbased
 
 * simpleThread_startEvent(thread_name)
 * simpleThread_startEvent_direct(thread_name)
 
 * mode: timebased - dynamic 
 * simpleThread_dynamic_setTime(thread_name, loop time in ms)
 * simpleThread_dynamic_getTim(thread_name)
 * simpleThread_dynamic_setDefault(thread_name)
 
 * signals
 * simpleThread_signal_init(cnt)
 * simpleThread_signal(id, name)
 * simpleThread_setSignal(name)
 * simpleThread_getSignal(name)
 * simpleThread_resetSignal(name)
 
 * initialization: 
 * simpleThread_timebased_static(id, thread_name, loop_time): create a new static thread with a name and a default loop time
 * simpleThread_timebased_dynamic(id, thread_name, loop_time on start): create a new dynamic thread with a name and a default loop time
 * simpleThread_eventbased(id, thread_name)
 * simpleThread_PTR(thread_name): containes the function pointer
 * simpleThread_loop(thread_name): thread loop, same feature as in the arduino software 
 * simpleThread_setup(thread_name): thread setup, same feature as in the arduino software *
 * simpleThread_run(mode): this macro must be called from the orginal arduino loop function 
 */

#ifndef TYPEDEF_FUNCPTR
#define TYPEDEF_FUNCPTR
	typedef void (* FuncPtr) ();
#endif


#ifndef SIMPLETHREAD_h
#define SIMPLETHREAD_h

#include "arduino.h"

// define loop modes
#define _sT_no_priority		0
#define _sT_priority		1

#define _sT_millis          millis
#define _sT_micros          micros

#define _sT_stop			0
#define _sT_start			1
#define _sT_stable			2


//non thread id
#define     _simpleThread_default_id				255

//init for threads
	// macro: setup function
	#define simpleThread_setup(name)\
		simpleThread_setup_##name(void)
	// macro: loop function
	#define simpleThread_loop(name)\
		simpleThread_loop_##name(void)
	// macro: stableState function
	#define simpleThread_stable(name)\
		simpleThread_stable_##name(void)




// init thread system
	// macro: creates the simpleThread name pointer on a function
	#define simpleThread(name)\
		simpleThread_function_##name
	// macro: creates the simpleThread managemand variables
	#define simpleThread_init(cnt)\
		FuncPtr g_simpleThread_priority[cnt+1];\
		const PROGMEM uint8_t g_simpleThread_cnt = cnt+1;\
		uint8_t g_simpleThread_start_stop[((cnt+1)/7)+1];\
		uint8_t g_simpleThread_reset[((cnt+1)/7)+1];\
		uint8_t g_simpleThread_loop_status = true;\
		unsigned long g_simpleThread_timer[(cnt+1)];	
	// macro: loop function  
	#define simpleThread_run(mode)\
		for(uint8_t l_simpleThread_i = 0; l_simpleThread_i<g_simpleThread_cnt;l_simpleThread_i++) {\
			g_simpleThread_priority[(l_simpleThread_i)]();\
			if (mode == true && g_simpleThread_loop_status == false)\
			{\
				g_simpleThread_loop_status = true;\
				break;\
			}\
		}



	



// thread create
	// macro: help to create a new thread: generate weak functions
	#define simpleThread_help_new_thread(id, name, status)\
		const PROGMEM uint8_t g_simpleThread_id__##name  = id;\
		void simpleThread_setup_##name(void);\
		boolean simpleThread_loop_##name(void);\
		void simpleThread_stable_##name(void);\
		void simpleThread_function_##name(void);\
		void simpleThread_setupInit_##id(void){ g_simpleThread_priority[id] = simpleThread_function_##name; if(status == 1) {simpleThread_start(name);} else if(status == 2) { simpleThread_stopStable(name); }}
	// macro: create a new thread with static times
	#define simpleThread_new_timebased_static(id, timebase, init_time, status, name)\
		simpleThread_help_new_thread(id, name, status);\
		simpleThread_THREAD_FUNCTION_TIME_BASED(name, init_time, timebase);
	// macro: create a new thread with dynamc times 
	#define simpleThread_new_timebased_dynamic(id, timebase, init_time, status, name)\
		simpleThread_help_new_thread(id, name, status);\
		const PROGMEM uint32_t   _simpleThread_time_default__##name  = init_time;\
		unsigned long g_simpleThread_dynTime_##name = init_time;\
		simpleThread_THREAD_FUNCTION_TIME_BASED(name, g_simpleThread_dynTime_##name, timebase);
	// macro: create a event based thread
	#define simpleThread_new_eventbased(id, name)\
		simpleThread_help_new_thread(id, name, false);\
		simpleThread_THREAD_FUNCTION_EVENT_BASED(name);
		



// thread control
	// macro: thread start single
	#define simpleThread_start(name)\
		bitWrite(g_simpleThread_start_stop[g_simpleThread_id__##name/7], g_simpleThread_id__##name%7, true);
	// macro: thread stop
	#define simpleThread_stop(name)\
		bitWrite(g_simpleThread_start_stop[g_simpleThread_id__##name/7], g_simpleThread_id__##name%7, false); 
	// macro: thread stop stable => calls a function at the end
	#define simpleThread_stopStable(name)\
		simpleThread_stop(name);\
		simpleThread_stable_##name();
	// macro: thread reset
	#define simpleThread_reset(name)\
		bitWrite(g_simpleThread_reset[g_simpleThread_id__##name/7], g_simpleThread_id__##name%7, false); \
		g_simpleThread_timer[g_simpleThread_id__##name] = 0;
	//macro: thread reStart 
	#define simpleThread_restart(name)\
		simpleThread_reset(name);\
		simpleThread_start(name);

// thread control event
	#define simpleThread_event_start(name)\
		simpleThread_start(name);
	#define simpleThread_event_reset(name)\
		simpleThread_reset(name);
	#define simpleThread_event_restart(name)\
		simpleThread_restart(name);


//all
	// macro: thread start all
	#define simpleThread_all_start()\
		for(uint8_t l_simpleThread_i = 0; l_simpleThread_i<g_simpleThread_cnt;l_simpleThread_i++) { \
			bitWrite(g_simpleThread_start_stop[l_simpleThread_i/7], l_simpleThread_i%7, true); \
		}
	// macro: thread stop all
	#define simpleThread_all_stop()\
		for(uint8_t l_simpleThread_i = 0; l_simpleThread_i<g_simpleThread_cnt;l_simpleThread_i++) { \
			bitWrite(g_simpleThread_start_stop[l_simpleThread_i/7], l_simpleThread_i%7, false); \
		}
	// macro: thread reset all
	#define simpleThread_all_reset()\
		for(uint8_t l_simpleThread_i = 0; l_simpleThread_i<g_simpleThread_cnt;l_simpleThread_i++) { \
			bitWrite(g_simpleThread_reset[l_simpleThread_i/7], l_simpleThread_i%7, false); \
		}
	//macro: thread reStart all 
	#define simpleThread_all_restart(name)\
		simpleThread_all_reset();\
		simpleThread_all_start();



//group
	// macro: add group elements
	#define simpleThread_group(name)\
		{g_simpleThread_id__##name/7, g_simpleThread_id__##name%7} 
	// macro: create a new group
	#define simpleThread_group_init(name, thread_cnt)\
		uint8_t g_simpleThread_group__##name##_cnt = thread_cnt;\
		uint8_t g_simpleThread_group__##name[thread_cnt][2] = 
	// macro: thread start group
	#define simpleThread_group_start(group_name)\
		for(uint8_t l_simpleThread_i = 0; l_simpleThread_i<(g_simpleThread_group__##group_name##_cnt);l_simpleThread_i++) {\
			bitWrite(g_simpleThread_start_stop[g_simpleThread_group__##group_name[l_simpleThread_i][0]], g_simpleThread_group__##group_name[l_simpleThread_i][1], true);\
		}
	// macro: thread stop group
	#define simpleThread_group_stop(group_name)\
		for(uint8_t l_simpleThread_i = 0; l_simpleThread_i<(g_simpleThread_group__##group_name##_cnt);l_simpleThread_i++) {\
			bitWrite(g_simpleThread_start_stop[g_simpleThread_group__##group_name[l_simpleThread_i][0]], g_simpleThread_group__##group_name[l_simpleThread_i][1], false);\
		}
	// macro: thread reset group
	#define simpleThread_group_reset(group_name)\
		for(uint8_t l_simpleThread_i = 0; l_simpleThread_i<(g_simpleThread_group__##group_name##_cnt);l_simpleThread_i++) {\
			bitWrite(g_simpleThread_reset[g_simpleThread_group__##group_name[l_simpleThread_i][0]], g_simpleThread_group__##group_name[l_simpleThread_i][1], false);\
		}
	// macro: thread start group
	#define simpleThread_group_restart(group_name)\
		simpleThread_group_reset(group_name);\
		simpleThread_group_start(group_name);
	


//signals
	// macro:
	#define simpleThread_signal(id, name)\
		PROGMEM prog_uint8_t  g_simpleSignal_id__##name  = id;		
	// macro: creates the simpleThread managemand variables
	#define simpleThread_signal_init(cnt)\
		uint8_t g_simpleSignal_status[(cnt/7)+1];
	// macro:
	#define simpleThread_signal_set(name)\
		bitWrite(g_simpleSignal_status[g_simpleSignal_id__##name/7], g_simpleSignal_id__##name%7, true);
	// macro:	
	#define simpleThread_signal_get(name)\
		bitRead(g_simpleSignal_status[g_simpleSignal_id__##name/7], g_simpleSignal_id__##name%7)
	// macro:
	#define simpleThread_signal_reset(name)\
		bitWrite(g_simpleSignal_status[g_simpleSignal_id__##name/7], g_simpleSignal_id__##name%7, false);


//is run
	//macro: thread is running ? 
	#define simpleThread_isRun(name)\
		bitRead(g_simpleThread_start_stop[g_simpleThread_id__##name/7], g_simpleThread_id__##name%7)
	
	
// dynamic times	
	//macro: edit thread loop time
	#define simpleThread_dynamic_setLoopTime(name, time)\
		g_simpleThread_dynTime_##name = time;
	//macro: get thread loop time
	#define simpleThread_dynamic_getLoopTime(name)\
		g_simpleThread_dynTime_##name	
	// macro: reset a dynamic thread to default settings
	#define simpleThread_dynamic_setDefaultTime(name)\
		g_simpleThread_dynTime_##name =_simpleThread_time_default__##name;
	// macro: restart dynamic thread time
	#define simpleThread_dynamic_restartTimer(name)\
		g_simpleThread_timer[g_simpleThread_id__##name] = (millis() + g_simpleThread_dynTime_##name);
	// macro: setTime 0
	#define simpleThread_dynamic_timeToZero(name)\
		g_simpleThread_timer[g_simpleThread_id__##name] = 0;





//direct call
	// macro: call a thread
	#define simpleThread_call(name)\
		simpleThread_function_##name();
	// macro: call a thread loop function
	#define simpleThread_call_loop(name)\
		simpleThread_loop_##name();
	// macro: call a thread setup function
	#define simpleThread_call_setup(name)\
		simpleThread_setup_##name();
	// macro: call a thread stable function
	#define simpleThread_call_stable(name)\
		simpleThread_stable_##name();		


	
	


//help macros
	// macro: thread timer with return
	#define simpleThread_THREAD_TIMER(timer_var, wait_time, timebase)\
		if(!((timebase() - timer_var) >= wait_time)) {  return; }\
		timer_var = timebase();

	//macro: thread is running with return !
	#define simpleThread_THREAD_isRun(name)\
	if(!bitRead(g_simpleThread_start_stop[g_simpleThread_id__##name/7], g_simpleThread_id__##name%7)) { return; } 

	//macro: reset the reset bin from a thread
	#define simpleThread_UNSET_reset(name)\
		bitWrite(g_simpleThread_reset[g_simpleThread_id__##name/7], g_simpleThread_id__##name%7, true)

	//macro: get the reset bit from a thread
	#define simpleThread_GET_reset(name)\
		bitRead(g_simpleThread_reset[g_simpleThread_id__##name/7], g_simpleThread_id__##name%7)

	//macro: create the thread setup function 
	#define simpleThread_THREAD_FUNCTION_TIME_BASED(name, time, timebase)\
		void simpleThread_function_##name(void)\
		{\
			simpleThread_THREAD_isRun(name);\
			simpleThread_THREAD_TIMER(g_simpleThread_timer[g_simpleThread_id__##name], time, timebase);\
			if(simpleThread_GET_reset(name) == false) {\
				simpleThread_UNSET_reset(name);\
				simpleThread_setup_##name();\
			}\
			g_simpleThread_loop_status = (simpleThread_loop_##name()) ? true : false;\
		}

	//macro: create the thread setup function 
	#define simpleThread_THREAD_FUNCTION_EVENT_BASED(name)\
		void simpleThread_function_##name(void)\
		{\
			simpleThread_THREAD_isRun(name);\
			if(simpleThread_GET_reset(name) == true) {\
				simpleThread_UNSET_reset(name);\
				simpleThread_setup_##name();\
			}\
			g_simpleThread_loop_status = (simpleThread_loop_##name()) ? true : false;\
			simpleThread_stop(name);\
		}

	#define simpleThread_help_setup(id)   simpleThread_setupInit_##id();


	#define simpleThread_help_setup_repeat_255() simpleThread_help_setup_repeat_254() simpleThread_help_setup(255)
	#define simpleThread_help_setup_repeat_254() simpleThread_help_setup_repeat_253() simpleThread_help_setup(254)
	#define simpleThread_help_setup_repeat_253() simpleThread_help_setup_repeat_252() simpleThread_help_setup(253)
	#define simpleThread_help_setup_repeat_252() simpleThread_help_setup_repeat_251() simpleThread_help_setup(252)
	#define simpleThread_help_setup_repeat_251() simpleThread_help_setup_repeat_250() simpleThread_help_setup(251)
	#define simpleThread_help_setup_repeat_250() simpleThread_help_setup_repeat_249() simpleThread_help_setup(250)
	#define simpleThread_help_setup_repeat_249() simpleThread_help_setup_repeat_248() simpleThread_help_setup(249)
	#define simpleThread_help_setup_repeat_248() simpleThread_help_setup_repeat_247() simpleThread_help_setup(248)
	#define simpleThread_help_setup_repeat_247() simpleThread_help_setup_repeat_246() simpleThread_help_setup(247)
	#define simpleThread_help_setup_repeat_246() simpleThread_help_setup_repeat_245() simpleThread_help_setup(246)
	#define simpleThread_help_setup_repeat_245() simpleThread_help_setup_repeat_244() simpleThread_help_setup(245)
	#define simpleThread_help_setup_repeat_244() simpleThread_help_setup_repeat_243() simpleThread_help_setup(244)
	#define simpleThread_help_setup_repeat_243() simpleThread_help_setup_repeat_242() simpleThread_help_setup(243)
	#define simpleThread_help_setup_repeat_242() simpleThread_help_setup_repeat_241() simpleThread_help_setup(242)
	#define simpleThread_help_setup_repeat_241() simpleThread_help_setup_repeat_240() simpleThread_help_setup(241)
	#define simpleThread_help_setup_repeat_240() simpleThread_help_setup_repeat_239() simpleThread_help_setup(240)
	#define simpleThread_help_setup_repeat_239() simpleThread_help_setup_repeat_238() simpleThread_help_setup(239)
	#define simpleThread_help_setup_repeat_238() simpleThread_help_setup_repeat_237() simpleThread_help_setup(238)
	#define simpleThread_help_setup_repeat_237() simpleThread_help_setup_repeat_236() simpleThread_help_setup(237)
	#define simpleThread_help_setup_repeat_236() simpleThread_help_setup_repeat_235() simpleThread_help_setup(236)
	#define simpleThread_help_setup_repeat_235() simpleThread_help_setup_repeat_234() simpleThread_help_setup(235)
	#define simpleThread_help_setup_repeat_234() simpleThread_help_setup_repeat_233() simpleThread_help_setup(234)
	#define simpleThread_help_setup_repeat_233() simpleThread_help_setup_repeat_232() simpleThread_help_setup(233)
	#define simpleThread_help_setup_repeat_232() simpleThread_help_setup_repeat_231() simpleThread_help_setup(232)
	#define simpleThread_help_setup_repeat_231() simpleThread_help_setup_repeat_230() simpleThread_help_setup(231)
	#define simpleThread_help_setup_repeat_230() simpleThread_help_setup_repeat_229() simpleThread_help_setup(230)
	#define simpleThread_help_setup_repeat_229() simpleThread_help_setup_repeat_228() simpleThread_help_setup(229)
	#define simpleThread_help_setup_repeat_228() simpleThread_help_setup_repeat_227() simpleThread_help_setup(228)
	#define simpleThread_help_setup_repeat_227() simpleThread_help_setup_repeat_226() simpleThread_help_setup(227)
	#define simpleThread_help_setup_repeat_226() simpleThread_help_setup_repeat_225() simpleThread_help_setup(226)
	#define simpleThread_help_setup_repeat_225() simpleThread_help_setup_repeat_224() simpleThread_help_setup(225)
	#define simpleThread_help_setup_repeat_224() simpleThread_help_setup_repeat_223() simpleThread_help_setup(224)
	#define simpleThread_help_setup_repeat_223() simpleThread_help_setup_repeat_222() simpleThread_help_setup(223)
	#define simpleThread_help_setup_repeat_222() simpleThread_help_setup_repeat_221() simpleThread_help_setup(222)
	#define simpleThread_help_setup_repeat_221() simpleThread_help_setup_repeat_220() simpleThread_help_setup(221)
	#define simpleThread_help_setup_repeat_220() simpleThread_help_setup_repeat_219() simpleThread_help_setup(220)
	#define simpleThread_help_setup_repeat_219() simpleThread_help_setup_repeat_218() simpleThread_help_setup(219)
	#define simpleThread_help_setup_repeat_218() simpleThread_help_setup_repeat_217() simpleThread_help_setup(218)
	#define simpleThread_help_setup_repeat_217() simpleThread_help_setup_repeat_216() simpleThread_help_setup(217)
	#define simpleThread_help_setup_repeat_216() simpleThread_help_setup_repeat_215() simpleThread_help_setup(216)
	#define simpleThread_help_setup_repeat_215() simpleThread_help_setup_repeat_214() simpleThread_help_setup(215)
	#define simpleThread_help_setup_repeat_214() simpleThread_help_setup_repeat_213() simpleThread_help_setup(214)
	#define simpleThread_help_setup_repeat_213() simpleThread_help_setup_repeat_212() simpleThread_help_setup(213)
	#define simpleThread_help_setup_repeat_212() simpleThread_help_setup_repeat_211() simpleThread_help_setup(212)
	#define simpleThread_help_setup_repeat_211() simpleThread_help_setup_repeat_210() simpleThread_help_setup(211)
	#define simpleThread_help_setup_repeat_210() simpleThread_help_setup_repeat_209() simpleThread_help_setup(210)
	#define simpleThread_help_setup_repeat_209() simpleThread_help_setup_repeat_208() simpleThread_help_setup(209)
	#define simpleThread_help_setup_repeat_208() simpleThread_help_setup_repeat_207() simpleThread_help_setup(208)
	#define simpleThread_help_setup_repeat_207() simpleThread_help_setup_repeat_206() simpleThread_help_setup(207)
	#define simpleThread_help_setup_repeat_206() simpleThread_help_setup_repeat_205() simpleThread_help_setup(206)
	#define simpleThread_help_setup_repeat_205() simpleThread_help_setup_repeat_204() simpleThread_help_setup(205)
	#define simpleThread_help_setup_repeat_204() simpleThread_help_setup_repeat_203() simpleThread_help_setup(204)
	#define simpleThread_help_setup_repeat_203() simpleThread_help_setup_repeat_202() simpleThread_help_setup(203)
	#define simpleThread_help_setup_repeat_202() simpleThread_help_setup_repeat_201() simpleThread_help_setup(202)
	#define simpleThread_help_setup_repeat_201() simpleThread_help_setup_repeat_200() simpleThread_help_setup(201)
	#define simpleThread_help_setup_repeat_200() simpleThread_help_setup_repeat_199() simpleThread_help_setup(200)
	#define simpleThread_help_setup_repeat_199() simpleThread_help_setup_repeat_198() simpleThread_help_setup(199)
	#define simpleThread_help_setup_repeat_198() simpleThread_help_setup_repeat_197() simpleThread_help_setup(198)
	#define simpleThread_help_setup_repeat_197() simpleThread_help_setup_repeat_196() simpleThread_help_setup(197)
	#define simpleThread_help_setup_repeat_196() simpleThread_help_setup_repeat_195() simpleThread_help_setup(196)
	#define simpleThread_help_setup_repeat_195() simpleThread_help_setup_repeat_194() simpleThread_help_setup(195)
	#define simpleThread_help_setup_repeat_194() simpleThread_help_setup_repeat_193() simpleThread_help_setup(194)
	#define simpleThread_help_setup_repeat_193() simpleThread_help_setup_repeat_192() simpleThread_help_setup(193)
	#define simpleThread_help_setup_repeat_192() simpleThread_help_setup_repeat_191() simpleThread_help_setup(192)
	#define simpleThread_help_setup_repeat_191() simpleThread_help_setup_repeat_190() simpleThread_help_setup(191)
	#define simpleThread_help_setup_repeat_190() simpleThread_help_setup_repeat_189() simpleThread_help_setup(190)
	#define simpleThread_help_setup_repeat_189() simpleThread_help_setup_repeat_188() simpleThread_help_setup(189)
	#define simpleThread_help_setup_repeat_188() simpleThread_help_setup_repeat_187() simpleThread_help_setup(188)
	#define simpleThread_help_setup_repeat_187() simpleThread_help_setup_repeat_186() simpleThread_help_setup(187)
	#define simpleThread_help_setup_repeat_186() simpleThread_help_setup_repeat_185() simpleThread_help_setup(186)
	#define simpleThread_help_setup_repeat_185() simpleThread_help_setup_repeat_184() simpleThread_help_setup(185)
	#define simpleThread_help_setup_repeat_184() simpleThread_help_setup_repeat_183() simpleThread_help_setup(184)
	#define simpleThread_help_setup_repeat_183() simpleThread_help_setup_repeat_182() simpleThread_help_setup(183)
	#define simpleThread_help_setup_repeat_182() simpleThread_help_setup_repeat_181() simpleThread_help_setup(182)
	#define simpleThread_help_setup_repeat_181() simpleThread_help_setup_repeat_180() simpleThread_help_setup(181)
	#define simpleThread_help_setup_repeat_180() simpleThread_help_setup_repeat_179() simpleThread_help_setup(180)
	#define simpleThread_help_setup_repeat_179() simpleThread_help_setup_repeat_178() simpleThread_help_setup(179)
	#define simpleThread_help_setup_repeat_178() simpleThread_help_setup_repeat_177() simpleThread_help_setup(178)
	#define simpleThread_help_setup_repeat_177() simpleThread_help_setup_repeat_176() simpleThread_help_setup(177)
	#define simpleThread_help_setup_repeat_176() simpleThread_help_setup_repeat_175() simpleThread_help_setup(176)
	#define simpleThread_help_setup_repeat_175() simpleThread_help_setup_repeat_174() simpleThread_help_setup(175)
	#define simpleThread_help_setup_repeat_174() simpleThread_help_setup_repeat_173() simpleThread_help_setup(174)
	#define simpleThread_help_setup_repeat_173() simpleThread_help_setup_repeat_172() simpleThread_help_setup(173)
	#define simpleThread_help_setup_repeat_172() simpleThread_help_setup_repeat_171() simpleThread_help_setup(172)
	#define simpleThread_help_setup_repeat_171() simpleThread_help_setup_repeat_170() simpleThread_help_setup(171)
	#define simpleThread_help_setup_repeat_170() simpleThread_help_setup_repeat_169() simpleThread_help_setup(170)
	#define simpleThread_help_setup_repeat_169() simpleThread_help_setup_repeat_168() simpleThread_help_setup(169)
	#define simpleThread_help_setup_repeat_168() simpleThread_help_setup_repeat_167() simpleThread_help_setup(168)
	#define simpleThread_help_setup_repeat_167() simpleThread_help_setup_repeat_166() simpleThread_help_setup(167)
	#define simpleThread_help_setup_repeat_166() simpleThread_help_setup_repeat_165() simpleThread_help_setup(166)
	#define simpleThread_help_setup_repeat_165() simpleThread_help_setup_repeat_164() simpleThread_help_setup(165)
	#define simpleThread_help_setup_repeat_164() simpleThread_help_setup_repeat_163() simpleThread_help_setup(164)
	#define simpleThread_help_setup_repeat_163() simpleThread_help_setup_repeat_162() simpleThread_help_setup(163)
	#define simpleThread_help_setup_repeat_162() simpleThread_help_setup_repeat_161() simpleThread_help_setup(162)
	#define simpleThread_help_setup_repeat_161() simpleThread_help_setup_repeat_160() simpleThread_help_setup(161)
	#define simpleThread_help_setup_repeat_160() simpleThread_help_setup_repeat_159() simpleThread_help_setup(160)
	#define simpleThread_help_setup_repeat_159() simpleThread_help_setup_repeat_158() simpleThread_help_setup(159)
	#define simpleThread_help_setup_repeat_158() simpleThread_help_setup_repeat_157() simpleThread_help_setup(158)
	#define simpleThread_help_setup_repeat_157() simpleThread_help_setup_repeat_156() simpleThread_help_setup(157)
	#define simpleThread_help_setup_repeat_156() simpleThread_help_setup_repeat_155() simpleThread_help_setup(156)
	#define simpleThread_help_setup_repeat_155() simpleThread_help_setup_repeat_154() simpleThread_help_setup(155)
	#define simpleThread_help_setup_repeat_154() simpleThread_help_setup_repeat_153() simpleThread_help_setup(154)
	#define simpleThread_help_setup_repeat_153() simpleThread_help_setup_repeat_152() simpleThread_help_setup(153)
	#define simpleThread_help_setup_repeat_152() simpleThread_help_setup_repeat_151() simpleThread_help_setup(152)
	#define simpleThread_help_setup_repeat_151() simpleThread_help_setup_repeat_150() simpleThread_help_setup(151)
	#define simpleThread_help_setup_repeat_150() simpleThread_help_setup_repeat_149() simpleThread_help_setup(150)
	#define simpleThread_help_setup_repeat_149() simpleThread_help_setup_repeat_148() simpleThread_help_setup(149)
	#define simpleThread_help_setup_repeat_148() simpleThread_help_setup_repeat_147() simpleThread_help_setup(148)
	#define simpleThread_help_setup_repeat_147() simpleThread_help_setup_repeat_146() simpleThread_help_setup(147)
	#define simpleThread_help_setup_repeat_146() simpleThread_help_setup_repeat_145() simpleThread_help_setup(146)
	#define simpleThread_help_setup_repeat_145() simpleThread_help_setup_repeat_144() simpleThread_help_setup(145)
	#define simpleThread_help_setup_repeat_144() simpleThread_help_setup_repeat_143() simpleThread_help_setup(144)
	#define simpleThread_help_setup_repeat_143() simpleThread_help_setup_repeat_142() simpleThread_help_setup(143)
	#define simpleThread_help_setup_repeat_142() simpleThread_help_setup_repeat_141() simpleThread_help_setup(142)
	#define simpleThread_help_setup_repeat_141() simpleThread_help_setup_repeat_140() simpleThread_help_setup(141)
	#define simpleThread_help_setup_repeat_140() simpleThread_help_setup_repeat_139() simpleThread_help_setup(140)
	#define simpleThread_help_setup_repeat_139() simpleThread_help_setup_repeat_138() simpleThread_help_setup(139)
	#define simpleThread_help_setup_repeat_138() simpleThread_help_setup_repeat_137() simpleThread_help_setup(138)
	#define simpleThread_help_setup_repeat_137() simpleThread_help_setup_repeat_136() simpleThread_help_setup(137)
	#define simpleThread_help_setup_repeat_136() simpleThread_help_setup_repeat_135() simpleThread_help_setup(136)
	#define simpleThread_help_setup_repeat_135() simpleThread_help_setup_repeat_134() simpleThread_help_setup(135)
	#define simpleThread_help_setup_repeat_134() simpleThread_help_setup_repeat_133() simpleThread_help_setup(134)
	#define simpleThread_help_setup_repeat_133() simpleThread_help_setup_repeat_132() simpleThread_help_setup(133)
	#define simpleThread_help_setup_repeat_132() simpleThread_help_setup_repeat_131() simpleThread_help_setup(132)
	#define simpleThread_help_setup_repeat_131() simpleThread_help_setup_repeat_130() simpleThread_help_setup(131)
	#define simpleThread_help_setup_repeat_130() simpleThread_help_setup_repeat_129() simpleThread_help_setup(130)
	#define simpleThread_help_setup_repeat_129() simpleThread_help_setup_repeat_128() simpleThread_help_setup(129)
	#define simpleThread_help_setup_repeat_128() simpleThread_help_setup_repeat_127() simpleThread_help_setup(128)
	#define simpleThread_help_setup_repeat_127() simpleThread_help_setup_repeat_126() simpleThread_help_setup(127)
	#define simpleThread_help_setup_repeat_126() simpleThread_help_setup_repeat_125() simpleThread_help_setup(126)
	#define simpleThread_help_setup_repeat_125() simpleThread_help_setup_repeat_124() simpleThread_help_setup(125)
	#define simpleThread_help_setup_repeat_124() simpleThread_help_setup_repeat_123() simpleThread_help_setup(124)
	#define simpleThread_help_setup_repeat_123() simpleThread_help_setup_repeat_122() simpleThread_help_setup(123)
	#define simpleThread_help_setup_repeat_122() simpleThread_help_setup_repeat_121() simpleThread_help_setup(122)
	#define simpleThread_help_setup_repeat_121() simpleThread_help_setup_repeat_120() simpleThread_help_setup(121)
	#define simpleThread_help_setup_repeat_120() simpleThread_help_setup_repeat_119() simpleThread_help_setup(120)
	#define simpleThread_help_setup_repeat_119() simpleThread_help_setup_repeat_118() simpleThread_help_setup(119)
	#define simpleThread_help_setup_repeat_118() simpleThread_help_setup_repeat_117() simpleThread_help_setup(118)
	#define simpleThread_help_setup_repeat_117() simpleThread_help_setup_repeat_116() simpleThread_help_setup(117)
	#define simpleThread_help_setup_repeat_116() simpleThread_help_setup_repeat_115() simpleThread_help_setup(116)
	#define simpleThread_help_setup_repeat_115() simpleThread_help_setup_repeat_114() simpleThread_help_setup(115)
	#define simpleThread_help_setup_repeat_114() simpleThread_help_setup_repeat_113() simpleThread_help_setup(114)
	#define simpleThread_help_setup_repeat_113() simpleThread_help_setup_repeat_112() simpleThread_help_setup(113)
	#define simpleThread_help_setup_repeat_112() simpleThread_help_setup_repeat_111() simpleThread_help_setup(112)
	#define simpleThread_help_setup_repeat_111() simpleThread_help_setup_repeat_110() simpleThread_help_setup(111)
	#define simpleThread_help_setup_repeat_110() simpleThread_help_setup_repeat_109() simpleThread_help_setup(110)
	#define simpleThread_help_setup_repeat_109() simpleThread_help_setup_repeat_108() simpleThread_help_setup(109)
	#define simpleThread_help_setup_repeat_108() simpleThread_help_setup_repeat_107() simpleThread_help_setup(108)
	#define simpleThread_help_setup_repeat_107() simpleThread_help_setup_repeat_106() simpleThread_help_setup(107)
	#define simpleThread_help_setup_repeat_106() simpleThread_help_setup_repeat_105() simpleThread_help_setup(106)
	#define simpleThread_help_setup_repeat_105() simpleThread_help_setup_repeat_104() simpleThread_help_setup(105)
	#define simpleThread_help_setup_repeat_104() simpleThread_help_setup_repeat_103() simpleThread_help_setup(104)
	#define simpleThread_help_setup_repeat_103() simpleThread_help_setup_repeat_102() simpleThread_help_setup(103)
	#define simpleThread_help_setup_repeat_102() simpleThread_help_setup_repeat_101() simpleThread_help_setup(102)
	#define simpleThread_help_setup_repeat_101() simpleThread_help_setup_repeat_100() simpleThread_help_setup(101)
	#define simpleThread_help_setup_repeat_100() simpleThread_help_setup_repeat_99() simpleThread_help_setup(100)
	#define simpleThread_help_setup_repeat_99() simpleThread_help_setup_repeat_98() simpleThread_help_setup(99)
	#define simpleThread_help_setup_repeat_98() simpleThread_help_setup_repeat_97() simpleThread_help_setup(98)
	#define simpleThread_help_setup_repeat_97() simpleThread_help_setup_repeat_96() simpleThread_help_setup(97)
	#define simpleThread_help_setup_repeat_96() simpleThread_help_setup_repeat_95() simpleThread_help_setup(96)
	#define simpleThread_help_setup_repeat_95() simpleThread_help_setup_repeat_94() simpleThread_help_setup(95)
	#define simpleThread_help_setup_repeat_94() simpleThread_help_setup_repeat_93() simpleThread_help_setup(94)
	#define simpleThread_help_setup_repeat_93() simpleThread_help_setup_repeat_92() simpleThread_help_setup(93)
	#define simpleThread_help_setup_repeat_92() simpleThread_help_setup_repeat_91() simpleThread_help_setup(92)
	#define simpleThread_help_setup_repeat_91() simpleThread_help_setup_repeat_90() simpleThread_help_setup(91)
	#define simpleThread_help_setup_repeat_90() simpleThread_help_setup_repeat_89() simpleThread_help_setup(90)
	#define simpleThread_help_setup_repeat_89() simpleThread_help_setup_repeat_88() simpleThread_help_setup(89)
	#define simpleThread_help_setup_repeat_88() simpleThread_help_setup_repeat_87() simpleThread_help_setup(88)
	#define simpleThread_help_setup_repeat_87() simpleThread_help_setup_repeat_86() simpleThread_help_setup(87)
	#define simpleThread_help_setup_repeat_86() simpleThread_help_setup_repeat_85() simpleThread_help_setup(86)
	#define simpleThread_help_setup_repeat_85() simpleThread_help_setup_repeat_84() simpleThread_help_setup(85)
	#define simpleThread_help_setup_repeat_84() simpleThread_help_setup_repeat_83() simpleThread_help_setup(84)
	#define simpleThread_help_setup_repeat_83() simpleThread_help_setup_repeat_82() simpleThread_help_setup(83)
	#define simpleThread_help_setup_repeat_82() simpleThread_help_setup_repeat_81() simpleThread_help_setup(82)
	#define simpleThread_help_setup_repeat_81() simpleThread_help_setup_repeat_80() simpleThread_help_setup(81)
	#define simpleThread_help_setup_repeat_80() simpleThread_help_setup_repeat_79() simpleThread_help_setup(80)
	#define simpleThread_help_setup_repeat_79() simpleThread_help_setup_repeat_78() simpleThread_help_setup(79)
	#define simpleThread_help_setup_repeat_78() simpleThread_help_setup_repeat_77() simpleThread_help_setup(78)
	#define simpleThread_help_setup_repeat_77() simpleThread_help_setup_repeat_76() simpleThread_help_setup(77)
	#define simpleThread_help_setup_repeat_76() simpleThread_help_setup_repeat_75() simpleThread_help_setup(76)
	#define simpleThread_help_setup_repeat_75() simpleThread_help_setup_repeat_74() simpleThread_help_setup(75)
	#define simpleThread_help_setup_repeat_74() simpleThread_help_setup_repeat_73() simpleThread_help_setup(74)
	#define simpleThread_help_setup_repeat_73() simpleThread_help_setup_repeat_72() simpleThread_help_setup(73)
	#define simpleThread_help_setup_repeat_72() simpleThread_help_setup_repeat_71() simpleThread_help_setup(72)
	#define simpleThread_help_setup_repeat_71() simpleThread_help_setup_repeat_70() simpleThread_help_setup(71)
	#define simpleThread_help_setup_repeat_70() simpleThread_help_setup_repeat_69() simpleThread_help_setup(70)
	#define simpleThread_help_setup_repeat_69() simpleThread_help_setup_repeat_68() simpleThread_help_setup(69)
	#define simpleThread_help_setup_repeat_68() simpleThread_help_setup_repeat_67() simpleThread_help_setup(68)
	#define simpleThread_help_setup_repeat_67() simpleThread_help_setup_repeat_66() simpleThread_help_setup(67)
	#define simpleThread_help_setup_repeat_66() simpleThread_help_setup_repeat_65() simpleThread_help_setup(66)
	#define simpleThread_help_setup_repeat_65() simpleThread_help_setup_repeat_64() simpleThread_help_setup(65)
	#define simpleThread_help_setup_repeat_64() simpleThread_help_setup_repeat_63() simpleThread_help_setup(64)
	#define simpleThread_help_setup_repeat_63() simpleThread_help_setup_repeat_62() simpleThread_help_setup(63)
	#define simpleThread_help_setup_repeat_62() simpleThread_help_setup_repeat_61() simpleThread_help_setup(62)
	#define simpleThread_help_setup_repeat_61() simpleThread_help_setup_repeat_60() simpleThread_help_setup(61)
	#define simpleThread_help_setup_repeat_60() simpleThread_help_setup_repeat_59() simpleThread_help_setup(60)
	#define simpleThread_help_setup_repeat_59() simpleThread_help_setup_repeat_58() simpleThread_help_setup(59)
	#define simpleThread_help_setup_repeat_58() simpleThread_help_setup_repeat_57() simpleThread_help_setup(58)
	#define simpleThread_help_setup_repeat_57() simpleThread_help_setup_repeat_56() simpleThread_help_setup(57)
	#define simpleThread_help_setup_repeat_56() simpleThread_help_setup_repeat_55() simpleThread_help_setup(56)
	#define simpleThread_help_setup_repeat_55() simpleThread_help_setup_repeat_54() simpleThread_help_setup(55)
	#define simpleThread_help_setup_repeat_54() simpleThread_help_setup_repeat_53() simpleThread_help_setup(54)
	#define simpleThread_help_setup_repeat_53() simpleThread_help_setup_repeat_52() simpleThread_help_setup(53)
	#define simpleThread_help_setup_repeat_52() simpleThread_help_setup_repeat_51() simpleThread_help_setup(52)
	#define simpleThread_help_setup_repeat_51() simpleThread_help_setup_repeat_50() simpleThread_help_setup(51)
	#define simpleThread_help_setup_repeat_50() simpleThread_help_setup_repeat_49() simpleThread_help_setup(50)
	#define simpleThread_help_setup_repeat_49() simpleThread_help_setup_repeat_48() simpleThread_help_setup(49)
	#define simpleThread_help_setup_repeat_48() simpleThread_help_setup_repeat_47() simpleThread_help_setup(48)
	#define simpleThread_help_setup_repeat_47() simpleThread_help_setup_repeat_46() simpleThread_help_setup(47)
	#define simpleThread_help_setup_repeat_46() simpleThread_help_setup_repeat_45() simpleThread_help_setup(46)
	#define simpleThread_help_setup_repeat_45() simpleThread_help_setup_repeat_44() simpleThread_help_setup(45)
	#define simpleThread_help_setup_repeat_44() simpleThread_help_setup_repeat_43() simpleThread_help_setup(44)
	#define simpleThread_help_setup_repeat_43() simpleThread_help_setup_repeat_42() simpleThread_help_setup(43)
	#define simpleThread_help_setup_repeat_42() simpleThread_help_setup_repeat_41() simpleThread_help_setup(42)
	#define simpleThread_help_setup_repeat_41() simpleThread_help_setup_repeat_40() simpleThread_help_setup(41)
	#define simpleThread_help_setup_repeat_40() simpleThread_help_setup_repeat_39() simpleThread_help_setup(40)
	#define simpleThread_help_setup_repeat_39() simpleThread_help_setup_repeat_38() simpleThread_help_setup(39)
	#define simpleThread_help_setup_repeat_38() simpleThread_help_setup_repeat_37() simpleThread_help_setup(38)
	#define simpleThread_help_setup_repeat_37() simpleThread_help_setup_repeat_36() simpleThread_help_setup(37)
	#define simpleThread_help_setup_repeat_36() simpleThread_help_setup_repeat_35() simpleThread_help_setup(36)
	#define simpleThread_help_setup_repeat_35() simpleThread_help_setup_repeat_34() simpleThread_help_setup(35)
	#define simpleThread_help_setup_repeat_34() simpleThread_help_setup_repeat_33() simpleThread_help_setup(34)
	#define simpleThread_help_setup_repeat_33() simpleThread_help_setup_repeat_32() simpleThread_help_setup(33)
	#define simpleThread_help_setup_repeat_32() simpleThread_help_setup_repeat_31() simpleThread_help_setup(32)
	#define simpleThread_help_setup_repeat_31() simpleThread_help_setup_repeat_30() simpleThread_help_setup(31)
	#define simpleThread_help_setup_repeat_30() simpleThread_help_setup_repeat_29() simpleThread_help_setup(30)
	#define simpleThread_help_setup_repeat_29() simpleThread_help_setup_repeat_28() simpleThread_help_setup(29)
	#define simpleThread_help_setup_repeat_28() simpleThread_help_setup_repeat_27() simpleThread_help_setup(28)
	#define simpleThread_help_setup_repeat_27() simpleThread_help_setup_repeat_26() simpleThread_help_setup(27)
	#define simpleThread_help_setup_repeat_26() simpleThread_help_setup_repeat_25() simpleThread_help_setup(26)
	#define simpleThread_help_setup_repeat_25() simpleThread_help_setup_repeat_24() simpleThread_help_setup(25)
	#define simpleThread_help_setup_repeat_24() simpleThread_help_setup_repeat_23() simpleThread_help_setup(24)
	#define simpleThread_help_setup_repeat_23() simpleThread_help_setup_repeat_22() simpleThread_help_setup(23)
	#define simpleThread_help_setup_repeat_22() simpleThread_help_setup_repeat_21() simpleThread_help_setup(22)
	#define simpleThread_help_setup_repeat_21() simpleThread_help_setup_repeat_20() simpleThread_help_setup(21)
	#define simpleThread_help_setup_repeat_20() simpleThread_help_setup_repeat_19() simpleThread_help_setup(20)
	#define simpleThread_help_setup_repeat_19() simpleThread_help_setup_repeat_18() simpleThread_help_setup(19)
	#define simpleThread_help_setup_repeat_18() simpleThread_help_setup_repeat_17() simpleThread_help_setup(18)
	#define simpleThread_help_setup_repeat_17() simpleThread_help_setup_repeat_16() simpleThread_help_setup(17)
	#define simpleThread_help_setup_repeat_16() simpleThread_help_setup_repeat_15() simpleThread_help_setup(16)
	#define simpleThread_help_setup_repeat_15() simpleThread_help_setup_repeat_14() simpleThread_help_setup(15)
	#define simpleThread_help_setup_repeat_14() simpleThread_help_setup_repeat_13() simpleThread_help_setup(14)
	#define simpleThread_help_setup_repeat_13() simpleThread_help_setup_repeat_12() simpleThread_help_setup(13)
	#define simpleThread_help_setup_repeat_12() simpleThread_help_setup_repeat_11() simpleThread_help_setup(12)
	#define simpleThread_help_setup_repeat_11() simpleThread_help_setup_repeat_10() simpleThread_help_setup(11)
	#define simpleThread_help_setup_repeat_10() simpleThread_help_setup_repeat_9() simpleThread_help_setup(10)
	#define simpleThread_help_setup_repeat_9() simpleThread_help_setup_repeat_8() simpleThread_help_setup(9)
	#define simpleThread_help_setup_repeat_8() simpleThread_help_setup_repeat_7() simpleThread_help_setup(8)
	#define simpleThread_help_setup_repeat_7() simpleThread_help_setup_repeat_6() simpleThread_help_setup(7)
	#define simpleThread_help_setup_repeat_6() simpleThread_help_setup_repeat_5() simpleThread_help_setup(6)
	#define simpleThread_help_setup_repeat_5() simpleThread_help_setup_repeat_4() simpleThread_help_setup(5)
	#define simpleThread_help_setup_repeat_4() simpleThread_help_setup_repeat_3() simpleThread_help_setup(4)
	#define simpleThread_help_setup_repeat_3() simpleThread_help_setup_repeat_2() simpleThread_help_setup(3)
	#define simpleThread_help_setup_repeat_2() simpleThread_help_setup_repeat_1() simpleThread_help_setup(2)
	#define simpleThread_help_setup_repeat_1() simpleThread_help_setup_repeat_0() simpleThread_help_setup(1) 
	#define simpleThread_help_setup_repeat_0() simpleThread_help_setup(0) 
	#define simpleThread_help_setupInit(N) simpleThread_help_setup_repeat_##N()

	#define simpleThread_initSetup(N)\
		simpleThread_help_setupInit(N);
		

	#define _sT_P1 0
	#define _sT_P2 1
	#define _sT_P3 2
	#define _sT_P4 3
	#define _sT_P5 4
	#define _sT_P6 5
	#define _sT_P7 6
	#define _sT_P8 7
	#define _sT_P9 8
	#define _sT_P10 9
	#define _sT_P11 10
	#define _sT_P12 11
	#define _sT_P13 12
	#define _sT_P14 13
	#define _sT_P15 14
	#define _sT_P16 15
	#define _sT_P17 16
	#define _sT_P18 17
	#define _sT_P19 18
	#define _sT_P20 19
	#define _sT_P21 20
	#define _sT_P22 21
	#define _sT_P23 22
	#define _sT_P24 23
	#define _sT_P25 24
	#define _sT_P26 25
	#define _sT_P27 26
	#define _sT_P28 27
	#define _sT_P29 28
	#define _sT_P30 29
	#define _sT_P31 30
	#define _sT_P32 31
	#define _sT_P33 32
	#define _sT_P34 33
	#define _sT_P35 34
	#define _sT_P36 35
	#define _sT_P37 36
	#define _sT_P38 37
	#define _sT_P39 38
	#define _sT_P40 39
	#define _sT_P41 40
	#define _sT_P42 41
	#define _sT_P43 42
	#define _sT_P44 43
	#define _sT_P45 44
	#define _sT_P46 45
	#define _sT_P47 46
	#define _sT_P48 47
	#define _sT_P49 48
	#define _sT_P50 49
	#define _sT_P51 50
	#define _sT_P52 51
	#define _sT_P53 52
	#define _sT_P54 53
	#define _sT_P55 54
	#define _sT_P56 55
	#define _sT_P57 56
	#define _sT_P58 57
	#define _sT_P59 58
	#define _sT_P60 59
	#define _sT_P61 60
	#define _sT_P62 61
	#define _sT_P63 62
	#define _sT_P64 63
	#define _sT_P65 64
	#define _sT_P66 65
	#define _sT_P67 66
	#define _sT_P68 67
	#define _sT_P69 68
	#define _sT_P70 69
	#define _sT_P71 70
	#define _sT_P72 71
	#define _sT_P73 72
	#define _sT_P74 73
	#define _sT_P75 74
	#define _sT_P76 75
	#define _sT_P77 76
	#define _sT_P78 77
	#define _sT_P79 78
	#define _sT_P80 79
	#define _sT_P81 80
	#define _sT_P82 81
	#define _sT_P83 82
	#define _sT_P84 83
	#define _sT_P85 84
	#define _sT_P86 85
	#define _sT_P87 86
	#define _sT_P88 87
	#define _sT_P89 88
	#define _sT_P90 89
	#define _sT_P91 90
	#define _sT_P92 91
	#define _sT_P93 92
	#define _sT_P94 93
	#define _sT_P95 94
	#define _sT_P96 95
	#define _sT_P97 96
	#define _sT_P98 97
	#define _sT_P99 98
	#define _sT_P100 99
	#define _sT_P101 100
	#define _sT_P102 101
	#define _sT_P103 102
	#define _sT_P104 103
	#define _sT_P105 104
	#define _sT_P106 105
	#define _sT_P107 106
	#define _sT_P108 107
	#define _sT_P109 108
	#define _sT_P110 109
	#define _sT_P111 110
	#define _sT_P112 111
	#define _sT_P113 112
	#define _sT_P114 113
	#define _sT_P115 114
	#define _sT_P116 115
	#define _sT_P117 116
	#define _sT_P118 117
	#define _sT_P119 118
	#define _sT_P120 119
	#define _sT_P121 120
	#define _sT_P122 121
	#define _sT_P123 122
	#define _sT_P124 123
	#define _sT_P125 124
	#define _sT_P126 125
	#define _sT_P127 126
	#define _sT_P128 127
	#define _sT_P129 128
	#define _sT_P130 129
	#define _sT_P131 130
	#define _sT_P132 131
	#define _sT_P133 132
	#define _sT_P134 133
	#define _sT_P135 134
	#define _sT_P136 135
	#define _sT_P137 136
	#define _sT_P138 137
	#define _sT_P139 138
	#define _sT_P140 139
	#define _sT_P141 140
	#define _sT_P142 141
	#define _sT_P143 142
	#define _sT_P144 143
	#define _sT_P145 144
	#define _sT_P146 145
	#define _sT_P147 146
	#define _sT_P148 147
	#define _sT_P149 148
	#define _sT_P150 149
	#define _sT_P151 150
	#define _sT_P152 151
	#define _sT_P153 152
	#define _sT_P154 153
	#define _sT_P155 154
	#define _sT_P156 155
	#define _sT_P157 156
	#define _sT_P158 157
	#define _sT_P159 158
	#define _sT_P160 159
	#define _sT_P161 160
	#define _sT_P162 161
	#define _sT_P163 162
	#define _sT_P164 163
	#define _sT_P165 164
	#define _sT_P166 165
	#define _sT_P167 166
	#define _sT_P168 167
	#define _sT_P169 168
	#define _sT_P170 169
	#define _sT_P171 170
	#define _sT_P172 171
	#define _sT_P173 172
	#define _sT_P174 173
	#define _sT_P175 174
	#define _sT_P176 175
	#define _sT_P177 176
	#define _sT_P178 177
	#define _sT_P179 178
	#define _sT_P180 179
	#define _sT_P181 180
	#define _sT_P182 181
	#define _sT_P183 182
	#define _sT_P184 183
	#define _sT_P185 184
	#define _sT_P186 185
	#define _sT_P187 186
	#define _sT_P188 187
	#define _sT_P189 188
	#define _sT_P190 189
	#define _sT_P191 190
	#define _sT_P192 191
	#define _sT_P193 192
	#define _sT_P194 193
	#define _sT_P195 194
	#define _sT_P196 195
	#define _sT_P197 196
	#define _sT_P198 197
	#define _sT_P199 198
	#define _sT_P200 199
	#define _sT_P201 200
	#define _sT_P202 201
	#define _sT_P203 202
	#define _sT_P204 203
	#define _sT_P205 204
	#define _sT_P206 205
	#define _sT_P207 206
	#define _sT_P208 207
	#define _sT_P209 208
	#define _sT_P210 209
	#define _sT_P211 210
	#define _sT_P212 211
	#define _sT_P213 212
	#define _sT_P214 213
	#define _sT_P215 214
	#define _sT_P216 215
	#define _sT_P217 216
	#define _sT_P218 217
	#define _sT_P219 218
	#define _sT_P220 219
	#define _sT_P221 220
	#define _sT_P222 221
	#define _sT_P223 222
	#define _sT_P224 223
	#define _sT_P225 224
	#define _sT_P226 225
	#define _sT_P227 226
	#define _sT_P228 227
	#define _sT_P229 228
	#define _sT_P230 229
	#define _sT_P231 230
	#define _sT_P232 231
	#define _sT_P233 232
	#define _sT_P234 233
	#define _sT_P235 234
	#define _sT_P236 235
	#define _sT_P237 236
	#define _sT_P238 237
	#define _sT_P239 238
	#define _sT_P240 239
	#define _sT_P241 240
	#define _sT_P242 241
	#define _sT_P243 242
	#define _sT_P244 243
	#define _sT_P245 244
	#define _sT_P246 245
	#define _sT_P247 246
	#define _sT_P248 247
	#define _sT_P249 248
	#define _sT_P250 249
	#define _sT_P251 250
	#define _sT_P252 251
	#define _sT_P253 252
	#define _sT_P254 253

	#define _sT_cnt_1 0
	#define _sT_cnt_2 1
	#define _sT_cnt_3 2
	#define _sT_cnt_4 3
	#define _sT_cnt_5 4
	#define _sT_cnt_6 5
	#define _sT_cnt_7 6
	#define _sT_cnt_8 7
	#define _sT_cnt_9 8
	#define _sT_cnt_10 9
	#define _sT_cnt_11 10
	#define _sT_cnt_12 11
	#define _sT_cnt_13 12
	#define _sT_cnt_14 13
	#define _sT_cnt_15 14
	#define _sT_cnt_16 15
	#define _sT_cnt_17 16
	#define _sT_cnt_18 17
	#define _sT_cnt_19 18
	#define _sT_cnt_20 19
	#define _sT_cnt_21 20
	#define _sT_cnt_22 21
	#define _sT_cnt_23 22
	#define _sT_cnt_24 23
	#define _sT_cnt_25 24
	#define _sT_cnt_26 25
	#define _sT_cnt_27 26
	#define _sT_cnt_28 27
	#define _sT_cnt_29 28
	#define _sT_cnt_30 29
	#define _sT_cnt_31 30
	#define _sT_cnt_32 31
	#define _sT_cnt_33 32
	#define _sT_cnt_34 33
	#define _sT_cnt_35 34
	#define _sT_cnt_36 35
	#define _sT_cnt_37 36
	#define _sT_cnt_38 37
	#define _sT_cnt_39 38
	#define _sT_cnt_40 39
	#define _sT_cnt_41 40
	#define _sT_cnt_42 41
	#define _sT_cnt_43 42
	#define _sT_cnt_44 43
	#define _sT_cnt_45 44
	#define _sT_cnt_46 45
	#define _sT_cnt_47 46
	#define _sT_cnt_48 47
	#define _sT_cnt_49 48
	#define _sT_cnt_50 49
	#define _sT_cnt_51 50
	#define _sT_cnt_52 51
	#define _sT_cnt_53 52
	#define _sT_cnt_54 53
	#define _sT_cnt_55 54
	#define _sT_cnt_56 55
	#define _sT_cnt_57 56
	#define _sT_cnt_58 57
	#define _sT_cnt_59 58
	#define _sT_cnt_60 59
	#define _sT_cnt_61 60
	#define _sT_cnt_62 61
	#define _sT_cnt_63 62
	#define _sT_cnt_64 63
	#define _sT_cnt_65 64
	#define _sT_cnt_66 65
	#define _sT_cnt_67 66
	#define _sT_cnt_68 67
	#define _sT_cnt_69 68
	#define _sT_cnt_70 69
	#define _sT_cnt_71 70
	#define _sT_cnt_72 71
	#define _sT_cnt_73 72
	#define _sT_cnt_74 73
	#define _sT_cnt_75 74
	#define _sT_cnt_76 75
	#define _sT_cnt_77 76
	#define _sT_cnt_78 77
	#define _sT_cnt_79 78
	#define _sT_cnt_80 79
	#define _sT_cnt_81 80
	#define _sT_cnt_82 81
	#define _sT_cnt_83 82
	#define _sT_cnt_84 83
	#define _sT_cnt_85 84
	#define _sT_cnt_86 85
	#define _sT_cnt_87 86
	#define _sT_cnt_88 87
	#define _sT_cnt_89 88
	#define _sT_cnt_90 89
	#define _sT_cnt_91 90
	#define _sT_cnt_92 91
	#define _sT_cnt_93 92
	#define _sT_cnt_94 93
	#define _sT_cnt_95 94
	#define _sT_cnt_96 95
	#define _sT_cnt_97 96
	#define _sT_cnt_98 97
	#define _sT_cnt_99 98
	#define _sT_cnt_100 99
	#define _sT_cnt_101 100
	#define _sT_cnt_102 101
	#define _sT_cnt_103 102
	#define _sT_cnt_104 103
	#define _sT_cnt_105 104
	#define _sT_cnt_106 105
	#define _sT_cnt_107 106
	#define _sT_cnt_108 107
	#define _sT_cnt_109 108
	#define _sT_cnt_110 109
	#define _sT_cnt_111 110
	#define _sT_cnt_112 111
	#define _sT_cnt_113 112
	#define _sT_cnt_114 113
	#define _sT_cnt_115 114
	#define _sT_cnt_116 115
	#define _sT_cnt_117 116
	#define _sT_cnt_118 117
	#define _sT_cnt_119 118
	#define _sT_cnt_120 119
	#define _sT_cnt_121 120
	#define _sT_cnt_122 121
	#define _sT_cnt_123 122
	#define _sT_cnt_124 123
	#define _sT_cnt_125 124
	#define _sT_cnt_126 125
	#define _sT_cnt_127 126
	#define _sT_cnt_128 127
	#define _sT_cnt_129 128
	#define _sT_cnt_130 129
	#define _sT_cnt_131 130
	#define _sT_cnt_132 131
	#define _sT_cnt_133 132
	#define _sT_cnt_134 133
	#define _sT_cnt_135 134
	#define _sT_cnt_136 135
	#define _sT_cnt_137 136
	#define _sT_cnt_138 137
	#define _sT_cnt_139 138
	#define _sT_cnt_140 139
	#define _sT_cnt_141 140
	#define _sT_cnt_142 141
	#define _sT_cnt_143 142
	#define _sT_cnt_144 143
	#define _sT_cnt_145 144
	#define _sT_cnt_146 145
	#define _sT_cnt_147 146
	#define _sT_cnt_148 147
	#define _sT_cnt_149 148
	#define _sT_cnt_150 149
	#define _sT_cnt_151 150
	#define _sT_cnt_152 151
	#define _sT_cnt_153 152
	#define _sT_cnt_154 153
	#define _sT_cnt_155 154
	#define _sT_cnt_156 155
	#define _sT_cnt_157 156
	#define _sT_cnt_158 157
	#define _sT_cnt_159 158
	#define _sT_cnt_160 159
	#define _sT_cnt_161 160
	#define _sT_cnt_162 161
	#define _sT_cnt_163 162
	#define _sT_cnt_164 163
	#define _sT_cnt_165 164
	#define _sT_cnt_166 165
	#define _sT_cnt_167 166
	#define _sT_cnt_168 167
	#define _sT_cnt_169 168
	#define _sT_cnt_170 169
	#define _sT_cnt_171 170
	#define _sT_cnt_172 171
	#define _sT_cnt_173 172
	#define _sT_cnt_174 173
	#define _sT_cnt_175 174
	#define _sT_cnt_176 175
	#define _sT_cnt_177 176
	#define _sT_cnt_178 177
	#define _sT_cnt_179 178
	#define _sT_cnt_180 179
	#define _sT_cnt_181 180
	#define _sT_cnt_182 181
	#define _sT_cnt_183 182
	#define _sT_cnt_184 183
	#define _sT_cnt_185 184
	#define _sT_cnt_186 185
	#define _sT_cnt_187 186
	#define _sT_cnt_188 187
	#define _sT_cnt_189 188
	#define _sT_cnt_190 189
	#define _sT_cnt_191 190
	#define _sT_cnt_192 191
	#define _sT_cnt_193 192
	#define _sT_cnt_194 193
	#define _sT_cnt_195 194
	#define _sT_cnt_196 195
	#define _sT_cnt_197 196
	#define _sT_cnt_198 197
	#define _sT_cnt_199 198
	#define _sT_cnt_200 199
	#define _sT_cnt_201 200
	#define _sT_cnt_202 201
	#define _sT_cnt_203 202
	#define _sT_cnt_204 203
	#define _sT_cnt_205 204
	#define _sT_cnt_206 205
	#define _sT_cnt_207 206
	#define _sT_cnt_208 207
	#define _sT_cnt_209 208
	#define _sT_cnt_210 209
	#define _sT_cnt_211 210
	#define _sT_cnt_212 211
	#define _sT_cnt_213 212
	#define _sT_cnt_214 213
	#define _sT_cnt_215 214
	#define _sT_cnt_216 215
	#define _sT_cnt_217 216
	#define _sT_cnt_218 217
	#define _sT_cnt_219 218
	#define _sT_cnt_220 219
	#define _sT_cnt_221 220
	#define _sT_cnt_222 221
	#define _sT_cnt_223 222
	#define _sT_cnt_224 223
	#define _sT_cnt_225 224
	#define _sT_cnt_226 225
	#define _sT_cnt_227 226
	#define _sT_cnt_228 227
	#define _sT_cnt_229 228
	#define _sT_cnt_230 229
	#define _sT_cnt_231 230
	#define _sT_cnt_232 231
	#define _sT_cnt_233 232
	#define _sT_cnt_234 233
	#define _sT_cnt_235 234
	#define _sT_cnt_236 235
	#define _sT_cnt_237 236
	#define _sT_cnt_238 237
	#define _sT_cnt_239 238
	#define _sT_cnt_240 239
	#define _sT_cnt_241 240
	#define _sT_cnt_242 241
	#define _sT_cnt_243 242
	#define _sT_cnt_244 243
	#define _sT_cnt_245 244
	#define _sT_cnt_246 245
	#define _sT_cnt_247 246
	#define _sT_cnt_248 247
	#define _sT_cnt_249 248
	#define _sT_cnt_250 249
	#define _sT_cnt_251 250
	#define _sT_cnt_252 251
	#define _sT_cnt_253 252
	#define _sT_cnt_254 253



#endif