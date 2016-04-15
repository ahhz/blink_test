#include <gtest/gtest.h>

#include <blink/iterator/zip_iterator.h>
#include <blink/iterator/zip_range.h>
#include <blink/iterator/transform_range.h>
#include <blink/iterator/range_algebra.h>
#include <blink/iterator/range_range.h>

#include <list>
#include <vector>


struct uncopyable_iterator : public boost::iterator_facade<uncopyable_iterator, int, boost::incrementable_traversal_tag >
{
public:
  uncopyable_iterator(std::vector<int>& data, int i = 0) : m_data(data), m_i(i)
  {}

  uncopyable_iterator(const uncopyable_iterator& that)
  //  :m_data(that.m_data), m_i(that.m_i)
  //{
  //  assert(false);
  //} 
  = delete;

  uncopyable_iterator(uncopyable_iterator&& that) : m_i(that.m_i), m_data(that.m_data)
  {}
  uncopyable_iterator& operator=(const uncopyable_iterator&) = delete;
  uncopyable_iterator& operator=(uncopyable_iterator&& that)
  {
    m_data = std::move(that.m_data);
    m_i = that.m_i;
  }
  void increment()
  {
    ++m_i;
  }

  bool equal(const uncopyable_iterator& that) const
  {
    return m_i == that.m_i;
  }

  int& dereference() const
  {
    return m_data[m_i];
  }

  int m_i;
  std::vector<int>& m_data;
};

// The range cannot be copied and the iterators neither
struct uncopyable_range
{
  uncopyable_range(const std::vector<int>& data) : m_data(data) {}
  uncopyable_range(const uncopyable_range& that) 
  //  : m_data(that.m_data)
  //{
  //  assert(false);
 // }
  = delete;
  uncopyable_range(uncopyable_range&& that) : m_data(std::move(that.m_data)) {}
  uncopyable_range& operator=(const uncopyable_range&) = delete;
  uncopyable_range& operator=(uncopyable_range&& that)
  {
    m_data = std::move(that.m_data);
    that.m_data.clear(); // just to avoid confusion
    return *this;
  }

  typedef uncopyable_iterator iterator;

  iterator begin()
  {
    return iterator(m_data);
  }
  iterator end()
  {
    return iterator(m_data, static_cast<int>(m_data.size()));
  }

  std::vector<int> m_data;
};



bool test_zip_range_ref()
{
  std::vector<int> a = { 1, 2, 3, 4 };
  std::list<int> b = { 2, 4, 6, 8 };
   
  auto z = blink::iterator::make_zip_range(std::ref(a), std::ref(b));

  int sumproduct = 0;
  for (auto i = z.begin(); i != z.end(); ++i)
  {
    sumproduct += std::get<0>(*i) * (std::get<1>(*i) - 1);
  }
  return sumproduct == 1 * 1 + 2 * 3 + 3 * 5 + 4 * 7;
}

bool test_zip_range_ref_uncopyable()
{
  uncopyable_range a(std::vector<int>{1, 2, 3, 4});
  uncopyable_range b(std::vector<int>{2, 4, 6, 8});
 
  auto z = blink::iterator::make_zip_range(std::ref(a), std::ref(b));

  int sumproduct = 0;
  for (auto i = z.begin(); i != z.end(); ++i)
  {
    sumproduct += std::get<0>(*i) * (std::get<1>(*i) - 1);
  }
  return sumproduct == 1 * 1 + 2 * 3 + 3 * 5 + 4 * 7;
}

bool test_zip_range_move_uncopyable()
{
  uncopyable_range a(std::vector<int>{1, 2, 3, 4});
  uncopyable_range b(std::vector<int>{2, 4, 6, 8});

  auto z = blink::iterator::make_zip_range(std::move(a), std::move(b));

  int sumproduct = 0;
  for (auto i = z.begin(); i != z.end(); ++i)
  {
    sumproduct += std::get<0>(*i) * (std::get<1>(*i) - 1);
  }
  return sumproduct == 1*1 + 2*3 + 3*5 +4*7;
}

bool test_zip_range_single_range()
{
  uncopyable_range a(std::vector<int>{1, 2, 3, 4});
  
  auto z = blink::iterator::make_zip_range(std::move(a));

  int sum = 0;
  for (auto i = z.begin(); i != z.end(); ++i)
  {
    sum += std::get<0>(*i);
  }
  return sum == 1+2+3+4;
}

