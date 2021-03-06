/* $CORTO_GENERATED
 *
 * observer.c
 *
 * Only code written between the begin and end tags will be preserved
 * when the file is regenerated.
 */

#include <corto/core/core.h>

/* $header() */
#include "../lang/_class.h"
#include "_object.h"

extern corto_uint32 corto_subscribers_count;

/* Fluent request */
typedef struct corto_observeRequest {
    corto_int16 err;
    corto_object instance;
    corto_eventMask mask;
    corto_dispatcher observable;
    corto_object dispatcher;
    corto_string type;
    corto_bool enabled;
    void (*callback)(corto_object, corto_eventMask, corto_object, corto_observer);
} corto_observeRequest;

/* Thread local storage key for administration that keeps track for which observables notifications take place.
 * This key points to an element in a list keyed by threadId's for which notifications have taken place. Value
 * of this element is the observable being notified at the moment. When a listen\silence call needs to clean
 * memory it can look at this administration to see if it is in use. To prevent deadlocks, listen\silence calls
 * will not look at their own threadId, since this would indicate listen\silence is called from an observer being
 * notified. This key is created in corto_start. */
/* TODO: when a thread exits, the corresponding element must be free'd again - use tls destructor function */
extern corto_threadKey CORTO_KEY_OBSERVER_ADMIN;

/* Administration that keeps track of listen operations for undefined instances.
 * These operations are temporarily stored in this admin and are evaluated &
 * removed from the admin when the object is defined */
extern corto_threadKey CORTO_KEY_LISTEN_ADMIN;

/* Key used for passing object between fluent API calls */
extern corto_threadKey CORTO_KEY_FLUENT;

typedef struct corto_observerDelayedAdmin {
    corto_object instance;
    corto_observer observer;
    corto_object observable;
} corto_observerDelayedAdmin;

void corto_observerDelayedAdminAdd(
    corto_object instance,
    corto_observer observer,
    corto_object observable)
{
    corto_ll admin = corto_threadTlsGet(CORTO_KEY_LISTEN_ADMIN);
    if (!admin) {
        admin = corto_llNew();
        corto_threadTlsSet(CORTO_KEY_LISTEN_ADMIN, admin);
    }

    corto_observerDelayedAdmin *elem = corto_alloc(sizeof(corto_observerDelayedAdmin));
    elem->instance = instance;
    elem->observer = observer;
    elem->observable = observable;
    corto_llAppend(admin, elem);
}

void corto_observerDelayedAdminRemove(
    corto_object instance,
    corto_observer observer)
{
    corto_ll admin = corto_threadTlsGet(CORTO_KEY_LISTEN_ADMIN);
    if (admin) {
        corto_iter it = corto_llIter(admin);
        while (corto_iterHasNext(&it)) {
            corto_observerDelayedAdmin *elem = corto_iterNext(&it);
            if ((elem->instance == instance) &&
                (elem->observer == observer))
            {
                corto_dealloc(elem);
                corto_llRemove(admin, elem);
            }
        }
        if (!corto_llSize(admin)) {
            corto_llFree(admin);
            corto_threadTlsSet(CORTO_KEY_LISTEN_ADMIN, NULL);
        }
    }
}

void corto_observerDelayedAdminDefine(
    corto_object instance)
{
    corto_ll admin = corto_threadTlsGet(CORTO_KEY_LISTEN_ADMIN);
    if (admin) {
        corto_iter it = corto_llIter(admin);
        while (corto_iterHasNext(&it)) {
            corto_observerDelayedAdmin *elem = corto_iterNext(&it);
            if (elem->instance == instance) {
                if (corto_observer_observe(elem->observer, elem->instance, elem->observable)) {
                    /* This should never happen as the input parameters have
                     * been checked when observe was first called */
                    corto_critical(
                      "corto: delayed observe for previously undefined instance '%s' failed",
                      corto_fullpath(NULL, instance));
                }

                corto_dealloc(elem);
                corto_llRemove(admin, elem);
            }
        }
        if (!corto_llSize(admin)) {
            corto_llFree(admin);
            corto_threadTlsSet(CORTO_KEY_LISTEN_ADMIN, NULL);
        }
    }
}

typedef struct corto_observerElement {
    corto__observer ** observers;
    corto_bool free;
} corto_observerElement;

typedef struct corto_observerAdmin {
    corto_thread id;
    corto_observerElement stack[CORTO_MAX_NOTIFY_DEPTH];
    corto_uint32 sp;
    corto_object from;
} corto_observerAdmin;
static corto_observerAdmin observerAdmin[CORTO_MAX_THREADS];

/* Assumption: no more notifications are in progress when the thread
 * exits */
void corto_observerAdminFree(void *admin) {
    corto_uint32 i;
    corto_thread self = corto_threadSelf();

    /* No actual memory is allocated in admin. This function just clears up the
     * slot in the thread administration */
    CORTO_UNUSED(admin);

    for(i = 0; i < CORTO_MAX_THREADS; i++) {
        if (observerAdmin[i].id && (observerAdmin[i].id == self)) {
            /* Free up slot of this thread */
            observerAdmin[i].id = 0;
        }
    }
}

