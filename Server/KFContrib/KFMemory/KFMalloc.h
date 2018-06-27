#ifndef __KF_MALLOC_H__
#define __KF_MALLOC_H__

#include "KFMemory.h"

namespace KFrame
{
#define __KF_NEW__( name, ... )\
	 KFMalloc::Instance()->New< name >( __FUNCTION_LINE__, ##__VA_ARGS__ )
#define __KF_DELETE__( object )\
	 KFMalloc::Instance()->Delete( object, __FUNCTION_LINE__ )

#define __KF_INT8__( length )\
	KFMalloc::Instance()->GetInt8( length, __FUNCTION_LINE__ )
#define __KF_UINT8__( length )\
	KFMalloc::Instance()->GetUInt8( length, __FUNCTION_LINE__ )
	
	// 多线程内存池
#define __KF_MALLOC_BATCH__(name, size, batch)\
	 KFMalloc::Instance()->Malloc< name >( size, batch, __FUNCTION_LINE__ );
#define __KF_MALLOC__(name, size)\
	 KFMalloc::Instance()->Malloc< name >( size, 30, __FUNCTION_LINE__ );
#define __KF_FREE__(name, memory, size)\
	 KFMalloc::Instance()->Free< name >( memory, size, __FUNCTION_LINE__ );
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////

	class KFThreadBuffer;
	class KFMutex;
	class KFMalloc
	{
	public:
		~KFMalloc();

		// 创建
		static void Initialize( KFMalloc* kfmalloc );

		// 接口
		static KFMalloc* Instance();

		// 设置打开内存日志
		void SetLogMemoryOpen( bool open );
		///////////////////////////////////////////////////////////////////////////////////
		template< class T, class ... P >
		inline T* New( const char* function, uint32 line, P&&...params )
		{
			auto object = new T( std::forward<P>( params )... );
			KFLogger::LogMemory( KFLogger::Info, "[%s:%u] new object[0x%x:%s]!",
				function, line, object, typeid( T ).name() );
			return object;
		}

		template< class T >
		inline void Delete( T*& object, const char* function, uint32 line )
		{
			if ( object == nullptr )
			{
				return;
			}

			KFLogger::LogMemory( KFLogger::Info, "[%s:%u] delete object[0x%x:%s]!",
				function, line, object, typeid( T ).name() );

			delete object;
			object = nullptr;
		}

		///////////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////
		template< class T, class ... P>
		inline T* MTCreate( const char* function, uint32 line, uint32 batch, P&&...params )
		{
			void* memory = _mt_memory->Create( typeid( T ).name(), sizeof( T ), batch, function, line );
			return new( memory ) T( std::forward<P>( params )... );
		}

		template< class T, class ... P>
		inline T* STCreate( const char* function, uint32 line, uint32 batch, P&&...params )
		{
			void* memory = _st_memory->Create( typeid( T ).name(), sizeof( T ), batch, function, line );
			return new( memory ) T( std::forward<P>( params )... );
		}

		template< class T >
		inline void MTDestroy( T* object, const char* function, uint32 line )
		{
			if ( object == nullptr )
			{
				return;
			}

			object->~T();
			_mt_memory->Delete( object, function, line );
		}

		template< class T >
		inline void STDestroy( T* object, const char* function, uint32 line )
		{
			if ( object == nullptr )
			{
				return;
			}

			object->~T();
			_st_memory->Delete( object, function, line );
		}
		///////////////////////////////////////////////////////////////////////////////////
		template< class T >
		inline T* Malloc( uint32 size, uint32 batch, const char* function, uint32 line )
		{
			return reinterpret_cast< T* >( MallocMemory( size, batch, function, line ) );
		}

		template< class T >
		inline void Free( void* memory, uint32 size, const char* function, uint32 line )
		{
			FreeMemory( memory, size, function, line );
		}
		///////////////////////////////////////////////////////////////////////////////////
		// 获得buff
		int8* GetInt8( uint32 length, const char* function, uint32 line );
		uint8* GetUInt8( uint32 length, const char* function, uint32 line );
		///////////////////////////////////////////////////////////////////////////////////
		// 打印内存信息
		void PrintLogMemory();

	protected:
		KFMalloc();
		// 接口
		static KFMalloc* _kf_malloc;

		// 分配内存
		void* MallocMemory( uint32 size, uint32 batch, const char* function, uint32 line );

		// 释放内存
		void FreeMemory( void* memory, uint32 size, const char* function, uint32 line );

		// 计算最小的2的整数倍
		uint32 CalcNearest2NSize( uint32 size );

		// 打印内存
		void PrintLogMemory( const char* type, uint32 count, uint64 usesize, uint64 totalsize );

	private:
		// 单线程内存
		KFMemory< KFNullMutex >* _st_memory;
		
		// 多线程内存
		KFMemory< KFMutex >* _mt_memory;
		
		KFMutex* _kf_mutex;
		std::set< void* > _mt_new_memory;

		// 线程内存
		KFThreadBuffer * _kf_buffer;
	};
}

#endif