bool test_zip_range_triple_range()
{
  auto a = std::vector<int>{1, 2, 3, 4};
  uncopyable_range b(std::vector<int>{2, 4, 6, 8});
  uncopyable_range c(std::vector<int>{3, 6, 9, 12});

  auto z = blink::iterator::make_zip_range(a, std::move(b), std::ref(c));

  int sum = 0;
  for (auto i = z.begin(); i != z.end(); ++i)
  {
    sum += std::get<0>(*i) *std::get<1>(*i)*std::get<2>(*i);
  }
  return sum == 1*2*3 + 2*4*6 + 3*6*9 + 4*8*12;
}

bool test_zip_range_unequal_length()
{
  auto a = std::vector<int>{1, 2, 3, 4};
  uncopyable_range b(std::vector<int>{2, 4, 6, 8, 10, 12});
  uncopyable_range c(std::vector<int>{3, 6, 9, 12});

  auto z = blink::iterator::make_zip_range(a, std::move(b), std::ref(c));

  int sum = 0;
  for (auto i = z.begin(); i != z.end(); ++i)
  {
    sum += std::get<0>(*i) *std::get<1>(*i)*std::get<2>(*i);
  }
  return sum == 1 * 2 * 3 + 2 * 4 * 6 + 3 * 6 * 9 + 4 * 8 * 12;
}
bool test_zip_range_assigning()
{
  auto a = std::vector<int>{1, 2, 3, 4};
  uncopyable_range b(std::vector<int>{2, 4, 6, 8});

  auto z = blink::iterator::make_zip_range(std::ref(a), std::move(b));
  for (auto i = z.begin(); i != z.end(); ++i)
  {
    std::get<0>(*i) *= (std::get<1>(*i)-1);
  }
  int sum = 0;
  for (auto&& i : a)
  {
    sum += i;
  }
  return sum == 1 * 1 + 2 * 3 + 3 * 5 + 4 * 7;
}

bool test_zip_range_range_based_for()
{
  auto a = std::vector<int>{1, 2, 3, 4};
  uncopyable_range b(std::vector<int>{2, 4, 6, 8});

  auto z = blink::iterator::make_zip_range(std::ref(a), std::move(b));
  for (auto&& i : z)
  {
    std::get<0>(i) = std::get<0>(i) * (std::get<1>(i) - 1);
  }
  int sum = 0;
  for (auto&& i : a)
  {
    sum += i;
  }
  return sum == 1 * 1 + 2 * 3 + 3 * 5 + 4 * 7;
}

bool test_zip_iterator()
{
  std::vector<int> a = { 1, 2, 3, 4 };
  std::list<int> b = { 2, 4, 6, 8 };
  auto i = a.begin();
  auto j = b.begin();
  auto i_end = a.end();
  auto j_end = b.end();

  auto z = blink::iterator::make_zip_iterator(i, j);
  auto z_end = blink::iterator::make_zip_iterator(i_end, j_end);
  int sumproduct = 0;
  for (; z != z_end; ++z)
  {
    sumproduct += std::get<0>(*z) * (std::get<1>(*z) - 1);
  }
  return sumproduct == 1 * 1 + 2 * 3 + 3 * 5 + 4 * 7;
}

bool test_zip_iterator_ref()
{
  std::vector<int> a = { 1, 2, 3, 4 };
  std::list<int> b = { 2, 4, 6, 8 };
  auto i = a.begin();
  auto j = b.begin();
  auto i_end = a.end();
  auto j_end = b.end();

  auto z = blink::iterator::make_zip_iterator(std::ref(i), j);
  auto z_end = blink::iterator::make_zip_iterator(i_end, j_end);
  int sumproduct = 0;
  for (; z != z_end; ++z)
  {
    sumproduct += std::get<0>(*z) * (std::get<1>(*z) - 1);
  }
  return i == i_end && j == b.begin();
}


bool test_zip_iterator_assign()
{
  std::vector<int> a = { 1, 2, 3, 4 };
  std::list<int> b = { 2, 4, 6, 8 };
  auto i = a.begin();
  auto j = b.begin();
  auto i_end = a.end();
  auto j_end = b.end();

  auto z = blink::iterator::make_zip_iterator(std::ref(i), j);
  auto z_end = blink::iterator::make_zip_iterator(i_end, j_end);
  int sumproduct = 0;
  for (; z != z_end; ++z)
  {
    std::get<0>(*z) = std::get<0>(*z)  * (std::get<1>(*z) - 1);
  }
  int sum = 0;
  for (auto&& i : a)
  {
    sum += i;
  }
  return sum == 1 * 1 + 2 * 3 + 3 * 5 + 4 * 7;
}

