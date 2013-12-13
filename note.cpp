#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

#include <time.h>
#include <sys/time.h>
#include <stdlib.h>

void usage(const char* arg0) {
  std::cout << "@ writes a sentence with a timestamp to a dated log file verbatim. This log file can then be used to keep track of your thoughts during the day.\n";
  std::cout << "Usage:\n\t" << arg0 << " <message>\n";
}

void now(struct tm* out_now) {
  struct timeval tv;
  ::gettimeofday(&tv, nullptr);
  struct tm nowtm;
  ::localtime_r(&tv.tv_sec, out_now);
}

std::vector<std::string> todays_logfile_path() {
  std::vector<std::string> result;
  const char* homedir = ::getenv("HOME");
  if (homedir == nullptr) {
    std::cerr << "FATAL: Environment variable HOME not set.";
    exit(1);
  }

  result.push_back(homedir);
  result.push_back(".@");

  struct tm nowtm;
  now(&nowtm);

  char year_buffer[5];
  char month_buffer[3];
  char day_buffer[3];

  strftime(year_buffer, sizeof(year_buffer), "%Y", &nowtm);
  strftime(month_buffer, sizeof(month_buffer), "%m", &nowtm);
  strftime(day_buffer, sizeof(day_buffer), "%d", &nowtm);

  result.push_back(year_buffer);
  result.push_back(month_buffer);
  result.push_back(day_buffer);

  return std::move(result);
}

int main(int argc, char** argv) {
  if (argc == 1) {
    usage(argv[0]);
    return 1;
  }

  struct tm nowtm;
  now(&nowtm);
  char time_buffer[1024];
  strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", &nowtm);

  std::stringstream line_buffer;
  line_buffer << time_buffer << ": ";
  for (int i = 1; i < argc; ++i) {
    line_buffer << argv[i];
  }

  std::string line = line_buffer.str();

  std::vector<std::string> logfile_path = todays_logfile_path();
  std::stringstream path_buffer;
  std::stringstream fullpath_buffer;
  for (auto it = logfile_path.begin(); it != logfile_path.end(); ++it) {
    fullpath_buffer << *it;
    if (it != logfile_path.end()-1) {
      path_buffer << *it << '/';
      fullpath_buffer << '/';
    }
  }
  std::string dir = path_buffer.str();
  std::stringstream mkdir_p_buffer;
  mkdir_p_buffer << "mkdir -p " << dir;
  std::string mkdir_p_command = mkdir_p_buffer.str();
  ::system(mkdir_p_command.data());

  std::string file_path = fullpath_buffer.str();
  std::ofstream of(file_path, std::ios_base::app);
  of << line << "\n";
  return 0;
}
