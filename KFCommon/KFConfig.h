#ifndef __KF_CONFIG_H__
#define __KF_CONFIG_H__

#include "KFMap.h"
#include "KFUtility/KFXml.h"

namespace KFrame
{
    static std::string _confit_path = "./config/";
    ///////////////////////////////////////////////////////////////
    class KFSetting
    {
    public:
        virtual ~KFSetting() = default;
    };

    ///////////////////////////////////////////////////////////////
    class KFIntSetting : public KFSetting
    {
    public:
        // 类型
        typedef uint64 Type;
        typedef uint64 ParamType;

    public:
        uint64 _id = 0u;
    };
    ///////////////////////////////////////////////////////////////
    class KFStrSetting : public KFSetting
    {
    public:
        // 类型
        typedef std::string Type;
        typedef const std::string& ParamType;

    public:
        std::string _id;
    };
    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    class KFConfig
    {
    public:
        KFConfig( const std::string& file, bool isclear = true )
            : _file( file ), _is_clear( isclear )
        {
        }
        virtual ~KFConfig() = default;

        // 加载配置
        virtual void LoadConfig( const std::string& file ) {};

        // 加载完配置
        virtual void LoadComplete( const std::string& file ) {}

        // 是否是配置文件
        virtual bool IsFile( const std::string& configfile, const std::string& file )
        {
            return configfile.find( file ) != std::string::npos;
        }
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
        void LoadConfig( const std::string& file )
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
        }

        // 获取配置
        const T* FindSetting( uint64 id )
        {
            return _settings.Find( id );
        }

    protected:
        virtual void ClearSetting()
        {
            if ( _is_clear )
            {
                _settings.Clear();
            }
        }

        virtual T* CreateSetting( KFNode& xmlnode )
        {
            auto id = xmlnode.GetUInt64( "Id" );
            auto kfsetting = _settings.Create( id );
            kfsetting->_id = id;

            return kfsetting;
        }

        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, T* kfsetting ) = 0;
    public:
        // 列表
        KFHashMap< uint64, uint64, T > _settings;
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
        void LoadConfig( const std::string& file )
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
        }

        // 获取配置
        const T* FindSetting( const std::string& id )
        {
            return _settings.Find( id );
        }

    protected:
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