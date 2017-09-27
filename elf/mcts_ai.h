#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <atomic>
#include <type_traits>
#include "ai.h"

namespace elf {

using namespace std;

template <typename S, typename A, typename LowS = S, typename LowA = A>
class MCTSAI : public AI_T<S, A> {
public:
    MCTSAI(bool persistent_tree) : _persistent_tree(false) { 
        _ts.reset(new mcts::TreeSearch<LowS, LowA>(_options));
    }

    const mcts::TSOptions &options() const { return _options; }

protected:
    mcts::TSOptions _options;
    unique_ptr<mcts::TreeSearchT<LowS, LowA>> _ts;
    bool _persistent_tree;

    // A low dimensional representation of the current situation.
    bool on_act(Tick t, A *a, const std::atomic_bool *done) override {
        // Planning on the low-dimensional representation.
        const LowS *low_s = is_same<LowS, S>::value ? s_ptr() : new LowS(s());

        if (_options.persistent_tree) {
            for (const LowA &prev_move : low_s->last_opponent_moves()) {
                _ts.TreeAdvance(prev_move);
            }
        }

        pair<LowA, float> res = _ts.Run(*low_s);
        const LowA &low_a = res.first; 
        cout << "Action: " << low_a << " Score:" << res.second << endl << flush;
        _ts.TreeAdvance(low_a);

        // Remove s if it is created.
        if (! is_same<LowS, S>::value) delete low_s;

        // Reply the action.
        return is_same<LowA, A>::value ? *a = low_a : low_a.convert_to(a);
    }
};

}  // namespace elf