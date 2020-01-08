#include "KFMongoModule.hpp"

namespace KFrame
{
    void KFMongoModule::BeforeRun()
    {
        /////////////////////////////////////////////////////////////////////////////////////////
        // 设置uuid生成配置
        _kf_uuid.InitSetting( 32, 0, 10, 21 );
        /////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFMongoModule::ShutDown()
    {
        KFLocker lock( _kf_mongo_mutex );
        for ( auto& iter : _mongo_logic_map._objects )
        {
            auto kflogic = iter.second;
            kflogic->ShutDown();
        }
    }

    uint64 KFMongoModule::MakeUUID()
    {
        auto kfglobal = KFGlobal::Instance();
        KFLocker lock( _kf_uuid_mutex );
        return _kf_uuid.Make( 0, kfglobal->_app_id->GetWorkId(), kfglobal->_real_time );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
        auto kfmongotype = KFMongoConfig::Instance()->FindMongoType( module, logicid );
        if ( kfmongotype == nullptr )
        {
            __LOG_ERROR__( "[{}:{}] can't find mysql type", module, logicid );
            return nullptr;
        }

        auto kflogic = FindMongoLogic( kfmongotype->_id );
        if ( kflogic != nullptr )
        {
            return kflogic;
        }

        kflogic = __KF_NEW__( KFMongoLogic );
        kflogic->Initialize( kfmongotype );
        InsertMongoLogic( kfmongotype->_id, kflogic );

        return kflogic;
    }

}