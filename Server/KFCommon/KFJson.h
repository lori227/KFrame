#ifndef __KF_JSON_H__
#define __KF_JSON_H__

#include "KFDefine.h"
#include "json/json.h"
#include "KFUtility/KFUtility.h"

namespace KFrame
{
    class KFJson : public Json::Value
    {
    public:
        KFJson()
        {

        }

        KFJson( const std::string& data )
        {
            Parse( data );
        }

        KFJson( const char* data, uint32 size )
        {
            Parse( data, size );
        }

        KFJson( const Json::Value& value )
        {
            this->copy( value );
        }
        ///////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////
        inline bool Parse( const char* data, uint32 size )
        {
            std::string temp;
            temp.assign( data, size );
            return Parse( temp );
        }

        inline bool Parse( const std::string& data )
        {
            if ( data.empty() )
            {
                return false;
            }

            // 解析
            Json::Reader reader;
            return reader.parse( data, *this, false );
        }

        inline bool Parse( const MapString& values )
        {
            for ( auto& iter : values )
            {
                SetValue< const std::string& >( iter.first, iter.second );
            }

            return true;
        }

        // 系列化
        inline std::string Serialize()
        {
            return this->toStyledString();
        }

        template< class T >
        inline void SetValue( const std::string& key, T value )
        {
            this->operator[]( key ) = value;
        }

        inline int32 GetInt32( const std::string& key )
        {
            auto& value = this->operator[]( key );

            if ( value.isUInt() || value.isInt() )
            {
                return value.asInt();
            }
            else if ( value.isString() )
            {
                return KFUtility::ToValue< int32 >( value.asString() );
            }

            return _invalid_int;
        }

        inline uint32 GetUInt32( const std::string& key )
        {
            auto& value = this->operator[]( key );

            if ( value.isUInt() || value.isInt() )
            {
                return value.asUInt();
            }
            else if ( value.isString() )
            {
                return KFUtility::ToValue< uint32 >( value.asString() );
            }

            return _invalid_int;
        }

        inline void SetUInt32( const std::string& key, const std::string& value )
        {
            auto intvalue = KFUtility::ToValue< uint32 >( value );
            SetValue( key, intvalue );
        }

        inline double GetDouble( const std::string& key )
        {
            auto& value = this->operator[]( key );

            if ( value.isDouble() )
            {
                return value.asDouble();
            }
            else if ( value.isString() )
            {
                return KFUtility::ToValue< double >( value.asString() );
            }

            return _invalid_int;
        }

        inline std::string GetString( const std::string& key )
        {
            auto& value = this->operator[]( key );

            if ( value.isUInt() )
            {
                return KFUtility::ToString< uint32 >( value.asUInt() );
            }
            else if ( value.isInt() )
            {
                return KFUtility::ToString< int32 >( value.asInt() );
            }
            else if ( value.isString() )
            {
                return value.asString();
            }
            return _invalid_str;
        }

        // 参数待定
        inline bool GetArray( const std::string& key, std::vector<std::string>& matchkeys, int& cursor, std::map<std::string, std::string>& des )
        {
            des.clear();
            auto& value = this->operator[]( key );
            if ( !value.isArray() )
            {
                return false;
            }
            if ( cursor > ( int )value.size() )
            {
                return false;
            }

            auto values = value[ cursor ];
            if ( !values )
            {
                return false;
            }

            for ( auto& iter : matchkeys )
            {
                std::string mapkey = iter;
                auto& elem = values.operator[]( iter );
                if ( elem.isInt() )
                {
                    auto strelem = KFUtility::ToString< uint32 >( elem.asUInt() );
                    des.insert( std::make_pair( mapkey, strelem ) );
                }
                else if ( elem.isUInt() )
                {
                    auto strelem = KFUtility::ToString< int32 >( value.asInt() );
                    des.insert( std::make_pair( mapkey, strelem ) );

                }
                else if ( elem.isString() )
                {
                    des.insert( std::make_pair( mapkey, elem.asString() ) );
                }
            }

            cursor++;
            return true;
        }
    };
}

#endif
