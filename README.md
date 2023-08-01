# chatservers
可以工作在nginx tcp负载均衡环境中的集群聊天服务器和客户端源代码 基于moduo实现 redis mysql

编译方式
cd build
rm -rf *
cmake ..
make


需要nginx tcp负载均衡进行部署  
需要安装：reids、mysql
