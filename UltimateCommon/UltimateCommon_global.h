#pragma once

#if 0//defined(BUILD_STATIC)

#if defined(ULTIMATECOMMON_LIB)
#define ULTIMATECOMMON_EXPORT Q_DECL_EXPORT
#else
#define ULTIMATECOMMON_EXPORT Q_DECL_IMPORT
#endif
#else
#define ULTIMATECOMMON_EXPORT
#endif


#include <memory>


// 安全释放普通指针
#define SAFE_DELETE(x) if (nullptr != (x)) \
{ \
	delete (x);\
	(x) = nullptr;\
}

// 安全释放数据指针
#define SAFE_DELETE_ARRAY(x) if (nullptr != (x)) \
{ \
	delete[] (x);\
	(x) = nullptr;\
}


#define DECLARE_STD_PTR(x) \
class x; \
using x##Ptr = std::shared_ptr<x>;\
using x##WPtr = std::weak_ptr<x>;