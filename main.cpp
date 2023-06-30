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
    olc::Sprite* getPtr() const{
        return sprCar.get();
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
        return {(float)7 * 30, (float) 0 , 0,-10, root + vFileName[index] + "-top-bottom.png"};
    }
    Car BottomToTop(){
        int index = rand() % 3;
//        std::cout << root + vFileName[index] + "bottom-top.png" << '\n';
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

        srand(time(nullptr));


        sprHorizontal = std::make_unique<olc::Sprite>("../sprites/horizontal_road.png");
        sprVertical = std::make_unique<olc::Sprite>("../sprites/vertical_road.png");
        sprJunction = std::make_unique<olc::Sprite>("../sprites/junction.png");




//        myCar.emplace_back(myFactory.All());

        myCar.emplace_back((float)13 * 15, (float) 9 * 32, 0,15, "../sprites/car-red-bottom-top.png");
        myCar.emplace_back((float)0, (float) 4 * 33, 13,0, "../sprites/car-red-left-right.png");
        // negative velocity represents movement in opposite direction..
        myCar.emplace_back((float)7 * 30, (float) 32 , 0,-15, "../sprites/car-silver-top-bottom.png");
        myCar.emplace_back((float)13 * 32, (float) 4 * 37, -15,0, "../sprites/car-yellow-right-left.png");

        m_StartTime = std::chrono::steady_clock::now();
        return true;
    }
    bool OnUserUpdate(float fElapsedTime) override{
        Clear(olc::BLACK);

        int curr_dur = (int)elapsedSeconds();
        drawRoad();

        for(auto& elt: myCar){
            DrawSprite(olc::vi2d((int)elt.getX(), (int)elt.getY()), elt.getPtr());
        }

        // Update Car Position..
        for(auto& elt: myCar){
            elt.updateCar(fElapsedTime);
        }

        // display time..
        DrawString(0,0,"Timer:" + std::to_string(curr_dur));

        if(curr_dur % 4 == 0 && curr_dur != last_manufactured) {
            myCar.emplace_back(myFactory.BottomToTop());
            myCar.emplace_back(myFactory.LeftToRight());
            myCar.emplace_back(myFactory.RightToLeft());
            myCar.emplace_back(myFactory.TopToBottom());

            last_manufactured = curr_dur;
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

    void drawGarden(){

    }

private:
    std::unique_ptr<olc::Sprite> sprHorizontal;
    std::unique_ptr<olc::Sprite> sprVertical;
    std::unique_ptr<olc::Sprite> sprJunction;
    std::unique_ptr<olc::Sprite> sprGarden;
    std::unique_ptr<olc::Sprite> sprPond;


    std::vector<Car> myCar;
    CarFactory myFactory;

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
