#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include <chrono>

class Car{
public:
    Car() = default;
    Car(float x, float y, float vx, float vy, const std::string& filePath):xPos(x),yPos(y),xVel(vx), yVel(vy){
        sprRedCar = std::make_unique<olc::Sprite>(filePath);
    }
    virtual void updateCar(float time) {
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
    auto getPtr(){
        return sprRedCar.get();
    }
private:
    float xPos;
    float yPos;
    float xVel;
    float yVel;
    std::unique_ptr<olc::Sprite> sprRedCar;

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

        // Create Car..
        // Todo: May be add these to a vector
        BottomUp = Car((float)13 * 15, (float) 9 * 32, 0,15, "../sprites/car-red.png");
        LeftRight = Car((float)0, (float) 4 * 33, 13,0, "../sprites/car-red-left.png");
        TopBottom = Car((float)7 * 30, (float) 0 , 0,-15, "../sprites/car-silver-top-bottom.png");
        RightLeft = Car((float)13 * 32, (float) 4 * 37, -15,0, "../sprites/car-yellow-right-left.png");

        m_StartTime = std::chrono::steady_clock::now();
        return true;
    }
    bool OnUserUpdate(float fElapsedTime) override{
        Clear(olc::BLACK);
        drawRoad();
        DrawSprite(olc::vi2d((int)BottomUp.getX(), (int)BottomUp.getY()), BottomUp.getPtr());
        DrawSprite(olc::vi2d( (int)LeftRight.getX(), (int)LeftRight.getY()), LeftRight.getPtr());
        DrawSprite(olc::vi2d((int)TopBottom.getX(), (int)TopBottom.getY()), TopBottom.getPtr());
        DrawSprite(olc::vi2d((int)RightLeft.getX(), (int)RightLeft.getY()), RightLeft.getPtr());


        // Update Car Position..
        BottomUp.updateCar(fElapsedTime);
        LeftRight.updateCar(fElapsedTime);
        TopBottom.updateCar(fElapsedTime);
        RightLeft.updateCar(fElapsedTime);


        int curr_dur = (int)elapsedSeconds();

        DrawString(0,0,"Timer:" + std::to_string(curr_dur));

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

private:
    std::unique_ptr<olc::Sprite> sprHorizontal;
    std::unique_ptr<olc::Sprite> sprVertical;
    std::unique_ptr<olc::Sprite> sprJunction;


    Car BottomUp;
    Car LeftRight;
    Car TopBottom;
    Car RightLeft;

    std::chrono::time_point<std::chrono::steady_clock> m_StartTime;

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
