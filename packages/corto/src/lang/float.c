/* $CORTO_GENERATED
 *
 * float.c
 *
 * Only code written between the begin and end tags will be preserved
 * when the file is regenerated.
 */

#include <corto/lang/lang.h>

corto_int16 _corto_float_init(
    corto_float this)
{
/* $begin(corto/lang/float/init) */
    corto_primitive(this)->kind = CORTO_FLOAT;
    return corto_primitive_init((corto_primitive)this);
/* $end */
}
