#include "geomman.h"


/*****************************************************************************
    PRIVATE
 *****************************************************************************/


//calls a geometry manager method
#define _DO(WGT, METHOD, PARAMS)\
    AWE_CALL_METHOD(WGT, AWE_ID_GEOMETRY_MANAGER, AWE_ID_AWE, AWE_GEOMETRY_MANAGER_VTABLE, METHOD, PARAMS)


//notifies parent
static int _notify_parent(AWE_WIDGET *wgt)
{
    AWE_WIDGET *parent = awe_get_parent_widget(wgt);
    if (!parent) return 0;
    _DO(parent, manage_child, (parent, wgt));
    return 1;
}


/*****************************************************************************
    PUBLIC
 *****************************************************************************/


//init root geometry
void awe_geometry_manager_begin_display(AWE_WIDGET *wgt)
{
    awe_widget_begin_display(wgt);
    if (awe_get_parent_widget(wgt)) return;
    awe_manage_geometry(wgt);
}


//insert managed child
int awe_geometry_manager_insert_widget(AWE_WIDGET *wgt, AWE_WIDGET *child, int z_order)
{
    if (!awe_widget_insert_widget(wgt, child, z_order)) return 0;
    _DO(wgt, manage_child, (wgt, child));
    return 1;
}


//remove managed child
int awe_geometry_manager_remove_widget(AWE_WIDGET *wgt, AWE_WIDGET *child)
{
    if (!awe_widget_remove_widget(wgt, child)) return 0;
    _DO(wgt, do_layout, (wgt));
    return 1;
}


//notify parent
void awe_geometry_manager_geometry_changed(AWE_WIDGET *wgt, int x, int y, int width, int height)
{
    _notify_parent(wgt);
}


//notify parent
void awe_geometry_manager_z_order_changed(AWE_WIDGET *wgt, int z)
{
    _notify_parent(wgt);
}


//notify parent
void awe_geometry_manager_visible_changed(AWE_WIDGET *wgt, int visible)
{
    _notify_parent(wgt);
}


//calls child->set_geometry, then do_layout
void awe_geometry_manager_manage_child(AWE_WIDGET *wgt, AWE_WIDGET *child)
{
    awe_manage_geometry(wgt);
}


//calls the relevant method of children
void awe_geometry_manager_set_geometry(AWE_WIDGET *wgt)
{
    AWE_WIDGET *child;

    for(child = awe_get_first_child_widget(wgt);
        child;
        child = awe_get_next_sibling_widget(child)) {
        _DO(child, set_geometry, (child));
    }
}


//calls the relevant method of children
void awe_geometry_manager_do_layout(AWE_WIDGET *wgt)
{
    AWE_WIDGET *child;

    for(child = awe_get_first_child_widget(wgt);
        child;
        child = awe_get_next_sibling_widget(child)) {
        _DO(child, do_layout, (child));
    }
}


//manual geometry management
void awe_manage_geometry(AWE_WIDGET *wgt)
{
    _DO(wgt, set_geometry, (wgt));
    if (!_notify_parent(wgt)) _DO(wgt, do_layout, (wgt));    
}
