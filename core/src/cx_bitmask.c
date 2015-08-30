/* cx_bitmask.c
 *
 * This file contains the implementation for the generated interface.
 *
 * Don't mess with the begin and end tags, since these will ensure that modified
 * code in interface functions isn't replaced when code is re-generated.
 */

#include "cx.h"

/* $header() */
#include "cx__bitmask.h"
cx_int16 cx__bitmask_bindConstant(cx_bitmask _this, cx_constant* c) {
    if (cx_checkState(cx_type_o, CX_DEFINED)) {
        *c = 0x1 << (cx_scopeSize(_this)-1);
    }
    cx_enum(_this)->constants.buffer = cx_realloc(cx_enum(_this)->constants.buffer, (cx_enum(_this)->constants.length+1) * sizeof(cx_constant*));
    cx_enum(_this)->constants.buffer[cx_enum(_this)->constants.length] = c;
    cx_enum(_this)->constants.length++;
    
    cx_claim(c);

    return 0;
}
/* $end */

/* ::corto::lang::bitmask::init() */
cx_int16 cx_bitmask_init(cx_bitmask _this) {
/* $begin(::corto::lang::bitmask::init) */
    cx_primitive(_this)->kind = CX_BITMASK;
    cx_primitive(_this)->width = CX_WIDTH_32;
    cx_setref(&cx_type(_this)->defaultType, cx_constant_o);
    return cx_primitive_init((cx_primitive)_this);
/* $end */
}