#include <SessionTrainData.h>

TrainData::TrainData()
{

}

TrainData::TrainData(const TrainData &other)
{
    vtrain_code = other.vtrain_code;
    track_code = other.track_code;
    offset_scale = other.offset_scale;
    direction = other.direction;
    sim_train_id = other.sim_train_id;
    sim_train_cab = other.sim_train_cab;
    trainee_id = other.trainee_id;
    train_speed_kmh = other.train_speed_kmh;
}

TrainData::~TrainData()
{

}
