#include <sys/sysctl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "oko.h"

int
retrieve_type(machine *machine)
{
    int rc;
    size_t len;
    
    rc = sysctlbyname("hw.targettype", NULL, &len, NULL, 0);
    if (rc != 0) {
        return (rc);
    }
    machine->type = malloc(len);
    rc = sysctlbyname("hw.targettype", machine->type, &len, NULL, 0);
    
    return (rc);
}

int
retrieve_model(machine *machine)
{
    int rc;
    size_t len;
    
    rc = sysctlbyname("hw.model", NULL, &len, NULL, 0);
    if (rc != 0) {
        return (rc);
    }
    machine->model = malloc(len);
    rc = sysctlbyname("hw.model", machine->model, &len, NULL, 0);
    
    return (rc);
}

int
retrieve_hostname(machine *machine)
{
    int rc;
    size_t len;
    
    rc = sysctlbyname("kern.hostname", NULL, &len, NULL, 0);
    if (rc != 0) {
        return (rc);
    }
    machine->hostname = malloc(len);
    rc = sysctlbyname("kern.hostname", machine->hostname, &len, NULL, 0);
    
    return (rc);
}

int
retrieve_cpu_arch(machine *machine)
{
    int rc;
    size_t len;
    
    rc = sysctlbyname("hw.machine", NULL, &len, NULL, 0);
    if (rc != 0) {
        return (rc);
    }
    machine->cpu.arch = malloc(len);
    rc = sysctlbyname("hw.machine", machine->cpu.arch, &len, NULL, 0);
    
    return (rc);
}

int
retrieve_cpu_model(machine *machine)
{
    int rc;
    size_t len;
    
    rc = sysctlbyname("machdep.cpu.brand_string", NULL, &len, NULL, 0);
    if (rc != 0) {
        return (rc);
    }
    machine->cpu.model = malloc(len);
    rc = sysctlbyname("machdep.cpu.brand_string", machine->cpu.model, &len, NULL, 0);
    
    return (rc);
}

int
retrive_ncpus(machine *machine)
{
    int rc;
    size_t len;
    
    len = sizeof(machine->cpu.ncpus);
    rc = sysctlbyname("hw.ncpu", &(machine->cpu.ncpus), &len, NULL, 0);
    
    return (rc);
}

int
retrive_physmem(machine *machine)
{
    int rc;
    size_t len;
    
    len = sizeof(machine->memory.physmem);
    rc = sysctlbyname("hw.memsize", &(machine->memory.physmem), &len, NULL, 0);
    
    return (rc);
}

int
retrieve_os_name(machine *machine)
{
    int rc;
    size_t len;
    
    rc = sysctlbyname("kern.ostype", NULL, &len, NULL, 0);
    if (rc != 0) {
        return (rc);
    }
    machine->os.name = malloc(len);
    rc = sysctlbyname("kern.ostype", machine->os.name, &len, NULL, 0);
    
    return (rc);
}

int
retrieve_os_release(machine *machine)
{
    int rc;
    size_t len;
    
    rc = sysctlbyname("kern.osrelease", NULL, &len, NULL, 0);
    if (rc != 0) {
        return (rc);
    }
    machine->os.release = malloc(len);
    rc = sysctlbyname("kern.osrelease", machine->os.release, &len, NULL, 0);
    
    return (rc);
}

int
retrieve_os_version(machine *machine)
{
    int rc;
    size_t len;
    
    rc = sysctlbyname("kern.version", NULL, &len, NULL, 0);
    if (rc != 0) {
        return (rc);
    }
    machine->os.version = malloc(len);
    rc = sysctlbyname("kern.version", machine->os.version, &len, NULL, 0);
    
    return (rc);
}

int
collect_info(machine *machine)
{
    int rc;
    
    /* id */
    machine->id = 0;
    
    /* type */
    rc = retrieve_type(machine);
    
    /* model */
    rc = retrieve_model(machine);
    
    /* hostname */
    rc = retrieve_hostname(machine);
    
    /* ipv4 */
    
    /* ipv6 */
    
    /* mac_address */
    
    /* cpu.arch */
    rc = retrieve_cpu_arch(machine);
    
    /* cpu.model */
    rc = retrieve_cpu_model(machine);
    
    /* cpu.ncpus */
    rc = retrive_ncpus(machine);
    
    /* memory.phys_mem */
    rc = retrive_physmem(machine);
    
    /* memory.swap */
    
    /* os.name */
    rc = retrieve_os_name(machine);
    
    /* os.release */
    rc = retrieve_os_release(machine);
    
    /* os.version */
    rc = retrieve_os_version(machine);
    
    return (rc);
}
