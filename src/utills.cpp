#include<chrono>
using namespace std;
long getEpoch()
{
  using namespace std::chrono;
  microseconds ms = duration_cast<microseconds>(
      system_clock::now().time_since_epoch());

  return ms.count();
}