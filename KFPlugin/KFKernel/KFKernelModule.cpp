#include "KFKernelModule.hpp"
#include "KFEntityEx.hpp"
#include "KFDataFactory.hpp"
#include "KFProtocol/KFProtocol.h"


namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFKernelModule::Run()
    {
        // 自动回收数据
        KFDataFactory::Instance()->RunAutoDestroyData();

        // 组件上下文逻辑
        for ( auto& iter : _kf_component._objects )
        {
            iter.second->Run();
        }
    }

    void KFKernelModule::AfterRun()
    {
        for ( auto& iter : _kf_component._objects )
        {
            iter.second->AfterRun();
        }
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    static std::string _global_class = "Global";
    KFComponent* KFKernelModule::FindComponent( const std::string& data_name )
    {
        auto kfcomponent = _kf_component.Find( data_name );
        if ( kfcomponent == nullptr )
        {
            kfcomponent = _kf_component.Create( data_name );
            kfcomponent->_component_name = data_name;
            kfcomponent->_data_setting = KFDataConfig::Instance()->FindDataSetting( _global_class, data_name );
        }

        return kfcomponent;
    }

    KFEntity* KFKernelModule::FindEntity( const std::string& data_name, uint64 key, const char* function, uint32 line )
    {
        auto kfcomponet = FindComponent( data_name );
        return kfcomponet->FindEntity( key, function, line );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    DataPtr KFKernelModule::CreateData( const KFDataSetting* datasetting )
    {
        return KFDataFactory::Instance()->CreateData( datasetting, true );
    }

    void KFKernelModule::DestroyData( DataPtr kfdata )
    {
        KFDataFactory::Instance()->DestroyData( kfdata );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFKernelModule::ParseFromProto( DataPtr kfdata, const KFMsg::PBObject* proto )
    {
        CopyFromObject( kfdata, proto );
        return true;
    }

    KFMsg::PBObject* KFKernelModule::Serialize( DataPtr kfdata )
    {
        return SerializeObject( kfdata, KFDataDefine::DataMaskNull, false, 0u );
    }

    KFMsg::PBObject* KFKernelModule::SerializeToData( DataPtr kfdata )
    {
        return SerializeObject( kfdata, KFDataDefine::DataMaskSave, false, 0u );
    }

    KFMsg::PBObject* KFKernelModule::SerializeToView( DataPtr kfdata )
    {
        return SerializeObject( kfdata, KFDataDefine::DataMaskView, false, 0u );
    }

    KFMsg::PBObject* KFKernelModule::SerializeToClient( DataPtr kfdata )
    {
        return SerializeObject( kfdata, KFDataDefine::DataMaskSync, false, 0u );
    }

    KFMsg::PBObject* KFKernelModule::SerializeToOnline( KFEntity* kfentity, uint32 delaytime /* = 0u */ )
    {
        // 把直接发送的数据返回给客户端
        return SerializeObject( kfentity, KFDataDefine::DataMaskSync, true, delaytime );
    }

    KFMsg::PBObject* KFKernelModule::SerializeObject( DataPtr kfdata, uint32 datamask, bool online, uint32 delaytime )
    {
        static KFMsg::PBObject proto_object;
        proto_object.Clear();

        SaveToEntity( kfdata, &proto_object, datamask, online, delaytime );
        return &proto_object;
    }

#define __COPY_FROM_PROTO__( data, proto, pbdata ) \
    {\
        auto proto_data = &(proto->pbdata());\
        for ( auto iter = proto_data->begin(); iter != proto_data->end(); ++iter )\
        {\
            auto child = data->Find( iter->first );\
            if ( child != nullptr )\
            {\
                child->Set( iter->second );\
            }\
            else\
            {\
                __LOG_CRITICAL__( "playerdata parent=[{}] not data=[{}]", data->_data_setting->_name, iter->first);\
            }\
        }\
    }\

    void KFKernelModule::CopyFromObject( DataPtr parent_data, const KFMsg::PBObject* proto )
    {
        // int32
        __COPY_FROM_PROTO__( parent_data, proto, pbint32 );

        // uint32
        __COPY_FROM_PROTO__( parent_data, proto, pbuint32 );

        // int64
        __COPY_FROM_PROTO__( parent_data, proto, pbint64 );

        // uint64
        __COPY_FROM_PROTO__( parent_data, proto, proto_uint64 );

        // double
        __COPY_FROM_PROTO__( parent_data, proto, pbdouble );

        // string
        __COPY_FROM_PROTO__( parent_data, proto, pbstring );

        // array_data
        auto proto_array = &proto->pbarray();
        for ( auto iter = proto_array->begin(); iter != proto_array->end(); ++iter )
        {
            auto array_data = parent_data->Find( iter->first );
            if ( array_data == nullptr )
            {
                __LOG_CRITICAL__( "playerdata parent=[{}] not array=[{}]", parent_data->_data_setting->_name, iter->first );
                continue;
            }

            // 数组列表
            auto proto_uint64 = &iter->second.pbuint64();
            for ( auto citer = proto_uint64->begin(); citer != proto_uint64->end(); ++citer )
            {
                auto child_data = array_data->Find( citer->first );
                if ( child_data == nullptr )
                {
                    child_data = KFDataFactory::Instance()->AddArray( array_data );
                }
                child_data->Set( citer->second );
            }
        }

        // object_data
        auto proto_object = &proto->pbobject();
        for ( auto iter = proto_object->begin(); iter != proto_object->end(); ++iter )
        {
            auto object_data = parent_data->Find( iter->first );
            if ( object_data == nullptr )
            {
                __LOG_CRITICAL__( "playerdata parent=[{}] not object=[{}]", parent_data->_data_setting->_name, iter->first );
                continue;
            }

            CopyFromObject( object_data, &iter->second );
        }

        // kfrecord
        auto proto_record = &proto->pbrecord();
        for ( auto iter = proto_record->begin(); iter != proto_record->end(); ++iter )
        {
            auto record_data = parent_data->Find( iter->first );
            if ( record_data == nullptr )
            {
                __LOG_CRITICAL__( "playerdata parent=[{}] not record=[{}]", parent_data->_data_setting->_name, iter->first );
                continue;
            }

            // 对象列表
            auto proto_object = &( iter->second.pbobject() );
            for ( auto citer = proto_object->begin(); citer != proto_object->end(); ++citer )
            {
                auto object_data = KFDataFactory::Instance()->CreateData( record_data->_data_setting, false );
                CopyFromObject( object_data, &citer->second );
                object_data->Add( citer->first, object_data );
            }
        }
    }

#define __SAVE_TO_PROTO__( datatype, pbdata, type )\
case datatype:\
    {\
        auto& pbdata = *(proto->mutable_##pbdata()); \
        pbdata[ datasetting->_name ] = kfchild->Get<type>();\
        break; \
    }\


    void KFKernelModule::SaveToEntity( DataPtr kfdata, KFMsg::PBObject* proto, uint32 datamask, bool online, uint32 delaytime )
    {
        auto datasetting = kfdata->_data_setting;
        if ( !datasetting->HaveMask( datamask ) )
        {
            return;
        }

        SaveToObject( kfdata, proto, datamask, online, delaytime );
    }

    void KFKernelModule::SaveToObject( DataPtr kfdata, KFMsg::PBObject* proto, uint32 datamask, bool online /* = false */, uint32 delaytime /* = 0u */ )
    {
        for ( auto kfchild = kfdata->First(); kfchild != nullptr; kfchild = kfdata->Next() )
        {
            auto datasetting = kfchild->_data_setting;
            if ( !datasetting->HaveMask( datamask ) ||
                    !kfchild->IsValid() ||
                    ( online && datasetting->_delay_online_sync_time != delaytime ) )
            {
                continue;
            }

            switch ( datasetting->_type )
            {
                __SAVE_TO_PROTO__( KFDataDefine::DataTypeInt32, pbint32, int32 );
                __SAVE_TO_PROTO__( KFDataDefine::DataTypeUInt32, pbuint32, uint32 );
                __SAVE_TO_PROTO__( KFDataDefine::DataTypeInt64, pbint64, int64 );
                __SAVE_TO_PROTO__( KFDataDefine::DataTypeUInt64, proto_uint64, uint64 );
                __SAVE_TO_PROTO__( KFDataDefine::DataTypeDouble, pbdouble, double );
                __SAVE_TO_PROTO__( KFDataDefine::DataTypeString, pbstring, std::string );

            case KFDataDefine::DataTypeArray:
            {
                auto& pbarray = ( *proto->mutable_pbarray() )[ datasetting->_name ];

                auto maxsize = kfchild->MaxSize();
                for ( uint32 i = KFGlobal::Instance()->_array_index; i < maxsize; ++i )
                {
                    auto kfuint64 = kfchild->Find( i );
                    if ( kfuint64 != nullptr && kfuint64->IsValid() )
                    {
                        ( *pbarray.mutable_pbuint64() )[ i ] = kfuint64->Get<int64>();
                    }
                }
                break;
            }
            case KFDataDefine::DataTypeObject:
            {
                auto& proto_object = ( *proto->mutable_pbobject() )[ datasetting->_name ];
                SaveToObject( kfchild, &proto_object, datamask );
                break;
            }
            case KFDataDefine::DataTypeRecord:
            {
                auto& pbrecord = ( *proto->mutable_pbrecord() )[ datasetting->_name ];
                for ( auto object_data = kfchild->First(); object_data != nullptr; object_data = kfchild->Next() )
                {
                    auto& proto_object = ( *pbrecord.mutable_pbobject() )[ object_data->GetKeyID() ];
                    SaveToObject( object_data, &proto_object, datamask );
                }
                break;
            }
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
}