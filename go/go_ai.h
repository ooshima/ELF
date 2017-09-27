#pragma once

#include "ai.h"

using namespace std;

class DirectPredictAI: public AIWithComm {
public:
    using Data = AIWithComm::Data;

    void get_last_pi(vector<pair<Coord, float>> *output_pi) const {
        if (data().size() == 0) {
            cout << "DirectPredictAI: history empty!" << endl;
            throw std::range_error("DirectPredictAI: history empty!");
        }
        const vector<float> &pi = data().newest().pi;

        output_pi->clear();
        const auto &bf = s().last_extractor();
        for (size_t i = 0; i < pi.size(); ++i) {
            Coord m = bf.Action2Coord(i);
            output_pi->push_back(make_pair(m, pi[i])); 
        }
    }

    float get_last_value() const { return data().newest().V; }

protected:
    void extract(Data *data) override {
        auto &gs = data->newest();
        gs.move_idx = s().GetPly();
        gs.winner = 0;
        const auto &bf = s().last_extractor();
        bf.Extract(&gs.s);
    }

    bool handle_response(const Data &data, Coord *c) override {
        auto action = data.newest().a;
        *c = s().last_extractor().Action2Coord(action);
        return true;
    }
};

// TODO: An AI surrogate for human playing.

// void Next(int64_t action) override;