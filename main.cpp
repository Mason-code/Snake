// By Mason
// Using Snake Case as the naming convention, C++ as the language, SFML as the graphical library, and Visual Studio as the IDE


/* 
multi que movement strat. 

---no : do movement based off of tiles
*/


#include <iostream>
#include <vector>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <random>
#include <math.h>
#include <cmath>
#include <deque>



//global
//Time
sf::Clock up_time;
sf::Clock down_time;
sf::Clock left_time;
sf::Clock right_time;


// quick vector
std::deque<sf::Vector2f> need_turn;


/*apple*/
sf::Texture apple_texture;

/*snake*/
float x_velocity = 0;
float y_velocity = 0;


/*tail*/
struct TailVectorData {
    sf::CircleShape shape;
    sf::Vector2f current_velocity;
    std::vector<sf::Vector2f> turn_position;
    std::vector<sf::Vector2f> next_velocty_at_pos;
    bool active = false; // ready to move: 44 away
};

std::vector<TailVectorData> snake_tail_vec = {};


float uni_speed = 0.8f; //0.8
int apples_eaten = 0;


sf::Vector2f last_direction_change_location = sf::Vector2f(-1.f, -1.f);

//  function declarations
sf::Sprite rand_apple(sf::Sprite& the_apple_sprite);
std::vector<sf::Vector2f> move_snake(sf::CircleShape& the_snake_circle);
sf::Vector2f get_current_tile(sf::Transformable& item);
sf::Vector2f get_center_position_of_tile(int x_tile, int y_tile);
void add_tail(sf::CircleShape& head);
void move_first_tail(int iteration_int, sf::CircleShape& iteratee_tail, sf::Vector2f new_velocity, sf::Vector2f new_position_of_turn, sf::CircleShape& snake_head);
void add_additional_tail(sf::Vector2f final_tail_pos, int last_index);
void move_additional_tail(int iteration_int, sf::CircleShape& tail_to_follow, sf::CircleShape& current_tail);
float distance_between_two_pos(sf::Vector2f pos_one, sf::Vector2f pos_two);


