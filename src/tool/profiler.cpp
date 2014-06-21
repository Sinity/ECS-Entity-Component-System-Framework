#include "profiler.h"
#include <fstream>
#include <cstring>
#include <limits>
#include <vector>
#include <SFML/System.hpp>
#include "logger.h"
#include "common/formatString.h"

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


	Profile& operator=(Profile& profile) = delete;
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
	std::string result = format(indent, "Name: ", name, "\n",
		indent, "Samples: ", samples.size(), "\n",
		indent, "Participation in parent time: ", parentParticipation, "%\n",
		indent, "Average: ", avg.asSeconds(), "s (", avg.asMicroseconds(), "us)\n",
		indent, "Min: ", min.asSeconds(), "s (", min.asMicroseconds(), "us) - ", minPartOfAverage, "% of average\n",
		indent, "Max: ", max.asSeconds(), "s (", max.asMicroseconds(), "us) - ", maxPartOfAverage, "% of average\n",
		indent, "Total: ", total.asSeconds(), "s (", total.asMicroseconds(), "us)\n\n");

    //repeat in childrens
    depth++;
    for (unsigned int i = 0; i < childs.size(); i++)
        result += childs[i]->calculateResults(depth);

    return result;
}

std::string Profile::dumpSamples() {
    std::string result;
    for (unsigned int i = 0; i < samples.size(); i++)
        result += format(i + 1, ". ", samples[i].asSeconds(), "s (", samples[i].asMicroseconds(), "us)\n");
    return result;
}

Profiler::Profiler(const std::string& filename) :
	main(new Profile("main", nullptr)),
	current(main),
	filename(filename),
	logger("Profiler") {
}

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
	result += format("Profiler life: ", maintime.asSeconds(), "s (", (long long)maintime.asMicroseconds(), "us)\n");

    //calculate childrens stats
	for (Profile* child : main->childs) {
		result += child->calculateResults(1);
	}


    //save to file
    std::ofstream resFile(filename);
    if(!resFile.is_open()) {
        logger.error("Profiler: Could not dump results; cannot open file ", filename, ". Will dump right here:\n\n\n", result, "\n\n\n");
        return;
    }
    resFile << result;
}


void Profiler::saveSamples(Profile* profile, bool childsToo) {
    //generate filename
    std::string filename = format(profile->name, "_profile_dmp", profile->name);
    for (Profile* ancestor = profile->parent; ancestor != nullptr; ancestor = ancestor->parent)
        filename += format(".", ancestor->name.c_str());
    filename += ".txt";

    //save samples to file
    std::ofstream file(filename);
    if (file.is_open()) {
        file << profile->dumpSamples();
	} else {
		logger.error("Profiler: Could not dump samples; cannot open file ", filename, ". Will dump right here:\n\n\n", profile->dumpSamples(), "\n\n\n");
		return;
	}

    //repeat in childs if required
	if (childsToo) {
		for (auto child : profile->childs) {
			saveSamples(child, true);
		}
	}
}


Profiler::~Profiler() {
    saveResults();
    saveSamples(main, true);
    delete main;
}

