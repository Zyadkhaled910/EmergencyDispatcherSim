#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <iomanip>

class Emergency {
protected:
    std::string id;
    std::string location;
    std::string description;
    int priority; // 1-5, where 1 is highest priority
    time_t timestamp;
    std::string status; // "Pending", "Dispatched", "Resolved"

public:
    Emergency(const std::string& loc, const std::string& desc, int pri)
        : location(loc), description(desc), priority(pri), status("Pending") {
        timestamp = time(nullptr);
        id = "E" + std::to_string(timestamp % 10000);
    }

    virtual ~Emergency() = default;

    virtual void display() const {
        std::cout << "Emergency ID: " << id << "\n";
        std::cout << "Location: " << location << "\n";
        std::cout << "Description: " << description << "\n";
        std::cout << "Priority: " << priority << "\n";
        std::cout << "Status: " << status << "\n";
    }

    virtual std::string getType() const { return "General"; }
    void updateStatus(const std::string& newStatus) { status = newStatus; }
    std::string getId() const { return id; }

    virtual std::string toFileString() const {
        return getType() + "," + id + "," + location + "," + description + "," +
               std::to_string(priority) + "," + status;
    }
};
class MedicalEmergency : public Emergency {
private:
    std::string patientName;
    std::string condition;
    bool isUrgent;

public:
    MedicalEmergency(const std::string& loc, const std::string& desc, int pri,
                    const std::string& patient, const std::string& cond, bool urgent)
        : Emergency(loc, desc, pri), patientName(patient),
          condition(cond), isUrgent(urgent) {}

    void display() const override {
        Emergency::display();
        std::cout << "Type: Medical Emergency\n";
        std::cout << "Patient: " << patientName << "\n";
        std::cout << "Condition: " << condition << "\n";
        std::cout << "Urgent: " << (isUrgent ? "Yes" : "No") << "\n";
        std::cout << "------------------------\n";
    }

    std::string getType() const override { return "Medical"; }

    std::string toFileString() const override {
        return Emergency::toFileString() + "," + patientName + "," +
               condition + "," + (isUrgent ? "1" : "0");
    }
};

class FireEmergency : public Emergency {
private:
    std::string buildingType;
    int severity; // 1-5
    bool hazardousMaterials;

public:
    FireEmergency(const std::string& loc, const std::string& desc, int pri,
                 const std::string& building, int sev, bool hazmat)
        : Emergency(loc, desc, pri), buildingType(building),
          severity(sev), hazardousMaterials(hazmat) {}

    void display() const override {
        Emergency::display();
        std::cout << "Type: Fire Emergency\n";
        std::cout << "Building Type: " << buildingType << "\n";
        std::cout << "Severity (1-5): " << severity << "\n";
        std::cout << "Hazardous Materials: " << (hazardousMaterials ? "Yes" : "No") << "\n";
        std::cout << "------------------------\n";
    }

    std::string getType() const override { return "Fire"; }

    std::string toFileString() const override {
        return Emergency::toFileString() + "," + buildingType + "," +
               std::to_string(severity) + "," + (hazardousMaterials ? "1" : "0");
    }
};

class PoliceEmergency : public Emergency {
private:
    std::string crimeType;
    bool suspectArmed;
    int officersNeeded;

public:
    PoliceEmergency(const std::string& loc, const std::string& desc, int pri,
                  const std::string& crime, bool armed, int officers)
        : Emergency(loc, desc, pri), crimeType(crime),
          suspectArmed(armed), officersNeeded(officers) {}

    void display() const override {
        Emergency::display();
        std::cout << "Type: Police Emergency\n";
        std::cout << "Crime Type: " << crimeType << "\n";
        std::cout << "Suspect Armed: " << (suspectArmed ? "Yes" : "No") << "\n";
        std::cout << "Officers Needed: " << officersNeeded << "\n";
        std::cout << "------------------------\n";
    }

    std::string getType() const override { return "Police"; }

