#ifndef __KF_DATA_CLIENT_INTERFACE_H__
#define __KF_DATA_CLIENT_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    typedef std::function< void( uint32, uint64, const KFMsg::PBObject* ) > KFQueryPlayerFunction;
    typedef std::function< void( uint32, const KFMsg::PBLoginData*, const KFMsg::PBObject* ) > KFLoadPlayerFunction;
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class KFDataClientInterface : public KFModule
    {
    public:
        // 绑定加载玩家数据回调函数
        template<class T>
        void BindLoadPlayerFunction( T* module, void ( T::*handle )( uint32, const KFMsg::PBLoginData*, const KFMsg::PBObject* ) )
        {
            KFLoadPlayerFunction function = std::bind( handle, module, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
            SetLoadPlayerFunction( module, function );
        }

        // 取消绑定
        template<class T>
        void UnBindLoadPlayerFunction( T* module )
        {
            KFLoadPlayerFunction function = nullptr;
            SetLoadPlayerFunction( module, function );
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 查询玩家数据回调函数
        template<class T>
        void BindQueryPlayerFunction( T* module, void ( T::*handle )( uint32, uint64, const KFMsg::PBObject* ) )
        {
            KFQueryPlayerFunction function = std::bind( handle, module, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
            SetQueryPlayerFunction( module, function );
        }

        // 取消绑定
        template<class T>
        void UnBindQueryPlayerFunction( T* module )
        {
            KFQueryPlayerFunction function = nullptr;
            SetQueryPlayerFunction( module, function );
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 加载玩家数据
        virtual bool LoadPlayerData( const KFMsg::PBLoginData* login_data ) = 0;
        virtual void RemoveLoadData( uint64 player_id ) = 0;

        // 保存玩家数据
        virtual bool SavePlayerData( uint64 player_id, const KFMsg::PBObject* player_data, uint32 save_flag ) = 0;

        // 查询玩家数据
        virtual bool QueryPlayerData( uint64 send_id, uint64 player_id ) = 0;
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////

    protected:
        // 设置回调函数
        virtual void SetLoadPlayerFunction( KFModule* module, KFLoadPlayerFunction& function ) = 0;
        virtual void SetQueryPlayerFunction( KFModule* module, KFQueryPlayerFunction& function ) = 0;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_data_client, KFDataClientInterface );
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}



#endif