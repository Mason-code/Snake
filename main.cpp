// By Mason
// Using Snake Case as the naming convention, C++ as the language, SFML as the graphical library, and Visual Studio as the IDE


/* 
multi que movement strat.
*/


#include <iostream>
#include <vector>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <random>
#include <math.h>
#include <cmath>


//global
/*apple*/
sf::Texture apple_texture;

/*snake*/
float x_velocity = 0;
float y_velocity = 0;
std::vector<sf::Vector2f> need_turn = {};

/*tail*/
struct TailVectorData {
    sf::CircleShape shape;
    sf::Vector2f current_velocity;
    sf::Vector2f turn_position;
    sf::Vector2f next_velocty_at_pos;
};

std::vector<TailVectorData> snake_tail_vec = {};



int apples_eaten = 0;




//  function declarations
sf::Sprite rand_apple(sf::Sprite& the_apple_sprite);
std::vector<sf::Vector2f> move_snake(sf::CircleShape& the_snake_circle);
sf::Vector2f get_current_tile(sf::CircleShape item);
sf::Vector2f get_center_position_of_tile(int x_tile, int y_tile);
void add_tail(sf::CircleShape& head);
void move_first_tail(int iteration_int, sf::CircleShape& iteratee_tail, sf::Vector2f new_velocity, sf::Vector2f new_position_of_turn);




int main()
{

    // create the window
    sf::RenderWindow window(sf::VideoMode({ 800, 800 }), "Snake");



    /*Position Point: testing*/
    sf::CircleShape point(5.f);
    // point.setPosition({ -point_radius + x_grid_start + half_of_tile_width + (tile_width * (which_tile - offset)), -point_radius + y_grid_start + half_of_tile_width + (tile_width * (which_tile - offset))});
    point.setPosition({ -5 + 25 + 22 +(44 * (5-1)), -5 + 105 + 22 +(44 * (5-1))});


    /*Map--Start*/
    //map border
    sf::RectangleShape map_border({ 800.f, 800.f });
    map_border.setFillColor(sf::Color(87, 138, 52));

    // map header
    sf::RectangleShape header_rectangle({ 800.f, 70.f });
    header_rectangle.setFillColor(sf::Color(74, 117, 44));
    
    // map
    std::vector<sf::RectangleShape> map_tiles = {};

    float tile_width = 44;
    float tile_height = 44;
    sf::RectangleShape tile({ tile_width, tile_height });
    tile.setPosition({ -tile_width + 25, 70 + 35 });
    for (int i = 0; i < 15; i++) {
        for (int k = 0; k < 17; k++) {
            tile.move({ tile_width, 0 });

            if ((k + i) % 2 == 0) {
                tile.setFillColor(sf::Color(162, 209, 73));
            }
            else {
                tile.setFillColor(sf::Color(170, 215, 81));
            }

            map_tiles.push_back(tile);
        }
        tile.move({ tile_width * -17, tile_height });
    }
    /*Map--End*/

    /*Apple--Start*/
    if (!apple_texture.loadFromFile("apple.png")) {
        return -1; 
    }
    sf::Sprite apple_sprite(apple_texture);
    apple_texture.setSmooth(true);
    apple_sprite.setPosition({ -40 + 19.5 + (13 * 44), 70 + -25 + (8 * 44) });

    sf::Vector2u texture_size = apple_texture.getSize();

    // Calculate the scale factors
    float scale_x = 50.f / texture_size.x;
    float scale_y = 70.f / texture_size.y;

    apple_sprite.setScale({ scale_x, scale_y });
    /*Apple--End*/
    


    /*Snake_head--Start*/
    sf::CircleShape snake_head(20.f);
    snake_head.setFillColor(sf::Color::Blue);
    snake_head.setPosition({ -20 + 25 + 22 + (44 * (4 - 1)), -20 + 105 + 22 + (44 * (8 - 1)) });
    
    /*Snake_head--End*/


    std::vector<sf::Vector2f> tail_values;

    
    add_tail(snake_head);


    // run the program as long as the window is open
    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        while (const std::optional event = window.pollEvent())
        {
            // "close requested" event: we close the window
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        // clear the window with black color
        window.clear(sf::Color::Black);

        // draw everything here...
        window.draw(map_border);
        window.draw(header_rectangle);

        // draw map 
        // for tile (immutable) in{ : } map_tiles -> loop
        for (const sf::RectangleShape& tile : map_tiles) {
            window.draw(tile);
        }



        /*window.draw(apple_sprite);
        once snake == apple{
            window.draw(rand_apple(apple_sprite));
        } */
        

        tail_values = move_snake(snake_head);
        window.draw(snake_head);

        int iteration_value = 0;

        TailVectorData& first_data = snake_tail_vec[iteration_value];
        move_first_tail(iteration_value, first_data.shape, tail_values[0], tail_values[1]);
        window.draw(first_data.shape);

        /*for (auto& tail : snake_tail_vec) {
            move_first_tail(tail.shape, tail_values[0], tail_values[1]);
            window.draw(tail.shape);
        }*/
        


        // end the current frame
        window.display();
    }
}



