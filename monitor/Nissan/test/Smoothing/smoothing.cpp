#include <iostream>
#include <iomanip>

/* 
    COMPILATION INSTRUCTIONS
    g++ -std=c++11 smoothing.cpp -o demo
*/


class CurrentLimitSmoothing {
public:
    CurrentLimitSmoothing(float max, float alpha)
        : m_max_current(max),
          m_alpha(alpha),
          m_output(0.0f),
          m_initialized(false)
    {
    }

    float process(float input) {
        if (input <= 0.1f) {
            m_output = 0.0f;
            m_initialized = false;
            return 0.0f;
        }

        if (input > m_max_current) {
            input = m_max_current;
        }

        if (!m_initialized) {
            m_output = input;
            m_initialized = true;
            return m_output;
        }

        m_output = m_output + m_alpha * (input - m_output);

        // optional: prevent output from exceeding input
        if (m_output > input) {
            m_output = input;
        }

        return m_output;
    }

private:
    float m_max_current;
    float m_alpha;
    float m_output;
    bool m_initialized;
};

// static const int HIST_SIZE = 10;

// class CurrentLimitSmoothing {
// public:
//     CurrentLimitSmoothing(float max)
//         : m_max_current(max),
//           m_hist_index(0),
//           m_sum(0.0f)
//     {
//         for (int i = 0; i < HIST_SIZE; i++) {
//             m_hist_data[i] = 0.0f;
//         }
//     }

//     float process(float input) {
//         // immediate zero output for near-zero input
//         if (input <= 0.1f) {
//             m_sum = 0.0f;
//             m_hist_index = 0;
//             for (int i = 0; i < HIST_SIZE; i++) {
//                 m_hist_data[i] = 0.0f;
//             }
//             return 0.0f;
//         }

//         if (input > m_max_current) {
//             input = m_max_current;
//         }

//         // moving average over last HIST_SIZE samples
//         m_sum -= m_hist_data[m_hist_index];
//         m_hist_data[m_hist_index] = input;
//         m_sum += input;
//         m_hist_index = (m_hist_index + 1) % HIST_SIZE;

//         return m_sum / HIST_SIZE;
//     }

// private:
//     float m_max_current;
//     float m_hist_data[HIST_SIZE];
//     int m_hist_index;
//     float m_sum;
// };

int main() {
    CurrentLimitSmoothing smoother(100.0f, 0.2f);

    // 50 sample inputs
    float inputs[] = {
        0,20,5,10,0,10,20,
        0,22,24,11,0,17,19,
        0.0f, 0.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f,
        10.0f, 11.0f, 12.0f, 10.0f, 10.0f, 10.0f, 10.0f, 10.0f, 10.0f, 10.0f,
        10.0f,10.0f,10.0f,10.0f, 1.5f, 2.5f, 3.5f, 4.5f, 5.5f, 6.5f, 7.5f, 8.5f, 9.5f,
        8,8,7,7,6,6,5,5,15,5,0,3,3,2,2,1,1,0,0,
        10.5f, 9.0f, 8.0f, 7.0f, 6.0f, 5.0f, 0.0f, 3.0f, 2.0f, 1.0f,
        0.0f, 0.0f, 2.0f, 4.0f, 6.0f, 8.0f, 10.0f, 12.0f, 5.0f, 0.0f
    };

    std::cout << std::fixed << std::setprecision(2);
    std::cout << " idx | input | output\n";
    std::cout << "----------------------\n";

    for (int i = 0; i < 50; i++) {
        float out = smoother.process(inputs[i]);
        std::cout << std::setw(4) << i
                  << " | " << std::setw(5) << inputs[i]
                  << " | " << std::setw(6) << out << "\n";
    }

    return 0;
}