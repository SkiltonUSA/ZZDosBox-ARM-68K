#pragma once
struct Node {struct Node *ln_Succ;char *ln_Name;};
struct List {struct Node *lh_Head;};
struct MsgPort {struct Node mp_Node;struct List mp_MsgList;};
struct Message {struct Node mn_Node;unsigned mn_Length;struct MsgPort *mn_ReplyPort;};
