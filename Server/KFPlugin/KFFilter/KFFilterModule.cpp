#include "KFFilterModule.h"
#include "KFFilterConfig.h"

namespace KFrame
{
    void KFFilterModule::InitModule()
    {
        __KF_ADD_CONFIG__( _kf_filter_config, true );
    }

    void KFFilterModule::ShutDown()
    {
        __KF_REMOVE_CONFIG__( _kf_filter_config );
    }

    bool KFFilterModule::CheckFilter( const std::string& source )
    {
        auto data = source.data();
        auto length = static_cast< uint32 >( source.size() );

        for ( auto i = 0u; i < length; ++i )
        {
            auto filter = _kf_filter_config->CheckFilter( data + i, length - i );
            if ( filter )
            {
                return true;
            }
        }

        return false;
    }

    bool KFFilterModule::CensorFilter( std::string& source )
    {
        bool filter = false;
        auto length = static_cast< uint32 >( source.size() );

        for ( auto i = 0u; i < length; ++i )
        {
            auto data = source.data();
            auto count = _kf_filter_config->CensorFilter( data + i, length - i );
            if ( count > 0 )
            {
                filter = true;
                source.replace( i, count, "**" );

                i += 1;
                length = static_cast< uint32 >( source.size() );
            }
        }

        return filter;
    }
}