#include "KFDataFactory.hpp"
#include "KFCore/KFObject.h"
#include "KFCore/KFInt32.hpp"
#include "KFCore/KFUInt32.hpp"
#include "KFCore/KFInt64.hpp"
#include "KFCore/KFUInt64.hpp"
#include "KFCore/KFDouble.hpp"
#include "KFCore/KFString.hpp"
#include "KFCore/KFRecord.hpp"
#include "KFCore/KFArray.hpp"
#include "KFCore/KFVector3D.hpp"
#include "KFDataConfig.hpp"
#include "KFKernelModule.hpp"

namespace KFrame
{
    KFDataFactory::KFDataFactory()
    {
    }

    KFDataFactory::~KFDataFactory()
    {
        ClearDataPool();
    }

    DataPtr KFDataFactory::Create( const std::string& type )
    {
        auto inttype = KFDataDefine::ConvertDataType( type );
        return Create( inttype );
    }

    DataPtr KFDataFactory::Create( uint32 type )
    {
        switch ( type )
        {
        case KFDataDefine::DataTypeInt32:
            return __KF_NEW__( KFInt32 );
            break;
        case KFDataDefine::DataTypeUInt32:
            return __KF_NEW__( KFUInt32 );
            break;
        case KFDataDefine::DataTypeInt64:
            return __KF_NEW__( KFInt64 );
            break;
        case KFDataDefine::DataTypeUInt64:
            return __KF_NEW__( KFUInt64 );
            break;
        case KFDataDefine::DataTypeDouble:
            return __KF_NEW__( KFDouble );
            break;
        case KFDataDefine::DataTypeString:
            return __KF_NEW__( KFString );
            break;
        case KFDataDefine::DataTypeVector3D:
            return __KF_NEW__( KFVector3D );
            break;
        case KFDataDefine::DataTypeObject:
            return __KF_NEW__( KFObject );
            break;
        case KFDataDefine::DataTypeRecord:
            return __KF_NEW__( KFRecord );
            break;
        case KFDataDefine::DataTypeArray:
            return __KF_NEW__( KFArray );
            break;
        default:
            break;
        }

        return nullptr;
    }

    void KFDataFactory::DestroyData( DataPtr kfdata )
    {
        RemoveDestroyData( kfdata );
        DestroyToDataPool( kfdata );
    }

    DataPtr KFDataFactory::CreateData( const KFDataSetting* datasetting, bool addautolist )
    {
        auto kfdata = CreateFromDataPool( datasetting );
        if ( kfdata == nullptr )
        {
            kfdata = Create( datasetting->_logic_type );
            InitCreateData( kfdata, datasetting );
        }

        if ( addautolist )
        {
            AddDestroyData( kfdata );
        }

        return kfdata;
    }

    DataPtr KFDataFactory::CreateFromDataPool( const KFDataSetting* datasetting )
    {
#ifdef __USE_DATA_POOL__
        auto iter = _data_pool.find( datasetting->_contain_class );
        if ( iter == _data_pool.end() )
        {
            return nullptr;
        }

        auto& datalist = iter->second;
        if ( datalist.empty() )
        {
            return nullptr;
        }

        auto begin = datalist.rbegin();
        auto kfdata = *( begin );
        datalist.erase( kfdata );

        // 初始化数据
        InitPoolData( kfdata );
        return kfdata;
#else
        return nullptr;
#endif
    }

    void KFDataFactory::InitPoolData( DataPtr kfdata )
    {
        kfdata->SetParent( nullptr );
        kfdata->SetKeyID( _invalid_int );

        for ( auto& iter : kfdata->_data_setting->_class_setting->_static_data._objects )
        {
            auto childdatasetting = iter.second;

            // 判断是否存在
            auto kfchild = kfdata->Find( childdatasetting->_name );
            if ( kfchild != nullptr )
            {
                kfchild->Reset();
            }
            else
            {
                if ( !childdatasetting->HaveMask( KFDataDefine::DataMaskShield ) )
                {
                    // 没有就创建出来
                    kfchild = CreateData( childdatasetting, false );
                    kfdata->Add( childdatasetting->_name, kfchild );
                }
            }
        }
    }