/* Free observer-array */
static void corto_observersFree(corto__observer** observers) {
    corto__observer* observer;
    while((observer = *observers)) {
        if (!corto_adec(&observer->count)) {
            corto_dealloc(observer);
        }
        ++observers;
    }
}

/* There is a small chance that a thread simultaneously with observe obtains a pointer
 * to an old observers-array, then gets scheduled out and this function in another thread
 * deletes that array. Although unlikely this scenario must be addressed. */
static void corto_observersArrayFree(corto__observer** array) {
    if (array) {
        corto_observersFree(array);
        corto_dealloc(&array[-1]);
    }
}

/* Copyout observers */
static void corto_observersCopyOut(corto_ll list, corto__observer** observers) {
    corto_iter iter;
    corto_uint32 i;

    iter = corto_llIter(list);
    i = 0;
    while(corto_iterHasNext(&iter)) {
        observers[i] = corto_iterNext(&iter);
        observers[i]->count++;
        i++;
    }
    observers[i] = NULL;
}

/* Event handling. */
static corto__observer* corto_observerFind(corto_ll on, corto_observer observer, corto_object instance) {
    corto_assertObject(observer);
    corto_assertObject(instance);

    corto__observer* result = NULL;
    corto_iter iter;

    if (on) {
        iter = corto_llIter(on);
        while(corto_iterHasNext(&iter)) {
            result = corto_iterNext(&iter);
            if ((result->observer == observer) && (result->_this == instance)) {
                break;
            } else {
                result = NULL;
            }
        }
    }

    return result;
}

/* Create observer-array */
static corto__observer** corto_observersArrayNew(corto_ll list) {
    corto__observer** array;

    array = corto_alloc((corto_llSize(list) + 2) * sizeof(corto__observer*));
    corto_observersCopyOut(list, &array[1]);

    /* Observers start from the second element */
    return &array[1];
}

static struct corto_observerAdmin* corto_observerAdminGet(void) {
    corto_observerAdmin *admin = corto_threadTlsGet(CORTO_KEY_OBSERVER_ADMIN);
    if (!admin) {
        corto_thread thr = corto_threadSelf();
        corto_uint32 i;
        do {
            i = 0;
            while(observerAdmin[i].id) { /* Find a free slot for thread in administration */
                i++;
                if (i >= CORTO_MAX_THREADS) {
                    corto_critical("maximum number of supported threads reached! (%d)", CORTO_MAX_THREADS);
                }
            }
        } while(!corto_cas(&observerAdmin[i].id,0,thr));
        admin = &observerAdmin[i];
        admin->sp = 0;
        corto_threadTlsSet(CORTO_KEY_OBSERVER_ADMIN, admin);
    }
    return admin;
}

/* Push observer-array to thread administration. Rather than passing the array pass the
 * address of the array so that setting the administration can be done atomic. */
static corto__observer** corto_observersPush(
    corto_observerAdmin *admin,
    corto__observer**  *observers)
{
    corto__observer** result;

    result = admin->stack[admin->sp].observers = *observers;
    if (result) {
        admin->stack[admin->sp++].free = FALSE;
    }

    return result;
}

static corto_bool corto_observersPop(corto_observerAdmin *admin) {
    return admin->stack[--admin->sp].free;
}

static corto_bool corto_observersWaitForUnused(corto__observer** observers) {
    corto_thread self = corto_threadSelf();
    corto_uint32 i, j;
    corto_bool inUse, freeArray = TRUE; /* Initialization merely to satisfy the compiler */

    if (!observers) {
        return FALSE;
    }

    /* Spinning loop which waits as long as an observer array is being used
     * in a notification by any of the running threads. */
    do {
        inUse = FALSE;
        for(i = 0; i < CORTO_MAX_THREADS; i++) {
            if (observerAdmin[i].id) {
                /* Check whether the observer array is in use by threads other than myself */
                if ((observerAdmin[i].id != self)) {
                    for(j = 0; j < observerAdmin[i].sp; j++) {
                        if (observerAdmin[i].stack[j].observers == observers) {
                            inUse = TRUE; /* Array is found, so keep waiting */
                        }
                    }

                /* Check whether the observer array is in use by my own thread */
                } else {
                    freeArray = TRUE;
                    for(j = 0; j < observerAdmin[i].sp; j++) {
                        /* The array is in use by my own thread so I can't keep spinning. Notify the observing function to
                         * free the array */
                        if (observerAdmin[i].stack[j].observers == observers) {
                            observerAdmin[i].stack[j].free = TRUE; /* Signal the notification routine to free the array */
                            freeArray = FALSE; /* Since the array is still in use by myself don't free array yet */
                            break;
                        }
                    }
                }
            }
        }
    } while(inUse);

    return freeArray;
}

static void corto_updateSubscriptionById(char *id) {
    corto_iter it;

    corto_select(NULL, id).subscribe(&it);
    while (corto_iterHasNext(&it)) {
        corto_result *r = corto_iterNext(&it);

        /* Reuse id buffer. Because this function is recursive, using a
         * large buffer allocated on stack is 'dangerous'. */
        sprintf(id, "%s/%s/", r->parent, r->id);
        corto_updateSubscriptionById(id);
    }
}

