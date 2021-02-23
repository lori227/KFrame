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
	class KFGoodsConfig : public KFConfigT<KFGoodsSetting>, public KFInstance<KFGoodsConfig>
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
			for ( auto& iter : _setting_list._objects )
			{
				auto setting = iter.second;

				KFGlobal::Instance()->ParseElement( setting->_buy_data, _file_name.c_str(), setting->_row );
				KFGlobal::Instance()->ParseElement( setting->_price, _file_name.c_str(), setting->_row );
				KFGlobal::Instance()->ParseElement( setting->_discount, _file_name.c_str(), setting->_row );
			}
		}

	protected:
		virtual void ReadSetting( KFXmlNode& xml_node, std::shared_ptr<KFGoodsSetting> setting )
		{
			setting->_store_id = xml_node.ReadUInt32( "storeid", true );
			setting->_buy_data._str_parse = xml_node.ReadString( "buydata", true );
			setting->_buy_start_time = xml_node.ReadDate( "buystarttime", true );
			setting->_buy_finish_time = xml_node.ReadDate( "buyfinishtime", true );
			setting->_price._str_parse = xml_node.ReadString( "price", true );
			setting->_discount._str_parse = xml_node.ReadString( "discount", true );
			setting->_discount_start_time = xml_node.ReadDate( "discountstarttime", true );
			setting->_discount_finish_time = xml_node.ReadDate( "discountfinishtime", true );
			setting->_limit_time_id = xml_node.ReadUInt32( "limittimeid", true );
			setting->_limit_count = xml_node.ReadUInt32( "limitcount", true );
			setting->_min_buy_count = xml_node.ReadUInt32( "minbuycount", true );
			setting->_max_buy_count = xml_node.ReadUInt32( "maxbuycount", true );
		}

	};

	/////////////////////////////////////////////////////////////////////////////////
}
#endif
