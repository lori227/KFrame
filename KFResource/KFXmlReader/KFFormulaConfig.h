#ifndef __KF_FORMULA_CONFIG_H__
#define __KF_FORMULA_CONFIG_H__

#include "KFConfig.h"
#include "KFParam.h"

namespace KFrame
{
    class KFFormulaParam
    {
    public:
        KFParam* AddParam()
        {
            auto param = __KF_NEW__( KFParam );
            _params.Add( param );
            return param;
        }

    public:
        // 类型
        std::string _type;

        // 参数列表
        KFVector< KFParam > _params;
    };

    class KFFormulaSetting : public KFIntSetting
    {
    public:
        // 公式列表
        KFVector< KFFormulaParam > _formula_param_list;

    public:
        const KFFormulaParam* GetFormulaParam() const
        {
            return _formula_param_list._objects.front();
        }

        template< class T >
        const KFFormulaParam* GetFormulaParam( uint32 paramindex, T value ) const
        {
            for ( auto formulaparam : _formula_param_list._objects )
            {
                auto param = formulaparam->_params._objects[ paramindex ]->_int_value;
                if ( param == value )
                {
                    return formulaparam;
                }
            }

            return nullptr;
        }
    };

    template<>
    inline const KFFormulaParam* KFFormulaSetting::GetFormulaParam( uint32 paramindex, double value ) const
    {
        for ( auto formulaparam : _formula_param_list._objects )
        {
            auto param = formulaparam->_params._objects[ paramindex ]->_double_value;
            if ( param == value )
            {
                return formulaparam;
            }
        }

        return nullptr;
    }

    template<>
    inline const KFFormulaParam* KFFormulaSetting::GetFormulaParam( uint32 paramindex, const std::string& value ) const
    {
        for ( auto formulaparam : _formula_param_list._objects )
        {
            auto param = formulaparam->_params._objects[ paramindex ]->_str_value;
            if ( param == value )
            {
                return formulaparam;
            }
        }

        return nullptr;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    class KFFormulaConfig : public KFConfigT< KFFormulaSetting >, public KFInstance< KFFormulaConfig >
    {
    public:
        KFFormulaConfig()
        {
            _key_name = "FormulaId";
            _file_name = "formula";
        }

        const KFFormulaParam* FindFormulaParam( uint32 id ) const
        {
            auto kfsetting = FindSetting( id );
            if ( kfsetting == nullptr )
            {
                return nullptr;
            }

            return kfsetting->GetFormulaParam();
        }

        template< class T >
        const KFFormulaParam* FindFormulaParam( uint32 id, uint32 paramindex, const T& value ) const
        {
            auto kfsetting = FindSetting( id );
            if ( kfsetting == nullptr )
            {
                return nullptr;
            }

            return kfsetting->GetFormulaParam< T >( paramindex, value );
        }

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFFormulaSetting* kfsetting )
        {
            auto kfformulaparam = __KF_NEW__( KFFormulaParam );
            kfsetting->_formula_param_list.Add( kfformulaparam );
            /////////////////////////////////////////////////////////////////////////////////
            kfformulaparam->_type = xmlnode.GetString( "Type" );

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
                auto param = kfformulaparam->AddParam();
                param->_str_value = strvalue;
                param->_int_value = __TO_UINT32__( strvalue );
                param->_double_value = __TO_DOUBLE__( strvalue );
            }
        }
    };
}

#endif