static void corto_updateSubscriptions(corto_eventMask observerMask, corto_eventMask mask, corto_object observable) {
    /* If there are no subscribers, then there are no mounts that are potentially
     * interested in subscriptions */
    if (corto_subscribers_count) {
        if (observerMask & CORTO_ON_TREE) {
            if (mask == CORTO_ON_DEFINE) {
                corto_id id;
                corto_fullpath(id, observable);
                strcat(id, "/");
                corto_updateSubscriptionById(id);
            } else if (mask == CORTO_ON_DELETE) {
                corto_id id;
                corto_fullpath(id, observable);
                corto_info("observer: unsubscribe for '%s'", id);
                corto_select(id, "//").unsubscribe();
            }
        }
    }
}

/* Notify one observer */
static void corto_notifyObserver(corto__observer *data, corto_object observable, corto_object source, corto_uint32 mask, int depth) {
    corto_observer observer = data->observer;
    corto_eventMask observerMask = observer->mask;

    if ((mask & observerMask) &&
        (!observer->typeReference || (observer->typeReference == corto_typeof(observable))) &&
        (!depth || (observerMask & CORTO_ON_TREE)))
    {
        corto_object this = data->_this;
        if (!this || (this != source)) {
            corto_call(observer, NULL, this, mask, observable, observer);
        }
    }
}

static void corto_notifyObserverCdecl(corto__observer *data, corto_object observable, corto_object source, corto_uint32 mask, int depth) {
    corto_observer observer = data->observer;
    corto_eventMask observerMask = observer->mask;

    if ((mask & observerMask) &&
        (!observer->typeReference || (observer->typeReference == corto_typeof(observable))) &&
        (!depth || (observerMask & CORTO_ON_TREE)))
    {
        corto_object this = data->_this;
        if (!this || (this != source)) {
            ((void(*)(
              corto_object,
              corto_eventMask,
              corto_object,
              corto_observer))((corto_function)observer)->fptr
            )(this, mask, observable, observer);
        }
    }
}

static void corto_notifyObserverDispatch(corto__observer *data, corto_object observable, corto_object source, corto_uint32 mask, int depth) {
    corto_observer observer = data->observer;
    corto_eventMask observerMask = observer->mask;

    if ((mask & observerMask) &&
        (!observer->typeReference || (observer->typeReference == corto_typeof(observable))) &&
        (!depth || (observerMask & CORTO_ON_TREE)))
    {
        corto_dispatcher dispatcher = observer->dispatcher;

        if (!data->_this || (data->_this != source)) {
            corto_attr oldAttr = corto_setAttr(0);
            corto_observableEvent event = corto_declare(corto_type(corto_observableEvent_o));
            corto_setAttr(oldAttr);

            corto_setref(&event->observer, observer);
            corto_setref(&event->me, data->_this);
            corto_setref(&event->observable, observable);
            corto_setref(&event->source, source);
            event->mask = mask;

            /* Set thread handle so the dispatcher can figure out whether a
             * readlock is needed */
            event->thread = corto_threadSelf();
            corto_dispatcher_post(dispatcher, corto_event(event));
        }
    }
}

static void corto_notifyObserversIntern(corto__observer** observers, corto_object observable, corto_uint32 mask, int depth) {
    corto__observer* data;

    corto_object prev = corto_setOwner(NULL);
    while((data = *observers)) {
#ifndef NDEBUG
        if (CORTO_TRACE_NOTIFICATIONS) {
            corto_string str = corto_strp(&mask, corto_eventMask_o, 0);
            corto_debug("notify:  %s %s: %s (%s)",
                corto_fullpath(NULL, data->_this),
                corto_fullpath(NULL, data->observer),
                corto_fullpath(NULL, observable),
                str);
            corto_dealloc(str);
        }
#endif
        /* Update subscriptions for tree observers */
        corto_updateSubscriptions(data->observer->mask, mask, observable);

        switch(data->notifyKind) {
        case 0: corto_notifyObserver(data, observable, prev, mask, depth); break;
        case 1: corto_notifyObserverCdecl(data, observable, prev, mask, depth); break;
        case 2: corto_notifyObserverDispatch(data, observable, prev, mask, depth); break;
        }
        
        observers++;
    }
    corto_setOwner(prev);
}

