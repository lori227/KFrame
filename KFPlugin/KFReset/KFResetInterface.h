#ifndef __KF_RESET_INTERFACE_H__
#define __KF_RESET_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    typedef std::function< void( KFEntity*, uint32, uint64, uint64 ) > KFResetFunction;
    class KFResetInterface : public KFModule
    {
    public:
        // 注册重置逻辑
        template< class T >
        void RegisterResetFunction( const std::string& functionname, uint32 count, T* object, void ( T::*handle )( KFEntity*, uint32, uint64, uint64 ) )
        {
            KFResetFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 );
            AddResetFunction( functionname, count, function );
        }

        void UnRegisterResetFunction( const std::string& functionname )
        {
            RemoveResetFunction( functionname );
        }

        // 获得下一次重置时间
        virtual uint64 CalcNextResetTime( KFEntity* player, uint32 timeid ) = 0;

        // 设置重置时间
        virtual void SetResetTime( KFEntity* player, uint32 timeid, uint64 nowtime ) = 0;
    protected:
        // 添加重置函数
        virtual void AddResetFunction( const std::string& functionname, uint32 count, KFResetFunction& function ) = 0;

        // 删除重置函数
        virtual void RemoveResetFunction( const std::string& functionname ) = 0;
    };

    //////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_reset, KFResetInterface );
    //////////////////////////////////////////////////////////////////////////////////////

#define __KF_RESET_FUNCTION__( function ) void function( KFEntity* player, uint32 timeid, uint64 lastresettime, uint64 nowresettime )
#define __REGISTER_RESET__( functionname, function ) _kf_reset->RegisterResetFunction( functionname, 1u, this, function )
#define __REGISTER_RESET_COUNT__( functionname, count, function ) _kf_reset->RegisterResetFunction( functionname, count, this, function )
#define __UN_RESET__( functionname ) _kf_reset->UnRegisterResetFunction( functionname )

}

#endif