/*function definitions*/


sf::Sprite rand_apple(sf::Sprite& the_apple_sprite) {
   /* fancy fucking random shit*/
    // Create a random device and seed the random number generator
    std::random_device rd;
    std::mt19937 gen(rd());

    // Define the range [0, 15]
    std::uniform_int_distribution<> distrib_y(1, 15);

    // Generate and print a random number in the range [0, 15]
    float random_y = distrib_y(gen);

    // rando y
    std::uniform_int_distribution<> distrib_x(1, 17);
    float random_x = distrib_x(gen);



    float starting_x = -40; 
    float starting_y = 70; 

    float center_x = 19.5;
    float center_y = -25;
    
    the_apple_sprite.setPosition({ starting_x + center_x + (random_x * 44), starting_y + center_y + (random_y * 44) });
    
    return the_apple_sprite;
}



std::vector<sf::Vector2f> move_snake(sf::CircleShape &the_snake_circle) {
    //sped
    float ing_donuts = .1f;

    /*dirction: TSA*/
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) && need_turn.size() < 2) {
        // need_turn.empty() is needed so it doesn't break
        need_turn.push_back(sf::Vector2f(-ing_donuts, 0));
        
    }
  
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) && need_turn.size() < 2) {
        need_turn.push_back(sf::Vector2f(ing_donuts, 0));
        
    }
 
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) && need_turn.size() < 2) {
        need_turn.push_back(sf::Vector2f(0, -ing_donuts));
        
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down) && need_turn.size() < 2) {
        need_turn.push_back(sf::Vector2f(0, ing_donuts));
        
    }


    /*direction: Screening*/

    // make sure there are no velocity double ups in the list :
    std::vector<sf::Vector2f> updated_velocity_list = {};

    for (const auto& check_velocity : need_turn) {
        if (updated_velocity_list.empty() || updated_velocity_list.back() != check_velocity) {
            updated_velocity_list.push_back(check_velocity);
        }
        
    }
    need_turn = updated_velocity_list;

    /*direction: Boarding*/
   
    //top-left corner
    sf::Vector2f current_position = the_snake_circle.getPosition();

    sf::Vector2f current_tile = get_current_tile(the_snake_circle);

    sf::Vector2f center_position = get_center_position_of_tile(current_tile.x, current_tile.y);
    
    // epsilon tolerance
    float epsilon = 1.0f;
    bool at_center_turn_point =
        std::abs(center_position.x - (current_position.x + 20)) < epsilon &&
        std::abs(center_position.y - (current_position.y + 20)) < epsilon;



    sf::Vector2f current_velocity = sf::Vector2f(x_velocity, y_velocity);
    

    /*direction: Lift - off*/ 
    if (!need_turn.empty() && need_turn[0] == current_velocity) {
        need_turn.erase(need_turn.begin());
    }
    sf::Vector2f where_tail_turn = sf::Vector2f(-1.f, -1.f);
    if (at_center_turn_point && !need_turn.empty()) {

        if (need_turn.size() == 3) {
            // make sure snake can't do 180
            /*if ((need_turn[0].x + x_velocity != 0 && need_turn[0].y + y_velocity != 0) || (y_velocity == 0 && x_velocity == 0)) {*/
            if (!(need_turn[0].x == -x_velocity && need_turn[0].y == -y_velocity) || !(need_turn[1].x == -x_velocity && need_turn[1].y == -y_velocity) || !(need_turn[2].x == -x_velocity && need_turn[2].y == -y_velocity) || (y_velocity == 0 && x_velocity == 0)) {
                // this fixes the conversion error that you get when using variables inside a function (usually .move function) that uses a sf::Vector2f variable
                // don't do this: the_snake_circle.move({ x_velocity, y_velocity });
                x_velocity = need_turn[0].x;
                y_velocity = need_turn[0].y;
                where_tail_turn = the_snake_circle.getPosition();
            }
        } else if (need_turn.size() == 2) {
            // make sure snake can't do 180
            /*if ((need_turn[0].x + x_velocity != 0 && need_turn[0].y + y_velocity != 0) || (y_velocity == 0 && x_velocity == 0)) {*/
            if (!(need_turn[0].x == -x_velocity && need_turn[0].y == -y_velocity) || !(need_turn[1].x == -x_velocity && need_turn[1].y == -y_velocity) || !(need_turn[2].x == -x_velocity && need_turn[2].y == -y_velocity) || (y_velocity == 0 && x_velocity == 0)) {
                // this fixes the conversion error that you get when using variables inside a function (usually .move function) that uses a sf::Vector2f variable
                // don't do this: the_snake_circle.move({ x_velocity, y_velocity });
                x_velocity = need_turn[0].x;
                y_velocity = need_turn[0].y;
                where_tail_turn = the_snake_circle.getPosition();
            }
        }
        else {
            if (!(need_turn[0].x == -x_velocity && need_turn[0].y == -y_velocity) || (y_velocity == 0 && x_velocity == 0)) {
                // this fixes the conversion error that you get when using variables inside a function (usually .move function) that uses a sf::Vector2f variable
                // don't do this: the_snake_circle.move({ x_velocity, y_velocity });
                x_velocity = need_turn[0].x;
                y_velocity = need_turn[0].y;
                where_tail_turn = the_snake_circle.getPosition();
            }
        }
    

        need_turn.erase(need_turn.begin());
        //std::cout << "erased!!!!!!!!!!!!!!!\n\n";
    }
    

    
     the_snake_circle.move(sf::Vector2f(x_velocity, y_velocity));
    
     // velocity (you know whether to get this based off the pos), position = sf::Vector2f(-1.f, -1.f)
     std::vector<sf::Vector2f> bruh = { sf::Vector2f(x_velocity, y_velocity), where_tail_turn };

     return bruh;
    }
    

    




