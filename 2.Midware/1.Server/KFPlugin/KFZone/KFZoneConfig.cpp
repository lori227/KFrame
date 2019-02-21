#include "KFZoneConfig.hpp"

namespace KFrame
{
    bool KFZoneConfig::LoadConfig()
    {
        _zone_list.Clear();
        ////////////////////////////////////////////////////////////////////////////////

        KFXml kfxml( _file );
        auto config = kfxml.RootNode();

        auto defaultnode = config.FindNode( "Default" );
        _zone_template._name = defaultnode.GetString( "Name" );
        _zone_template._logic_id = defaultnode.GetUInt32( "LogicId" );
        _zone_template._login_id = defaultnode.GetUInt32( "LoginId" );

        // 本小区属性
        _zone = _zone_template;
        /////////////////////////////////////////////////////////////////////////////////////
        auto zones = config.FindNode( "Zones" );
        auto xmlnode = zones.FindNode( "Zone" );
        while ( xmlnode.IsValid() )
        {
            auto id = xmlnode.GetUInt32( "Id" );
            auto name = xmlnode.GetString( "Name" );
            auto logicid = xmlnode.GetUInt32( "LogicId" );
            auto loginid = xmlnode.GetUInt32( "LoginId" );

            // 小区列表
            auto zone = _zone_list.Create( id );
            zone->_id = id;
            zone->_name = name;
            zone->_logic_id = logicid;
            zone->_login_id = loginid;

            // 本小区
            if ( KFGlobal::Instance()->_app_id->GetZoneId() == id )
            {
                _zone._id = id;
                _zone._name = xmlnode.GetString( "Name" );
                _zone._logic_id = xmlnode.GetUInt32( "LogicId" );
                _zone._login_id = xmlnode.GetUInt32( "LoginId" );
            }

            xmlnode.NextNode();
        }
        /////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////
        if ( _zone._id == _invalid_int )
        {
            SetZoneData( &_zone, KFGlobal::Instance()->_app_id->GetZoneId() );
        }
        /////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////

        return true;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFZoneConfig::SetZoneData( KFZone* zone, uint32 zoneid )
    {
        // id
        zone->_id = zoneid;

        // 名字
        zone->_name = __FORMAT__( zone->_name, zone->_id );

        // 逻辑id
        if ( zone->_logic_id == _invalid_int )
        {
            zone->_logic_id = zone->_id;
        }

        // 登录id
        if ( zone->_login_id == _invalid_int )
        {
            zone->_login_id = zone->_id;
        }
    }

    const KFZone* KFZoneConfig::FindZone( uint32 zoneid )
    {
        auto zone = _zone_list.Create( zoneid );
        if ( zone->_id == _invalid_int )
        {
            *zone = _zone_template;
            SetZoneData( zone, zoneid );
        }

        return zone;
    }
}