#include "KFData.h"

namespace KFrame
{
    KFData::KFData()
    {
        _parent = nullptr;
        _class_setting = nullptr;
        _data_setting = nullptr;
    }

    KFData::~KFData()
    {
        _parent = nullptr;
        _data_setting = nullptr;
        _class_setting = nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////
    void KFData::Initialize( const KFClassSetting* classsetting, const KFDataSetting* datasetting )
    {
        _data_setting = datasetting;
        _class_setting = classsetting;

        FromString( datasetting->_init_value );
    }

    const std::string& KFData::GetName() const
    {
        return _data_setting->_name;
    }

    uint32 KFData::GetType() const
    {
        return _data_setting->_type;
    }

    const KFDataSetting* KFData::GetDataSetting() const
    {
        return _data_setting;
    }

    const KFClassSetting* KFData::GetClassSetting() const
    {
        return _class_setting;
    }

    bool KFData::HaveFlagMask( uint32 mask )
    {
        return _data_setting->HaveFlagMask( mask );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////
}