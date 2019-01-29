#ifndef __KF_ATTRIBUTE_INTERFACE_H__
#define __KF_ATTRIBUTE_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    typedef std::function< void( KFEntity*, uint64, uint64 ) > KFRewardFunction;

    class KFAttributeInterface : public KFModule
    {
    public:
        template< class T >
        void RegisterRewardFunction( const std::string& dataname, T* object, void ( T::*handle )( KFEntity*, uint64, uint64 ) )
        {
            KFRewardFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
            BindRewardFunction( dataname, function );
        }

        virtual void UnRegisterRewardFunction( const std::string& dataname ) = 0;

    protected:
        virtual void BindRewardFunction( const std::string& dataname ) = 0;
    };

    /////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_attribute, KFAttributeInterface );
    /////////////////////////////////////////////////////////////////////////
#define __KF_REWARD_FUNCTION__( function ) \
    void function( KFEntity* player, uint64 dataid, uint64 subdataid )

#define __REGISTER_REWARD_FUNCTION__( dataname, function )\
    _kf_attribute->RegisterRewardFunction( dataname, this, function )

#define __UNREGISTER_REWARD_FUNCTION__(dataname)\
    _kf_attribute->UnRegisterRewardFunction( this )
}

#endif