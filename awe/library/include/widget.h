#ifndef AWE_WIDGET_H
#define AWE_WIDGET_H


#include "com.h"
#include "gdi.h"
#include "rect.h"
#include "input.h"
#include "linkedlist.h"


#ifdef __cplusplus
   extern "C" {
#endif


/**@name Widget
    <p>The Widget class is the base class for most GUI widgets. It provides
       the basic mechanisms for a GUI:</p>
    <li>widget tree management; widgets have children and siblings.</li>
    <li>the outputing and update mechanism; widgets can output directly on the
        current buffer (screen/game buffer) or to a video or memory bitmap
        for increased speed / alpha-blending effects.</li>
    <li>widget properties:<br>
        <ol>X: int</ol> 
        <ol>Y: int</ol> 
        <ol>Width: int</ol> 
        <ol>Height: int</ol> 
        <ol>Visible: boolean</ol> 
        <ol>Enabled: boolean</ol> 
        <ol>Opaque: boolean; if not true, widget is show through</ol>
        <ol>Translucency: 0 to 255; if < 255, widget is translucent</ol> 
        <ol>OutputType: see enum AWE_WIDGET_OUTPUT_TYPE</ol>
        </li>
    <li>focus widget tree management; at any point in time, only one widget
        has the input focus.</li>
    <li>basic event and property changed callbacks</li>
    <p>The Widget class is a COM class, i.e. it uses the AWE's COM mechanism
       for managing widget-orientation. Other layers above widget offer 
       more capabilities (examples: drag-n-drop, geometry management). </p>
 */
/*@{*/


///widget class name
#define AWE_ID_WIDGET           "Widget"

///X property name
#define AWE_ID_X                "X"

///Y property name
#define AWE_ID_Y                "Y"

///width property name
#define AWE_ID_WIDTH            "Width"

///height property name
#define AWE_ID_HEIGHT           "Height"

///visible property name
#define AWE_ID_VISIBLE          "Visible"

///enabled property name
#define AWE_ID_ENABLED          "Enabled"

///opaque property name
#define AWE_ID_OPAQUE           "Opaque"

///translucency property name
#define AWE_ID_TRANSLUCENCY     "Translucency"

///output type property name
#define AWE_ID_OUTPUT_TYPE      "OutputType"


///update mode for the GUI
enum AWE_GUI_UPDATE_MODE {
    ///update changes only; default value
    AWE_GUI_UPDATE_CHANGES,

    ///update the whole widget tree
    AWE_GUI_UPDATE_FULL
};
typedef enum AWE_GUI_UPDATE_MODE AWE_GUI_UPDATE_MODE;


///widget output type enumeration
enum AWE_WIDGET_OUTPUT_TYPE {
    ///widget draws directly on the graphics buffer; default value
    AWE_WIDGET_OUTPUT_DIRECT,

    ///widget draws on a video bitmap; final image is composed by blitting
    AWE_WIDGET_OUTPUT_VIDEO_BITMAP,

    ///widget draws on a memory bitmap; set when alpha-blending is required
    AWE_WIDGET_OUTPUT_MEMORY_BITMAP
};
typedef enum AWE_WIDGET_OUTPUT_TYPE AWE_WIDGET_OUTPUT_TYPE;


/** type of hook procedure for implementing a DRS.
    @param update_rect the screen-relative rectangle that is being drawn
 */
typedef void (*AWE_DRS_PROC)(AL_CONST AWE_RECT *update_rect);


/** Widget struct; it must be the first member of sub-classes. It inherits
    from AWE_WIDGET.
 */
struct AWE_WIDGET {
    AWE_OBJECT object;
    struct AWE_WIDGET *parent;
    AWE_DL_DATA_NODE node;
    AWE_DL_LIST children;
    short children_count;
    short x;
    short y;
    short width;
    short height;
    AWE_RECT pos;
    AWE_RECT clip;
    AWE_RECT dirty;
    AWE_WIDGET_OUTPUT_TYPE output_type;
    BITMAP *buffer;
    unsigned translucency:8;
    unsigned on_screen:1;
    unsigned drawable:1;
    unsigned visible:1;
    unsigned enabled:1;
    unsigned enabled_tree:1;
    unsigned opaque:1;
    unsigned geometry_changed:1;
    unsigned visible_changed:1;
    unsigned enabled_changed:1;
    unsigned opaque_changed:1;
    unsigned translucency_changed:1;
    unsigned output_type_changed:1;
    unsigned update_geometry:1;
    unsigned update_geometry_children:1;
    unsigned update_drawable:1;
    unsigned update_drawable_children:1;
    unsigned update_redraw:1;
    unsigned update_redraw_children:1;
    unsigned redraw:1;
    unsigned redraw_children:1;
    unsigned repaint:1;
};
typedef struct AWE_WIDGET AWE_WIDGET;


/** widget vtable struct/interface; inherits from object vtable.
 */
struct AWE_WIDGET_VTABLE {
    AWE_OBJECT_VTABLE object;

    /** called when the widget is about to be painted. It is not optional.
        @param wgt widget that should be painted
        @param canvas canvas object used for drawing
        @param dirty_rect the area of the canvas that is to be drawn
     */
    void (*paint)(AWE_WIDGET *wgt, AL_CONST AWE_CANVAS *canvas, AL_CONST AWE_RECT *dirty_rect);

    /** called when a button is pressed over the widget; optional method. The
        default implementation sends the event to its parent.
        @param wgt widget
        @param event event
     */
    void (*button_down)(AWE_WIDGET *wgt, AL_CONST AWE_EVENT *event);

    /** called when a button is released; optional method. The
        default implementation sends the event to its parent.
        @param wgt widget
        @param event event
     */
    void (*button_up)(AWE_WIDGET *wgt, AL_CONST AWE_EVENT *event);

    /** called when the mouse cursor enters a widget; optional method. The
        default implementation sends the event to its parent.
        @param wgt widget
        @param event event
     */
    void (*mouse_enter)(AWE_WIDGET *wgt, AL_CONST AWE_EVENT *event);

    /** called when the mouse cursor moves over a widget; optional method. The
        default implementation sends the event to its parent.
        @param wgt widget
        @param event event
     */
    void (*mouse_move)(AWE_WIDGET *wgt, AL_CONST AWE_EVENT *event);

    /** called when the mouse cursor leaves the widget; optional method. The
        default implementation sends the event to its parent.
        @param wgt widget
        @param event event
     */
    void (*mouse_leave)(AWE_WIDGET *wgt, AL_CONST AWE_EVENT *event);

    /** called when the mouse wheel is rolled; optional method called when
        the widget has the input focus. The
        default implementation sends the event to its parent.
        @param wgt widget
        @param event event
     */
    void (*mouse_wheel)(AWE_WIDGET *wgt, AL_CONST AWE_EVENT *event);

    /** called when a key is pressed; optional method called when
        the widget has the input focus. The
        default implementation sends the event to its parent.
        @param wgt widget
        @param event event
     */
    void (*key_down)(AWE_WIDGET *wgt, AL_CONST AWE_EVENT *event);

    /** called when a key is released; optional method called when
        the widget has the input focus. The
        default implementation sends the event to its parent.
        @param wgt widget
        @param event event
     */
    void (*key_up)(AWE_WIDGET *wgt, AL_CONST AWE_EVENT *event);

    /** called on a timer event; optional method
        @param wgt widget
        @param event event
     */
    void (*timer)(AWE_WIDGET *wgt, AL_CONST AWE_EVENT *event);

    /** called when there is a request to set the focus to this widget. The
        default implementation asks the current focus widget to let the focus
        go, then it takes the focus. This is an optional method: set it to NULL
        to avoid getting the focus alltogether.
        @param wgt widget the widget to get the focus
        @return non-zero if focus changed successfully, false otherwise.
     */
    int (*get_focus)(AWE_WIDGET *wgt);

    /** called when the focus is requested from another widget. The default
        implementation removes the focus from this widget.
        @param wgt widget the widget to loose the focus
        @return non-zero if focus changed successfully, false otherwise.
     */
    int (*loose_focus)(AWE_WIDGET *wgt);

    /** called when the widget is about to be displayed for the first time.
        The default implementation calls the relevant method of children widgets.
        It is an optional method. It is called when a root widget is set or
        when a widget is inserted to an on-screen widget.
        @param wgt widget that is about to be displayed
     */
    void (*begin_display)(AWE_WIDGET *wgt);

    /** called when the widget is about to stop being displayed.
        The default implementation calls the relevant method of children widgets.
        It is an optional method. It is called when a widget is removed from
        an on-screen widget or when the root widget is being changed.
        @param wgt widget that is about to be stopped being displayed
     */
    void (*end_display)(AWE_WIDGET *wgt);

    /** called to insert a child widget; the default implementation inserts
        a widget as a child.
        @param wgt widget that is to become parent
        @param child widget that is about to become child
        @param z_order z-order that the child widget should be inserted at:
               if 0, child widget is inserted on top; if < 0, child widget is
               inserted below all its siblings; other values are middle positions.
        @return zero if given widget could not be inserted as a child, due to
                being already a child of another widget, or is the root widget,
                or an ancestor of the parent widget
     */
    int (*insert_widget)(AWE_WIDGET *wgt, AWE_WIDGET *child, int z_order);

    /** called to remove a child widget; the request may be denied.
        @param wgt parent widget
        @param child child widget
        @return non-zero if the child widget was succesfully removed
     */
    int (*remove_widget)(AWE_WIDGET *wgt, AWE_WIDGET *child);

    /** called when a widget's geometry is changed. Optional method called when
        a widget's geometry is changed. If geometry is changed from inside this method,
        this method will not be called again.
        @param wgt the widget that is affected
        @param x the widget's left coordinate relative to its parent (or the screen if it is root)
        @param y the widget's top coordinate relative to its parent (or the screen if it is root)
        @param width the widget's width
        @param height the widget's height
     */
    void (*geometry_changed)(AWE_WIDGET *wgt, int x, int y, int width, int height);

    /** called when a widget's z-order changes. Optional method.
        @param wgt the widget that is affected
        @param z the new z-order
     */
    void (*z_order_changed)(AWE_WIDGET *wgt, int z);

    /** called when a widget's visible state changes. Optional method.
        @param wgt the widget that is affected
        @param z the new visible state
     */
    void (*visible_changed)(AWE_WIDGET *wgt, int visible);

    /** called to test the live areas of the widget; called from inside event
        handling. Optional method.
        @param wgt the widget that is queried
        @param x horizontal coordinate relative to the widget
        @param y vertical coordinate relative to the widget
        @return non-zero if widget was hit
     */
    int (*hit_test)(AWE_WIDGET *wgt, int x, int y);
};
typedef struct AWE_WIDGET_VTABLE AWE_WIDGET_VTABLE;


///the one and only widget class; base class for all widgets
extern AWE_CLASS awe_widget_class;


/** the default implementation of get_interface
    @param wgt object that is called
    @param name name of the interface
    @param pnamespace namespace of the interface
    @return pointer to interface or null if the interface is not supported
 */
void *awe_widget_get_interface(AWE_OBJECT *wgt, const char *name, const char *pnamespace);


/** the default implementation of the properties_changed method
    @param wgt object that is called
 */
void awe_widget_properties_changed(AWE_OBJECT *wgt);


/** the default implementation of the clone interface
    @param wgt widget to clone
    @return pointer to cloned widget
 */
AWE_OBJECT *awe_widget_clone(AWE_OBJECT *wgt);


/** the default implementation of the paint method
    @param wgt widget that is called
    @param canvas canvas to draw onto
    @param update_rect rectangle, relative to widget, that is being affected
 */
void awe_widget_paint(AWE_WIDGET *wgt, AL_CONST AWE_CANVAS *canvas, AL_CONST AWE_RECT *update_rect);


/** the default implementation of the relevant event; it sends the event to
    the widget's parent
    @param wgt widget to process the event
    @param event the event to process
 */
void awe_widget_button_down(AWE_WIDGET *wgt, AL_CONST AWE_EVENT *event);


/** the default implementation of the relevant event; it sends the event to
    the widget's parent
    @param wgt widget to process the event
    @param event the event to process
 */
void awe_widget_button_up(AWE_WIDGET *wgt, AL_CONST AWE_EVENT *event);


/** the default implementation of the relevant event; it sends the event to
    the widget's parent
    @param wgt widget to process the event
    @param event the event to process
 */
void awe_widget_mouse_enter(AWE_WIDGET *wgt, AL_CONST AWE_EVENT *event);


/** the default implementation of the relevant event; it sends the event to
    the widget's parent
    @param wgt widget to process the event
    @param event the event to process
 */
void awe_widget_mouse_move(AWE_WIDGET *wgt, AL_CONST AWE_EVENT *event);


/** the default implementation of the relevant event; it sends the event to
    the widget's parent
    @param wgt widget to process the event
    @param event the event to process
 */
void awe_widget_mouse_leave(AWE_WIDGET *wgt, AL_CONST AWE_EVENT *event);


/** the default implementation of the relevant event; it sends the event to
    the widget's parent
    @param wgt widget to process the event
    @param event the event to process
 */
void awe_widget_mouse_wheel(AWE_WIDGET *wgt, AL_CONST AWE_EVENT *event);


/** the default implementation of the relevant event; it sends the event to
    the widget's parent
    @param wgt widget to process the event
    @param event the event to process
 */
void awe_widget_key_down(AWE_WIDGET *wgt, AL_CONST AWE_EVENT *event);


/** the default implementation of the relevant event; it sends the event to
    the widget's parent
    @param wgt widget to process the event
    @param event the event to process
 */
void awe_widget_key_up(AWE_WIDGET *wgt, AL_CONST AWE_EVENT *event);


/** the default implementation of the get_focus method
    @param wgt widget that is called
    @return non-zero if widget gets the focus successfully
 */
int awe_widget_get_focus(AWE_WIDGET *wgt);


/** the default implementation of the loose_focus method
    @param wgt widget that is called
    @return non-zero if widget looses the focus
 */
int awe_widget_loose_focus(AWE_WIDGET *wgt);


/** the default implementation of the begin_display method; calls the
    relevant method of the children
    @param wgt widget that is called
 */
void awe_widget_begin_display(AWE_WIDGET *wgt);


/** the default implementation of the end_display method; calls the
    relevant method of the children
    @param wgt widget that is called
 */
void awe_widget_end_display(AWE_WIDGET *wgt);


/** the default implementation of the 'insert_widget' method
    @param wgt parent widget 
    @param child child widget
    @param z_order z-order to place the child to
    @return non-zero if the child was successfully inserted
 */
int awe_widget_insert_widget(AWE_WIDGET *wgt, AWE_WIDGET *child, int z_order);


/** the default implementation of the 'remove_widget' method
    @param wgt parent widget
    @param child child widget
    @return non-zero if the child was removed successfully
 */
int awe_widget_remove_widget(AWE_WIDGET *wgt, AWE_WIDGET *child);


/** casts 'widget' to 'object'.
    @param wgt widget to get the widget of
    @return pointer to object that is the widget
 */
AWE_OBJECT *awe_get_widget_object(AWE_WIDGET *wgt);


/** returns the parent widget
    @param wgt widget to get the parent of
    @return pointer to parent widget or NULL if the widget is orphan
 */
AWE_WIDGET *awe_get_parent_widget(AWE_WIDGET *wgt);


/** returns the previous sibling widget; the previous sibling is the one
    that is at a z-order greater than the z-order of the given widget. This
    widget is drawn on top of its previous sibling
    @param wgt widget to get the previous sibling of
    @return pointer to previous sibling widget or NULL if the widget is the
            bottom child of its parent
 */
AWE_WIDGET *awe_get_prev_sibling_widget(AWE_WIDGET *wgt);


/** returns the next sibling widget; the next sibling is the one
    that is at a z-order lesser than the z-order of the given widget. This
    widget is drawn below its next sibling
    @param wgt widget to get the next sibling of
    @return pointer to next sibling widget or NULL if the widget is the
            bottom child of its parent
 */
AWE_WIDGET *awe_get_next_sibling_widget(AWE_WIDGET *wgt);


/** returns the first child widget; the first child is the bottom child in 
    the z-order, i.e. the one that is drawn first (widgets are drawn from back
    to front)
    @param wgt widget to get the first child of
    @return pointer to first child widget or NULL if the given widget has no children
 */
AWE_WIDGET *awe_get_first_child_widget(AWE_WIDGET *wgt);


/** returns the last child widget; the last child is the top child in 
    the z-order, i.e. the one that is drawn last (widgets are drawn from back
    to front) and on top of all the others
    @param wgt widget to get the last child of
    @return pointer to last child widget or NULL if the given widget has no children
 */
AWE_WIDGET *awe_get_last_child_widget(AWE_WIDGET *wgt);


/** returns the number of children widgets
    @param wgt parent widget to get the number of children
    @return the number of children widgets the given widget has
 */
int awe_get_widget_count(AWE_WIDGET *wgt);


/** retrieves the z-order of a widget
    @param wgt the widget to get the z-order of
    @return the widget's z-order
 */
int awe_get_widget_z_order(AWE_WIDGET *wgt);


/** checks if given widget's ancestors are enabled. Widgets appear disabled
    if the user disables them or if an ancestor of them is disabled. This gives
    the user the capability of disabling a whole bunch of widgets by disabling
    their container.
    @param wgt widget to test if all its ancestors are enabled
    @return non-zero if all ancestors of given widget are enabled
 */
int awe_is_enabled_widget_tree(AWE_WIDGET *wgt);


/** checks if the given widget is on the screen
    @param wgt widget to check
    @return non-zero if widget is on the screen
 */
int awe_is_on_screen_widget(AWE_WIDGET *wgt);


/** checks if a widget is ancestor of another widget
    @param ancestor widget to check if ancestor
    @param descentant widget to check if descentant
    @return non-zero if 'descentant' widget is really a descentant of given ancestor
 */
int awe_is_ancestor_widget(AWE_WIDGET *ancestor, AWE_WIDGET *descentant);


/** returns a pointer to the screen rectangle of the widget. The pointer can
    be kept around as long as the widget is around
    @param wgt widget to get the screen rectangle of
    @return pointer to the widget's screen rectangle
 */
AL_CONST AWE_RECT *awe_get_widget_rect(AWE_WIDGET *wgt);


/** tests if given coordinates are inside the given widget; if the widget
    defines the 'hit_test' method, it is called
    @param wgt widget to test
    @param x horizontal coordinate relative to the widget
    @param y vertical coordinate relative to the widget
    @return non-zero if coordinate inside the widget
 */
int awe_widget_includes_point(AWE_WIDGET *wgt, int x, int y);


/** returns a widget from given point; the result might be the given widget,
    or a descentant widget, or none if the 'hit_test' method returns false.
    @param wgt root widget of the widget tree to search; must be on-screen
    @param x horizontal coordinate relative to the widget
    @param y vertical coordinate relative to the widget
    @return the widget at specified coordinates or null if not found
 */
AWE_WIDGET *awe_get_widget_from_point(AWE_WIDGET *wgt, int x, int y);


/** checks if an widget is or derives from a specific class
    @param wgt widget to check
    @param name NULL-terminated C string of the class' name
    @param pnamespace NULL-terminated C string of the class' namespace
    @return non-zero if the given widget derives from or is of given class
 */
int awe_widget_is_class(AWE_WIDGET *wgt, const char *name, const char *pnamespace);


/** retrieves an interface (a vtable pointer) from an widget, calling the
    relevant method
    @param wgt widget to get an interface of
    @param name NULL-terminated C string name of the interface's name
    @param pnamespace NULL-terminated C string name of the interface's namespace
    @return a pointer to the requested interface or NULL if the interface is 
            not supported
 */
void *awe_get_widget_interface(AWE_WIDGET *wgt, const char *name, const char *pnamespace);


/** returns the class of an widget
    @param wgt widget to get the class of
    @return the widget's class
 */
AWE_CLASS *awe_get_widget_class(AWE_WIDGET *wgt);


/** registers an widget as a listener to an event of another widget. The
    binding between event producer and event listener is automatically removed
    when one of the two widgets is destroyed.
    @param src 'source widget'; it is the widget that will fire the event
    @param name name of the event; same as the one provided by the class
    @param dst 'destination widget'; optional pointer to widget to call when the event happens;
           if you would like to pass a function instead of a method, pass NULL
    @param proc address of procedure to call when the event is fired; the
           first parameter of the widget must be AWE_WIDGET*; the rest of
           the parameters must match those of the event
    @return non-zero if the event is added successfully
 */
int awe_add_widget_event(AWE_WIDGET *src, const char *name, AWE_WIDGET *dst, void *proc);


/** unregisters an widget as listener of an event that is produced by another
    widget.
    when one of the two widgets is destroyed.
    @param src 'source widget'; it is the widget that will fire the event
    @param name name of the event; same as the one provided by the class
    @param dst 'destination widget'; widget to call when the event happens; same value
           as passed during registration; pass NULL for functions
    @param proc address of procedure to call when the event is fired
    @return non-zero if the event is removed successfully
 */
int awe_remove_widget_event(AWE_WIDGET *src, const char *name, AWE_WIDGET *dst, void *proc);


/** macro that calls an widget's event with 0 parameters
    @param WGT pointer to AWE_WIDGET derived widget to fire the event from
    @param NAME name of the event, as registered by the widget's class
 */
#define awe_do_widget_event0(WGT, NAME)\
    awe_do_object_event0(WGT, NAME)


/** macro that calls an widget's event with 1 parameter
    @param WGT pointer to AWE_WIDGET derived widget to fire the event from
    @param NAME name of the event, as registered by the widget's class
    @param P1 1st parameter
 */
#define awe_do_widget_event1(WGT, NAME, P1)\
    awe_do_object_event1(WGT, NAME, P1)


/** macro that calls an widget's event with 2 parameters
    @param WGT pointer to AWE_WIDGET derived widget to fire the event from
    @param NAME name of the event, as registered by the widget's class
    @param P1 1st parameter
    @param P2 2nd parameter
 */
#define awe_do_widget_event2(WGT, NAME, P1, P2)\
    awe_do_object_event2(WGT, NAME, P1, P2)


/** macro that calls an widget's event with 3 parameters
    @param WGT pointer to AWE_WIDGET derived widget to fire the event from
    @param NAME name of the event, as registered by the widget's class
    @param P1 1st parameter
    @param P2 2nd parameter
    @param P3 3rd parameter
 */
#define awe_do_widget_event3(WGT, NAME, P1, P2, P3)\
    awe_do_object_event3(WGT, NAME, P1, P2, P3)


/** macro that calls an widget's event with 4 parameters
    @param WGT pointer to AWE_WIDGET derived widget to fire the event from
    @param NAME name of the event, as registered by the widget's class
    @param P1 1st parameter
    @param P2 2nd parameter
    @param P3 3rd parameter
    @param P4 4th parameter
 */
#define awe_do_widget_event4(WGT, NAME, P1, P2, P3, P4)\
    awe_do_object_event4(WGT, NAME, P1, P2, P3, P4)


/** macro that calls an widget's event with 5 parameters
    @param WGT pointer to AWE_WIDGET derived widget to fire the event from
    @param NAME name of the event, as registered by the widget's class
    @param P1 1st parameter
    @param P2 2nd parameter
    @param P3 3rd parameter
    @param P4 4th parameter
    @param P5 5th parameter
 */
#define awe_do_widget_event5(WGT, NAME, P1, P2, P3, P4, P5)\
    awe_do_object_event5(WGT, NAME, P1, P2, P3, P4, P5)


/** macro that calls an widget's event with 6 parameters
    @param WGT pointer to AWE_WIDGET derived widget to fire the event from
    @param NAME name of the event, as registered by the widget's class
    @param P1 1st parameter
    @param P2 2nd parameter
    @param P3 3rd parameter
    @param P4 4th parameter
    @param P5 5th parameter
    @param P6 6th parameter
 */
#define awe_do_widget_event6(WGT, NAME, P1, P2, P3, P4, P5, P6)\
    awe_do_object_event6(WGT, NAME, P1, P2, P3, P4, P5, P6)


/** macro that calls an widget's event with 7 parameters
    @param WGT pointer to AWE_WIDGET derived widget to fire the event from
    @param NAME name of the event, as registered by the widget's class
    @param P1 1st parameter
    @param P2 2nd parameter
    @param P3 3rd parameter
    @param P4 4th parameter
    @param P5 5th parameter
    @param P6 6th parameter
    @param P7 7th parameter
 */
#define awe_do_widget_event7(WGT, NAME, P1, P2, P3, P4, P5, P6, P7)\
    awe_do_object_event7(WGT, NAME, P1, P2, P3, P4, P5, P6, P7)


/** macro that calls an widget's event with 8 parameters
    @param WGT pointer to AWE_WIDGET derived widget to fire the event from
    @param NAME name of the event, as registered by the widget's class
    @param P1 1st parameter
    @param P2 2nd parameter
    @param P3 3rd parameter
    @param P4 4th parameter
    @param P5 5th parameter
    @param P6 6th parameter
    @param P7 7th parameter
    @param P8 8th parameter
 */
#define awe_do_widget_event8(WGT, NAME, P1, P2, P3, P4, P5, P6, P7, P8)\
    awe_do_object_event8(WGT, NAME, P1, P2, P3, P4, P5, P6, P7, P8)


/** macro that calls an widget's event with 9 parameters
    @param WGT pointer to AWE_WIDGET derived widget to fire the event from
    @param NAME name of the event, as registered by the widget's class
    @param P1 1st parameter
    @param P2 2nd parameter
    @param P3 3rd parameter
    @param P4 4th parameter
    @param P5 5th parameter
    @param P6 6th parameter
    @param P7 7th parameter
    @param P8 8th parameter
    @param P9 9th parameter
 */
#define awe_do_widget_event9(WGT, NAME, P1, P2, P3, P4, P5, P6, P7, P8, P9)\
    awe_do_object_event9(WGT, NAME, P1, P2, P3, P4, P5, P6, P7, P8, P9)


/** macro that calls an widget's event with 10 parameters
    @param WGT pointer to AWE_WIDGET derived widget to fire the event from
    @param NAME name of the event, as registered by the widget's class
    @param P1 1st parameter
    @param P2 2nd parameter
    @param P3 3rd parameter
    @param P4 4th parameter
    @param P5 5th parameter
    @param P6 6th parameter
    @param P7 7th parameter
    @param P8 8th parameter
    @param P9 9th parameter
    @param P10 10th parameter
 */
#define awe_do_widget_event10(WGT, NAME, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10)\
    awe_do_object_event10(WGT, NAME, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10)


/** creates a widget from given COM class and properties. 
    @param pclass COM class of widget to create
    @param parent parent widget; may be NULL
    @param params variable-argument NULL-terminated list of widget properties
    @return pointer to created widget or NULL if the widget could not be created,
            due to not enough memory, wrong class being passed etc.
 */
AWE_WIDGET *awe_create_widget_va(AWE_CLASS *pclass, AWE_WIDGET *parent, va_list params);


/** creates a widget from given COM class and properties. 
    @param pclass COM class of widget to create
    @param parent parent widget; may be NULL
    @param ... variable-argument NULL-terminated list of widget properties
    @return pointer to created widget or NULL if the widget could not be created,
            due to not enough memory, wrong class being passed etc.
 */
AWE_WIDGET *awe_create_widget(AWE_CLASS *pclass, AWE_WIDGET *parent, ...);


/** creates a widget from named class and properties. 
    @param name name of the class to create
    @param pnamespace namespace that the class belongs    
    @param parent parent widget; may be NULL
    @param ... variable-argument NULL-terminated list of widget properties
    @return pointer to created widget or NULL if the widget could not be created,
            due to not enough memory, wrong class being passed etc.
 */
AWE_WIDGET *awe_create_widget_by_name(const char *name, const char *pnamespace, AWE_WIDGET *parent, ...);


/** clones a widget
    @param wgt widget to clone
    @return a duplicate of given widget or NULL if it could not be created
 */
AWE_WIDGET *awe_clone_widget(AWE_WIDGET *wgt);


/** destroys a widget. The widget will be removed from screen; children widgets
    will also be destroyed.
    @param wgt the widget to destroy
 */
void awe_destroy_widget(AWE_WIDGET *wgt);


/** retrieves the properties of a widget
    @param wgt widget to retrieve the properties of
    @param params variable-argument NULL-terminated list of variables to receive
                  the widget's properties
    @return number of properties processed
 */
int awe_get_widget_properties_va(AWE_WIDGET *wgt, va_list params);


/** retrieves the properties of a widget
    @param wgt widget to retrieve the properties of
    @param ... variable-argument NULL-terminated list of variables to receive
               the widget's properties
    @return number of properties processed
 */
int awe_get_widget_properties(AWE_WIDGET *wgt, ...);


/** sets the properties of a widget
    @param wgt widget to set the properties of
    @param params variable-argument NULL-terminated list of widget properties to set
    @return number of properties processed
 */
int awe_set_widget_properties_va(AWE_WIDGET *wgt, va_list params);


/** sets the properties of a widget
    @param wgt widget to set the properties of
    @param ... variable-argument NULL-terminated list of widget properties to set
    @return number of properties processed
 */
int awe_set_widget_properties(AWE_WIDGET *wgt, ...);


/** sets the properties of a widget tree
    @param wgt widget to set the properties of
    @param params variable-argument NULL-terminated list of widget properties to set
    @return number of properties processed
 */
int awe_set_widget_tree_properties_va(AWE_WIDGET *wgt, va_list params);


/** sets the properties of a widget tree
    @param wgt widget to set the properties of
    @param ... variable-argument NULL-terminated list of widget properties to set
    @return number of properties processed
 */
int awe_set_widget_tree_properties(AWE_WIDGET *wgt, ...);


/** returns the current root widget
    @return the current root widget or NULL if no root widget has been set
 */
AWE_WIDGET *awe_get_root_widget();


/** sets the root widget. At any given point in time, there is only one root
    widget. The root widget is the top-level widget that usually occupies all
    the screen and contains all the other widgets. If there is another
    widget as root, it is stopped being root. Only widgets belonging to the
    root tree participate in event handling.
    @param wgt the root widget; if NULL, the current root widget is reset
    @return zero if the root widget could not be set, because given widget is
            disabled
 */
int awe_set_root_widget(AWE_WIDGET *wgt);


/** returns the current focus widget
    @return the current focus widget or NULL if no focus widget has been set
 */
AWE_WIDGET *awe_get_focus_widget();


/** sets the focus widget. At any given point in time, there is only one focus
    widget. The focus widget is widget that receives the input events. The
    function calls the relevant method of the widget vtable, which in turn
    may call another widget to let the input focus go.
    @param wgt the focus widget
    @return non-zero if the focus is set successfully; zero if the widget that
            currently has the focus denied to let the focus go or if the given
            widget is disabled or not on the screen
 */
int awe_set_focus_widget(AWE_WIDGET *wgt);


/** sets up widgets that fall inside the given rectangle to be updated the
    next time the GUI is updated
    @param wgt the widget to update
    @param r pointer to rectangle to update; only widgets that fall inside this
           rectangle will be updated; it must be relative to the widget
 */
void awe_set_widget_dirty_rect(AWE_WIDGET *wgt, AWE_RECT *r);


/** sets up the given widget to be updated the next time the GUI is updated
    @param wgt the widget to update
 */
void awe_set_widget_dirty(AWE_WIDGET *wgt);


/** inserts a widget as a child into another widget. It calls the relevant
    method of the parent widget.
    @param parent parent widget
    @param child child widget
    @param z_order z-order to insert the child widget at; if 0, the child widget
           is inserted above all other children; if < 0, the child
           widget is inserted below all other children. Any other value results
           in the widget being inserted in between
    @return non-zero if the widget was inserted successfully
 */
int awe_insert_widget(AWE_WIDGET *parent, AWE_WIDGET *child, int z_order);


/** same as 'insert', only that it places the child widget at z == 0
    @param parent parent widget
    @param child child widget
    @return non-zero if the widget was inserted successfully
 */
int awe_add_widget(AWE_WIDGET *parent, AWE_WIDGET *child);


/** removes a widget from its parent
    @param wgt widget to remove from its parent
    @return non-zero if the widget was removed successfully
 */
int awe_remove_widget(AWE_WIDGET *wgt);


/** sets a widget's z-order
    @param wgt widget to set the z-order of; it must be a child widget
    @param z_order z-order to insert the child widget at; if 0, the child widget
           is inserted above all other children; if < 0, the child
           widget is inserted below all other children. Any other value results
           in the widget being inserted in between
 */
void awe_set_widget_z_order(AWE_WIDGET *wgt, int z_order);


/** maps a series of points from the coordinate system of one widget to the other
    @param src widget of the source coordinates; if null, the input coordinates
           are considered relative to the screen
    @param dst widget of the destination coordinates; if null, the output coordinates
           are relative to the screen
    @param num_points number of points to map
    @param src_points input points (pairs of x, y coordinates) to map
    @param dst_points output points to map; can be source
 */
void awe_map_points(AWE_WIDGET *src, AWE_WIDGET *dst, int num_points, int src_points[], int dst_points[]);


/** maps a single point from the coordinate system of one widget to the other
    @param src widget of the source coordinates; if null, the input coordinates
           are considered relative to the screen
    @param dst widget of the destination coordinates; if null, the output coordinates
           are relative to the screen
    @param in_x x coordinate of input point
    @param in_y y coordinate of input point
    @param out_x x coordinate of output point to store the result
    @param out_y y coordinate of output point to store the result
 */
void awe_map_point(AWE_WIDGET *src, AWE_WIDGET *dst, int in_x, int in_y, int *out_x, int *out_y);


/** maps a rectangle from the coordinate system of one widget to the other
    @param src widget of the source coordinates; if null, the input coordinates
           are considered relative to the screen
    @param dst widget of the destination coordinates; if null, the output coordinates
           are relative to the screen
    @param in_r input rectangle
    @param out_r output rectangle; can be the same as the input
 */
void awe_map_rect(AWE_WIDGET *src, AWE_WIDGET *dst, AWE_RECT *in_r, AWE_RECT *out_r);


/** sets up the GUI to be dirty; only widgets that fall inside the given rectangle
    are dirty
    @param r pointer to rectangle to update; only widgets that fall inside this
           rectangle will be updated; it is relative to the gui screen's top-left corner
 */
void awe_set_gui_dirty_rect(AWE_RECT *r);


/** sets up the whole GUI to be updated
 */
void awe_set_gui_dirty();


/** returns the GUI screen, ie the screen to be used for drawing the GUI.
 */
BITMAP *awe_get_gui_screen();


/** sets the GUI screen, ie the screen to be used for drawing the GUI.
    @param bmp bitmap to draw the gui later onto
 */
void awe_set_gui_screen(BITMAP *bmp);


/** returns the gui's update mode.
 */
AWE_GUI_UPDATE_MODE awe_get_gui_update_mode();


/** sets the gui's update mode
    @param mode the gui's update mode
    @param drs_proc optional dirty rectangle system procedure; if set, and the
           system draws changes only, it is called for each rectangle being
           drawn. 
 */
void awe_set_gui_update_mode(AWE_GUI_UPDATE_MODE mode, AWE_DRS_PROC drs_proc);


/** updates the GUI. It draws either the changes or all widgets, according to
    the update mode. The changes are drawn on the GUI screen.
 */
void awe_update_gui();


/** returns the default output type for the widgets
 */
AWE_WIDGET_OUTPUT_TYPE awe_get_widget_output_type();


/** sets the default output type of the widgets; the rest of the widgets created
    from now on will be of the given output type
    @param type the new default output type
 */
void awe_set_widget_output_type(AWE_WIDGET_OUTPUT_TYPE type);


/** sets or resets a widget timer; if the widget is destroyed, timers will be
    automatically removed
    @param wgt widget to receive the timer event
    @param id id of timer event; user defined
    @param msecs interval of the timer in miliseconds
    @return zero if there are no more timer resources left
 */
int awe_add_widget_timer(AWE_WIDGET *wgt, int id, int msecs);


/** removes a widget timer
    @param wgt widget that receives the timer event
    @param id id of timer event; same as the one passed when the timer was set
    @return zero if the timer is not found
 */
int awe_remove_widget_timer(AWE_WIDGET *wgt, int id);


/** special function for setting a widget's geometry directly without the
    widget being notified. It is provided for geometry management algorithms
    in order to avoid infinite recursion, when it is not desirable to 
    notify the resized widget.
    @param wgt widget to set the geometry of
    @param x left coordinate relative to parent (or to screen if root)
    @param y top coordinate relative to parent (or to screen if root)
    @param width width of widget
    @param height height of widget
    @return zero if supplied geometry is the same as the current one; used
            in knowing when a widget's geometry is actually changed
 */
int awe_override_widget_geometry(AWE_WIDGET *wgt, int x, int y, int width, int height);


/** same as 'awe_override_widget_geometry', but for position only
    @param wgt widget to set the geometry of
    @param x left coordinate relative to parent (or to screen if root)
    @param y top coordinate relative to parent (or to screen if root)
    @return zero if supplied geometry is the same as the current one; used
            in knowing when a widget's geometry is actually changed
 */
int awe_override_widget_position(AWE_WIDGET *wgt, int x, int y);


/** same as 'awe_override_widget_geometry', but for size only
    @param wgt widget to set the geometry of
    @param width width of widget
    @param height height of widget
    @return zero if supplied geometry is the same as the current one; used
            in knowing when a widget's geometry is actually changed
 */
int awe_override_widget_size(AWE_WIDGET *wgt, int width, int height);


/*@}*/


#ifdef __cplusplus
   }
#endif


#endif //AWE_WIDGET_H
