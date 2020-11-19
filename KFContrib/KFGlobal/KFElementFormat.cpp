#include "KFElementFormat.hpp"

namespace KFrame
{
    void KFElementFormat::BindParseToElementStringFunction( KFParseToElementStringFunction& function )
    {
        _parse_string_function = function;
    }

    void KFElementFormat::BindIntToElementStringFunction( KFIntToElementStringFunction& function )
    {
        _int_string_function = function;
    }

    void KFElementFormat::BindStrToElementStringFunction( KFStrToElementStringFunction& function )
    {
        _str_string_function = function;
    }

    void KFElementFormat::BindDataToElementStringFunction( KFDataToElementStringFunction& function )
    {
        _data_string_function = function;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    const std::string& KFElementFormat::ParseString( const std::string& strparse )
    {
        return _parse_string_function( strparse );
    }

    const std::string& KFElementFormat::FormatString( const std::string& dataname, uint32 datavalue, uint32 dataid )
    {
        return _int_string_function( dataname, datavalue, dataid );
    }

    const std::string& KFElementFormat::FormatString( const std::string& dataname, const std::string& datavalue, uint32 dataid )
    {
        return _str_string_function( dataname, datavalue, dataid );
    }

    const std::string& KFElementFormat::FormatString( const KFElementData& elementdata )
    {
        return _data_string_function( elementdata );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFElementFormat::FormatElement( KFElements& kfelements, const std::string& dataname, uint32 datavalue, uint32 dataid )
    {
        auto& strelement = _int_string_function( dataname, datavalue, dataid );
        if ( strelement.empty() )
        {
            return false;
        }

        return kfelements.Parse( strelement, __FUNC_LINE__ );
    }

    bool KFElementFormat::FormatElement( KFElements& kfelements, const std::string& dataname, const std::string& datavalue, uint32 dataid )
    {
        auto& strelement = _str_string_function( dataname, datavalue, dataid );
        if ( strelement.empty() )
        {
            return false;
        }

        return kfelements.Parse( strelement, __FUNC_LINE__ );
    }

    bool KFElementFormat::FormatElement( KFElements& kfelements, const KFElementData& elementdata )
    {
        auto& strelement = _data_string_function( elementdata );
        if ( strelement.empty() )
        {
            return false;
        }

        return kfelements.Parse( strelement, __FUNC_LINE__ );
    }

    bool KFElementFormat::ParseElement( KFElements& kfelements, const char* file, uint64 id )
    {
        if ( kfelements._str_parse.empty() )
        {
            return true;
        }

        auto ok = false;
        try
        {
            auto& strelement = _parse_string_function( kfelements._str_parse );
            if ( !strelement.empty() )
            {
                ok = kfelements.Parse( strelement, file, ( uint32 )id );
                if ( ok )
                {
                    kfelements._str_parse.clear();
                }
            }
            else
            {
                __LOG_ERROR__( "file=[{}] id=[{}] element=[{}] parse failed", file, id, kfelements._str_parse );
            }

        }
        catch ( ... )
        {
            __LOG_ERROR__( "file=[{}] id=[{}] element=[{}] exception", file, id, kfelements._str_parse );
        }

        return ok;
    }
}