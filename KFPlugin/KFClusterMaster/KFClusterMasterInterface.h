#ifndef __KF_CLUSTER_SERVER_INTERFACE_H__
#define __KF_CLUSTER_SERVER_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFClusterMasterInterface : public KFModule
    {
    public:

    };

    __KF_INTERFACE__( _kf_cluster_master, KFClusterMasterInterface );
}



#endif