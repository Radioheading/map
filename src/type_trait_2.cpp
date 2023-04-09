#include <iostream>
#include "map.hpp"

using namespace sjtu;

template<typename T>
void whether_assignable(const T &data) {
  if (my_type_traits<T>::check == assign_check::yes) {
    std::cout << "the iterator of key: " << data->first << " is assignable!\n";
  }
  if (my_type_traits<T>::check == assign_check::no) {
    std::cout << "the iterator of key: " << data->first << " isn't assignable!\n";
  }
}

template<typename T, typename U>
void same_check(const T &cmp_1, const U &cmp_2) {
  if (cmp<T, U>::value) {
    std::cout << "iterators of keys: " << cmp_1->first << " and " << cmp_2->first << " have the same type!\n";
  } else {
    std::cout << "iterators of keys: " << cmp_1->first << " and " << cmp_2->first << " have different types!\n";
  }
}
int main() {
  puts("checking the assignable trait of iterators: yes/no");
  map<int, char> my_map;
  for (int i = 0; i < 10; ++i) {
    my_map[i] = 'a' + i;
  }
  auto todo_1 = my_map.begin();
  auto todo_2 = --my_map.cend();
  whether_assignable(todo_1);
  whether_assignable(todo_2);
  my_map[todo_1->first] = 'z';
  my_map[todo_2->first] = 'z';
  same_check(todo_1, todo_2);
  same_check(todo_1, ++todo_1);
  return 0;
}