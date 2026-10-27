#include <bits/stdc++.h>
using namespace std;

struct position{
    double x = 0;
    double y = 0;
    double z = 0;
};

static vector<position> obstacles;

class Vehicle{
    protected:
    string id;
    position pos;
    double battery;

    public:
    Vehicle(const string id, double battery):id(id), pos(), battery(battery){}

    virtual void move (double dx, double dy, double dz) = 0;
    virtual void status() const{
        cout << fixed << setprecision(2);
        cout << id <<  "Position: (" << pos.x << ", " << pos.y << ", " << pos.z << ") ";
        cout << "Battery: " << battery << endl; 
    };
    virtual unique_ptr<Vehicle> clone() const = 0;
    virtual ~Vehicle() = default;

    bool consumeBattery(double amount){
        if(battery >= amount){
            battery -= amount;
            return true;
        }
        else {
            return false;
        }
    }
    const position getPosition() const {return pos;}
    const string getId() const {return id;}
};

class GroundRobot : public virtual Vehicle{ 
    public: 
    GroundRobot(string id, double battery) : Vehicle(id, battery){
        pos.z = 0.0;
    }

    void move (double dx, double dy, double dz) override {
       double dist_xy = hypot(dx,dy);
       double cost = dist_xy * 0.5;
       double actualDist = dist_xy;
       double factor = 1.0;
       bool obstacle = obstacleAhead(dist_xy, dx, dy);

       if (obstacle) {
            actualDist = dist_xy * 0.5;
            factor = 0.5;
            cout << id << " - obstacle detected, slowing down" << endl;
       }
       if (!consumeBattery(cost)){
            cout << "id: " << id << "Low battery, can't move" << endl;
            return;
       }
       if (dist_xy > 1e-9){
            pos.x += dx * factor;
            pos.y += dy * factor;
       }
       pos.z = 0.0;
    }

    void move (double heading_deg, double distance) {
        double heading_rad = heading_deg * M_PI / 180.0;
        double dx = cos(heading_rad) * distance;
        double dy = sin(heading_rad) * distance;
        move (dx, dy, 0.0);
    }

    
    virtual bool obstacleAhead (double distance, double dx = 0.0, double dy = 0.0) {
        position nextPos = pos;
        double step = 0.1;
        double dx_step = (dx / distance) * step;
        double dy_step = (dy / distance) * step;
        if (distance < 1e-9) return false;

        for (double d = step; d <= distance; d += step) {
            nextPos.x += dx_step;
            nextPos.y += dy_step;
            for (const auto& obs : obstacles) {
                double dist_to_Obs = hypot(nextPos.x - obs.x, nextPos.y - obs.y);
                if (dist_to_Obs < 1.0) {
                    return true;
                }
            }
        }
        return false;
    }

    unique_ptr<Vehicle> clone() const override {
        return make_unique<GroundRobot>(*this);
    }

    void status() const override{
        cout << "Ground Robot - ";
        Vehicle::status();
    }
};

class AerialDrone : public virtual Vehicle{
    public:
    AerialDrone(const string id, double battery) : Vehicle(id, battery){}

    void move(double dx, double dy, double dz) override{
        double dist = sqrt(dx*dx +dy*dy + dz*dz);
        double cost = dist * 1.0;
        if (!consumeBattery(cost)){
            cout << "id: " << id << "Low battery, can't move" << endl;
            return;
       }
       pos.x += dx; 
       pos.y += dy; 
       pos.z += dz;
    }

    void move(const position& target){
        double dx = target.x - pos.x;
        double dy = target.y - pos.y;
        double dz = target.z - pos.z;
        move (dx, dy, dz);
    }

    unique_ptr<Vehicle> clone() const override{
        return make_unique<AerialDrone>(*this);
    }

    void status() const override{
        cout << "Aerial Drone - ";
        Vehicle::status();
    } 
};

class Amphibious : public GroundRobot, public AerialDrone{
    public:
    enum class Mode {GROUND, AIR};

    protected:
    Mode mode; // GROUND atau AIR

    public:
    Amphibious(const string id, double battery, Mode startMode = Mode::GROUND)
        : Vehicle(id, battery), GroundRobot(id, battery), AerialDrone(id, battery), mode(startMode) {
        if (mode == Mode::GROUND) {
            pos.z = 0.0;
        }
    }
    void setMode(Mode newMode){
        mode = newMode;
        if (mode == Mode::GROUND) {
            pos.z = 0.0;
        }
    }

    Mode getMode() const {
        return mode;
    }

    void move(double dx, double dy, double dz) override{
        if (mode == Mode::GROUND) {
            GroundRobot::move(dx, dy, 0);
        } else {
            AerialDrone::move(dx, dy, dz);
        }
    }

