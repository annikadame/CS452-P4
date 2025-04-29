
#include "harness/unity.h"
#include "../src/lab.h"
// NOTE: Due to the multi-threaded nature of this project. Unit testing for this
// project is limited. I have provided you with a command line tester in
// the file app/main.cp. Be aware that the examples below do not test the
// multi-threaded nature of the queue. You will need to use the command line
// tester to test the multi-threaded nature of your queue. Passing these tests
// does not mean your queue is correct. It just means that it can add and remove
// elements from the queue below the blocking threshold.
void setUp(void) {
// set stuff up here
}
void tearDown(void) {
// clean stuff up here
}
void test_create_destroy(void)
{
queue_t q = queue_init(10);
TEST_ASSERT_TRUE(q != NULL);
queue_destroy(q);
}
void test_queue_dequeue(void)
{
queue_t q = queue_init(10);
TEST_ASSERT_TRUE(q != NULL);
int data = 1;
enqueue(q, &data);
TEST_ASSERT_TRUE(dequeue(q) == &data);
queue_destroy(q);
}
void test_queue_dequeue_multiple(void)
{
queue_t q = queue_init(10);
TEST_ASSERT_TRUE(q != NULL);
int data = 1;
int data2 = 2;
int data3 = 3;
enqueue(q, &data);
enqueue(q, &data2);
enqueue(q, &data3);
TEST_ASSERT_TRUE(dequeue(q) == &data);
TEST_ASSERT_TRUE(dequeue(q) == &data2);
TEST_ASSERT_TRUE(dequeue(q) == &data3);
queue_destroy(q);
}
void test_queue_dequeue_shutdown(void)
{
queue_t q = queue_init(10);
TEST_ASSERT_TRUE(q != NULL);
int data = 1;
int data2 = 2;
int data3 = 3;
enqueue(q, &data);
enqueue(q, &data2);
enqueue(q, &data3);
TEST_ASSERT_TRUE(dequeue(q) == &data);
TEST_ASSERT_TRUE(dequeue(q) == &data2);
queue_shutdown(q);
TEST_ASSERT_TRUE(dequeue(q) == &data3);
TEST_ASSERT_TRUE(is_shutdown(q));
TEST_ASSERT_TRUE(is_empty(q));
queue_destroy(q);
}



// NEW TESTS
void test_dequeue_empty_after_shutdown(void)
{
    queue_t q = queue_init(5);
    TEST_ASSERT_TRUE(q != NULL);
    queue_shutdown(q);
    TEST_ASSERT_TRUE(dequeue(q) == NULL);
    queue_destroy(q);
}
void test_enqueue_after_shutdown(void)
{
    queue_t q = queue_init(5);
    TEST_ASSERT_TRUE(q != NULL);
    int data = 42;
    queue_shutdown(q);
    // Try to enqueue after shutdown. Should not crash.
    enqueue(q, &data);
    TEST_ASSERT_TRUE(is_empty(q)); // No items should be enqueued after shutdown
    queue_destroy(q);
}

void test_fill_queue_to_capacity(void)
{
    queue_t q = queue_init(3);
    TEST_ASSERT_TRUE(q != NULL);
    int a = 1, b = 2, c = 3;
    enqueue(q, &a);
    enqueue(q, &b);
    enqueue(q, &c);
    TEST_ASSERT_FALSE(is_empty(q));
    //Dequeuing
    TEST_ASSERT_TRUE(dequeue(q) == &a);
    TEST_ASSERT_TRUE(dequeue(q) == &b);
    TEST_ASSERT_TRUE(dequeue(q) == &c);
    TEST_ASSERT_TRUE(is_empty(q));
    queue_destroy(q);
}



int main(void) {
UNITY_BEGIN();
RUN_TEST(test_create_destroy);
RUN_TEST(test_queue_dequeue);
RUN_TEST(test_queue_dequeue_multiple);
RUN_TEST(test_queue_dequeue_shutdown);
RUN_TEST(test_dequeue_empty_after_shutdown);
RUN_TEST(test_enqueue_after_shutdown);
RUN_TEST(test_fill_queue_to_capacity);
return UNITY_END();
}
