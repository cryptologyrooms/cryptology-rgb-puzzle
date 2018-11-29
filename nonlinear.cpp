#include <stdint.h>
#include <math.h>

void update_nonlinear_table(uint16_t * p_table, uint32_t maximum, uint8_t nsteps)
{
    uint32_t sqrt_max;
    uint32_t linear_step;
    uint32_t linear;
    uint32_t ngaps = nsteps - 1;

    if (p_table)
    {
        sqrt_max = sqrt(maximum);
        linear_step = (sqrt_max + (ngaps / 2)) / ngaps;
        linear = 0;
        for (uint8_t i = 0; i<ngaps; i++)
        {
            p_table[i] = linear * linear;
            linear += linear_step;
        }
        p_table[ngaps] = maximum;
    }
}

#ifdef UNIT_TEST

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

class BrightnessTableTestTest : public CppUnit::TestFixture  {

    CPPUNIT_TEST_SUITE(BrightnessTableTestTest);

    CPPUNIT_TEST(test_three_step_generation_with_maximum_of_4);
    CPPUNIT_TEST(test_three_step_generation_with_maximum_of_5);
    CPPUNIT_TEST(test_five_step_generation_with_maximum_of_4095);
    CPPUNIT_TEST(test_eight_step_generation_with_maximum_of_4095);

    CPPUNIT_TEST_SUITE_END();

    void test_three_step_generation_with_maximum_of_4()
    {
        uint16_t test_table[3];
        update_nonlinear_table(test_table, 4, 3);
        CPPUNIT_ASSERT_EQUAL((uint16_t)0U, test_table[0]);
        CPPUNIT_ASSERT_EQUAL((uint16_t)1U, test_table[1]);
        CPPUNIT_ASSERT_EQUAL((uint16_t)4U, test_table[2]);
    }

    void test_three_step_generation_with_maximum_of_5()
    {
        uint16_t test_table[3];
        update_nonlinear_table(test_table, 5, 3);
        CPPUNIT_ASSERT_EQUAL((uint16_t)0U, test_table[0]);
        CPPUNIT_ASSERT_EQUAL((uint16_t)1U, test_table[1]);
        CPPUNIT_ASSERT_EQUAL((uint16_t)5U, test_table[2]);
    }

    void test_five_step_generation_with_maximum_of_4095()
    {
        uint16_t test_table[5];
        update_nonlinear_table(test_table, 4095, 5);
        CPPUNIT_ASSERT_EQUAL((uint16_t)0U, test_table[0]);
        CPPUNIT_ASSERT_EQUAL((uint16_t)256U, test_table[1]);
        CPPUNIT_ASSERT_EQUAL((uint16_t)1024U, test_table[2]);
        CPPUNIT_ASSERT_EQUAL((uint16_t)2304U, test_table[3]);
        CPPUNIT_ASSERT_EQUAL((uint16_t)4095U, test_table[4]);
    }

    void test_eight_step_generation_with_maximum_of_4095()
    {
        uint16_t test_table[8];
        update_nonlinear_table(test_table, 4095, 8);
        CPPUNIT_ASSERT_EQUAL((uint16_t)0U, test_table[0]);
        CPPUNIT_ASSERT_EQUAL((uint16_t)81U, test_table[1]);
        CPPUNIT_ASSERT_EQUAL((uint16_t)324U, test_table[2]);
        CPPUNIT_ASSERT_EQUAL((uint16_t)729U, test_table[3]);
        CPPUNIT_ASSERT_EQUAL((uint16_t)1296U, test_table[4]);
        CPPUNIT_ASSERT_EQUAL((uint16_t)2025U, test_table[5]);
        CPPUNIT_ASSERT_EQUAL((uint16_t)2916U, test_table[6]);
        CPPUNIT_ASSERT_EQUAL((uint16_t)4095U, test_table[7]);
    }
};

int main()
{
   CppUnit::TextUi::TestRunner runner;
   
   CPPUNIT_TEST_SUITE_REGISTRATION( BrightnessTableTestTest );

   CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();

   runner.addTest( registry.makeTest() );
   runner.run();

   return 0;
}

#endif
