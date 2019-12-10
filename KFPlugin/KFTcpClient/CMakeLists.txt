set(EXTEND_LIB_DEBUG KFNetworkd KFUtilityd uv pthread lz4)
set(EXTEND_LIB_RELEASE KFNetwork KFUtility uv pthread lz4)

BUILD_PLUGIN_WITH_PROTO("KFTcpClient")