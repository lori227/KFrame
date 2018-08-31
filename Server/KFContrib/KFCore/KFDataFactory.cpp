#include "KFDataFactory.h"
#include "KFInt32.h"
#include "KFUInt32.h"
#include "KFInt64.h"
#include "KFUInt64.h"
#include "KFDouble.h"
#include "KFString.h"
#include "KFVector3D.h"
#include "KFObject.h"
#include "KFRecord.h"
#include "KFArray.h"
#include "KFDataConfig.h"

namespace KFrame
{
    KFData* KFDataFactory::Create( const std::string& type )
    {
        auto inttype = KFDataDefine::ConvertDataType( type );
        return Create( inttype );
    }

    KFData* KFDataFactory::Create( uint32 type )
    {
        switch ( type )
        {
        case KFDataDefine::Type_Int32:
            return __KF_CREATE_BATCH__( KFInt32, 50 );
            break;
        case KFDataDefine::Type_UInt32:
            return __KF_CREATE_BATCH__( KFUInt32, 50 );
            break;
        case KFDataDefine::Type_Int64:
            return __KF_CREATE_BATCH__( KFInt64, 50 );
            break;
        case KFDataDefine::Type_UInt64:
            return __KF_CREATE_BATCH__( KFUInt64, 50 );
            break;
        case KFDataDefine::Type_Double:
            return __KF_CREATE_BATCH__( KFDouble, 50 );
            break;
        case KFDataDefine::Type_String:
            return __KF_CREATE_BATCH__( KFString, 50 );
            break;
        case KFDataDefine::Type_Vector3D:
            return __KF_CREATE_BATCH__( KFVector3D, 50 );
            break;
        case KFDataDefine::Type_Object:
            return __KF_CREATE_BATCH__( KFObject, 50 );
            break;
        case KFDataDefine::Type_Record:
            return __KF_CREATE_BATCH__( KFRecord, 50 );
            break;
        case KFDataDefine::Type_Array:
            return __KF_CREATE_BATCH__( KFArray, 50 );
            break;
        default:
            break;
        }

        return nullptr;
    }

    void KFDataFactory::Release( KFData* kfdata )
    {
        __KF_DESTROY__( KFData, kfdata );
    }

    KFData* KFDataFactory::CreateData( const std::string& classname, const std::string& dataname )
    {
        auto datasetting = _kf_data_config->FindDataSetting( classname, dataname );
        if ( datasetting == nullptr )
        {
            __LOG_ERROR__( KFLogEnum::System, "[{}:{}] datasetting can't find!", classname, dataname );
            return nullptr;
        }

        return CreateData( datasetting );
    }

    KFData* KFDataFactory::CreateData( const KFDataSetting* datasetting )
    {
        auto classsetting = _kf_data_config->FindClassSetting( datasetting->_contain_class );
        if ( classsetting == nullptr )
        {
            __LOG_ERROR__( KFLogEnum::System, "[{}:{}] dataclass can't find!", datasetting->_name, datasetting->_contain_class );
            return nullptr;
        }

        auto kfdata = KFDataFactory::Create( KFDataDefine::Type_Object );
        InitData( kfdata, classsetting, datasetting );
        return kfdata;
    }

    void KFDataFactory::InitData( KFData* kfdata, const KFClassSetting* classsetting, const KFDataSetting* datasetting )
    {
        kfdata->Initialize( classsetting, datasetting );

        for ( auto& iter : classsetting->_static_data )
        {
            auto childdatasetting = &iter.second;

            KFData* kfchilddata = nullptr;
            switch ( childdatasetting->_type )
            {
            case KFDataDefine::Type_Object:	// 对象需要创建他的子属性
                kfchilddata = CreateData( childdatasetting );
                break;
            default:	// 其他类型直接创建
                kfchilddata = KFDataFactory::Create( childdatasetting->_type );
                break;
            }

            if ( kfchilddata == nullptr )
            {
                __LOG_ERROR__( KFLogEnum::System, "create data type error[{}:{}]!", childdatasetting->_name, childdatasetting->_type );
                continue;
            }

            auto initsetting = classsetting;
            if ( !childdatasetting->_contain_class.empty() )
            {
                auto tempsetting = _kf_data_config->FindClassSetting( childdatasetting->_contain_class );
                if ( tempsetting != nullptr )
                {
                    initsetting = tempsetting;
                }
            }

            kfchilddata->Initialize( initsetting, childdatasetting );
            kfdata->AddData( childdatasetting->_name, kfchilddata );
        }
    }

}