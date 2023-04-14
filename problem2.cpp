#include <iostream>
#include <algorithm>
#include <thread>
#include <vector>
#include <mutex>
using namespace std;

// Problem 2: Atmospheric Temperature Reading Module

// 1 minute = 1 timestep
const int TIMESTEPS = 60;
const int NUM_SENSORS = 8;
const int MAXTEMP = 70;
const int MINTEMP = -100;

struct Reading
{
  int temp, time;

  Reading(int temp, int time): temp(temp), time(time) {}
};

struct Node
{
  Reading val;
  Node *next;
  bool marked = false;
  mutex m;

  Node(Reading val, Node *next): val(val), next(next) {}

  void lock()
  {
    m.lock();
  }

  void unlock()
  {
    m.unlock();
  }
};

// Lazy Synchronization Linked List
// Adds to the front, pops from the front (Stack)
// Ordering is not important for this purpose
struct LazyList
{
  Node *tail = new Node(Reading(-1, -1), nullptr);
  Node *head = new Node(Reading(-1, -1), tail);

  bool validate(Node *pred, Node *curr)
  {
    return !pred->marked && !curr->marked && pred->next == curr;
  }

  bool add(Reading item)
  {
    while (true)
    {
      Node *pred = head;
      Node *curr = head->next;
      
      pred->lock();
      try
      {
        curr->lock();
        try
        {
          if (validate(pred, curr))
          {
            Node *node = new Node(item, curr);
            pred->next = node;
            pred->unlock();
            curr->unlock();
            return true;
          }
        }
        catch (...)
        {

        }
        curr->unlock();
      }
      catch (...)
      {

      }
      pred->unlock();
    }
  }

  Reading pop_front()
  {
    while (true)
    {
      Node *pred = head;
      Node *curr = head->next;

      pred->lock();
      try
      {
        curr->lock();
        try
        {
          if (validate(pred, curr))
          {
            if (curr->val.time != -1)
            {
              curr->marked = true;
              pred->next = curr->next;
              pred->unlock();
              curr->unlock();
              return curr->val;
            }
            else
            {
              pred->unlock();
              curr->unlock();
              return Reading(-1, -1);
            }
          }
        }
        catch (...)
        {

        }
        curr->unlock();
      }
      catch (...)
      {

      }
      pred->unlock();
    }
  }
};

// Simulate the Atmospheric Temperature Reading Module over one hour
void simulate()
{
  srand(time(0));
  LazyList readings;
  vector<thread> sensors(NUM_SENSORS);

  for (int i = 0; i < NUM_SENSORS; i++)
    sensors[i] = thread([&](int id)
    {
      for (int i = 0; i < TIMESTEPS; i++)
        readings.add(Reading(rand() % (MAXTEMP - MINTEMP + 1) + MINTEMP, i));
    }, i);

  for (thread &t : sensors)
    t.join();

  vector<Reading> data;
  while (true)
  {
    data.push_back(readings.pop_front());
    if (data.back().time == -1)
    {
      data.pop_back();
      break;
    }
  }
  
  // Decreasing order of temperature
  sort(data.begin(), data.end(), [](Reading &a, Reading &b)
  {
    return a.temp > b.temp;
  });

  cout << "5 highest temperatures recorded:\n";
  for (int i = 0; i < 5; i++)
    cout << data[i].temp << " degrees F\n";

  cout << endl;

  cout << "5 lowest temperatures recorded:\n";
  for (int i = 0; i < 5; i++)
    cout << data[int(data.size()) - i - 1].temp << " degrees F\n";

  // Increasing order of time
  sort(data.begin(), data.end(), [](Reading &a, Reading &b)
  {
    return a.time < b.time;
  });

  cout << endl;

  int bestdiff = 0, besttime = 0;

  for (int i = 0; i + 9 < (int)data.size(); i++)
  {
    int maxtemp = data[i].temp;
    int mintemp = data[i].temp;
    for (int j = i; j <= i + 9; j++)
    {
      maxtemp = max(maxtemp, data[j].temp);
      mintemp = min(mintemp, data[j].temp);
    }

    if (maxtemp - mintemp > bestdiff)
    {
      bestdiff = maxtemp - mintemp;
      besttime = data[i].time;
    }
  }

  cout << "The largest temperature difference occurs in the interval [" << besttime + 1 << ", " << besttime + 11 << "] minutes." << endl;
}

int main(void)
{
  simulate();

  return 0;
}