set(EXTEND_LIB_DEBUG KFHttpd KFUtilityd PocoNetd PocoNetSSLd PocoCryptod PocoFoundationd ssl crypto pthread)
set(EXTEND_LIB_RELEASE KFHttp KFUtility PocoNet PocoNetSSL PocoCrypto PocoFoundation ssl crypto pthread)
BUILD_PLUGIN_NO_PROTO("KFHttpClient")
