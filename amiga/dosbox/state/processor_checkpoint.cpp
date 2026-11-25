/* GPL-2.0-or-later. Called with guest execution stopped between instructions. */
#include "processor_checkpoint.h"
#include <stdint.h>
#include <string.h>
bool db_processor_checkpoint_save(void *buffer,size_t size){
 if(!buffer||size!=db_processor_checkpoint_bytes)return false;
 uint8_t record[db_processor_checkpoint_bytes];
 if(!db_cpu_checkpoint_save(record,db_cpu_checkpoint_bytes)||
    !db_fpu_checkpoint_save(record+db_cpu_checkpoint_bytes,db_fpu_checkpoint_bytes))return false;
 memcpy(buffer,record,sizeof(record));return true;
}
bool db_processor_checkpoint_load(const void *buffer,size_t size){
 if(!buffer||size!=db_processor_checkpoint_bytes)return false;
 uint8_t record[db_processor_checkpoint_bytes];memcpy(record,buffer,sizeof(record));
 if(!db_cpu_checkpoint_validate(record,db_cpu_checkpoint_bytes)||
    !db_fpu_checkpoint_validate(record+db_cpu_checkpoint_bytes,db_fpu_checkpoint_bytes))return false;
 // Private immutable records, stopped execution and no intervening callbacks:
 // both deterministic loads now have exactly the inputs already validated.
 return db_cpu_checkpoint_load(record,db_cpu_checkpoint_bytes)&&
        db_fpu_checkpoint_load(record+db_cpu_checkpoint_bytes,db_fpu_checkpoint_bytes);
}
