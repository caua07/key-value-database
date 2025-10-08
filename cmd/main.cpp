#include "../src/handmade.h"
#include <limits>

int 
main()
{
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
    std::cout << "(c)reate  (g)et  get-(s)ize (r)emove (u)pdate (q)uit\n";
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
      auto getData = db.get(key);
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
    }else if (choice == 'q' || choice == 'Q'){
      db.save();
      break;
    }
  }

  return 0;
}
