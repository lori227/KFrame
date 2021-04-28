#include "KFXmlNode.h"
#include "KFrame.h"
#include "xml/rapidxml.hpp"
#include "xml/rapidxml_print.hpp"
#include "xml/rapidxml_utils.hpp"
#include "KFStaticAnalysis.hpp"

namespace KFrame
{
    KFXmlNode::KFXmlNode( KFXml* xml )
    {
        _xml = xml;
        _node = nullptr;
    }

    KFXmlNode::~KFXmlNode()
    {
        _node = nullptr;
    }

    bool KFXmlNode::IsValid()
    {
        return _node != nullptr;
    }

    void KFXmlNode::NextNode( const char* key /* = nullptr */ )
    {
        auto xml_node = reinterpret_cast<rapidxml::xml_node<>*>( _node );
        _node = xml_node->next_sibling( key );
    }

    KFXmlNode KFXmlNode::FindNode( const char* key )
    {
        auto xml_node = reinterpret_cast<rapidxml::xml_node<>*>( _node );

        KFXmlNode node( _xml );
        node._node = xml_node->first_node( key );
        return node;
    }

    bool KFXmlNode::ReadBool( const char* key, bool optional /* = false */, bool default_value /* = false */ )
    {
        auto xml_node = reinterpret_cast<rapidxml::xml_node<>*>( _node );

        auto attribute = xml_node->first_attribute( key );
        if ( attribute == nullptr )
        {
            if ( !optional )
            {
                __LOG_ERROR__( "[{}] can't find node = [{}]", _xml->GetFileName(), key );
            }
            return default_value;
        }

        return __TO_UINT32__( attribute->value() ) == 1;
    }

    int32 KFXmlNode::ReadInt32( const char* key, bool optional /* = false */, int32 default_value /* = 0 */ )
    {
        auto xml_node = reinterpret_cast< rapidxml::xml_node<>* >( _node );

        auto attribute = xml_node->first_attribute( key );
        if ( attribute == nullptr )
        {
            if ( !optional )
            {
                __LOG_ERROR__( "[{}] can't find node = [{}]", _xml->GetFileName(), key );
            }
            return default_value;
        }

        return __TO_INT32__( attribute->value() );
    }


    uint32 KFXmlNode::ReadUInt32( const char* key, bool optional /* = false */, uint32 default_value /* = 0 */ )
    {
        auto xml_node = reinterpret_cast<rapidxml::xml_node<>*>( _node );

        auto attribute = xml_node->first_attribute( key );
        if ( attribute == nullptr )
        {
            if ( !optional )
            {
                __LOG_ERROR__( "[{}] can't find node = [{}]", _xml->GetFileName(), key );
            }
            return default_value;
        }

        return __TO_UINT32__( attribute->value() );
    }

    uint64 KFXmlNode::ReadUInt64( const char* key, bool optional /* = false */, uint64 default_value /* = 0 */ )
    {
        auto xml_node = reinterpret_cast< rapidxml::xml_node<>* >( _node );

        auto attribute = xml_node->first_attribute( key );
        if ( attribute == nullptr )
        {
            if ( !optional )
            {
                __LOG_ERROR__( "[{}] can't find node = [{}]", _xml->GetFileName(), key );
            }
            return default_value;
        }

        return __TO_UINT64__( attribute->value() );
    }

    std::string KFXmlNode::ReadString( const char* key, bool optional /* = false */, std::string default_value/* = _invalid_string */ )
    {
        auto xml_node = reinterpret_cast<rapidxml::xml_node<>*>( _node );

        auto attribute = xml_node->first_attribute( key );
        if ( attribute == nullptr )
        {
            if ( !optional )
            {
                __LOG_ERROR__( "[{}] can't find node = [{}]", _xml->GetFileName(), key );
            }

            return default_value;
        }

        return attribute->value();
    }

    double KFXmlNode::ReadDouble( const char* key, bool optional /* = false */, double default_value /* = 0.0f */ )
    {
        auto xml_node = reinterpret_cast< rapidxml::xml_node<>* >( _node );

        auto attribute = xml_node->first_attribute( key );
        if ( attribute == nullptr )
        {
            if ( !optional )
            {
                __LOG_ERROR__( "[{}] can't find node = [{}]", _xml->GetFileName(), key );
            }
            return default_value;
        }

        return atof( attribute->value() );
    }

