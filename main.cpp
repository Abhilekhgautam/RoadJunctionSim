#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include <chrono>
#include <algorithm>

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

    // overload
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

    bool outOfScreen(Car& car){
        if(car.getX() < -10 || car.getY() < -10 || car.getX() > ScreenWidth() + 30 || car.getY() > ScreenHeight()) return true;
        else return false;
    }

    bool OnUserCreate() override {

        sprHorizontal = std::make_unique<olc::Sprite>("../sprites/horizontal_road.png");
        sprVertical = std::make_unique<olc::Sprite>("../sprites/vertical_road.png");
        sprJunction = std::make_unique<olc::Sprite>("../sprites/junction.png");
        sprPond = std::make_unique<olc::Sprite>("../sprites/pond.png");

        vCar.emplace_back((float)13 * 15, (float) 9 * 32, 0,15, "../sprites/car-red-bottom-top.png");
        vCar.emplace_back((float)0, (float) 4 * 33, 13,0, "../sprites/car-red-left-right.png");
        // negative velocity represents movement in opposite direction..
        vCar.emplace_back((float)7 * 30, (float) 32 , 0,-15, "../sprites/car-silver-top-bottom.png");
        vCar.emplace_back((float)13 * 32, (float) 4 * 37, -15,0, "../sprites/car-yellow-right-left.png");

        m_StartTime = std::chrono::steady_clock::now();
        moveX = false;
        moveY = true;

        return true;
    }
    bool OnUserUpdate(float fElapsedTime) override{
        Clear(olc::CYAN);
        drawRoad();

        int curr_dur = (int)elapsedSeconds();

        for(auto& elt: vCar){
            if(!outOfScreen(elt))
             DrawSprite(olc::vi2d((int)elt.getX(), (int)elt.getY()), elt.getPtr());
        }

        //Draw pond
        DrawSprite(olc::vi2d(300, 200), sprPond.get());
        DrawSprite(olc::vi2d(10, 160), sprPond.get());
        DrawSprite(olc::vi2d(10, 60), sprPond.get());


        // Draw Strip(for Traffic Light)
        for(int i = 0 ; i < 60 ; i++){
            if(moveX){
                FillCircle( i * 8,  4 * 32 + 15,1, olc::GREEN);
                if(i >= 16 && i <= 20) continue;
                FillCircle(6 * 32 + 15, i * 8, 1, olc::RED);
            }

            if(moveY) {
                FillCircle(i * 8, 4 * 32 + 15, 1, olc::RED);
                if(i >= 16 && i <= 20) continue;
                FillCircle(6 * 32 + 15, i * 8, 1, olc::GREEN);
            }
        }

        // Update Car Position..
        for(auto& elt: vCar){
            if(!shouldStopJun(elt) && !shouldStopCollision(elt))
              elt.updateCar(fElapsedTime);
        }
        // display time..
        DrawString(0,0,"Timer:" + std::to_string(curr_dur) + " Sec");

	// produce car every 5 secs
        if(curr_dur % 5 == 0 && curr_dur != last_manufactured) {
                vCar.emplace_back(myFactory.BottomToTop());
                if(curr_dur % 10 == 0 )
                   vCar.emplace_back(myFactory.TopToBottom());

                vCar.emplace_back(myFactory.LeftToRight());
                vCar.emplace_back(myFactory.RightToLeft());
            
            last_manufactured = curr_dur;
        }

        // update traffic light
        if(curr_dur != 0 && curr_dur % 20 == 0 && last_updated !=curr_dur){
            moveX = !moveX;
            moveY = !moveY;

            last_updated = curr_dur;
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
        for(int i = 7 ; i < 15 ; i++){
            DrawSprite(olc::vi2d((int)  i * 32, (int)  4 * 32), sprHorizontal.get());
        }
    }
    // return distance to junction relative from the car
    float getJunDist(Car& car) const {
        if(getMoveDirection(car) == 0)
            return (car.getY() - 3.50f * 32.0f);
            // bottom to top
        else if(getMoveDirection(car) == 2)
            return(car.getY() - 5.0f * 32.0f);
            // right to left
        else if(getMoveDirection(car) == 1)
            return(car.getX() - 7 * 32.0f);
        //left to right
        else return (car.getX() - 5 * 32.0f);
    }

    // checks if a car crossed the junction
    bool crossedJun(Car& car){
        if(getMoveDirection(car) == 0) {
            if ((car.getY() - 3.50f * 32.0f) < 0) return false;
            return true;
        }
        else if(getMoveDirection(car) == 2) {
                if ((car.getY() - 5.0f * 32.0f) > 0) return false;
                return true;
            }
        else if(getMoveDirection(car) == 1){
            if((car.getX() - 7 * 32.0f) > 0) return false;
            return true;
        }
        else {
            if((car.getX() - 6 * 32.0f) < 0) return false;
            return true;
        }
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

    // fix this :(
    bool shouldStopJun(Car& car) {
        auto dir = getMoveDirection(car);
        if((dir == 0 || dir == 2) && moveX){
            if(crossedJun(car)) return false;
            else {
                if(dir == 0){
                    if(abs(car.getY() + 4 - 3.50f * 32.0f) > 0 && abs(car.getY() - 3.50f * 32.0f) <= 3) return true;
                    else return false;
                }
                else{
                    if(abs(car.getY() + 4- 5 * 32.0f) > 0 && abs(car.getY()  - 5 * 32.0f) <= 3) return true;
                    else return false;
                }
            }
        }
        else if((dir == 1 || dir == 3) && moveY){
            if(crossedJun(car)) return false;
            else {
                if(dir == 1){
                    if(abs(car.getX()  - 7 * 32.0f) > 0 && abs(car.getX() - 7 * 32.0f) <= 3) return true;
                    else return false;
                }
                else{
                    if(abs(car.getX() - 5.5f * 32.0f) > 0 && abs(car.getX() - 5.5f * 32.0f) <= 3) return true;
                    else return false;
                }
            }
        }
        else if((dir == 1 || dir == 3) && moveX){
           return false;
        }
        else if((dir == 0 || dir == 2) && moveY){
            return false;
        }
	// probably won't reach here
        else return false;
    }

    // same lane collision prevention
    bool shouldStopCollision(Car& car)  {
          for (auto &elt: vCar) {
            if (getMoveDirection(car) == getMoveDirection(elt) && elt != car) {
                int dir = getMoveDirection(car);
                if(dir == 0){
                    if(car.getY() < elt.getY()){
                        if(std::abs(car.getY() - elt.getY()) < 30) return true;
                    }
                }
                else if(dir == 1){
                    if(car.getX() > elt.getX()){
                        if(std::abs(car.getX() - elt.getX()) < 30) return true;
                    }
                }
                else if(dir == 2){
                    if(car.getY() > elt.getY()){
                        if(std::abs(car.getY() - elt.getY()) < 30) return true;
                    }
                }
                else{
                    if(car.getX() < elt.getX()){
                        if(std::abs(car.getX() - elt.getX()) < 30) return true;
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

    std::unique_ptr<olc::Sprite> sprPond;
    std::unique_ptr<olc::Sprite> sprGarden;

    bool moveX;
    bool moveY;

    std::vector<Car> vCar;
    CarFactory myFactory;


    std::chrono::time_point<std::chrono::steady_clock> m_StartTime;
    int last_manufactured = 0;
    int last_updated = 0;

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

    return 0; // happy compiler :)
}
