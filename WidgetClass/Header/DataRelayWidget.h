#ifndef DATARELAYWIDGET_H
#define DATARELAYWIDGET_H

#include <QWidget>
#include <QMap>
#include <QLabel>
#include <QComboBox>
#include "sessions_h/SimulatorSession.h"

namespace Ui {
class DataRelayWidget;
}

// -------------------------------------------------------
// 每列车一组控件引用（对应 Tab 内的各 QLabel）
// -------------------------------------------------------
struct TrainDisplayLabels
{
    // ---- Simulator → ATS （接收侧）----
    QLabel *sim_seqnum        = nullptr;
    QLabel *sim_packet_type   = nullptr;
    QLabel *sim_train_code    = nullptr;
    QLabel *sim_direction     = nullptr;
    QLabel *sim_speed         = nullptr;
    QLabel *sim_track_code    = nullptr;
    QLabel *sim_landmark      = nullptr;
    QLabel *sim_offset_scale  = nullptr;
    QLabel *sim_TC1_act       = nullptr;
    QLabel *sim_TC2_act       = nullptr;
    QLabel *sim_cab_forward   = nullptr;
    QLabel *sim_cab_reverse   = nullptr;
    QLabel *sim_traction      = nullptr;
    QLabel *sim_braking       = nullptr;
    QLabel *sim_coasting      = nullptr;
    QLabel *sim_EB_applied    = nullptr;
    QLabel *sim_ATO_release   = nullptr;
    QLabel *sim_traction_en   = nullptr;
    QLabel *sim_TC1_key       = nullptr;
    QLabel *sim_TC2_key       = nullptr;
    QLabel *sim_mode_set      = nullptr;
    QLabel *sim_TC1_ATP_cut   = nullptr;
    QLabel *sim_TC2_ATP_cut   = nullptr;
    QLabel *sim_TC1_ATO_PB    = nullptr;
    QLabel *sim_TC2_ATO_PB    = nullptr;
    QLabel *sim_TC1_change_PB = nullptr;
    QLabel *sim_TC2_change_PB = nullptr;
    QLabel *sim_TC1_door_mode = nullptr;
    QLabel *sim_TC2_door_mode = nullptr;
    QLabel *sim_ats_fault     = nullptr;
    QLabel *sim_A_door_close  = nullptr;
    QLabel *sim_A_door_open   = nullptr;
    QLabel *sim_A_door_close_req = nullptr;
    QLabel *sim_B_door_close  = nullptr;
    QLabel *sim_B_door_open   = nullptr;
    QLabel *sim_B_door_close_req = nullptr;
    QLabel *sim_rev_count     = nullptr;   ///< 收包计数

    // ---- ATS → Simulator （发送侧）----
    // ATP/ATO
    QLabel *ats_train_code       = nullptr;
    QLabel *ats_car_mode         = nullptr;
    QLabel *ats_op_mode          = nullptr;
    QLabel *ats_fault            = nullptr;
    QLabel *ats_A_door_enable    = nullptr;
    QLabel *ats_A_door_open      = nullptr;
    QLabel *ats_A_door_close     = nullptr;
    QLabel *ats_B_door_enable    = nullptr;
    QLabel *ats_B_door_open      = nullptr;
    QLabel *ats_B_door_close     = nullptr;
    QLabel *ats_ATP_EB           = nullptr;
    QLabel *ats_ATO_activate     = nullptr;
    QLabel *ats_ATO_avail        = nullptr;
    QLabel *ats_ATO_traction     = nullptr;
    QLabel *ats_ATO_brake        = nullptr;
    QLabel *ats_change_ends      = nullptr;
    QLabel *ats_changeend_act    = nullptr;
    QLabel *ats_changeend_avail  = nullptr;
    QLabel *ats_ATO_speed        = nullptr;
    // MMI
    QLabel *ats_brake_sign       = nullptr;
    QLabel *ats_run_speed        = nullptr;
    QLabel *ats_ATP_speed        = nullptr;
    QLabel *ats_EM_speed         = nullptr;
    QLabel *ats_drive_state      = nullptr;
    QLabel *ats_turn_back        = nullptr;
    QLabel *ats_pos_depot        = nullptr;
    QLabel *ats_next_skip        = nullptr;
    QLabel *ats_cur_detain       = nullptr;
    QLabel *ats_station_stop     = nullptr;
    QLabel *ats_dwell_time       = nullptr;
    QLabel *ats_PSD              = nullptr;
    QLabel *ats_close_door_info  = nullptr;
    QLabel *ats_dep_req          = nullptr;
    QLabel *ats_unit_number      = nullptr;
    QLabel *ats_dest_number      = nullptr;
    QLabel *ats_real_number      = nullptr;
    QLabel *ats_send_count       = nullptr;   ///< 发包计数

    // 线路设备数据（ATSLineDeviceDataJsonMsg）
    QComboBox *ats_turnout_combo  = nullptr;  ///< 道岔 key 下拉
    QLabel    *ats_turnout_value  = nullptr;  ///< 道岔 value 显示
    QComboBox *ats_signal_combo   = nullptr;  ///< 信号机 key 下拉
    QLabel    *ats_signal_value   = nullptr;  ///< 信号机 value 显示
};

// -------------------------------------------------------
class DataRelayWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DataRelayWidget(QMap<int, SimulatorSession *> trainIDMapSimulatorSession, QWidget *parent = nullptr);
    ~DataRelayWidget();

private:
    // UI 构建
    void buildTabs();
    QWidget *buildTrainTab(int trainID, TrainDisplayLabels &labels);

    // 刷新单列车显示
    void refreshSimLabels(TrainDisplayLabels &lb, const SimulatorJsonMsg &msg);
    void refreshATSLabels(TrainDisplayLabels &lb, const ATSJsonMsg &msg);

    // 工具
    static QString boolStr(bool v, const QString &trueStr  = "✔ 是", const QString &falseStr = "✗ 否");
    static void applyStatusStyle(QLabel *lb, bool ok);
    static void updateMapCombo(QComboBox *combo, QLabel *valueLabel, QMap<QString, QString> map);

    Ui::DataRelayWidget *ui;

    QMap<int, SimulatorSession *> m_sessions;    ///< simTrainID → session
    QMap<int, TrainDisplayLabels> m_labels;      ///< simTrainID → 控件组
    QMap<int, int>                m_revCountMap;  ///< simTrainID → 收包数
    QMap<int, int>                m_sendCountMap; ///< simTrainID → 发包数

    int m_totalRev  = 0;
    int m_totalSend = 0;
};

#endif // DATARELAYWIDGET_H
