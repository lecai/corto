/* cx_iterator.h
 *
 * This file contains generated code. Do not modify!
 */

#ifndef cx_iterator_H
#define cx_iterator_H

#include "cortex.h"
#include "cx_type.h"
#include "cx__type.h"

#include "cx__api.h"

#include "cx__meta.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ::cortex::lang::iterator::hasNext() */
cx_bool cx_iterator_hasNext(cx_any _this);

/* ::cortex::lang::iterator::init() */
cx_int16 cx_iterator_init(cx_iterator _this);

/* ::cortex::lang::iterator::next() */
cx_any cx_iterator_next(cx_any _this);

#ifdef __cplusplus
}
#endif
#endif