bool test_zip_iterator_stride()
{
  std::vector<int> a = { 1, 2, 3, 4, 5, 6 };
  std::vector<int> b = { 6, 7, 8, 9, 10, 11 };
  auto i = a.begin();
  auto j = b.begin();
  auto i_end = a.end();
  auto j_end = b.end();

  auto z = blink::iterator::make_zip_iterator(std::ref(i), j);
  auto z_end = blink::iterator::make_zip_iterator(i_end, j_end);
  int sum = 0;
  for (; z < z_end; z+=2)
  {
    sum += std::get<0>(*z)  * (std::get<1>(*z) - 1);
  }
  return sum == 1 * 5 + 3 * 7 + 5 * 9;
}

bool test_zip_iterator_backward()
{
  std::vector<int> a = { 1, 2, 3, 4, 5, 6 };
  std::vector<int> b = { 6, 7, 8, 9, 10, 11 };
  auto i = a.end() - 1;
  auto j = b.end() - 1;
 
  auto z = blink::iterator::make_zip_iterator(std::ref(i), j);
  int sum = 0;
  for (int count = 0; count < 2; ++count)
  {
    sum += std::get<0>(*z)  * (std::get<1>(*z) - 1);
    z -= 2;
  }
  return sum == 6 * 10 + 4 * 8;
}

bool test_transform_range_lambda()
{
  std::vector<int> a = { 10, 20, 30, 40 };
  std::vector<int> b = { 100, 200, 300, 400};
  auto fun = [](int a, int b) {return 5 * a + b; };
  auto tr = blink::iterator::make_transform_range(fun, a, b);
  int sum = 0;
  for (auto&& i : tr)
  {
    sum += i;
  }
  return sum == 100 + 5 * 10 + 200 + 5 * 20 + 300 + 5 * 30 + 400 + 5 * 40;
}

bool test_transform_range_object()
{
  std::vector<int> a = { 10, 20, 30, 40 };
  std::vector<int> b = { 100, 200, 300, 400 };
  auto tr = blink::iterator::make_transform_range(std::plus<>{}, a, b);
  int sum = 0;
  for (auto&& i : tr)
  {
    sum += i;
  }
  return sum == 100 + 10 + 200 + 20 + 300 + 30 + 400 + 40;
}

int add(int a, int b) { return a + b; };
bool test_transform_range_function()
{
  std::vector<int> a = { 10, 20, 30, 40 };
  std::vector<int> b = { 100, 200, 300, 400 };
  auto tr = blink::iterator::make_transform_range(add, a, b);
  int sum = 0;
  for (auto&& i : tr)
  {
    sum += i;
  }
  return sum == 100 + 10 + 200 + 20 + 300 + 30 + 400 + 40;
}
bool test_transform_range_single()
{
  std::vector<int> a = { 10, 20, 30, 40 };
   auto fun = [](int a) {return 5 * a; };
  auto tr = blink::iterator::make_transform_range(fun, a);
  int sum = 0;
  for (auto&& i : tr)
  {
    sum += i;
  }
  return sum == 5 * 10 + 5 * 20 + 5 * 30 + 5 * 40;
}

bool test_range_algebra_reference()
{
  std::vector<int> a = { 10, 20, 30, 40 };
  auto ra = blink::iterator::range_algebra_ref(a); // reference
  ra = ra + 5;
  return *(ra.begin()+2) == 35 && a[2] == 35;
}

bool test_range_algebra_reference2()
{
  std::vector<int> a = { 10, 20, 30, 40 };
  auto ra = blink::iterator::range_algebra(std::ref(a)); // reference
  ra = ra + 5;
  return *(ra.begin() + 2) == 35 && a[2] == 35;
}

bool test_range_algebra_copy()
{
  std::vector<int> a = { 10, 20, 30, 40 };
  auto ra = blink::iterator::range_algebra(a); // copy
  ra = ra + 5;
  return *(ra.begin() + 2) == 35 && a[2] == 30;
}
bool test_range_algebra_move()
{
  uncopyable_range a(std::vector<int>{ 10, 20, 30, 40 });
  auto ra = blink::iterator::range_algebra(std::move(a)); // move
  ra = ra + 5;
  return *(++ra.begin()) == 25;
}
bool test_range_algebra_operators()
{
  uncopyable_range a(std::vector<int>{ 10, 20, 30, 40 });
  uncopyable_range b(std::vector<int>{ 1, 2, 3, 4 });
  auto ra = blink::iterator::range_algebra(std::move(a)); // move
  auto rb = blink::iterator::range_algebra(std::move(b)); // move
  ra = -ra + 5 * rb - ra * 3 / rb + ra % 7;
  return *(++ra.begin()) == -20 + 5 * 2 - 20 * 3 / 2 + 20 % 7;
}

