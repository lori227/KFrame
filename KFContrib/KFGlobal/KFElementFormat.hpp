#ifndef __KF_ELEMENT_FORMAT_H__
#define __KF_ELEMENT_FORMAT_H__

#include "KFMacros.h"
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
        const std::string& ParseString( const std::string& str_parse );

        // 格式化成元数据结构
        const std::string& FormatString( const std::string& data_name, uint32 data_value, uint32 data_id = 0u );
        const std::string& FormatString( const std::string& data_name, const std::string& data_value, uint32 data_id = 0u );
        const std::string& FormatString( const KFElementData& element_data );

        // 格式化
        bool FormatElement( KFElements& elements, const std::string& data_name, uint32 data_value, uint32 data_id = 0u );
        bool FormatElement( KFElements& elements, const std::string& data_name, const std::string& data_value, uint32 data_id = 0u );
        bool FormatElement( KFElements& elements, const KFElementData& element_data );
        bool ParseElement( KFElements& elements, const char* file, uint64 id );

    private:
        // 解析函数
        KFParseToElementStringFunction _parse_string_function = nullptr;
        KFIntToElementStringFunction _int_string_function = nullptr;
        KFStrToElementStringFunction _str_string_function = nullptr;
        KFDataToElementStringFunction _data_string_function = nullptr;
    };
}

#endif