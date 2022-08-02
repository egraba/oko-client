#include <check.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "testutils.h"

#include "oko.h"
 
START_TEST(test_retrieve_serial_number)
{
    machine m;
    char *serialnumber = (char *) malloc(12);
    
    retrieve_serialnumber(&m);
    execute("system_profiler SPHardwareDataType | grep Serial | awk '{print $4}'", serialnumber);
    
    ck_assert_str_eq(m.serialnumber, serialnumber);

    free(serialnumber);
}
END_TEST

START_TEST(test_retrieve_type)
{
    machine m;
    char *type = (char *) malloc(10);
    
    retrieve_type(&m);
    execute("sysctl hw.targettype | awk '{print $2}'", type);
    
    ck_assert_str_eq(m.type, type);

    free(type);
}
END_TEST

START_TEST(test_retrieve_model)
{
    machine m;
    char *model = (char *) malloc(100);

    retrieve_model(&m);
    execute("system_profiler SPHardwareDataType | grep 'Model Identifier' | awk '{print $3}'", model);
    
    ck_assert_str_eq(m.model, model);

    free(model);
}
END_TEST

START_TEST(test_retrieve_hostname)
{
    machine m;
    char *hostname = (char *) malloc(253);
    
    retrieve_hostname(&m);
    execute("hostname", hostname);

    ck_assert_str_eq(m.hostname, hostname);

    free(hostname);
}
END_TEST

START_TEST(test_retrieve_ip)
{
    machine m;
    char *ip = (char *) malloc(16);

    retrieve_ip(&m);
    execute("ifconfig | grep \"inet \" | grep -Fv 127.0.0.1 | awk '{print $2}'", ip);

    ck_assert_str_eq(m.network.ip, ip);

    free(ip);
}
END_TEST

START_TEST(test_retrieve_macaddress)
{
    machine m;
    char *macaddress = (char *) malloc(18);

    retrieve_macaddress(&m);
    execute("ifconfig en0 | grep \"ether \" | awk '{print $2}'", macaddress);

    ck_assert_str_eq(m.network.macaddress, macaddress);

    free(macaddress);
}
END_TEST

START_TEST(test_retrieve_cpu_arch)
{
    machine m;
    char *cpu_arch = (char *) malloc(32);

    retrieve_cpu_arch(&m);
    execute("sysctl hw.machine | awk '{print $2}'", cpu_arch);

    ck_assert_str_eq(m.cpu.arch, cpu_arch);

    free(cpu_arch);
}
END_TEST

START_TEST(test_retrieve_cpu_model)
{
    machine m;
    char *cpu_model = (char *) malloc(255);

    retrieve_cpu_model(&m);
    execute("sysctl machdep.cpu.brand_string | awk '{print $2\" \"$3\" \"$4\" \"$5\" \"$6\" \"$7}'", cpu_model);

    ck_assert_str_eq(m.cpu.model, cpu_model);

    free(cpu_model);
}
END_TEST

START_TEST(test_retrieve_ncpus)
{
    machine m;
    char *ncpus = (char *) malloc(3);

    retrieve_ncpus(&m);
    execute("sysctl hw.ncpu | awk '{print $2}'", ncpus);

    ck_assert_int_eq(m.cpu.ncpus, atoi(ncpus));

    free(ncpus);
}
END_TEST

START_TEST(test_retrieve_physmem)
{
    machine m;
    char *physmem = (char *) malloc(255);
    
    retrieve_physmem(&m);
    execute("sysctl hw.memsize | awk '{print $2}'", physmem);

    ck_assert_int_eq(m.memory.physmem, atol(physmem));

    free(physmem);
}
END_TEST

START_TEST(test_retrieve_os_name)
{
    machine m;
    char *os_name = malloc(255);
 
    retrieve_os_name(&m);
    execute("uname -s", os_name);

    ck_assert_str_eq(m.os.name, os_name);

    free(os_name);
}
END_TEST

START_TEST(test_retrieve_os_release)
{
    machine m;
    char *os_release = malloc(255);
    
    retrieve_os_release(&m);
    execute("uname -r", os_release);

    ck_assert_str_eq(m.os.release, os_release);

    free(os_release);
}
END_TEST

