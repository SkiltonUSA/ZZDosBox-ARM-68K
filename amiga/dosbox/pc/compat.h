/* GPL-2.0-or-later. Embedded memory managers; booted DOS owns its drivers. */
#pragma once
extern bool db_ems_enabled,db_xms_enabled,db_vcpi_enabled,db_umb_enabled;
void db_memory_pool_reset();
void db_memory_services_open();
void db_memory_services_close();
