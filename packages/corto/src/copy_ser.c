
#include "corto/corto.h"

static corto_int16 corto_ser_any(corto_serializer s, corto_value *info, void *userData) {
    corto_any *this = corto_value_getPtr(info);
    corto_copy_ser_t *data = userData, privateData;
    corto_any *value = (void*)((corto_word)corto_value_getPtr(&data->value) + ((corto_word)this - (corto_word)data->base));
    corto_value v;

    if (this->type->kind == CORTO_PRIMITIVE) {
        value->value = corto_calloc(corto_type_sizeof(this->type));
    }

    v = corto_value_value(this->type, this->value);
    privateData.value = corto_value_value(value->type, value->value);

    /* Set base of privateData. Because we're reusing the serializer, the
     * construct callback won't be called again */
    privateData.base = this->value;

    corto_serializeValue(s, &v, &privateData);

    value->type = this->type;
    value->owner = TRUE;

    return 0;
}

static corto_int16 corto_ser_primitive(corto_serializer s, corto_value *info, void *userData) {
    corto_type type = corto_value_getType(info);
    corto_copy_ser_t *data = userData;
    void *this = corto_value_getPtr(info);
    void *value = (void*)((corto_word)corto_value_getPtr(&data->value) + ((corto_word)this - (corto_word)data->base));

    CORTO_UNUSED(s);

    if (corto_primitive(type)->kind != CORTO_TEXT) {
        memcpy(value, this, type->size);
    } else {
        corto_setstr((corto_string*)value, *(corto_string*)this);
    }

    return 0;
}

static corto_int16 corto_ser_reference(corto_serializer s, corto_value *info, void *userData) {
    corto_copy_ser_t *data = userData;
    void *this = corto_value_getPtr(info);
    void *value = (void*)((corto_word)corto_value_getPtr(&data->value) + ((corto_word)this - (corto_word)data->base));
    CORTO_UNUSED(s);

    corto_setref(value, *(corto_object*)this);

    return 0;
}

static corto_int16 corto_ser_composite(corto_serializer s, corto_value *info, void *userData) {
    corto_copy_ser_t *data = userData;
    void *this = corto_value_getPtr(info);
    void *value = (void*)((corto_word)corto_value_getPtr(&data->value) + ((corto_word)this - (corto_word)data->base));
    corto_type t = corto_value_getType(info);

    if (corto_interface(t)->kind == CORTO_UNION) {
        *(corto_int32*)value = *(corto_int32*)this;
    }

    return corto_serializeMembers(s, info, userData);
}

/* Deinit element */
void corto_collection_deinitElement(corto_collection t, void *ptr) {
    corto_value v;
    if (corto_collection_requiresAlloc(t->elementType)) {
        v = corto_value_value(corto_type(t->elementType), ptr);
    } else {
        v = corto_value_value(corto_type(t->elementType), &ptr);
    }
    corto_deinitv(&v);
}

static corto_int16 corto_collection_copyListToArray(corto_collection t, void *array, corto_uint32 elementSize, corto_ll list, corto_bool reverse) {
    corto_equalityKind result = 0;
    corto_uint32 i=0;
    corto_iter iter;
    void *e1, *e2;
    corto_type elementType = t->elementType;

    iter = corto_llIter(list);
    while(corto_iterHasNext(&iter)) {
        if (corto_collection_requiresAlloc(t->elementType)) {
            e2 = corto_iterNext(&iter);
        } else {
            e2 = corto_iterNextPtr(&iter);
        }
        e1 = CORTO_OFFSET(array, elementSize * i);
        if (reverse) {
            void *tmp = e1;
            e1 = e2;
            e2 = tmp;
        }

        if (elementType->reference) {
            corto_setref(e1, *(corto_object*)e2);
        } else {
            result = corto_copyp(e1, elementType, e2);
        }

        i++;
    }

    return result;
}

