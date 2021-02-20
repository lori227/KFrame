#ifndef __KF_ELEMENT_FORMAT_H__
#define __KF_ELEMENT_FORMAT_H__

#include "KFUtility/KFElement.h"

namespace KFrame
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    class KFElementFormat
    {
    public:
        template<class T>
        void RegisterParseToElementStringFunction( T* object, const std::string & ( T::*handle )( const std::string& ) )
        {
            KFParseToElementStringFunction function = std::bind( handle, object, std::placeholders::_1 );
            BindParseToElementStringFunction( function );
        }
        void BindParseToElementStringFunction( KFParseToElementStringFunction& function );

        template<class T>
        void RegisterIntStringFunction( T* object, const std::string & ( T::*handle )( const std::string&, uint32, uint32 ) )
        {
            KFIntToElementStringFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
            BindIntToElementStringFunction( function );
        }
        void BindIntToElementStringFunction( KFIntToElementStringFunction& function );

        template<class T>
        void RegisterStrStringFunction( T* object, const std::string & ( T::*handle )( const std::string&, const std::string&, uint32 ) )
        {
            KFStrToElementStringFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
            BindStrToElementStringFunction( function );
        }
        void BindStrToElementStringFunction( KFStrToElementStringFunction& function );

        template<class T>
        void RegisterSettingStringFunction( T* object, const std::string & ( T::*handle )( const KFElementData& ) )
        {
            KFDataToElementStringFunction function = std::bind( handle, object, std::placeholders::_1 );
            BindDataToElementStringFunction( function );
        }
        void BindDataToElementStringFunction( KFDataToElementStringFunction& function );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 解析元数据字串
        const std::string& ParseString( const std::string& strparse );

        // 格式化成元数据结构
        const std::string& FormatString( const std::string& data_name, uint32 datavalue, uint32 dataid = 0u );
        const std::string& FormatString( const std::string& data_name, const std::string& datavalue, uint32 dataid = 0u );
        const std::string& FormatString( const KFElementData& elementdata );

        // 格式化
        bool FormatElement( KFElements& kfelements, const std::string& data_name, uint32 datavalue, uint32 dataid = 0u );
        bool FormatElement( KFElements& kfelements, const std::string& data_name, const std::string& datavalue, uint32 dataid = 0u );
        bool FormatElement( KFElements& kfelements, const KFElementData& elementdata );
        bool ParseElement( KFElements& kfelements, const char* file, uint64 id );

    private:
        // 解析函数
        KFParseToElementStringFunction _parse_string_function = nullptr;
        KFIntToElementStringFunction _int_string_function = nullptr;
        KFStrToElementStringFunction _str_string_function = nullptr;
        KFDataToElementStringFunction _data_string_function = nullptr;
    };
}

#endif