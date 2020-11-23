#ifndef __KF_CONFIG_H__
#define __KF_CONFIG_H__

#include "KFrame.h"

namespace KFrame
{
    namespace KFLoaderEnum
    {
        enum MyEnum
        {
            ClearTypeNone = 0,	// 不需要清空数据
            ClearTypeAll = 1,	// 清空所有数据
            ClearTypeFile = 2,	// 清空某个文件数据
        };
    }
    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    class KFConfig
    {
    public:
        KFConfig() = default;
        virtual ~KFConfig() = default;

        ////////////////////////////////////////////////////////////////////////////
        // 加载配置
        virtual bool LoadConfig( const std::string& filepath, uint32 loadmask )
        {
            return false;
        }

        // 加载完配置
        virtual void LoadComplete() {}

        // 所有配置加载完
        virtual void LoadAllComplete() {}
    public:
        // 默认配置文件名
        std::string _file_name;

        // 配置表主键字段名
        std::string _key_name;

        // 版本号
        std::string _version;

        // 是否加载成功
        bool _load_ok = false;

    public:
        //////////////////////////////////////////////////////////////////////
        // 添加版本号
        void AddFileVersion( const std::string& name, const std::string& version )
        {
            _file_version_list[ name ] = version;
            if ( _version.empty() )
            {
                _version = version;
            }
        }

        // 获得文件版本号
        const std::string& GetFileVersion( const std::string& name )
        {
            auto iter = _file_version_list.find( name );
            if ( iter == _file_version_list.end() )
            {
                return _invalid_string;
            }

            return iter->second;
        }
        //////////////////////////////////////////////////////////////////////
    private:
        // 版本号列表
        StringMap _file_version_list;
    };
    ///////////////////////////////////////////////////////////////
    template< class T >
    class KFConfigT : public KFConfig
    {
        typedef typename T::Type KeyType;
    public:
        // 加载配置
        bool LoadConfig( const std::string& filepath, uint32 cleartype )
        {
            KFXml kfxml( filepath );
            auto config = kfxml.RootNode();

            CheckClearSetting( filepath, cleartype );

            auto xmlnode = config.FindNode( __STRING__( node ).c_str() );
            while ( xmlnode.IsValid() )
            {
                auto kfsetting = CreateSetting( xmlnode );
                if ( kfsetting != nullptr )
                {
                    kfsetting->_file_path = filepath;
                    ReadSetting( xmlnode, kfsetting );
                }
                xmlnode.NextNode();
            }

            return true;
        }

        // 获取配置
        const T* FindSetting( const KeyType& id ) const
        {
            return _settings.Find( id );
        }

    protected:
        void CheckClearSetting( const std::string& filepath, uint32 cleartype )
        {
            switch ( cleartype )
            {
            case KFLoaderEnum::ClearTypeAll:
                ClearSetting();
                break;
            case KFLoaderEnum::ClearTypeFile:
                ClearFileSetting( filepath );
                break;
            default:
                break;
            }
        }

        virtual void ClearSetting()
        {
            _settings.Clear();
            _version.clear();
        }

        virtual void ClearFileSetting( const std::string& filepath )
        {
            std::list< KeyType > removes;
            for ( auto& iter : _settings._objects )
            {
                auto kfsetting = iter.second;
                if ( kfsetting->_file_path == filepath )
                {
                    removes.push_back( kfsetting->_id );
                }
            }

            for ( auto id : removes )
            {
                _settings.Remove( id );
            }
        }

        virtual T* CreateSetting( KFXmlNode& xmlnode )
        {
            auto service = xmlnode.ReadUInt32( __STRING__( service ).c_str(), true );
            auto channel = xmlnode.ReadUInt32( __STRING__( channel ).c_str(), true );
            auto ok = KFGlobal::Instance()->CheckChannelService( channel, service );
            if ( !ok )
            {
                return nullptr;
            }

            auto id = xmlnode.ReadT< KeyType >( _key_name.c_str(), true );
            auto kfsetting = _settings.Create( id );
            kfsetting->_id = id;
            kfsetting->_row = xmlnode.ReadUInt32( __STRING__( row ).c_str(), true );
            return kfsetting;
        }

        // 读取配置
        virtual void ReadSetting( KFXmlNode& xmlnode, T* kfsetting ) = 0;

    public:
        // 列表
        KFHashMap< KeyType, T > _settings;
    };

    ///////////////////////////////////////////////////////////////
}

#endif