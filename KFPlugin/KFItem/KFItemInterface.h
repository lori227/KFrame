#ifndef __KF_ITEM_INTERFACE_H__
#define __KF_ITEM_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFItemSetting;
    typedef std::function< void( KFEntity*, KFData*, const KFItemSetting* ) > KFItemInitFunction;

    class KFItemInterface : public KFModule
    {
    public:
        // 背包格子最大数量
        virtual uint32 GetItemBagMaxCount( KFEntity* player, KFData* kfitemrecord ) = 0;

        // 创建一个道具( 可传入一个存在的kfitem, 直接返回 )
        virtual KFData* CreateItem( KFEntity* player, uint32 itemid, KFData* kfitem, const char* function, uint32 line ) = 0;

        // 获取能添加物品数量
        virtual uint32 GetCanAddItemCount( KFEntity* player, uint32 itemid, uint32 count ) = 0;

        // 查找道具  返回<背包指针, 道具指针>
        virtual std::tuple<KFData*, KFData*> FindItem( KFEntity* player, uint64 itemuuid ) = 0;

        // 获得背包空格子数量
        virtual uint32 GetItemEmptyCount( KFEntity* player, KFData* kfitemrecord ) = 0;

        // 判断包裹是否满了
        virtual bool CheckItemBagFull( KFEntity* player, KFData* kfitemrecord ) = 0;

        // 获得包裹内道具数量( maxcount 最多查找数量, 优化用 )
        virtual uint32 GetItemCount( KFEntity* player, KFData* kfitemrecord, uint32 itemid, uint32 maxcount = __MAX_UINT32__ ) = 0;

        // 删除包裹内道具
        virtual void RemoveItem( KFEntity* player, KFData* kfitemrecord, uint32 itemid, uint32 itemcount ) = 0;
        virtual uint32 RemoveItemCount( KFEntity* player, KFData* kfitem, uint32 itemcount ) = 0;

        // 获得背包属性
        virtual KFData* FindItemBag( KFEntity* player, uint32 itemid ) = 0;
        virtual KFData* FindItemBag( KFEntity* player, KFData* kfitem ) = 0;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 注册道具初始化函数
        template< class T >
        void RegisteItemInitFunction( uint32 itemtype, T* object, void ( T::*handle )( KFEntity*, KFData*, const KFItemSetting* ) )
        {
            KFItemInitFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
            BindItemInitFunction( itemtype, typeid( T ).name(), function );
        }

        template< class T >
        void UnRegisteItemInitFunction( T* object, uint32 itemtype )
        {
            UnBindItemInitFunction( itemtype, typeid( T ).name() );
        }

    protected:
        virtual void BindItemInitFunction( uint32 itemtype, const std::string& module, KFItemInitFunction& function ) = 0;
        virtual void UnBindItemInitFunction( uint32 itemtype, const std::string& module ) = 0;
    };
    __KF_INTERFACE__( _kf_item, KFItemInterface );

#define __KF_ITEM_INIT_FUNCTION__( function )\
    void function( KFEntity* player, KFData* kfitem, const KFItemSetting* kfsetting )
#define __REGISTER_ITEM_INIT__( itemtype, function )\
    _kf_item->RegisteItemInitFunction( itemtype, this, function )
#define __UN_ITEM_INIT__( itemtype )\
    _kf_item->UnRegisteItemInitFunction( this, itemtype )


}

#endif