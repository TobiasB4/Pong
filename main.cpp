#include <SFML/Graphics.hpp>
#include <iostream>
#include <stdlib.h>
#include <time.h>

const float WINDOWHEIGHT = 720.f;
const float WINDOWWIDTH = 960.f;

//Idk why this doesn't work something to do with different 
//parameter types??? to be compared with sf::Keyboard::Keys
//enum Keys {
//    Left=sf::Keyboard::Left,
//    Right,
//    Up,
//    Down
//};

class Ball {
private:
    float dy = 5.0f;
    float dx = 5.0f;
public:
    //Ball Shape and Size
    sf::RectangleShape ballShape;
    float width = 20.f;
    float height = width;

    //Ball Position
    float xcurr = NULL;
    float ycurr = NULL;

    Ball() {
        ballShape.setFillColor(sf::Color::White);
        ballShape.setSize(sf::Vector2f(width, height));
        xcurr = WINDOWWIDTH / 2.f - width / 2.f;
        ycurr = WINDOWHEIGHT / 2.f - height / 2.f;
        ballShape.setPosition(xcurr, ycurr);
    }

    void updatePos(){
        if (xcurr <= 0) {
            dx = abs(dx);
        }else if(xcurr >= WINDOWWIDTH) {
            dx = -abs(dx);
        }
        if (ycurr <= 0) {
            dy = abs(dy);
        }
        else if (ycurr >= WINDOWHEIGHT) {
            dy = -abs(dy);
        }
        xcurr += dx;
        ycurr += dy;
        ballShape.setPosition(xcurr, ycurr);
        
    }

    bool goalCheck() {
        bool goal = false;
        if ((xcurr <= 0.f)||(xcurr >= WINDOWWIDTH-width)) {
            xcurr = WINDOWWIDTH / 2.f - width / 2.f;
            ycurr = WINDOWHEIGHT / 2.f - height / 2.f;
            ballShape.setPosition(xcurr, ycurr);
            goal = true;
            sf::sleep(sf::seconds(0.25));
        }
        return goal;
    }

    void collisionCheck(float padOnex, float padOney,float padTwox, float padTwoy, float padWidth, float padHeight) {
        //if inside paddle one then set ball pos to right edge of paddle
        if ((xcurr >=padOnex && xcurr <= padOnex+padWidth) && (ycurr >=padOney && ycurr <=padOney+padHeight)) {
            xcurr += (padOnex+padWidth)-xcurr;
            ballShape.setPosition(xcurr,ycurr);
            //after bounce change direction
            //RANDOMIZE
            dx = abs(dx);
            dy = this->randomBounce(rand()%2)*abs(dy);
        }
        //if inside paddle two then set ball pos to left edge of paddle
        else if ((xcurr >= padTwox && xcurr <= padTwox + padWidth) && (ycurr >= padTwoy && ycurr <= padTwoy + padHeight)) {
            xcurr -= (padTwox + padWidth) - xcurr;
            ballShape.setPosition(xcurr, ycurr);
            //randomize the bounce
            dx = -abs(dx);
            dy = this->randomBounce(rand() % 2) * abs(dy);
        }
    }

    float randomBounce(bool neg) {
        float result = 0.5f + static_cast<float>(rand())/(static_cast<float>(RAND_MAX/(1.25f-0.5f)));
        if (neg) {
            return -result;
        }else
        return result;
    }
};

class Paddles {
private:
    float dy = 7.5f;
public:
    //Paddle Shape and Size
    sf::RectangleShape Pad;
    float width = 20.f;
    float height = 100.f;

    //Paddle Position
    float x = NULL;
    float ycurr = NULL;


    Paddles(float xstart){
        Pad.setFillColor(sf::Color::White);
        Pad.setSize(sf::Vector2f(width,height));
        this->resetPaddles(xstart);
    }

     void updatePos(float x, float y,bool flag) {

         if (flag) {
             if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)&&y>=0.f) {
                 dy = -abs(dy);
                 ycurr = y + dy;
             }
             //No idea where this random 10.f came from but need it so that the paddle reaches the bottom of the screen
             if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)&& y <= WINDOWHEIGHT - height + 5.f) {
                 dy = abs(dy);
                 ycurr = y + dy;
             }
         }
         else {
             if (sf::Keyboard::isKeyPressed(sf::Keyboard::W ) && y >= 0.f) {
                 dy = -abs(dy);
                 ycurr = y + dy;
             }
             if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && y <= WINDOWHEIGHT-height) {
                 dy = abs(dy);
                 ycurr = y + dy;
             }
         }
         Pad.setPosition(x, ycurr);
         
    }
     void resetPaddles(float xstart) {
         ycurr = WINDOWHEIGHT / 2.f - height / 2.f;
         x = xstart;
         Pad.setPosition(x, ycurr);
     }
};

int main()
{
    sf::RenderWindow window(sf::VideoMode(WINDOWWIDTH,WINDOWHEIGHT), "Pong");
    window.setFramerateLimit(60);
    srand(time(NULL));
    //for PadRight I just used subtracted the width of the paddle so it would start on screen
    //Look for fix
    Paddles PadLeft(40.f), PadRight(WINDOWWIDTH-70.f);
    Ball ball;
    bool flag = false;
    
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        PadRight.updatePos(PadRight.x,PadRight.ycurr,true);
        PadLeft.updatePos(PadLeft.x,PadLeft.ycurr,false);
        ball.updatePos();

        ball.collisionCheck(PadLeft.x, PadLeft.ycurr, PadRight.x, PadRight.ycurr, PadRight.width, PadRight.height);
        flag = ball.goalCheck();
        if (flag) {
            PadLeft.resetPaddles(PadLeft.x);
            PadRight.resetPaddles(PadRight.x);
        }

        window.clear();
        window.draw(PadLeft.Pad);
        window.draw(PadRight.Pad);
        window.draw(ball.ballShape);
        window.display();
        if (flag) {
            sf::sleep(sf::seconds(1.5));
        }
    }

    return 0;
}