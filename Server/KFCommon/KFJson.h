#ifndef __KF_JSON_H__
#define __KF_JSON_H__

#include "KFDefine.h"
#include "KFMacros.h"
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

        inline void Serialize( MapString& values )
        {
            for ( auto iter = this->begin(); iter != this->end(); ++iter )
            {
                if ( iter->isString() )
                {
                    values[ iter.name() ] = iter->asString();
                }
                else if ( iter->isUInt() )
                {
                    values[ iter.name() ] = __TO_STRING__( iter->asUInt() );
                }
                else if ( iter->isInt() )
                {
                    values[ iter.name() ] = __TO_STRING__( iter->asInt() );
                }
                else if ( iter->isUInt64() )
                {
                    values[ iter.name() ] = __TO_STRING__( iter->asUInt64() );
                }
                else if ( iter->isInt64() )
                {
                    values[ iter.name() ] = __TO_STRING__( iter->asInt64() );
                }
                else if ( iter->isDouble() )
                {
                    values[ iter.name() ] = __TO_STRING__( iter->asDouble() );
                }
            }
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

        template<>
        inline void SetValue( const std::string& key, uint64 value )
        {
            this->operator[]( key ) = static_cast<uint32>( value );
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

            if ( value.isInt() )
            {
                return value.asInt();
            }
            else if ( value.isUInt() )
            {
                return value.asUInt();
            }
            else if ( value.isString() )
            {
                return KFUtility::ToValue< uint32 >( value.asString() );
            }

            return _invalid_int;
        }

        inline uint64 GetUInt64( const std::string& key )
        {
            auto& value = this->operator[]( key );

            if ( value.isInt() )
            {
                return value.asInt();
            }
            else if ( value.isUInt() )
            {
                return value.asUInt();
            }
            else if ( value.isInt64() )
            {
                return value.asInt64();
            }
            else if ( value.isUInt64() )
            {
                return value.asUInt64();
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

        inline void SetUInt64( const std::string& key, const std::string& value )
        {
            auto intvalue = KFUtility::ToValue< uint64 >( value );
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

            if ( value.isString() )
            {
                return value.asString();
            }
            else if ( value.isUInt() )
            {
                return __TO_STRING__( value.asUInt() );
            }
            else if ( value.isInt() )
            {
                return __TO_STRING__( value.asInt() );
            }
            else if ( value.isInt64() )
            {
                return __TO_STRING__( value.asInt64() );
            }
            else if ( value.isUInt64() )
            {
                return __TO_STRING__( value.asUInt64() );
            }
            else if ( value.isDouble() )
            {
                return __TO_STRING__( value.asDouble() );
            }

            return _invalid_str;
        }

        inline bool GetArray( const std::string& key, std::vector<std::string>& matchkeys, int& cursor, std::map<std::string, std::string>& des )
        {
            des.clear();
            auto& value = this->operator[]( key );
            if ( value.isNull() )
            {
                return false;
            }

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
                    auto strelem = __TO_STRING__( elem.asUInt() );
                    des.insert( std::make_pair( mapkey, strelem ) );
                }
                else if ( elem.isUInt() )
                {
                    auto strelem = __TO_STRING__( value.asInt() );
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

        /*
        template<typename T>
        inline bool GetArray( const std::string& key, std::vector<T>& des )
        {
            des.clear();
            if ( !this->isMember( key ) )
            {
                return false;
            }

            auto& value = this->operator[]( key );
            if ( !value.isArray() )
            {
                return false;
            }

            for ( auto i = 0; i < value.size(); ++i )
            {
                auto elem = value[i];
                if ( elem.isNull() )
                {
                    continue;
                }
                else if ( elem.isBool() )
                {
                    des.push_back( static_cast<T>( elem.asBool() ) );
                }
                else if ( elem.isInt() )
                {
                    des.push_back( static_cast<T>( elem.asInt() ) );
                }
                else if ( elem.isInt64() )
                {
                    des.push_back( static_cast<T>( elem.asInt64() ) );
                }

                else if ( elem.isUInt() )
                {
                    des.push_back( static_cast<T>( elem.asUInt() ) );
                }

                else if ( elem.isUInt64() )
                {
                    des.push_back( static_cast<T>( elem.asUInt64() ) );
                }

                else if ( elem.isIntegral() )
                {
                    des.push_back( static_cast<T>( elem.asLargestInt() ) );
                }

                else if ( elem.isDouble() )
                {
                    des.push_back( static_cast<T>( elem.asDouble() ) );
                }

            }

            return true;
        }*/

    };
}

#endif
