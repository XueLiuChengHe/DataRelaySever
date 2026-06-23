#include "DataRelayWidget.h"
#include "ui_DataRelayWidget.h"

#include <ATSJsonMsg.h>
#include <SimulatorJsonMsg.h>

#include <QGroupBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QScrollArea>

// ======================================================
//  样式常量
// ======================================================
static const char *kStyleGrp =
    "QGroupBox{"
    "  border:1px solid #45475a;"
    "  border-radius:6px;"
    "  margin-top:8px;"
    "  padding-top:6px;"
    "  font-weight:bold;"
    "  color:#89b4fa;"
    "}";

static const char *kStyleOk  =
    "color:#a6e3a1; background:#313244; border-radius:3px; padding:1px 4px;";
static const char *kStyleErr =
    "color:#f38ba8; background:#313244; border-radius:3px; padding:1px 4px;";
static const char *kStyleVal =
    "color:#cdd6f4; background:#313244; border-radius:3px; padding:1px 4px;";
static const char *kStyleCnt =
    "color:#f9e2af; font-weight:bold; font-size:13px;";

// ======================================================
//  构造 / 析构
// ======================================================
DataRelayWidget::DataRelayWidget(QMap<int, SimulatorSession *> trainIDMapSimulatorSession, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DataRelayWidget)
    , m_sessions(trainIDMapSimulatorSession)
{
    ui->setupUi(this);
    ui->tabWidgetTrains->clear();
    buildTabs();

    for (auto it = m_sessions.begin(); it != m_sessions.end(); ++it) {
        SimulatorSession *session = it.value();
        int trainID = session->getSimTrainID();

        m_revCountMap[trainID]  = 0;
        m_sendCountMap[trainID] = 0;

        // lambda 捕获 trainID，避免从消息体中解析
        connect(session, &SimulatorSession::sendSimulatorAndATSMsg,
                this, [this, trainID](const ATSJsonMsg *atsMsg, const SimulatorJsonMsg *simMsg)
                {
                    if (!simMsg || !atsMsg) return;
                    if (!m_labels.contains(trainID)) return;

                    TrainDisplayLabels &lb = m_labels[trainID];

                    m_revCountMap[trainID]++;
                    m_sendCountMap[trainID]++;
                    m_totalRev++;
                    m_totalSend++;

                    refreshSimLabels(lb, *simMsg);
                    refreshATSLabels(lb, *atsMsg);

                    lb.sim_rev_count->setText(QString::number(m_revCountMap[trainID]));
                    lb.ats_send_count->setText(QString::number(m_sendCountMap[trainID]));

                    ui->labelTotalRev->setText(QString("总收包: %1").arg(m_totalRev));
                    ui->labelTotalSend->setText(QString("总发包: %1").arg(m_totalSend));
                },Qt::QueuedConnection);
    }
}

DataRelayWidget::~DataRelayWidget()
{
    delete ui;
}

// ======================================================
//  动态构建 Tab
// ======================================================
void DataRelayWidget::buildTabs()
{
    for (auto it = m_sessions.begin(); it != m_sessions.end(); ++it) {
        int trainID = it.value()->getSimTrainID();
        TrainDisplayLabels labels;
        QWidget *tab = buildTrainTab(trainID, labels);
        m_labels[trainID] = labels;

        QString title = QString("列车 %1").arg(trainID, 2, 10, QChar('0'));
        ui->tabWidgetTrains->addTab(tab, title);
    }
}

