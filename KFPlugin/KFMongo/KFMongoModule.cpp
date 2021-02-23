#include "KFMongoModule.hpp"

namespace KFrame
{
    void KFMongoModule::ShutDown()
    {
        KFLocker lock( _kf_mongo_mutex );
        for ( auto& iter : _mongo_logic_map._objects )
        {
            auto kflogic = iter.second;
            kflogic->ShutDown();
        }
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    const KFMongoConnectOption* KFMongoModule::FindMongoConnectOption( const std::string& module, uint32 logicid )
    {
        auto setting = KFMongoConfig::Instance()->FindSetting( module );
        if ( setting == nullptr )
        {
            __LOG_ERROR__( "[{}:{}] can't find mongo setting", module, logicid );
            return nullptr;
        }

        for ( auto& connectoption : setting->_connect_option )
        {
            if ( logicid >= connectoption._min_logic_id && logicid <= connectoption._max_logic_id )
            {
                return &connectoption;
            }
        }

        return nullptr;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    KFMongoLogic* KFMongoModule::FindMongoLogic( uint32 id )
    {
        auto threadid = KFThread::GetThreadID();
        auto key = MongoKey( threadid, id );

        KFLocker lock( _kf_mongo_mutex );
        return _mongo_logic_map.Find( key );
    }

    void KFMongoModule::InsertMongoLogic( uint32 id, KFMongoLogic* kflogic )
    {
        auto threadid = KFThread::GetThreadID();
        auto key = MongoKey( threadid, id );

        KFLocker lock( _kf_mongo_mutex );
        _mongo_logic_map.Insert( key, kflogic );
    }

    KFMongoDriver* KFMongoModule::Create( const std::string& module, uint32 logicid /* = 0 */ )
    {
        auto kfmongooption = FindMongoConnectOption( module, logicid );
        if ( kfmongooption == nullptr )
        {
            __LOG_ERROR__( "[{}:{}] can't find mongo option", module, logicid );
            return nullptr;
        }

        auto kflogic = FindMongoLogic( kfmongooption->_runtime_id );
        if ( kflogic != nullptr )
        {
            return kflogic;
        }

        kflogic = __KF_NEW__( KFMongoLogic );
        kflogic->Initialize( module, kfmongooption );
        InsertMongoLogic( kfmongooption->_runtime_id, kflogic );

        return kflogic;
    }

}