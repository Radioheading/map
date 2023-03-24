#include "mymap.hpp"
#include <iostream>

using namespace sjtu;

using std::cin, std::cout, std::endl, std::string;
//void check(const map<int, int> &cmp) {
//  auto b = cmp.cbegin();
//  while (b != cmp.cend()) {
//    if (b.node->ls && b.node->ls->father != b.node || b.node->rs && b.node->rs->father != b.node) {
//      cout << b->first << ": failing" << '\n';
//    }
//    ++b;
//  }
//}
int main() {
  // freopen("myout.txt", "w", stdout);
  map<int, int> a;
  for (int i = 1; i <= 10; ++i) {
    a.insert(pair<int, int>(i, i));
    // cout << a.size() << ' ';
  }
//  for (int j = 1; j <= 10; ++j) {
//    if (a.find(j) == a.end()) continue;
//    cout << a.find(j).node->datum->first << ' ';
//    if (a.find(j).node->father) cout << a.find(j).node->father->datum->first << '\n';
//    else puts("");
//  }
  a[11] = 114514;
  cout << a.find(11)->second << '\n';
  return 0;
}