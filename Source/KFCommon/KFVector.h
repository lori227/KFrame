#ifndef __KF_VECTOR_H__
#define __KF_VECTOR_H__

#include "KFInclude.h"
#include "KFMemory/KFMalloc.h"

namespace KFrame
{
	template< class T >
	class KFVector
	{
	public:
		KFVector();
		virtual ~KFVector();

		// 清空
		void Clear();

		// 重置数量
		void Resize( uint32 maxcount );

		// 空位置
		uint32 FindEmpty();

		// 最大数量
		uint32 MaxSize();

		// 是否为空
		bool IsValid( uint32 index );

		// 添加 
		void Insert( T* object );
		void Insert( uint32 index, T* object );

		// 查找
		T* Find( uint32 index );

		// 删除
		void Remove( uint32 index );

		T* First();
		T* Next();
		bool End();

	public:
		// 列表
		std::vector< T* > _objects;

	protected:
		typename std::vector< T* >::iterator _iter;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////
	template< class T >
	KFVector< T >::KFVector()
	{
		_objects.clear();
	}

	template< class T >
	KFVector< T >::~KFVector()
	{
		for ( auto object : _objects )
		{
			__KF_DESTROY__( T, object );
		}

		_objects.clear();
	}

	template< class T >
	void KFVector< T >::Clear()
	{
		for ( auto object : _objects )
		{
			__KF_DESTROY__( T, object );
		}

		_objects.clear();
	}

	template< class T >
	void KFVector< T >::Resize( uint32 maxcount )
	{
		auto count = _objects.size();
		if ( maxcount <= count )
		{
			return;
		}

		_objects.resize( maxcount, nullptr );
	}

	template< class T >
	uint32 KFVector< T >::FindEmpty()
	{
		uint32 index = 0;
		for ( auto* object : _objects )
		{
			if ( object == nullptr )
			{
				return index;
			}

			++index;
		}

		return MaxSize();
	}

	template< class T >
	uint32 KFVector< T >::MaxSize()
	{
		return static_cast<uint32>(_objects.size());
	}

	template< class T >
	bool KFVector< T >::IsValid( uint32 index )
	{
		return index < MaxSize();
	}

	template< class T >
	void KFVector< T >::Insert( T* object )
	{
		uint32 index = FindEmpty();
		Insert( index, object );
	}

	template< class T >
	void KFVector< T >::Insert( uint32 index, T* object )
	{
		if ( IsValid( index ) )
		{
			_objects[ index ] = object;
		}
		else
		{
			_objects.push_back( object );
		}
	}

	template< class T >
	T* KFVector< T >::Find( uint32 index )
	{
		if ( !IsValid( index ) )
		{
			return nullptr;
		}

		return _objects[ index ];
	}

	template< class T >
	void KFVector< T >::Remove( uint32 index )
	{
		if ( !IsValid( index ) )
		{
			return;
		}

		auto* object = _objects[ index ];
		__KF_DESTROY__( T, object );
		_objects[ index ] = nullptr;
	}

	template< class T >
	T* KFVector< T >::First()
	{
		_iter = _objects.begin();
		if ( _iter == _objects.end() )
		{
			return nullptr;
		}

		return *_iter;
	}

	template< class T >
	T* KFVector< T >::Next()
	{
		++_iter;
		if ( _iter == _objects.end() )
		{
			return nullptr;
		}

		return *_iter;
	}

	template< class T >
	bool KFVector< T >::End()
	{
		return _iter == _objects.end();
	}
}

#endif