    // 读取uint32 vector
    UInt32Vector& KFXmlNode::ReadUInt32Vector( const char* key, bool optional )
    {
        static UInt32Vector _result;
        _result.clear();

        auto data = ReadString( key, optional );
        KFUtility::SplitList( data, __SPLIT_STRING__, _result );
        return _result;
    }

    // 读取uint32 map
    UInt32Map& KFXmlNode::ReadUInt32Map( const char* key, bool optional )
    {
        static UInt32Map _result;
        _result.clear();

        auto str_line = ReadString( key, optional );
        while ( !str_line.empty() )
        {
            auto data = KFUtility::SplitString( str_line, __SPLIT_STRING__ );
            if ( !data.empty() )
            {
                auto key = KFUtility::SplitValue<uint32>( data, __DOMAIN_STRING__ );
                auto value = KFUtility::SplitValue<uint32>( data, __DOMAIN_STRING__ );
                _result[ key ] = value;
            }
        }

        return _result;
    }

    // 读取uint32 set
    UInt32Set& KFXmlNode::ReadUInt32Set( const char* key, bool optional )
    {
        static UInt32Set _result;
        _result.clear();

        auto data = ReadString( key, optional );
        KFUtility::SplitSet( data, __SPLIT_STRING__, _result );
        return _result;
    }

    // 读取string set
    StringSet& KFXmlNode::ReadStringSet( const char* key, bool optional )
    {
        static StringSet _result;
        _result.clear();

        auto data = ReadString( key, optional );
        KFUtility::SplitSet( data, __SPLIT_STRING__, _result );
        return _result;
    }

    // 读取string list
    StringList& KFXmlNode::ReadStringList( const char* key, bool optional )
    {
        static StringList _result;
        _result.clear();

        auto data = ReadString( key, optional );
        KFUtility::SplitList( data, __SPLIT_STRING__, _result );
        return _result;
    }

    // 读取string vector
    StringVector& KFXmlNode::ReadStringVector( const char* key, bool optional )
    {
        static StringVector _result;
        _result.clear();

        auto data = ReadString( key, optional );
        KFUtility::SplitList( data, __SPLIT_STRING__, _result );
        return _result;
    }

    // 读取<string,uint64>
    StringUInt64& KFXmlNode::ReadStringUInt64( const char* key, bool optional )
    {
        static StringUInt64 _result;
        _result.clear();

        auto str_line = ReadString( key, optional );
        while ( !str_line.empty() )
        {
            auto data = KFUtility::SplitString( str_line, __SPLIT_STRING__ );
            if ( !data.empty() )
            {
                auto key = KFUtility::SplitString( data, __DOMAIN_STRING__ );
                auto value = KFUtility::SplitValue<uint32>( data, __DOMAIN_STRING__ );
                _result[ key ] = value;
            }
        }

        return _result;
    }

    // 读取<uint64,string>
    UInt64String& KFXmlNode::ReadUInt64String( const char* key, bool optional )
    {
        static UInt64String _result;
        _result.clear();

        auto str_line = ReadString( key, optional );
        while ( !str_line.empty() )
        {
            auto data = KFUtility::SplitString( str_line, __SPLIT_STRING__ );
            if ( !data.empty() )
            {
                auto key = KFUtility::SplitValue<uint32>( data, __DOMAIN_STRING__ );
                auto value = KFUtility::SplitString( data, __DOMAIN_STRING__ );
                _result[ key ] = value;
            }
        }

        return _result;
    }

    void KFXmlNode::GetKeyList( StringList& out_list )
    {
        out_list.clear();

        auto xml_node = reinterpret_cast< rapidxml::xml_node<>* >( _node );
        auto attribute = xml_node->first_attribute();
        while ( attribute != nullptr )
        {
            out_list.push_back( attribute->name() );
            attribute = attribute->next_attribute();
        }
    }

    bool KFXmlNode::HaveChild( const char* key )
    {
        auto xml_node = reinterpret_cast< rapidxml::xml_node<>* >( _node );
        auto attribute = xml_node->first_attribute( key );
        return attribute != nullptr;
    }

