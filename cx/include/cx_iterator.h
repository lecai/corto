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

/* ::cortex::lang::iterator::advance() */
cx_any cx_iterator_advance(cx_any _this);

/* callback ::cortex::lang::type::init(lang::object object) -> ::cortex::lang::iterator::init(lang::iterator object) */
cx_int16 cx_iterator_init(cx_iterator object);

/* ::cortex::lang::iterator::retrieve() */
cx_any cx_iterator_retrieve(cx_any _this);

#ifdef __cplusplus
}
#endif
#endif

