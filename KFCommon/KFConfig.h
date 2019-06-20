#ifndef __KF_CONFIG_H__
#define __KF_CONFIG_H__

#include "KFMap.h"
#include "KFUtility/KFXml.h"

namespace KFrame
{
    ///////////////////////////////////////////////////////////////
    class KFSetting
    {
    public:
        virtual ~KFSetting() = default;
    };

    template< class T >
    class KFSettingT : public KFSetting
    {
    public:
        // 类型
        typedef T Type;

        // 配置id
        T _id;
    };
    ///////////////////////////////////////////////////////////////
    class KFIntSetting : public KFSettingT< uint32 >
    {
    public:
        typedef uint32 ParamType;
    };
    ///////////////////////////////////////////////////////////////
    class KFStrSetting : public KFSettingT< std::string >
    {
    public:
        typedef const std::string& ParamType;
    };
    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    class KFConfig
    {
    public:
        KFConfig( const std::string& file, bool isclear )
            : _file( file ), _is_clear( isclear )
        {
        }
        virtual ~KFConfig() = default;

        // 加载配置
        virtual bool LoadConfig( const std::string& file ) = 0;

        // 加载完配置
        virtual void LoadComplete() {}
    public:
        // 配置文件
        std::string _file;

        // 版本号
        std::string _version;
    protected:
        // 是否是要清除原来的数据
        bool _is_clear = true;
    };
    ///////////////////////////////////////////////////////////////
    template< class T >
    class KFIntConfigT : public KFConfig
    {
    public:
        KFIntConfigT( const std::string& file, bool isclear )
            : KFConfig( file, isclear )
        {
        }

        // 加载配置
        bool LoadConfig( const std::string& file )
        {
            ClearSetting();

            KFXml kfxml( file );
            auto config = kfxml.RootNode();
            _version = config.GetString( "version" );

            auto xmlnode = config.FindNode( "item" );
            while ( xmlnode.IsValid() )
            {
                auto kfsetting = CreateSetting( xmlnode );
                if ( kfsetting != nullptr )
                {
                    ReadSetting( xmlnode, kfsetting );
                }
                xmlnode.NextNode();
            }

            return true;
        }

        // 获取配置
        const T* FindSetting( uint32 id )
        {
            return _settings.Find( id );
        }


    protected:
        // 清空配置
        virtual void ClearSetting()
        {
            if ( _is_clear )
            {
                _settings.Clear();
            }
        }

        virtual T* CreateSetting( KFNode& xmlnode )
        {
            auto id = xmlnode.GetUInt32( "Id" );
            auto kfsetting = _settings.Create( id );
            kfsetting->_id = id;

            return kfsetting;
        }

        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, T* kfsetting ) = 0;

    public:
        // 列表
        KFHashMap< uint32, uint32, T > _settings;
    };

    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    template< class T >
    class KFStrConfigT : public KFConfig
    {
    public:
        KFStrConfigT( const std::string& file, bool isclear )
            : KFConfig( file, isclear )
        {
        }

        // 加载配置
        bool LoadConfig( const std::string& file )
        {
            ClearSetting();

            KFXml kfxml( file );
            auto config = kfxml.RootNode();
            _version = config.GetString( "version" );

            auto xmlnode = config.FindNode( "item" );
            while ( xmlnode.IsValid() )
            {
                auto kfsetting = CreateSetting( xmlnode );
                if ( kfsetting != nullptr )
                {
                    ReadSetting( xmlnode, kfsetting );
                }
                xmlnode.NextNode();
            }

            return true;
        }

        // 获取配置
        const T* FindSetting( const std::string& id )
        {
            return _settings.Find( id );
        }

    protected:
        // 清空配置
        virtual void ClearSetting()
        {
            if ( _is_clear )
            {
                _settings.Clear();
            }
        }

        virtual T* CreateSetting( KFNode& xmlnode )
        {
            auto id = xmlnode.GetString( "Id" );
            auto kfsetting = _settings.Create( id );
            kfsetting->_id = id;

            return kfsetting;
        }

        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, T* kfsetting ) = 0;
    public:
        // 列表
        KFHashMap< std::string, const std::string&, T > _settings;
    };
}

#endif