#ifndef __KF_RESET_INTERFACE_H__
#define __KF_RESET_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    typedef std::function< void( KFEntity*, uint64, uint64 ) > KFResetFunction;
    class KFResetInterface : public KFModule
    {
    public:
        // 注册重置逻辑
        template< class T >
        void RegisterResetFunction( const KFTimeData& timedata, uint32 type, uint32 count, T* object, void ( T::*handle )( KFEntity*, uint64, uint64 ) )
        {
            KFResetFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
            AddResetFunction( timedata, type, count, typeid( T ).name(), function );
        }

        template< class T >
        void UnRegisterResetFunction( T* object )
        {
            RemoveResetFunction( typeid( T ).name() );
        }

    protected:
        // 添加重置函数
        virtual void AddResetFunction( const KFTimeData& timedata, uint32 type, uint32 count, const std::string& module, KFResetFunction& function ) = 0;

        // 删除重置函数
        virtual void RemoveResetFunction( const std::string& module ) = 0;
    };

    //////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_reset, KFResetInterface );
    //////////////////////////////////////////////////////////////////////////////////////

#define __KF_RESET_FUNCTION__( function ) void function( KFEntity* player, uint64 lasttime, uint64 nowtime )
#define __REGISTER_RESET_ONCE__( timedata, function ) _kf_reset->RegisterResetFunction( timedata, KFResetEnum::Once, 1u, this, function )
#define __REGISTER_RESET_DAILY__( timedata, count, function ) _kf_reset->RegisterResetFunction( timedata, KFResetEnum::Daily, count, this, function )
#define __UN_RESET__() _kf_reset->UnRegisterResetFunction( this )

}

#endif