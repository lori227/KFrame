#ifndef __KF_ELEMENT_CONFIG_H__
#define __KF_ELEMENT_CONFIG_H__

#include "KFConfig.h"
#include "KFCore/KFElement.h"

namespace KFrame
{
    class KFElementSetting
    {
    public:
        void AddData( const std::string& dataname, const std::string& datavalue, uint32 dataid = 0u )
        {
            _data_list.emplace_back( std::make_tuple( dataname, datavalue, dataid ) );
        }

        void AddData( const std::string& dataname, uint32 datavalue, uint32 dataid = 0u )
        {
            _data_list.emplace_back( std::make_tuple( dataname, __TO_STRING__( datavalue ), dataid ) );
        }
    public:
        std::list<std::tuple<std::string, std::string, uint32>> _data_list;
    };
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef std::function< const std::string&( const std::string& ) > KFElementParseStringFunction;
    typedef std::function< const std::string&( const std::string&, const std::string&, uint32 ) > KFElementStrStringFunction;
    typedef std::function< const std::string&( const std::string&, uint32, uint32 ) > KFElementIntStringFunction;
    typedef std::function< const std::string&( const KFElementSetting& ) > KFElementSettingStringFunction;
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    class KFElementConfig : public KFConfig, public KFInstance< KFElementConfig >
    {
    public:
        template< class T >
        void SetParseStringFunction( T* object, const std::string & ( T::*handle )( const std::string& ) )
        {
            _parse_string_function = std::bind( handle, object, std::placeholders::_1 );
        }

        template< class T >
        void SetIntStringFunction( T* object, const std::string & ( T::*handle )( const std::string&, uint32, uint32 ) )
        {
            _int_string_function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
        }

        template< class T >
        void SetStrStringFunction( T* object, const std::string & ( T::*handle )( const std::string&, const std::string&, uint32 ) )
        {
            _str_string_function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
        }

        template< class T >
        void SetSettingStringFunction( T* object, const std::string & ( T::*handle )( const KFElementSetting& ) )
        {
            _setting_string_function = std::bind( handle, object, std::placeholders::_1 );
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////
        const std::string& ParseString( const std::string& strparse )
        {
            return _parse_string_function( strparse );
        }

        const std::string& FormatString( const std::string& dataname, uint32 datavalue, uint32 dataid = 0u )
        {
            return _int_string_function( dataname, datavalue, dataid );
        }

        const std::string& FormatString( const std::string& dataname, const std::string& datavalue, uint32 dataid = 0u )
        {
            return _str_string_function( dataname, datavalue, dataid );
        }

        const std::string& FormatString( const KFElementSetting& kfsetting )
        {
            return _setting_string_function( kfsetting );
        }

        bool FormatElement( KFElements& kfelements, const std::string& dataname, uint32 datavalue, uint32 dataid = 0u )
        {
            auto& strelement = _int_string_function( dataname, datavalue, dataid );
            if ( strelement.empty() )
            {
                return false;
            }

            return kfelements.Parse( strelement, __FUNC_LINE__ );
        }

        bool FormatElement( KFElements& kfelements, const std::string& dataname, const std::string& datavalue, uint32 dataid = 0u )
        {
            auto& strelement = _str_string_function( dataname, datavalue, dataid );
            if ( strelement.empty() )
            {
                return false;
            }

            return kfelements.Parse( strelement, __FUNC_LINE__ );
        }

        bool FormatElement( KFElements& kfelements, const KFElementSetting& kfsetting )
        {
            auto& strelement = _setting_string_function( kfsetting );
            if ( strelement.empty() )
            {
                return false;
            }

            return kfelements.Parse( strelement, __FUNC_LINE__ );
        }

        bool ParseElement( KFElements& kfelements, const char* file, uint64 id )
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
    private:
        // 解析函数
        KFElementParseStringFunction _parse_string_function = nullptr;
        KFElementIntStringFunction _int_string_function = nullptr;
        KFElementStrStringFunction _str_string_function = nullptr;
        KFElementSettingStringFunction _setting_string_function = nullptr;
    };
}

#endif