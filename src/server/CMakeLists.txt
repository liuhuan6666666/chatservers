#将源文件加载到SRC_list
aux_source_directory(. SRC_list)
aux_source_directory(./db DB_list)
aux_source_directory(./model Model_list)
aux_source_directory(./redis REDIS_list)
#生成可执行文件
add_executable(Chatserver ${SRC_list} ${DB_list} ${Model_list} ${REDIS_list})
#依赖库
target_link_libraries(Chatserver muduo_net muduo_base mysqlclient hiredis pthread)
