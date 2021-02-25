#ifndef __KF_RESET_INTERFACE_H__
#define __KF_RESET_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    typedef std::function< void( EntityPtr, uint32, uint64, uint64 ) > KFResetFunction;
    class KFResetInterface : public KFModule
    {
    public:
        // 注册重置逻辑
        template<class T>
        void RegisterResetFunction( const std::string& function_name, uint32 count, T* module, void ( T::*handle )( EntityPtr, uint32, uint64, uint64 ) )
        {
            KFResetFunction function = std::bind( handle, module, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 );
            AddResetFunction( function_name, count, module, function );
        }

        void UnRegisterResetFunction( const std::string& function_name )
        {
            RemoveResetFunction( function_name );
        }

        // 获得下一次重置时间
        virtual uint64 CalcNextResetTime( EntityPtr player, uint32 time_id ) = 0;

        // 设置重置时间
        virtual void SetResetTime( EntityPtr player, uint32 time_id, uint64 now_time ) = 0;
    protected:
        // 添加重置函数
        virtual void AddResetFunction( const std::string& function_name, uint32 count, KFModule* module, KFResetFunction& function ) = 0;

        // 删除重置函数
        virtual void RemoveResetFunction( const std::string& function_name ) = 0;
    };

    //////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_reset, KFResetInterface );
    //////////////////////////////////////////////////////////////////////////////////////

#define __KF_RESET_FUNCTION__( function ) void function( EntityPtr player, uint32 timeid, uint64 last_reset_time, uint64 now_reset_time )
#define __REGISTER_RESET__( function_name, function ) _kf_reset->RegisterResetFunction( function_name, 1u, this, function )
#define __REGISTER_RESET_COUNT__( function_name, count, function ) _kf_reset->RegisterResetFunction( function_name, count, this, function )
#define __UN_RESET__( function_name ) _kf_reset->UnRegisterResetFunction( function_name )

}

#endif