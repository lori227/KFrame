#ifndef __KF_RESOURCE_DATA_H__
#define __KF_RESOURCE_DATA_H__

#include "KFResourceDefine.h"
///////////////////////////////////////////////////////////////////////
class ResourceData : public KFSingleton< ResourceData >
{
public:

};

static auto _resource_data = ResourceData::Instance();
#endif
