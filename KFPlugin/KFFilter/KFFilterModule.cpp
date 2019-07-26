#include "KFFilterModule.hpp"
#include "KFFilterConfig.hpp"

namespace KFrame
{
    void KFFilterModule::InitModule()
    {
        __KF_ADD_CONFIG__( KFFilterConfig );
    }

    bool KFFilterModule::CheckFilter( const std::string& source )
    {
        // 不能有 空格 %
        if ( source.find( " " ) != std::string::npos ||
                source.find( "%" ) != std::string::npos )
        {
            return true;
        }

        auto data = source.data();
        auto length = static_cast< uint32 >( source.size() );

        for ( auto i = 0u; i < length; ++i )
        {
            auto filter = KFFilterConfig::Instance()->CheckFilter( data + i, length - i );
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
            auto count = KFFilterConfig::Instance()->CensorFilter( data + i, length - i );
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