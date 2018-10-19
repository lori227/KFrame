#include "KFKernelModule.h"
#include "KFEntityEx.h"
#include "KFCore/KFDataConfig.h"
#include "KFCore/KFDataFactory.h"
#include "KFKernelConfig.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    KFKernelModule::KFKernelModule()
    {

    }

    KFKernelModule::~KFKernelModule()
    {
        _kf_component.Clear();
    }

    void KFKernelModule::InitModule()
    {
        __KF_ADD_CONFIG__( _kf_kernel_config, this );
    }

    void KFKernelModule::AfterLoad()
    {
        auto strmultipledata = _kf_option->GetString( __KF_STRING__( multipledata ) );
        while ( !strmultipledata.empty() )
        {
            _multiple_data.insert( KFUtility::SplitString( strmultipledata, ";" ) );
        }
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
        __KF_REMOVE_CONFIG__();
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    const KFClassSetting* KFKernelModule::FindClassSetting( const std::string& dataname )
    {
        static auto _global_class_name_option = _kf_option->FindOption( __KF_STRING__( globalclassname ) );
        auto kfdatasetting = _kf_data_config->FindDataSetting( _global_class_name_option->_str_value, dataname );
        if ( kfdatasetting == nullptr )
        {
            return nullptr;
        }

        return _kf_data_config->FindClassSetting( kfdatasetting->_contain_class );
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
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
        static auto _global_class_name_option = _kf_option->FindOption( __KF_STRING__( globalclassname ) );

        return KFDataFactory::CreateData( _global_class_name_option->_str_value, dataname );
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
    bool KFKernelModule::ParseFromProto( KFData* kfdata, const std::string& proto )
    {
        KFMsg::PBObject pbobject;
        if ( pbobject.ParseFromString( proto ) )
        {
            CopyFromObject( kfdata, &pbobject );
            return true;
        }

        return false;
    }

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

    void KFKernelModule::CopyFromObject( KFData* kfdata, const KFMsg::PBObject* proto )
    {
        kfdata->SetKeyID( proto->key() );

        // kfint32
        for ( auto i = 0; i < proto->pbint32_size(); ++i )
        {
            auto pbint32 = &proto->pbint32( i );
            CopyFromInt32( kfdata, pbint32 );
        }

        // kfuint32
        for ( auto i = 0; i < proto->pbuint32_size(); ++i )
        {
            auto pbuint32 = &proto->pbuint32( i );
            CopyFromUInt32( kfdata, pbuint32 );
        }

        // kfint64
        for ( auto i = 0; i < proto->pbint64_size(); ++i )
        {
            auto pbint64 = &proto->pbint64( i );
            CopyFromInt64( kfdata, pbint64 );
        }

        // kfuint64
        for ( auto i = 0; i < proto->pbuint64_size(); ++i )
        {
            auto pbuint64 = &proto->pbuint64( i );
            CopyFromUInt64( kfdata, pbuint64 );
        }

        // kfdouble
        for ( auto i = 0; i < proto->pbdouble_size(); ++i )
        {
            auto pbdouble = &proto->pbdouble( i );
            CopyFromDouble( kfdata, pbdouble );
        }

        // kfstring
        for ( auto i = 0; i < proto->pbstring_size(); ++i )
        {
            auto pbstring = &proto->pbstring( i );
            CopyFromString( kfdata, pbstring );
        }

        // kfvector3d
        for ( auto i = 0; i < proto->pbvector3d_size(); ++i )
        {
            auto pbvector3d = &proto->pbvector3d( i );
            CopyFromVector3D( kfdata, pbvector3d );
        }

        // kfarray
        for ( auto i = 0; i < proto->pbarray_size(); ++i )
        {
            auto pbarray = &proto->pbarray( i );
            CopyFromArray( kfdata, pbarray );
        }

        // kfobject
        for ( auto i = 0; i < proto->pbobject_size(); ++i )
        {
            auto pbobject = &proto->pbobject( i );
            auto kfobject = kfdata->FindData( pbobject->name() );
            if ( kfobject != nullptr )
            {
                CopyFromObject( kfobject, pbobject );
            }
        }

        // kfrecord
        for ( auto i = 0; i < proto->pbrecord_size(); ++i )
        {
            auto pbrecord = &proto->pbrecord( i );
            auto kfrecord = kfdata->FindData( pbrecord->name() );
            if ( kfrecord != nullptr )
            {
                CopyFromRecord( kfrecord, pbrecord );
            }
        }
    }

    void KFKernelModule::CopyFromString( KFData* kfdata, const KFMsg::PBString* proto )
    {
        auto kfstring = kfdata->FindData( proto->name() );
        if ( kfstring == nullptr )
        {
            return;
        }

        kfstring->SetValue<std::string>( proto->value() );
    }

    void KFKernelModule::CopyFromInt32( KFData* kfdata, const KFMsg::PBInt32* proto )
    {
        auto kfint32 = kfdata->FindData( proto->name() );
        if ( kfint32 == nullptr )
        {
            return;
        }

        kfint32->SetValue<int32>( proto->value() );
    }

    void KFKernelModule::CopyFromUInt32( KFData* kfdata, const KFMsg::PBUInt32* proto )
    {
        auto kfuint32 = kfdata->FindData( proto->name() );
        if ( kfuint32 == nullptr )
        {
            return;
        }

        kfuint32->SetValue<uint32>( proto->value() );
    }

    void KFKernelModule::CopyFromInt64( KFData* kfdata, const KFMsg::PBInt64* proto )
    {
        auto kfint64 = kfdata->FindData( proto->name() );
        if ( kfint64 == nullptr )
        {
            return;
        }

        kfint64->SetValue<int64>( proto->value() );
    }

    void KFKernelModule::CopyFromUInt64( KFData* kfdata, const KFMsg::PBUInt64* proto )
    {
        auto kfuint64 = kfdata->FindData( proto->name() );
        if ( kfuint64 == nullptr )
        {
            return;
        }

        kfuint64->SetValue<uint64>( proto->value() );
    }

    void KFKernelModule::CopyFromDouble( KFData* kfdata, const KFMsg::PBDouble* proto )
    {
        auto kffloat = kfdata->FindData( proto->name() );
        if ( kffloat == nullptr )
        {
            return;
        }

        kffloat->SetValue<double>( proto->value() );
    }

    void KFKernelModule::CopyFromVector3D( KFData* kfdata, const KFMsg::PBVector3D* proto )
    {
        auto kfvector3d = kfdata->FindData( proto->name() );
        if ( kfvector3d == nullptr )
        {
            return;
        }

        auto& value = proto->value();
        Math3D::Vector3D vector3d( value.x(), value.y(), value.z() );
        kfvector3d->SetValue<Math3D::Vector3D>( vector3d );
    }

    void KFKernelModule::CopyFromArray( KFData* kfdata, const KFMsg::PBArray* proto )
    {
        auto kfarray = kfdata->FindData( proto->name() );
        if ( kfarray == nullptr )
        {
            return;
        }

        // int32
        for ( auto i = 0; i < proto->pbint32_size(); ++i )
        {
            auto pbint32 = proto->pbint32( i );

            auto kftemp = kfarray->FindData( i );
            if ( kftemp != nullptr )
            {
                kftemp->SetValue< int32 >( pbint32 );
            }
        }

        // uint32
        for ( auto i = 0; i < proto->pbuint32_size(); ++i )
        {
            auto pbuint32 = proto->pbuint32( i );

            auto kftemp = kfarray->FindData( i );
            if ( kftemp != nullptr )
            {
                kftemp->SetValue< uint32 >( pbuint32 );
            }
        }

        // int64
        for ( auto i = 0; i < proto->pbint64_size(); ++i )
        {
            auto pbint64 = proto->pbint64( i );

            auto kftemp = kfarray->FindData( i );
            if ( kftemp != nullptr )
            {
                kftemp->SetValue< int64 >( pbint64 );
            }
        }

        // uint64
        for ( auto i = 0; i < proto->pbuint64_size(); ++i )
        {
            auto pbuint64 = proto->pbuint64( i );

            auto kftemp = kfarray->FindData( i );
            if ( kftemp != nullptr )
            {
                kftemp->SetValue< uint64 >( pbuint64 );
            }
        }

        // double
        for ( auto i = 0; i < proto->pbdouble_size(); ++i )
        {
            auto pbdouble = proto->pbdouble( i );

            auto kftemp = kfarray->FindData( i );
            if ( kftemp != nullptr )
            {
                kftemp->SetValue< double >( pbdouble );
            }
        }

        // string
        for ( auto i = 0; i < proto->pbstring_size(); ++i )
        {
            auto pbstring = proto->pbstring( i );

            auto kftemp = kfarray->FindData( i );
            if ( kftemp != nullptr )
            {
                kftemp->SetValue< std::string >( pbstring );
            }
        }

        // vector3d
        for ( auto i = 0; i < proto->pbvector3d_size(); ++i )
        {
            auto& pbvector3d = proto->pbvector3d( i );

            auto kftemp = kfarray->FindData( i );
            if ( kftemp != nullptr )
            {
                Math3D::Vector3D vector3d( pbvector3d.x(), pbvector3d.y(), pbvector3d.z() );
                kftemp->SetValue< Math3D::Vector3D >( vector3d );
            }
        }
    }

    void KFKernelModule::CopyFromRecord( KFData* kfparent, const KFMsg::PBRecord* proto )
    {
        for ( auto i = 0; i < proto->pbobject_size(); ++i )
        {
            auto pbobject = &proto->pbobject( i );

            auto kfobject = KFDataFactory::CreateData( kfparent->_data_setting );
            if ( kfobject == nullptr )
            {
                continue;
            }

            CopyFromObject( kfobject, pbobject );

            auto key = kfobject->GetKeyID();
            kfparent->AddData( key, kfobject );
        }
    }

    void KFKernelModule::SaveToInt32( KFData* kfdata, KFMsg::PBInt32* pbint32 )
    {
        pbint32->set_name( kfdata->_data_setting->_name );
        pbint32->set_value( kfdata->GetValue<int32>() );
    }

    void KFKernelModule::SaveToUInt32( KFData* kfdata, KFMsg::PBUInt32* pbuint32 )
    {
        pbuint32->set_name( kfdata->_data_setting->_name );
        pbuint32->set_value( kfdata->GetValue<uint32>() );
    }

    void KFKernelModule::SaveToInt64( KFData* kfdata, KFMsg::PBInt64* pbint64 )
    {
        pbint64->set_name( kfdata->_data_setting->_name );
        pbint64->set_value( kfdata->GetValue<int64>() );
    }

    void KFKernelModule::SaveToUInt64( KFData* kfdata, KFMsg::PBUInt64* pbuint64 )
    {
        pbuint64->set_name( kfdata->_data_setting->_name );
        pbuint64->set_value( kfdata->GetValue<uint64>() );
    }

    void KFKernelModule::SaveToDouble( KFData* kfdata, KFMsg::PBDouble* pbdouble )
    {
        pbdouble->set_name( kfdata->_data_setting->_name );
        pbdouble->set_value( kfdata->GetValue<double>() );
    }

    void KFKernelModule::SaveToString( KFData* kfdata, KFMsg::PBString* pbstring )
    {
        pbstring->set_name( kfdata->_data_setting->_name );
        pbstring->set_value( kfdata->GetValue<std::string>() );
    }

    void KFKernelModule::SaveToVector3D( KFData* kfdata, KFMsg::PBVector3D* proto )
    {
        proto->set_name( kfdata->_data_setting->_name );

        auto pbvector3d = proto->mutable_value();
        auto kfvector3d = kfdata->GetValue<Math3D::Vector3D>();

        pbvector3d->set_x( kfvector3d.GetX() );
        pbvector3d->set_y( kfvector3d.GetY() );
        pbvector3d->set_z( kfvector3d.GetZ() );
    }

    void KFKernelModule::SaveToArray( KFData* kfdata, KFMsg::PBArray* proto )
    {
        auto datasetting = kfdata->GetDataSetting();
        proto->set_name( datasetting->_name );

        switch ( datasetting->_type )
        {
        case KFDataDefine::Type_Int32:
        {
            auto child = kfdata->FirstData();
            while ( child != nullptr )
            {
                proto->add_pbint32( child->GetValue<int32>() );
                child = kfdata->NextData();
            }
            break;
        }
        case KFDataDefine::Type_UInt32:
        {
            auto child = kfdata->FirstData();
            while ( child != nullptr )
            {
                proto->add_pbuint32( child->GetValue<uint32>() );
                child = kfdata->NextData();
            }
            break;
        }
        case KFDataDefine::Type_Int64:
        {
            auto child = kfdata->FirstData();
            while ( child != nullptr )
            {
                proto->add_pbint64( child->GetValue<int64>() );
                child = kfdata->NextData();
            }
            break;
        }
        case KFDataDefine::Type_UInt64:
        {
            auto child = kfdata->FirstData();
            while ( child != nullptr )
            {
                proto->add_pbuint64( child->GetValue<uint64>() );
                child = kfdata->NextData();
            }
            break;
        }
        case KFDataDefine::Type_Double:
        {
            auto child = kfdata->FirstData();
            while ( child != nullptr )
            {
                proto->add_pbdouble( child->GetValue<double>() );
                child = kfdata->NextData();
            }
            break;
        }
        case KFDataDefine::Type_Vector3D:
        {
            auto child = kfdata->FirstData();
            while ( child != nullptr )
            {
                auto pbvector3d = proto->add_pbvector3d();
                auto kfvector3d = child->GetValue<Math3D::Vector3D>();

                pbvector3d->set_x( kfvector3d.GetX() );
                pbvector3d->set_y( kfvector3d.GetY() );
                pbvector3d->set_z( kfvector3d.GetZ() );

                child = kfdata->NextData();
            }
            break;
        }
        case KFDataDefine::Type_String:
        {
            auto child = kfdata->FirstData();
            while ( child != nullptr )
            {
                proto->add_pbstring( child->GetValue<std::string>() );
                child = kfdata->NextData();
            }
            break;
        }
        default:
            break;
        }
    }

    void KFKernelModule::SaveToObject( KFData* kfdata, KFMsg::PBObject* proto, uint32 datamask )
    {
        auto datasetting = kfdata->GetDataSetting();
        if ( !datasetting->HaveFlagMask( datamask ) )
        {
            return;
        }

        proto->set_name( datasetting->_name );
        auto keyid = kfdata->GetKeyID();
        if ( keyid != _invalid_int )
        {
            proto->set_key( keyid );
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
            case KFDataDefine::Type_Int32:
                SaveToInt32( kfchild, proto->add_pbint32() );
                break;
            case KFDataDefine::Type_UInt32:
                SaveToUInt32( kfchild, proto->add_pbuint32() );
                break;
            case KFDataDefine::Type_Int64:
                SaveToInt64( kfchild, proto->add_pbint64() );
                break;
            case KFDataDefine::Type_UInt64:
                SaveToUInt64( kfchild, proto->add_pbuint64() );
                break;
            case KFDataDefine::Type_Double:
                SaveToDouble( kfchild, proto->add_pbdouble() );
                break;
            case KFDataDefine::Type_String:
                SaveToString( kfchild, proto->add_pbstring() );
                break;
            case KFDataDefine::Type_Array:
                SaveToArray( kfchild, proto->add_pbarray() );
                break;
            case KFDataDefine::Type_Vector3D:
                SaveToVector3D( kfchild, proto->add_pbvector3d() );
                break;
            case KFDataDefine::Type_Object:
                SaveToObject( kfchild, proto->add_pbobject(), datamask );
                break;
            case KFDataDefine::Type_Record:
                SaveToRecord( kfchild, proto->add_pbrecord(), datamask );
                break;
            }
        }
    }

    void KFKernelModule::SaveToRecord( KFData* kfdata, KFMsg::PBRecord* proto, uint32 datamask )
    {
        auto datasetting = kfdata->GetDataSetting();
        proto->set_name( datasetting->_name );
        for ( auto child = kfdata->FirstData(); child != nullptr; child = kfdata->NextData() )
        {
            SaveToObject( child, proto->add_pbobject(), datamask );
        }
    }

    bool KFKernelModule::IsMultipleData( const std::string& dataname )
    {
        return _multiple_data.find( dataname ) != _multiple_data.end();
    }

    uint32 KFKernelModule::CalcAgentValue( KFAgentValue* kfvalue, float multiple )
    {
        auto ok = IsMultipleData( kfvalue->_data_name );
        if ( !ok )
        {
            multiple = 1.0f;
        }

        return kfvalue->GetValue( multiple );
    }


}