int main()
{

    // create the window
    sf::RenderWindow window(sf::VideoMode({ 800, 800 }), "Snake");




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
            float ing_donuts = uni_speed;

            /* 
            
            If you have a regular object, you use the dot '.' operator:
            sf::Event event;
            event.type; // access the 'type' member of the event object
            
            If you have a pointer to an object, you use the arrow '->' operator:
            sf::Event* eventPtr = &event;
            eventPtr->type; // access the 'type' member through the pointer
            
            */

            sf::Time elapsed_up = up_time.getElapsedTime();
            sf::Time elapsed_right = right_time.getElapsedTime();
            sf::Time elapsed_left = left_time.getElapsedTime(); // && elapsed_up.asSeconds() > (ing_donuts * .05
            sf::Time elapsed_down = down_time.getElapsedTime();


            if (const auto* key = event->getIf<sf::Event::KeyPressed>()) { //Check if the current event is a KeyPressed event. | If it is, get a pointer to the KeyPressed data and store it in 'key'.
                
                if (key->code == sf::Keyboard::Key::W && y_velocity != ing_donuts) {
                    need_turn.push_back(sf::Vector2f(0.f,-ing_donuts));
                    up_time.restart();

                }
                if (key->code == sf::Keyboard::Key::A && x_velocity != ing_donuts) {
                    need_turn.push_back(sf::Vector2f(-ing_donuts, 0.f));
                    left_time.restart();

                }
                if (key->code == sf::Keyboard::Key::S && y_velocity != -ing_donuts) {
                    need_turn.push_back(sf::Vector2f(0.f, ing_donuts));
                    down_time.restart();

                }
                if (key->code == sf::Keyboard::Key::D && x_velocity != -ing_donuts) {
                    need_turn.push_back(sf::Vector2f(ing_donuts, 0.f));
                    right_time.restart();
                }
                
                
            }


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


        

        tail_values = move_snake(snake_head);
        window.draw(snake_head);



        int iteration_value = 0;

        for (auto& tail : snake_tail_vec) {
            
            if (iteration_value == 0) {
                move_first_tail(iteration_value, tail.shape, tail_values[0], tail_values[1], snake_head);
                window.draw(tail.shape);
                iteration_value++;
                continue;
            }
            else {
                TailVectorData& prev_data = snake_tail_vec[iteration_value - 1];
                move_additional_tail(iteration_value, prev_data.shape , tail.shape); // parameters -> the previous tail 
                window.draw(tail.shape);
                iteration_value++;
            }   
        }
        
      
        
        // do I move last tail?
        TailVectorData& ultima = snake_tail_vec.back();
        if (!ultima.active && snake_tail_vec.size() > 1) {
            TailVectorData& penultima = snake_tail_vec[snake_tail_vec.size() - 2];
            TailVectorData& ultima = snake_tail_vec.back();

            sf::Vector2f penultima_pos = penultima.shape.getPosition();
            sf::Vector2f ultima_pos = ultima.shape.getPosition();
            float dx = penultima_pos.x - ultima_pos.x;
            float dy = penultima_pos.y - ultima_pos.y;
            float distance = std::sqrt(dx * dx + dy * dy); // math oooooo
            if (distance >= 44) {
                ultima.active = true;
            }
        }
        


        // Collision detection
        sf::Vector2f apple_location = get_current_tile(apple_sprite);
        sf::Vector2f snake_location = get_current_tile(snake_head);

        if (apple_location == snake_location) {
            rand_apple(apple_sprite); // implement a procedure to make sure this doesn't land on tail!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            apples_eaten++;

            TailVectorData& data = snake_tail_vec.back();
            add_additional_tail(data.shape.getPosition(), snake_tail_vec.size());
        }

        window.draw(apple_sprite);


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
    /*direction: TSA-Screening*/
    if (!need_turn.empty() && need_turn[0] == sf::Vector2f(x_velocity, y_velocity)) {
        need_turn.pop_front();
    }

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
    sf::Vector2f where_tail_turn = sf::Vector2f(-1.f, -1.f);
    if (at_center_turn_point && !need_turn.empty()) {
        if (!(x_velocity == -need_turn[0].x) && !(y_velocity == -need_turn[0].y) || (x_velocity == 0.f && y_velocity == 0.f)) {
            x_velocity = need_turn[0].x;
            y_velocity = need_turn[0].y;
            where_tail_turn = the_snake_circle.getPosition();
        }
        need_turn.pop_front();
    }
    
    
     the_snake_circle.move(sf::Vector2f(x_velocity, y_velocity));
    
     // velocity (you know whether to get this based off the pos), position = sf::Vector2f(-1.f, -1.f)
     std::vector<sf::Vector2f> bruh = { sf::Vector2f(x_velocity, y_velocity), where_tail_turn };

     return bruh;
    }
    


sf::Vector2f get_current_tile(sf::Transformable& item) { // I can pass any object derived from sf::Transformable (like sf::Sprite, sf::CircleShape, sf::RectangleShape, etc.)

    sf::Vector2f position = item.getPosition();


    float x_tile = ceil((position.x + 20.0f - 25.0f) / 44.0f);
    float y_tile = ceil((position.y + 20.0f - 105.0f) / 44.0f);

    

    return sf::Vector2f(x_tile, y_tile);
}


sf::Vector2f get_center_position_of_tile(int x_tile, int y_tile) { // I think this is incorrect

    int x_pos = 25 + 22 + (44 * (x_tile - 1));
    int y_pos = 105 + 22 + (44 * (y_tile - 1));

    return sf::Vector2f(x_pos, y_pos);
}



//only for staarting amount
void add_tail(sf::CircleShape& head) {

    sf::CircleShape new_snake_tail(20.f);
    new_snake_tail.setFillColor(sf::Color::Red);

    sf::Vector2f head_pos = head.getPosition();
    new_snake_tail.setPosition(sf::Vector2f(head_pos.x-44, head_pos.y));

     



    TailVectorData data;
    data.shape = new_snake_tail;

    data.current_velocity = sf::Vector2f(0.f, 0.f);

    data.turn_position = {};

    data.next_velocty_at_pos = {};
    
    data.active = true;

    snake_tail_vec.push_back(data);


}

