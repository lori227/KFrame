#ifndef __KF_DROP_INTERFACE_H__
#define __KF_DROP_INTERFACE_H__

#include "KFrame.h"
#include "KFXmlReader/KFDropData.h"

namespace KFrame
{
    typedef std::function< void( KFEntity*, const KFDropData*, const std::string&, uint64, const char*, uint32 ) > KFDropLogicFunction;
    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    class KFDropInterface : public KFModule
    {
    public:
        // 返回的DropDataList只包含掉落属性数据
        virtual const DropDataList& Drop( KFEntity* player, uint32 dropid, const std::string& modulename, uint64 moduleid, const char* function, uint32 line ) = 0;
        virtual const DropDataList& Drop( KFEntity* player, uint32 dropid, uint32 count, const std::string& modulename, uint64 moduleid, const char* function, uint32 line ) = 0;
        virtual const DropDataList& Drop( KFEntity* player, const UInt32Vector& droplist, const std::string& modulename, uint64 moduleid, const char* function, uint32 line ) = 0;
        /////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 注册掉落逻辑
        template< class T >
        void RegisterDropLogicFunction( const std::string& logicname, T* module, void ( T::*handle )( KFEntity*, const KFDropData*, const std::string&, uint64, const char*, uint32 ) )
        {
            KFDropLogicFunction function = std::bind( handle, module, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
                                           std::placeholders::_4, std::placeholders::_5, std::placeholders::_6 );
            BindDropLogicFunction( module, logicname, function );
        }
        virtual void UnRegisterDropLogicFunction( const std::string& logicname ) = 0;
        /////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:
        // 绑定掉落逻辑函数
        virtual void BindDropLogicFunction( KFModule* module, const std::string& logicname, KFDropLogicFunction& function ) = 0;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_drop, KFDropInterface );
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define __KF_DROP_LOGIC_FUNCTION__( _function )\
    void _function( KFEntity* player, const KFDropData* dropdata,const std::string& modulename, uint64 moduleid, const char* function, uint32 line )
#define __REGISTER_DROP_LOGIC__( dataname, function )\
    _kf_drop->RegisterDropLogicFunction( dataname, this, function )
#define  __UN_DROP_LOGIC__( dataname )\
    _kf_drop->UnRegisterDropLogicFunction( dataname )
}

#endif