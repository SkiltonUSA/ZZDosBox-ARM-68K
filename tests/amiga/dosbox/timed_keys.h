/* Native research harness only. Times are emulated milliseconds, not poll counts. */
#ifndef DB_NATIVE_TIMED_KEYS_H
#define DB_NATIVE_TIMED_KEYS_H
#include <cstdint>
#include <istream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
namespace db_native {
struct KeyEvent { uint32_t tick,key,down; };
class TimedKeys {
 std::vector<KeyEvent> events;
 size_t next=0;
public:
 bool load(std::istream &input,unsigned last_key){
  std::vector<KeyEvent> parsed;
  std::string line;
  while(std::getline(input,line)){
   if(line.find_first_not_of(" \t\r")==std::string::npos)continue;
   std::istringstream row(line);uint64_t tick,key,down;std::string extra;
   if(!(row>>tick>>key>>down)||(row>>extra)||tick>UINT32_MAX||!key||key>=last_key||down>1)return false;
   if(!parsed.empty()&&tick<parsed.back().tick)return false;
   parsed.push_back({(uint32_t)tick,(uint32_t)key,(uint32_t)down});
  }
  if(input.bad())return false;
  events=std::move(parsed);next=0;return true;
 }
 template<class Emit> void advance(uint32_t tick,Emit emit){
  while(next<events.size()&&events[next].tick<=tick)emit(events[next++]);
 }
};
}
#endif