corto_int16 corto_notify(corto_object observable, corto_uint32 mask) {
    corto_assertObject(observable);
    corto_object *parent;

    corto_bool declared = corto_checkState(observable, CORTO_DECLARED);
    int depth = 0;
    corto__object *__o = CORTO_OFFSET(observable, -sizeof(corto__object));
    corto__observable *_o = NULL;
    if (__o->align.attrs.observable) {
        _o = CORTO_OFFSET(__o, -sizeof(corto__observable));
    }
    corto_observerAdmin *admin = corto_observerAdminGet();

    corto_object owner = NULL;
    if ((owner = corto_getOwner())) {
        if (corto_instanceof(corto_mount_o, owner)) {
            corto_mount m = owner;
            if (mask & CORTO_ON_DECLARE) {
                m->received.declares ++;
            }
            if (mask & (CORTO_ON_UPDATE | CORTO_ON_DEFINE)) {
                m->received.updates ++;
            }
            if (mask & CORTO_ON_DELETE) {
                m->received.deletes ++;
            }
        }
    }

    /* Notify direct observers */
    if (_o) {
        /* Notify observers of observable */
        corto__observer** observers = corto_observersPush(admin, &_o->onSelfArray);
        if (observers) {
            corto_notifyObserversIntern(observers, observable, mask, depth);
            if (corto_observersPop(admin)) {
                corto_observersArrayFree(observers);
            }
        }
    }

    /* Bubble event up in hierarchy */
    parent = observable;
    while((parent = corto_parentof(parent))) {
        corto__observable *_parent =
          corto__objectObservable(CORTO_OFFSET(parent, -sizeof(corto__object)));

        if (_parent) {
            if (declared) {
                corto__observer** observers = corto_observersPush(admin, &_parent->onChildArray);
                if (observers) {
                    corto_notifyObserversIntern(observers, observable, mask, depth);
                    if (corto_observersPop(admin)) {
                        corto_observersArrayFree(observers);
                    }
                }
            } else {
                corto__observer** observers = corto_observersPush(admin, &_parent->onSelfArray);
                if (observers) {
                    corto_notifyObserversIntern(observers, parent, mask, depth);
                    if (corto_observersPop(admin)) {
                        corto_observersArrayFree(observers);
                    }
                }
            }
        }
        if (declared) {
            depth++;
        }
        declared = corto_checkState(parent, CORTO_DECLARED);
    }

    if (corto_notifySubscribers(mask, observable)) {
        goto error;
    }

    return 0;
error:
    return -1;
}

corto_int16 corto_notifySecured(corto_object observable, corto_uint32 mask) {
    if (!corto_authorized(observable, CORTO_SECURE_ACTION_READ)) {
        return 0;
    }
    return corto_notify(observable, mask);
}

/* If an observer is subscribed to NEW events align it with existing objects so that
 * an observer doesn't need to do an object-walk over the observable manually to discover
 * all objects.
 */
typedef struct corto_observerAlignData {
    corto__observer *observer;
    corto_object observable;
    int mask;
    int depth;
} corto_observerAlignData;

int corto_observerAlignScope(corto_object o, void *userData) {
    corto_assertObject(o);

    corto_observerAlignData *data = userData;

    if (corto_checkAttr(o, CORTO_ATTR_PERSISTENT)) {
        if ((data->mask & CORTO_ON_DECLARE) && (data->mask & (CORTO_ON_SCOPE|CORTO_ON_TREE)))
        {
            corto_notifyObserver(data->observer, o, o, CORTO_ON_DECLARE, data->depth);
        }
    }

    if (corto_checkAttr(o, CORTO_ATTR_PERSISTENT)) {
        if ((data->mask & CORTO_ON_DEFINE) && (data->mask & (CORTO_ON_SCOPE|CORTO_ON_TREE)) &&
            corto_checkState(o, CORTO_DEFINED))
        {
            corto_notifyObserver(data->observer, o, o, CORTO_ON_DEFINE, data->depth);
        }
    }

    if (data->mask & CORTO_ON_TREE) {
        int result = 1;
        corto_objectseq scope;
        corto_uint32 i;
        data->depth++;
        scope = corto_scopeClaim(o);
        for(i = 0; i < scope.length; i++) {
            result = corto_observerAlignScope(scope.buffer[i], userData);
        }
        corto_scopeRelease(scope);
        data->depth--;
        return result;
    } else {
        return 1;
    }
}

void corto_observerAlign(corto_object observable, corto__observer *observer, int mask) {
    corto_assertObject(observable);

    corto_observerAlignData walkData;
    corto_objectseq scope;

    /* Do recursive walk over scope */
    walkData.observable = observable;
    walkData.observer = observer;
    walkData.mask = mask;
    walkData.depth = 0;

    if (corto_checkAttr(observable, CORTO_ATTR_PERSISTENT)) {
        if (((mask & CORTO_ON_DECLARE) && (mask & CORTO_ON_SELF) && corto_checkState(observable, CORTO_DECLARED)) ||
            ((mask & CORTO_ON_DEFINE) && (mask & CORTO_ON_SELF) && corto_checkState(observable, CORTO_DEFINED))) {
            corto_notifyObserver(observer, observable, observable, mask, 0);
        }
    }

    scope = corto_scopeClaim(observable);
    corto_int32 i;
    for (i = 0; i < scope.length; i++) {
        corto_observerAlignScope(scope.buffer[i], &walkData);
    }
    corto_scopeRelease(scope);
}


static corto_observeFluent corto_observeFluentGet(void);

static corto_observer corto_observeObserve(corto_observeRequest *r)
{
    corto_observer result = corto_declare(corto_observer_o);

    result->mask = r->mask;
    corto_setref(&result->observable, r->observable);
    corto_setref(&result->instance, r->instance);
    corto_setref(&result->dispatcher, r->dispatcher);
    corto_setstr(&result->type, r->type);
    result->enabled = r->enabled;
    ((corto_function)result)->fptr = (corto_word)r->callback;
    ((corto_function)result)->kind = CORTO_PROCEDURE_CDECL;

    if (corto_define(result)) {
        corto_delete(result);
        result = NULL;
    }

    return result;
}

