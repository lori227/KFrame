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
        auto data_type = KFDataDefine::ConvertDataType( type );
        return Create( data_type );
    }

    DataPtr KFDataFactory::Create( uint32 data_type )
    {
        switch ( data_type )
        {
        case KFDataDefine::DataTypeInt32:
            return __MAKE_SHARED__( KFInt32 );
            break;
        case KFDataDefine::DataTypeUInt32:
            return __MAKE_SHARED__( KFUInt32 );
            break;
        case KFDataDefine::DataTypeInt64:
            return __MAKE_SHARED__( KFInt64 );
            break;
        case KFDataDefine::DataTypeUInt64:
            return __MAKE_SHARED__( KFUInt64 );
            break;
        case KFDataDefine::DataTypeDouble:
            return __MAKE_SHARED__( KFDouble );
            break;
        case KFDataDefine::DataTypeString:
            return __MAKE_SHARED__( KFString );
            break;
        case KFDataDefine::DataTypeVector3D:
            return __MAKE_SHARED__( KFVector3D );
            break;
        case KFDataDefine::DataTypeObject:
            return __MAKE_SHARED__( KFObject );
            break;
        case KFDataDefine::DataTypeRecord:
            return __MAKE_SHARED__( KFRecord );
            break;
        case KFDataDefine::DataTypeArray:
            return __MAKE_SHARED__( KFArray );
            break;
        default:
            break;
        }

        return nullptr;
    }

    void KFDataFactory::DestroyData( DataPtr data )
    {
        RemoveDestroyData( data );
        DestroyToDataPool( data );
    }

    DataPtr KFDataFactory::CreateData( std::shared_ptr<const KFDataSetting> data_setting, bool add_auto_list )
    {
        auto data = CreateFromDataPool( data_setting );
        if ( data == nullptr )
        {
            data = Create( data_setting->_logic_type );
            InitCreateData( data, data_setting );
        }

        if ( add_auto_list )
        {
            AddDestroyData( data );
        }

        return data;
    }

    DataPtr KFDataFactory::CreateFromDataPool( std::shared_ptr<const KFDataSetting> data_setting )
    {
#ifdef __USE_DATA_POOL__
        auto iter = _data_pool.find( data_setting->_contain_class );
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
        auto data = *( begin );
        datalist.erase( data );

        // 初始化数据
        InitPoolData( data );
        return data;
#else
        return nullptr;
#endif
    }

    void KFDataFactory::InitPoolData( DataPtr data )
    {
        data->SetParent( nullptr );
        data->SetKeyID( _invalid_int );

        for ( auto& iter : data->_data_setting->_class_setting->_static_data._objects )
        {
            auto child_data_setting = iter.second;

            // 判断是否存在
            auto child = data->Find( child_data_setting->_name );
            if ( child != nullptr )
            {
                child->Reset();
            }
            else
            {
                if ( !child_data_setting->HaveMask( KFDataDefine::DataMaskShield ) )
                {
                    // 没有就创建出来
                    child = CreateData( child_data_setting, false );
                    data->Add( child_data_setting->_name, child );
                }
            }
        }
    }

    void KFDataFactory::DestroyToDataPool( DataPtr data )
    {
#ifdef __USE_DATA_POOL__
        DestroyObjectToDataPool( data );

        // 保存在对象池
        _data_pool[data->_data_setting->_contain_class].insert( data );
#endif
    }

    void KFDataFactory::DestroyObjectToDataPool( DataPtr data )
    {
        // 把record分离出来
        for ( auto child = data->First(); child != nullptr; child = data->Next() )
        {
            switch ( child->_data_type )
            {
            case KFDataDefine::DataTypeObject:
                DestroyObjectToDataPool( child );
                break;
            case KFDataDefine::DataTypeRecord:
                DestroyRecordToDataPool( child );
                break;
            }
        }
    }

    void KFDataFactory::DestroyRecordToDataPool( DataPtr data )
    {
        for ( auto child = data->First(); child != nullptr; child = data->Next() )
        {
            DestroyToDataPool( child );
        }

        // 清空列表
        data->Reset();
    }

    void KFDataFactory::ClearDataPool()
    {
        _data_pool.clear();
    }

    void KFDataFactory::InitCreateData( DataPtr data, std::shared_ptr<const KFDataSetting> data_setting )
    {
        data->InitSetting( data_setting );
        if ( data_setting->_class_setting == nullptr )
        {
            return;
        }

        // 如果是对象, 需要初始化子属性
        for ( auto& iter : data_setting->_class_setting->_static_data._objects )
        {
            auto child_data_setting = iter.second;
            if ( child_data_setting->HaveMask( KFDataDefine::DataMaskShield ) )
            {
                continue;
            }

            auto child = Create( child_data_setting->_type );
            if ( child == nullptr )
            {
                __LOG_ERROR__( "create data type error[{}:{}]", child_data_setting->_name, child_data_setting->_type );
                continue;
            }

            child->InitSetting( child_data_setting );
            data->Add( child_data_setting->_name, child );

            // 初始化
            switch ( child_data_setting->_type )
            {
            case KFDataDefine::DataTypeObject:
                InitCreateData( child, child_data_setting );
                break;
            case KFDataDefine::DataTypeArray:
                InitArray( child, child_data_setting->_int_max_capacity );
                break;
            default:
                break;
            }
        }
    }

    void KFDataFactory::InitArray( DataPtr data, uint32 size )
    {
        size += KFGlobal::Instance()->_array_index;
        data->Resize( size );
        for ( auto i = KFGlobal::Instance()->_array_index; i < size; ++i )
        {
            auto child = Create( data->_data_setting->_contain_class );
            child->InitSetting( data->_data_setting );
            data->Add( i, child );
        }
    }

    DataPtr KFDataFactory::AddArray( DataPtr data )
    {
        auto child = Create( data->_data_setting->_contain_class );
        child->InitSetting( data->_data_setting );
        data->Add( data->MaxSize(), child );
        return child;
    }

    void KFDataFactory::AddDestroyData( DataPtr data )
    {
        _auto_destroy_list.insert( data );
    }

    void KFDataFactory::RemoveDestroyData( DataPtr data )
    {
        _auto_destroy_list.erase( data );
    }

    void KFDataFactory::RunAutoDestroyData()
    {
        if ( _auto_destroy_list.empty() )
        {
            return;
        }

        for ( auto data : _auto_destroy_list )
        {
            // 如果数据没有被引用, 则释放掉
            if ( data->GetParent() == nullptr )
            {
                DestroyToDataPool( data );
            }
        }
        _auto_destroy_list.clear();
    }
}