START_TEST(test_retrieve_os_version)
{
    machine m;
    char *os_version = (char *) malloc(255);
    
    retrieve_os_version(&m);
    execute("uname -v", os_version);
    
    ck_assert_str_eq(m.os.version, os_version);

    free(os_version);
}
END_TEST

START_TEST(test_retrieve_cpu_usage)
{
    usage u;
    int cu;

    /*
     * It is not possible to perform tests with enough accuracy.
     * Therefore, the test consists in checking that the sum of the different usage percentages is 100.
     */
    retrieve_cpu_usage(&u);
    cu = u.cpu.user + u.cpu.system + u.cpu.idle;
    ck_assert_int_eq(cu, 100);
}
END_TEST

START_TEST(test_retrieve_memory_usage)
{
    usage u;
    char *mem_used = (char *) malloc(10);
    char *mem_free = (char *) malloc(10);
    int upper_margin, lower_margin;

    retrieve_memory_usage(&u);

    execute("sysctl vm.pages | awk '{print $2*4096}'", mem_used);
    
    ck_assert_uint_eq(atol(mem_used), u.memory.used);

    execute("sysctl vm.page_free_count | awk '{print $2*4096}'", mem_free);
    
    /* 5% margin */
    upper_margin = u.memory.free + 0.05 * u.memory.free;
    lower_margin = u.memory.free - 0.05 * u.memory.free;
    long mf = atol(mem_free);
    if (mf < upper_margin) {
        ck_assert_uint_lt(mf, upper_margin);
    }
    else {
        ck_assert_uint_ge(mf, lower_margin);
    }

    free(mem_used);
    free(mem_free);
}
END_TEST

START_TEST(test_retrieve_memory_swapusage)
{
    usage u;
    char *swaptotal = (char *) malloc(10);
    char *swapused = (char *) malloc(10);
    char *swapfree = (char *) malloc(10);
    char *s = (char *) malloc(10);
    
    retrieve_memory_swapusage(&u);
    
    execute("sysctl -a | grep swapusage | awk '{print $4}'", swaptotal);
    sprintf(s, "%.2fM", u.memory.swaptotal / pow(1024, 2));

    ck_assert_str_eq(s, swaptotal);
    
    execute("sysctl -a | grep swapusage | awk '{print $7}'", swapused);
    sprintf(s, "%.2fM", u.memory.swapused / pow(1024, 2));

    ck_assert_str_eq(s, swapused);

    execute("sysctl -a | grep swapusage | awk '{print $10}'", swapfree);
    sprintf(s, "%.2fM", u.memory.swapfree / pow(1024, 2));

    ck_assert_str_eq(s, swapfree);

    free(swaptotal);
    free(swapused);
    free(swapfree);
    free(s);
}
END_TEST

Suite *
oko_suite()
{
    Suite *s;
    TCase *tc_machine;
    TCase *tc_usage;

    s = suite_create("oko");
    
    tc_machine = tcase_create("Machine");
    tcase_add_test(tc_machine, test_retrieve_serial_number);
    tcase_add_test(tc_machine, test_retrieve_type);
    tcase_add_test(tc_machine, test_retrieve_model);
    tcase_add_test(tc_machine, test_retrieve_ip);
    tcase_add_test(tc_machine, test_retrieve_macaddress);
    tcase_add_test(tc_machine, test_retrieve_hostname);
    tcase_add_test(tc_machine, test_retrieve_cpu_arch);
    tcase_add_test(tc_machine, test_retrieve_cpu_model);
    tcase_add_test(tc_machine, test_retrieve_ncpus);
    tcase_add_test(tc_machine, test_retrieve_physmem);
    tcase_add_test(tc_machine, test_retrieve_os_name);
    tcase_add_test(tc_machine, test_retrieve_os_release);
    tcase_add_test(tc_machine, test_retrieve_os_version);
    suite_add_tcase(s, tc_machine);

    tc_usage = tcase_create("Usage");
    tcase_add_test(tc_usage, test_retrieve_cpu_usage);
    tcase_add_test(tc_usage, test_retrieve_memory_usage);
    tcase_add_test(tc_usage, test_retrieve_memory_swapusage);
    suite_add_tcase(s, tc_usage);

    return s;
}

int
main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = oko_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