// ======================================================
//  构建单个列车 Tab 页
// ======================================================
QWidget *DataRelayWidget::buildTrainTab(int /*trainID*/, TrainDisplayLabels &lb)
{
    QWidget     *tab     = new QWidget(this);
    QHBoxLayout *hLayout = new QHBoxLayout(tab);
    hLayout->setSpacing(10);
    hLayout->setContentsMargins(8, 8, 8, 8);

    // ---- 辅助 lambda ----
    auto makeHeader = [](const QString &text) -> QLabel * {
        QLabel *l = new QLabel(text);
        l->setStyleSheet("color:#89dceb; font-weight:bold; font-size:11px;");
        return l;
    };
    auto makeValue = []() -> QLabel * {
        QLabel *l = new QLabel("-");
        l->setStyleSheet(kStyleVal);
        l->setMinimumWidth(110);
        return l;
    };

    // ==================================================
    // 左侧：Simulator 接收
    // ==================================================
    QGroupBox   *grpSim = new QGroupBox("Simulator  (接收)");
    grpSim->setStyleSheet(kStyleGrp);
    QFormLayout *fSim   = new QFormLayout(grpSim);
    fSim->setHorizontalSpacing(12);
    fSim->setVerticalSpacing(4);

    auto addSim = [&](const QString &label, QLabel *&target) {
        target = makeValue();
        fSim->addRow(makeHeader(label), target);
    };

    // 消息头
    addSim("包序号",           lb.sim_seqnum);
    addSim("消息类型",         lb.sim_packet_type);
    // 列车基本信息
    addSim("列车编号",         lb.sim_train_code);
    addSim("方向",             lb.sim_direction);
    addSim("速度 (km/h)",      lb.sim_speed);
    addSim("轨道编号",         lb.sim_track_code);
    addSim("公里标",           lb.sim_landmark);
    addSim("轨道偏移量",       lb.sim_offset_scale);
    // 驾驶室
    addSim("TC1 激活",         lb.sim_TC1_act);
    addSim("TC2 激活",         lb.sim_TC2_act);
    addSim("主驾前进",         lb.sim_cab_forward);
    addSim("主驾后退",         lb.sim_cab_reverse);
    addSim("牵引",             lb.sim_traction);
    addSim("制动",             lb.sim_braking);
    addSim("滑行",             lb.sim_coasting);
    addSim("EB 施加",          lb.sim_EB_applied);
    addSim("ATO 释放",         lb.sim_ATO_release);
    addSim("牵引使能",         lb.sim_traction_en);
    addSim("TC1 钥匙",         lb.sim_TC1_key);
    addSim("TC2 钥匙",         lb.sim_TC2_key);
    addSim("列车模式升降",     lb.sim_mode_set);
    // ATO 信息
    addSim("TC1 ATP切除",      lb.sim_TC1_ATP_cut);
    addSim("TC2 ATP切除",      lb.sim_TC2_ATP_cut);
    addSim("ATO启动PB(TC1)",   lb.sim_TC1_ATO_PB);
    addSim("ATO启动PB(TC2)",   lb.sim_TC2_ATO_PB);
    addSim("自动切端PB(TC1)",  lb.sim_TC1_change_PB);
    addSim("自动切端PB(TC2)",  lb.sim_TC2_change_PB);
    addSim("TC1 门模式",       lb.sim_TC1_door_mode);
    addSim("TC2 门模式",       lb.sim_TC2_door_mode);
    addSim("sim 故障状态",     lb.sim_ats_fault);
    // 车门
    addSim("A侧门关闭状态",    lb.sim_A_door_close);
    addSim("A侧门开门请求",    lb.sim_A_door_open);
    addSim("A侧门关门请求",    lb.sim_A_door_close_req);
    addSim("B侧门关闭状态",    lb.sim_B_door_close);
    addSim("B侧门开门请求",    lb.sim_B_door_open);
    addSim("B侧门关门请求",    lb.sim_B_door_close_req);
    // 计数
    lb.sim_rev_count = new QLabel("0");
    lb.sim_rev_count->setStyleSheet(kStyleCnt);
    fSim->addRow(makeHeader("收包总数"), lb.sim_rev_count);

    QScrollArea *scrollSim = new QScrollArea();
    scrollSim->setWidgetResizable(true);
    scrollSim->setWidget(grpSim);
    scrollSim->setStyleSheet("border:none;");

    // ==================================================
    // 垂直分割线
    // ==================================================
    QFrame *sep = new QFrame();
    sep->setFrameShape(QFrame::VLine);
    sep->setFrameShadow(QFrame::Sunken);
    sep->setStyleSheet("background:#45475a;");

    // ==================================================
    // 右侧：ATS 发送
    // ==================================================
    QGroupBox   *grpATS = new QGroupBox("ATS  (发送)");
    grpATS->setStyleSheet(kStyleGrp);
    QFormLayout *fATS   = new QFormLayout(grpATS);
    fATS->setHorizontalSpacing(12);
    fATS->setVerticalSpacing(4);

    auto addATS = [&](const QString &label, QLabel *&target) {
        target = makeValue();
        fATS->addRow(makeHeader(label), target);
    };

    // ATP/ATO 部分
    addATS("列车号",            lb.ats_train_code);
    addATS("当前车模式",        lb.ats_car_mode);
    addATS("当前运行模式",      lb.ats_op_mode);
    addATS("ATS发出故障",       lb.ats_fault);
    addATS("A侧门使能",         lb.ats_A_door_enable);
    addATS("A侧门开指令",       lb.ats_A_door_open);
    addATS("A侧门关指令",       lb.ats_A_door_close);
    addATS("B侧门使能",         lb.ats_B_door_enable);
    addATS("B侧门开指令",       lb.ats_B_door_open);
    addATS("B侧门关指令",       lb.ats_B_door_close);
    addATS("ATP EB指令",        lb.ats_ATP_EB);
    addATS("ATO激活",           lb.ats_ATO_activate);
    addATS("ATO就绪",           lb.ats_ATO_avail);
    addATS("ATO牵引指令",       lb.ats_ATO_traction);
    addATS("ATO制动指令",       lb.ats_ATO_brake);
    addATS("自动折返指令",      lb.ats_change_ends);
    addATS("自动折返激活",      lb.ats_changeend_act);
    addATS("自动折返就绪",      lb.ats_changeend_avail);
    addATS("ATO推荐速度",       lb.ats_ATO_speed);
    // MMI 部分
    addATS("制动标志",          lb.ats_brake_sign);
    addATS("列车当前速度",      lb.ats_run_speed);
    addATS("ATP推荐速度",       lb.ats_ATP_speed);
    addATS("EM制动触发速度",    lb.ats_EM_speed);
    addATS("牵引制动状态",      lb.ats_drive_state);
    addATS("列车折返状态",      lb.ats_turn_back);
    addATS("车辆段位置",        lb.ats_pos_depot);
    addATS("下一站跳停",        lb.ats_next_skip);
    addATS("当前站扣车",        lb.ats_cur_detain);
    addATS("停车状态",          lb.ats_station_stop);
    addATS("剩余停站时间",      lb.ats_dwell_time);
    addATS("屏蔽门状态",        lb.ats_PSD);
    addATS("提示关闭车门",      lb.ats_close_door_info);
    addATS("发车请求",          lb.ats_dep_req);
    addATS("车次号",            lb.ats_unit_number);
    addATS("目的地号",          lb.ats_dest_number);
    addATS("车次实时编号",      lb.ats_real_number);
    // 计数
    lb.ats_send_count = new QLabel("0");
    lb.ats_send_count->setStyleSheet(kStyleCnt);
    fATS->addRow(makeHeader("发包总数"), lb.ats_send_count);

    // 道岔状态（下拉选key，下方显示value）
    lb.ats_turnout_combo = new QComboBox();
    lb.ats_turnout_combo->setStyleSheet(
        "QComboBox{background:#313244;color:#cdd6f4;border:1px solid #45475a;"
        "border-radius:3px;padding:1px 4px;min-width:110px;}"
        "QComboBox::drop-down{border:none;}"
        "QComboBox QAbstractItemView{background:#313244;color:#cdd6f4;"
        "selection-background-color:#45475a;}");
    lb.ats_turnout_value = new QLabel("-");
    lb.ats_turnout_value->setStyleSheet(kStyleVal);
    fATS->addRow(makeHeader("道岔 (选编号)"), lb.ats_turnout_combo);
    fATS->addRow(makeHeader("道岔状态值"),    lb.ats_turnout_value);

    // 信号机状态（下拉选key，下方显示value）
    lb.ats_signal_combo = new QComboBox();
    lb.ats_signal_combo->setStyleSheet(
        "QComboBox{background:#313244;color:#cdd6f4;border:1px solid #45475a;"
        "border-radius:3px;padding:1px 4px;min-width:110px;}"
        "QComboBox::drop-down{border:none;}"
        "QComboBox QAbstractItemView{background:#313244;color:#cdd6f4;"
        "selection-background-color:#45475a;}");
    lb.ats_signal_value = new QLabel("-");
    lb.ats_signal_value->setStyleSheet(kStyleVal);
    fATS->addRow(makeHeader("信号机 (选编号)"), lb.ats_signal_combo);
    fATS->addRow(makeHeader("信号机状态值"),    lb.ats_signal_value);

    // 下拉切换时同步更新 value label
    // 直接捕获控件指针，避免捕获结构体地址（结构体拷贝后地址变化会导致野指针）
    QComboBox *turnoutCombo = lb.ats_turnout_combo;
    QLabel    *turnoutValue = lb.ats_turnout_value;
    QComboBox *signalCombo  = lb.ats_signal_combo;
    QLabel    *signalValue  = lb.ats_signal_value;

    QObject::connect(turnoutCombo,
                     QOverload<int>::of(&QComboBox::currentIndexChanged),
                     turnoutCombo,   // 以combo自身为context，combo销毁时自动断开
                     [turnoutCombo, turnoutValue](int idx) {
                         if (!turnoutCombo || !turnoutValue) return;
                         if (idx < 0 || idx >= turnoutCombo->count()) return;
                         turnoutValue->setText(turnoutCombo->itemData(idx).toString());
                     }, Qt::AutoConnection);

    QObject::connect(signalCombo,
                     QOverload<int>::of(&QComboBox::currentIndexChanged),
                     signalCombo,    // 以combo自身为context，combo销毁时自动断开
                     [signalCombo, signalValue](int idx) {
                         if (!signalCombo || !signalValue) return;
                         if (idx < 0 || idx >= signalCombo->count()) return;
                         signalValue->setText(signalCombo->itemData(idx).toString());
                     }, Qt::AutoConnection);

    QScrollArea *scrollATS = new QScrollArea();
    scrollATS->setWidgetResizable(true);
    scrollATS->setWidget(grpATS);
    scrollATS->setStyleSheet("border:none;");

    // ==================================================
    // 组装
    // ==================================================
    hLayout->addWidget(scrollSim, 1);
    hLayout->addWidget(sep);
    hLayout->addWidget(scrollATS, 1);

    return tab;
}