    void KFDataFactory::DestroyToDataPool( DataPtr kfdata )
    {
#ifdef __USE_DATA_POOL__
        DestroyObjectToDataPool( kfdata );

        // 保存在对象池
        _data_pool[ kfdata->_data_setting->_contain_class ].insert( kfdata );

#else
        __KF_DELETE__( KFData, kfdata );
#endif
    }

    void KFDataFactory::DestroyObjectToDataPool( DataPtr kfdata )
    {
        // 把record分离出来
        for ( auto kfchild = kfdata->First(); kfchild != nullptr; kfchild = kfdata->Next() )
        {
            switch ( kfchild->_data_type )
            {
            case KFDataDefine::DataTypeObject:
                DestroyObjectToDataPool( kfchild );
                break;
            case KFDataDefine::DataTypeRecord:
                DestroyRecordToDataPool( kfchild );
                break;
            }
        }
    }

    void KFDataFactory::DestroyRecordToDataPool( DataPtr kfdata )
    {
        for ( auto kfchild = kfdata->First(); kfchild != nullptr; kfchild = kfdata->Next() )
        {
            DestroyToDataPool( kfchild );
        }

        // 清空列表
        kfdata->Reset( false );
    }

    void KFDataFactory::ClearDataPool()
    {
        for ( auto& iter : _data_pool )
        {
            for ( auto kfdata : iter.second )
            {
                __KF_DELETE__( KFData, kfdata );
            }
        }
    }

    void KFDataFactory::InitCreateData( DataPtr kfdata, const KFDataSetting* datasetting )
    {
        kfdata->Initialize( datasetting );
        if ( datasetting->_class_setting == nullptr )
        {
            return;
        }

        // 如果是对象, 需要初始化子属性
        for ( auto& iter : datasetting->_class_setting->_static_data._objects )
        {
            auto childdatasetting = iter.second;
            if ( childdatasetting->HaveMask( KFDataDefine::DataMaskShield ) )
            {
                continue;
            }

            auto kfchilddata = Create( childdatasetting->_type );
            if ( kfchilddata == nullptr )
            {
                __LOG_ERROR__( "create data type error[{}:{}]", childdatasetting->_name, childdatasetting->_type );
                continue;
            }

            kfchilddata->Initialize( childdatasetting );
            kfdata->Add( childdatasetting->_name, kfchilddata );

            // 初始化
            switch ( childdatasetting->_type )
            {
            case KFDataDefine::DataTypeObject:
                InitCreateData( kfchilddata, childdatasetting );
                break;
            case KFDataDefine::DataTypeArray:
                InitArray( kfchilddata, childdatasetting->_int_max_capacity );
                break;
            default:
                break;
            }
        }
    }

    void KFDataFactory::InitArray( DataPtr array_data, uint32 size )
    {
        size += KFGlobal::Instance()->_array_index;
        array_data->Resize( size );
        for ( auto i = KFGlobal::Instance()->_array_index; i < size; ++i )
        {
            auto kfarraydata = Create( array_data->_data_setting->_contain_class );
            kfarraydata->Initialize( array_data->_data_setting );
            array_data->Add( i, kfarraydata );
        }
    }

    DataPtr KFDataFactory::AddArray( DataPtr array_data )
    {
        auto kfarraydata = Create( array_data->_data_setting->_contain_class );
        kfarraydata->Initialize( array_data->_data_setting );
        array_data->Add( array_data->MaxSize(), kfarraydata );
        return kfarraydata;
    }

    void KFDataFactory::AddDestroyData( DataPtr kfdata )
    {
        _auto_destroy_list.insert( kfdata );
    }

    void KFDataFactory::RemoveDestroyData( DataPtr kfdata )
    {
        _auto_destroy_list.erase( kfdata );
    }

    void KFDataFactory::RunAutoDestroyData()
    {
        if ( _auto_destroy_list.empty() )
        {
            return;
        }

        for ( auto kfdata : _auto_destroy_list )
        {
            // 如果数据没有被引用, 则释放掉
            if ( kfdata->GetParent() == nullptr )
            {
                DestroyToDataPool( kfdata );
            }
        }
        _auto_destroy_list.clear();
    }
}