#include "KFMongoModule.hpp"

namespace KFrame
{
    void KFMongoModule::ShutDown()
    {
        KFLocker lock( _mongo_mutex );
        for ( auto& iter : _mongo_logic_map._objects )
        {
            iter.second->ShutDown();
        }
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    const KFMongoConnectOption* KFMongoModule::FindMongoConnectOption( const std::string& module, uint32 logic_id )
    {
        auto setting = KFMongoConfig::Instance()->FindSetting( module );
        if ( setting == nullptr )
        {
            __LOG_ERROR__( "[{}:{}] can't find mongo setting", module, logic_id );
            return nullptr;
        }

        for ( auto& connect_option : setting->_connect_option )
        {
            if ( logic_id >= connect_option._min_logic_id && logic_id <= connect_option._max_logic_id )
            {
                return &connect_option;
            }
        }

        return nullptr;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::shared_ptr<KFMongoLogic> KFMongoModule::FindMongoLogic( uint32 logic_id )
    {
        auto thread_id = KFThread::GetID();
        auto key = MongoKey( thread_id, logic_id );

        KFLocker lock( _mongo_mutex );
        return _mongo_logic_map.Find( key );
    }

    void KFMongoModule::InsertMongoLogic( uint32 logic_id, std::shared_ptr<KFMongoLogic> mongo_logic )
    {
        auto thread_id = KFThread::GetID();
        auto key = MongoKey( thread_id, logic_id );

        KFLocker lock( _mongo_mutex );
        _mongo_logic_map.Insert( key, mongo_logic );
    }

    std::shared_ptr<KFMongoDriver> KFMongoModule::Create( const std::string& module, uint32 logic_id /* = 0 */ )
    {
        auto mongo_option = FindMongoConnectOption( module, logic_id );
        if ( mongo_option == nullptr )
        {
            __LOG_ERROR__( "[{}:{}] can't find mongo option", module, logic_id );
            return nullptr;
        }

        auto mongo_logic = FindMongoLogic( mongo_option->_runtime_id );
        if ( mongo_logic == nullptr )
        {
            mongo_logic = __MAKE_SHARED__( KFMongoLogic );
            mongo_logic->Initialize( module, mongo_option );
            InsertMongoLogic( mongo_option->_runtime_id, mongo_logic );
        }

        return mongo_logic;
    }

}