// velocity (you know whether to get this based off the pos), position = sf::Vector2f(-1.f, -1.f)
void move_first_tail(int iteration_int, sf::CircleShape& iteratee_tail, sf::Vector2f new_velocity, sf::Vector2f new_position_of_turn, sf::CircleShape& snake_head) {
    float speed_value = 1 / uni_speed;

    TailVectorData& data = snake_tail_vec[iteration_int];

    //jumpstart
    if ((x_velocity != 0 || y_velocity != 0) && data.current_velocity == sf::Vector2f(0.f, 0.f)) {
        data.current_velocity = sf::Vector2f(uni_speed, 0.f);
        //std::cout << "check3\n";
        data.next_velocty_at_pos = {};
        data.turn_position = {};
        
    }


    if (new_position_of_turn != sf::Vector2f(-1.f, -1.f)) {

        
        for (int i = 0; i < snake_tail_vec.size(); i++) {
            TailVectorData& shared_data = snake_tail_vec[i];
            if (std::find(shared_data.turn_position.begin(), shared_data.turn_position.end(), new_position_of_turn) == shared_data.turn_position.end()) { // if (!shared_data.turn_position.contains(new_position_of_turn)) {
                shared_data.turn_position.push_back(new_position_of_turn);
                shared_data.next_velocty_at_pos.push_back(new_velocity);
            }
            
        }
      

    } else {
        // nothing new; just keep swimming
        iteratee_tail.move(sf::Vector2f(data.current_velocity.x, data.current_velocity.y)); 
    }

    /* check if able to turn */
    sf::Vector2f tail_current_position = iteratee_tail.getPosition();
    // checks if tail is past turn point
    // epsilon tolerance
    float epsilon = 2.0f; // determines how close tail gets to head before direction changes
    bool past_turn_point = // this all depends on direction so it gets messy, thats why i just did absolute v
        (!data.turn_position.empty() && std::abs(data.turn_position[0].x - tail_current_position.x) <= epsilon) &&
        (!data.turn_position.empty() && std::abs(data.turn_position[0].y - tail_current_position.y) <= epsilon);



    


    
    if (past_turn_point) { 
        
        data.current_velocity = data.next_velocty_at_pos[0];
        data.next_velocty_at_pos.erase(data.next_velocty_at_pos.begin());
        data.turn_position.erase(data.turn_position.begin());
        
    }


    sf::Vector2f tile = get_current_tile(iteratee_tail);
    sf::Vector2f intended_pos = snake_head.getPosition();//get_center_position_of_tile(tile.x, tile.y);

    sf::Vector2f current_position = iteratee_tail.getPosition(); // this returns top-left pos and not center pos || radius of circle is 20 -> gets us real center(but its not needed because all are offset by this)
       

    /*&& data.next_velocty_at_pos.empty() will work for a small snake but once large enough it will look strange and break
    so we need to do && distance between current position and turn position > 88 */
    /*float distance = 89;
    if (!data.turn_position.empty()) {
        float distance = distance_between_two_pos(current_position, data.turn_position[0]);
    }
    else {
        float distance = 89;
    }*/
    
    if (x_velocity == data.current_velocity.x && x_velocity != 0 && data.next_velocty_at_pos.empty()) { // if going in straight line along x-axis
        //ensures in line with head
        float delta_y = intended_pos.y - current_position.y;
        iteratee_tail.move(sf::Vector2f(0, delta_y));  
        //std::cout << "Adjusted Y by: " << delta_y << "\n";
        //ensures distance from head (must be 44 away in opposite direction of movement)
        float delta_x = intended_pos.x+(44*speed_value*-data.current_velocity.x) - current_position.x;
        iteratee_tail.move(sf::Vector2f(delta_x, 0));

    }
    tile = get_current_tile(iteratee_tail);
    intended_pos = snake_head.getPosition();//get_center_position_of_tile(tile.x, tile.y);
    current_position = iteratee_tail.getPosition();
    if (y_velocity == data.current_velocity.y && y_velocity != 0 && data.next_velocty_at_pos.empty()) { // if going in straight line along x-axis
        //ensures in line with head
        float delta_x = intended_pos.x - current_position.x;
        iteratee_tail.move(sf::Vector2f(delta_x, 0));  
        //std::cout << "Adjusted X by: " << delta_x << "\n";
        //ensures distance from head (must be 44 away in opposite direction of movement)| the direction of where the 44 is offset depends ton the velocity(to multiply the velocity and only get the pos/neg direction I multiple it by speed_value to make it 1 and keep the 44)
        float delta_y = intended_pos.y + (44 * speed_value * -data.current_velocity.y) - current_position.y;
        iteratee_tail.move(sf::Vector2f(0, delta_y));

    }


    


    

}


void add_additional_tail(sf::Vector2f final_tail_pos, int last_index) {
    
    TailVectorData data_to_follow = snake_tail_vec[last_index-1];
    
    data_to_follow.shape.setPosition(final_tail_pos);

    data_to_follow.active = false;

    snake_tail_vec.push_back(data_to_follow);

}