// ======================================================
//  刷新 Simulator 侧标签（SimulatorJsonMsg 全字段）
// ======================================================
void DataRelayWidget::refreshSimLabels(TrainDisplayLabels &lb, const SimulatorJsonMsg &msg)
{
    // 消息头
    lb.sim_seqnum->setText(QString::number(msg.message_seqnum));
    lb.sim_packet_type->setText(msg.packet_type);

    // 列车基本信息
    lb.sim_train_code->setText(msg.train_code);
    lb.sim_direction->setText(msg.direction);
    lb.sim_speed->setText(QString::number(msg.train_speed_km_per_h, 'f', 1));
    lb.sim_track_code->setText(msg.track_code);
    lb.sim_landmark->setText(msg.landmark);
    lb.sim_offset_scale->setText(QString::number(msg.offset_scale));

    // 驾驶室
    lb.sim_TC1_act->setText(boolStr(msg.TC1_activated));
    applyStatusStyle(lb.sim_TC1_act, msg.TC1_activated);

    lb.sim_TC2_act->setText(boolStr(msg.TC2_activated));
    applyStatusStyle(lb.sim_TC2_act, msg.TC2_activated);

    lb.sim_cab_forward->setText(boolStr(msg.active_cab_forward_state));
    lb.sim_cab_reverse->setText(boolStr(msg.active_cab_reverse_state));

    lb.sim_traction->setText(boolStr(msg.train_traction, "✔ 牵引中", "-"));
    applyStatusStyle(lb.sim_traction, msg.train_traction);

    lb.sim_braking->setText(boolStr(msg.train_braking, "⚠ 制动中", "-"));
    applyStatusStyle(lb.sim_braking, !msg.train_braking);

    lb.sim_coasting->setText(boolStr(msg.train_coasting, "滑行中", "-"));

    lb.sim_EB_applied->setText(boolStr(msg.EB_applied, "🔴 EB施加", "✔ 正常"));
    applyStatusStyle(lb.sim_EB_applied, !msg.EB_applied);

    lb.sim_ATO_release->setText(boolStr(msg.ATO_release, "✔ 释放", "✗ 未释放"));
    applyStatusStyle(lb.sim_ATO_release, msg.ATO_release);

    lb.sim_traction_en->setText(boolStr(msg.traction_enabled));
    lb.sim_TC1_key->setText(boolStr(msg.TC1_key_state));
    lb.sim_TC2_key->setText(boolStr(msg.TC2_key_state));
    lb.sim_mode_set->setText(msg.train_mode_set);

    // ATO 信息
    lb.sim_TC1_ATP_cut->setText(boolStr(msg.TC1_ATP_cutout, "✔ 切除", "-"));
    applyStatusStyle(lb.sim_TC1_ATP_cut, !msg.TC1_ATP_cutout);

    lb.sim_TC2_ATP_cut->setText(boolStr(msg.TC2_ATP_cutout, "✔ 切除", "-"));
    applyStatusStyle(lb.sim_TC2_ATP_cut, !msg.TC2_ATP_cutout);

    lb.sim_TC1_ATO_PB->setText(boolStr(msg.TC1_ATO_start_PB));
    lb.sim_TC2_ATO_PB->setText(boolStr(msg.TC2_ATO_start_PB));
    lb.sim_TC1_change_PB->setText(boolStr(msg.TC1_auto_change_end_PB));
    lb.sim_TC2_change_PB->setText(boolStr(msg.TC2_auto_change_end_PB));
    lb.sim_TC1_door_mode->setText(msg.TC1_door_mode);
    lb.sim_TC2_door_mode->setText(msg.TC2_door_mode);

    QString eventString = "当前故障:";
    for(int i = 0; i<10; i++){
        if(msg.event_type_map[i] == 1){
            if(i == 0){
                eventString.append("ATO");
            }
            if(i == 1){
                eventString.append("  ATP");
            }
            if(i == 2){
                eventString.append("  无电事件");
            }
            if(i == 3){
                eventString.append("电线脱落事件");
            }
        }
    }
    lb.sim_ats_fault->setText(eventString);
    applyStatusStyle(lb.sim_ats_fault, false);

    // 车门
    lb.sim_A_door_close->setText(boolStr(msg.A_doors_close_state, "✔ 已关", "✗ 未关"));
    applyStatusStyle(lb.sim_A_door_close, msg.A_doors_close_state);

    lb.sim_A_door_open->setText(boolStr(msg.A_door_open_request, "请求开门", "-"));
    lb.sim_A_door_close_req->setText(boolStr(msg.A_door_close_request, "请求关门", "-"));

    lb.sim_B_door_close->setText(boolStr(msg.B_doors_close_state, "✔ 已关", "✗ 未关"));
    applyStatusStyle(lb.sim_B_door_close, msg.B_doors_close_state);

    lb.sim_B_door_open->setText(boolStr(msg.B_door_open_request, "请求开门", "-"));
    lb.sim_B_door_close_req->setText(boolStr(msg.B_door_close_request, "请求关门", "-"));
}

