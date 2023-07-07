#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <chrono>

class Car{
public:
    Car() = default;
    Car(float x, float y, float vx, float vy, const std::string& filePath):xPos(x),yPos(y),xVel(vx), yVel(vy){
        sprCar = std::make_unique<olc::Sprite>(filePath);
    }
    void updateCar(float time) {
       yPos = yPos - yVel * time;
       xPos = xPos + xVel * time;
    }
    [[nodiscard]]
    float getX() const{
        return xPos;
    }
    [[nodiscard]]
    float getY() const{
        return yPos;
    }
    [[nodiscard]]
    float getXVel() const{
        return xVel;
    }
    [[nodiscard]]
    float getYVel() const{
        return yVel;
    }
    [[nodiscard]]
    olc::Sprite* getPtr() const{
        return sprCar.get();
    }

    bool operator != (const Car& car) const{
        if (this->getX() == car.getX() && this->getY() == car.getY() && this->getXVel() == car.getYVel()) return false;
        else return true;
    }

private:
    float xPos;
    float yPos;
    float xVel;
    float yVel;
    std::unique_ptr<olc::Sprite> sprCar;
};

class CarFactory{
public:
    CarFactory() = default;
    Car LeftToRight(){
        int index = rand() % 3;

        return {(float)-1 * 32, (float) 4 * 33, 13,0, root + vFileName[index] + "-left-right.png"};
    }
    Car RightToLeft(){
        int index = rand() % 3;
        return {(float)15 * 32, (float) 4 * 37, -15,0, root + vFileName[index] + "-right-left.png"};
    }
    Car TopToBottom(){
        int index = rand() % 3;
        return {(float)7 * 30, (float) -9.0f , 0,-10, root + vFileName[index] + "-top-bottom.png"};
    }
    Car BottomToTop(){
        int index = rand() % 3;
        return {(float)13 * 15, (float) 13 * 32 , 0,15, root + vFileName[index] + "-bottom-top.png"};
    }

private:
    std::vector<std::string> vFileName = {"car-yellow","car-red", "car-silver"};
    std::string root = "../sprites/";
};


class RoadSim : public olc::PixelGameEngine {
public:
    RoadSim() {
        sAppName = "Simulation of Road Junction";
    }
    bool OnUserCreate() override {

        sprHorizontal = std::make_unique<olc::Sprite>("../sprites/horizontal_road.png");
        sprVertical = std::make_unique<olc::Sprite>("../sprites/vertical_road.png");
        sprJunction = std::make_unique<olc::Sprite>("../sprites/junction.png");

        vCar.emplace_back((float)13 * 15, (float) 9 * 32, 0,15, "../sprites/car-red-bottom-top.png");
        vCar.emplace_back((float)0, (float) 4 * 33, 13,0, "../sprites/car-red-left-right.png");
        // negative velocity represents movement in opposite direction..
        vCar.emplace_back((float)7 * 30, (float) 32 , 0,-15, "../sprites/car-silver-top-bottom.png");
        vCar.emplace_back((float)13 * 32, (float) 4 * 37, -15,0, "../sprites/car-yellow-right-left.png");

        m_StartTime = std::chrono::steady_clock::now();
        return true;
    }
    bool OnUserUpdate(float fElapsedTime) override{
        Clear(olc::BLACK);

        int curr_dur = (int)elapsedSeconds();
        drawRoad();

        for(auto& elt: vCar){
            DrawSprite(olc::vi2d((int)elt.getX(), (int)elt.getY()), elt.getPtr());
        }

        // Update Car Position..
        for(auto& elt: vCar){
            if(!shouldStopJun(elt) && !shouldStopCollision(elt))
              elt.updateCar(fElapsedTime);
        }
        // display time..
        DrawString(0,0,"Timer:" + std::to_string(curr_dur) + " Sec");

        if(curr_dur % 4 == 0 && curr_dur != last_manufactured) {
                vCar.emplace_back(myFactory.BottomToTop());
                vCar.emplace_back(myFactory.TopToBottom());
                vCar.emplace_back(myFactory.LeftToRight());
                vCar.emplace_back(myFactory.RightToLeft());

            last_manufactured = curr_dur;
        }

        // update traffic light
        if(curr_dur != 0 && curr_dur % 40 == 0){
            moveX = !moveX;
            moveY = !moveY;
        }

        return true;
    }
    void drawRoad(){
        // draw horizontal portion of the road
        for(int i = 0 ; i < 6 ; i++){
            DrawSprite(olc::vi2d((int)  i * 32, (int)  4 * 32), sprHorizontal.get());
        }
        // draw a junction
        DrawSprite(olc::vi2d((int)  6 * 32, (int)  4 * 32), sprJunction.get());
        // draw a vertical road portion
        for(int i = 0 ; i < 11 ; i++){
            // skip the junction
            if (i == 4) continue;
            DrawSprite(olc::vi2d((int)  6 * 32, (int)  i * 32), sprVertical.get());
        }
        //draw horizontal portion of the road
        for(int i = 7 ; i < 14 ; i++){
            DrawSprite(olc::vi2d((int)  i * 32, (int)  4 * 32), sprHorizontal.get());
        }
    }

