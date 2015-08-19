#include <helix/order_book.hh>
#include <iostream>
#include <chrono>

using namespace helix::core;

using clock_type = std::chrono::high_resolution_clock;

auto test_add(order_book& ob, unsigned long count)
{
    auto start = clock_type::now();
    for (unsigned long i = 0; i < count; i++) {
        order o{i, 8000, 10, side_type::buy, i};
        ob.add(std::move(o));
    }
    auto end = clock_type::now();
    return end - start;
}

auto test_remove(order_book& ob, unsigned long count)
{
    auto start = clock_type::now();
    for (unsigned long i = 0; i < count; i++) {
        ob.remove(i);
    }
    auto end = clock_type::now();
    return end - start;
}

int main()
{
    unsigned long count = 20000000;

    order_book ob{"AXP", 0};
    auto add_duration = test_add(ob, count);
    auto remove_duration = test_remove(ob, count);

    std::cout << "order_book::add()    " << std::chrono::duration_cast<std::chrono::nanoseconds>(add_duration).count() / count << " ns/op" << std::endl;
    std::cout << "order_book::remove() " << std::chrono::duration_cast<std::chrono::nanoseconds>(remove_duration).count() / count << " ns/op" << std::endl;
}
