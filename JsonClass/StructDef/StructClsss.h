#ifndef STRUCTCLASS_H
#define STRUCTCLASS_H

#include <QString>

//#pragma pack(push, 1)
struct VTrainDataStruct{
    int vtrain_code;                     ///< 虚拟列车编号
    int track_code;                      ///< 虚拟列车轨道编号
    float offset_scale;                  ///< 虚拟列车偏移量  0~10000 万分比
    int direction;                       // 虚拟列车方向  0上行  1下行
    int vtrain_train_speed_kmh;   // 虚拟列车速度
};

// --------------------------------
// 列车信息
// --------------------------------
struct TrainDataStruct{
    int vtrain_session_id;
    int vtrain_code;                     ///< 虚拟列车编号
    int track_code;                      ///< 初始轨道编号
    float offset_scale;                  ///< 初始轨道偏移量  0~10000 万分比
    int vtrain_train_speed_kmh;         // 虚拟列车速度
    int direction;                       //方向  0上行  1下行
    int sim_train_id;                    //模拟器 ID  对应四台模拟器  比如 1 - 4
    int sim_train_cab;                   //模拟器激活端  0未激活   1-TC1端激活   2-TC2端激活  3-双端激活
    int trainee_id;                      //学员号
//    int vtrain_code2;                   // 场景内虚拟车编号
//    int vtrain_track_code;              // 当前轨道编号
//    int vtrain_offset_scale;            // 当前轨道偏移量万分比 0~10000 万分比
//    int vtrain_direction;               // 当前方向 0上行  1下行
};

// ----------------------------------------
// 接收模拟器的数据
// ----------------------------------------
struct SimulatorData{
    int lineNo;                             // 场景线路  0无   1-1号线  5-5号线  10-10号线  12-12号线
    int train_code;                      //列车编号
    int direction;                       //方向  0上行  1下行
    int train_speed_km_per_h;            // 车速 0-100 km/h
    int track_code;                      ///< 轨道编号
    float landmark;                      ///< 当前公里标
    float offset_scale;                  ///< 轨道偏移量  0~10000 万分比
    bool TC1_activated;                  ///< TC1 车头激活: true=激活, false=不激活
    bool TC2_activated;                  ///< TC2 车头激活: true=激活, false=不激活
    bool active_cab_forward_state;       ///< 激活端向前: true=向前, false=无效
    bool active_cab_reverse_state;       ///< 激活端向后: true=向后, false=无效
    bool rain_coasting;                  ///< 列车惰行状态: true=惰行, false=无效
    bool train_braking;                  ///< 列车制动状态: true=制动, false=无效
    bool train_traction;                 ///< 列车牵引状态: true=牵引, false=无效
    bool ATO_release;                    ///< ATO 释放: true=有效, false=无效
    bool traction_enabled;               ///< 牵引允许状态: true=允许, false=封锁
    bool TC1_key_state;                  ///< TC1 车头钥匙状态: true=开, false=关
    bool TC2_key_state;                  ///< TC2 车头钥匙状态: true=开, false=关
    bool EB_applied;                        ///< 列车EB施加的回馈信号
    int train_mode_set;                 ///< 列车模式升降状态0:none,1:up,2:down,3:confirm
    bool TC1_ATP_cutout;                 ///< TC1 车头 ATP 切除开关: true=ATP切除, false=ATP未切除
    bool TC2_ATP_cutout;                 ///< TC2 车头 ATP 切除开关: true=ATP切除, false=ATP未切除
    bool TC1_ATO_start_PB;               ///< TC1 车头 ATO 启动按钮: true=激活, false=未激活
    bool TC2_ATO_start_PB;               ///< TC2 车头 ATO 启动按钮: true=激活, false=未激活
    bool TC1_auto_change_end_PB;         ///< TC1 车头自动折返按钮: true=激活, false=未激活
    bool TC2_auto_change_end_PB;         ///< TC2 车头自动折返按钮: true=激活, false=未激活
    int  TC1_door_mode;                  ///< TC1 车头门模式: 1=AA(自动开自动关), 2=AM(自动开手动关), 3=MM(手动开手动关)
    int  TC2_door_mode;                  ///< TC2 车头门模式: 1=AA, 2=AM, 3=MM

    int sim_ats_event[20];      ///< 最多20个故障 0未触发故障、1触发故障 .  20个故障的顺序为1 ATO   2 ATP(下标从0开始)   3列车无电故障  4电线脱落事件  5被迫停车(联挂)   6VTM操作联挂    7列车撞击 8轨道异物   9隧道坍塌故障  10拿取灭火工具包
    //成都  7列车撞击   8车门解锁状态    9整列车门状态    10司机操作故障车门隔离