    float getJunDist(Car& car) const {
        if(getMoveDirection(car) == 0)
            return (abs(car.getY() - 3.50f * 32.0f));
            // bottom to top
        else if(getMoveDirection(car) == 2)
            return(abs(car.getY() - 5.0f * 32.0f));
            // right to left
        else if(getMoveDirection(car) == 1)
            return(abs(car.getX() - 7.20f * 32.0f));
        //left to right
        else return (abs(car.getX() - 5.30f * 32.0f));
    }

    int getMoveDirection(Car& car) const{
        // top to bottom
        if(car.getXVel() == 0 && car.getYVel() < 0) return 0;
        // right to left
        else if(car.getXVel() < 0 && car.getYVel() == 0) return 1;
        // bottom to top
        else if(car.getXVel() == 0 && car.getYVel() > 0) return 2;
        //left to right
        else return 3;
    }

    bool shouldStopJun(Car& car) const {
        int dir = getMoveDirection(car);
        if(moveX){
            if((dir == 1 || dir == 3)) return false;
            else if (getJunDist(car) > 0) return false;
            else return true;
        }
        else if(moveY){
            if((dir == 0 || dir == 2)) return false;
            else if (getJunDist(car) > 0) return false;
            else return true;
        }
        else return true;
    }

    bool shouldStopCollision(Car& car)  {
          for (auto &elt: vCar) {
            if (getMoveDirection(car) == getMoveDirection(elt) && elt != car) {
                int dir = getMoveDirection(car);
                if(dir == 0){
                    if(car.getY() < elt.getY()){
                        if(abs(car.getY() - elt.getY()) < 25) return true;
                    }
                }
                else if(dir == 1){
                    if(car.getX() > elt.getX()){
                        if(abs(car.getX() - elt.getX()) < 25) return true;
                    }
                }
                else if(dir == 2){
                    if(car.getY() > elt.getY()){
                        if(abs(car.getY() - elt.getY()) < 25) return true;
                    }
                }
                else{
                    if(car.getX() < elt.getX()){
                        if(abs(car.getX() - elt.getX()) < 25) return true;
                    }
                }
            }
        }
    return false;
    }

private:
    std::unique_ptr<olc::Sprite> sprHorizontal;
    std::unique_ptr<olc::Sprite> sprVertical;
    std::unique_ptr<olc::Sprite> sprJunction;

    bool moveX = true;
    bool moveY = false;

    std::vector<Car> vCar;
    CarFactory myFactory;

    //Coordinates for Road Junction
    std::vector<int> vTopBottomJun = {6 * 32, 5 * 32, 7 * 32, 5 * 32};
    std::vector<int> vBottomTopJun = {6 * 32, 4 * 32, 7 * 32, 4 * 32};
    std::vector<int> vLeftRightJun = {6 * 32, 4 * 32, 6 * 32, 5 * 32};
    std::vector<int> vRightLeftJun = {7 * 32, 4 * 32, 7 * 32, 5 * 32};

    std::chrono::time_point<std::chrono::steady_clock> m_StartTime;
    int last_manufactured = 0;

    double elapsedSeconds(){
        std::chrono::time_point<std::chrono::steady_clock> endTime;
        endTime = std::chrono::steady_clock::now();

        return std::chrono::duration<double>(endTime - m_StartTime).count();
    }

};

int main() {
    RoadSim mySimulation;

    if(mySimulation.Construct(450, 340, 4,4))
        mySimulation.Start();
}
