// diff 可以找出两个列表元素的差异。这里用于找出两个列表的差异，即找出两个 std::vector<HostsLine> 的差异。
#pragma once
#include <vector>

template <typename T>
struct diff_result
{
    std::vector<T> only_in_first;
    std::vector<T> only_in_second;
    std::vector<T> in_both;
};

// diff_lists 找出两个列表的差异，不考虑重复元素！因为我们认为DNS解析不会返回重复的记录，同时Hosts文件中的d2hs块由软件维护，软件正常运行时也不可能产生重复的记录。
template <typename T>
diff_result<T> diff_lists(const std::vector<T> &v1, const std::vector<T> &v2)
{
    diff_result<T> result;

    // 创建拷贝以避免修改原vector，并进行排序
    std::vector<T> sorted_v1(v1);
    std::vector<T> sorted_v2(v2);
    std::sort(sorted_v1.begin(), sorted_v1.end());
    std::sort(sorted_v2.begin(), sorted_v2.end());

    auto it1 = sorted_v1.begin();
    auto it2 = sorted_v2.begin();

    while (it1 != sorted_v1.end() && it2 != sorted_v2.end())
    {
        if (*it1 == *it2)
        {
            result.in_both.push_back(*it1);
            ++it1;
            ++it2;
        }
        else if (*it1 < *it2)
        {
            result.only_in_first.push_back(*it1);
            ++it1;
        }
        else
        {
            result.only_in_second.push_back(*it2);
            ++it2;
        }
    }

    // 处理第一个列表中剩余的元素
    while (it1 != sorted_v1.end())
    {
        result.only_in_first.push_back(*it1);
        ++it1;
    }

    // 处理第二个列表中剩余的元素
    while (it2 != sorted_v2.end())
    {
        result.only_in_second.push_back(*it2);
        ++it2;
    }

    return result;
}
