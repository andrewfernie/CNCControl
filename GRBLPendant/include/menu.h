// menu.h

#ifndef _MENU_h
#define _MENU_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <LiquidCrystal_I2C.h>   // LCD over I2C

const int MenuTextLength = 12;

enum MenuParameterType {
    ParamStatic = 0,
    ParamFloat,
    ParamInt,
    ParamOnOff,
    ParamInMm,
    ParamYesNo
};
struct MenuParameterItem 
{
	MenuParameterType type;			
	const char	name[MenuTextLength];
	uint8_t scale;			// number of decimal places
	float	min;
	float	max;
	void* variable;
};

class Menu
{
public:
    Menu(MenuParameterItem menuParameters[], int numItems, LiquidCrystal_I2C* panel, long* encoderValue);


    /// Event codes used by handleEvent and to explain to the superclass
    /// why we are leaving.
    enum event {
        NOP = 0,
        UP,             ///< cursor up
        DOWN,           ///< cursor down
        LEFT,           ///< cursor left
        RIGHT,          ///< cursor right
        LEFTAC,         ///< cursor left if AC mode
        RIGHTAC,        ///< cursor right if AC mode
        OK,             ///< ok button
        CANCEL,         ///< cancel button
        TIMEOUT,        ///< timeout event
        NONE            ///< ignore this event
    };



    // Draw the page
    void     Draw();

    void     EditItem(int itemIndex);

    ///// initialise the page system, draw the first page
    /////
    //static void     begin();

    ///// give the current page a chance to update
    /////
    //static void     update();

    ///// force the display to move to a given page
    /////
    ///// @param newPage      page number for the new page
    /////
    //static void     forcePage(uint8_t newPage);

    ///// send the current page an event
    /////
    ///// @param eventCode    event to be sent to the page
    /////
    //static void     handleEvent(Page::event eventCode);

    //static void     handleDirectPageEvent(uint8_t page);

    ///// get the return code from any buttons associated with the page being presses
    /////		
    //static uint8_t   GetButtonReturnCode();

    //void DrawDefaultButtons();

    uint8_t			getType(uint8_t paramNum);
    uint8_t			getRow(uint8_t paramNum);
    uint8_t			getCol(uint8_t paramNum);
    float			getMin(uint8_t paramNum);
    float			getMax(uint8_t paramNum);
    uint8_t			getScale(uint8_t paramNum);
    void			getName(uint8_t paramNum, char* nameReturn);
    uint8_t			getIndexFromName(char* nameIn);
    void			setValueFloat(uint8_t index, float value);
    uint8_t			setValueFloatFromName(char* nameIn, float valueIn);
    void            printFloat(char* outBuffer, float val, byte precision);
    int             GetNumMenuItems();
    int             GetCurrentItemIndex();

protected:
    MenuParameterItem *pParameters;
    LiquidCrystal_I2C* pMenuPanel;
    long* pEncoderPosition;
    int m_NumMenuItems;
    
//    //TouchButton* defaultPageButtonArray;				// The default buttons for this page
//
//    //uint8_t numDefaultPageButtonArray;				// Number of default buttons for this page
//
//    ///// draw any buttons associated with the page
//    /////
//    ///// @param pageIndex     index into the array of page definitions for the page being entered, or in
//    /////	
//    //virtual void     _drawButtons(uint8_t pageIndex);
//
//    ///// Notification from the page system that the page has been entered,
//    ///// implemented by the subclass.
//    /////
//    ///// @param fromPage     the page that was just left
//    /////
//    //virtual void    _enter(uint8_t fromPage, uint8_t newPageIndex) = 0;
//
//    /// Periodic update call, implemented by the subclass.
//    ///
//    /// @todo rate limit this?
//    ///
//    virtual void    _update(void);
//
//    /// Notification of an event, implemented by the subclass.
//    ///
//    /// @param eventCode    event code being delivered
//    ///
//    virtual void    _handleEvent(Page::event eventCode);
//
//    virtual void    _handleDirectPageEvent(uint8_t page);
//
//    /// Departure request from the page subclass.
//    ///
//    /// @param eventCode    The event causing the page to leave,
//    ///                     used to decide which page to make current
//    ///                     next.
//    ///
//    static void     _leave(Page::event reasonCode);
//
//    /// Departure request from the page subclass.
//    ///
//    /// @param newPage      The new page code to go to
//    ///
//    static void     _leaveDirectPage(uint8_t newPage);
//
//    /// Transition to a new page, either called by the subclass when
//    /// it is doing explicit navigation (e.g. returning to the page that
//    /// called it) or due to an external page force.
//    ///
//    /// @param newPage      page number for the new page
//    ///
//    static void     _goPage(uint8_t newPage);
//
//    /// Accessor for the button definition for pageIndex in the navigation map
//    ///
//    /// @param pageIndex    page for which this is to be fetched
//    /// @returns            class instance pointer
//    ///
//    static TouchButton* _pageButtons(uint8_t pageIndex);
//
//
//    /// Accessor for the number of buttons for pageIndex in the navigation map
//    ///
//    /// @param pageIndex    page for which this is to be fetched
//    /// @returns            number of buttons
//    ///
//    static uint8_t _numButtons(uint8_t pageIndex);
//
//
//private:
//    /// Accessor for the pageCode for pageIndex in the navigation map
//    ///
//    /// @param pageIndex    page for which code is to be fetched
//    /// @returns            page code
//    ///
//    static uint8_t  _pageCode(uint8_t pageIndex);
//
//    /// Accessor for the page class instance for pageIndex in the navigation map
//    ///
//    /// @param pageIndex    page for which this is to be fetched
//    /// @returns            class instance pointer
//    ///
//    static Page* _pageThis(uint8_t pageIndex);
//
//    /// Accessor for the exitIndex page exit code for pageIndex in the navigation map
//    ///
//    /// @param pageIndex    page for which the exit direction is to be fetched
//    /// @param exitIndex    exit index for which the exit direction is to be fetched
//    /// @returns            page exit direction
//    ///
//    static uint8_t  _pageExit(uint8_t pageIndex, uint8_t exitIndex);
};

#endif

