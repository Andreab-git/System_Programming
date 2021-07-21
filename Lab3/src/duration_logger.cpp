//
// Created by andrea on 24/03/21.
//

#include "../lib/duration_logger.h"

#include <utility>

DurationLogger::DurationLogger(std::string name) : name(std::move(name)),
                                                   start(std::chrono::high_resolution_clock::now()) {
  std::cout << " >>>>>>>>>>>>>> starting [" << this->name << "]" << std::endl;
}

DurationLogger::~DurationLogger() {
  stop = std::chrono::high_resolution_clock::now();
  auto duration = duration_cast<std::chrono::microseconds>(stop - start);
  std::cout << " <<<<<<<<<<<<<<< ending [" << name << "]: [" << duration.count() << "] millisec " << std::endl;
}
