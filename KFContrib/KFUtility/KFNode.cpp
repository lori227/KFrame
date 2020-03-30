#include "KFNode.h"
#include "KFDefine.h"
#include "KFXml.h"
#include "KFrame.h"
#include "xml/rapidxml.hpp"
#include "xml/rapidxml_print.hpp"
#include "xml/rapidxml_utils.hpp"
#include "KFUtility/KFUtility.h"
#include "KFUtility/KFConvert.h"

namespace KFrame
{
    KFNode::KFNode( KFXml* kfxml )
    {
        _kf_xml = kfxml;
        _node = nullptr;
    }

    KFNode::~KFNode()
    {
        _node = nullptr;
    }

    bool KFNode::IsValid()
    {
        return _node != nullptr;
    }

    void KFNode::NextNode( const char* key /* = nullptr */ )
    {
        auto xmlnode = reinterpret_cast<rapidxml::xml_node<>*>( _node );
        _node = xmlnode->next_sibling( key );
    }

    KFNode KFNode::FindNode( const char* key )
    {
        auto xmlnode = reinterpret_cast<rapidxml::xml_node<>*>( _node );

        KFNode kfnode( _kf_xml );
        kfnode._node = xmlnode->first_node( key );
        return kfnode;
    }

    bool KFNode::GetBoolen( const char* key, bool optional /* = false */, bool defaultvale /* = false */ )
    {
        auto xmlnode = reinterpret_cast<rapidxml::xml_node<>*>( _node );

        auto attribute = xmlnode->first_attribute( key );
        if ( attribute == nullptr )
        {
            if ( !optional )
            {
                __LOG_ERROR__( "[{}] can't find node = [{}]", _kf_xml->GetFileName(), key );
            }
            return defaultvale;
        }

        return __TO_UINT32__( attribute->value() ) == 1;
    }

    int32 KFNode::GetInt32( const char* key, bool optional /* = false */, int32 defaultvalue /* = 0 */ )
    {
        auto xmlnode = reinterpret_cast< rapidxml::xml_node<>* >( _node );

        auto attribute = xmlnode->first_attribute( key );
        if ( attribute == nullptr )
        {
            if ( !optional )
            {
                __LOG_ERROR__( "[{}] can't find node = [{}]", _kf_xml->GetFileName(), key );
            }
            return defaultvalue;
        }

        return __TO_INT32__( attribute->value() );
    }


    uint32 KFNode::GetUInt32( const char* key, bool optional /* = false */, uint32 defaultvalue /* = 0 */ )
    {
        auto xmlnode = reinterpret_cast<rapidxml::xml_node<>*>( _node );

        auto attribute = xmlnode->first_attribute( key );
        if ( attribute == nullptr )
        {
            if ( !optional )
            {
                __LOG_ERROR__( "[{}] can't find node = [{}]", _kf_xml->GetFileName(), key );
            }
            return defaultvalue;
        }

        return __TO_UINT32__( attribute->value() );
    }

    uint64 KFNode::GetUInt64( const char* key, bool optional /* = false */, uint64 defaultvalue /* = 0 */ )
    {
        auto xmlnode = reinterpret_cast< rapidxml::xml_node<>* >( _node );

        auto attribute = xmlnode->first_attribute( key );
        if ( attribute == nullptr )
        {
            if ( !optional )
            {
                __LOG_ERROR__( "[{}] can't find node = [{}]", _kf_xml->GetFileName(), key );
            }
            return defaultvalue;
        }

        return __TO_UINT64__( attribute->value() );
    }

    std::string KFNode::GetString( const char* key, bool optional /* = false */, std::string defaultvalue/* = _invalid_string */ )
    {
        auto xmlnode = reinterpret_cast<rapidxml::xml_node<>*>( _node );

        auto attribute = xmlnode->first_attribute( key );
        if ( attribute == nullptr )
        {
            if ( !optional )
            {
                __LOG_ERROR__( "[{}] can't find node = [{}]", _kf_xml->GetFileName(), key );
            }

            return defaultvalue;
        }

        return attribute->value();
    }

    double KFNode::GetDouble( const char* key, bool optional /* = false */, double defaultvalue /* = 0.0f */ )
    {
        auto xmlnode = reinterpret_cast< rapidxml::xml_node<>* >( _node );

        auto attribute = xmlnode->first_attribute( key );
        if ( attribute == nullptr )
        {
            if ( !optional )
            {
                __LOG_ERROR__( "[{}] can't find node = [{}]", _kf_xml->GetFileName(), key );
            }
            return defaultvalue;
        }

        return atof( attribute->value() );
    }

    void KFNode::GetKeyList( StringList& outlist )
    {
        outlist.clear();

        auto xmlnode = reinterpret_cast< rapidxml::xml_node<>* >( _node );
        auto attribute = xmlnode->first_attribute();
        while ( attribute != nullptr )
        {
            outlist.push_back( attribute->name() );
            attribute = attribute->next_attribute();
        }
    }

    bool KFNode::HaveChild( const char* key )
    {
        auto xmlnode = reinterpret_cast< rapidxml::xml_node<>* >( _node );
        auto attribute = xmlnode->first_attribute( key );
        return attribute != nullptr;
    }
}
