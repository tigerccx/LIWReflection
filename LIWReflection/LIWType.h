#pragma once
#include <typeinfo>
#include <string>
#include <iostream>
#include <set>
#include <unordered_set>

namespace LIW {
	class LIWObject;
	class LIWBaseType;

	/*
	* Class: LIWReflectMember
	* Author: tigerccx
	* Description:	This class is used to represent a reflectable attribute,
	*				which record the name and offset from class instance pointer.
	*/
	class LIWReflectMember {
	public:
		LIWReflectMember() = default;
		LIWReflectMember(const std::string& name, const LIWBaseType* type, int offset) :
			m_name(name), __m_hashname(std::hash<std::string>()(m_name)), m_type(type), m_offset(offset) {}
		LIWReflectMember(const LIWReflectMember& other) = default;
		LIWReflectMember(LIWReflectMember&& other) = default;
		LIWReflectMember& operator=(const LIWReflectMember& other) = default;
		LIWReflectMember& operator=(LIWReflectMember&& other) = default;

		bool operator<(const LIWReflectMember& rec) const { return __m_hashname < rec.__m_hashname; }
		bool operator>(const LIWReflectMember& rec) const { return __m_hashname > rec.__m_hashname; }
		bool operator==(const LIWReflectMember& rec) const { return __m_hashname == rec.__m_hashname; }

		std::string m_name;
		size_t __m_hashname;
		const LIWBaseType* m_type;
		int m_offset;

	public:
		static LIWReflectMember __m_find_helper;
	};
}

template <>
class std::hash<LIW::LIWReflectMember>
{
public:
	size_t operator()(const LIW::LIWReflectMember& rec) const
	{
		return rec.__m_hashname;
	}
};


namespace LIW {
	//typedef std::set<LIWReflectMember> LIWReflectAttrBook;
	typedef std::unordered_set<LIWReflectMember> LIWReflectAttrBook;

	typedef size_t LIWTypeID;
	
	class LIWBaseType {
	public:
		inline virtual const LIWTypeID& GetID() const = 0;
		inline virtual const std::string& GetName() const = 0;
		inline const LIWBaseType* GetBase() const { return baseType; }
		void __SetBase(LIWBaseType* base) { baseType = base; m_book.insert(base->m_book.begin(), base->m_book.end()); }

		bool operator==(const LIWBaseType& rec) const { return GetID() == rec.GetID(); }
		bool operator!=(const LIWBaseType& rec) const { return GetID() != rec.GetID(); }

	protected:
		LIWBaseType* baseType = nullptr;
		LIWReflectAttrBook m_book;

	public:
		template<class Base>
		const short __SetUpBaseType();

		const short __Record(LIWObject* object, void* data, const LIWBaseType* type, const std::string& name) {
			m_book.insert(std::move(LIWReflectMember(name, type, (char*)(data)-(char*)(object))));
			return 0;
		}

	public:
		/*
		* Func: Set
		* Description:	Set reflectable attribute.
		*/
		template<class T> bool Set(void* object, const std::string& memberName, const T& value) const {
			const LIWReflectMember* member;
			if (!this->GetReflectMember<T>(memberName, member)) {
				return false;
			}
			*(T*)((char*)(object)+member->m_offset) = value;
			return true;
		}

		/*
		* Func: Get
		* Description:	Get reflectable attribute value.
		*/
		template<class T> bool Get(const void* object, const std::string& memberName, volatile T& value) const {
			const LIWReflectMember* member;
			if (!this->GetReflectMember<T>(memberName, member)) {
				return false;
			}
			value = *(T*)((char*)(object)+member->m_offset);
			return true;
		}

		/*
		* Func: GetPt
		* Description:	Get pointer to reflectable attribute value.
		*/
		template<class T> T* GetPt(const void* object, const std::string& memberName) const {
			const LIWReflectMember* member;
			if (!this->GetReflectMember<T>(memberName, member)) {
				return nullptr;
			}
			return (T*)((char*)(object)+member->m_offset);
		}

		/*
		* Func: GetVoidPt
		* Description:	Get void pointer to reflectable attribute value.
		*/
		void* GetPtVoid(const void* object, const std::string& memberName) const {
			const LIWReflectMember* member;
			if (!this->GetReflectMemberTypeless(memberName, member)) {
				return nullptr;
			}
			return (void*)((char*)(object)+member->m_offset);
		}

		/*
		* Func: GetAttrs
		* Description:	Get table to different types of reflectable attributes.
		*/
		const LIWReflectAttrBook& GetAttrs() const {
			return m_book;
		}

	protected:
		/*
		* Func: GetReflectMember
		* Description:	Get a reflectable member.
		*/
		template<class T> bool GetReflectMember(const std::string& memberName, const LIWReflectMember*& member) const;

		bool GetReflectMemberTypeless(const std::string& memberName, const LIWReflectMember*& member) const {
			LIWReflectMember::__m_find_helper.__m_hashname = std::hash<std::string>()(memberName);
			auto itr1 = m_book.find(LIWReflectMember::__m_find_helper);
			if (itr1 == m_book.end()) return false;
			member = &(*itr1);
			return true;
		}
	};

	template<class T>
	class LIWType :
		public LIWBaseType
	{
	public:
		static LIWType type;
		inline const LIWTypeID& GetID() const override { return m_id; }
		inline const std::string& GetName() const override { return m_name; }
		template<class T> void __SetBase() { LIWBaseType::__SetBase(&LIWType<T>::type); }
		
	protected:
		LIWType() {
			m_name = typeid(std::decay_t<T>).name();
			m_id = typeid(std::decay_t<T>).hash_code();
			//std::cout << m_name << std::endl;
		}
		std::string m_name;
		LIWTypeID m_id;
	};
	template<class T> LIWType<T> LIWType<T>::type;
	
	/*
	* Func: GetBase
	* Description: Get base type (LIWBaseType) of type T
	*/
	template<class T> inline const LIWBaseType* GetBase() {
		return LIWType<std::decay_t<T>>::type.GetBase();
	}

	/*
	* Func: GetType
	* Description: Get type (LIWBaseType) of type T
	*/
	template<class T> inline const LIWBaseType* GetType() {
		return &LIWType<std::decay_t<T>>::type;
	}

	/*
	* Func: GetTypeID
	* Description: Get type id of type T
	*/
	template<class T> inline const LIWTypeID& GetTypeID() {
		return LIWType<std::decay_t<T>>::type.GetID();
	}

	/*
	* Func: GetTypeName
	* Description: Get type name of type T
	*/
	template<class T> inline const std::string& GetTypeName() {
		return LIWType<std::decay_t<T>>::type.GetName();
	}

	template<class Base>
	inline const short LIWBaseType::__SetUpBaseType()
	{
		__SetBase(&LIWType<Base>::type);
		return 0;
	}

	template<class T> bool LIWBaseType::GetReflectMember(const std::string& memberName, const LIWReflectMember*& member) const
	{
		const LIWBaseType* type = GetType<T>();
		LIWReflectMember::__m_find_helper.__m_hashname = std::hash<std::string>()(memberName);
		auto itr1 = m_book.find(LIWReflectMember::__m_find_helper);
		if (itr1 == m_book.end() || itr1->m_type != type) return false;
		member = &(*itr1);
		return true;
	}
}