void move_additional_tail(int iteration_int, sf::CircleShape& tail_to_follow, sf::CircleShape& current_tail) {

    float speed_value = 1 / uni_speed;

    TailVectorData& data = snake_tail_vec[iteration_int];


    if (!data.active) {
        return;
    }

    //jumpstart
    if ((x_velocity != 0 || y_velocity != 0) && data.current_velocity == sf::Vector2f(0.f, 0.f)) {
        data.current_velocity = sf::Vector2f(uni_speed, 0.f);
        data.next_velocty_at_pos = {};
        data.turn_position = {};

    }    


    
    // Dory
    current_tail.move(sf::Vector2f(data.current_velocity.x, data.current_velocity.y));
    //std::cout << "moved: " << data.current_velocity.x << ", " << data.current_velocity.y << "\n";
    
    

    /* check if able to turn */
    sf::Vector2f tail_current_position = current_tail.getPosition();
    // checks if tail is past turn point
    // epsilon tolerance
    float epsilon = 2.0f; // determines how close tail gets to head before direction changes
 
    bool past_turn_point = // this all depends on direction so it gets messy, thats why i just did absolute v
        (!data.turn_position.empty() && std::abs(data.turn_position[0].x - tail_current_position.x) <= epsilon) &&
        (!data.turn_position.empty() && std::abs(data.turn_position[0].y - tail_current_position.y) <= epsilon);



    if (past_turn_point) {
        
        data.current_velocity = data.next_velocty_at_pos[0];
        data.next_velocty_at_pos.erase(data.next_velocty_at_pos.begin());
        data.turn_position.erase(data.turn_position.begin());

    }



    

    
    sf::Vector2f tile = get_current_tile(current_tail);
    sf::Vector2f intended_pos = tail_to_follow.getPosition();//get_center_position_of_tile(tile.x, tile.y);

    sf::Vector2f current_position = current_tail.getPosition(); // this returns top-left pos and not center pos || radius of circle is 20 -> gets us real center(but its not needed because all are offset by this)
    float distance = -1;
    if (!data.turn_position.empty()) {
        distance = distance_between_two_pos(current_position, data.turn_position[0]);
    }
    else {
        distance = 89;
    }

    /*&& data.next_velocty_at_pos.empty() will work for a small snake but once large enough it will look strange and break */
    /*if (!data.next_velocty_at_pos.empty()) {
        std::cout << "distance until turn: " << distance << "\n";
    }*/

    if (x_velocity == data.current_velocity.x && x_velocity != 0 && (data.next_velocty_at_pos.empty())) { // if going in straight line along x-axis
        //ensures in line with head
        float delta_y = intended_pos.y - current_position.y;
        current_tail.move(sf::Vector2f(0, delta_y));
        //std::cout << "Adjusted Y by: " << delta_y << "\n";
        //ensures distance from head (must be 44 away in opposite direction of movement)
        float delta_x = intended_pos.x + (44 * speed_value * -data.current_velocity.x) - current_position.x;
        current_tail.move(sf::Vector2f(delta_x, 0));

    }
    tile = get_current_tile(current_tail);
    intended_pos = tail_to_follow.getPosition();//get_center_position_of_tile(tile.x, tile.y);
    current_position = current_tail.getPosition();
    if (y_velocity == data.current_velocity.y && y_velocity != 0 && (data.next_velocty_at_pos.empty())) { // if going in straight line along x-axis
        //ensures in line with head
        float delta_x = intended_pos.x - current_position.x;
        current_tail.move(sf::Vector2f(delta_x, 0));
        //std::cout << "Adjusted X by: " << delta_x << "\n";
        //ensures distance from head (must be 44 away in opposite direction of movement)| the direction of where the 44 is offset depends ton the velocity(to multiply the velocity and only get the pos/neg direction I multiple it by speed_value to make it 1 and keep the 44)
        float delta_y = intended_pos.y + (44 * speed_value * -data.current_velocity.y) - current_position.y;
        current_tail.move(sf::Vector2f(0, delta_y));

    }
    

}


float distance_between_two_pos(sf::Vector2f pos_one, sf::Vector2f pos_two) {
    // there is a formula for this
    float dx = pos_two.x - pos_one.x;
    float dy = pos_two.y - pos_one.y;
    float distance = std::sqrt(dx * dx + dy * dy); 
    return distance;
}