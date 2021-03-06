/*
 * corto__object.h
 *
 *  Created on: Aug 3, 2012
 *      Author: sander
 */

#ifndef CORTO__OBJECT_H_
#define CORTO__OBJECT_H_

#include "corto/corto.h"

#ifdef __cplusplus
extern "C" {
#endif

void corto_drop(corto_object o, corto_bool delete);
corto_object corto_resumePersistent(corto_object o);

typedef struct corto_contentType *corto_contentType;
struct corto_contentType {
    corto_string name;

    /* Translate values to and from a contentType value */
    corto_word ___ (*fromValue)(corto_value *v);
    corto_int16 ___ (*toValue)(corto_value *v, corto_word content);

    /* Translate objects to and from self-contained contentType values */
    corto_word ___ (*fromResult)(corto_result *o);
    corto_int16 ___ (*toResult)(corto_result* o, corto_word content);

    /* Translate objects to and from self-contained contentType values */
    corto_word ___ (*fromObject)(corto_object *o);
    corto_int16 ___ (*toObject)(corto_object* o, corto_word content);

    /* Duplicate a contentType value */
    corto_word ___ (*copy)(corto_word content);

    /* Free a contentType value */
    void (*release)(corto_word content);
};

corto_contentType corto_loadContentType(corto_string contentType);

typedef struct corto__attr {
    unsigned scope:1;
    unsigned write:1;
    unsigned observable:1;
    unsigned persistent:1;
    unsigned state:4;
}corto__attr;

struct corto__object;
typedef struct corto__object {
    /* Force max alignment to be used for struct. This ensures that the
     * member after this struct won't insert any padding bytes, regardless of
     * its alignment. */
    union {
        corto__attr attrs;
        corto_uint64 dummy;
    } align; /* Anonymous union would've been nice, but not supported in C99 */
    #ifndef NDEBUG
        /* Magic number to check in debugging whether value is an object. This value
         * should not be used in application logic. */
        corto_uint32 magic;
    #endif
    corto_int32 refcount;
    corto_type type;
}corto__object;

typedef struct corto__ols {
    corto_int8 key;
    void *value;
}corto__ols;

typedef struct corto__scope {
    corto_object parent;
    corto_string id;
    corto_rbtree scope;

    /* See corto__object */
    union {
        struct corto_rwmutex_s scopeLock;
        corto_int64 dummy;
    } align;
    corto__ols *ols;
}corto__scope;

typedef struct corto__writable {
    /* See corto__object */
    union {
        struct corto_rwmutex_s lock;
        corto_int64 dummy;
    } align;
}corto__writable;

typedef struct corto__observer corto__observer;
typedef void (*corto__notifyCallback)(corto__observer* data, corto_object _this, corto_object observable, corto_uint32 mask);

struct corto__observer {
    corto_object _this;
    corto_observer observer;
    char notifyKind;
    corto_int32 count;
};

typedef struct corto__observable corto__observable;
struct corto__observable {
    /* Protected by lock */
    corto_ll onSelf;
    corto_ll onChild;

    /* See corto__object */
    union {
        struct corto_rwmutex_s selfLock;
        corto_int64 dummy;
    } align;

    /* Lockless access to observers (zero-terminated)
     *  Element -1 contains a counter which indicates by how many
     *  notify-functions it is being used. When this counter reaches zero, the array
     *  can be deleted. */
    corto__observer **onSelfArray;
    corto__observer **onChildArray;
};

typedef struct corto__persistent corto__persistent;
struct corto__persistent {
    /* This struct is 32 bit on 32 bit architectures, and could result in extra
     * padding bytes in headers. However, as this attribute isn't used in
     * builtin objects, it isn't necessary to take into account struct
     * alignment. */
    corto_object owner;
};

typedef struct corto_mount_olsData_t {
    corto_mount mount;
    corto_eventMask mask;
} corto_mount_olsData_t;

typedef struct corto_augment_olsData_t {
    corto_mount mount;
    corto_string id;
} corto_augment_olsData_t;

/* Initialize static scoped object */
void corto__newSSO(corto_object sso);
corto_int16 corto__freeSSO(corto_object sso);

/* Adopt static scoped object */
int corto__adoptSSO(corto_object sso);

/* Orphan object */
void corto__orphan(corto_object o);

/* Set state on object */
void corto__setState(corto_object o, corto_uint8 state);

/* Provide extra debug information to release */
corto_int32 corto_release_ext(corto_object src, corto_object o, corto_string context);

corto_bool corto_destruct(corto_object o, corto_bool delete);

/* Get & lock scope */
corto__scope *corto__scopeClaim(corto_object o);
void corto__scopeRelease(corto_object o);
void* corto_olsFind(corto__scope *scope, corto_int8 key);

/* Get scope tree */
corto_rbtree corto_scopeof(corto_object o);

corto_int16 corto_notifySubscribers(corto_eventMask mask, corto_object o);
corto_int16 corto_notifySubscribersId(
    corto_eventMask mask,
    corto_string path,
    corto_string type,
    corto_string contentType,
    corto_word value);

corto__observable* corto__objectObservable(corto__object* o);
corto_int16 corto_notify(corto_object observable, corto_uint32 mask);
corto_int16 corto_notifySecured(corto_object observable, corto_uint32 mask);
void corto_notifyObservers(corto__observable* _o, corto_object observable, corto_object source, corto_uint32 mask, int depth);
void corto_notifyParentObservers(corto__observable* _o, corto_object observable, corto_object source, corto_uint32 mask, int depth);
void corto_observerDelayedAdminDefine(corto_object instance);

corto_object corto_resume(corto_object parent, corto_string expr, corto_object o);
corto_int16 corto_suspend(corto_object o);
int corto_loadIntern(corto_string str, int argc, char* argv[], corto_bool _try, corto_bool ignoreRecursive);

corto_uint32 corto_collection_size(corto_any _this);
typedef int (*corto_subscriptionWalkAction)(corto_subscriber s, corto_object instance, void *userData);
int corto_subscriptionWalk(corto_subscriptionWalkAction action, void *userData);


/* proxy for corto/expr functions */
typedef struct ext_corto_expr {
    corto_function function;
} ext_corto_expr;

typedef struct ext_corto_expr_opt {
    corto_object scope;
    corto_type returnType;
    corto_bool returnsReference;
    corto_bool inverse;
} ext_corto_expr_opt;

ext_corto_expr* ext_corto_expr_alloc(void);
corto_int16 ext_corto_expr_compb(ext_corto_expr *out, ext_corto_expr_opt *opt, char *expr, char **types);
corto_int16 ext_corto_expr_runb(ext_corto_expr *expr, corto_value *out, void **args);
corto_int16 ext_corto_expr_free(ext_corto_expr *expr);

corto_procedure corto_function_getProcedureType(corto_function this);

#ifdef __cplusplus
}
#endif

#endif /* CORTO__OBJECT_H_ */