static corto_observeFluent corto_observeInstance(
    corto_object instance)
{
    corto_observeRequest *request = corto_threadTlsGet(CORTO_KEY_FLUENT);
    if (request) {
        request->instance = instance;
    }
    return corto_observeFluentGet();
}

static corto_observeFluent corto_observeType(
    corto_string type)
{
    corto_observeRequest *request = corto_threadTlsGet(CORTO_KEY_FLUENT);
    if (request) {
        request->type = type;
    }
    return corto_observeFluentGet();
}

static corto_observeFluent corto_observeDisabled(void)
{
    corto_observeRequest *request = corto_threadTlsGet(CORTO_KEY_FLUENT);
    if (request) {
        request->enabled = FALSE;
    }
    return corto_observeFluentGet();
}

static corto_observeFluent corto_observeDispatcher(
    corto_dispatcher dispatcher)
{
    corto_observeRequest *request = corto_threadTlsGet(CORTO_KEY_FLUENT);
    if (request) {
        request->dispatcher = dispatcher;
    }
    return corto_observeFluentGet();
}

static corto_observer corto_observeCallback(
    void (*callback)(corto_object, corto_eventMask, corto_object, corto_observer))
{
    corto_observer result = NULL;

    corto_observeRequest *request = corto_threadTlsGet(CORTO_KEY_FLUENT);
    if (request) {
        request->callback = callback;
        corto_threadTlsSet(CORTO_KEY_FLUENT, NULL);
        result = corto_observeObserve(request);
        corto_dealloc(request);
    }

    return result;
}

static corto_observeFluent corto_observeFluentGet(void)
{
    corto_observeFluent result;
    result.callback = corto_observeCallback;
    result.instance = corto_observeInstance;
    result.type = corto_observeType;
    result.dispatcher = corto_observeDispatcher;
    result.disabled = corto_observeDisabled;
    return result;
}

struct corto_observeFluent corto_observe(corto_eventMask mask, corto_object observable) {
    corto_observeRequest *request = corto_threadTlsGet(CORTO_KEY_FLUENT);
    if (!request) {
        request = corto_calloc(sizeof(corto_observeRequest));
        corto_threadTlsSet(CORTO_KEY_FLUENT, request);
    } else {
        memset(request, 0, sizeof(corto_observeRequest));
    }

    request->mask = mask;
    request->observable = observable;
    request->enabled = TRUE;
    return corto_observeFluentGet();
}

corto_int16 corto_unobserve(corto_observer observer) {
    return corto_delete(observer);
}

/* $end */

corto_int16 _corto_observer_construct(
    corto_observer this)
{
/* $begin(corto/core/observer/construct) */
    corto_type t = corto_object_o;
    if (this->type) {
        t = corto_resolve(NULL, this->type);
        if (!t) {
            corto_seterr("unresolved type '%s'", this->type);
            goto error;
        }
        if (!corto_instanceof(corto_type_o, t)) {
            corto_seterr("'%s' is not a type", this->type);
            goto error;
        }
        corto_setref(&this->typeReference, t);
    }

    if (!corto_function(this)->parameters.length) {
        if (!corto_checkAttr(this, CORTO_ATTR_SCOPED) || !strchr(corto_idof(this), '(')) {
            corto_function(this)->parameters.buffer = corto_calloc(sizeof(corto_parameter) * 3);
            corto_function(this)->parameters.length = 3;
            corto_parameter *p;

            /* Parameter event */
            p = &corto_function(this)->parameters.buffer[0];
            p->name = corto_strdup("event");
            p->passByReference = FALSE;
            corto_setref(&p->type, corto_eventMask_o);

            /* Parameter object */
            p = &corto_function(this)->parameters.buffer[1];
            p->name = corto_strdup("object");
            p->passByReference = TRUE;
            corto_setref(&p->type, t);

            /* Parameter subscriber */
            p = &corto_function(this)->parameters.buffer[2];
            p->name = corto_strdup("observer");
            p->passByReference = TRUE;
            corto_setref(&p->type, corto_observer_o);
        }
    }

    /* Bind function. Run this function before listening to observable, as
     * the call handler must be initialized before the observer can be invoked.
     */
    if (corto_function_construct(this)) {
        goto error;
    }

    if (this->enabled && !corto_instanceof(corto_subscriber_o, this)) {
        if (corto_observer_observe(this, this->instance, this->observable)) {
            goto error;
        }
    }

    return 0;
error:
    return -1;
/* $end */
}

corto_void _corto_observer_destruct(
    corto_observer this)
{
/* $begin(corto/core/observer/destruct) */
    corto_function_destruct(this);

    if (this->enabled && this->observable) {
        if (corto_observer_unobserve(this, this->instance, this->observable)) {
            corto_error("unobserve failed: %s", corto_lasterr());
        }
    }

    if (this->active) {
        corto_error("observer '%s' deleted with %d active subscription(s)",
          corto_fullpath(NULL, this), this->active);
    }

    this->enabled = FALSE;
    
/* $end */
}

