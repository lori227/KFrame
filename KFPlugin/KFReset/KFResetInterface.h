#ifndef __KF_RESET_INTERFACE_H__
#define __KF_RESET_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    typedef std::function< void( KFEntity*, const KFTimeData*, uint64, uint64 ) > KFResetFunction;
    class KFResetInterface : public KFModule
    {
    public:
        // 注册重置逻辑
        template< class T >
        void RegisterResetFunction( uint32 timeid, uint32 count, T* object, void ( T::*handle )( KFEntity*, const KFTimeData*, uint64, uint64 ) )
        {
            KFResetFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 );
            AddResetFunction( timeid, count, typeid( T ).name(), function );
        }

        template< class T >
        void UnRegisterResetFunction( T* object )
        {
            RemoveResetFunction( typeid( T ).name() );
        }

    protected:
        // 添加重置函数
        virtual void AddResetFunction( uint32 timeid, uint32 count, const std::string& module, KFResetFunction& function ) = 0;

        // 删除重置函数
        virtual void RemoveResetFunction( const std::string& module ) = 0;
    };

    //////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_reset, KFResetInterface );
    //////////////////////////////////////////////////////////////////////////////////////

#define __KF_RESET_FUNCTION__( function ) void function( KFEntity* player, const KFTimeData* timedata, uint64 lasttime, uint64 nowtime )
#define __REGISTER_RESET__( timeid, function ) _kf_reset->RegisterResetFunction( timeid, 1u, this, function )
#define __REGISTER_RESET_COUNT__( timeid, count, function ) _kf_reset->RegisterResetFunction( timeid, count, this, function )
#define __UN_RESET__() _kf_reset->UnRegisterResetFunction( this )

}

#endif