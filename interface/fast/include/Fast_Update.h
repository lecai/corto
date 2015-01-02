/* Fast_Update.h
 *
 * This file contains generated code. Do not modify!
 */

#ifndef Fast_Update_H
#define Fast_Update_H

#include "cortex.h"
#include "Fast_Node.h"
#include "Fast__type.h"

#include "Fast__api.h"

#include "Fast__meta.h"

#ifdef __cplusplus
extern "C" {
#endif

/* callback ::cortex::lang::class::construct(lang::object object) -> ::cortex::Fast::Update::construct(Update object) */
cx_int16 Fast_Update_construct(Fast_Update object);

/* virtual ::cortex::Fast::Update::toIc(lang::alias{"cx_icProgram"} program,lang::alias{"cx_icStorage"} storage,lang::bool stored) */
cx_ic Fast_Update_toIc(Fast_Update _this, cx_icProgram program, cx_icStorage storage, cx_bool stored);

/* ::cortex::Fast::Update::toIc(lang::alias{"cx_icProgram"} program,lang::alias{"cx_icStorage"} storage,lang::bool stored) */
cx_ic Fast_Update_toIc_v(Fast_Update _this, cx_icProgram program, cx_icStorage storage, cx_bool stored);

#ifdef __cplusplus
}
#endif
#endif