corto_int16 _corto_observer_init(
    corto_observer this)
{
/* $begin(corto/core/observer/init) */
    corto_setref(&corto_function(this)->returnType, corto_void_o);

    /* Set parameters of observer: (this, observable) */
    if (corto_checkAttr(this, CORTO_ATTR_SCOPED) && strchr(corto_idof(this), '(')) {
        /* id of function contains a parameter list, parse in function/init */
        corto_int16 result = corto_function_init(this);
        if (result) {
            goto error;
        }

        if (corto_function(this)->parameters.length != 3) {
            corto_seterr("observers must have three arguments");
            goto error;
        }

        if (corto_function(this)->parameters.buffer[0].type != corto_type(corto_eventMask_o)) {
            corto_seterr("first argument must be of type core/eventMask");
            goto error;
        }

        if (!corto_function(this)->parameters.buffer[1].passByReference &&
            !corto_function(this)->parameters.buffer[1].type->reference)
        {
            corto_seterr("observer parameter must be of a reference type");
            goto error;
        }

        if (corto_function(this)->parameters.buffer[2].type != corto_type(corto_observer_o)) {
            corto_seterr("third argument must be of type core/observer");
            goto error;
        }
    }

    return 0;
error:
    return -1;
/* $end */
}

corto_int16 _corto_observer_observe(
    corto_observer this,
    corto_object instance,
    corto_object observable)
{
/* $begin(corto/core/observer/observe) */
    corto__observable* _o;
    corto__observer* _observerData = NULL;
    corto_bool added = FALSE;
    corto__observer **oldSelfArray = NULL, **oldChildArray = NULL;

    /* Check if mask specifies either SELF or CHILDS, if not enable SELF */
    if (!(this->mask & (CORTO_ON_SELF|CORTO_ON_SCOPE|CORTO_ON_TREE))) {
        this->mask |= CORTO_ON_SELF;
    }

    /* Check if mask specifies either VALUE or METAVALUE, if not enable VALUE */
    if (!((this->mask & CORTO_ON_VALUE) || (this->mask & CORTO_ON_METAVALUE))) {
        this->mask |= CORTO_ON_VALUE;
    }

    corto_eventMask mask = this->mask;

    /* If no observable is provided, use observable specified on observer */
    if (!observable) {
        observable = this->observable;
    } else {
        /* If observer doesn't specify an observable, a custom observable may be
         * provided. This is especially useful when an observer belonging to a class
         * is subscribing for instances of the class. When the observer does
         * set the observable and it differs from the observable parameter,
         * throw an error. */
        if (this->observable && (this->observable != observable)) {
            corto_seterr(
              "parameter observable ('%s') differs from observer %s ('%s')",
              corto_fullpath(NULL, observable),
              corto_fullpath(NULL, this),
              corto_fullpath(NULL, this->observable));
            goto error;
        }
    }

    if (!observable) {
        corto_seterr("no observable provided for observer");
        goto error;
    }

    /* Test for error conditions before making changes */
    if (mask & (CORTO_ON_SCOPE|CORTO_ON_TREE)) {
        if (!corto_checkAttr(observable, CORTO_ATTR_SCOPED)) {
            corto_seterr(
                "invalid nested subscription, observable is not scoped");
            goto error;
        }
    }

    if (!corto_checkAttr(observable, CORTO_ATTR_OBSERVABLE)) {
        corto_seterr(
            "'%s' is not an observable",
            corto_fullpath(NULL, observable));
        goto error;
    }

    /* If instance has not yet been defined, postpone listen */
    if (instance && !corto_checkState(instance, CORTO_DEFINED)) {
        corto_observerDelayedAdminAdd(
            instance,
            this,
            observable
        );
  #ifndef NDEBUG
        if (CORTO_TRACE_NOTIFICATIONS) {
            corto_debug("postpone observe (instance not defined): %s %s %s (%s%s%s%s%s%s%s)",
                corto_fullpath(NULL, instance),
                corto_fullpath(NULL, this),
                corto_fullpath(NULL, observable),
                mask & CORTO_ON_SELF ? " self" : "",
                mask & CORTO_ON_SCOPE ? " scope" : "",
                mask & CORTO_ON_TREE ? " tree" : "",
                mask & CORTO_ON_DECLARE ? " declare" : "",
                mask & CORTO_ON_DEFINE ? " define" : "",
                mask & CORTO_ON_UPDATE ? " update" : "",
                mask & CORTO_ON_DELETE ? " delete" : "");
          }
#endif
        goto postponed;
    }

    _o = corto__objectObservable(
        CORTO_OFFSET(observable, -sizeof(corto__object)));

    #ifndef NDEBUG
        if (CORTO_TRACE_NOTIFICATIONS) {
            corto_debug("observe: %s %s %s (%s%s%s%s%s%s%s)",
                corto_fullpath(NULL, instance),
                corto_fullpath(NULL, this),
                corto_fullpath(NULL, observable),
                mask & CORTO_ON_SELF ? " self" : "",
                mask & CORTO_ON_SCOPE ? " scope" : "",
                mask & CORTO_ON_TREE ? " tree" : "",
                mask & CORTO_ON_DECLARE ? " declare" : "",
                mask & CORTO_ON_DEFINE ? " define" : "",
                mask & CORTO_ON_UPDATE ? " update" : "",
                mask & CORTO_ON_DELETE ? " delete" : "");
        }
    #endif

    /* Create observerData */
    _observerData = corto_alloc(sizeof(corto__observer));
    _observerData->observer = this;
    _observerData->_this = instance;
    _observerData->count = 0;
    if (this->dispatcher) {
        _observerData->notifyKind = 2;
    } else if (corto_function(this)->kind == CORTO_PROCEDURE_CDECL) {
        _observerData->notifyKind = 1;
    } else {
        _observerData->notifyKind = 0;
    }

    /* If observer must trigger on updates of me, add it to onSelf list */
    if (mask & CORTO_ON_SELF) {
        if (corto_rwmutexWrite(&_o->align.selfLock)) {
            goto error;
        }
        if (!corto_observerFind(_o->onSelf, this, instance)) {
            if (!_o->onSelf) {
                _o->onSelf = corto_llNew();
            }
            corto_llAppend(_o->onSelf, _observerData);
            added = TRUE;

            /* Build new observer array. This array can be accessed without
             * locking and is faster than walking the linked list. */
            oldSelfArray = _o->onSelfArray;
            _o->onSelfArray = corto_observersArrayNew(_o->onSelf);
        }
        if (corto_rwmutexUnlock(&_o->align.selfLock)) {
            goto error;
        }
    }

    /* If observer must trigger on updates of childs, add it to onChilds list */
    if (mask & (CORTO_ON_SCOPE|CORTO_ON_TREE)) {
        if (corto_rwmutexWrite(&_o->align.selfLock)) {
            goto error;
        }
        if (!corto_observerFind(_o->onChild, this, instance)) {
            if (!_o->onChild) {
                _o->onChild = corto_llNew();
            }

            corto_llAppend(_o->onChild, _observerData);
            added = TRUE;

            /* Build new observer array. This array can be accessed without
             * locking and is faster than walking a linked list. */
            oldChildArray = _o->onChildArray;
            _o->onChildArray = corto_observersArrayNew(_o->onChild);
        }
        if (corto_rwmutexUnlock(&_o->align.selfLock)) {
            goto error;
        }
    }

    if (!added) {
        corto_dealloc(_observerData);
    } else {
        /* If observer is subscribed to declare/define events, align observer
         * with existing */
        if ((mask & CORTO_ON_DECLARE) || (mask & CORTO_ON_DEFINE)) {
            corto_observerAlign(observable, _observerData, mask);
        }

        this->active ++;
        this->enabled = TRUE;

        /* Let mounts know of observer */
        corto_iter it;
        corto_id observableId;
        corto_fullpath(observableId, observable);
        corto_int16 ret = corto_select(
            observableId,
            mask & CORTO_ON_SELF ? "." :
            mask & CORTO_ON_SCOPE ? "/" : "//")
          .instance(this)
          .subscribe(&it);
        if (ret) {
            corto_seterr("observer: failed to notify mounts of subscription");
            goto error;
        }

        /* TODO: use data from select to align observer */
        while (corto_iterHasNext(&it)) corto_iterNext(&it);
    }

    /* From this point onwards the old observer arrays are no longer accessible.
     * However, since notifications can still be in progress these arrays can't
     * be deleted yet. Therefore wait until the arrays are no longer being
     * used.
     *
     * The administration where this information is stored is not protected by
     * locking so that notifying objects can remain lock-free. There is however
     * a slight chance that a notification pushed the old array to the
     * administration but that this change is not yet visible. In this case the
     * functions below will assume the array is unused, which is incorrect. The
     * chance of this happening is extremely slim as this scenario illustrates:
     *
     * notify: push observable array
     * notify: call observers...
     * listen: add observer (build new observable array)
     * listen: waitForUnused <- due to out of order execution this thread
     *                          can in theory not have seen the push yet.
     *
     * To be absolutely sure that the observed administration is up to date a
     * memory barrier is required here. A simple mutex will not do since this
     * would encumber the notifications too much.
     */

    if (corto_observersWaitForUnused(oldSelfArray)) {
        corto_observersArrayFree(oldSelfArray);
    }
    if (corto_observersWaitForUnused(oldChildArray)) {
        corto_observersArrayFree(oldChildArray);
    }

    /* Ensure that observer isn't deleted before instance unsubscribes */
    corto_claim(this);

postponed:
    return 0;
error:
    return -1;
/* $end */
}