sf::Vector2f get_current_tile(sf::CircleShape item) {

    sf::Vector2f position = item.getPosition();


    float x_tile = ceil((position.x + 20.0f - 25.0f) / 44.0f);
    float y_tile = ceil((position.y + 20.0f - 105.0f) / 44.0f);

    

    return sf::Vector2f(x_tile, y_tile);
}


sf::Vector2f get_center_position_of_tile(int x_tile, int y_tile) {

    int x_pos = 25 + 22 + (44 * (x_tile - 1));
    int y_pos = 105 + 22 + (44 * (y_tile - 1));

    return sf::Vector2f(x_pos, y_pos);
}



//only for staarting amount
void add_tail(sf::CircleShape& head) {

    sf::CircleShape new_snake_tail(20.f);
    new_snake_tail.setFillColor(sf::Color::Red);

    sf::Vector2f head_pos = head.getPosition();
    new_snake_tail.setPosition(sf::Vector2f(-20 + 25 + 22 + (44 * ((3 - snake_tail_vec.size()) - 1)), -20 + 105 + 22 + (44 * (8 - 1))));

     



    TailVectorData data;
    data.shape = new_snake_tail;

    data.current_velocity = sf::Vector2f(0.f, 0.f);

    data.turn_position = sf::Vector2f(-1.0f, -1.0f);

    data.next_velocty_at_pos = sf::Vector2f(x_velocity, y_velocity);

    snake_tail_vec.push_back(data);


    apples_eaten += 1;
}

// velocity (you know whether to get this based off the pos), position = sf::Vector2f(-1.f, -1.f)
void move_first_tail(int iteration_int, sf::CircleShape& iteratee_tail, sf::Vector2f new_velocity , sf::Vector2f new_position_of_turn) {
    
    TailVectorData& data = snake_tail_vec[iteration_int];


    /* check if turn */
    sf::Vector2f current_position = iteratee_tail.getPosition();

    // epsilon tolerance
    float epsilon = 1.0f;
    bool amlost_same_position =
        std::abs(data.turn_position.x - (current_position.x)) < epsilon &&
        std::abs(data.turn_position.y - (current_position.y)) < epsilon;

    if (amlost_same_position) { //current_position == data.turn_position
        std::cout << "?\n";
        iteratee_tail.move(sf::Vector2f(data.next_velocty_at_pos.x, data.next_velocty_at_pos.y));
        data.current_velocity = data.next_velocty_at_pos;
        data.turn_position = sf::Vector2f(-1.f, -1.f);
        return;
    }

    //jumpstart
    if ((x_velocity != 0 || y_velocity != 0) && data.current_velocity == sf::Vector2f(0.f, 0.f)) {
        data.current_velocity = sf::Vector2f(0.1f, 0.f);
        
    }

    /* update variables */
    //new_position_of_turn == sf::Vector2f(-1.f, -1.f) or data.turn_position == sf::Vector2f(-1.f, -1.f)
    if (new_position_of_turn == sf::Vector2f(-1.f, -1.f) ) {
        //std::cout << data.current_velocity.x <<  ", " << data.current_velocity.y << "\n";
        iteratee_tail.move(sf::Vector2f(data.current_velocity.x, data.current_velocity.y));
    } else {
        data.turn_position = new_position_of_turn;
        data.next_velocty_at_pos = new_velocity;     
    }


    
    

}


