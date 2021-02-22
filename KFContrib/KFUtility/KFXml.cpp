#include "KFXml.h"
#include "xml/rapidxml.hpp"
#include "xml/rapidxml_print.hpp"
#include "xml/rapidxml_utils.hpp"

namespace KFrame
{
    KFXml::KFXml( const std::string& file )
    {
        _file = file;
        _data = new rapidxml::file<>();
        _document = new rapidxml::xml_document<>();

        Parse();
    }

    KFXml::~KFXml()
    {
        delete reinterpret_cast<rapidxml::file<>*>( _data );
        delete reinterpret_cast<rapidxml::xml_document<>*>( _document );
    }

    const char* KFXml::GetFileName() const
    {
        return _file.c_str();
    }

    const char* KFXml::Data()
    {
        auto xml_file = reinterpret_cast< rapidxml::file<>* >( _data );
        return xml_file->data();
    }

    uint32 KFXml::Size()
    {
        auto xml_file = reinterpret_cast< rapidxml::file<>* >( _data );
        return static_cast<uint32>( xml_file->size() );
    }

    void KFXml::Parse()
    {
        auto xml_file = reinterpret_cast<rapidxml::file<>*>( _data );
        auto xml_document = reinterpret_cast<rapidxml::xml_document<>*>( _document );

        xml_file->from( _file.c_str() );
        xml_document->parse<0>( xml_file->data() );
    }

    KFXmlNode KFXml::RootNode()
    {
        auto xml_document = reinterpret_cast<rapidxml::xml_document<>*>( _document );

        KFXmlNode xml_node( this );
        xml_node._node = xml_document->first_node();
        return xml_node;
    }

    KFXmlNode KFXml::FindNode( const char* key )
    {
        auto xml_document = reinterpret_cast<rapidxml::xml_document<>*>( _document );

        KFXmlNode xml_node( this );
        xml_node._node = xml_document->first_node( key );
        return xml_node;
    }
}
