/* GPL-2.0-or-later. DOSBox EMS board and XMS share the same guest page pool. */
#include "dosbox.h"
#include "compat.h"
extern void EMS_Init(Section*),EMS_ShutDown(Section*),XMS_Init(Section*),XMS_ShutDown(Section*);
bool db_ems_enabled=true,db_xms_enabled=true,db_vcpi_enabled=false,db_umb_enabled=false;
static bool opened;
void db_memory_services_open(){
 db_memory_pool_reset();opened=true;
 XMS_Init(nullptr);EMS_Init(nullptr);
}
void db_memory_services_close(){
 if(!opened)return;
 EMS_ShutDown(nullptr);XMS_ShutDown(nullptr);opened=false;
}