    DynamicConditionGroupPtr KFXmlNode::ReadDynamicConditionGroup( const char* key, bool optional /* = false */ )
    {
        auto condition_group = __MAKE_SHARED__( KFDynamicConditionGroup );
        condition_group->_type = KFEnum::Or;

        auto str_line = ReadString( key, optional );
        if ( !str_line.empty() )
        {
            if ( str_line.find( __OR_STRING__ ) != std::string::npos )
            {
                condition_group->_type = KFEnum::Or;
                KFUtility::SplitList( str_line, __OR_STRING__, condition_group->_id_list );
            }
            else if ( str_line.find( __AND_STRING__ ) != std::string::npos )
            {
                condition_group->_type = KFEnum::And;
                KFUtility::SplitList( str_line, __AND_STRING__, condition_group->_id_list );
            }
            else
            {
                auto condition_id = __TO_UINT32__( str_line );
                if ( condition_id != 0u )
                {
                    condition_group->_id_list.push_back( condition_id );
                }
            }
        }

        return condition_group;
    }

    ExecuteDataPtr KFXmlNode::ReadExecuteData( const char* key, bool optional /* = false */ )
    {
        auto execute_data = __MAKE_SHARED__( KFExecuteData );
        execute_data->_name = ReadString( key, optional );

        for (auto i = 0; i < 4; ++i) {
            auto param_key = __FORMAT__( "executeparam{}", i + 1 );
            auto param = execute_data->_param_list.AddParam();
            param->_str_value = ReadString( param_key.c_str(), true );
            param->_int_value = ReadUInt32( param_key.c_str(), true );
            param->_map_value = ReadUInt32Map( param_key.c_str() , true);
            param->_vector_value = ReadUInt32Vector( param_key.c_str() , true);
        }

        return execute_data;
    }

    StaticConditionsPtr KFXmlNode::ReadStaticConditions( const char* key, bool optional /* = false */ )
    {
        auto condition = ReadString( key, optional );
        return KFStaticConditionAnalysis::Parse( condition );
    }

    StaticConditionListPtr KFXmlNode::ReadStaticConditionList( const char* key, bool optional /* = false */ )
    {
        auto condition_list = __MAKE_SHARED__( KFStaticConditionList );
        condition_list->_check_type = KFEnum::And;

        {
            auto condition = ReadString( key, optional );
            auto static_conditions = KFStaticConditionAnalysis::Parse( condition );
            condition_list->_conditions_list.push_back( static_conditions );
        }

        auto index = 1u;
        while ( true )
        {
            auto condition_key = __FORMAT__( "{}{}", key, index++ );
            auto ok = HaveChild( condition_key.c_str() );
            if ( !ok )
            {
                break;
            }

            auto condition = ReadString( condition_key.c_str() );
            auto static_conditions = KFStaticConditionAnalysis::Parse( condition );
            condition_list->_conditions_list.push_back( static_conditions );
        }

        return condition_list;
    }

    uint64 KFXmlNode::ReadDate( const char* key, bool optional /* = false */ )
    {
        auto data = ReadString( key, optional );
        return KFDate::FromString( data );
    }

    uint32 KFXmlNode::ReadOperateType( const char* key, bool optional /* = false */ )
    {
        auto data = ReadString( key, optional );
        if ( data.empty() )
        {
            return KFEnum::Null;
        }

        auto type = KFStaticConditionAnalysis::GetOperatorType( data.at( 0 ) );
        if ( type == KFEnum::Null )
        {
            type = __TO_UINT32__( data );
        }

        return type;
    }

    uint32 KFXmlNode::ReadCheckType( const char* key, bool optional /* = false */ )
    {
        auto data = ReadString( key, optional );
        auto type = KFStaticConditionAnalysis::GetCheckType( data );
        if ( type == KFEnum::Null )
        {
            type = __TO_UINT32__( data );
        }

        return type;
    }

    KFRange<uint32> KFXmlNode::ReadRange( const char* key, bool optional /* = false */ )
    {
        KFRange<uint32> range;
        auto data = ReadString( key, optional );

        range._min_value = KFUtility::SplitValue<uint32>( data, __RANGE_STRING__ );
        range._max_value = KFUtility::SplitValue<uint32>( data, __RANGE_STRING__ );
        if ( range._max_value < range._min_value )
        {
            range._max_value = range._min_value;
        }

        return range;
    }

    std::string KFXmlNode::ReadChannelString( const char* key, bool optional /* = false */ )
    {
        auto data = ReadString( key, optional );
        auto result = __FORMAT__( data, KFGlobal::Instance()->_channel, KFGlobal::Instance()->_service );
        return result;
    }
}
