#include "../src/handmade.h"
#include <chrono>
#include <limits>
#include <sstream>
#include <vector>
#include <thread>

int 
main()
{

  unsigned int num_threads = std::thread::hardware_concurrency();
  std::cout << "THREADS AVAILABLE: " << num_threads << '\n';
  ThreadPool pool(num_threads > 0 ? num_threads : 2);

  KeyValueStore db;
  char choice;
  std::string key;
  std::string value;
  db.load();



  while (true){
    std::cout << "\n------------------------------------\n";
    std::cout << "          DATABASE OPERATOR         \n";
    std::cout << "------------------------------------\n";
    std::cout << "\nSelect your operation:\n";
    std::cout << "(c)reate  (g)et  get-(s)ize (r)emove (u)pdate (m)ultiple-gets (l)ist-keys (q)uit\n";
    std::cin >> choice;
    
    if(choice == 'c' || choice == 'C') {
      std::cout << "type the group to add the info(single word)- \n";
      std::cin >> key;
      
      std::cout << "type the value to add to database (can be multiple words)-\n";
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      std::getline(std::cin, value);
      db.put(key, value);
    } else if (choice == 'g' || choice == 'G'){
      std::cout << "type the group to get the data in database-\n";
      std::cin >> key;
      Status getData = db.get(key);
      if(!getData.message.empty()){
        std::cout << getData.message << '\n';
      }
    } else if (choice == 'r' || choice == 'R') {
      std::cout << "type group to remove-\n";
      std::cin >> key;
      db.remove(key);
    } else if (choice == 's' || choice == 'S')  { 
      std::cout << db.size() << '\n';
    } else if (choice == 'u' || choice == 'U')  { 
      std::cout << "type the group to update in the info(single word)- \n";
      std::cin >> key;
      
      std::cout << "type the value to update in the database (can be multiple words)-\n";
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      std::getline(std::cin, value);
      db.update(key, value);
    } else if (choice == 'm' || choice == 'M') {
      std::cout << "type the keys to get, separated by spaces (e.g., key1 key2 key3):\n";
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      std::string line;
      std::getline(std::cin, line);
      std::istringstream iss(line);
      std::vector<std::string> keys;
      std::string current_key;
      while (iss >> current_key) {
        keys.push_back(current_key);
      }

      for (const auto& k : keys) {
        pool.enqueue([&db, k] {
          Status getData = db.get(k);
          if (getData.code == StatusCode::OK) {
            std::cout << "Result for " << k << ": " << getData.message << '\n';
          }
        });
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(250));
    } else if (choice == 'q' || choice == 'Q'){
      db.save();
      break;
    } else if (choice == 'l' || choice == 'L'){
      db.listKeys();
    }
  }

  return 0;
}
