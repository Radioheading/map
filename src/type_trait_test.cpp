//
// Created by Radioheading on 2023/3/26.
//
#include <mymap.hpp>
#include <iostream>

/**
 *
 * this is the checking of type_trait
 * in map.hpp, we defined whether the two iterator classes are assignable
 * so we can make different templates for them with the checking function having the same name
 *
 * @wankupi, @clorf6, many thanks!
 */
template<class right_type>
void assignable_check(right_type check, sjtu::my_true_type) {
  std::cout << "the iterator to key: " << check->first << " is assignable!\n";
}

template<class wrong_type>
void assignable_check(wrong_type check, sjtu::my_false_type) {
  std::cout << "the iterator to key: " << check->first << " isn't assignable!\n";
}

template<class type>
void type_trait_tester(type check) {
  typename sjtu::my_type_trait<type>::iterator_assignable iterator_assignable;
  assignable_check(check, iterator_assignable);
}

using namespace sjtu;

int main() {
  map<int, char> my_map;
  for (int i = 0; i < 10; ++i) {
    my_map[i] = 'a' + i;
  }
  auto todo_1 = my_map.begin();
  auto todo_2 = --my_map.cend();
  type_trait_tester(my_map.begin());
  type_trait_tester(--my_map.cend());
  my_map[todo_1->first] = 'z';
  my_map[todo_2->first] = 'z';
  return 0;
}