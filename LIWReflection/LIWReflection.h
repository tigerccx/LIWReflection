#pragma once
/*
* Module: LIWReflection
* Author: tigerccx
* Description:	In this module, a semi dynamic reflection mechanism is implemented,
*				allowing code to query
*/
#include <map>
#include <set>
#include <string>
#include "LIWObject.h"
#include "LIWType.h"


//
// Member Reflection
//

#define __LIWRegisterReflectAttr(type, name) \
const short __##name = const_cast<LIW::LIWBaseType*>(GetObjectType())->__Record(this, &this->name, LIW::GetType<type>(), #name);

/*
* Macro: LIWReflectAttr
* Description: This macro is used to declare reflectable attributes.
*/
#define LIWReflectAttr(type, name)	\
type name;							\
__LIWRegisterReflectAttr(type, name)

/*
* Macro: LIWReflectAttrVal
* Description: This macro is used to declare reflectable attributes and set default value.
*/
#define LIWReflectAttrVal(type, name, val)	\
type name = val;							\
__LIWRegisterReflectAttr(type, name)

/* Define some macros to make things easier */
#define LIWReflectInt(name) LIWReflectAttr(int, name)
#define LIWReflectBool(name) LIWReflectAttr(bool, name)
#define LIWReflectFloat(name) LIWReflectAttr(float, name)
#define LIWReflectDouble(name) LIWReflectAttr(double, name)
#define LIWReflectString(name) LIWReflectAttr(std::string, name)

#define LIWReflectIntVal(name, val) LIWReflectAttrVal(int, name, val)
#define LIWReflectBoolVal(name, val) LIWReflectAttrVal(bool, name, val)
#define LIWReflectFloatVal(name, val) LIWReflectAttrVal(float, name, val)
#define LIWReflectDoubleVal(name, val) LIWReflectAttrVal(double, name, val)
#define LIWReflectStringVal(name, val) LIWReflectAttrVal(std::string, name, val)


//
// Type Reflection
//

/*Set up class hierarchy*/
#define __LIW_REFLECT_SET_UP_BASE(className, classBase) \
const short __##className = LIW::LIWType<className>::type.__SetUpBaseType<classBase>(); \

/*Set up dynamic type info*/
/*Note: Don't know if there's a better way to do this.*/
#define __LIW_REFLECT_GET_TYPE_BLOCK(className) \
inline virtual const LIW::LIWTypeID GetObjectTypeID() const override { return LIW::GetTypeID<className>(); } \
inline virtual const std::string GetObjectTypeName() const override { return LIW::GetTypeName<className>(); } \
inline virtual const LIW::LIWBaseType* GetObjectType() const override { return LIW::GetType<className>(); }

/*
* Macro: LIW_REFLECT_CLASS_INHERIT_BEG, LIW_REFLECT_CLASS_INHERIT_END
* Description:	This pair of macros are used to define a class (className) capable of using LIW Reflection
*				derived from a base class (classBase).
*				eg.
*				LIW_REFLECT_CLASS_INHERIT_BEG(MyClassDerive, MyClassBase)
*				// Good-old-normal class definition ...
*				// Or not-so-good-old-normal LIWReflectAttr attribute definition ...
*				LIW_REFLECT_CLASS_INHERIT_END
*/
#define LIW_REFLECT_CLASS_INHERIT_BEG(className, classBase) \
static_assert(std::is_base_of_v<LIW::LIWObject, classBase>, "Base class has to be derived from LIWObject to support LIW Reflection."); \
class className: public classBase { \
public: \
	using typeBase = classBase; \
	using type = className; \
private: \
	__LIW_REFLECT_SET_UP_BASE(className, classBase) \
public: \
	__LIW_REFLECT_GET_TYPE_BLOCK(className) \
private: 
#define LIW_REFLECT_CLASS_INHERIT_END };

/*
* Macro: LIW_REFLECT_CLASS, LIW_REFLECT_CLASS_END
* Description:	This pair of macros are used to define a class capable of using LIW Reflection
*				derived from LIWObject.
*				eg.
*				LIW_REFLECT_CLASS(MyClassDerive, MyClassBase)
*				// Good-old-normal class definition ...
*				// Or not-so-good-old-normal LIWReflectAttr attribute definition ...
*				LIW_REFLECT_CLASS_END
*/
#define LIW_REFLECT_CLASS(className) LIW_REFLECT_CLASS_INHERIT_BEG(className, LIW::LIWObject)
#define LIW_REFLECT_CLASS_END LIW_REFLECT_CLASS_INHERIT_END


#define LIW_REFLECT_TYPE(className) className::classType
#define LIW_REFLECT_BASETYPE(className) className::baseType