corto_bool _corto_observer_observing(
    corto_observer this,
    corto_object instance,
    corto_object observable)
{
/* $begin(corto/core/observer/observing) */
    corto__observer* observerData;
    corto__observable* _o;
    corto_bool result = FALSE;

    _o = corto__objectObservable(CORTO_OFFSET(observable, -sizeof(corto__object)));
    observerData = NULL;

    if (_o) {
        if (this->mask & CORTO_ON_SELF) {
            corto_rwmutexWrite(&_o->align.selfLock);
            observerData = corto_observerFind(_o->onSelf, this, instance);
            if (observerData) {
                result = TRUE;
            }
            corto_rwmutexUnlock(&_o->align.selfLock);
        }

        if (!result) {
            if (corto_checkAttr(observable, CORTO_ATTR_OBSERVABLE)) {
                if ((this->mask & (CORTO_ON_SCOPE|CORTO_ON_TREE))) {
                    if (corto_checkAttr(observable, CORTO_ATTR_SCOPED)) {
                        corto_rwmutexWrite(&_o->align.selfLock);
                        observerData = corto_observerFind(_o->onChild, this, instance);
                        if (observerData) {
                            result = TRUE;
                        }
                        corto_rwmutexUnlock(&_o->align.selfLock);
                    }
                }
            } else {
                corto_seterr("object '%s' is not an observable",
                    corto_fullpath(NULL, observable));
                goto error;
            }
        }
    }

    return result;
error:
    return FALSE;
/* $end */
}

