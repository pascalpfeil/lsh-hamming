#include <iostream>

#include "lsh/bitvector.hpp"
#include "lsh/table.hpp"


int main() {
  lsh::Table table(4, 2, 3);
  table.insert(lsh::vector_t(std::string("1011")));
  table.insert(lsh::vector_t(std::string("0100")));
  table.insert(lsh::vector_t(std::string("0110")));

  const auto result = table.query(lsh::vector_t(std::string("1001")));

  std::string buffer;
  boost::to_string(result, buffer);
  std::cout << "result: " << buffer << std::endl;

  return 0;
}
