#include <fstream>
#include <sstream>
#include "PersistenceManager.h"

#ifdef _WIN32
#include <direct.h>
#define MKDIR(d) _mkdir(d)
#else
#include <sys/stat.h>
#define MKDIR(d) mkdir(d, 0777)
#endif

using namespace std;

PersistenceManager::PersistenceManager() {
    MKDIR("data");
}


void PersistenceManager::appendCommand(const string& command)
{
    ofstream file("data/appendonly.aof", ios::app);

    if(!file)
    {
        cout << "Could not open AOF file\n";
        return;
    }

    file << command << '\n';
}
vector<string> PersistenceManager::loadCommands()
{
    vector<string> commands;

    ifstream file("data/appendonly.aof");

    if(!file)
        return commands;

    string line;

    while(getline(file, line))
    {
        if(!line.empty())
            commands.push_back(line);
    }

    return commands;
}