    std::string toFileString() const override {
        return Emergency::toFileString() + "," + crimeType + "," +
               (suspectArmed ? "1" : "0") + "," + std::to_string(officersNeeded);
    }
};
class EmergencyDispatcher {
private:
    static const int MAX_EMERGENCIES = 100;
    Emergency* emergencies[MAX_EMERGENCIES];
    int count;
    std::string filename;

public:
    EmergencyDispatcher(const std::string& file) : count(0), filename(file) {
        for (int i = 0; i < MAX_EMERGENCIES; i++) {
            emergencies[i] = nullptr;
        }
        loadFromFile();
    }

    ~EmergencyDispatcher() {
        for (int i = 0; i < count; i++) {
            delete emergencies[i];
        }
    }

    bool addEmergency(Emergency* emergency) {
        if (count >= MAX_EMERGENCIES) return false;
        emergencies[count++] = emergency;
        saveToFile();
        return true;
    }

    void displayAllEmergencies() const {
        if (count == 0) {
            std::cout << "No emergencies recorded.\n";
            return;
        }

        for (int i = 0; i < count; i++) {
            emergencies[i]->display();
        }
    }

    bool updateEmergencyStatus(const std::string& id, const std::string& status) {
        for (int i = 0; i < count; i++) {
            if (emergencies[i]->getId() == id) {
                emergencies[i]->updateStatus(status);
                saveToFile();
                return true;
            }
        }
        return false;
    }

    void saveToFile() const {
        std::ofstream file(filename);
        if (!file) {
            std::cerr << "Error: Could not open file for writing.\n";
            return;
        }

        for (int i = 0; i < count; i++) {
            file << emergencies[i]->toFileString() << "\n";
        }
        file.close();
    }

