#include <cstdlib>
#include <limits>
#include <list>
#include <sstream>
#include <string>
#include <vector>
#include <qi/application.hpp>
#include <gtest/gtest.h>
#include <qi/detail/conceptpredicate.hpp>
#include <qi/functional.hpp>
#include <qi/detail/conceptpredicate.hpp>
#include <qi/range.hpp>
#include <qi/future.hpp>
#include <qi/type/traits.hpp>
#include "tools.hpp"

TEST(FunctionalPolymorphicConstantFunction, RegularNonVoid)
{
  using namespace qi;
  using namespace qi::detail;
  using F = PolymorphicConstantFunction<int>;
  const auto incr = [](F& f) {
    ++f.ret;
  };
  // F is regular because int is.
  ASSERT_TRUE(isRegular(incrRange(F{0}, F{100}, incr)));
}

TEST(FunctionalPolymorphicConstantFunction, RegularVoid)
{
  using namespace qi;
  using namespace qi::detail;
  using F = PolymorphicConstantFunction<void>;
  ASSERT_TRUE(isRegular({F{}}));
}

struct NonRegular
{
  int i;
  friend bool operator==(NonRegular a, NonRegular b)
  {
    return &a == &b;
  }
  friend bool operator<(NonRegular a, NonRegular b)
  {
    return &a < &b;
  }
};

TEST(FunctionalPolymorphicConstantFunction, NonRegularNonVoid)
{
  using namespace qi;
  using namespace qi::detail;
  using F = PolymorphicConstantFunction<NonRegular>;
  auto incr = [](F& f) {
    ++f.ret.i;
  };
  // F is not regular because NonRegular isn't.
  ASSERT_FALSE(isRegular(incrRange(F{{0}}, F{{100}}, incr)));
}

TEST(FunctionalPolymorphicConstantFunction, BasicNonVoid)
{
  using namespace qi;
  const char c = 'z';
  PolymorphicConstantFunction<unsigned char> f{c};
  ASSERT_EQ(c, f());
  ASSERT_EQ(c, f(1));
  ASSERT_EQ(c, f(2.345));
  ASSERT_EQ(c, f("abcd"));
  ASSERT_EQ(c, f(true));
  ASSERT_EQ(c, f(std::vector<int>{5, 7, 2, 1}));
  ASSERT_EQ(c, f(1, 2.345, "abcd", true));
}

TEST(FunctionalPolymorphicConstantFunction, BasicVoid)
{
  using namespace qi;
  PolymorphicConstantFunction<void> f;
  ASSERT_NO_THROW(f());
  ASSERT_NO_THROW(f(1));
  ASSERT_NO_THROW(f(2.345));
  ASSERT_NO_THROW(f("abcd"));
  ASSERT_NO_THROW(f(true));
  ASSERT_NO_THROW(f(std::vector<int>{5, 7, 2, 1}));
  ASSERT_NO_THROW(f(1, 2.345, "abcd", true));
}

namespace
{
  // For use to test isRegular only.
  // The returned strings are irrelevant, the only point is that these functions
  // are regular.
  std::string strbool0(bool x)
  {
    return x ? "test test" : "1, 2, 1, 2";
  }

  std::string strbool1(bool x)
  {
    return x ? "mic mic" : "Vous etes chauds ce soir?!";
  }
}

TEST(FunctionalCompose, Regular)
{
  using namespace qi;
  using namespace std;
  using C = Composition<string (*)(bool), bool (*)(float)>;
  ASSERT_TRUE(detail::isRegular({
    C{strbool0, isnan}, C{strbool0, isfinite}, C{strbool1, isinf}
  }));
}

TEST(FunctionalCompose, NonVoid)
{
  using namespace qi;

  auto half = [](int x) {
    return x / 2.f;
  };
  auto greater_1 = [](float x) {
    return x > 1.f;
  };
  auto half_greater_1 = compose(greater_1, half);
  static_assert(traits::Equal<bool, decltype(half_greater_1(3))>::value, "");

  ASSERT_TRUE(half_greater_1(3));
  ASSERT_FALSE(half_greater_1(1));
}

TEST(FunctionalCompose, Void)
{
  using namespace qi;
  using namespace qi::traits;
  const int uninitialized = std::numeric_limits<int>::max();
  int order = 0;
  int fOrder = uninitialized;
  int gOrder = uninitialized;
  auto f = [&](int) {
    fOrder = order++;
  };
  auto g = [&] {
    gOrder = order++;
  };
  auto k = compose(g, f);
  static_assert(Equal<void, decltype(k(3))>::value, "");

  ASSERT_EQ(uninitialized, fOrder);
  ASSERT_EQ(uninitialized, gOrder);
  k(3);
  ASSERT_EQ(0, fOrder);
  ASSERT_EQ(1, gOrder);
}

