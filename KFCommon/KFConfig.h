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
        KFConfig()
        {
            _key_name = "id";
        }

        virtual ~KFConfig() = default;
        ////////////////////////////////////////////////////////////////////////////
        // 加载配置
        virtual bool LoadConfig( const std::string& file_path, uint32 clear_type )
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
    template<class T>
    class KFConfigT : public KFConfig
    {
        typedef typename T::Type KeyType;
        typedef std::shared_ptr<T> ObjectPtr;
    public:
        // 加载配置
        bool LoadConfig( const std::string& file_path, uint32 clear_type )
        {
            KFXml xml_reader( file_path );
            auto root = xml_reader.FindNode( __STRING__( root ).c_str() );
            if ( !root.IsValid() )
            {
                return false;
            }

            CheckClearSetting( file_path, clear_type );

            auto xml_node = root.FindNode( __STRING__( node ).c_str() );
            while ( xml_node.IsValid() )
            {
                auto setting = CreateSetting( xml_node );
                if ( setting != nullptr )
                {
                    setting->_file_path = file_path;
                    ReadSetting( xml_node, setting );
                }
                xml_node.NextNode();
            }

            return true;
        }

        // 获取配置
        inline const ObjectPtr FindSetting( const KeyType& id ) const
        {
            return _setting_list.Find( id );
        }

    protected:
        void CheckClearSetting( const std::string& file_path, uint32 clear_type )
        {
            switch ( clear_type )
            {
            case KFLoaderEnum::ClearTypeAll:
                ClearSetting();
                break;
            case KFLoaderEnum::ClearTypeFile:
                ClearFileSetting( file_path );
                break;
            default:
                break;
            }
        }

        virtual void ClearSetting()
        {
            _version.clear();
            _setting_list.Clear();
        }

        virtual void ClearFileSetting( const std::string& file_path )
        {
            for ( auto iter = _setting_list._objects.begin(); iter != _setting_list._objects.end(); )
            {
                if ( iter->second->_file_path == file_path )
                {
                    iter = _setting_list._objects.erase( iter );
                }
                else
                {
                    ++iter;
                }
            }
        }

        virtual ObjectPtr CreateSetting( KFXmlNode& xml_node )
        {
            auto service = xml_node.ReadUInt32( __STRING__( service ).c_str(), true );
            auto channel = xml_node.ReadUInt32( __STRING__( channel ).c_str(), true );
            auto ok = KFGlobal::Instance()->CheckChannelService( channel, service );
            if ( !ok )
            {
                return nullptr;
            }

            auto id = xml_node.ReadT<KeyType>( _key_name.c_str(), true );
            auto setting = _setting_list.Create( id );
            setting->_id = id;
            setting->_row = xml_node.ReadUInt32( __STRING__( row ).c_str(), true );
            return setting;
        }

        // 读取配置
        virtual void ReadSetting( KFXmlNode& xml_node, ObjectPtr setting ) = 0;

    public:
        // 列表
        KFHashMap<KeyType, T> _setting_list;
    };

    ///////////////////////////////////////////////////////////////
}

#endif