/* TODO: should new elements be initialized? Should all elements be deinitialized first and then initialized? */
static corto_int16 corto_collection_copyListToList(corto_collection t, corto_ll list1, corto_ll list2) {
    corto_equalityKind result = 0;
    corto_iter iter1, iter2;
    void *e1, *e2;
    corto_type elementType = t->elementType;

    iter1 = corto_llIter(list1);
    iter2 = corto_llIter(list2);
    while(corto_iterHasNext(&iter1) && corto_iterHasNext(&iter2)) {
        if (corto_collection_requiresAlloc(t->elementType)) {
            e1 = corto_iterNext(&iter1);
            e2 = corto_iterNext(&iter2);
        } else {
            e1 = corto_iterNextPtr(&iter1);
            e2 = corto_iterNextPtr(&iter2);
        }

        if (elementType->reference) {
            corto_setref((corto_object*)e1, *(corto_object*)e2);
        } else {
            result = corto_copyp(e1, elementType, e2);
        }
    }

    return result;
}

/* Resize list */
static void corto_collection_resizeList(corto_collection t, corto_ll list, corto_uint32 size) {
    corto_uint32 ownSize = corto_llSize(list);
    corto_type elementType = t->elementType;

    /* If there are more elements in the destination, remove superfluous elements */
    if (ownSize > size) {
        corto_uint32 i;
        void *ptr;
        for(i=size; i<ownSize; i++) {
            ptr = corto_llTakeFirst(list);
            corto_collection_deinitElement(t, ptr);
        }
        /* If there are less elements in the destination, add new elements */
    } else if (ownSize < size) {
        corto_uint32 i;
        for(i=ownSize; i<size; i++) {
            void *ptr = NULL;
            if (corto_collection_requiresAlloc(t->elementType)) {
                ptr = corto_calloc(elementType->size);
            }
            corto_llInsert(list, ptr);
        }
    }
}


/* Resize list */
static void* corto_collection_resizeArray(corto_collection t, void* sequence, corto_uint32 size) {
    void *result = sequence;

    /* Only sequences can be resized */
    if (t->kind == CORTO_SEQUENCE) {
        corto_uint32 ownSize = ((corto_objectseq*)sequence)->length;
        corto_type elementType = t->elementType;
        result = ((corto_objectseq*)sequence)->buffer;

        /* If there are more elements in the destination, remove superfluous elements */
        if (ownSize > size) {
            corto_uint32 i;
            for(i = size; i < ownSize; i++) {
                corto_deinitp(CORTO_OFFSET(((corto_objectseq*)sequence)->buffer, elementType->size * i), elementType);
            }
            /* Reallocate buffer */
            result = ((corto_objectseq*)sequence)->buffer =
              corto_realloc(((corto_objectseq*)sequence)->buffer, elementType->size * size);

            /* If there are less elements in the destination, add new elements */
        } else if (ownSize < size) {
            /* Reallocate buffer */
            result = ((corto_objectseq*)sequence)->buffer =
              corto_realloc(((corto_objectseq*)sequence)->buffer, elementType->size * size);

            /* Memset new memory */
            memset(CORTO_OFFSET(((corto_objectseq*)sequence)->buffer, elementType->size * ownSize), 0, (size - ownSize) * elementType->size);
        }
        ((corto_objectseq*)sequence)->length = size;
    }

    return result;
}

