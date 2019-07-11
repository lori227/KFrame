# linux下core文件设置

ulimit -c
0：core文件生成是受限制; ulimit：core文件生成不受限制

修改设置让core文件正常生成

#ulimit -c unlimited
打开文件/etc/profile  然后在最末尾添加一行 ulimit -c unlimited ,然后保存退出，使用命令 source /etc/profile  使其生效

# enable coredump whith unlimited file-size for all users(解除core文件的大小限制)
echo -e "\n# enable coredump whith unlimited file-size for all users\n* soft core unlimited" >> /etc/security/limits.conf

# format the name of core file. (设置格式化的core文件名和保存位置)
# %% – 符号%
# %p – 进程号
# %u – 进程用户id
# %g – 进程用户组id
# %s – 生成core文件时收到的信号
# %t – 生成core文件的时间戳(seconds since 0:00h, 1 Jan 1970)
# %h – 主机名
# %e – 程序文件名
echo -e "core-%e-%s-%p-%t" > /proc/sys/kernel/core_pattern

# suffix of the core file name(添加pid作为core文件扩展)
echo -e "1" > /proc/sys/kernel/core_uses_pid

#持久化这些操作，防止重启指令失效
echo -e "\nkernel.core_pattern=core-%e-%s-%p-%t" >> /etc/sysctl.conf

#不重启查看添加好的效果
sysctl -p /etc/sysctl.conf
