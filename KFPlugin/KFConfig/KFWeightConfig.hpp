#ifndef	__KF_WEIGHT_CONFIG_H__
#define	__KF_WEIGHT_CONFIG_H__

#include "KFConfig.h"

namespace KFrame
{
	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	class KFWeightSetting : public KFIntSetting
	{
	public:
		// 权重列表
		KFWeightList<KFWeight> _weight;
	};

	/////////////////////////////////////////////////////////////////////////////////
	class KFWeightConfig : public KFConfigT<KFWeightSetting>, public KFInstance<KFWeightConfig>
	{
	public:
		KFWeightConfig()
		{
			_key_name = "id";
			_file_name = "weight";
		}

		~KFWeightConfig() = default;

	protected:
		virtual void ReadSetting( KFXmlNode& xml_node, std::shared_ptr<KFWeightSetting> setting )
		{
		
			KFWeight weight;
			weight._value = xml_node.ReadUInt32( "value", true );
			weight._weight = xml_node.ReadUInt32( "weight", true );
			setting->_weight.Create( weight._value, weight._weight );
		}

	};

	/////////////////////////////////////////////////////////////////////////////////
}
#endif
