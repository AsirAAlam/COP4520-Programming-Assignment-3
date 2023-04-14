#include <algorithm>
#include <numeric>
#include <iostream>
#include <list>
#include <mutex>
#include <thread>
#include <vector>
#include <atomic>
#include <random>
#include <chrono>
using namespace std;

// Problem 1: The Birthday Presents Party

const int GIFTS = 500000;
const int NUMSERVANTS = 4;
atomic<int> bag_idx(0), cards_written(0);

struct Node
{
  int val;
  Node *next;
  bool marked = false;
  mutex m;

  Node(int val, Node *next): val(val), next(next) {}

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
// Adds in sorted order, pops first item
struct LazyList
{
  Node *tail = new Node(1e9, nullptr);
  Node *head = new Node(0, tail);

  bool validate(Node *pred, Node *curr)
  {
    return !pred->marked && !curr->marked && pred->next == curr;
  }

  bool add(int item)
  {
    while (true)
    {
      Node *pred = head;
      Node *curr = head->next;

      while (curr->val < item)
      {
        pred = curr;
        curr = curr->next;
      }
      
      pred->lock();
      try
      {
        curr->lock();
        try
        {
          if (validate(pred, curr))
          {
            if (curr->val == item)
            {
              pred->unlock();
              curr->unlock();
              return false;
            }
            else
            {
              Node *node = new Node(item, curr);
              pred->next = node;
              pred->unlock();
              curr->unlock();
              return true;
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

  int pop_front()
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
            if (curr->val < 1e9)
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
              return false;
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

  bool contains(int item)
  {
    Node *curr = head;
    while (curr->val < item)
      curr = curr->next;

    return curr->val == item && !curr->marked;
  }
};

void simulate()
{
  LazyList chain;
  vector<thread> servants(NUMSERVANTS);
  vector<int> order(GIFTS);

  // Generate random order of grabbing gifts from the large bag
  iota(order.begin(), order.end(), 1);
  unsigned seed = chrono::system_clock::now().time_since_epoch().count();
  shuffle(order.begin(), order.end(), std::default_random_engine(seed));

  for (int i = 0; i < NUMSERVANTS; i++)
    servants[i] = thread([&](int id)
    {
      // 0 = add to chain, 1 = remove from chain
      bool flag = 0;
      while (cards_written < GIFTS)
      {
        if (flag == 0)
        {
          int idx = bag_idx++;
          if (idx < GIFTS)
          {
            chain.add(order[idx]);
          }
          else
          {
            // If no more gifts to add, write cards instead
            flag = 1;
          }
        }

        if (flag == 1)
        {
          int gift = chain.pop_front();

          if (gift >= 1)
          {
            cards_written++;
          }
          else
          {
            break;
          }

        }

        // Alternate adding to chain and writing cards.
        flag ^= 1;
      }
    }, i);

  for (thread &servant : servants)
    servant.join();

  cout << cards_written << " \"Thank you\" cards have been written." << endl;
}

int main(void)
{
  simulate();

  return 0;
}