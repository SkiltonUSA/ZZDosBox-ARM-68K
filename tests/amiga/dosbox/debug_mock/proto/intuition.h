#pragma once
struct Window *BuildEasyRequestArgs(struct Window*,struct EasyStruct*,ULONG,void*);
LONG SysReqHandler(struct Window*,ULONG*,LONG);
void FreeSysRequest(struct Window*);
