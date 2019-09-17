#ifndef __KF_DROP_INTERFACE_H__
#define __KF_DROP_INTERFACE_H__

#include "KFrame.h"
#include "KFZConfig/KFDropData.h"

namespace KFrame
{
    typedef std::function< void( KFEntity*, const KFDropData* ) > KFDropLogicFunction;
    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    class KFDropInterface : public KFModule
    {
    public:
        // 掉落逻辑
        // 返回的DropDataList只包含掉落属性数据
        virtual DropDataList& Drop( KFEntity* player, uint32 dropid, const char* function, uint32 line ) = 0;
        virtual DropDataList& Drop( KFEntity* player, uint32 dropid, uint32 count, const char* function, uint32 line ) = 0;
        virtual DropDataList& Drop( KFEntity* player, uint32 dropid, uint32 count, bool showclient, const char* function, uint32 line ) = 0;
        /////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 注册掉落逻辑
        template< class T >
        void RegisterDropLogicFunction( const std::string& dataname, T* object, void ( T::*handle )( KFEntity*, const KFDropData* ) )
        {
            KFDropLogicFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2 );
            BindDropLogicFunction( dataname, function );
        }
        virtual void UnRegisterDropLogicFunction( const std::string& dataname ) = 0;

    protected:
        // 绑定掉落逻辑函数
        virtual void BindDropLogicFunction( const std::string& dataname, KFDropLogicFunction& function ) = 0;
    };

    //////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_drop, KFDropInterface );
    //////////////////////////////////////////////////////////////////////////
#define __KF_DROP_LOGIC_FUNCTION__( function )\
    void function( KFEntity* player, const KFDropData* dropdata )
#define __REGISTER_DROP_LOGIC__( dataname, function )\
    _kf_drop->RegisterDropLogicFunction( dataname, this, function )
#define  __UN_DROP_LOGIC__( dataname )\
    _kf_drop->UnRegisterDropLogicFunction( dataname )
}

#endif