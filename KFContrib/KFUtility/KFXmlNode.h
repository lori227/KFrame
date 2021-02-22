#ifndef __KF_XML_NODE_H__
#define __KF_XML_NODE_H__

#include "KFExecuteData.h"
#include "KFStaticCondition.h"
#include "KFDynamicCondition.h"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    class KFXml;
    class KFXmlNode
    {
    public:
        KFXmlNode( KFXml* xml );
        ~KFXmlNode();

        // 是否有效
        bool IsValid();

        // 兄弟节点
        void NextNode( const char* key = nullptr );

        // 查找子节点
        KFXmlNode FindNode( const char* key );
        /////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////
        template<class T>
        T ReadT( const char* key, bool optional = false )
        {
            return ReadUInt32( key, optional );
        }

        // 读取bool
        bool ReadBool( const char* key, bool optional = false, bool default_value = false );

        // 读取int32
        int32 ReadInt32( const char* key, bool optional = false, int32 default_value = 0 );

        // 读取uint32
        uint32 ReadUInt32( const char* key, bool optional = false, uint32 default_value = 0 );

        // 读取uint64
        uint64 ReadUInt64( const char* key, bool optional = false, uint64 default_value = 0 );

        // 读取string
        std::string ReadString( const char* key, bool optional = false, std::string default_value = "" );

        // 读取double
        double ReadDouble( const char* key, bool optional = false, double default_value = 0.0f );

        // 读取uint32 vector
        UInt32Vector& ReadUInt32Vector( const char* key, bool optional = false );

        // 读取uint32 map
        UInt32Map& ReadUInt32Map( const char* key, bool optional = false );

        // 读取uint32 set
        UInt32Set& ReadUInt32Set( const char* key, bool optional = false );

        // 读取string set
        StringSet& ReadStringSet( const char* key, bool optional = false );

        // 读取string list
        StringList& ReadStringList( const char* key, bool optional = false );

        // 读取string vector
        StringVector& ReadStringVector( const char* key, bool optional = false );

        // 读取<string,uint64>
        StringUInt64& ReadStringUInt64( const char* key, bool optional = false );

        // 读取<uint64,string>
        UInt64String& ReadUInt64String( const char* key, bool optional = false );

        // 读取时间
        uint64 ReadDate( const char* key, bool optional = false );

        // 读取操作类型
        uint32 ReadOperateType( const char* key, bool optional = false );

        // 读取判断类型
        uint32 ReadCheckType( const char* key, bool optional = false );

        // 读取返回
        KFRange<uint32> ReadRange( const char* key, bool optional = false );

        // 读取渠道格式化字串({}_{}_xxxx)
        std::string ReadChannelString( const char* key, bool optional = false );
        /////////////////////////////////////////////////////////////////////////////////////
        // 读取静态条件配置列表( 如:money=100 )
        StaticConditionsPtr ReadStaticConditions( const char* key, bool optional = false );

        // 读取静态条件配置列表( 如:money=100 )
        StaticConditionListPtr ReadStaticConditionList( const char* key, bool optional = false );

        // 读取条件id列表
        DynamicConditionGroupPtr ReadDynamicConditionGroup( const char* key, bool optional = false );

        // 读取执行列表
        ExecuteDataPtr ReadExecuteData( const char* key, bool optional = false );
        /////////////////////////////////////////////////////////////////////////////////////
    protected:
        void GetKeyList( StringList& out_list );

        // 是否有子属性
        bool HaveChild( const char* key );
    private:
        friend class KFXml;
        KFXml* _xml;
        // 节点
        void* _node;
    };

    template<>
    inline bool KFXmlNode::ReadT( const char* key, bool optional /* = false */ )
    {
        return ReadBool( key, optional );
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
