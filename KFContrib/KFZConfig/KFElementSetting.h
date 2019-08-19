#ifndef __KF_ELEMENT_SETTING_H__
#define __KF_ELEMENT_SETTING_H__

#include "KFrame.h"
#include "KFSetting.h"

namespace KFrame
{
    class KFElementSetting : public KFStrSetting
    {
    public:
        // 类型
        uint32 _type;

        // 格式化字串( 顺序: name value id )
        std::string _element_template;
    };
    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
}
#endif
