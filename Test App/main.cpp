
//
// Disclaimer:
// ----------
//
// This code will work only if you selected window, graphics and audio.
//
// Note that the "Run Script" build phase will copy the required frameworks
// or dylibs to your application bundle so you can execute it on any OS X
// computer.
//
// Your resource files (images, sounds, fonts, ...) are also copied to your
// application bundle. To get the path to these resources, use the helper
// function `resourcePath()` from ResourcePath.hpp
//

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <sstream>
#include <string>

// Here is a small helper for you! Have a look.
#include "ResourcePath.hpp"

using namespace sf;

class Player {
    public:
        RectangleShape object;
        Player(float x);
};
Player::Player(float x) {
    object.setSize(Vector2f(20, 80));
    object.setPosition(x, 200);
}

bool intersects (const RectangleShape & rect1, const RectangleShape & rect2)
{
    FloatRect r1 = rect1.getGlobalBounds();
    FloatRect r2 = rect2.getGlobalBounds();
    return r1.intersects(r2);
}

int main(int, char const**)
{
    // Create the main window
    //sf::RenderWindow window(sf::VideoMode(800, 600), "SFML window");
    
    const int width = 640;
    const int height = 480;
    
    const int borderWidth = 20;
    const int margin = 50;
    
    const int moveDistance = 20;
    
    VideoMode videoMode(640,480);
    RenderWindow window(videoMode,"Pong!");
    
    // setting up borders
    
    RectangleShape top;
    RectangleShape left;
    RectangleShape right;
    RectangleShape bottom;
    
    top.setPosition(borderWidth, 0);
    top.setSize(Vector2f(width-2*borderWidth, borderWidth));
    
    left.setPosition(0, 0);
    left.setSize(Vector2f(borderWidth, height));
    
    right.setPosition(width-borderWidth, 0);
    right.setSize(Vector2f(borderWidth, height));
    
    bottom.setPosition(borderWidth, height-borderWidth);
    bottom.setSize(Vector2f(width-2*borderWidth, borderWidth));
    
    // creating ball
    
    RectangleShape ball;
    ball.setPosition(width/2, height/2);
    ball.setSize(Vector2f(20,20));
    
    Vector2f ballSpeed(0.3,0.3);
    
    // central line
    
    RectangleShape line;
    line.setPosition(width/2, 0);
    line.setSize(Vector2f(5, height));
    
    // creating players
    
    Player player1 (margin-borderWidth);
    Player player2 (width-margin);
    
    // setting up text
    
    sf::Font font;
    if (!font.loadFromFile(resourcePath() + "sansation.ttf")) {
        return EXIT_FAILURE;
    }
    
    Text title("Pong!", font, 50);
    title.setPosition(width/2-title.getGlobalBounds().width/2, 100);
    
    Text start("Press any key to start", font, 30);
    start.setPosition(width/2-start.getGlobalBounds().width/2, 400);
    
    Text won("Congratulations, you won!", font, 30);
    won.setPosition(width/2-won.getGlobalBounds().width/2, 200);
    
    Text lost("Bad luck, you lost!", font, 30);
    lost.setPosition(width/2-lost.getGlobalBounds().width/2, 200);
    
    Text score1("0", font, 20);
    score1.setPosition(width/2-100, 30);
    
    Text score2("0", font, 20);
    score2.setPosition(width/2+100, 30);
    
    // variables for tracking game state
    
    unsigned int p1Score = 0;
    unsigned int p2Score = 0;
    
    enum states {INTRO, PLAYING, P1WON, P1LOST};
    
    int gameState = INTRO;
    
    while (window.isOpen())
    {
        window.clear(Color::Black);
        
        switch (gameState)
        {
            case INTRO:
                window.draw(start);
                window.draw(title);
                break;
            case PLAYING:
                // draw borders
                window.draw(top);
                window.draw(left);
                window.draw(right);
                window.draw(bottom);
                
                // draw ball
                window.draw(ball);
                
                // draw players
                window.draw(player1.object);
                window.draw(player2.object);
                
                // draw line
                window.draw(line);
                
                // draw score
                window.draw(score1);
                window.draw(score2);
                
                break;
            case P1WON:
                window.draw(won);
                break;
            case P1LOST:
                window.draw(lost);
                break;
        }
        window.display();
        
        // handle inputs
        
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed) window.close();
            else if (event.type == Event::KeyPressed && gameState == INTRO) gameState = PLAYING;
        }
        
        if (gameState != PLAYING) continue;
        
        // MAIN GAME LOOP
        
        // moving the p2 pad
        if (ball.getPosition().y < player2.object.getPosition().y)
            player2.object.move(0, -moveDistance/50.0);
        else if (ball.getPosition().y > player2.object.getPosition().y + player2.object.getSize().y)
            player2.object.move(0, moveDistance/50.0);
        
        // handling input for p1 pad
        if (Keyboard::isKeyPressed(Keyboard::Up))
            player1.object.move(0, -moveDistance/30.0);
        else if (Keyboard::isKeyPressed(Keyboard::Down))
            player1.object.move(0, moveDistance/30.0);
        
        // ball collisions
        if (intersects(ball, top) || intersects(ball, bottom))
        {
            // vertical bounce
            ballSpeed.y = -ballSpeed.y;
        }
        if (intersects(ball, left) || intersects(ball, right))
        {
            // horizontal bounce
            ballSpeed.x = -ballSpeed.x;
        }
        if (intersects(ball, player1.object) || intersects(ball, player2.object))
        {
            ballSpeed.x = -ballSpeed.x;
            
            FloatRect p;
            if (intersects(ball, player1.object))
                p = player1.object.getGlobalBounds();
            else
                p = player2.object.getGlobalBounds();
            FloatRect b = ball.getGlobalBounds();
            ballSpeed.y = (b.top - p.top - p.height/2) / 100;
            
            ballSpeed.x = ballSpeed.x * 1.1;
            ballSpeed.y = ballSpeed.y * 1.1;
        }
        
        // update ball
        ball.move(ballSpeed.x, ballSpeed.y);
        
        // check for points scored
        if (intersects(ball, left))
        {
            p2Score++;
            score2.setString(std::to_string(p2Score));
            ball.setPosition(width/2, height/2);
            ballSpeed.x = 0.3;
            ballSpeed.y = 0.3;
        }
        if (intersects(ball, right))
        {
            p1Score++;
            score1.setString(std::to_string(p1Score));
            ball.setPosition(width/2, height/2);
            ballSpeed.x = -0.3;
            ballSpeed.y = 0.3;
        }
        
        // check for end conditions
        if (p1Score >= 10 && p1Score >= p2Score + 2)
            gameState = P1WON;
        if (p2Score >= 10 && p2Score >= p1Score + 2)
            gameState = P1LOST;
        
    }
    
    return EXIT_SUCCESS;
    
}
