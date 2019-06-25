#include "KFMongoLogic.hpp"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////
    KFMongoLogic::KFMongoLogic()
    {
        _read_execute = __KF_NEW__( KFReadExecute );
        _write_execute = __KF_NEW__( KFWriteExecute );
    }

    KFMongoLogic::~KFMongoLogic()
    {
        __KF_DELETE__( KFReadExecute, _read_execute );
        __KF_DELETE__( KFWriteExecute, _write_execute );
    }

    void KFMongoLogic::ShutDown()
    {
        _read_execute->ShutDown();
        _write_execute->ShutDown();
    }

    void KFMongoLogic::Initialize( KFMongoType* kfmongotype )
    {
        {
            auto kfmongolist = kfmongotype->FindMongoList( KFDatabaseEnum::Read );
            if ( kfmongolist != nullptr )
            {
                auto kfsetting = kfmongolist->FindSetting();
                if ( kfsetting != nullptr )
                {
                    _read_execute->InitMongo( kfsetting );
                }
            }
        }

        {
            auto kfmongolist = kfmongotype->FindMongoList( KFDatabaseEnum::Write );
            if ( kfmongolist != nullptr )
            {
                auto kfsetting = kfmongolist->FindSetting();
                if ( kfsetting != nullptr )
                {
                    _write_execute->InitMongo( kfsetting );
                }
            }
        }
    }
}