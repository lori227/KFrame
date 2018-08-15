#ifndef __KF_STORE_CONFIG_H__
#define __KF_STORE_CONFIG_H__

#include "KFrame.h"
#include "KFCore/KFAgent.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    namespace KFStoreEnum
    {

        // 商城购买类型枚举
        enum EConstDefine
        {
            NoLimit = 0,				// 未限购
            LimitTotalCountSelf = 1,	// 限制周期内总限购
            limitDayCountSelf = 2,		// 限制周期内单日限购
        };

        //商店操作
        enum StoreOperaType
        {
            Buy = 1,					// 购买
            Give = 2,					// 赠送
        };
    }
    class KFStoreSetting
    {
    public:
        // 商品
        uint32 _id;

        // 商店类型
        uint32 _type;

        //道具类型
        uint32 _item_type;

        // 单次购买上限
        uint32 _buy_max_num;

        // 购买得到的道具
        KFAgents _buy_item;

        // 购买花费的道具
        KFMap< std::string, const std::string&, KFAgents > _cost_item;

        // 折扣后的价格
        KFMap< std::string, const std::string&, KFAgents> _discount_item;

        // 折扣的开始时间
        uint64 _start_discount_time;

        // 折扣的结束时间
        uint64 _end_discount_time;

        // 售卖的开始时间
        uint64 _start_buy_time;

        // 售卖的结束时间
        uint64 _end_buy_time;

        // 商城限制类型
        uint32 _limit_type;

        // 商城购买限制数量
        uint32 _limit_count;

        // 赠送的消耗
        std::string _cost_give;

        // 最大拥有限制数
        uint32 _max_owm_;

        // 赠送道具收到的邮件id
        uint32 _give_mail_id;

        // 赠送道具收到的感谢邮件id
        uint32 _return_mail_id;

        // 赠送物品好感度增加值
        uint32 _give_friend_liness;

        //判断参数列表合法（检测购买和赠送的消耗列表是否存在）
        bool CheckParam( const std::string& buytype, KFStoreEnum::StoreOperaType operatype, uint32 num, uint64 nowtime ) const;

        //获得购买消耗物品列表
        const KFAgents* FindBuyCostAgents( const std::string& buytype, uint64 nowtime, uint32 num ) const;

        //获取赠送消耗物品列表
        const KFAgents* FindGiveCostAgents( uint64 nowtime ) const;

        //检测道具是否超过限购日期
        bool CheckOutOfData( uint64 nowtime ) const;

        //检测是否是限购道具
        inline bool CheckLimit() const
        {
            return _limit_type != KFStoreEnum::NoLimit;
        }

        // 获取道具最大拥有数量
        inline uint32 GetMaxOwns() const
        {
            return _max_owm_;
        }

    private:
        //判断是否打折
        bool CheckDiscount( uint64 nowtime ) const;

        //判断是否可赠送
        bool CheckGive() const;

        //判断消耗参数合法
        bool CheckCostParam( const std::string& buytype, uint64 nowtime ) const;
    };

    class KFStoreConfig : public KFConfig, public KFSingleton< KFStoreConfig >
    {
    public:
        KFStoreConfig();
        ~KFStoreConfig();

        //加载配置
        bool LoadConfig();

        //查找商城配置
        const KFStoreSetting* FindStoreSetting( uint32 id ) const;
        inline uint32 GetVersion() const
        {
            return _version;
        }

        const KFMsg::MsgQueryStoreInfoAck* GetStoreClientShowInfo() const;

    public:
        KFMap< uint32, uint32, KFStoreSetting > _store_setting;

        // 热更新包
        uint32 _version;
        KFMsg::MsgQueryStoreInfoAck _client_show;

    private:
        bool StringSplit( KFMap< std::string, const std::string&, KFAgents >& des, std::string src, std::string key );

        // 格式化热更新包
        void MakePBStore( KFNode& xmlnode );

    };

    //////////////////////////////////////////////////////////////////////////
    static auto _kf_store_config = KFStoreConfig::Instance();
    //////////////////////////////////////////////////////////////////////////
}


#endif//__KF_STORE_CONFIG_H__
