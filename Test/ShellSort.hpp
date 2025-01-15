#pragma once

template <class RandomIt>
void ShellSort(RandomIt begin, RandomIt end) {
    for (auto dk{ (end - begin) / 2 }; dk > 0; dk /= 2) { // 每一趟的增量 dk 是前一趟的一半
        for (RandomIt i{ begin + dk + 1 }; i != end; ++i) {
            RandomIt j{ i - dk };
            if (*i < *j) {
                auto pivot{ *i };
                for (; j >= begin && pivot < *j; j -= dk) {
                    *(j + dk) = *j;
                }
                *(j + dk) = pivot;
            }
        }
    }
}

