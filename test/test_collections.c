#include "unity.h"

#include "collections.h"

void setUp(void) {}
void tearDown(void) {}

void test_AverageThreeBytes_should_AverageMidRangeValues(void) {
  TEST_ASSERT_EQUAL_CHAR('c', 'c');
}

void test_PushBack(void) {
  CharPtrVec* vec = MakeCharPtrVec();
  for (uint64_t i = 0; i < 12; ++i) {
    PushBack(vec, "a");
  }
  TEST_ASSERT_EQUAL_size_t(12, VEC_LENGTH(vec));
  CharPtrVecIt it;
  VEC_FOREACH(it, vec) { TEST_ASSERT_EQUAL(*it, "a"); }
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_AverageThreeBytes_should_AverageMidRangeValues);
  RUN_TEST(test_PushBack);
  return UNITY_END();
}