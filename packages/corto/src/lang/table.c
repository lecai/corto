/* $CORTO_GENERATED
 *
 * table.c
 *
 * Only code written between the begin and end tags will be preserved
 * when the file is regenerated.
 */

#include <corto/lang/lang.h>

corto_int16 _corto_table_construct(
    corto_table this)
{
/* $begin(corto/lang/table/construct) */

    corto_setref(&corto_type(this)->options.parentType, corto_tablescope_o);

    return corto_class_construct(this);
/* $end */
}
