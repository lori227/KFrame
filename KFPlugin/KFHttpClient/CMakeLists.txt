set(EXTEND_LIB_DEBUG KFHttpd KFUtilityd PocoNetd PocoNetSSLd PocoCryptod PocoFoundationd crypto ssl pthread)
set(EXTEND_LIB_RELEASE KFHttp KFUtility PocoNet PocoNetSSL PocoCrypto PocoFoundation crypto ssl pthread)
BUILD_PLUGIN_NO_PROTO("KFHttpClient")
