#ifndef __KF_ITEM_USE_INTERFACE_H__
#define __KF_ITEM_USE_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFItemSetting;
    class KFItemTypeSetting;
    typedef std::function< bool( KFEntity*, KFData*, const KFItemSetting*, const KFItemTypeSetting* ) > KFItemUseFunction;
    //////////////////////////////////////////////////////////////////////////
    class KFItemUseInterface : public KFModule
    {
    public:
        // 注册检查道具是否可以使用函数
        template< class T >
        void RegisteCheckItemUseFunction( uint32 itemtype, T* object, bool ( T::*handle )( KFEntity*, KFData*, const KFItemSetting*, const KFItemTypeSetting* ) )
        {
            KFItemUseFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 );
            BindCheckItemUseFunction( itemtype, typeid( T ).name(), function );
        }

        template< class T >
        void UnRegisteCheckItemUseFunction( T* object, uint32 itemtype )
        {
            UnBindCheckItemUseFunction( itemtype, typeid( T ).name() );
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 注册道具使用函数
        template< class T >
        void RegisteItemUseFunction( uint32 itemtype, T* object, bool ( T::*handle )( KFEntity*, KFData*, const KFItemSetting*, const KFItemTypeSetting* ) )
        {
            KFItemUseFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 );
            BindItemUseFunction( itemtype, typeid( T ).name(), function );
        }

        template< class T >
        void UnRegisteItemUseFunction( T* object, uint32 itemtype )
        {
            UnBindItemUseFunction( itemtype, typeid( T ).name() );
        }

    protected:
        virtual void BindCheckItemUseFunction( uint32 itemtype, const std::string& module, KFItemUseFunction& function ) = 0;
        virtual void UnBindCheckItemUseFunction( uint32 itemtype, const std::string& module ) = 0;

        virtual void BindItemUseFunction( uint32 itemtype, const std::string& module, KFItemUseFunction& function ) = 0;
        virtual void UnBindItemUseFunction( uint32 itemtype, const std::string& module ) = 0;
    };
    __KF_INTERFACE__( _kf_item_use, KFItemUseInterface );

    ////////////////////////////////////////////////////////////////////////////////////////////
#define __KF_ITEM_USE_FUNCTION__( function )\
    void function( KFEntity* player, KFData* kfitem, const KFItemSetting* kfitemsetting, const KFItemTypeSetting* kftypesetting )
#define __REGISTER_ITEM_USE__( itemtype, function )\
    _kf_item->RegisteItemUseFunction( itemtype, this, function )
#define __UN_ITEM_USE__( itemtype )\
    _kf_item->UnRegisteItemUseFunction( this, itemtype )

#define __KF_CHECK_ITEM_USE_FUNCTION__( function )\
    void function( KFEntity* player, KFData* kfitem, const KFItemSetting* kfitemsetting, const KFItemTypeSetting* kftypesetting )
#define __REGISTER_CHECK_ITEM_USE__( itemtype, function )\
    _kf_item->RegisteItemUseFunction( itemtype, this, function )
#define __UN_CHECK_ITEM_USE__( itemtype )\
    _kf_item->UnRegisteItemUseFunction( this, itemtype )
}

#endif