    void move(double heading_deg, double distance) {
        if (mode == Mode::AIR) {
            double rad = heading_deg * M_PI / 180.0;
            double dx = cos(rad) * distance;
            double dy = sin(rad) * distance;
            double dz = (mode == Mode::AIR) ? distance * 0.1 : 0.0;
            move(dx, dy, dz);
        } else {
            double rad = heading_deg * M_PI / 180.0;
            double dx = cos(rad) * distance;
            double dy = sin(rad) * distance;
            move(dx, dy, 0.0);
        }
    }

    unique_ptr<Vehicle> clone() const override{
        return make_unique<Amphibious>(*this);
    }

    void status() const override{
        cout << "Amphibious - ";
        Vehicle::status();
    }
};

void executeMove(Vehicle& v, double dx, double dy, double dz){
    v.move (dx, dy, dz);
    v.status();
}

void executeMove(Vehicle& v, double heading_deg, double distance) {
    if (auto grnd = dynamic_cast<GroundRobot*>(&v)) {
        grnd->move(heading_deg, distance);
        return;
    }
    if (auto am = dynamic_cast<Amphibious*>(&v)) {
        am->move(heading_deg, distance);
        return;
    }

    double rad = heading_deg * M_PI / 180.0;
    double dx = cos(rad) * distance;
    double dy = sin(rad) * distance;
    v.move(dx, dy, 0.0);
}


int main() {
    vector<unique_ptr<Vehicle>> fleet;

    cout << fixed << setprecision(2);
    cout << "=== STUDI KASUS MINIMAL ===\n\n";

    // 1. GroundRobot: move tanpa obstacle
    cout << "[1] GroundRobot: Move tanpa obstacle\n";
    obstacles.clear(); // pastikan area kosong
    GroundRobot gr1("GR-Test1", 10.0);
    gr1.status();
    gr1.move(1.0, 0.0, 0.0); // bergerak 1 meter di sumbu X
    gr1.status();
    cout << "----------------------------------------\n";

    // 2️. GroundRobot: move menuju obstacle
    cout << "[2] GroundRobot: Move menuju obstacle\n";
    obstacles.clear();
    obstacles.push_back({2.0, 2.0, 0.0}); // obstacle di depan
    GroundRobot gr2("GR-Test2", 10.0);
    gr2.status();
    gr2.move(3.0, 2.0, 0.0); // harus terdeteksi obstacle
    gr2.status();
    cout << "----------------------------------------\n";

    // 3️. AerialDrone: move(targetPosition)
    cout << "[3] AerialDrone: Move to targetPosition\n";
    obstacles.clear();
    AerialDrone ad1("AD-Test1", 15.0);
    ad1.status();
    position target = {3.0, 4.0, 2.0}; // jarak 3D = 5.38
    ad1.move(target);
    ad1.status();
    cout << "----------------------------------------\n";

    // 4️. Fleet: 3 Waypoints Mission
    cout << "[4] Fleet Mission 3 Waypoints\n";
    obstacles.clear();
    obstacles.push_back({5.0, 5.0, 0.0});
    obstacles.push_back({12.0, 3.0, 0.0});

    fleet.push_back(make_unique<GroundRobot>("GR-1", 20.0));
    fleet.push_back(make_unique<AerialDrone>("AD-1", 30.0));
    fleet.push_back(make_unique<Amphibious>("AM-1", 25.0, Amphibious::Mode::GROUND));
    fleet.push_back(make_unique<Amphibious>("AM-2", 25.0, Amphibious::Mode::AIR));

    vector<position> waypoints = {
        {4, 4, 0},
        {10, 2, 0},
        {15, 5, 5}
    };

    for (size_t i = 0; i < waypoints.size(); ++i) {
        cout << "\n== Waypoint " << i+1 << " ==\n";
        for (auto& vptr : fleet) {
            position target = waypoints[i];
            cout << "-> " << vptr->getId() << " menuju (" 
                 << target.x << ", " << target.y << ", " << target.z << ")\n";

            // Tentukan jenis kendaraan dan cara bergerak
            if (auto ad = dynamic_cast<AerialDrone*>(vptr.get())) {
                if (target.z <= 0) target.z = 1.0;
                ad->move(target);
            } 
            else if (auto gr = dynamic_cast<GroundRobot*>(vptr.get())) {
                double dx = target.x - gr->getPosition().x;
                double dy = target.y - gr->getPosition().y;
                gr->move(dx, dy, 0.0);
            } 
            else if (auto am = dynamic_cast<Amphibious*>(vptr.get())) {
                if (target.z > 0) am->setMode(Amphibious::Mode::AIR);
                else am->setMode(Amphibious::Mode::GROUND);
                double dx = target.x - am->getPosition().x;
                double dy = target.y - am->getPosition().y;
                double dz = target.z - am->getPosition().z;
                am->move(dx, dy, dz);
            }
            vptr->status();
        }
    }

    cout << "\n=== SIMULASI SELESAI ===\n";
    return 0;
}
