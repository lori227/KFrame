#ifndef __KF_DROP_INTERFACE_H__
#define __KF_DROP_INTERFACE_H__

#include "KFrame.h"
#include "KFDropData.h"

namespace KFrame
{
    typedef std::function< void( EntityPtr, const KFDropData*, const std::string&, uint64, const char*, uint32 ) > KFDropLogicFunction;
    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    class KFDropInterface : public KFModule
    {
    public:
        // 返回的DropDataList只包含掉落属性数据
        virtual const DropDataList& Drop( EntityPtr player, uint32 drop_id, const std::string& module_name, uint64 module_id, const char* function, uint32 line ) = 0;
        virtual const DropDataList& Drop( EntityPtr player, uint32 drop_id, uint32 count, const std::string& module_name, uint64 module_id, const char* function, uint32 line ) = 0;
        virtual const DropDataList& Drop( EntityPtr player, const UInt32Vector& drop_list, const std::string& module_name, uint64 module_id, const char* function, uint32 line ) = 0;
        /////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 注册掉落逻辑
        template<class T>
        void RegisterDropLogicFunction( const std::string& logic_name, T* module, void ( T::*handle )( EntityPtr, const KFDropData*, const std::string&, uint64, const char*, uint32 ) )
        {
            KFDropLogicFunction function = std::bind( handle, module, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
                                           std::placeholders::_4, std::placeholders::_5, std::placeholders::_6 );
            BindDropLogicFunction( logic_name, module, function );
        }
        virtual void UnRegisterDropLogicFunction( const std::string& logic_name ) = 0;
        /////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:
        // 绑定掉落逻辑函数
        virtual void BindDropLogicFunction(  const std::string& logic_name, KFModule* module, KFDropLogicFunction& function ) = 0;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_drop, KFDropInterface );
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define __KF_DROP_LOGIC_FUNCTION__( function_name )\
    void function_name( EntityPtr player, const KFDropData* drop_data,const std::string& module_name, uint64 module_id, const char* function, uint32 line )
#define __REGISTER_DROP_LOGIC__( data_name, function )\
    _kf_drop->RegisterDropLogicFunction( data_name, this, function )
#define  __UN_DROP_LOGIC__( data_name )\
    _kf_drop->UnRegisterDropLogicFunction( data_name )
}

#endif