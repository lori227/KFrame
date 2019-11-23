#ifndef __KF_FORMULA_CONFIG_H__
#define __KF_FORMULA_CONFIG_H__

#include "KFConfig.h"
#include "KFParam.h"

namespace KFrame
{
    class KFFormulaSetting : public KFIntSetting
    {
    public:
        ~KFFormulaSetting()
        {
            Reset();
        }

        void Reset()
        {
            for ( auto param : _params )
            {
                __KF_DELETE__( KFParam, param );
            }
            _params.clear();
        }

        KFParam* AddParam()
        {
            auto param = __KF_NEW__( KFParam );
            _params.push_back( param );
            return param;
        }

    public:
        // 类型
        std::string _type;
        std::vector< KFParam* > _params;
    };
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    class KFFormulaConfig : public KFConfigT< KFFormulaSetting >, public KFInstance< KFFormulaConfig >
    {
    public:
        KFFormulaConfig()
        {
            _file_name = "formula";
        }

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFFormulaSetting* kfsetting )
        {
            kfsetting->_params.clear();
            kfsetting->_type = xmlnode.GetString( "Type" );

            auto index = 1u;
            while ( true )
            {
                auto strkey = __FORMAT__( "Param{}", index++ );
                auto ok = xmlnode.HaveChild( strkey.c_str() );
                if ( !ok )
                {
                    break;
                }

                auto strvalue = xmlnode.GetString( strkey.c_str() );
                auto param = kfsetting->AddParam();
                param->_str_value = strvalue;
                param->_int_value = KFUtility::ToValue<uint32>( strvalue );
                param->_double_value = KFUtility::ToValue<double>( strvalue );
            }
        }
    };
}

#endif