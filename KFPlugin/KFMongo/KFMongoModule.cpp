#include "KFMongoModule.hpp"

namespace KFrame
{
    void KFMongoModule::ShutDown()
    {
        KFLocker lock( _kf_mutex );
        for ( auto& iter : _mongo_logic_map._objects )
        {
            auto kflogic = iter.second;
            kflogic->ShutDown();
        }
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    KFMongoLogic* KFMongoModule::FindMongoLogic( uint32 id )
    {
        auto threadid = KFThread::GetThreadID();
        auto key = MongoKey( threadid, id );

        KFLocker lock( _kf_mutex );
        return _mongo_logic_map.Find( key );
    }

    void KFMongoModule::InsertMongoLogic( uint32 id, KFMongoLogic* kflogic )
    {
        auto threadid = KFThread::GetThreadID();
        auto key = MongoKey( threadid, id );

        KFLocker lock( _kf_mutex );
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