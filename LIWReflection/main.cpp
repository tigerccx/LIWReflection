#include <iostream>
#include <string>
#include <cassert>
#include <type_traits>
#include "ticktock.h"


#include "LIWType.h"
#include "LIWReflection.h"


LIW_REFLECT_CLASS(MyTest)
public:
	LIWReflectIntVal(test0, 20)
	LIWReflectFloat(test1)
LIW_REFLECT_CLASS_END

LIW_REFLECT_CLASS_INHERIT_BEG(MyTest1, MyTest)
public:
	LIWReflectDoubleVal(test2, 20.5)
	LIWReflectInt(test3)
LIW_REFLECT_CLASS_INHERIT_END

LIW_REFLECT_CLASS_INHERIT_BEG(MyTest2, MyTest1)
public:
	LIWReflectIntVal(test4, 305)
	LIWReflectBoolVal(test5, false)
LIW_REFLECT_CLASS_INHERIT_END

bool grslt;
#define MY_ASSERT(expr) grslt=(expr); assert(grslt);

void CorrectnessTest() {
	std::cout << "------------------------------" << std::endl << "Correctness Test" << std::endl << std::endl;
	MyTest t0;
	MyTest1 t1;
	MyTest2 t2;
	MyTest* pt0 = &t1;

	auto type0 = pt0->GetObjectType();
	auto id0 = pt0->GetObjectTypeID();
	auto name0 = pt0->GetObjectTypeName();

	auto type00 = LIW::GetType<MyTest1>();
	auto id00 = LIW::GetTypeID<MyTest1>();
	auto name00 = LIW::GetTypeName<MyTest1>();

	assert(type0 == type00);
	assert(id0 == id00);
	assert(name0 == name00);

	auto type10 = LIW::GetType<MyTest>();
	auto id10 = LIW::GetTypeID<MyTest>();
	auto name10 = LIW::GetTypeName<MyTest>();

	assert(type10 != type00);
	assert(id0 != id10);
	assert(name0 != name10);

	auto type11 = LIW::GetBase<MyTest1>();
	auto type12 = type00->GetBase();
	assert(type11 == type10 && type11 == type12);


	MY_ASSERT(pt0->GetObjectType()->Set<int>(pt0, "test0", 25));
	MY_ASSERT(pt0->GetObjectType()->Set<int>(pt0, "test0", 34));
	MY_ASSERT(pt0->GetObjectType()->Set<float>(pt0, "test1", 10.4f));
	MY_ASSERT(pt0->GetObjectType()->Set<double>(pt0, "test2", 5.8));
	MY_ASSERT(pt0->GetObjectType()->Set<int>(pt0, "test3", 91));
	MY_ASSERT(!pt0->GetObjectType()->Set<float>(pt0, "test3", 88));
	MY_ASSERT(!pt0->GetObjectType()->Set<float>(pt0, "test4", 99));


	int intval = 10;
	float floatval = 20.2;
	double doubleval = 30.3;
	MY_ASSERT(pt0->GetObjectType()->Get<int>(pt0, "test0", intval));
	std::cout << "test0 " << intval << std::endl;
	MY_ASSERT(!pt0->GetObjectType()->Get<float>(pt0, "test0", floatval));
	MY_ASSERT(!pt0->GetObjectType()->Get<int>(pt0, "test123", intval));
	MY_ASSERT(pt0->GetObjectType()->Get<float>(pt0, "test1", floatval));
	std::cout << "test1 " << floatval << std::endl;
	MY_ASSERT(pt0->GetObjectType()->Get<double>(pt0, "test2", doubleval));
	std::cout << "test2 " << doubleval << std::endl;
	MY_ASSERT(pt0->GetObjectType()->Get<int>(pt0, "test3", intval));
	std::cout << "test3 " << intval << std::endl;

	std::cout << std::endl;

	const LIW::LIWReflectAttrBook& book = pt0->GetObjectType()->GetAttrs();
	for (auto& itr : book) {
		std::cout << itr.m_type << " " << itr.m_name << std::endl;
	}
	std::cout << std::endl;
}

void EfficiencyTest() {
	std::cout << "------------------------------" << std::endl << "Efficiency Test 1000 data fetches" << std::endl << std::endl;
	MyTest t0;
	MyTest1 t1;
	MyTest2 t2;
	MyTest* pt0 = &t1;
	MyTest1* pt1 = &t1;

	TICK(Reflection)
	for (int i = 0; i < 1000; ++i) {
		int intval;
		pt1->GetObjectType()->Get<int>(pt1, "test0", intval);
		double doubleval;
		pt1->GetObjectType()->Get<double>(pt1, "test2", doubleval);
	}
	TOCK(Reflection)
	
	pt1 = &t2;
	TICK(Reflection_Hierarchy)
	for (int i = 0; i < 1000; ++i) {
		int intval;
		pt1->GetObjectType()->Get<int>(pt1, "test0", intval);
		double doubleval;
		pt1->GetObjectType()->Get<double>(pt1, "test2", doubleval);
	}
	TOCK(Reflection_Hierarchy)

	TICK(Direct)
	for (int i = 0; i < 1000; ++i) {
		int intval = pt1->test0;
		double doubleval = pt1->test2;
	}
	TOCK(Direct)
}

void EfficiencyTest1() {
	std::cout << "------------------------------" << std::endl << "Efficiency Test 1000 data fetches" << std::endl << std::endl;
	MyTest t0;
	MyTest1 t1;
	MyTest2 t2;
	MyTest* pt0 = &t1;
	MyTest1* pt1 = &t1;

	while (true) {
		int intval;
		pt1->GetObjectType()->Get<int>(pt1, "test0", intval);
	}
}

int main() {
	CorrectnessTest();
	EfficiencyTest();
	EfficiencyTest1();
}
