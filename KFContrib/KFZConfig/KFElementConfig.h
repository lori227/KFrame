#ifndef __KF_ELEMENT_CONFIG_H__
#define __KF_ELEMENT_CONFIG_H__

#include "KFConfig.h"
#include "KFCore/KFElement.h"

namespace KFrame
{
    typedef std::function< bool( KFElements&, const std::string& ) > KFElementParseFunction;
    typedef std::function< bool( KFElements&, const std::string&, const std::string&, uint32 ) > KFElementFormatFunction;
    typedef std::function< const std::string&( const std::string&, uint32, uint32 ) > KFElementStringFunction;
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    class KFElementConfig : public KFConfig, public KFInstance< KFElementConfig >
    {
    public:
        template< class T >
        void SetParseFunction( T* object, bool ( T::*handle )( KFElements&, const std::string& ) )
        {
            _parse_function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2 );
        }

        template< class T >
        void SetFormatFunction( T* object, bool ( T::*handle )( KFElements&, const std::string&, const std::string&, uint32 ) )
        {
            _format_function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 );
        }

        template< class T >
        void SetStringFunction( T* object, const std::string & ( T::*handle )( const std::string&, uint32, uint32 ) )
        {
            _string_function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////
        bool ParseElement( KFElements& kfelements, const std::string& strelement, const char* file, uint64 id )
        {
            auto ok = false;
            try
            {
                ok = _parse_function( kfelements, strelement );
                if ( !ok )
                {
                    __LOG_ERROR__( "file=[{}] id=[{}] element=[{}] parse failed", file, id, strelement );
                }
            }
            catch ( ... )
            {
                __LOG_ERROR__( "file=[{}] id=[{}] element=[{}] exception", file, id, strelement );
            }

            return ok;
        }

        bool FormatElemnt( KFElements& kfelements, const std::string& dataname, const std::string& datavalue, uint32 dataid )
        {
            return _format_function( kfelements, dataname, datavalue, dataid );
        }

        const std::string& StringElemnt( const std::string& dataname, uint32 datavalue, uint32 dataid )
        {
            return _string_function( dataname, datavalue, dataid );
        }

    private:
        // 解析函数
        KFElementParseFunction _parse_function = nullptr;

        // 格式化函数
        KFElementFormatFunction _format_function = nullptr;
        KFElementStringFunction _string_function = nullptr;
    };
}

#endif