TEST(FunctionalCompose, Multi)
{
  using namespace qi;
  using namespace qi::traits;
  using std::string;

  auto half = [](int x) {
    return x / 2.f;
  };
  auto greater_1 = [](float x) {
    return x > 1.f;
  };
  auto str = [](bool x) -> string {
    return x ? "true" : "false";
  };

  auto f = compose(str, compose(greater_1, half));
  static_assert(Equal<string, decltype(f(3))>::value, "");

  ASSERT_EQ("true", f(3));
  ASSERT_EQ("false", f(1));
}

namespace
{
  template<typename G, typename F>
  qi::Composition<qi::traits::Decay<G>, qi::traits::Decay<F>> operator*(G&& g, F&& f)
  {
    return {g, f};
  }
} // namespace

TEST(FunctionalCompose, Associative)
{
  using namespace qi;
  using std::string;

  auto f = [](int x) {
    return x / 2.f;
  };
  auto g = [](float x) {
    return x > 1.f;
  };
  auto h = [](bool x) -> string {
    return x ? "true" : "false";
  };
  auto i = [](const string& x) -> std::size_t {
    return x.size();
  };

  auto a = ((i * h) * g) * f;
  auto b = (i * (h * g)) * f;
  auto c = i * (h * (g * f));
  auto d = (i * h) * (g * f);
  auto e = i * ((h * g) * f);

  ASSERT_EQ(a(3), b(3));
  ASSERT_EQ(b(3), c(3));
  ASSERT_EQ(c(3), d(3));
  ASSERT_EQ(d(3), e(3));

  ASSERT_EQ(a(0), b(0));
  ASSERT_EQ(b(0), c(0));
  ASSERT_EQ(c(0), d(0));
  ASSERT_EQ(d(0), e(0));
}

TEST(FunctionalCompose, Id)
{
  using namespace qi;
  using std::string;

  auto f = [](int x) {
    return x / 2.f;
  };
  auto g = [](float x) {
    return x > 1.f;
  };
  IdTransfo id;

  auto a = f * id;
  auto b = id * f;
  auto c = (g * f) * id;
  auto d = id * (g * f);

  ASSERT_EQ(a(3), b(3));
  ASSERT_EQ(c(3), d(3));

  ASSERT_EQ(a(0), b(0));
  ASSERT_EQ(c(0), d(0));
}

namespace
{
  struct X {
    bool b;
    bool operator==(X x) const {return b == x.b;}
  };

  template<typename T>
  struct ConstantUnit
  {
    T operator()() const
    {
      return T{};
    }
  };

  template<>
  struct ConstantUnit<qi::Future<void>>
  {
    qi::Future<void> operator()() const
    {
      return qi::Future<void>{nullptr};
    }
  };

  template<typename T>
  bool equal(const T& a, const T& b)
  {
    return a == b;
  }

  template<typename T>
  bool equal(const qi::Future<T>& a, const qi::Future<T>& b)
  {
    return a.value() == b.value();
  }
}

using types = testing::Types<
  X, qi::Future<bool>, boost::optional<bool>, std::list<bool>
>;

template<typename T>
struct FunctionalSemiLift0 : testing::Test
{
};

TYPED_TEST_CASE(FunctionalSemiLift0, types);

TYPED_TEST(FunctionalSemiLift0, NonVoidCodomain)
{
  using namespace qi;
  using T = TypeParam;

  auto positive = [](int i) {
    return i > 0;
  };
  auto unit = [](bool b) {
    return T{b};
  };
  auto f = semiLift(positive, unit);

  static_assert(traits::Equal<T, decltype(f(0))>::value, "");
  ASSERT_TRUE(equal(T{true}, f(1)));
  ASSERT_TRUE(equal(T{false}, f(-1)));
}

using void_types = testing::Types<
  X, qi::Future<void>, boost::optional<bool>, std::list<bool>
>;

template<typename T>
struct FunctionalSemiLift1 : testing::Test
{
};

TYPED_TEST_CASE(FunctionalSemiLift1, void_types);

TYPED_TEST(FunctionalSemiLift1, VoidCodomain)
{
  using namespace qi;
  using namespace qi::traits;
  using T = TypeParam;

  auto noop = [](int) {
  };
  ConstantUnit<T> unit;
  auto f = semiLift(noop, unit);

  static_assert(Equal<T, decltype(f(0))>::value, "");
  ASSERT_TRUE(equal(unit(), f(0)));
}

