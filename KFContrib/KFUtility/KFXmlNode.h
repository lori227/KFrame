#ifndef __KF_XML_NODE_H__
#define __KF_XML_NODE_H__

#include "KFInclude.h"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    class KFXml;
    class KFConditions;
    class KFExecuteData;
    class KFConditionGroup;
    class KFXmlNode
    {
    public:
        KFXmlNode( KFXml* kfxml );
        ~KFXmlNode();

        // 是否有效
        bool IsValid();

        // 兄弟节点
        void NextNode( const char* key = nullptr );

        // 查找子节点
        KFXmlNode FindNode( const char* key );
        /////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////
        template< class T >
        T ReadT( const char* key, bool optional = false )
        {
            return ReadUInt32( key, optional );
        }

        // 读取bool
        bool ReadBoolen( const char* key, bool optional = false, bool defaultvale = false );

        // 读取int32
        int32 ReadInt32( const char* key, bool optional = false, int32 defaultvalue = 0 );

        // 读取uint32
        uint32 ReadUInt32( const char* key, bool optional = false, uint32 defaultvalue = 0 );

        // 读取uint64
        uint64 ReadUInt64( const char* key, bool optional = false, uint64 defaultvalue = 0 );

        // 读取string
        std::string ReadString( const char* key, bool optional = false, std::string defaultvalue = "" );

        // 读取double
        double ReadDouble( const char* key, bool optional = false, double defaultvalue = 0.0f );

        // 读取uint32 vector
        UInt32Vector& ReadUInt32Vector( const char* key, bool optional = false );

        // 读取uint32 map
        UInt32Map& ReadUInt32Map( const char* key, bool optional = false );

        // 读取uint32 set
        UInt32Set& ReadUInt32Set( const char* key, bool optional = false );

        // 读取string set
        StringSet& ReadStringSet( const char* key, bool optional = false );

        // 读取<string,uint64>
        StringUInt64& ReadStringUInt64( const char* key, bool optional = false );

        // 读取<uint64,string>
        UInt64String& ReadUInt64String( const char* key, bool optional = false );

        // 读取时间
        uint64 ReadDate( const char* key, bool optional = false );
        /////////////////////////////////////////////////////////////////////////////////////
        // 读取静态条件配置列表( 如:money=100 )
        bool ReadStaticCondition( KFConditions& conditions, const char* key, bool optional = false );

        // 读取条件id列表
        const KFConditionGroup& ReadConditionGroup( const char* key, bool optional = false );

        // 读取执行列表
        void ReadExecuteData( KFExecuteData* executedata, const char* key, bool optional = false );
        /////////////////////////////////////////////////////////////////////////////////////
        void GetKeyList( StringList& outlist );

        // 是否有子属性
        bool HaveChild( const char* key );

    private:
        friend class KFXml;
        KFXml* _kf_xml;
        // 节点
        void* _node;
    };

    template<>
    inline bool KFXmlNode::ReadT( const char* key, bool optional /* = false */ )
    {
        return ReadBoolen( key, optional );
    }

    template<>
    inline int32 KFXmlNode::ReadT( const char* key, bool optional /* = false */ )
    {
        return ReadInt32( key, optional );
    }

    template<>
    inline uint32 KFXmlNode::ReadT( const char* key, bool optional /* = false */ )
    {
        return ReadUInt32( key, optional );
    }

    template<>
    inline uint64 KFXmlNode::ReadT( const char* key, bool optional /* = false */ )
    {
        return ReadUInt64( key, optional );
    }

    template<>
    inline std::string KFXmlNode::ReadT( const char* key, bool optional /* = false */ )
    {
        return ReadString( key, optional );
    }

    template<>
    inline double KFXmlNode::ReadT( const char* key, bool optional /* = false */ )
    {
        return ReadDouble( key, optional );
    }
}

#endif
