#pragma once
#include <string>

#include "LIWType.h"

namespace LIW{
	typedef long LIWID;

	class LIWObject {
	public:
		inline LIWID GetID() { return (LIWID)this; }
		inline virtual const LIWTypeID GetObjectTypeID() const { return GetTypeID<LIWObject>(); }
		inline virtual const std::string GetObjectTypeName() const { return GetTypeName<LIWObject>(); }
		inline virtual const LIWBaseType* GetObjectType() const { return GetType<LIWObject>(); }
	};
}