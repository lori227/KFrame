#include "KFHttpURL.h"
#include "KFURLCharacter.hpp"

namespace KFrame
{

    std::string KFHttpURL::EncodeParams( const std::map<std::string, std::string>& params )
    {
        std::string source = "?";
        for ( auto it = params.begin(); it != params.end(); ++it )
        {
            if ( it != params.begin() )
            {
                source.append( "&" );
            }
            source.append( it->first );
            source.append( "=" );
            source.append( it->second );
        }

        source = KFURLCharacter::encodeURIValue( source );

        return source;
    }

}