    void loadFromFile() {
        std::ifstream file(filename);
        if (!file) return;

        std::string line;
        std::string type, id, location, description, status;
        int priority;

        while (std::getline(file, line) && count < MAX_EMERGENCIES) {
            size_t pos = 0;
            std::string token;

            // Parse common fields
            pos = line.find(',');
            type = line.substr(0, pos);
            line.erase(0, pos + 1);

            pos = line.find(',');
            id = line.substr(0, pos);
            line.erase(0, pos + 1);

            pos = line.find(',');
            location = line.substr(0, pos);
            line.erase(0, pos + 1);

            pos = line.find(',');
            description = line.substr(0, pos);
            line.erase(0, pos + 1);

            pos = line.find(',');
            priority = std::stoi(line.substr(0, pos));
            line.erase(0, pos + 1);

            pos = line.find(',');
            status = line.substr(0, pos);
            line.erase(0, pos + 1);

            // Create appropriate emergency type
            if (type == "Medical") {
                std::string patient, condition;
                bool urgent;

                pos = line.find(',');
                patient = line.substr(0, pos);
                line.erase(0, pos + 1);

                pos = line.find(',');
                condition = line.substr(0, pos);
                line.erase(0, pos + 1);

                urgent = line == "1";

                auto* emergency = new MedicalEmergency(location, description, priority,
                                                     patient, condition, urgent);
                emergency->updateStatus(status);
                emergencies[count++] = emergency;
            }
            else if (type == "Fire") {
                std::string building;
                int severity;
                bool hazmat;

                pos = line.find(',');
                building = line.substr(0, pos);
                line.erase(0, pos + 1);

                pos = line.find(',');
                severity = std::stoi(line.substr(0, pos));
                line.erase(0, pos + 1);

                hazmat = line == "1";

                auto* emergency = new FireEmergency(location, description, priority,
                                                  building, severity, hazmat);
                emergency->updateStatus(status);
                emergencies[count++] = emergency;
            }
            else if (type == "Police") {
                std::string crime;
                bool armed;
                int officers;

                pos = line.find(',');
                crime = line.substr(0, pos);
                line.erase(0, pos + 1);

                pos = line.find(',');
                armed = line.substr(0, pos) == "1";
                line.erase(0, pos + 1);

                officers = std::stoi(line);

                auto* emergency = new PoliceEmergency(location, description, priority,
                                                    crime, armed, officers);
                emergency->updateStatus(status);
                emergencies[count++] = emergency;
            }
        }

        file.close();
    }
};
int main() {
    EmergencyDispatcher dispatcher("emergencies.txt");
    int choice;
    bool running = true;

    while (running) {
        // Display menu options
        std::cout << "\n===== Emergency Dispatcher Simulator =====\n";
        std::cout << "1. Add New Emergency\n";
        std::cout << "2. Update Emergency Status\n";
        std::cout << "3. Display All Emergencies\n";
        std::cout << "4. Exit\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
            case 1: {
                // Add new emergency
                int emergencyType;
                std::cout << "\nEmergency Type:\n";
                std::cout << "1. Medical Emergency\n";
                std::cout << "2. Fire Emergency\n";
                std::cout << "3. Police Emergency\n";
                std::cout << "Enter type: ";
                std::cin >> emergencyType;

                std::cin.ignore(); // Clear input buffer

                std::string location, description;
                int priority;

                std::cout << "Enter location: ";
                std::getline(std::cin, location);

                std::cout << "Enter description: ";
                std::getline(std::cin, description);

                std::cout << "Enter priority (1-5, where 1 is highest): ";
                std::cin >> priority;

                Emergency* newEmergency = nullptr;

                switch (emergencyType) {
                    case 1: {
                        // Medical emergency specific data
                        std::string patientName, condition;
                        bool isUrgent;

                        std::cin.ignore();
                        std::cout << "Enter patient name: ";
                        std::getline(std::cin, patientName);

                        std::cout << "Enter condition: ";
                        std::getline(std::cin, condition);

                        char urgent;
                        std::cout << "Is it urgent? (Y/N): ";
                        std::cin >> urgent;
                        isUrgent = (urgent == 'Y' || urgent == 'y');

                        newEmergency = new MedicalEmergency(location, description, priority,
                                                           patientName, condition, isUrgent);
                        break;
                    }
                    case 2: {
                        // Fire emergency specific data
                        std::string buildingType;
                        int severity;
                        bool hazmat;

                        std::cin.ignore();
                        std::cout << "Enter building type: ";
                        std::getline(std::cin, buildingType);

                        std::cout << "Enter severity (1-5): ";
                        std::cin >> severity;

                        char hazardous;
                        std::cout << "Hazardous materials present? (Y/N): ";
                        std::cin >> hazardous;
                        hazmat = (hazardous == 'Y' || hazardous == 'y');

                        newEmergency = new FireEmergency(location, description, priority,
                                                        buildingType, severity, hazmat);
                        break;
                    }
                    case 3: {
                        // Police emergency specific data
                        std::string crimeType;
                        bool armed;
                        int officers;

                        std::cin.ignore();
                        std::cout << "Enter crime type: ";
                        std::getline(std::cin, crimeType);

                        char suspect;
                        std::cout << "Is suspect armed? (Y/N): ";
                        std::cin >> suspect;
                        armed = (suspect == 'Y' || suspect == 'y');

                        std::cout << "Number of officers needed: ";
                        std::cin >> officers;

                        newEmergency = new PoliceEmergency(location, description, priority,
                                                         crimeType, armed, officers);
                        break;
                    }
                    default:
                        std::cout << "Invalid emergency type!\n";
                        break;
                }

                if (newEmergency && dispatcher.addEmergency(newEmergency)) {
                    std::cout << "Emergency added successfully!\n";
                } else {
                    std::cout << "Failed to add emergency.\n";
                    delete newEmergency; // Clean up if not added
                }
                break;
            }
            case 2: {
                // Update emergency status
                std::string id, newStatus;

                std::cin.ignore();
                std::cout << "Enter emergency ID: ";
                std::getline(std::cin, id);

                std::cout << "Enter new status (Pending/Dispatched/Resolved): ";
                std::getline(std::cin, newStatus);

                if (dispatcher.updateEmergencyStatus(id, newStatus)) {
                    std::cout << "Status updated successfully!\n";
                } else {
                    std::cout << "Failed to update status. Emergency not found.\n";
                }
                break;
            }
            case 3:
                // Display all emergencies
                std::cout << "\n===== Current Emergencies =====\n";
                dispatcher.displayAllEmergencies();
                break;
            case 4:
                // Exit
                running = false;
                std::cout << "Exiting Emergency Dispatcher Simulator. Goodbye!\n";
                break;
            default:
                std::cout << "Invalid option. Please try again.\n";
                break;
        }
    }

    return 0;
}