1. ngx_daemon() - 创建守护进程
作用: 使当前程序脱离终端控制，后台运行。
详细步骤:
fork(): 分叉出一个子进程，父进程退出。
setsid(): 使子进程成为新的会话组长和进程组长，脱离控制终端。
umask(0): 设置文件模式创建屏蔽字，不受继承的文件模式创建屏蔽字影响。
dup2(): 重定向标准输入、输出和错误到/dev/null。
逻辑: 子进程继续执行后续代码，而父进程退出，实现守护进程的启动。

2. ngx_master_process_cycle() - 主进程循环
作用: 初始化信号集合，屏蔽不需要处理的信号，并循环等待信号。
详细步骤:
屏蔽信号: 使用sigprocmask()屏蔽不希望处理的信号，避免在关键操作时被中断。
创建worker子进程: 调用ngx_start_worker_processes()根据配置创建指定数量的worker进程。
循环等待信号: 使用sigsuspend()等待信号，这时主进程阻塞，直到接收到信号。
逻辑: 主进程负责管理worker进程，不直接处理客户端请求，只处理信号和进程管理任务。

3. ngx_start_worker_processes() - 启动worker子进程
作用: 创建指定数量的worker进程。
操作: 循环调用ngx_spawn_process()创建子进程。
逻辑: 为服务器的并发处理提供基础，每个worker进程独立处理网络事件和客户端请求。

4. ngx_spawn_process() - 生成worker子进程
作用: 实际生成单个子进程。
操作: 使用fork()产生子进程，并根据fork返回值的不同执行不同的逻辑分支。
逻辑: 子进程进入ngx_worker_process_cycle()，成为一个独立运行的worker进程；父进程则继续执行ngx_start_worker_processes()中的循环。

5. ngx_worker_process_cycle() - worker进程主循环
作用: 无限循环处理网络事件和定时器事件。
初始化: 调用ngx_worker_process_init()进行worker进程的环境设置和信号屏蔽。
逻辑: 每个worker进程在这个函数中独立运行，处理所有的网络请求和事件。

6. ngx_worker_process_init() - worker进程初始化
作用: 初始化worker进程的运行环境。
详细步骤:
清空信号屏蔽: 为子进程接收新信号准备。
初始化网络库和线程池: 准备处理网络事件。
逻辑: 每个worker进程在开始处理事件前进行必要的设置。



ngx_daemon() 负责创建一个后台运行的守护进程。
ngx_master_process_cycle() 是服务器运行的核心，管理信号和worker进程。
ngx_start_worker_processes() 和 ngx_spawn_process() 协作创建和管理多个worker子进程。
ngx_worker_process_cycle() 和 ngx_worker_process_init() 定义了每个worker进程的行为和初始化过程，负责实际的网络请求处理。
这种架构模型允许高效处理网络事件，通过多进程模型提高服务器的并发处理能力，同时通过主进程管理子进程来提高系统的稳定性和可控性。