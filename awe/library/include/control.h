#ifndef AWE_CONTROL_H
#define AWE_CONTROL_H


#include "widget.h"


#ifdef __cplusplus
    extern "C" {
#endif


struct AWE_CLASS;


/**@name Tab focusing and hot keys
    <p> In order to solve the problem of hot keys and tab focusing, AWE introduces
        two special interfaces that any widget can implement:
    </p>
    <li> The <b>Control</b> interface provides the means for managing unused
         key presses/hot keys.
    </li>
    <li> The <b>Dialog</b> interface provides the means for dispatching
         unused keypresses and managing tab focus.
    </li>
    <p> The Control interface consists of a vtable that contains a method for
        unused key presses. The Dialog interface consists of a widget key down
        implementation that manages unused key dispatch and tab focusing.
    </p>
    <p> A dialog-derived widget that is the ancestor of control-derived widgets
        first dispatches the unused keypress to all its descentants in 
        back to front order, then if no descentant widget processes the unused
        key it moves the focus around using the cursor keys, tab or tab+shift.
        This action takes place from inside the dialog's key down implementation.
    </p>
 */
/*@{*/


///id of the control interface
#define AWE_ID_CONTROL       "Control"


///the control interface
struct AWE_CONTROL_VTABLE {
    /** method for managing unused keypresses/hot keys. It is called from the
        dialog ancestor.
        @param wgt widget that is being called
        @param event key event 
        @return non-zero if the keypress is processed, zero otherwise.
     */
    int (*key_press)(AWE_WIDGET *wgt, const AWE_EVENT *event);
};
typedef struct AWE_CONTROL_VTABLE AWE_CONTROL_VTABLE;


/** a dialog's key down implementation; it manages control key press dispatch
    and tab focusing. It has two steps:
    1) it traverses all descentant widgets from back to front; if a widget
       implements the control interface, then the function calls the 'key_down'
       method of the control interface. If the widget returns non-zero, the
       function returns.
    2) if no control widget processes the unused key press, then the function
       manages the input focus. If left/up/tab+shift is pressed, then the focus
       is moved backward; if right/down/tab is pressed, then the focus is
       moved forward. If there is no other descentant widget to move the focus
       to, then the focus is recycled.
    @param wgt widget that is being called
    @param event key event 
 */
void awe_dialog_key_down(AWE_WIDGET *wgt, const AWE_EVENT *event);


/*@}*/


#ifdef __cplusplus
    }
#endif


#endif //AWE_CONTROL_H
