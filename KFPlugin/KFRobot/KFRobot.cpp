#include "KFRobot.hpp"
#include "KFFsm.hpp"
#include "KFRobotModule.hpp"

namespace KFrame
{
    KFRobot::KFRobot()
    {
        _fsm = nullptr;
        _net_client = nullptr;
        _kf_component = nullptr;
    }

    KFRobot::~KFRobot()
    {
        __DELETE_OBJECT__( _fsm );

        _player = nullptr;
        _kf_component->RemoveEntity( _player_id );
    }

    void KFRobot::Init( uint64 id, KFNetClientEngine* netclient )
    {
        _id = id;
        _account = __FORMAT__( "{}{}", KFRobotConfig::Instance()->_robot_account, id );

        _net_client = netclient;
        _fsm = new KFFsm( this );

        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );
        ////////////////////////////////////////////////////////////////////////////////
    }

    void KFRobot::Run()
    {
        _fsm->Run();
    }

    void KFRobot::ChangeState( uint32 state )
    {
        _fsm->ChangeState( state );
    }

    void KFRobot::StartConnectGate()
    {
        _client_id = 0u;

        KFIpAddress ipaddress;
        ipaddress._name = __STRING__( zone );
        ipaddress._type = __STRING__( gate );
        ipaddress._id = _id;
        ipaddress._ip = _login_gate_ip;
        ipaddress._port = _login_gate_port;
        ipaddress._str_id = __TO_STRING__( _id );
        _net_client->StartClient( &ipaddress );
    }

    bool KFRobot::SendNetMessage( uint32 msg_id, google::protobuf::Message* message )
    {
        auto strdata = message->SerializeAsString();
        return _net_client->SendNetMessage( _client_id, msg_id, strdata.data(), strdata.size() );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFRobot::SyncAddObject( DataPtr kfobject, const KFMsg::PBObject* pbobject )
    {
        // 对象
        {
            auto pbchild = &pbobject->pbobject();
            for ( auto iter = pbchild->begin(); iter != pbchild->end(); ++iter )
            {
                auto kfdata = kfobject->FindData( iter->first );
                if ( kfdata == nullptr )
                {
                    continue;
                }

                SyncAddObject( kfdata, &iter->second );
            }
        }

        // 集合
        {
            auto pbchild = &pbobject->pbrecord();
            for ( auto iter = pbchild->begin(); iter != pbchild->end(); ++iter )
            {
                auto kfdata = kfobject->FindData( iter->first );
                if ( kfdata == nullptr )
                {
                    continue;
                }

                SyncAddRecord( kfdata, &iter->second );
            }
        }
    }

    void KFRobot::SyncAddRecord( DataPtr kfrecord, const KFMsg::PBRecord* pbrecord )
    {
        auto pbchild = &pbrecord->pbobject();
        for ( auto iter = pbchild->begin(); iter != pbchild->end(); ++iter )
        {
            auto kfdata = _kf_kernel->CreateObject( kfrecord->_data_setting, &iter->second );
            kfrecord->AddData( iter->first, kfdata );
        }
    }

#define __SYNC_UPDATE_DATA__( datatype, pbdata )\
    {\
        auto pbdata = &pbobject->pbdata();\
        for ( auto iter = pbdata->begin(); iter != pbdata->end(); ++iter )\
        {\
            auto kfdata = kfobject->FindData( iter->first );\
            if ( kfdata != nullptr )\
            {\
                kfdata->SetValue< datatype >( iter->second );\
            }\
        }\
    }

    void KFRobot::SyncUpdateData( DataPtr kfobject, const KFMsg::PBObject* pbobject )
    {
        __SYNC_UPDATE_DATA__( int32, pbint32 );
        __SYNC_UPDATE_DATA__( uint32, pbuint32 );
        __SYNC_UPDATE_DATA__( int64, pbint64 );
        __SYNC_UPDATE_DATA__( uint64, pbuint64 );
        __SYNC_UPDATE_DATA__( double, pbdouble );
        __SYNC_UPDATE_DATA__( std::string, pbstring );

        // array
        {
            auto pbarray = &pbobject->pbarray();
            for ( auto iter  = pbarray->begin(); iter != pbarray->end(); ++iter )
            {
                auto kfdata = kfobject->FindData( iter->first );
                if ( kfdata == nullptr )
                {
                    continue;
                }

                auto pbuint64 = &iter->second.pbuint64();
                for ( auto citer = pbuint64->begin(); citer != pbuint64->end(); ++citer )
                {
                    auto kfchilddata = kfdata->FindData( citer->first );
                    if ( kfchilddata != nullptr )
                    {
                        kfchilddata->SetValue< uint64 >( citer->second );
                    }
                }
            }
        }


        // object
        {
            auto pbchild = &pbobject->pbobject();
            for ( auto iter = pbchild->begin(); iter != pbchild->end(); ++iter )
            {
                auto kfdata = kfobject->FindData( iter->first );
                if ( kfdata != nullptr )
                {
                    SyncUpdateData( kfdata, &iter->second );
                }
            }
        }

        // record
        {
            auto pbchild = &pbobject->pbrecord();
            for ( auto iter = pbchild->begin(); iter != pbchild->end(); ++iter )
            {
                auto kfrecord = kfobject->FindData( iter->first );
                if ( kfrecord == nullptr )
                {
                    continue;
                }

                auto pbchildobject = &iter->second.pbobject();
                for ( auto citer = pbchildobject->begin(); citer != pbchildobject->end(); ++citer )
                {
                    auto kfchildobject = kfrecord->FindData( citer->first );
                    if ( kfchildobject != nullptr )
                    {
                        SyncUpdateData( kfchildobject, &citer->second );
                    }
                }
            }
        }
    }

    void KFRobot::SyncRemoveData( DataPtr kfobject, const KFMsg::PBObject* pbobject )
    {
        // object
        {
            auto pbchild = &pbobject->pbobject();
            for ( auto iter = pbchild->begin(); iter != pbchild->end(); ++iter )
            {
                auto kfdata = kfobject->FindData( iter->first );
                if ( kfdata != nullptr )
                {
                    SyncRemoveData( kfdata, &iter->second );
                }
            }
        }

        // record
        {
            auto pbchild = &pbobject->pbrecord();
            for ( auto iter = pbchild->begin(); iter != pbchild->end(); ++iter )
            {
                auto kfrecord = kfobject->FindData( iter->first );
                if ( kfrecord == nullptr )
                {
                    continue;
                }

                auto pbchildobject = &iter->second.pbobject();
                for ( auto citer = pbchildobject->begin(); citer != pbchildobject->end(); ++citer )
                {
                    kfrecord->RemoveData( citer->first );
                }
            }
        }
    }
}