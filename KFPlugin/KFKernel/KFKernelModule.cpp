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
    std::shared_ptr<KFComponent> KFKernelModule::FindComponent( const std::string& data_name )
    {
        auto component = _kf_component.Find( data_name );
        if ( component == nullptr )
        {
            component = _kf_component.Create( data_name );
            component->_name = data_name;
            component->_data_setting = KFDataConfig::Instance()->FindDataSetting( _global_class, data_name );
        }

        return component;
    }

    EntityPtr KFKernelModule::FindEntity( const std::string& data_name, uint64 key, const char* function, uint32 line )
    {
        auto component = FindComponent( data_name );
        return component->FindEntity( key, function, line );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFKernelModule::ParseFromMessage( DataPtr object_data, const KFMsg::PBObject* message )
    {
        CopyFromObject( object_data, message );
        return true;
    }

    KFMsg::PBObject* KFKernelModule::Serialize( DataPtr object_data )
    {
        return SerializeObject( object_data, KFDataDefine::DataMaskNull, false, 0u );
    }

    KFMsg::PBObject* KFKernelModule::SerializeToData( DataPtr object_data )
    {
        return SerializeObject( object_data, KFDataDefine::DataMaskSave, false, 0u );
    }

    KFMsg::PBObject* KFKernelModule::SerializeToView( DataPtr object_data )
    {
        return SerializeObject( object_data, KFDataDefine::DataMaskView, false, 0u );
    }

    KFMsg::PBObject* KFKernelModule::SerializeToClient( DataPtr object_data )
    {
        return SerializeObject( object_data, KFDataDefine::DataMaskSync, false, 0u );
    }

    KFMsg::PBObject* KFKernelModule::SerializeToOnline( EntityPtr entity, uint32 delay_time /* = 0u */ )
    {
        // 把直接发送的数据返回给客户端
        return SerializeObject( entity, KFDataDefine::DataMaskSync, true, delay_time );
    }

    KFMsg::PBObject* KFKernelModule::SerializeObject( DataPtr object_data, uint32 data_mask, bool online, uint32 delay_time )
    {
        static KFMsg::PBObject proto_object;
        proto_object.Clear();

        SaveToEntity( object_data, &proto_object, data_mask, online, delay_time );
        return &proto_object;
    }

#define __COPY_FROM_PROTO__( data, data_mask, function_name ) \
    {\
        auto proto_data = &(data_mask->function_name());\
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

    void KFKernelModule::CopyFromObject( DataPtr object_data, const KFMsg::PBObject* message )
    {
        // int32
        __COPY_FROM_PROTO__( object_data, message, pbint32 );

        // uint32
        __COPY_FROM_PROTO__( object_data, message, pbuint32 );

        // int64
        __COPY_FROM_PROTO__( object_data, message, pbint64 );

        // uint64
        __COPY_FROM_PROTO__( object_data, message, pbuint64 );

        // double
        __COPY_FROM_PROTO__( object_data, message, pbdouble );

        // string
        __COPY_FROM_PROTO__( object_data, message, pbstring );

        // array_data
        auto proto_array = &message->pbarray();
        for ( auto iter = proto_array->begin(); iter != proto_array->end(); ++iter )
        {
            auto child_array_data = object_data->Find( iter->first );
            if ( child_array_data == nullptr )
            {
                __LOG_CRITICAL__( "playerdata parent=[{}] not array=[{}]", object_data->_data_setting->_name, iter->first );
                continue;
            }

            // 数组列表
            auto proto_uint64 = &iter->second.pbuint64();
            for ( auto value_iter = proto_uint64->begin(); value_iter != proto_uint64->end(); ++value_iter )
            {
                auto child_data = child_array_data->Find( value_iter->first );
                if ( child_data == nullptr )
                {
                    child_data = KFDataFactory::Instance()->AddArray( child_array_data );
                }
                child_data->Set( value_iter->second );
            }
        }

        // object_data
        auto proto_object = &message->pbobject();
        for ( auto iter = proto_object->begin(); iter != proto_object->end(); ++iter )
        {
            auto child_object_data = object_data->Find( iter->first );
            if ( child_object_data == nullptr )
            {
                __LOG_CRITICAL__( "playerdata parent=[{}] not object=[{}]", object_data->_data_setting->_name, iter->first );
                continue;
            }

            CopyFromObject( child_object_data, &iter->second );
        }

        // kfrecord
        auto proto_record = &message->pbrecord();
        for ( auto iter = proto_record->begin(); iter != proto_record->end(); ++iter )
        {
            auto child_record_data = object_data->Find( iter->first );
            if ( child_record_data == nullptr )
            {
                __LOG_CRITICAL__( "playerdata parent=[{}] not record=[{}]", object_data->_data_setting->_name, iter->first );
                continue;
            }

            // 对象列表
            auto proto_object = &( iter->second.pbobject() );
            for ( auto object_iter = proto_object->begin(); object_iter != proto_object->end(); ++object_iter )
            {
                auto child_object_data = KFDataFactory::Instance()->CreateData( child_record_data->_data_setting, false );
                CopyFromObject( child_object_data, &object_iter->second );
                object_data->Add( object_iter->first, child_object_data );
            }
        }
    }

#define __SAVE_TO_PROTO__( data_type, data_name, type )\
case data_type:\
    {\
        auto& data_name = *(message->mutable_##data_name()); \
        data_name[child_data_setting->_name] = child->Get<type>();\
        break; \
    }\


    void KFKernelModule::SaveToEntity( DataPtr object_data, KFMsg::PBObject* message, uint32 data_mask, bool online, uint32 delay_time )
    {
        auto data_setting = object_data->_data_setting;
        if ( !data_setting->HaveMask( data_mask ) )
        {
            return;
        }

        SaveToObject( object_data, message, data_mask, online, delay_time );
    }

    void KFKernelModule::SaveToObject( DataPtr object_data, KFMsg::PBObject* message, uint32 data_mask, bool online /* = false */, uint32 delay_time /* = 0u */ )
    {
        for ( auto child = object_data->First(); child != nullptr; child = object_data->Next() )
        {
            auto child_data_setting = child->_data_setting;
            if ( !child_data_setting->HaveMask( data_mask ) ||
                    !child->IsValid() ||
                    ( online && child_data_setting->_delay_online_sync_time != delay_time ) )
            {
                continue;
            }

            switch ( child_data_setting->_type )
            {
                __SAVE_TO_PROTO__( KFDataDefine::DataTypeInt32, pbint32, int32 );
                __SAVE_TO_PROTO__( KFDataDefine::DataTypeUInt32, pbuint32, uint32 );
                __SAVE_TO_PROTO__( KFDataDefine::DataTypeInt64, pbint64, int64 );
                __SAVE_TO_PROTO__( KFDataDefine::DataTypeUInt64, pbuint64, uint64 );
                __SAVE_TO_PROTO__( KFDataDefine::DataTypeDouble, pbdouble, double );
                __SAVE_TO_PROTO__( KFDataDefine::DataTypeString, pbstring, std::string );

            case KFDataDefine::DataTypeArray:
            {
                auto& proto_array = ( *message->mutable_pbarray() )[child_data_setting->_name];

                auto max_size = child->MaxSize();
                for ( auto i = KFGlobal::Instance()->_array_index; i < max_size; ++i )
                {
                    auto value_data = child->Find( i );
                    if ( value_data != nullptr && value_data->IsValid() )
                    {
                        ( *proto_array.mutable_pbuint64() )[i] = value_data->Get();
                    }
                }
                break;
            }
            case KFDataDefine::DataTypeObject:
            {
                auto& proto_object = ( *message->mutable_pbobject() )[child_data_setting->_name];
                SaveToObject( child, &proto_object, data_mask );
                break;
            }
            case KFDataDefine::DataTypeRecord:
            {
                auto& proto_record = ( *message->mutable_pbrecord() )[child_data_setting->_name];
                for ( auto child_object_data = child->First(); child_object_data != nullptr; child_object_data = child->Next() )
                {
                    auto& proto_object = ( *proto_record.mutable_pbobject() )[child_object_data->GetKeyID()];
                    SaveToObject( child_object_data, &proto_object, data_mask );
                }
                break;
            }
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
}