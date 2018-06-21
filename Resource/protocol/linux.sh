cd proto
sh __cpp_class.sh
cd ../

\cp KFFrameMessage.* ../../Source/KFPlugin/KFTcpClient/
\cp KFFrameMessage.* ../../Source/KFPlugin/KFTcpServer/

\cp KFClusterMessage.* ../../Source/KFPlugin/KFClusterServer/
\cp KFClusterMessage.* ../../Source/KFPlugin/KFClusterClient/
\cp KFClusterMessage.* ../../Source/KFPlugin/KFClusterProxy/
\cp KFClusterMessage.* ../../Source/KFPlugin/KFClusterShard/

\cp KFRouteMessage.* ../../Source/KFPlugin/KFRouteClient/
\cp KFRouteMessage.* ../../Source/KFPlugin/KFRouteProxy/
\cp KFRouteMessage.* ../../Source/KFPlugin/KFRouteShard/
\cp KFRouteMessage.* ../../Source/KFPlugin/KFClusterShard/

\cp KFDataMessage.* ../../Source/KFContrib/KFProtocol/
\cp KFClientMessage.* ../../Source/KFContrib/KFProtocol/
\cp KFCommonMessage.* ../../Source/KFContrib/KFProtocol/
\cp KFServerMessage.* ../../Source/KFContrib/KFProtocol/
\cp KFBattleMessage.* ../../Source/KFContrib/KFProtocol/
\cp KFMatchMessage.* ../../Source/KFContrib/KFProtocol/

rm -f *.pb.*

