#ifndef __KF_ID_H__
#define __KF_ID_H__

#include "KFInclude.h"

namespace KFrame
{
	template< class T >
	class KFID
	{
	public:
		KFID()
		{
			_head_id = 0;
			_data_id = 0;
		}

		KFID( T headid, T dataid )
		{
			_head_id = headid;
			_data_id = dataid;
		}

		KFID( const KFID& kfid )
		{
			_head_id = kfid._head_id;
			_data_id = kfid._data_id;
		}

		KFID& operator = ( const KFID& kfid )
		{
			_head_id = kfid._head_id;
			_data_id = kfid._data_id;

			return *this;
		}

		bool operator == ( const KFID& kfid ) const
		{
			return _head_id == kfid._head_id && _data_id == kfid._data_id;
		}

		bool operator != ( const KFID& kfid ) const
		{
			return _head_id != kfid._head_id || _data_id != kfid._data_id;
		}

		bool operator > ( const KFID& kfid ) const
		{
			if ( _head_id == kfid._head_id )
			{
				return _data_id > kfid._data_id;
			}

			return _head_id > kfid._head_id;
		}

		bool operator < ( const KFID& kfid ) const
		{
			if ( _head_id == kfid._head_id )
			{
				return _data_id < kfid._data_id;
			}

			return _head_id < kfid._head_id;
		}

	public:
		T _head_id;
		T _data_id;
	};

	typedef KFID< uint32 > KFGuid;

#define  __KF_DATA_ID__( kfguid ) kfguid._data_id
#define  __KF_HEAD_ID__( kfguid ) kfguid._head_id
	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
}

#endif
