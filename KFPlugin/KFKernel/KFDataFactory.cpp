#include "KFDataFactory.hpp"
#include "KFCore/KFInt32.h"
#include "KFCore/KFUInt32.h"
#include "KFCore/KFInt64.h"
#include "KFCore/KFUInt64.h"
#include "KFCore/KFDouble.h"
#include "KFCore/KFString.h"
#include "KFCore/KFObject.h"
#include "KFCore/KFRecord.h"
#include "KFCore/KFArray.h"
#include "KFCore/KFVector3D.h"
#include "KFKernelModule.hpp"

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
            return __KF_NEW__( KFInt32 );
            break;
        case KFDataDefine::Type_UInt32:
            return __KF_NEW__( KFUInt32 );
            break;
        case KFDataDefine::Type_Int64:
            return __KF_NEW__( KFInt64 );
            break;
        case KFDataDefine::Type_UInt64:
            return __KF_NEW__( KFUInt64 );
            break;
        case KFDataDefine::Type_Double:
            return __KF_NEW__( KFDouble );
            break;
        case KFDataDefine::Type_String:
            return __KF_NEW__( KFString );
            break;
        case KFDataDefine::Type_Vector3D:
            return __KF_NEW__( KFVector3D );
            break;
        case KFDataDefine::Type_Object:
            return __KF_NEW__( KFObject );
            break;
        case KFDataDefine::Type_Record:
            return __KF_NEW__( KFRecord );
            break;
        case KFDataDefine::Type_Array:
            return __KF_NEW__( KFArray );
            break;
        default:
            break;
        }

        return nullptr;
    }

    void KFDataFactory::Release( KFData* kfdata )
    {
        __KF_DELETE__( KFData, kfdata );
    }

    KFData* KFDataFactory::CreateData( const std::string& classname, const std::string& dataname )
    {
        auto datasetting = KFDataConfig::Instance()->FindDataSetting( classname, dataname );
        if ( datasetting == nullptr )
        {
            __LOG_ERROR__( "[{}:{}] datasetting can't find!", classname, dataname );
            return nullptr;
        }

        return CreateData( datasetting );
    }

    KFData* KFDataFactory::CreateData( const KFDataSetting* datasetting )
    {
        if ( datasetting->_class_setting == nullptr )
        {
            __LOG_ERROR__( "[{}:{}] dataclass can't find!", datasetting->_name, datasetting->_contain_class );
            return nullptr;
        }

        auto kfdata = KFDataFactory::Create( datasetting->_logic_type );
        InitData( kfdata, datasetting->_class_setting, datasetting );
        return kfdata;
    }

    void KFDataFactory::InitData( KFData* kfdata, const KFClassSetting* classsetting, const KFDataSetting* datasetting )
    {
        kfdata->Initialize( classsetting, datasetting );

        for ( auto& iter : classsetting->_static_data._objects )
        {
            auto childdatasetting = iter.second;
            if ( !childdatasetting->HaveMask( KFDataDefine::Mask_Create ) )
            {
                continue;
            }

            KFData* kfchilddata = nullptr;
            if ( childdatasetting->_type == KFDataDefine::Type_Object )
            {
                kfchilddata = CreateData( childdatasetting );
            }
            else
            {
                kfchilddata = KFDataFactory::Create( childdatasetting->_type );
            }

            if ( kfchilddata == nullptr )
            {
                __LOG_ERROR__( "create data type error[{}:{}]!", childdatasetting->_name, childdatasetting->_type );
                continue;
            }

            auto initclasssetting = childdatasetting->_class_setting != nullptr ? childdatasetting->_class_setting : classsetting;
            kfchilddata->Initialize( initclasssetting, childdatasetting );
            if ( childdatasetting->_type == KFDataDefine::Type_Array )
            {
                for ( uint32 i = KFDataDefine::Array_Index; i < kfchilddata->Size(); ++i )
                {
                    auto kfarraydata = KFDataFactory::Create( childdatasetting->_contain_class );
                    kfarraydata->Initialize( initclasssetting, childdatasetting );
                    kfchilddata->AddData( i, kfarraydata );
                }
            }

            kfdata->AddData( childdatasetting->_name, kfchilddata );
        }
    }
}