// ======================================================
//  刷新 ATS 侧标签（ATSJsonMsg 全字段）
// ======================================================
void DataRelayWidget::refreshATSLabels(TrainDisplayLabels &lb, const ATSJsonMsg &msg)
{
    if(msg.ats_data.control_type == 4){
        lb.sim_ats_fault->setText("当前故障:");
    }

    // ---------- ATP/ATO ----------
    ATSATPATOInfoJsonMsg atp;
    ATSSIMFaultInfo ats_fault;
    atp.fromATPATOMsg(msg.ats_atp_ato_info_json_msg); // 做拷贝防止访问到非法内存,下面的拷贝操作亦是如此(2026/4/28改)
    ats_fault.fromATSSIMFaultInfo(msg.ats_sim_fault_info_json_msg);

    lb.ats_train_code->setText(atp.train_code);
    lb.ats_car_mode->setText(atp.current_car_mode);
    lb.ats_op_mode->setText(atp.current_operation_mode);

    bool faultOk = ats_fault.fault_type.isEmpty();
    lb.ats_fault->setText(ats_fault.fault_type.isEmpty() ? "正常" : ats_fault.fault_type);
    applyStatusStyle(lb.ats_fault, faultOk);

    lb.ats_A_door_enable->setText(boolStr(atp.A_doors_enable));
    lb.ats_A_door_open->setText(boolStr(atp.A_door_open_cmd, "✔ 开", "-"));
    lb.ats_A_door_close->setText(boolStr(atp.A_door_close_cmd, "✔ 关", "-"));
    lb.ats_B_door_enable->setText(boolStr(atp.B_doors_enable));
    lb.ats_B_door_open->setText(boolStr(atp.B_door_open_cmd, "✔ 开", "-"));
    lb.ats_B_door_close->setText(boolStr(atp.B_door_close_cmd, "✔ 关", "-"));

    lb.ats_ATP_EB->setText(boolStr(atp.ATP_EB_cmd, "✔ EB缓解", "🔴 EB施加"));
    applyStatusStyle(lb.ats_ATP_EB, atp.ATP_EB_cmd);

    lb.ats_ATO_activate->setText(boolStr(atp.ATO_activate));
    applyStatusStyle(lb.ats_ATO_activate, atp.ATO_activate);

    lb.ats_ATO_avail->setText(boolStr(atp.ATO_available));
    applyStatusStyle(lb.ats_ATO_avail, atp.ATO_available);

    lb.ats_ATO_traction->setText(boolStr(atp.ATO_traction_cmd, "✔ 牵引", "-"));
    lb.ats_ATO_brake->setText(boolStr(atp.ATO_brake_cmd, "⚠ 制动", "-"));

    lb.ats_change_ends->setText(boolStr(atp.auto_change_ends_cmd));
    lb.ats_changeend_act->setText(boolStr(atp.auto_changeend_activate));
    lb.ats_changeend_avail->setText(boolStr(atp.auto_changeend_available));

    lb.ats_ATO_speed->setText(QString::number(atp.ATO_rec_speed));

    // ---------- MMI ----------
    ATSMMIInfoJsonMsg mmi;
    mmi.fromMMIMsg(msg.ats_mmi_info_json_msg); // 做拷贝防止访问到非法内存,下面的拷贝操作亦是如此(2026/4/28改)

    lb.ats_brake_sign->setText(mmi.brake_sign);
    lb.ats_run_speed->setText(QString::number(mmi.train_run_speed));
    lb.ats_ATP_speed->setText(QString::number(mmi.ATP_recommended_speed));
    lb.ats_EM_speed->setText(QString::number(mmi.EM_brake_trigger_speed));
    lb.ats_drive_state->setText(mmi.train_driving_state);
    lb.ats_turn_back->setText(mmi.train_turn_back);
    lb.ats_pos_depot->setText(mmi.position_with_depot);

    lb.ats_next_skip->setText(boolStr(mmi.next_station_skip, "✔ 跳停", "-"));
    lb.ats_cur_detain->setText(boolStr(mmi.current_station_detain, "✔ 扣车", "-"));

    lb.ats_station_stop->setText(mmi.train_station_stop);
    lb.ats_dwell_time->setText(mmi.dwell_time);
    lb.ats_PSD->setText(mmi.current_PSD_state);

    lb.ats_close_door_info->setText(boolStr(mmi.close_door_info, "✔ 提示关门", "-"));
    lb.ats_dep_req->setText(boolStr(mmi.departure_request, "✔ 有请求", "-"));
    applyStatusStyle(lb.ats_dep_req, mmi.departure_request);

    lb.ats_unit_number->setText(mmi.train_unit_number);
    lb.ats_dest_number->setText(mmi.destination_number);
    lb.ats_real_number->setText(mmi.train_real_number);

    // ---------- 线路设备（道岔 / 信号机）----------
    ATSLineDeviceDataJsonMsg dev;
    dev.fromLineDataMsg(msg.ats_line_device_data_json_msg); // 做拷贝防止访问到非法内存,  << f0 83 00 01 lock addl $0x1,(%rax)>>这里报段错误一般指的就是访问了无效地址(2026/4/28改)
    // if (dev.send_status) {
    // 更新道岔下拉，保留当前选中的 key
    updateMapCombo(lb.ats_turnout_combo, lb.ats_turnout_value, dev.turnout);
    // 更新信号机下拉，保留当前选中的 key
    updateMapCombo(lb.ats_signal_combo,  lb.ats_signal_value,  dev.signal);
    // }
}

