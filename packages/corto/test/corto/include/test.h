/* test.h
 *
 * This file contains generated code. Do not modify!
 */

#ifndef TEST_H
#define TEST_H

#include <corto/corto.h>
#include <corto/corto.h>
#include <include/_project.h>
#include <corto/test/test.h>
#include <corto/core/c/c.h>
#include <corto/lang/c/c.h>

/* $header() */
/* Enter additional code here. */
/* $end */

#include <include/_type.h>
#include <include/_load.h>
#include <include/_api.h>

/* $body() */
/* Enter code that requires types here */
/* $end */

#ifdef __cplusplus
extern "C" {
#endif

#define test_functionToResolve(a, b) _test_functionToResolve(a, b)
TEST_EXPORT
corto_void _test_functionToResolve(
    corto_int32 a,
    corto_int32 b);

#define test_ol_any__test_Color(a) _test_ol_any__test_Color(a)
TEST_EXPORT
corto_void _test_ol_any__test_Color(
    test_Color a);

#define test_ol_any_bool(a) _test_ol_any_bool(a)
TEST_EXPORT
corto_void _test_ol_any_bool(
    corto_bool a);

#define test_ol_any_char(a) _test_ol_any_char(a)
TEST_EXPORT
corto_void _test_ol_any_char(
    corto_char a);

#define test_ol_any_float32(a) _test_ol_any_float32(a)
TEST_EXPORT
corto_void _test_ol_any_float32(
    corto_float32 a);

#define test_ol_any_int32(a) _test_ol_any_int32(a)
TEST_EXPORT
corto_void _test_ol_any_int32(
    corto_int32 a);

#define test_ol_any_object(a) _test_ol_any_object(a)
TEST_EXPORT
corto_void _test_ol_any_object(
    corto_object a);

#define test_ol_any_string(a) _test_ol_any_string(a)
TEST_EXPORT
corto_void _test_ol_any_string(
    corto_string a);

#define test_ol_bool(a) _test_ol_bool(a)
TEST_EXPORT
corto_void _test_ol_bool(
    corto_bool a);

#define test_ol_inherit__test_Animal(a) _test_ol_inherit__test_Animal(test_Animal(a))
TEST_EXPORT
corto_void _test_ol_inherit__test_Animal(
    test_Animal a);

#define test_ol_inherit__test_Dog(a) _test_ol_inherit__test_Dog(test_Dog(a))
TEST_EXPORT
corto_void _test_ol_inherit__test_Dog(
    test_Dog a);

#define test_ol_int(a) _test_ol_int(a)
TEST_EXPORT
corto_void _test_ol_int(
    corto_int32 a);

#define test_ol_null_bool(a) _test_ol_null_bool(a)
TEST_EXPORT
corto_void _test_ol_null_bool(
    corto_bool a);

#define test_ol_null_object(a) _test_ol_null_object(a)
TEST_EXPORT
corto_void _test_ol_null_object(
    corto_object a);

#define test_ol_null_string(a) _test_ol_null_string(a)
TEST_EXPORT
corto_void _test_ol_null_string(
    corto_string a);

#define test_ol_num_float64(a) _test_ol_num_float64(a)
TEST_EXPORT
corto_void _test_ol_num_float64(
    corto_float64 a);

#define test_ol_num_int32(a) _test_ol_num_int32(a)
TEST_EXPORT
corto_void _test_ol_num_int32(
    corto_int32 a);

#define test_ol_object(a) _test_ol_object(a)
TEST_EXPORT
corto_void _test_ol_object(
    corto_object a);

#define test_ol_string(a) _test_ol_string(a)
TEST_EXPORT
corto_void _test_ol_string(
    corto_string a);

#define test_ol_uint(a) _test_ol_uint(a)
TEST_EXPORT
corto_void _test_ol_uint(
    corto_int32 a);

#define test_ol_wildcard_float32_string(a, b) _test_ol_wildcard_float32_string(a, b)
TEST_EXPORT
corto_void _test_ol_wildcard_float32_string(
    corto_float32 a,
    corto_string b);

#define test_ol_wildcard_int32_string(a, b) _test_ol_wildcard_int32_string(a, b)
TEST_EXPORT
corto_void _test_ol_wildcard_int32_string(
    corto_int32 a,
    corto_string b);

#include <include/AccessRule.h>
#include <include/Animal.h>
#include <include/AnonymousTest.h>
#include <include/AttrCheck.h>
#include <include/AttrConstructorTest.h>
#include <include/Attributes.h>
#include <include/Bar.h>
#include <include/Buffer.h>
#include <include/Car.h>
#include <include/Car_Engine.h>
#include <include/Car_Wheel.h>
#include <include/Car_Wheel_Tire.h>
#include <include/Cat.h>
#include <include/Compare.h>
#include <include/CompositeWithString.h>
#include <include/Container.h>
#include <include/ContainerEmpty.h>
#include <include/ContainerNestedContainer.h>
#include <include/ContainerNestedContainer_ChildContainer.h>
#include <include/ContainerNestedContainer_ChildContainer_ChildLeaf.h>
#include <include/ContainerNestedLeaf.h>
#include <include/ContainerNestedLeaf_ChildLeaf.h>
#include <include/ContainerNestedTable.h>
#include <include/ContainerNestedTable_ChildTable.h>
#include <include/Copy.h>
#include <include/DeclaredParent.h>
#include <include/DefinedParent.h>
#include <include/Dog.h>
#include <include/Env.h>
#include <include/Err.h>
#include <include/Event.h>
#include <include/EventCount.h>
#include <include/EventTest.h>
#include <include/Foo.h>
#include <include/FooReplicator.h>
#include <include/Fullname.h>
#include <include/GoldenRetriever.h>
#include <include/LeafRoot.h>
#include <include/Line.h>
#include <include/Loader.h>
#include <include/Lookup.h>
#include <include/Matcher.h>
#include <include/MethodForwardTest.h>
#include <include/MethodResolver.h>
#include <include/MethodTester.h>
#include <include/MethodTesterInherit.h>
#include <include/NestedConstructFailInRoot.h>
#include <include/NestedConstructFailInTier1.h>
#include <include/NestedConstructFailInTier2.h>
#include <include/NestedInitFailInRoot.h>
#include <include/NestedInitFailInTier1.h>
#include <include/NestedInitFailInTier2.h>
#include <include/ObjectMgmt.h>
#include <include/ObserverDispatcher.h>
#include <include/Observers.h>
#include <include/Overload.h>
#include <include/Ownership.h>
#include <include/Point.h>
#include <include/Point3D.h>
#include <include/ReferenceMember.h>
#include <include/ReferenceTargetMember.h>
#include <include/RelativeName.h>
#include <include/Resolver.h>
#include <include/Security.h>
#include <include/Select.h>
#include <include/StringDeserializer.h>
#include <include/StringSerializer.h>
#include <include/Subscribe.h>
#include <include/SubscriberEvent.h>
#include <include/SubscriberTest.h>
#include <include/TableMultiKey.h>
#include <include/TableMultiMixedKey.h>
#include <include/TableMultiStringKey.h>
#include <include/TableNested.h>
#include <include/TableNested_ChildTable.h>
#include <include/TableNestedContainer.h>
#include <include/TableNestedContainer_ChildContainer.h>
#include <include/TableNestedContainer_ChildContainer_ChildLeaf.h>
#include <include/TableNestedLeafs.h>
#include <include/TableNestedLeafs_ChildLeaf1.h>
#include <include/TableNestedLeafs_ChildLeaf2.h>
#include <include/TableSingleKey.h>
#include <include/TableStringKey.h>
#include <include/TargetActual.h>
#include <include/TargetActualMember.h>
#include <include/TestKey.h>
#include <include/TestLock.h>
#include <include/TypeAttr.h>
#include <include/ValueCast.h>
#include <include/ValueExpr.h>
#include <include/VoidParent.h>

#ifdef __cplusplus
}
#endif

#endif

