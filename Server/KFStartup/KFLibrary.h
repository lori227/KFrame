#ifndef __KF_LIBRARY_H__
#define __KF_LIBRARY_H__

#include "KFrame.h"

namespace KFrame
{
	__ST_CLASS__( KFLibrary )
	{
	public:
		KFLibrary();
		~KFLibrary();

		// 加载插件
		bool Load( const std::string& path, const std::string& name );

		// 卸载插件
		void UnLoad();

		// 获得函数地址
		void* GetFunction( const char* function );
		
	protected:
		// 插件句柄
		void* _instance;

	public:
		// 插件路径
		std::string _path;
	};
}

#endif
