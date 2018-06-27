#ifndef __KF_MAP_H__
#define __KF_MAP_H__

#include "KFInclude.h"
#include "KFMemory/KFMalloc.h"

namespace KFrame
{
	template< class KeyType, class ParamType, class ObjectType >
	class KFMap
	{
	public:
		KFMap();
		virtual ~KFMap();

		// 大小
		uint32 Size();

		// 添加
		void Insert( ParamType, ObjectType* object );

		// 查找
		ObjectType* Find( ParamType key ) const;
		ObjectType* Create( ParamType key );
		
		// 存在
		bool IsExist( ParamType key );

		// 删除
		bool Remove( ParamType key, bool isdelete = true );
		
		// 清除
		void Clear( bool isdelete = true );

		// 是否为空
		bool IsEmpty();

		// 遍历
		// 不能嵌套遍历, 中间不能有删除添加操作
		ObjectType* First();
		ObjectType* Next();

		// 添加数据
		void AddMap( KFMap& kfother );

	public:
		// 迭代器
		typename std::map< KeyType, ObjectType* >::iterator _iter;

		// 列表
		std::map< KeyType, ObjectType* > _objects;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////
	template< class KeyType, class ParamType, class ObjectType >
	KFMap< KeyType, ParamType, ObjectType >::KFMap()
	{
		_objects.clear();
	}

	template< class KeyType, class ParamType, class ObjectType >
	KFMap< KeyType, ParamType, ObjectType >::~KFMap()
	{
		Clear();
	}

	template< class KeyType, class ParamType, class ObjectType >
	uint32 KFMap< KeyType, ParamType, ObjectType >::Size()
	{
		return static_cast<uint32>(_objects.size());
	}

	template< class KeyType, class ParamType, class ObjectType >
	void KFMap< KeyType, ParamType, ObjectType >::Clear( bool isdelete /* = true */ )
	{
		if ( isdelete )
		{
			for ( auto iter = _objects.begin(); iter != _objects.end(); ++iter )
			{
				__KF_DESTROY__( ObjectType, iter->second );
			}
		}

		_objects.clear();
	}
	
	template< class KeyType, class ParamType, class ObjectType >
	bool KFMap< KeyType, ParamType, ObjectType >::IsEmpty()
	{
		return _objects.empty();
	}

	template< class KeyType, class ParamType, class ObjectType >
	bool KFMap< KeyType, ParamType, ObjectType >::IsExist( ParamType key )
	{
		return Find( key ) != nullptr;
	}

	template< class KeyType, class ParamType, class ObjectType >
	void KFMap< KeyType, ParamType, ObjectType >::Insert( ParamType key, ObjectType* object )
	{
		Remove( key, true );
		_objects[ key ] = object;
	}

	template< class KeyType, class ParamType, class ObjectType >
	ObjectType* KFMap< KeyType, ParamType, ObjectType >::Find( ParamType key ) const
	{
		auto iter = _objects.find( key );
		if ( iter == _objects.end() )
		{
			return nullptr;
		}

		return iter->second;
	}

	template< class KeyType, class ParamType, class ObjectType >
	ObjectType* KFMap< KeyType, ParamType, ObjectType >::Create( ParamType key )
	{
		auto object = Find( key );
		if ( object != nullptr )
		{
			return object;
		}

		object = __KF_CREATE__( ObjectType );
		Insert( key, object );
		return object;
	}


	template< class KeyType, class ParamType, class ObjectType >
	bool KFMap< KeyType, ParamType, ObjectType >::Remove( ParamType key, bool isdelete )
	{
		auto iter = _objects.find( key );
		if ( iter == _objects.end() )
		{
			return false;
		}

		if ( isdelete )
		{
			__KF_DESTROY__( ObjectType, iter->second );
		}

		_objects.erase( iter );
		return true;
	}

	template< class KeyType, class ParamType, class ObjectType >
	ObjectType* KFMap< KeyType, ParamType, ObjectType >::First()
	{
		if ( _objects.empty() )
		{
			return nullptr;
		}

		_iter = _objects.begin();
		return _iter->second;
}

	template< class KeyType, class ParamType, class ObjectType >
	ObjectType* KFMap< KeyType, ParamType, ObjectType >::Next()
	{
		++_iter;
		if ( _iter == _objects.end() )
		{
			return nullptr;
		}

		return _iter->second;
	}

	template< class KeyType, class ParamType, class ObjectType >
	void KFMap< KeyType, ParamType, ObjectType >::AddMap( KFMap& kfother )
	{
		for ( auto iter : kfother._objects )
		{
			Insert( iter.first, iter.second );
		}

		kfother._objects.clear();
	}
}
#endif
