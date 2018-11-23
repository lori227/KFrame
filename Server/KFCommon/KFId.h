#ifndef __KF_ID_H__
#define __KF_ID_H__

#include "KFInclude.h"

namespace KFrame
{
    template< class T >
    class KFIDTemplate
    {
    public:
        KFIDTemplate()
        {
            _head_id = 0;
            _data_id = 0;
        }

        KFIDTemplate( T headid, T dataid )
        {
            _head_id = headid;
            _data_id = dataid;
        }

        KFIDTemplate( const KFIDTemplate& kfid )
        {
            _head_id = kfid._head_id;
            _data_id = kfid._data_id;
        }

        KFIDTemplate& operator = ( const KFIDTemplate& kfid )
        {
            _head_id = kfid._head_id;
            _data_id = kfid._data_id;

            return *this;
        }

        bool operator == ( const KFIDTemplate& kfid ) const
        {
            return _head_id == kfid._head_id && _data_id == kfid._data_id;
        }

        bool operator != ( const KFIDTemplate& kfid ) const
        {
            return _head_id != kfid._head_id || _data_id != kfid._data_id;
        }

        bool operator > ( const KFIDTemplate& kfid ) const
        {
            if ( _head_id == kfid._head_id )
            {
                return _data_id > kfid._data_id;
            }

            return _head_id > kfid._head_id;
        }

        bool operator < ( const KFIDTemplate& kfid ) const
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

    typedef KFIDTemplate< uint32 > KFId;

#define  __KF_DATA_ID__( kfid ) kfid._data_id
#define  __KF_HEAD_ID__( kfid ) kfid._head_id
    /////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////
}

#endif
