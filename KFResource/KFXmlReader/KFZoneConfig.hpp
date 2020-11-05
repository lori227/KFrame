#ifndef __KF_ZONE_CONFIG_H__
#define __KF_ZONE_CONFIG_H__

#include "KFConfig.h"

namespace KFrame
{
    // 分区设置
    class KFZoneSetting : public KFIntSetting
    {
    public:
        // 游戏服名字
        std::string _name;

        // 登录指定id
        uint32 _login_id = 0u;

        // 数据指定id
        uint32 _data_id = 0u;

        // 小区标识
        std::string _flag;

        // 是否推荐
        uint32 _recommend = 0u;
    };

    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    class KFZoneConfig : public KFConfig, public KFInstance< KFZoneConfig >
    {
    public:
        KFZoneConfig()
        {
            _file_name = "zone";
        }

        // 加载配置
        bool LoadConfig( const std::string& filename, const std::string& filepath, uint32 loadmask )
        {
            _zone_list.Clear();
            ////////////////////////////////////////////////////////////////////////////////

            KFXml kfxml( filepath );
            auto config = kfxml.RootNode();

            auto defaultnode = config.FindNode( "Default" );
            _zone_template._name = defaultnode.GetString( "Name" );
            _zone_template._login_id = defaultnode.GetUInt32( "LoginId" );
            _zone_template._data_id = defaultnode.GetUInt32( "DataId" );
            _zone_template._flag = defaultnode.GetString( "Flag" );
            _zone_template._recommend = defaultnode.GetUInt32( "Recommend" );

            // 本小区属性
            _zone = _zone_template;
            /////////////////////////////////////////////////////////////////////////////////////
            auto zones = config.FindNode( "Zones" );
            _is_open_recommend = zones.GetBoolen( "OpenRecommend", true );

            auto xmlnode = zones.FindNode( "Zone" );
            while ( xmlnode.IsValid() )
            {
                // 小区列表
                auto id = xmlnode.GetUInt32( "Id" );
                auto zone = _zone_list.Create( id );
                zone->_id = id;
                zone->_name = xmlnode.GetString( "Name" );;
                zone->_login_id = xmlnode.GetUInt32( "LoginId" );
                zone->_data_id = xmlnode.GetUInt32( "DataId" );
                zone->_flag = xmlnode.GetString( "Flag" );
                zone->_recommend = xmlnode.GetUInt32( "Recommend" );

                // 本小区
                if ( KFGlobal::Instance()->_app_id->GetZoneId() == id )
                {
                    _zone = *zone;
                }

                xmlnode.NextNode();
            }
            /////////////////////////////////////////////////////////////////////////////////////
            /////////////////////////////////////////////////////////////////////////////////////
            SetZoneData( &_zone, KFGlobal::Instance()->_app_id->GetZoneId() );
            /////////////////////////////////////////////////////////////////////////////////////
            /////////////////////////////////////////////////////////////////////////////////////
            return true;
        }

        // 查找小区信息
        const KFZoneSetting* ZoneSetting()
        {
            return &_zone;
        }

        const KFZoneSetting* FindSetting( uint32 zoneid )
        {
            auto zone = _zone_list.Create( zoneid );
            if ( zone->_id == _invalid_int )
            {
                *zone = _zone_template;
                SetZoneData( zone, zoneid );
            }

            return zone;
        }

        // 是否开启推荐
        bool IsOpenRecommend() const
        {
            return _is_open_recommend;
        }

    protected:
        // 设置小区信息
        void SetZoneData( KFZoneSetting* kfsetting, uint32 zoneid )
        {
            // id
            kfsetting->_id = zoneid;

            // 名字
            kfsetting->_name = __FORMAT__( kfsetting->_name, kfsetting->_id );

            // 登录小区id
            if ( kfsetting->_login_id == _invalid_int )
            {
                kfsetting->_login_id = ( uint32 )kfsetting->_id;
            }

            // 数据小区id
            if ( kfsetting->_data_id == _invalid_int )
            {
                kfsetting->_data_id = ( uint32 )kfsetting->_id;
            }
        }

    private:
        // 本分区属性
        KFZoneSetting _zone;

        // 小区模板
        KFZoneSetting _zone_template;

        // 小区列表
        KFHashMap< uint32, uint32, KFZoneSetting > _zone_list;

        // 推荐的小区
        bool _is_open_recommend = false;
    };
}

#endif