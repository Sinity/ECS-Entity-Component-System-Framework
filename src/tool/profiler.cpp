#include "profiler.h"
#include <fstream>
#include <cstring>
#include <limits>
#include <SFML/System.hpp>
#include "logger.h"

class Profile {
public:
    Profile(const std::string& name, Profile* parent = nullptr) : name(name), parent(parent) {}
    ~Profile();

    std::string calculateResults(unsigned int depth);
    std::string dumpSamples();

    std::string name;
    sf::Clock clock;
    std::vector<sf::Time> samples;
    std::vector<sf::Time> meanTimes;
    std::vector<Profile*> childs;

    Profile* const parent;
};

Profile::~Profile() {
    for (unsigned int i = 0; i < childs.size(); i++)
        delete childs[i];
}

std::string Profile::calculateResults(unsigned int depth) { //TODO: replace this function with external tool that will calculate it
    //calculate indents
    std::string indent;
    for (unsigned int i = 0; i < depth; i++)
        indent += '\t';

    //calculate min, max, average and total
    sf::Time avg, min, max, total;
    avg = sf::milliseconds(0);
    min = sf::milliseconds(std::numeric_limits<int>::max());
    max = sf::milliseconds(std::numeric_limits<int>::min());
    total = sf::milliseconds(0);
    for (unsigned int i = 0; i < samples.size(); i++) {
        total += samples[i];
        if (samples[i] < min)
            min = samples[i];
        if (samples[i] > max)
            max = samples[i];
    }
    if (samples.size() != 0)
        avg = total / (sf::Int64) samples.size();

    //calculate participation in parent time
    sf::Time parentTotal;
    for (sf::Time sample : parent->samples)
        parentTotal += sample;
    double parentParticipation = (1.0 / (parentTotal.asSeconds() / total.asSeconds())) * 100;

    //calculate min/max % of average
    double minPartOfAverage = (1.0 / (avg.asSeconds() / min.asSeconds())) * 100;
    double maxPartOfAverage = (1.0 / (avg.asSeconds() / max.asSeconds())) * 100;
    
    //format stats
    std::string result = fmt("%sName: %s\n"
            "%sSamples: %d\n"
            "%sParticipation in parent time: %f%%\n"
            "%sAverage: %fs (%lldus)\n"
            "%sMin: %fs (%lldus) - %f%% of average\n"
            "%sMax: %fs (%lldus) - %f%% of average\n"
            "%sTotal: %fs (%lldus)\n\n",
            indent.c_str(), name.c_str(),
            indent.c_str(), samples.size(),
            indent.c_str(), parentParticipation,
            indent.c_str(), avg.asSeconds(), avg.asMicroseconds(),
            indent.c_str(), min.asSeconds(), min.asMicroseconds(), minPartOfAverage,
            indent.c_str(), max.asSeconds(), max.asMicroseconds(), maxPartOfAverage,
            indent.c_str(), total.asSeconds(), total.asMicroseconds());


    //repeat in childrens
    depth++;
    for (unsigned int i = 0; i < childs.size(); i++)
        result += childs[i]->calculateResults(depth);

    return result;
}

std::string Profile::dumpSamples() {
    std::string result;
    for (unsigned int i = 0; i < samples.size(); i++)
        result += fmt("%d. %fs (%lldus)\n", i + 1, samples[i].asSeconds(), samples[i].asMicroseconds());
    return result;
}

Profiler::Profiler(const std::string& filename, Logger& logger)
        : main(new Profile("main", nullptr)), current(main), filename(filename), logger(logger) {}

void Profiler::start(const char* name) {
    //search for existing profile
    for (unsigned int i = 0; i < current->childs.size(); i++)
        if (current->childs[i]->name == name) {
            current = current->childs[i];
            current->meanTimes.emplace_back(current->clock.restart());
            return;
        }

    //not found; it's new
    current->childs.push_back(new Profile(name, current));
    current = current->childs.back();
}


void Profiler::stop() {
    sf::Time currTime = current->clock.restart();
    current->samples.push_back(currTime);
    current = current->parent;
}


void Profiler::saveResults() {
    std::string result;

    //calculate main time
    sf::Time maintime = main->clock.restart();
    main->samples.push_back(maintime);
    result += fmt("Profiler life: %fs (%lldus)\n",
            maintime.asSeconds(),
            maintime.asMicroseconds());

    //calculate childrens stats
    for(Profile* child : main->childs)
        result += child->calculateResults(1);


    //save to file
    std::ofstream resFile(filename);
    if(!resFile.is_open()) {
        logger.error("Profiler: Could not dump results; cannot open file %s. Will dump right here:\n\n\n%s\n\n\n", filename.c_str(), result.c_str());
        return;
    }
    resFile << result;
}


void Profiler::saveSamples(Profile* profile, bool childsToo) {
    //generate filename
    std::string filename = fmt("%s_profile_dmp", profile->name.c_str());
    for (Profile* ancestor = profile->parent; ancestor != nullptr; ancestor = ancestor->parent)
        filename += fmt(".%s", ancestor->name.c_str());
    filename += ".txt";

    //save samples to file
    std::ofstream file(filename);
    if (file.is_open()) {
        file << profile->dumpSamples();
    }

    //repeat in childs if required
    if (childsToo)
        for (auto child : profile->childs)
            saveSamples(child, true);
}


Profiler::~Profiler() {
    saveResults();
    saveSamples(main, true);
    delete main;
}

