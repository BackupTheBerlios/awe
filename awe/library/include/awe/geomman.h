#ifndef AWE_GEOMMAN_H
#define AWE_GEOMMAN_H


#include "widget.h"


#ifdef __cplusplus
   extern "C" {
#endif


/**@name Geometry Management
    <p> The geometry management module is responsible for providing the
        interface for automating geometry management. The concept of 
        geometry management revolves around parent-child widget relationships:
        a parent widget is responsible for managing the positioning of a 
        child widget. By implementing the geometry manager interface, a widget
        can specify its preferred geometry and a layout for its children.
    </p>
    <p> Besides the geometry manager interface, the module offers standard
        implementations of methods that are used for geometry management.
    </p>
 */
/*@{*/


///AWE id of the geometry manager interface
#define AWE_ID_GEOMETRY_MANAGER        "GeometryManager"


/** the interface that widgets should implement if they wish to provide
    geometry management.
 */
struct AWE_GEOMETRY_MANAGER_VTABLE {
    /** optional method that is called from children widgets to notify the
        parent that some attribute that affects geometry has been changed.
        The default implementation first initializes the sizes of widgets in the
        child tree by calling the 'set_geometry' method of the child,
        then calls the method 'do_layout'. The parent widget could also chose
        to do other actions:
        <li>do nothing; the child widget will have the geometry it currently has</li>
        <li>resize itself, by changing the geometry properties (x, y, width, height).
            If the parent is inside a geometry manager widget, the geometry
            management will be propagated upwards.
        </li>
        <li> specify a new geometry for the child. The parent should not change
             the child's geometry properties, but rather use the function
             'awe_override_widget_geometry' to avoid infinite recursion.
        </li>
        @param parent the parent widget
        @param child the child widget that has been changed
     */
    void (*manage_child)(AWE_WIDGET *parent, AWE_WIDGET *child);

    /** optional method for specifying the preferred geometry. The default
        implementation calls the relevant method of the children. The widget
        must use the function 'awe_override_widget_geometry' to specify its
        geometry. This function is usually called before the relevant function
        of the parent, so as that a parent widget can specify its preferred
        size relative to its children (i.e. to wrap itself around its children).
        @param wgt the widget to specify its preferred geometry
     */
    void (*set_geometry)(AWE_WIDGET *wgt);

    /** optional method for specifying the layout of the children of the given
        widgets. The function 'awe_override_widget_geometry' should be used
        on children in order to avoid infinite recursion. There is no default
        implementation. The method is called from the parent, after the final
        size of the widget has been set.
        @param wgt the widget to specify the layout of
     */
    void (*do_layout)(AWE_WIDGET *wgt);

};
typedef struct AWE_GEOMETRY_MANAGER_VTABLE AWE_GEOMETRY_MANAGER_VTABLE;


/** When a widget is root and a geometry manager, it does geometry initialization
    by first calling its own 'set_geometry' method, then calling its own 'do_layout'
    method. Thus, when a root widget is displayed, all widgets in the tree
    that are geometry managers take the appropriate position and size. The
    geometry initialization takes place after the 'begin_display' method of
    children has been called.
    @param wgt widget that is about to be displayed
 */
void awe_geometry_manager_begin_display(AWE_WIDGET *wgt);


/** When a widget is inserted in a geometry manager, the geometry manager
    calls its own 'manage_child' method with the 'child' parameter set to the
    newly inserted child. Therefore the widget insertion results into geometry
    being managed throughout the widget tree.
    @param wgt widget that is to become parent
    @param child widget that is about to become child
    @param z_order z-order that the child widget should be inserted at:
           if 0, child widget is inserted on top; if < 0, child widget is
           inserted below all its siblings; other values are middle positions.
    @return zero if given widget could not be inserted as a child, due to
            being already a child of another widget, or is the root widget,
            or an ancestor of the parent widget
 */
int awe_geometry_manager_insert_widget(AWE_WIDGET *wgt, AWE_WIDGET *child, int z_order);


/** When a widget is removed, it calls its own 'do_layout' method.
    @param wgt parent widget
    @param child child widget
    @return non-zero if the child widget was succesfully removed
 */
int awe_geometry_manager_remove_widget(AWE_WIDGET *wgt, AWE_WIDGET *child);


/** It notifies its parent that it must be managed by calling the 'manage_child'
    method of its geometry manager parent.
    @param wgt the widget that is affected
    @param x the widget's left coordinate relative to its parent (or the screen if it is root)
    @param y the widget's top coordinate relative to its parent (or the screen if it is root)
    @param width the widget's width
    @param height the widget's height
 */
void awe_geometry_manager_geometry_changed(AWE_WIDGET *wgt, int x, int y, int width, int height);


/** It notifies its parent that it must be managed by calling the 'manage_child'
    method of its geometry manager parent.
    @param wgt the widget that is affected
    @param z the new z-order
 */
void awe_geometry_manager_z_order_changed(AWE_WIDGET *wgt, int z);


/** It notifies its parent that it must be managed by calling the 'manage_child'
    method of its geometry manager parent.
    @param wgt the widget that is affected
    @param z the new visible state
 */
void awe_geometry_manager_visible_changed(AWE_WIDGET *wgt, int visible);


/** the default implementation of the manage_child method; first it calls
    the 'set_geometry' method of the children, then calls the 'do_layout'
    method.
    @param wgt the widget that is affected
    @param child child that was affected
 */
void awe_geometry_manager_manage_child(AWE_WIDGET *wgt, AWE_WIDGET *child);


/** the default implementation of the set_geometry method of the geometry manager;
    it calls the relevant method of children. Derived implementations should
    call this function before selecting their own geometry.
    @param wgt the widget that is affected
 */
void awe_geometry_manager_set_geometry(AWE_WIDGET *wgt);


/** the default implementation of the do_layout method of the geometry manager;
    it calls the relevant method of children. Derived implementations should
    call this function after setting their children to the size defined by
    the layout.
    @param wgt the widget that is affected
 */
void awe_geometry_manager_do_layout(AWE_WIDGET *wgt);


/** causes manual geometry management. At first, it calls the 'set_geometry'
    method of the given widget. Then, if the widget has a parent, the parent
    is called to manage the given child widget. Otherwise, the 'do_layout'
    method is called. This method should be called from the 'properties_changed'
    method in order to cause geometry management.
    @param wgt widget to start geometry management from.
 */
void awe_manage_geometry(AWE_WIDGET *wgt);


/*@}*/


#ifdef __cplusplus
   }
#endif


#endif //AWE_GEOMMAN_H
