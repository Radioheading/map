/**
 * implement a container like std::map
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"
#include <typeinfo>
#include <iostream>
#include <cstdio>
// 注意调用 Compare 而不是默认的小于号

#define WTF

namespace sjtu {
template<
    class Key,
    class T,
    class Compare = std::less<Key>
>
class map {
 public:
  /**
   * the internal type of data.
   * it should have a default constructor, a copy constructor.
   * You can use sjtu::map as value_type by typedef.
   */
  typedef pair<const Key, T> value_type;
  enum { BLACK, RED };
  struct Node {
    value_type *val;
    Node *p, *left, *right;
    bool color; // false -- black, true -- red
    Node() : val(nullptr), p(nullptr), left(nullptr), right(nullptr), color(BLACK) {}
    Node(const Node &other) : p(nullptr), left(other.left), right(other.right), color(other.color) {
      if (other.val != nullptr) {
        val = new value_type(*(other.val));
      } else {
        val = nullptr;
      }
    }
    ~Node() {
      if (!val) return;
      // val->first.~Key(), val->second.~T();
      delete val;
    }
  };
  Node nil;
  void Proceed(Node *&node) {
    node->p = node->left = node->right = &nil;
  }

 private:
  Node *root = &nil;
  int siz; // No size_t!!!
  Compare cmp;
  bool equal(const Key &x, const Key &y) const {
    return (!cmp(x, y)) && (!cmp(y, x));
  }
  bool nequal(const Key &x, const Key &y) const {
    return cmp(x, y) || cmp(y, x);
  }

  Node *Search(Node *x, const Key &k) const {
    while (x != &nil && nequal(k, x->val->first)) {
      if (cmp(k, x->val->first)) {
        x = x->left;
      } else {
        x = x->right;
      }
    }
    return x;
  }
  Node *GetMin(Node *x) const {
    while (x->left && x->left != &nil) {
      x = x->left;
    }
    return x;
  }
  Node *GetMax(Node *x) const {
    while (x->right != &nil) {
      x = x->right;
    }
    return x;
  }
  Node *Successor(Node *x) const {
    if (x->right != &nil) {
      return GetMin(x->right);
    } else {
      Node *y = x->p;
      while (y != &nil && x == y->right) {
        x = y;
        y = y->p;
      }
      return y;
    }
  }
  Node *Predecessor(Node *x) const {
    if (x->left != &nil) {
      return GetMax(x->left);
    } else {
      Node *y = x->p;
      while (y != &nil && x == y->left) {
        x = y;
        y = y->p;
      }
      return y;
    }
  }
  void LeftRotate(Node *x) {
    Node *y = x->right;
    x->right = y->left;
    if (y->left != &nil) {
      y->left->p = x;
    }
    y->p = x->p;
    if (x->p == &nil) {
      root = y;
    } else if (x == x->p->left) {
      x->p->left = y;
    } else {
      x->p->right = y;
    }
    y->left = x;
    x->p = y;
  }
  void RightRotate(Node *x) {
    Node *y = x->left;
    x->left = y->right;
    if (y->right != &nil) {
      y->right->p = x;
    }
    y->p = x->p;
    if (x->p == &nil) {
      root = y;
    } else if (x == x->p->right) {
      x->p->right = y;
    } else {
      x->p->left = y;
    }
    y->right = x;
    x->p = y;
  }
  void InternalInsertFixup(Node *z) {
    while (z->p->color == RED) {
      if (z->p == z->p->p->left) {
        Node *y = z->p->p->right;
        if (y->color == RED) {
          z->p->color = BLACK;
          y->color = BLACK;
          z->p->p->color = RED;
          z = z->p->p;
        } else {
          if (z == z->p->right) {
            z = z->p;
            LeftRotate(z);
          }
          z->p->color = BLACK;
          z->p->p->color = RED;
          RightRotate(z->p->p);
        }
      } else {
        Node *y = z->p->p->left;
        if (y->color == RED) {
          z->p->color = BLACK;
          y->color = BLACK;
          z->p->p->color = RED;
          z = z->p->p;
        } else {
          if (z == z->p->left) {
            z = z->p;
            RightRotate(z);
          }
          z->p->color = BLACK;
          z->p->p->color = RED;
          LeftRotate(z->p->p);
        }
      }
    }
    root->color = BLACK;
  }
  Node *InternalInsert(Node *z) {
    Node *x = root, *y = &nil;
    while (x != &nil) {
      y = x;
      if (cmp(z->val->first, x->val->first)) {
        x = x->left;
      } else {
        x = x->right;
      }
    }
    z->p = y;
    if (y == &nil) {
      root = z;
    } else if (cmp(z->val->first, y->val->first)) {
      y->left = z;
    } else {
      y->right = z;
    }
    z->left = z->right = &nil;
    z->color = RED;
    InternalInsertFixup(z);
    return z;
  }
  void InternalTransplant(Node *u, Node *v) {
    if (u->p == &nil) {
      root = v;
    } else if (u == u->p->left) {
      u->p->left = v;
    } else {
      u->p->right = v;
    }
    v->p = u->p;
  }
  void InternalDeleteFixup(Node *x) {
    while (x != root && x->color == BLACK) {
      if (x == x->p->left) {
        Node *w = x->p->right; // w is x's sibling
        if (w->color == RED) {
          w->color = BLACK;
          x->p->color = RED;
          LeftRotate(x->p);
          w = x->p->right;
        }
        if (w->left->color == BLACK && w->right->color == BLACK) {
          w->color = RED;
          x = x->p;
        } else {
          if (w->right->color == BLACK) {
            w->left->color = BLACK;
            w->color = RED;
            RightRotate(w);
            w = x->p->right;
          }
          w->color = x->p->color;
          x->p->color = BLACK;
          w->right->color = BLACK;
          LeftRotate(x->p);
          x = root;
        }
      } else {
        Node *w = x->p->left; // w is x's sibling
        if (w->color == RED) {
          w->color = BLACK;
          x->p->color = RED;
          RightRotate(x->p);
          w = x->p->left;
        }
        if (w->right->color == BLACK && w->left->color == BLACK) {
          w->color = RED;
          x = x->p;
        } else {
          if (w->left->color == BLACK) {
            w->right->color = BLACK;
            w->color = RED;
            LeftRotate(w);
            w = x->p->left;
          }
          w->color = x->p->color;
          x->p->color = BLACK;
          w->left->color = BLACK;
          RightRotate(x->p);
          x = root;
        }
      }
    }
    x->color = BLACK;
  }
  void InternalDelete(Node *z) {
    Node *x = &nil, *y = z;
    bool original_color = y->color;
    if (z->left == &nil) {
      x = z->right;
      InternalTransplant(z, z->right);
    } else if (z->right == &nil) {
      x = z->left;
      InternalTransplant(z, z->left);
    } else {
      y = GetMin(z->right); // successor
      original_color = y->color;
      x = y->right;
      if (y != z->right) {
        InternalTransplant(y, y->right);
        y->right = z->right;
        y->right->p = y;
      } else {
        x->p = y;
      }
      InternalTransplant(z, y);
      y->left = z->left;
      y->left->p = y;
      y->color = z->color;
    }
    if (original_color == BLACK) {
      InternalDeleteFixup(x);
    }
  }

  void Copy(Node *x, Node *y, Node *xnil, const Node *ynil) {
    if (y == nullptr) return;
    if (y == ynil) {
      x = xnil;
      return;
    }
    if (y->left == ynil || y->left == nullptr) {
      x->left = xnil;
    }
    if (y->left != ynil) {
      x->left = new Node(*(y->left));
      x->left->p = x;
      Copy(x->left, y->left, xnil, ynil);
    }
    if (y->right == ynil || y->right == nullptr) {
      x->right = xnil;
    }
    if (y->right != ynil) {
      x->right = new Node(*(y->right));
      x->right->p = x;
      Copy(x->right, y->right, xnil, ynil);
    }
  }
  // delete 时要特判 nil，防止 nil 重复删除
  void Delete(Node *x) {
    if (x == nullptr || x == &nil) {
      return;
    }
    if (x->left != nullptr && x->left != &nil) {
      Delete(x->left);
    }
    if (x->right != nullptr && x->right != &nil) {
      Delete(x->right);
    }
    delete x;
    x = nullptr;
  }
 public:
  /**
   * see BidirectionalIterator at CppReference for help.
   *
   * if there is anything wrong throw invalid_iterator.
   *     like it = map.begin(); --it;
   *       or it = map.end(); ++end();
   */
  class const_iterator;
  class iterator {
    friend class map;
   private:
    /**
     * TODO add data members
     *   just add whatever you want.
     */
    const map<Key, T, Compare> *cur;
    Node *ptr;
   public:
    iterator(const map *curr = nullptr, Node *ptrr = nullptr) : cur(curr), ptr(ptrr) {}
    iterator(const iterator &other) : cur(other.cur), ptr(other.ptr) {}

    /**
     * TODO iter++
     */
    iterator operator++(int) {
      if (ptr == nullptr || cur == nullptr || ptr == &(cur->nil)) {
        throw invalid_iterator();
      }
      iterator ret(*this);
      ptr = cur->Successor(ptr);
      return ret;
    }
    /**
     * TODO ++iter
     */
    iterator &operator++() {
      if (ptr == nullptr || cur == nullptr || ptr == &(cur->nil)) {
        throw invalid_iterator();
      }
      ptr = cur->Successor(ptr);
      return *this;
    }
    /**
     * TODO iter--
     */
    // nil 的 -- 要照顾到
    iterator operator--(int) {
      if (ptr == nullptr || cur == nullptr) {
        throw invalid_iterator();
      }
      try {
        if (ptr == &(cur->nil)) {
          iterator ret(*this);
          ptr = cur->GetMax(cur->root);
          return ret;
        }
        iterator ret(*this);
        return ret;
        ptr = cur->Predecessor(ptr);
      } catch (...) {
        throw invalid_iterator();
      }
    }
    /**
     * TODO --iter
     */
    iterator &operator--() {
      if (ptr == nullptr || cur == nullptr) {
        throw invalid_iterator();
      }
      if (ptr == &(cur->nil)) {
        ptr = cur->GetMax(cur->root);
        return *this;
      }
      ptr = cur->Predecessor(ptr);
      return *this;
    }
    /**
     * a operator to check whether two iterators are same (pointing to the same memory).
     */
    value_type &operator*() const {
      if (ptr == nullptr || cur == nullptr || ptr == &(cur->nil)) {
        throw invalid_iterator();
      }
      return *(ptr->val);
    }
    bool operator==(const iterator &rhs) const {
      if (cur != rhs.cur) {
        return false;
      }
      return (ptr == rhs.ptr);
    }
    bool operator==(const const_iterator &rhs) const {
      if (cur != rhs.cur) {
        return false;
      }
      return (ptr == rhs.ptr);
    }
    /**
     * some other operator for iterator.
     */
    bool operator!=(const iterator &rhs) const {
      return !((*this) == rhs);
    }
    bool operator!=(const const_iterator &rhs) const {
      return !((*this) == rhs);
    }

    /**
     * for the support of it->first.
     * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
     */
    value_type *operator->() const noexcept {
      if (ptr == nullptr || cur == nullptr || ptr == &(cur->nil)) {
        throw invalid_iterator();
      }
      return ptr->val;
    }
  };
  class const_iterator {
    // it should has similar member method as iterator.
    //  and it should be able to construct from an iterator.
    friend class map;
   private:
    const map<Key, T, Compare> *cur;
    const Node *ptr;
   public:
    const_iterator(const map *curr = nullptr, const Node *ptrr = nullptr) : cur(curr), ptr(ptrr) {}
    const_iterator(const map *curr, Node *ptrr) : cur(curr), ptr(ptrr) {}
    const_iterator(const const_iterator &other) : cur(other.cur), ptr(other.ptr) {}
    const_iterator(const iterator &other) : cur(other.cur), ptr(other.ptr) {}
    /**
     * TODO iter++
     */
    const_iterator operator++(int) {
      if (ptr == nullptr || cur == nullptr || ptr == &(cur->nil)) {
        throw invalid_iterator();
      }
      const_iterator ret(*this);
      ptr = cur->Successor(const_cast<Node *>(ptr));
      return ret;
    }
    /**
     * TODO ++iter
     */
    const_iterator &operator++() {
      if (ptr == nullptr || cur == nullptr || ptr == &(cur->nil)) {
        throw invalid_iterator();
      }
      ptr = cur->Successor(const_cast<Node *>(ptr));
      return *this;
    }
    /**
     * TODO iter--
     */
    const_iterator operator--(int) {
      if (ptr == nullptr || cur == nullptr) {
        throw invalid_iterator();
      }
      if (ptr == &(cur->nil)) {
        const_iterator ret(*this);
        ptr = cur->GetMax(cur->root);
        return ret;
      }
      const_iterator ret(*this);
      ptr = cur->Predecessor(const_cast<Node *>(ptr));
      return ret;
    }
    /**
     * TODO --iter
     */
    const_iterator &operator--() {
      if (ptr == nullptr || cur == nullptr) {
        throw invalid_iterator();
      }
      if (ptr == &(cur->nil)) {
        ptr = cur->GetMax(cur->root);
        return *this;
      }
      ptr = cur->Predecessor(const_cast<Node *>(ptr));
      return *this;
    }
    value_type &operator*() const {
      if (ptr == nullptr || cur == nullptr || ptr == &(cur->nil)) {
        throw invalid_iterator();
      }
      return *(ptr->val);
    }
    bool operator==(const iterator &rhs) const {
      if (cur != rhs.cur) {
        return false;
      }
      return (ptr == rhs.ptr);
    }
    bool operator==(const const_iterator &rhs) const {
      if (cur != rhs.cur) {
        return false;
      }
      return (ptr == rhs.ptr);
    }
    /**
     * some other operator for iterator.
     */
    bool operator!=(const iterator &rhs) const {
      return !((*this) == rhs);
    }
    bool operator!=(const const_iterator &rhs) const {
      return !((*this) == rhs);
    }

    /**
     * for the support of it->first.
     * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
     */
    value_type *operator->() const noexcept {
      if (ptr == nullptr || cur == nullptr || ptr == &(cur->nil)) {
        throw invalid_iterator();
      }
      return ptr->val;
    }
  };
  /**
   * TODO two constructors
   */
  map() {
    root = &nil;
    siz = 0;
  }
  map(const map &other) {
    if (this == &other) {
      return;
    }
    siz = other.siz;
    if (other.root != &other.nil && other.root != nullptr) {
      root = new Node(*(other.root));
      root->p = &nil;
      Copy(root, other.root, &nil, &(other.nil));
    }
  }
  /**
   * TODO assignment operator
   */
  map &operator=(const map &other) {
    if (this == &other) {
      return *this;
    }
    // std::cout << "fuck";
    Delete(root);
    // std::cout << "fuck2";
    if (other.root != &other.nil && other.root != nullptr) {
      root = new Node(*(other.root));
      root->p = &nil;
      Copy(root, other.root, &nil, &(other.nil));
    }
    siz = other.siz;
    return *this;
  }
  /**
   * TODO Destructors
   */
  ~map() {
    Delete(root);
    siz = 0;
  }
  /**
   * TODO
   * access specified element with bounds checking
   * Returns a reference to the mapped value of the element with key equivalent to key.
   * If no such element exists, an exception of type `index_out_of_bound'
   */
  T &at(const Key &key) {
    Node *x = Search(root, key);
    if (x == nullptr || x == &nil) {
      throw index_out_of_bound();
    }
    return x->val->second;
  }
  const T &at(const Key &key) const {
    Node *x = Search(root, key);
    if (x == nullptr || x == &nil) {
      throw index_out_of_bound();
    }
    return x->val->second;
  }
  /**
   * TODO
   * access specified element
   * Returns a reference to the value that is mapped to a key equivalent to key,
   *   performing an insertion if such key does not already exist.
   */
  T &operator[](const Key &key) {
    Node *x = Search(root, key);
    if (x == nullptr || x == &nil) {
      Node *y = new Node();
      Proceed(y);
      value_type *newVal = new pair<const Key, T>(key, T());
      y->val = newVal;
      InternalInsert(y);
      ++siz;
      return y->val->second;
    }
    return x->val->second;
  }
  /**
   * behave like at() throw index_out_of_bound if such key does not exist.
   */
  const T &operator[](const Key &key) const {
    Node *x = Search(root, key);
    if (x == nullptr || x == &nil) {
      throw index_out_of_bound();
    }
    return x->val->second;
  }
  /**
   * return a iterator to the beginning
   */
  iterator begin() {
//		if (root == nullptr || root == &nil) {
//			throw container_is_empty();
//		}
    Node *x = GetMin(root);
    return iterator(this, x);
  }
  const_iterator cbegin() const {
//		if (root == nullptr || root == &nil) {
//			throw container_is_empty();
//		}
    Node *x = GetMin(root);
    return const_iterator(this, x);
  }
  /**
   * return a iterator to the end
   * in fact, it returns past-the-end.
   */
  iterator end() {
    return iterator(this, &nil);
  }
  const_iterator cend() const {
    return const_iterator(this, &nil);
  }
  /**
   * checks whether the container is empty
   * return true if empty, otherwise false.
   */
  bool empty() const {
    return (siz == 0);
  }
  /**
   * returns the number of elements.
   */
  size_t size() const {
    return siz;
  }
  /**
   * clears the contents
   */
  void clear() {
    // 跑一遍深搜
    Delete(root);
    siz = 0;
    root = &nil;
  }
  /**
   * insert an element.
   * return a pair, the first of the pair is
   *   the iterator to the new element (or the element that prevented the insertion),
   *   the second one is true if insert successfully, or false.
   */
  pair<iterator, bool> insert(const value_type &value) {
    Node *x = Search(root, value.first);
    // std::cout << "1" << std::endl;
    if (x == &nil || x == nullptr) {
      Node *z = new Node();
      Proceed(z);
      ++siz;
      value_type *newVal = new value_type(value);
      z->val = newVal;
      InternalInsert(z);
      return pair<iterator, bool>(iterator(this, z), true);
    }
    return pair<iterator, bool>(iterator(this, x), false);
    // 记得 ++siz，如果成功插入再加
  }
  /**
   * erase the element at pos.
   *
   * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
   */
  void erase(iterator pos) {
    // --siz if success
    if (pos == end() || pos.cur != this || pos.ptr == &nil || pos.ptr == nullptr) {
      throw invalid_iterator();
    }
    --siz;
    InternalDelete(pos.ptr);
    delete (pos.ptr);
  }
  /**
   * Returns the number of elements with key
   *   that compares equivalent to the specified argument,
   *   which is either 1 or 0
   *     since this container does not allow duplicates.
   * The default method of check the equivalence is !(a < b || b > a)
   */
  size_t count(const Key &key) const {
    Node *x = Search(root, key);
    if (x == &nil || x == nullptr) return 0u;
    return 1u;
  }
  /**
   * Finds an element with key equivalent to key.
   * key value of the element to search for.
   * Iterator to an element with key equivalent to key.
   *   If no such element is found, past-the-end (see end()) iterator is returned.
   */
  iterator find(const Key &key) {
    Node *x = Search(root, key);
    if (x == &nil || x == nullptr) return end();
    return iterator(this, x);
  }
  const_iterator find(const Key &key) const {
    Node *x = Search(root, key);
    if (x == &nil || x == nullptr) return cend();
    return const_iterator(this, x);
  }
};

}

#endif
