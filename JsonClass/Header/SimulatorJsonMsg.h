#ifndef SIMULATORJSONMSG_H
#define SIMULATORJSONMSG_H

#include <QList>
#include <QMap>
#include <QString>
#include <StructClsss.h>


class SimulatorJsonMsg{
public:
    SimulatorJsonMsg();
    SimulatorJsonMsg(const SimulatorData &data);
    SimulatorJsonMsg(const SimulatorJsonMsg &other); // 对象复制
    ~SimulatorJsonMsg();


    /* ------- 序列化与反序列化函数 ------- */
    bool fromJson(const QString &jsonStr); // 序列化函数(带校验)
    QByteArray toJson() const; // 反序列化函数

    void fromSimulatorData(const SimulatorData &data);
    void fromSimulatorJsonMsg(const SimulatorJsonMsg &other);
    void fromSimulatorJsonMsg(const SimulatorJsonMsg *other);

// -----------------------
// Json消息字段
// -----------------------
public:

    // -------------------------
    // 消息状态
    // -------------------------
    bool send_statius;                    ///< 消息是否发送的状态
    int sim_train_id;

    // -------------------------
    // 消息头信息(Header)
    // -------------------------
    int device_type;                  ///< 设备类型
    QString session_instance_id;          ///< 会话实例ID
    QString packet_type;                  ///< 消息类型
    int message_seqnum;                   ///< 消息序号
    QString request_id;                   ///< 请求ID

    // -------------------------
    // 列车基本信息
    // -------------------------
    QString train_code;                   ///< 列车编号
    QString direction;                    ///< 行驶方向 ("up"/"down")
    double train_speed_km_per_h;          ///< 列车速度（km/h）
    QString track_code;                      ///< 轨道编号
    QString landmark;                      ///< 当前公里标
    int offset_scale;                  ///< 轨道偏移量  0~10000 万分比

    // -------------------------
    // 驾驶室控制信息
    // -------------------------
    bool TC1_activated;           ///< 驾驶室1是否激活
    bool TC2_activated;           ///< 驾驶室2是否激活
    bool active_cab_forward_state;///< 主驾驶室前进状态
    bool active_cab_reverse_state;///< 主驾驶室倒退状态
    bool train_coasting;          ///< 列车是否滑行
    bool train_braking;           ///< 列车制动状态
    bool train_traction;          ///< 列车牵引状态
    bool ATO_release;             ///< ATO释放状态
    bool traction_enabled;        ///< 牵引使能
    bool TC1_key_state;           ///< TC1钥匙状态
    bool TC2_key_state;           ///< TC2钥匙状态
    bool EB_applied;                ///< 列车EB施加的回馈信号
    QString train_mode_set;     ///< 列车模式升降状态

    // -------------------------
    // ATO 信息
    // -------------------------
    bool TC1_ATP_cutout;          ///< TC1 ATP切除
    bool TC2_ATP_cutout;          ///< TC2 ATP切除
    bool TC1_ATO_start_PB;        ///< TC1 ATO启动按钮
    bool TC2_ATO_start_PB;        ///< TC2 ATO启动按钮
    bool TC1_auto_change_end_PB;  ///< TC1 自动切换结束按钮
    bool TC2_auto_change_end_PB;  ///< TC2 自动切换结束按钮
    QString TC1_door_mode;                ///< TC1 门模式
    QString TC2_door_mode;                ///< TC2 门模式

    // -------------------------
    // 门控制信息
    // -------------------------
    bool A_doors_close_state;     ///< A侧车门关闭状态
    bool A_door_open_request;     ///< A侧车门开门请求
    bool A_door_close_request;    //< A侧车门关门请求
    bool B_doors_close_state;     ///< B侧车门关闭状态
    bool B_door_open_request;     ///< B侧车门开门请求
    bool B_door_close_request;    //< B侧车门关门请求

    int event_type_map[20];  ///< 最多10个故障 0未触发故障、1触发故障 .  10个故障的顺序为1 ATO   2 ATP(下标从0开始)   3列车无电故障  4电线脱落事件
    bool UpSingleGong; ///< 升奇数弓0:没升弓,1升弓
    bool firstDoorOpen;  ///< 接触网无电时车门状态  0close    1open
    int leaveSiJiShi;    // 正常0   离开1    回来2
};
#endif // SIMULATORSERVER_H
