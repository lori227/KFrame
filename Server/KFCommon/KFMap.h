#ifndef __KF_MAP_H__
#define __KF_MAP_H__

#include "KFInclude.h"
#include "KFThread/KFMutex.h"
#include "KFMemory/KFMemoryObject.h"

namespace KFrame
{
	template< class KeyType, class ParamType, class ObjectType, class Mutex = KFNullMutex >
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
		ObjectType* Find( ParamType key );
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

	private:
		Mutex _kf_mutex;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////
	template< class KeyType, class ParamType, class ObjectType, class Mutex >
	KFMap< KeyType, ParamType, ObjectType, Mutex >::KFMap()
	{
		_objects.clear();
	}

	template< class KeyType, class ParamType, class ObjectType, class Mutex >
	KFMap< KeyType, ParamType, ObjectType, Mutex >::~KFMap()
	{
		Clear();
	}

	template< class KeyType, class ParamType, class ObjectType, class Mutex >
	uint32 KFMap< KeyType, ParamType, ObjectType, Mutex >::Size()
	{
		KFLocker< Mutex > locker( _kf_mutex );
		return static_cast< uint32 >( _objects.size() );
	}

	template< class KeyType, class ParamType, class ObjectType, class Mutex >
	void KFMap< KeyType, ParamType, ObjectType, Mutex >::Clear( bool isdelete /* = true */ )
	{
		KFLocker< Mutex > locker( _kf_mutex );

		if ( isdelete )
		{
			for ( auto iter = _objects.begin(); iter != _objects.end(); ++iter )
			{
				__KF_DESTROY__( ObjectType, iter->second );
			}
		}

		_objects.clear();
	}

	template< class KeyType, class ParamType, class ObjectType, class Mutex >
	bool KFMap< KeyType, ParamType, ObjectType, Mutex >::IsEmpty()
	{
		KFLocker< Mutex > locker( _kf_mutex );
		return _objects.empty();
	}

	template< class KeyType, class ParamType, class ObjectType, class Mutex >
	bool KFMap< KeyType, ParamType, ObjectType, Mutex >::IsExist( ParamType key )
	{
		return Find( key ) != nullptr;
	}

	template< class KeyType, class ParamType, class ObjectType, class Mutex >
	void KFMap< KeyType, ParamType, ObjectType, Mutex >::Insert( ParamType key, ObjectType* object )
	{
		Remove( key, true );

		KFLocker< Mutex > locker( _kf_mutex );
		_objects[ key ] = object;
	}

	template< class KeyType, class ParamType, class ObjectType, class Mutex >
	ObjectType* KFMap< KeyType, ParamType, ObjectType, Mutex >::Find( ParamType key )
	{
		KFLocker< Mutex > locker( _kf_mutex );
		auto iter = _objects.find( key );
		if ( iter == _objects.end() )
		{
			return nullptr;
		}

		return iter->second;
	}

	template< class KeyType, class ParamType, class ObjectType, class Mutex >
	ObjectType* KFMap< KeyType, ParamType, ObjectType, Mutex >::Create( ParamType key )
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

	template< class KeyType, class ParamType, class ObjectType, class Mutex >
	bool KFMap< KeyType, ParamType, ObjectType, Mutex >::Remove( ParamType key, bool isdelete )
	{
		KFLocker< Mutex > locker( _kf_mutex );

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

	template< class KeyType, class ParamType, class ObjectType, class Mutex >
	ObjectType* KFMap< KeyType, ParamType, ObjectType, Mutex >::First()
	{
		KFLocker< Mutex > locker( _kf_mutex );

		if ( _objects.empty() )
		{
			return nullptr;
		}

		_iter = _objects.begin();
		return _iter->second;
	}

	template< class KeyType, class ParamType, class ObjectType, class Mutex >
	ObjectType* KFMap< KeyType, ParamType, ObjectType, Mutex >::Next()
	{
		KFLocker< Mutex > locker( _kf_mutex );

		++_iter;
		if ( _iter == _objects.end() )
		{
			return nullptr;
		}

		return _iter->second;
	}

	template< class KeyType, class ParamType, class ObjectType, class Mutex >
	void KFMap< KeyType, ParamType, ObjectType, Mutex >::AddMap( KFMap& kfother )
	{
		for ( auto iter : kfother._objects )
		{
			Insert( iter.first, iter.second );
		}

		kfother._objects.clear();
	}
}
#endif
