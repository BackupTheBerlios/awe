#ifndef AWE_RECT_H
#define AWE_RECT_H


#include <allegro.h>


#ifdef __cplusplus
   extern "C" {
#endif


/**@name Rect
    Struct that represents a rectangle. It has left, top, right and bottom
    members. The module also has several macros for getting various properties,
    for hit testing, intersection and union.
 */
/*@{*/


///The rect structure
struct AWE_RECT {
    ///left coordinate
    short left;

    ///top coordinate
    short top;

    ///right coordinate
    short right;

    ///bottom coordinate
    short bottom;
};
typedef struct AWE_RECT AWE_RECT;


/** macro that calculates the width of a rectangle
    @param R rectangle to calculate the width of
    @return the width of the rectangle
 */
#define AWE_RECT_WIDTH(R)    ((R).right - (R).left + 1)


/** macro that calculates the height of a rectangle
    @param R rectangle to calculate the height of
    @return the height of the rectangle
 */
#define AWE_RECT_HEIGHT(R)   ((R).bottom - (R).top + 1)


/** macro that calculates the horizontal center of a rectangle
    @param R rectangle to calculate the horizontal center of
    @return the horizontal center coordinate of a rectangle
 */
#define AWE_RECT_CENTER_HOR(R)    ((R).left + (AWE_RECT_WIDTH(R) / 2))


/** macro that calculates the vertical center of a rectangle
    @param R rectangle to calculate the vertical center of
    @return the vertical center coordinate of a rectangle
 */
#define AWE_RECT_CENTER_VER(R)    ((R).top + (AWE_RECT_HEIGHT(R) / 2))


/** macro that calculates if the rectangle is normalized, i.e. if left and top
    are actually at left and top of the right and bottom coordinates,
    respectivelly.
    @param R rectangle to test
    @return non-zero if the rectangle is normalized
 */
#define AWE_RECT_NORMAL(R)   ((R).left <= (R).right && (R).top <= (R).bottom)


/** macro that checks if a X, Y point lies within the rectangle
    @param R rectangle to check
    @param X horizontal coordinate
    @param Y vertical coordinate
    @return non-zero if given X, Y point lies within the rectangle
 */
#define AWE_RECT_INCLUDE(R, X, Y)\
    ((X) >= (R).left && (X) <= (R).right && (Y) >= (R).top && (Y) <= (R).bottom)


/** macro that checks if a rectangle overlaps with another rectangle
    @param R1 first rectangle
    @param R2 second rectangle
    @return non-zero if the two rectangles overlap
 */
#define AWE_RECT_OVERLAP(R1, R2)\
    ((R1).left   <= (R2).right  &&\
     (R1).top    <= (R2).bottom &&\
     (R1).right  >= (R2).left   &&\
     (R1).bottom >= (R2).top)


/** macro that sets a rectangle's coordinates 
    @param R rectangle to set the coordinates of
    @param LEFT left coordinate
    @param TOP top coordinate
    @param RIGHT right coordinate
    @param BOTTOM bottom coordinate
 */
#define AWE_RECT_SET(R, LEFT, TOP, RIGHT, BOTTOM) {\
    (R).left   = LEFT  ;\
    (R).top    = TOP   ;\
    (R).right  = RIGHT ;\
    (R).bottom = BOTTOM;\
}


/** macro that sets a rectangle from position and size
    @param R rectangle to set the coordinates of
    @param X horizontal coordinate
    @param Y vertical coordinate
    @param WIDTH width
    @param HEIGHT height
 */
#define AWE_RECT_SET_S(R, X, Y, WIDTH, HEIGHT) {\
    int right  = (X) + (WIDTH)  - 1;\
    int bottom = (Y) + (HEIGHT) - 1;\
    AWE_RECT_SET(R, X, Y, right, bottom);\
}


/** macro that calculates the intersection of two rectangles, and stores the
    result in another rectangle.
    @param R rectangle to store the result of calculation to; it can be any of
             the input rectangles
    @param A first input rectangle
    @param B second input rectangle
 */
#define AWE_RECT_INTERSECTION(R, A, B) {\
    (R).left   = MAX((A).left  , (B).left  );\
    (R).top    = MAX((A).top   , (B).top   );\
    (R).right  = MIN((A).right , (B).right );\
    (R).bottom = MIN((A).bottom, (B).bottom);\
}


/** macro that calculates the union of two rectangles, and stores the
    result in another rectangle.
    @param R rectangle to store the result of calculation to; it can be any of
             the input rectangles
    @param A first input rectangle
    @param B second input rectangle
 */
#define AWE_RECT_UNION(R, A, B) {\
    (R).left   = MIN((A).left  , (B).left  );\
    (R).top    = MIN((A).top   , (B).top   );\
    (R).right  = MAX((A).right , (B).right );\
    (R).bottom = MAX((A).bottom, (B).bottom);\
}


/** macro that shifts a rectangle by a specified amount
    @param R rectangle to shift
    @param DX horizontal amount of shift
    @param DY vertical amount of shift
 */
#define AWE_RECT_SHIFT(R, DX, DY) {\
    (R).left   += DX;\
    (R).top    += DY;\
    (R).right  += DX;\
    (R).bottom += DY;\
}


/** macro that moves a rectangle to a specified position
    @param R rectangle to move
    @param X new left position
    @param Y new top position
 */
#define AWE_RECT_MOVE(R, X, Y) {\
    int dx = (X) - (R).left;\
    int dy = (Y) - (R).top;\
    AWE_RECT_SHIFT(R, dx, dy);\
}


/** macro that invalidates (de-normalizes) a rectangle by setting 
    left, top to maximum allowed value and right, bottom to minimum allowed value
    @param R rectangle to invalidate
 */
#define AWE_RECT_INVALIDATE(R)\
    AWE_RECT_SET(R, SHRT_MAX, SHRT_MAX, SHRT_MIN, SHRT_MIN)


/*@}*/


#ifdef __cplusplus
   }
#endif


#endif //AWE_RECT_H
