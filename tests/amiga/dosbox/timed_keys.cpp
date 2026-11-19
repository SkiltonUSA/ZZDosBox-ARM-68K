#include <cassert>
#include <sstream>
#include <vector>
#include <cstdio>
#include "timed_keys.h"
int main(){
 db_native::TimedKeys keys;
 std::istringstream source("100 52 1\n600 52 0\n1000 49 1\n1500 49 0\n");
 assert(keys.load(source,100));
 std::vector<db_native::KeyEvent> received;
 auto emit=[&](const db_native::KeyEvent &event){received.push_back(event);};
 keys.advance(99,emit);assert(received.empty());
 keys.advance(101,emit);assert(received.size()==1&&received.back().down==1);
 keys.advance(101,emit);assert(received.size()==1);
 keys.advance(1001,emit);assert(received.size()==3&&received[1].down==0&&received[2].key==49);
 keys.advance(1600,emit);assert(received.size()==4&&received.back().down==0);
 keys.advance(50000,emit);assert(received.size()==4);
 for(const char *bad:{"10 0 1\n","10 100 1\n","10 49 2\n","10 49\n","10 49 1 extra\n","10 49 1\n9 49 0\n","-1 49 1\n","4294967296 49 1\n"}){
  std::istringstream input(bad);assert(!keys.load(input,100));
  keys.advance(60000,emit);assert(received.size()==4); // Failed load preserves the consumed plan.
 }
 std::istringstream simultaneous("0 49 1\n0 49 0\n");assert(keys.load(simultaneous,100));
 keys.advance(0,emit);assert(received.size()==6&&received[4].down==1&&received[5].down==0);
 puts("PASS timed keyboard: irregular polls, single delivery, ordered catch-up and invalid schedules");
}
