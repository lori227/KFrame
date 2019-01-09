#ifndef __KF_DATA_CLIENT_INTERFACE_H__
#define __KF_DATA_CLIENT_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFEntity;
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class KFDataClientInterface : public KFModule
    {
    public:
        // 绑定加载玩家数据回调函数
        template< class T >
        void BindLoadPlayerFunction( T* object, void ( T::*handle )( uint32, const KFMsg::PBLoginData*, KFMsg::PBObject* ) )
        {
            KFLoadPlayerFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
            SetLoadPlayerFunction( function );
        }

        // 取消绑定
        template< class T >
        void UnBindLoadPlayerFunction( T* object )
        {
            KFLoadPlayerFunction function = nullptr;
            SetLoadPlayerFunction( function );
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 加载玩家数据
        virtual bool LoadPlayerData( const KFMsg::PBLoginData* pblogin ) = 0;

        // 保存玩家数据
        virtual bool SavePlayerData( KFEntity* player ) = 0;

    protected:
        // 设置回调函数
        virtual void SetLoadPlayerFunction( KFLoadPlayerFunction& function ) = 0;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_data, KFDataClientInterface );
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}



#endif