    bool A_doors_close_state;            ///< A 侧门锁闭状态: true=锁闭, false=未锁闭
    bool A_door_open_request;            ///< A 侧门开门请求: true=激活, false=不激活
    bool A_door_close_request;           ///< A 侧门关门请求: true=激活, false=不激活
    bool B_doors_close_state;            ///< B 侧门锁闭状态: true=锁闭, false=未锁闭
    bool B_door_open_request;            ///< B 侧门开门请求: true=激活, false=不激活
    bool B_door_close_request;           ///< B 侧门关门请求: true=激活, false=不激活
    bool UpSingleGong; ///< 升奇数弓0:没升弓,1升弓
    bool firstDoorOpen;    ///< 接触网无电时车门状态  0close    1open   (成都  夹物车门关开状态)
    int leaveSiJiShi;    // 正常0   离开1    回来2
};

// ----------------------------------------
// 向模拟器发送的数据
// ----------------------------------------
struct ATSData{
    // ----------------------------------------
    // 信息标识
    // ----------------------------------------
    int device_type;                     //接口设备类型 0无   1可移动仿真车   2列车模拟器   3通信系统设备   4轨旁设备
    int session_instance_id;             //场景 ID
    int packet_type;                     //0无  1场景定义  2场景状态  3ATS 虚拟时间   4模拟器列车信息   5ATP/ATO 信息   6MMI 信号屏信息   7虚拟列车信息  8线路设备状态信息  9回复信息
    int message_seqnum;                  //消息序列号  32 位 unsigned int 值，超出后从零开始
    int request_id;                      //请求编号

    // ----------------------------------------
    // 场景定义数据
    // ----------------------------------------
    int session_id;                     // 场景 ID  教员台客户端生成的唯一 ID 号
    int session_name;                    //场景名称  0无   1列车故障演练场景
    int course_id;                      // 课程ID 1:道岔故障   2:列车冒烟起火故障    3:淹水倒灌故障   4:信号设备故障     5:区间无电   6:结构坍塌    7:列车撞击人员    8:列车救援    9:区间疏散  10轨道异常
    int session_line_type;               // 场景线路  0无   1-1号线  5-5号线  10-10号线  12-12号线

    // --------------------------------
    // 虚拟列车信息
    // --------------------------------
    TrainDataStruct train_data[10];

    // ----------------------------------------
    // 场景状态控制数据
    // ----------------------------------------
    int control_type;                    // 场景状态控制  0无   1场景开始  2运行  3暂停  4场景结束

    // ----------------------------------------
    // 虚拟时间
    // ----------------------------------------
    //场景启动的时间
    unsigned int nYear;
    unsigned int nMonth;
    unsigned int nDay;
    unsigned int nHour;
    unsigned int nMin;
    unsigned int nSec;

    //场景当前的时间
    unsigned int nYearNow;
    unsigned int nMonthNow;
    unsigned int nDayNow;
    unsigned int nHourNow;
    unsigned int nMinNow;
    unsigned int nSecNow;

    int ats_elapsed_time_s;             //场景运行时间 单位：秒

    // ----------------------------------------
    // ATP/ATO数据
    // ----------------------------------------
    int train_code;                     //列车编号
    // modes:模式
    int current_car_mode;               //当前列车驾驶模式  1-初始状态  2-ATO模式  3-CM(ATP监控模式)  4-RM限制模式  5EUM(ATP旁路)  6LACP(救援联挂)
    int current_operation_mode;         //当前运营等级  1-初始状态  2-"IXL" 联锁级  3-"BLOC" 点式 ATP    4CBTC" 连续式通信  5LACP(救援联挂)

    // door_control:车门控制
    bool A_doors_enable;                ///< A 侧门使能指令: true=激活, false=不激活
    bool A_door_open_cmd;               ///< A 侧门开指令: true=激活, false=不激活
    bool A_door_close_cmd;              ///< A 侧门关指令: true=激活, false=不激活
    bool B_doors_enable;                ///< B 侧门使能指令: true=激活, false=不激活
    bool B_door_open_cmd;               ///< B   侧门开指令: true=激活, false=不激活
    bool B_door_close_cmd;              ///< B 侧门关指令: true=激活, false=不激活
    // ATP_ATO_control:ATP/ATO 控制
    bool ATP_EB_cmd;                    ///< ATP 紧急制动指令: true=EB Release (缓解), false=EB Apply (施加)
    bool ATO_activate;                  ///< ATO 激活指令: true=激活, false=不激活
    bool ATO_available;                 ///< ATO 准备就绪: true=激活, false=不激活
    bool ATO_traction_cmd;              ///< ATO 牵引状态指令: true=激活, false=不激活
    bool ATO_brake_cmd;                 ///< ATO 制动状态指令: true=激活, false=不激活
    bool auto_change_ends_cmd;          ///< 自动折返指令: true=激活, false=不激活
    bool auto_changeend_activate;       ///< 自动折返激活: true=激活, false=不激活
    bool auto_changeend_available;      ///< 自动折返准备就绪: true=激活, false=不激活
    int  ATO_rec_speed;                 ///< ATO 推荐速度/牵引制动指令: 1% ~ 100% (值范围: 1 ~ 100)  ？？？怎么区分牵引还是制动