/* Copy collections */
static corto_int16 corto_ser_collection(corto_serializer s, corto_value *info, void* userData) {
    corto_type t1, t2;
    void *v1, *v2;
    corto_uint32 size1 = 0;
    corto_copy_ser_t *data = userData;
    corto_uint32 result = 0;
    corto_bool v1IsArray = FALSE, v2IsArray = FALSE;

    CORTO_UNUSED(s);


    /* If this function is reached, collection-types are either equal or comparable. When the
     * base-object was a collection, the collection type can be different. When the base-object
     * was a composite type, the collection type has to be equal, since different composite
     * types are considered non-comparable. */
    t1 = corto_value_getType(info);
    v1 = corto_value_getPtr(info);

    /* Verify whether current serialized object is the base-object */
    if (info->parent) {
        t2 = t1;
        v2 = (void*)((corto_word)corto_value_getPtr(&data->value) + ((corto_word)v1 - (corto_word)data->base));
    } else {
        t2 = corto_value_getType(&data->value);
        v2 = corto_value_getPtr(&data->value);
    }

    {
        void *array1=NULL, *array2=NULL;
        corto_ll list1=NULL, list2=NULL;
        corto_uint32 elementSize=0;

        elementSize = corto_type_sizeof(corto_collection(t1)->elementType);

        switch(corto_collection(t1)->kind) {
            case CORTO_ARRAY:
                array1 = v1;
                elementSize = corto_type_sizeof(corto_collection(t1)->elementType);
                size1 = corto_collection(t1)->max;
                v1IsArray = TRUE;
                break;
            case CORTO_SEQUENCE:
                array1 = ((corto_objectseq*)v1)->buffer;
                elementSize = corto_type_sizeof(corto_collection(t1)->elementType);
                size1 = ((corto_objectseq*)v1)->length;
                v1IsArray = TRUE;
                break;
            case CORTO_LIST:
                list1 = *(corto_ll*)v1;
                size1 = corto_llSize(list1);
                break;
            case CORTO_MAP:
                break;
        }

        switch(corto_collection(t2)->kind) {
            case CORTO_ARRAY:
                array2 = v2;
                v2IsArray = TRUE;
                break;
            case CORTO_SEQUENCE:
                array2 = ((corto_objectseq*)v2)->buffer;
                v2IsArray = TRUE;
                break;
            case CORTO_LIST:
                list2 = *(corto_ll*)v2;
                break;
            case CORTO_MAP:
                break;
        }

        if (v1IsArray) {
            if (v2IsArray) {
                corto_copy_ser_t privateData;

                /* This is a bit tricky: passing the pointer to what is potentially a sequence-buffer
                 * while providing a sequence type. */
                array2 = corto_collection_resizeArray(corto_collection(t2), v2, size1);

                privateData.value = corto_value_value(corto_type(t2), array2);
                privateData.base = array1;
                result = corto_serializeElements(s, info, &privateData);
            } else if (list2) {
                corto_collection_resizeList(corto_collection(t1), list2, size1);
                result = corto_collection_copyListToArray(corto_collection(t1), array1, elementSize, list2, TRUE);
            }
        } else if (list1) {
            if (array2) {
                array2 = corto_collection_resizeArray(corto_collection(t2), v2, size1);
                result = corto_collection_copyListToArray(corto_collection(t1), array2, elementSize, list1, FALSE);
            } else if (list2) {
                corto_collection_resizeList(corto_collection(t1), list2, size1);
                result = corto_collection_copyListToList(corto_collection(t1), list2, list1);
            }
        }
    }

    return result;
}

static corto_int16 corto_ser_construct(corto_serializer s, corto_value *info, void *userData) {
    corto_copy_ser_t *data = userData;
    corto_int16 ret;
    CORTO_UNUSED(s);

    corto_type t1 = corto_value_getType(info);
    corto_type t2 = corto_value_getType(&data->value);

    data->base = corto_value_getPtr(info);

    ret = !corto_type_castable(t2, t1);
    if (ret) {
        corto_seterr("cannot copy value of type '%s' to '%s'",
            corto_fullpath(NULL, t1), corto_fullpath(NULL, t2));
    }

    return ret;
}

struct corto_serializer_s corto_copy_ser(corto_modifier access, corto_operatorKind accessKind, corto_serializerTraceKind trace) {
    struct corto_serializer_s s;

    corto_serializerInit(&s);

    s.access = access;
    s.accessKind = accessKind;
    s.traceKind = trace;
    s.aliasAction = CORTO_SERIALIZER_ALIAS_IGNORE;
    s.optionalAction = CORTO_SERIALIZER_OPTIONAL_IF_SET;
    s.construct = corto_ser_construct;
    s.program[CORTO_VOID] = NULL;
    s.program[CORTO_ANY] = corto_ser_any;
    s.program[CORTO_PRIMITIVE] = corto_ser_primitive;
    s.program[CORTO_COMPOSITE] = corto_ser_composite;
    s.program[CORTO_COLLECTION] = corto_ser_collection;
    s.reference = corto_ser_reference;
    return s;
}
