/* $CORTO_GENERATED
 *
 * lock.c
 *
 * Only code written between the begin and end tags will be preserved
 * when the file is regenerated.
 */

#include <corto/secure/secure.h>

corto_secure_accessKind _corto_secure_lock_authorize_v(
    corto_secure_lock this,
    corto_object object,
    corto_secure_actionKind action)
{
/* $begin(corto/secure/lock/authorize) */

    CORTO_UNUSED(this);
    CORTO_UNUSED(object);
    CORTO_UNUSED(action);

    return CORTO_SECURE_ACCESS_DENIED;
/* $end */
}

corto_int16 _corto_secure_lock_construct(
    corto_secure_lock this)
{
/* $begin(corto/secure/lock/construct) */

    CORTO_UNUSED(this);
    return 0;

/* $end */
}

corto_void _corto_secure_lock_destruct(
    corto_secure_lock this)
{
/* $begin(corto/secure/lock/destruct) */

    CORTO_UNUSED(this);

/* $end */
}