    // --------------------------------
    // MMI 信号屏信息
    // --------------------------------
    int train_code2;                    ///< 列车编号 (ATS虚拟车编号), eg: 01
    int brake_sign;                     ///< 制动标志: 1=初始, 2=制动请求, 3=紧急制动施加
    int train_run_speed;                ///< 列车当前速度: 0-100 km/h
    int ATP_recommended_speed;          ///< ATP 推荐速度: 0-100 km/h
    int EM_brake_trigger_speed;         ///< 紧急制动触发速度: 0-100 km/h
    int train_driving_state;            ///< 牵引制动状态: 1=惰行, 2=牵引, 3=制动
    int train_turn_back;                ///< 列车折返状态: 1=不在折返区, 2=可折返, 3=已确认折返
    int position_with_depot;            ///< 是否进入车辆段: 1=不显示, 2=进入, 3=在段内缓行
    bool next_station_skip;             ///< 下一站是否跳停: true=跳停, false=不跳停
    bool current_station_detain;        ///< 当前站台扣车: true=扣车, false=不扣车
    int train_station_stop;             ///< 停车状态: 1=不在站台, 2=未停准, 3=精准停车
    int dwell_time;                     ///< 剩余停站时间: 1=不显示, 2=剩余5秒（或其他数值也可扩展）
    int current_PSD_state;              ///< 屏蔽门状态: 1=未关闭, 2=关闭, 3=未知
    bool close_door_info;               ///< 提示关闭车门: true=提示, false=未提示
    bool departure_request;             ///< 发车请求: true=有请求, false=无请求
    char train_unit_number[10];              ///< 车次号 (具体内容待定)
    char destination_number[10];             ///< 目的地号 (具体内容待定)
    int train_real_number;
    int platform_stop_distance;                 ///< 站点停车点距离(0到999)其中越标多少米(-1到-999)没有进站-10000

    // ----------------------------------------
    // Line_Device_Data – 线路设备状态信息
    // ----------------------------------------
    int Line_session_id;                //场景ID

    //occ 的虚拟时间
    unsigned int nOCC_Year;
    unsigned int nOCC_Month;
    unsigned int nOCC_Day;
    unsigned int nOCC_Hour;
    unsigned int nOCC_Min;
    unsigned int nOCC_Sec;

    int turnout[10];                    //道岔状态 0定位  1反位  按位存储  从低到高 一个int存32个道岔
    int signal[100];                    //信号机   0-全灭 1-开红 2-开绿 3-开黄  5-白灯 8-蓝  24-红黄  按位存储 从低到高 一个int存4个信号机
    int changeTurnout [50];             // 变化的道岔数据
    int changeTurnoutID [50];           // 变化的道岔ID
    int changeSignal [50];              // 变化的信号灯数据
    int changeSignalID [50];              // 变化的信号灯ID
    int nPSD[2];                        //安全门 0关闭  1打开  按位存储  从低到高 一个int存32个门
    int platform_countdown_timer[6];    // 站台计时

    int ats_sim_fault[10];      ///< 最多10个故障: 0未触发故障、1触发故障 .  10个故障的顺序为:1 ATO   2 ATP(下标从0开始)   3列车救援联挂    4移除轨道障碍物    5列车起火    6移除列车撞击
    int fault_param1;               ///<  3列车救援联挂: 救援列车编号,  4移除轨道障碍物:  障碍物所在轨道编号
    int fault_param2;               ///<  3列车救援联挂:  故障列车编号
    int fault_param3;

    VTrainDataStruct vtrainData[10];        // 其他虚拟列车
};

struct Reply{
    // ----------------------------------------
    // 信息标识
    // ----------------------------------------
    int lineNo;
    int device_type;                     //接口设备类型 0无   1可移动仿真车   2列车模拟器   3通信系统设备   4轨旁设备
    int session_instance_id;             //场景 ID
    int packet_type;                     //0无  1场景定义  2场景状态  3ATS 虚拟时间   4模拟器列车信息   5ATP/ATO 信息   6MMI 信号屏信息   7虚拟列车信息  8线路设备状态信息  9回复信息
    int message_seqnum;                  //消息序列号  32 位 unsigned int 值，超出后从零开始
    int request_id;                      //请求编号

    int result_code;                     // 200
    bool result_desc;                    // true:"ok"
};

// ----------------------------------------
// 模拟器基本信息结构体
// ----------------------------------------
struct SimulatorStatius{
    QString ip;                                         // 模拟器ip
    unsigned short port;                                // 模拟器的端口

    int heartbeatInterval;                              // 心跳间隔(ms)
    int missedHeartbeats;                               // 未响应次数
    int maxMissedHeartbeats;                            // 最大允许未响应次数

    bool online = false;                                // 是否已经连接
};

//#pragma pack(pop)
#endif // STRUCTCLASS_H
