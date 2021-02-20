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
    const std::string& KFElementFormat::ParseString( const std::string& str_parse )
    {
        return _parse_string_function( str_parse );
    }

    const std::string& KFElementFormat::FormatString( const std::string& data_name, uint32 data_value, uint32 data_id )
    {
        return _int_string_function( data_name, data_value, data_id );
    }

    const std::string& KFElementFormat::FormatString( const std::string& data_name, const std::string& data_value, uint32 data_id )
    {
        return _str_string_function( data_name, data_value, data_id );
    }

    const std::string& KFElementFormat::FormatString( const KFElementData& element_data )
    {
        return _data_string_function( element_data );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFElementFormat::FormatElement( KFElements& elements, const std::string& data_name, uint32 data_value, uint32 data_id )
    {
        auto& str_element = _int_string_function( data_name, data_value, data_id );
        if ( str_element.empty() )
        {
            return false;
        }

        return elements.Parse( str_element, __FUNC_LINE__ );
    }

    bool KFElementFormat::FormatElement( KFElements& elements, const std::string& data_name, const std::string& data_value, uint32 data_id )
    {
        auto& str_element = _str_string_function( data_name, data_value, data_id );
        if ( str_element.empty() )
        {
            return false;
        }

        return elements.Parse( str_element, __FUNC_LINE__ );
    }

    bool KFElementFormat::FormatElement( KFElements& elements, const KFElementData& element_data )
    {
        auto& str_element = _data_string_function( element_data );
        if ( str_element.empty() )
        {
            return false;
        }

        return elements.Parse( str_element, __FUNC_LINE__ );
    }

    bool KFElementFormat::ParseElement( KFElements& elements, const char* file, uint64 id )
    {
        if ( elements._str_parse.empty() )
        {
            return true;
        }

        auto ok = false;
        try
        {
            auto& str_element = _parse_string_function( elements._str_parse );
            if ( !str_element.empty() )
            {
                ok = elements.Parse( str_element, file, ( uint32 )id );
                if ( ok )
                {
                    elements._str_parse.clear();
                }
            }
            else
            {
                __LOG_ERROR__( "file=[{}] id=[{}] element=[{}] parse failed", file, id, elements._str_parse );
            }

        }
        catch ( ... )
        {
            __LOG_ERROR__( "file=[{}] id=[{}] element=[{}] exception", file, id, elements._str_parse );
        }

        return ok;
    }
}