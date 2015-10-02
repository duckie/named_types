#include <iostream>
#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <tuple>
#include <array>
#include <functional>
#include <named_types/named_tuple.hpp>
//#include <named_types/rt_named_tuple.hpp>

using namespace named_types;

namespace {
  struct name {
  };
  struct age {
    inline static char const* name() { return "age4"; }
  };
  struct taille;
  struct size;
  struct liste;
  struct func;
  struct birthday;  // Not used, for failure tests
  struct surname;
} 


class UnitTests : public ::testing::Test {
 protected:
  named_tuple<
    std::string(name)
    , int(age)
    , double(taille)
    , std::vector<int>(liste)
    , std::function<int(int)>(func)
    >
    test {"Roger", 47, 1.92, {1,2,3}, [](int a) { return a*a; } };
};

static named_tag<name> name_key;

TEST_F(UnitTests, Construction1) {
  EXPECT_EQ(std::string("Roger"), std::get<0>(test));
  EXPECT_EQ(std::string("Roger"), std::get<named_tag<name>>(test));
  EXPECT_EQ(std::string("Roger"), named_types::get<name>(test));
  EXPECT_EQ(std::string("Roger"), named_types::get<named_tag<name>>(test));
  EXPECT_EQ(std::string("Roger"), test.get<name>());
  EXPECT_EQ(std::string("Roger"), test.get<named_tag<name>>());
  EXPECT_EQ(std::string("Roger"), test[name_key]);
  EXPECT_EQ(std::string("Roger"), name_key(test));
}

TEST_F(UnitTests, Traits1) {
  using TupleType = decltype(test);
  EXPECT_TRUE(TupleType::has_tag<named_tag<name>>::value);
  EXPECT_FALSE(TupleType::has_tag<named_tag<birthday>>::value);
}

TEST_F(UnitTests, MakeTuple1) {
  auto t = make_named_tuple( name_key = std::string("Roger") );
  EXPECT_EQ(std::string("Roger"), t[name_key]);
}

TEST_F(UnitTests, Promotion1) {
  named_tag<name> name_k;
  named_tag<size> size_k;
  named_tag<surname> surname_k;
  named_tag<birthday> bday_k;
  
  auto t1 = make_named_tuple(
    name_k = std::string("Roger"),
    surname_k = std::string("LeGros"),
    size_k = 3u
  );

  auto t2 = make_named_tuple(
	  size_k = 4u,
	  name_k = std::string("Marcel"),
	  bday_k = 1e6
  );
  
  decltype(t1) t3(t2);  // Copy
  EXPECT_EQ(std::string("Marcel"), t3[name_k]);
  EXPECT_EQ(4u, t3[size_k]);

  decltype(t1) t4(std::move(t2));  // Move
  EXPECT_EQ(std::string("Marcel"), t4[name_k]);
  EXPECT_NE(std::string("Marcel"), t2[name_k]);
  EXPECT_EQ(4u, t4[size_k]);
}

TEST_F(UnitTests, Injection1) {
	named_tag<name> name_k;
	named_tag<size> size_k;
	named_tag<surname> surname_k;
	named_tag<birthday> bday_k;

	auto t1 = make_named_tuple(
		name_k = std::string("Roger"),
		surname_k = std::string("LeGros"),
		size_k = 3u
		);

	auto t2 = make_named_tuple(
		size_k = 4u,
		name_k = std::string("Marcel"),
		bday_k = 1e6
		);

	t1 = t2;  // Copy
	EXPECT_EQ(std::string("Marcel"), t1[name_k]);
	EXPECT_EQ(std::string("LeGros"), t1[surname_k]);
	EXPECT_EQ(4u, t1[size_k]);

	t1[name_k] = "Robert";
	EXPECT_EQ(std::string("Robert"), t1[name_k]);
	
	t2 = std::move(t1);  // Move
	EXPECT_EQ(std::string("Robert"), t2[name_k]);
	EXPECT_NE(std::string("Robert"), t1[name_k]);
	EXPECT_EQ(std::string("LeGros"), t1[surname_k]);
	EXPECT_EQ(4u, t1[size_k]);

}

TEST_F(UnitTests, TaggedTuple) {
  struct name : std::tag::basic_tag {};
  struct size : std::tag::basic_tag {};
  struct yo : std::tag::basic_tag {};
  using T1 = std::tagged_tuple<name(std::string),size(size_t),yo(name)>;
  T1 t {"Roger",3, {}};

  EXPECT_EQ(0, T1::tag_index<name>::value);
  EXPECT_EQ(std::string("Roger"), std::get<T1::tag_index<name>::value>(t));
  EXPECT_EQ(std::string("Roger"), std::get<name>(t));
  
  EXPECT_EQ(1, T1::tag_index<size>::value);
  EXPECT_EQ(3, std::get<T1::tag_index<size>::value>(t));
  EXPECT_EQ(3, std::get<size>(t));

  auto t2 = t;
  EXPECT_EQ(3, std::get<size>(t2));
}

//TEST_F(UnitTests, Experiment1) {
  //std::cout << string_literal<'a','b','c'>::hash << std::endl;
//}