// ======================================================
//  工具函数
// ======================================================
QString DataRelayWidget::boolStr(bool v, const QString &trueStr, const QString &falseStr)
{
    return v ? trueStr : falseStr;
}

void DataRelayWidget::applyStatusStyle(QLabel *lb, bool ok)
{
    if (!lb) return;
    lb->setStyleSheet(ok ? kStyleOk : kStyleErr);
}

// 增量更新 QComboBox：新增 key 追加，已有 key 只更新 userData（value）
// 保留用户当前选中的 key，更新后同步刷新 valueLabel
void DataRelayWidget::updateMapCombo(QComboBox *combo, QLabel *valueLabel, QMap<QString, QString> map)
{
    if (!combo || !valueLabel) return;

    combo->blockSignals(true);

    QString currentKey = combo->currentText();

    for (auto it = map.constBegin(); it != map.constEnd(); ++it) {
        int idx = combo->findText(it.key());
        if (idx == -1) {
            combo->addItem(it.key(), QVariant(it.value()));
        } else {
            combo->setItemData(idx, QVariant(it.value()));
        }
    }

    // 恢复选中项
    int restoreIdx = combo->findText(currentKey);
    combo->setCurrentIndex(restoreIdx >= 0 ? restoreIdx : 0);

    combo->blockSignals(false);

    // 手动刷新 label，不走信号路径
    int cur = combo->currentIndex();
    valueLabel->setText(cur >= 0 ? combo->itemData(cur).toString() : QString("-"));
}