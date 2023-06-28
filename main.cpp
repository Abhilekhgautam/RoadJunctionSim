#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

class Car{
public:
    Car() = default;
    Car(float x, float y, float vx, float vy, std::string filePath):xPos(x),yPos(y),xVel(vx), yVel(vy){
        sprRedCar = std::make_unique<olc::Sprite>(filePath);
    }
    virtual void updateCar(float time) {
       yPos = yPos - yVel * time;
       xPos = xPos + xVel * time;
    }
    float getX() const{
        return xPos;
    }
    float getY() const{
        return yPos;
    }

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

        BottomUp = Car((float)13 * 15, (float) 9 * 32, 0,15, "../sprites/car-red.png");
        LeftRight = Car((float)0, (float) 4 * 33, 10,0, "../sprites/car-red-left.png");
        TopBottom = Car((float)7 * 30, (float) 0 , 0,-10, "../sprites/car-silver-top-bottom.png");
        RightLeft = Car((float)13 * 32, (float) 4 * 37, -10,0, "../sprites/car-yellow-right-left.png");

        return true;
    }
    bool OnUserUpdate(float fElapsedTime) override{
        Clear(olc::BLACK);
        drawRoad();
        DrawSprite(olc::vi2d(BottomUp.getX(), BottomUp.getY()), BottomUp.getPtr());
        DrawSprite(olc::vi2d(LeftRight.getX(), LeftRight.getY()), LeftRight.getPtr());
        DrawSprite(olc::vi2d(TopBottom.getX(), TopBottom.getY()), TopBottom.getPtr());
        DrawSprite(olc::vi2d(RightLeft.getX(), RightLeft.getY()), RightLeft.getPtr());


        BottomUp.updateCar(fElapsedTime);
        LeftRight.updateCar(fElapsedTime);
        TopBottom.updateCar(fElapsedTime);
        RightLeft.updateCar(fElapsedTime);

        return true;
    }
    void drawRoad(){

        // I know this is manual but nested loop are bad (Evil Laughter..)
        // draw horizontal portion of the road
        DrawSprite(olc::vi2d((int)  0 * 32, (int)  4 * 32), sprHorizontal.get());
        DrawSprite(olc::vi2d((int)  1 * 32, (int)  4 * 32), sprHorizontal.get());
        DrawSprite(olc::vi2d((int)  2 * 32, (int)  4 * 32), sprHorizontal.get());
        DrawSprite(olc::vi2d((int)  3 * 32, (int)  4 * 32), sprHorizontal.get());
        DrawSprite(olc::vi2d((int)  4 * 32, (int)  4 * 32), sprHorizontal.get());
        DrawSprite(olc::vi2d((int)  5 * 32, (int)  4 * 32), sprHorizontal.get());
        // draw a junction
        DrawSprite(olc::vi2d((int)  6 * 32, (int)  4 * 32), sprJunction.get());
        // draw a vertical road portion
        DrawSprite(olc::vi2d((int)  6 * 32, (int)  0 * 32), sprVertical.get());
        DrawSprite(olc::vi2d((int)  6 * 32, (int)  1 * 32), sprVertical.get());
        DrawSprite(olc::vi2d((int)  6 * 32, (int)  2 * 32), sprVertical.get());
        DrawSprite(olc::vi2d((int)  6 * 32, (int)  3 * 32), sprVertical.get());
        DrawSprite(olc::vi2d((int)  6 * 32, (int)  5 * 32), sprVertical.get());
        DrawSprite(olc::vi2d((int)  6 * 32, (int)  6 * 32), sprVertical.get());
        DrawSprite(olc::vi2d((int)  6 * 32, (int)  7 * 32), sprVertical.get());
        DrawSprite(olc::vi2d((int)  6 * 32, (int)  8 * 32), sprVertical.get());
        DrawSprite(olc::vi2d((int)  6 * 32, (int)  9 * 32), sprVertical.get());
        DrawSprite(olc::vi2d((int)  6 * 32, (int)  10 * 32), sprVertical.get());

        //draw horizontal portion of the road
        DrawSprite(olc::vi2d((int)  7 * 32, (int)  4 * 32), sprHorizontal.get());
        DrawSprite(olc::vi2d((int)  8 * 32, (int)  4 * 32), sprHorizontal.get());
        DrawSprite(olc::vi2d((int)  9 * 32, (int)  4 * 32), sprHorizontal.get());
        DrawSprite(olc::vi2d((int)  10 * 32, (int)  4 * 32), sprHorizontal.get());
        DrawSprite(olc::vi2d((int)  11 * 32, (int)  4 * 32), sprHorizontal.get());
        DrawSprite(olc::vi2d((int)  12 * 32, (int)  4 * 32), sprHorizontal.get());
        DrawSprite(olc::vi2d((int)  13 * 32, (int)  4 * 32), sprHorizontal.get());

    }

private:
    std::unique_ptr<olc::Sprite> sprHorizontal;
    std::unique_ptr<olc::Sprite> sprVertical;
    std::unique_ptr<olc::Sprite> sprJunction;

    Car BottomUp;
    Car LeftRight;
    Car TopBottom;
    Car RightLeft;
};

int main() {
    RoadSim mySimulation;

    if(mySimulation.Construct(450, 340, 4,4))
        mySimulation.Start();
}
