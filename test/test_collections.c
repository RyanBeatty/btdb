#include "unity.h"

#include "collections.h"

void setUp(void) {}
void tearDown(void) {}

void test_PushBack(void) {
  CharPtrVec* vec = MakeCharPtrVec();
  for (uint64_t i = 0; i < 12; ++i) {
    PushBack(vec, "a");
  }
  TEST_ASSERT_EQUAL_size_t(12, VEC_LENGTH(vec));
  CharPtrVecIt it;
  VEC_FOREACH(it, vec) { TEST_ASSERT_EQUAL(*it, "a"); }
}

void test_Erase(void) {
  CharPtrVec* vec = MakeCharPtrVec();
  TEST_ASSERT_EQUAL(0, VEC_LENGTH(vec));
  Erase(vec, 1);
  TEST_ASSERT_EQUAL(0, VEC_LENGTH(vec));
  PushBack(vec, "a");
  PushBack(vec, "b");
  PushBack(vec, "c");
  Erase(vec, 0);
  TEST_ASSERT_EQUAL("c", *Get(vec, 0));
  TEST_ASSERT_EQUAL("b", *Get(vec, 1));
  TEST_ASSERT_EQUAL(NULL, Get(vec, 2));
  Erase(vec, 1);
  TEST_ASSERT_EQUAL("c", *Get(vec, 0));
  TEST_ASSERT_EQUAL(NULL, Get(vec, 1));
}

void test_Get(void) {
  CharPtrVec* vec = MakeCharPtrVec();
  TEST_ASSERT_EQUAL(NULL, Get(vec, 0));
  PushBack(vec, "foo");
  TEST_ASSERT_EQUAL("foo", *Get(vec, 0));
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_AverageThreeBytes_should_AverageMidRangeValues);
  RUN_TEST(test_PushBack);
  RUN_TEST(test_Erase);
  RUN_TEST(test_Get);
  return UNITY_END();
}