bool test_range_algebra_transform_all_ranges()
{
  uncopyable_range a(std::vector<int>{ 10, 20, 30, 40 });
  uncopyable_range b(std::vector<int>{ 1, 2, 3, 4 });
  auto ra = blink::iterator::range_algebra(std::move(a)); // move
  auto rb = blink::iterator::range_algebra(std::move(b)); // move
  auto out = make_range_algebra_transform(std::plus<>{}, std::ref(ra), std::ref(rb));
  return *(++out.begin()) == 20 + 2;
}

bool test_range_algebra_transform_mixed_1()
{
   uncopyable_range b(std::vector<int>{ 1, 2, 3, 4 });
  auto rb = blink::iterator::range_algebra(std::move(b)); // move
  auto out = make_range_algebra_transform(std::plus<>{}, 1, std::ref(rb));
  return *(++out.begin()) == 1 + 2;
}

bool test_range_algebra_transform_mixed_2()
{
  uncopyable_range a(std::vector<int>{ 10, 20, 30, 40 });
  auto ra = blink::iterator::range_algebra(std::move(a)); // move
  auto out = make_range_algebra_transform(std::plus<>{}, std::ref(ra), 3);
  return *(++out.begin()) == 20 + 3;
}

bool test_range_range()
{
  std::vector<int> a = { 1, 2, 3, 4 };
  std::vector<int> b = { 2, 4, 8, 16 };
  std::vector<int> c = { 3, 9, 27, 81 };
  
  std::vector<std::vector<int> > abc = { a, b, c };
  auto rr = blink::iterator::make_range_zip_range(std::ref(abc));

  int product = 1;
  for (auto&& i : rr)
  {
    int sum = 0;
    for (auto&& j : i)
    {
      sum += j;
    }
    product *= sum;
  }
  return product == (1 + 2 + 3) * (2 + 4 + 9) * (3 + 8 + 27) * (4 + 16 + 81);

}

bool test_range_range_uncopyable()
{
  std::vector<int> a = { 1, 2, 3, 4 };
  std::vector<int> b = { 2, 4, 8, 16 };
  std::vector<int> c = { 3, 9, 27, 81 };

  std::vector<uncopyable_range> abc;
  abc.emplace_back(uncopyable_range{ a });
  abc.emplace_back(uncopyable_range{ b }); 
  abc.emplace_back(uncopyable_range{ c });
  
  auto rr = blink::iterator::make_range_zip_range(std::ref(abc));

  int product = 1;
  for (auto&& i : rr)
  {
    int sum = 0;
    for (auto&& j : i)
    {
      sum += j;
    }
    product *= sum;
  }
  return product == (1 + 2 + 3) * (2 + 4 + 9) * (3 + 8 + 27) * (4 + 16 + 81);

}

// Tests factorial of 0.
TEST(Iterator, ZipRange) {
  EXPECT_TRUE(test_zip_range_ref());
  EXPECT_TRUE(test_zip_range_ref_uncopyable());
  EXPECT_TRUE(test_zip_range_move_uncopyable());
  EXPECT_TRUE(test_zip_range_single_range());
  EXPECT_TRUE(test_zip_range_triple_range());
  EXPECT_TRUE(test_zip_range_unequal_length());
  EXPECT_TRUE(test_zip_range_assigning());
  EXPECT_TRUE(test_zip_range_range_based_for());
}

TEST(Iterator, ZipIterator) {
  EXPECT_TRUE(test_zip_iterator());
  EXPECT_TRUE(test_zip_iterator_ref());
  EXPECT_TRUE(test_zip_iterator_assign());
  EXPECT_TRUE(test_zip_iterator_stride());
  EXPECT_TRUE(test_zip_iterator_backward());
}

TEST(Iterator, TransformRange) {
  EXPECT_TRUE(test_transform_range_lambda());
  EXPECT_TRUE(test_transform_range_object());
  EXPECT_TRUE(test_transform_range_function());
  EXPECT_TRUE(test_transform_range_single());
}

TEST(Iterator, RangeAlgebra) {
  EXPECT_TRUE(test_range_algebra_reference());
  EXPECT_TRUE(test_range_algebra_reference2());
  EXPECT_TRUE(test_range_algebra_copy());
  EXPECT_TRUE(test_range_algebra_move());
  EXPECT_TRUE(test_range_algebra_operators());
  EXPECT_TRUE(test_range_algebra_transform_all_ranges());
  EXPECT_TRUE(test_range_algebra_transform_mixed_1());
  EXPECT_TRUE(test_range_algebra_transform_mixed_2());
}
TEST(Iterator, RangeRange) {
  EXPECT_TRUE(test_range_range());
  EXPECT_TRUE(test_range_range_uncopyable());
}


