# Dorognoe
web api for dorognoe.ru radio station site
# main
```cpp
#include "Dorognoe.h"
#include <iostream>

int main() {
   Dorognoe api;

    auto cities = api.get_cities_list().then([](json::value result) {
        std::cout << result<< std::endl;
    });
    cities.wait();
    
    return 0;
}
```

# Launch (your script)
```
g++ -std=c++11 -o main main.cpp -lcpprest -lssl -lcrypto -lpthread -lboost_system -lboost_chrono -lboost_thread
./main
```

