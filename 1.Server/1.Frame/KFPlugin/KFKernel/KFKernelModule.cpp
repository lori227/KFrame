#include "KFKernelModule.h"
#include "KFEntityEx.h"
#include "KFCore/KFDataConfig.h"
#include "KFCore/KFDataFactory.h"
#include "KFKernelConfig.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFKernelModule::InitModule()
    {
        __KF_ADD_CONFIG__( _kf_kernel_config, true );
    }

    void KFKernelModule::Run()
    {
        for ( auto& iter : _kf_component._objects )
        {
            auto kfcomponent = iter.second;
            kfcomponent->Run();
        }
    }

    void KFKernelModule::AfterRun()
    {
        for ( auto& iter : _kf_component._objects )
        {
            auto kfcomponent = iter.second;
            kfcomponent->AfterRun();
        }
    }

    void KFKernelModule::ShutDown()
    {
        __KF_REMOVE_CONFIG__( _kf_kernel_config );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    KFComponent* KFKernelModule::FindComponent( const std::string& dataname )
    {
        auto kfcomponent = _kf_component.Find( dataname );
        if ( kfcomponent == nullptr )
        {
            kfcomponent = _kf_component.Create( dataname );
            kfcomponent->SetName( dataname );
            static_cast< KFComponentEx* >( kfcomponent )->SetKernelModule( this );
        }

        return kfcomponent;
    }

    KFEntity* KFKernelModule::FindEntity( const std::string& dataname, uint64 key, const char* function, uint32 line )
    {
        auto kfcomponet = FindComponent( dataname );
        return kfcomponet->FindEntity( key, function, line );
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    KFData* KFKernelModule::CreateObject( const std::string& dataname )
    {
        return KFDataFactory::CreateData( "Global", dataname );
    }

    KFData* KFKernelModule::CreateObject( const KFDataSetting* datasetting )
    {
        return KFDataFactory::CreateData( datasetting );
    }

    KFData* KFKernelModule::CreateObject( const std::string& classname, const std::string& dataname )
    {
        return KFDataFactory::CreateData( classname, dataname );
    }

    KFData* KFKernelModule::CreateObject( const KFDataSetting* datasetting, const KFMsg::PBObject* proto )
    {
        auto kfdata = CreateObject( datasetting );
        ParseFromProto( kfdata, proto );
        return kfdata;
    }

    void KFKernelModule::ReleaseObject( KFData* kfdata )
    {
        KFDataFactory::Release( kfdata );
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFKernelModule::ParseFromProto( KFData* kfdata, const KFMsg::PBObject* proto )
    {
        CopyFromObject( kfdata, proto );
        return true;
    }

    void KFKernelModule::SerializeToClient( KFData* kfdata, KFMsg::PBObject* proto )
    {
        proto->Clear();
        SaveToObject( kfdata, proto, KFDataDefine::Mask_Sync_Client );
    }

    void KFKernelModule::SerializeToData( KFData* kfdata, KFMsg::PBObject* proto )
    {
        proto->Clear();
        SaveToObject( kfdata, proto, KFDataDefine::Mask_Save_Database );
    }

    void KFKernelModule::SerializeToView( KFData* kfdata, KFMsg::PBObject* proto )
    {
        proto->Clear();
        SaveToObject( kfdata, proto, KFDataDefine::Mask_Sync_View );
    }

    void KFKernelModule::SerializeToOnline( KFData* kfdata, KFMsg::PBObject* proto )
    {
        proto->Clear();
        SaveToObject( kfdata, proto, KFDataDefine::Mask_Online_Data );
    }

#define __COPY_FROM_PROTO__( kfdata, proto, pbdata ) \
    {\
        auto pbdata = &(proto->pbdata());\
        for ( auto iter = pbdata->begin(); iter != pbdata->end(); ++iter )\
        {\
            auto kfchild = kfdata->FindData( iter->first );\
            if ( kfchild != nullptr )\
            {\
                kfchild->SetValue( iter->second );\
            }\
        }\
    }\

    void KFKernelModule::CopyFromObject( KFData* kfdata, const KFMsg::PBObject* proto )
    {
        // int32
        __COPY_FROM_PROTO__( kfdata, proto, pbint32 );

        // uint32
        __COPY_FROM_PROTO__( kfdata, proto, pbuint32 );

        // int64
        __COPY_FROM_PROTO__( kfdata, proto, pbint64 );

        // uint64
        __COPY_FROM_PROTO__( kfdata, proto, pbuint64 );

        // double
        __COPY_FROM_PROTO__( kfdata, proto, pbdouble );

        // string
        __COPY_FROM_PROTO__( kfdata, proto, pbstring );

        // kfarray
        auto pbarray = &proto->pbarray();
        for ( auto iter = pbarray->begin(); iter != pbarray->end(); ++iter )
        {
            auto kfarray = kfdata->FindData( iter->first );
            if ( kfarray == nullptr )
            {
                continue;
            }

            // 数组列表
            auto pbuint64 = &( iter->second.pbuint64() );
            for ( auto citer = pbuint64->begin(); citer != pbuint64->end(); ++citer )
            {
                auto kfchild = KFDataFactory::CreateData( kfarray->GetDataSetting() );
                kfchild->SetValue( citer->second );

                kfarray->AddData( iter->first, kfchild );
            }
        }

        // kfobject
        auto pbobject = &proto->pbobject();
        for ( auto iter = pbobject->begin(); iter != pbobject->end(); ++iter )
        {
            auto kfobject = kfdata->FindData( iter->first );
            if ( kfobject == nullptr )
            {
                continue;
            }

            CopyFromObject( kfobject, &iter->second );
        }

        // kfrecord
        auto pbrecord = &proto->pbrecord();
        for ( auto iter = pbrecord->begin(); iter != pbrecord->end(); ++iter )
        {
            auto kfrecord = kfdata->FindData( iter->first );
            if ( kfrecord == nullptr )
            {
                continue;
            }

            // 对象列表
            auto pbobject = &( iter->second.pbobject() );
            for ( auto citer = pbobject->begin(); citer != pbobject->end(); ++citer )
            {
                auto kfobject = KFDataFactory::CreateData( kfrecord->GetDataSetting() );

                CopyFromObject( kfobject, &citer->second );
                kfrecord->AddData( iter->first, kfobject );
            }
        }
    }

#define __SAVE_TO_PROTO__( datatype, pbdata, type )\
case datatype:\
    {\
        auto& pbdata = *(proto->mutable_##pbdata()); \
        pbdata[ datasetting->_name ] = kfchild->GetValue<type>();\
        break; \
    }\

    void KFKernelModule::SaveToObject( KFData* kfdata, KFMsg::PBObject* proto, uint32 datamask )
    {
        auto datasetting = kfdata->GetDataSetting();
        if ( !datasetting->HaveFlagMask( datamask ) )
        {
            return;
        }

        for ( auto kfchild = kfdata->FirstData(); kfchild != nullptr; kfchild = kfdata->NextData() )
        {
            auto datasetting = kfchild->GetDataSetting();
            if ( !datasetting->HaveFlagMask( datamask ) || !kfchild->IsValid() )
            {
                continue;
            }

            switch ( datasetting->_type )
            {
                __SAVE_TO_PROTO__( KFDataDefine::Type_Int32, pbint32, int32 );
                __SAVE_TO_PROTO__( KFDataDefine::Type_UInt32, pbuint32, uint32 );
                __SAVE_TO_PROTO__( KFDataDefine::Type_Int64, pbint64, int64 );
                __SAVE_TO_PROTO__( KFDataDefine::Type_UInt64, pbuint64, uint64 );
                __SAVE_TO_PROTO__( KFDataDefine::Type_Double, pbdouble, double );
                __SAVE_TO_PROTO__( KFDataDefine::Type_String, pbstring, std::string );

            case KFDataDefine::Type_Object:
            {
                auto& pbobject = ( *proto->mutable_pbobject() )[ datasetting->_name ];
                SaveToObject( kfchild, &pbobject, datamask );
                break;
            }
            case KFDataDefine::Type_Record:
            {
                auto& pbrecord = ( *proto->mutable_pbrecord() )[ datasetting->_name ];
                for ( auto kfobject = kfchild->FirstData(); kfobject != nullptr; kfobject = kfchild->NextData() )
                {
                    auto& pbobject = ( *pbrecord.mutable_pbobject() )[ kfobject->GetKeyID() ];
                    SaveToObject( kfobject, &pbobject, datamask );
                }

                break;
            }
            }
        }
    }
}