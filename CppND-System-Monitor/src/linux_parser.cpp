#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <string>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// read data from the filesystem
string LinuxParser::OperatingSystem() {
  string l;
  string k;
  string v;
  std::ifstream Stream(kOSPath);
  if (Stream.is_open()) {
    while (std::getline(Stream, l)) {
      std::replace(l.begin(), l.end(), ' ', '_');
      std::replace(l.begin(), l.end(), '=', ' ');
      std::replace(l.begin(), l.end(), '"', ' ');
      std::istringstream linestream(l);
      while (linestream >> k >> v)
      {
        if (k == "PRETTY_NAME") {
          std::replace(v.begin(), v.end(), '_', ' ');
          return v;
        }
      }
    }
  }
  return v;
}

// read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel,line, version;
  std::ifstream Stream(kProcDirectory + kVersionFilename);
  if (Stream.is_open()) {
    std::getline(Stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  string l, k;
  float value,Total,Free;
  std::ifstream myStream(kProcDirectory + kMeminfoFilename);
  if (myStream.is_open())
  {
    while (std::getline(myStream, l))
    {
	  std::istringstream linestream(l);
      while (linestream >> k >> value)
      {
        if (k == "MemTotal:") {
          Total = value * 0.001;
        } 
        else if (k == "MemFree:") {
          Free = value * 0.001;
        }
      }
    }
  }
 
  return (Total - Free) / Total;
}

// Read and return the system uptime
long LinuxParser::UpTime() { 
 
  string line;
  
  long upt;
 
  std::ifstream Stream(kProcDirectory + kUptimeFilename);
  if (Stream.is_open())
  {
    std::getline(Stream, line);
    std::istringstream linestream(line);
    linestream >> upt;
  }
  return upt;
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() 

{
  return LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies();
}

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) { 
  
  string l, v;
  long ut, st, cutime, cstime;
  std::ifstream myStream(kProcDirectory + to_string(pid) + kStatFilename);
  if (myStream.is_open()) 
  {
    std::getline(myStream, l);
    std::istringstream linestream(l);
    for (int i = 0; i < 13; i++)
    {
      linestream >> v;
  	}
    linestream >> ut >> st >> cutime >> cstime;
  }
  return (ut + st + cutime + cstime) / sysconf(_SC_CLK_TCK);
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() 
{ 
  
  long user, nice, system, idle, iowait, irq, softirq, steal, active;
  string l, k;
  std::ifstream myStream(kProcDirectory + kStatFilename);
  if (myStream.is_open())
  {
    while (std::getline(myStream, l))
    {
      std::istringstream linestream(l);
      while (linestream >> k >> user >> nice >> system >> idle >> iowait >>
             irq >> softirq >> steal)
      {
        if (k == "cpu") 
        {
          active = user + nice + system + irq + softirq + steal;
        }
      }
    }
  }
  return active / sysconf(_SC_CLK_TCK);
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() 

{  
  string line, key;
  long user, nice, system, idle, iowait, irq, softirq, steal, idleTime;
  std::ifstream myStream(kProcDirectory + kStatFilename);
  if (myStream.is_open()) 
  {
    while (std::getline(myStream, line)) 
    {
      std::istringstream linestream(line);
      while (linestream >> key >> user >> nice >> system >> idle >> iowait >>
             irq >> softirq >> steal) 
      {
        if (key == "cpu") 
        {
          idleTime = idle + iowait;
        }
      }
    }
  }
  return idleTime / sysconf(_SC_CLK_TCK);
}

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization()
{ 
  
  std::ifstream myStream(LinuxParser::kProcDirectory +
                           LinuxParser::kStatFilename);
  
  vector<string> values{};
  string l, k, v;
  
  
  if (myStream.is_open())
  {
    while (std::getline(myStream, l)) 
    {
      std::istringstream linestream(l);
      while (linestream >> k)
      {
        if (k == "cpu") 
        {
          linestream >> v;
          values.push_back(v);
        }
      }
    }
  }
  return values;
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses()
{ 
  
  std::ifstream myStream(kProcDirectory + kStatFilename);
   int total;
  string l, k;
 
 
  if (myStream.is_open()) 
  {
    while (std::getline(myStream, l)) 
    {
      std::istringstream linestream(l);
      while (linestream >> k >> total) 
      {
        if (k == "processes") 
        {
          return total;
        }
      }
    }
  }
  return total;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses()
{ 
  
  int runnig;
  std::ifstream myStream(kProcDirectory + kStatFilename);
  string l, k;
  
  if (myStream.is_open())
  {
    while (std::getline(myStream, l)) 
    {
      std::istringstream lineStream(l);
      while (lineStream >> k >> runnig) 
      {
        
        if (k == "procs_running") 
          return runnig;
  
      }
    }
  }
  return runnig;
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) 
{ 
  
  std::ifstream myStream(kProcDirectory + to_string(pid) + kStatusFilename);
  string l,  k,  uid;
  
  if (myStream.is_open()) 
  {
    while (std::getline(myStream, l))
    {
      std::istringstream linestream(l);
      while (linestream >> k >> uid)
      {
        if (k == "Uid:") 
          return uid;
        
      }
    }
  }
  return uid;
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) 
{
  
  std::ifstream myStream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  string l;
 
  if (myStream.is_open()) 
  {
    std::getline(myStream, l);
  }
  return l;
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) 
{ 
  
  std::ifstream myStream(kPasswordPath);
  string l, user, pwd, uid;
 
  if (myStream.is_open()) 
  {
    while (std::getline(myStream, l)) 
    {
      std::replace(l.begin(), l.end(), ':', ' ');
      std::istringstream linestream(l);
      while (linestream >> user >> pwd >> uid)
      {
        if (uid == Uid(pid)) 
        {
          return user;
        }
      }
    }
  }
  return user;
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) 
{ 
   std::ifstream Stream(kProcDirectory + to_string(pid) + kStatusFilename);
  
   int ram;
  float val;
  string l, k;
  
 
  if (Stream.is_open()) 
  
  {
    while (std::getline(Stream, l))
    {
      std::istringstream linestream(l);
      while (linestream >> k >> val) 
      {
        if (k == "VmSize:") 
          ram = val * 0.001;
        
      }
    }
  }
  return to_string(ram);
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) { 
   std::ifstream stream(kProcDirectory+std::to_string(pid)+kStatFilename);
  string line,value;
  
    if (stream.is_open()){
    std::getline(stream,line);
    std::istringstream lstream(line);

    for (int j=0; j<=21;j++) {
      lstream>>value;
      if (j==21) {
        
        long uptime = stof(value) / sysconf(_SC_CLK_TCK);
        return uptime;

      }
    }
  }
return 0.0;
  }