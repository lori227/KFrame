#ifndef	__KF_GOODS_CONFIG_H__
#define	__KF_GOODS_CONFIG_H__

#include "KFConfig.h"

namespace KFrame
{
	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	class KFGoodsSetting : public KFIntSetting
	{
	public:
		// 商店id
		uint32 _store_id = 0u;

		// 购买的道具和数量
		KFElements _buy_data;

		// 上架时间
		uint64 _buy_start_time;

		// 下架时间
		uint64 _buy_finish_time;

		// 原始价格
		KFElements _price;

		// 折扣价格
		KFElements _discount;

		// 折扣开始时间
		uint64 _discount_start_time;

		// 折扣结束时间
		uint64 _discount_finish_time;

		// 限购重置时间id
		uint32 _limit_time_id = 0u;

		// 限购次数
		uint32 _limit_count = 0u;

		// 每次最小购买次数
		uint32 _min_buy_count = 0u;

		// 每次最大购买次数
		uint32 _max_buy_count = 0u;

	};

	/////////////////////////////////////////////////////////////////////////////////
	class KFGoodsConfig : public KFConfigT< KFGoodsSetting >, public KFInstance< KFGoodsConfig >
	{
	public:
		KFGoodsConfig()
		{
			_key_name = "id";
			_file_name = "goods";
		}

		~KFGoodsConfig() = default;

		virtual void LoadAllComplete()
		{
			for ( auto& iter : _settings._objects )
			{
				auto kfsetting = iter.second;

				KFGlobal::Instance()->ParseElement( kfsetting->_buy_data, _file_name.c_str(), kfsetting->_row );
				KFGlobal::Instance()->ParseElement( kfsetting->_price, _file_name.c_str(), kfsetting->_row );
				KFGlobal::Instance()->ParseElement( kfsetting->_discount, _file_name.c_str(), kfsetting->_row );
			}
		}

	protected:
		virtual void ReadSetting( KFXmlNode& xmlnode, KFGoodsSetting* kfsetting )
		{
			kfsetting->_store_id = xmlnode.ReadUInt32( "storeid", true );
			kfsetting->_buy_data._str_parse = xmlnode.ReadString( "buydata", true );
			kfsetting->_buy_start_time = xmlnode.ReadDate( "buystarttime", true );
			kfsetting->_buy_finish_time = xmlnode.ReadDate( "buyfinishtime", true );
			kfsetting->_price._str_parse = xmlnode.ReadString( "price", true );
			kfsetting->_discount._str_parse = xmlnode.ReadString( "discount", true );
			kfsetting->_discount_start_time = xmlnode.ReadDate( "discountstarttime", true );
			kfsetting->_discount_finish_time = xmlnode.ReadDate( "discountfinishtime", true );
			kfsetting->_limit_time_id = xmlnode.ReadUInt32( "limittimeid", true );
			kfsetting->_limit_count = xmlnode.ReadUInt32( "limitcount", true );
			kfsetting->_min_buy_count = xmlnode.ReadUInt32( "minbuycount", true );
			kfsetting->_max_buy_count = xmlnode.ReadUInt32( "maxbuycount", true );
		}

	};

	/////////////////////////////////////////////////////////////////////////////////
}
#endif
