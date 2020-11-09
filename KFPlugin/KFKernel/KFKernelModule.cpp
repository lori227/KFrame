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
    KFComponent* KFKernelModule::FindComponent( const std::string& dataname )
    {
        auto kfcomponent = _kf_component.Find( dataname );
        if ( kfcomponent == nullptr )
        {
            kfcomponent = _kf_component.Create( dataname );
            kfcomponent->_component_name = dataname;
            kfcomponent->_data_setting = KFDataConfig::Instance()->FindDataSetting( _global_class, dataname );
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
    KFData* KFKernelModule::CreateData( const KFDataSetting* datasetting )
    {
        return KFDataFactory::Instance()->CreateData( datasetting, true );
    }

    void KFKernelModule::DestroyData( KFData* kfdata )
    {
        KFDataFactory::Instance()->DestroyData( kfdata );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFKernelModule::ParseFromProto( KFData* kfdata, const KFMsg::PBObject* proto )
    {
        CopyFromObject( kfdata, proto );
        return true;
    }

    KFMsg::PBObject* KFKernelModule::Serialize( KFData* kfdata )
    {
        return SerializeObject( kfdata, KFDataDefine::DataMaskNull, false, 0u );
    }

    KFMsg::PBObject* KFKernelModule::SerializeToData( KFData* kfdata )
    {
        return SerializeObject( kfdata, KFDataDefine::DataMaskSave, false, 0u );
    }

    KFMsg::PBObject* KFKernelModule::SerializeToView( KFData* kfdata )
    {
        return SerializeObject( kfdata, KFDataDefine::DataMaskView, false, 0u );
    }

    KFMsg::PBObject* KFKernelModule::SerializeToClient( KFData* kfdata )
    {
        return SerializeObject( kfdata, KFDataDefine::DataMaskSync, false, 0u );
    }

    KFMsg::PBObject* KFKernelModule::SerializeToOnline( KFEntity* kfentity, uint32 delaytime /* = 0u */ )
    {
        // 把直接发送的数据返回给客户端
        return SerializeObject( kfentity, KFDataDefine::DataMaskSync, true, delaytime );
    }

    KFMsg::PBObject* KFKernelModule::SerializeObject( KFData* kfdata, uint32 datamask, bool online, uint32 delaytime )
    {
        static KFMsg::PBObject pbobject;
        pbobject.Clear();

        SaveToEntity( kfdata, &pbobject, datamask, online, delaytime );
        return &pbobject;
    }

#define __COPY_FROM_PROTO__( kfdata, proto, pbdata ) \
    {\
        auto pbdata = &(proto->pbdata());\
        for ( auto iter = pbdata->begin(); iter != pbdata->end(); ++iter )\
        {\
            auto kfchild = kfdata->Find( iter->first );\
            if ( kfchild != nullptr )\
            {\
                kfchild->Set( iter->second );\
            }\
            else\
            {\
                __LOG_CRITICAL__( "playerdata parent=[{}] not data=[{}]", kfdata->_data_setting->_name, iter->first);\
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
            auto kfarray = kfdata->Find( iter->first );
            if ( kfarray == nullptr )
            {
                __LOG_CRITICAL__( "playerdata parent=[{}] not array=[{}]", kfdata->_data_setting->_name, iter->first );
                continue;
            }

            // 数组列表
            auto pbuint64 = &iter->second.pbuint64();
            for ( auto citer = pbuint64->begin(); citer != pbuint64->end(); ++citer )
            {
                auto kfchild = kfarray->Find( citer->first );
                if ( kfchild == nullptr )
                {
                    kfchild = KFDataFactory::Instance()->AddArray( kfarray );
                }
                kfchild->Set<int64>( citer->second );
            }
        }

        // kfobject
        auto pbobject = &proto->pbobject();
        for ( auto iter = pbobject->begin(); iter != pbobject->end(); ++iter )
        {
            auto kfobject = kfdata->Find( iter->first );
            if ( kfobject == nullptr )
            {
                __LOG_CRITICAL__( "playerdata parent=[{}] not object=[{}]", kfdata->_data_setting->_name, iter->first );
                continue;
            }

            CopyFromObject( kfobject, &iter->second );
        }

        // kfrecord
        auto pbrecord = &proto->pbrecord();
        for ( auto iter = pbrecord->begin(); iter != pbrecord->end(); ++iter )
        {
            auto kfrecord = kfdata->Find( iter->first );
            if ( kfrecord == nullptr )
            {
                __LOG_CRITICAL__( "playerdata parent=[{}] not record=[{}]", kfdata->_data_setting->_name, iter->first );
                continue;
            }

            // 对象列表
            auto pbobject = &( iter->second.pbobject() );
            for ( auto citer = pbobject->begin(); citer != pbobject->end(); ++citer )
            {
                auto kfobject = KFDataFactory::Instance()->CreateData( kfrecord->_data_setting, false );
                CopyFromObject( kfobject, &citer->second );
                kfrecord->Add( citer->first, kfobject );
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


    void KFKernelModule::SaveToEntity( KFData* kfdata, KFMsg::PBObject* proto, uint32 datamask, bool online, uint32 delaytime )
    {
        auto datasetting = kfdata->_data_setting;
        if ( !datasetting->HaveMask( datamask ) )
        {
            return;
        }

        SaveToObject( kfdata, proto, datamask, online, delaytime );
    }

    void KFKernelModule::SaveToObject( KFData* kfdata, KFMsg::PBObject* proto, uint32 datamask, bool online /* = false */, uint32 delaytime /* = 0u */ )
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
                __SAVE_TO_PROTO__( KFDataDefine::DataTypeUInt64, pbuint64, uint64 );
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
                auto& pbobject = ( *proto->mutable_pbobject() )[ datasetting->_name ];
                SaveToObject( kfchild, &pbobject, datamask );
                break;
            }
            case KFDataDefine::DataTypeRecord:
            {
                auto& pbrecord = ( *proto->mutable_pbrecord() )[ datasetting->_name ];
                for ( auto kfobject = kfchild->First(); kfobject != nullptr; kfobject = kfchild->Next() )
                {
                    auto& pbobject = ( *pbrecord.mutable_pbobject() )[ kfobject->GetKeyID() ];
                    SaveToObject( kfobject, &pbobject, datamask );
                }
                break;
            }
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
}