corto_int16 _corto_observer_unobserve(
    corto_observer this,
    corto_object instance,
    corto_object observable)
{
/* $begin(corto/core/observer/unobserve) */
    corto__observer* observerData;
    corto__observable* _o;
    corto__observer **oldSelfArray = NULL, **oldChildArray = NULL;
    corto_eventMask mask = this->mask;
    corto_bool removed = FALSE;

    if (!observable) {
        corto_seterr("unobserve: observable not provided");
        goto error;
    }

    /* If instance has not yet been defined, undo delayed listen */
    if (instance && !corto_checkState(instance, CORTO_DEFINED)) {
        corto_observerDelayedAdminRemove(
            instance,
            this
        );
        goto ignore;
    }

    /* Observers don't necessarily take a refcount on their observables, and if
     * the observable has already been destructed, it has already been silenced. */
    if (corto_checkState(observable, CORTO_DESTRUCTED)) {
        goto ignore;
    }

    if (corto_checkAttr(observable, CORTO_ATTR_OBSERVABLE)) {
        _o = corto__objectObservable(CORTO_OFFSET(observable, -sizeof(corto__object)));
        observerData = NULL;

        /* If observer triggered on updates of me, remove from onSelf list */
        if (mask & CORTO_ON_SELF) {
            if (corto_rwmutexWrite(&_o->align.selfLock)) {
                goto error;
            }
            observerData = corto_observerFind(_o->onSelf, this, instance);
            if (observerData) {
                corto_llRemove(_o->onSelf, observerData);
                observerData->count--;
                removed = TRUE;

                /* Build new observer array */
                oldSelfArray = _o->onSelfArray;
                _o->onSelfArray = corto_observersArrayNew(_o->onSelf);

    #ifndef NDEBUG
                if (CORTO_TRACE_NOTIFICATIONS) {
                    corto_debug("unobserve: %s %s %s",
                        corto_fullpath(NULL, instance),
                        corto_fullpath(NULL, this),
                        corto_fullpath(NULL, observable));
                }
    #endif
            }
            if (corto_rwmutexUnlock(&_o->align.selfLock)) {
                goto error;
            }
        }

        /* If observer triggered on updates of childs, remove from onChilds list */
        if (mask & (CORTO_ON_SCOPE|CORTO_ON_TREE)) {
            if (corto_checkAttr(observable, CORTO_ATTR_SCOPED)) {
                if (corto_rwmutexWrite(&_o->align.selfLock)) {
                    goto error;
                }
                observerData = corto_observerFind(_o->onChild, this, instance);
                if (observerData) {
                    corto_llRemove(_o->onChild, observerData);
                    observerData->count--;
                    removed = TRUE;

                    /* Build new observer array */
                    oldChildArray = _o->onChildArray;
                    _o->onChildArray = corto_observersArrayNew(_o->onChild);
                }
                if (corto_rwmutexUnlock(&_o->align.selfLock)) {
                    goto error;
                }
            } else {
                corto_seterr(
                    "observer subscribed on childs of non-scoped object");
                goto error;
            }
        }
    } else {
        corto_seterr("object '%s' is not an observable",
            corto_fullpath(NULL, observable));
        goto error;
    }

    if (removed) {
        this->active --;
        if (!this->active) {
            this->enabled = FALSE;
        }
        
        corto_id observableId;
        corto_fullpath(observableId, observable);
        corto_select(observableId,
            mask & CORTO_ON_SELF ? "." :
            mask & CORTO_ON_SCOPE ? "/" : "//")
           .instance(this).unsubscribe();
    }

    /* See comments in observe */
    /*__atomic_thread_fence (__ATOMIC_SEQ_CST);*/

    if (corto_observersWaitForUnused(oldSelfArray)) {
        corto_observersArrayFree(oldSelfArray);
    }
    if (corto_observersWaitForUnused(oldChildArray)) {
        corto_observersArrayFree(oldChildArray);
    }

    /*if (observerData) {
        TODO: corto_dealloc(observerData);
    }*/

    /* Release observer after unsubscribe */
    corto_release(this);

ignore:
    return 0;
error:
    return -1;
/* $end */
}