TYPED_TEST(FunctionalSemiLift1, VoidCodomainVoidDomain)
{
  using namespace qi;
  using namespace qi::traits;
  using T = TypeParam;

  auto noop = [] {
  };
  ConstantUnit<T> unit;
  auto f = semiLift(noop, unit);

  static_assert(Equal<T, decltype(f())>::value, "");
  ASSERT_TRUE(equal(unit(), f()));
}

TEST(FunctionalMoveAssign, Basic)
{
  using namespace qi;
  using MoveOnly = test::MoveOnly<int>;
  const int i = 3;
  MoveOnly original{i};
  MoveAssign<MoveOnly, MoveOnly> moveAssign{std::move(original)};
  MoveOnly x{i + 1};
  moveAssign(x); // x = std::move(original);
  ASSERT_EQ(i, x.value);
}

TEST(FunctionalIncr, Regular)
{
  using namespace qi;
  using namespace qi::detail;
  Incr<int> incr;
  ASSERT_TRUE(isRegular({incr})); // only one possible value because no state
}

TEST(FunctionalIncr, Arithmetic)
{
  using namespace qi;
  {
    Incr<int> incr;
    int x = 0;
    incr(x);
    ASSERT_EQ(1, x);
  }
  {
    Incr<double> incr;
    double x = 0.0;
    incr(x);
    ASSERT_EQ(1.0, x);
  }
}

TEST(FunctionalIncr, InputIterator)
{
  using namespace qi;
  using namespace std;
  stringstream ss{"youpi les amis"};
  using I = istream_iterator<string>;
  I b(ss);
  Incr<I> incr;
  ASSERT_EQ("youpi", *b);
  incr(b);
  ASSERT_EQ("les", *b);
  incr(b);
  ASSERT_EQ("amis", *b);
}

TEST(FunctionalDecr, Regular)
{
  using namespace qi;
  using namespace qi::detail;
  Decr<int> decr;
  ASSERT_TRUE(isRegular({decr})); // only one possible value because no state
}

TEST(FunctionalDecr, Arithmetic)
{
  using namespace qi;
  {
    Decr<int> decr;
    int x = 1;
    decr(x);
    ASSERT_EQ(0, x);
  }
  {
    Decr<double> decr;
    double x = 1.0;
    decr(x);
    ASSERT_EQ(0.0, x);
  }
}

TEST(FunctionalDecr, BidirectionalIterator)
{
  using namespace qi;
  using namespace std;
  Decr<list<string>::iterator> decr;
  list<string> l{"youpi", "les", "amis"};
  auto b = end(l);
  decr(b);
  ASSERT_EQ("amis", *b);
  decr(b);
  ASSERT_EQ("les", *b);
  decr(b);
  ASSERT_EQ("youpi", *b);
}

TEST(FunctionalIncr, IsomorphicIntegral)
{
  using namespace qi;
  {
    Incr<int> incr;
    auto inv = retract(incr);
    int i = 0;
    incr(i);
    inv(i);
    ASSERT_EQ(0, i);
  }
  {
    Incr<int> incr;
    auto inv = retract(incr);
    int i = 0;
    inv(i);
    incr(i);
    ASSERT_EQ(0, i);
  }
}

TEST(FunctionalIncr, IsomorphicBidirectionalIterator)
{
  using namespace qi;
  using namespace std;
  Incr<list<string>::iterator> incr;
  auto inv = retract(incr);
  list<string> l{"youpi", "les", "amis"};
  auto b = begin(l);
  ++b;
  incr(b);
  inv(b);
  ASSERT_EQ("les", *b);
  inv(b);
  incr(b);
  ASSERT_EQ("les", *b);
}

TEST(FunctionalDecr, IsomorphicIntegral)
{
  using namespace qi;
  {
    Decr<int> decr;
    auto inv = retract(decr);
    int i = 0;
    decr(i);
    inv(i);
    ASSERT_EQ(0, i);
  }
  {
    Decr<int> decr;
    auto inv = retract(decr);
    int i = 0;
    inv(i);
    decr(i);
    ASSERT_EQ(0, i);
  }
}

TEST(FunctionalDecr, IsomorphicBidirectionalIterator)
{
  using namespace qi;
  using namespace std;
  Decr<list<string>::iterator> decr;
  auto inv = retract(decr);
  list<string> l{"youpi", "les", "amis"};
  auto b = begin(l);
  ++b;
  decr(b);
  inv(b);
  ASSERT_EQ("les", *b);
  inv(b);
  decr(b);
  ASSERT_EQ("les", *b);
}
