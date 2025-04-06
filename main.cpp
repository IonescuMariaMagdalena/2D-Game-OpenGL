#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>

// Include GLEW
#include "dependente/glew/glew.h"

// Include GLFW
#include "dependente/glfw/glfw3.h"

// Include GLM
#include "dependente/glm/glm.hpp"
#include "dependente/glm/gtc/matrix_transform.hpp"
#include "dependente/glm/gtc/type_ptr.hpp"

#include "shader.hpp"

// Variables
GLFWwindow* window;
const int width = 1024, height = 1024;
glm::mat4 trans(1.0f);
int player_change = 0;
bool keystate = 0;
float zombieSpawnTimer = 0.0f;      // Timer for zombie spawning
float zombieSpawnInterval = 3.0f;   // Increased interval for slower spawning (3 seconds)

// Define structures for objects,zombies and bullets
struct object {
    glm::vec3 pozitie;
    glm::vec3 culoare;
    int tag;
    object(glm::vec3 pozitie, glm::vec3 culoare, int tag) {
        this->pozitie = pozitie;
        this->culoare = culoare;
        this->tag = tag;
    }
};

struct Bullet {
    glm::vec3 pozitie;
    glm::vec3 direction;
    bool active;

    Bullet(glm::vec3 pozitie, glm::vec3 direction) {
        this->pozitie = pozitie;
        this->direction = direction;
        this->active = true;
    }
};

struct Zombie {
    glm::vec3 pozitie;
    glm::vec3 culoare;
    bool active;

    Zombie(glm::vec3 pozitie) {
        this->pozitie = pozitie;
        this->culoare = glm::vec3(0.0f, 1.0f, 0.0f); // Green
        this->active = true;
    }
};

// Functions
bool CheckCollision(const glm::vec3& one, const glm::vec3& two) {
    bool collisionX = one.x + 0.1f >= two.x && two.x + 0.1f >= one.x;
    bool collisionY = one.y + 0.1f >= two.y && two.y + 0.1f >= one.y;
    return collisionX && collisionY;
}

// Check if Barbie or Ken collides with a zombie
bool CheckCollisionWithZombies(object& character, const std::vector<Zombie>& zombies) {
    for (const auto& zombie : zombies) {
        if (zombie.active && CheckCollision(character.pozitie, zombie.pozitie)) {
            return true;  // Collision detected
        }
    }
    return false;  // No collision
}

void window_callback(GLFWwindow* window, int new_width, int new_height) {
    glViewport(0, 0, new_width, new_height);
}

// Main
int main(void) {
    srand(time(NULL)); // Seed for random number generation

    // Initialize GLFW
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    // Open a window and create its OpenGL context
    window = glfwCreateWindow(width, height, "Lab 4", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to open GLFW window.");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        glfwTerminate();
        return -1;
    }

    // Specify the size of the rendering window
    glViewport(0, 0, width, height);

    // Background
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders("SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader");

    GLfloat vertices[] = {
        0.05f,  0.05f, 0.0f,  // top right
        0.05f, -0.05f, 0.0f,  // bottom right
        -0.05f, -0.05f, 0.0f,  // bottom left
        -0.05f,  0.05f, 0.0f   // top left 
    };

    GLuint indices[] = {  // note that we start from 0!
        0, 3, 1,  // first Triangle
        1, 3, 2,   // second Triangle
    };

    GLfloat vertices_bullet[] = {
    0.0f,  0.577f/20, 0.0f,   // Vertex 1 (top)
   -0.5f/20, -0.289f/20, 0.0f,   // Vertex 2 (bottom left)
    0.5f/20, -0.289f/20, 0.0f    // Vertex 3 (bottom right)
    };

    GLuint indices_bullet[] = {
        0, 1, 2   // The single triangle
    };

    // A Vertex Array Object (VAO) is an object which contains one or more Vertex Buffer Objects
    GLuint vbo, vao, ibo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ibo);

    // Bind VAO
    glBindVertexArray(vao);

    // Bind VBO
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Bind IBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Set attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    GLuint vbo_bullet, vao_bullet, ibo_bullet;
    glGenVertexArrays(1, &vao_bullet);
    glGenBuffers(1, &vbo_bullet);
    glGenBuffers(1, &ibo_bullet);

    // Bind VAO
    glBindVertexArray(vao_bullet);

    // Bind VBO
    glBindBuffer(GL_ARRAY_BUFFER, vbo_bullet);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_bullet), vertices_bullet, GL_STATIC_DRAW);

    // Bind IBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_bullet);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_bullet), indices_bullet, GL_STATIC_DRAW);

    // Set attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glfwSetFramebufferSizeCallback(window, window_callback);

    std::vector<glm::vec3> positions = {
        glm::vec3(-0.4f, -0.22f, 0), //Barbie
        glm::vec3(-0.6f, -0.2f, 0), //Ken
        glm::vec3(-0.1f , -0.5f, 0.),//red object
        glm::vec3(0.3f, -0.5f, 0)//white object
    };

    glm::vec3 color_chr[] = {
        glm::vec3(1.0f, 0.75f, 0.8f),
        glm::vec3(0.0f, 1.0f, 1.0f),
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f)
    };

    std::vector<object> obiect = {
        object(positions[0], color_chr[0], 0), // Barbie
        object(positions[1], color_chr[1], 0), // Ken
        object(positions[2], color_chr[2], 1), // item 1
        object(positions[3], color_chr[3], 2)  // item 2
    };

    // Initialize bullets and zombies
    std::vector<Bullet> bullets;
    std::vector<Zombie> zombies;

    // Function to spawn a random zombie
    auto spawnZombie = [&]() {
        float x = (rand() % 20 - 10) / 10.0f;
        float y = 1.3 + (rand() % 20 - 10) / 10.0f;
        zombies.push_back(Zombie(glm::vec3(x, y, 0.0f)));
        };

    // Randomly spawn some zombies at the start
    for (int i = 0; i < 5; ++i) {
        spawnZombie();
    }

    bool gameOver = false;  // Game over flag

    while (!glfwWindowShouldClose(window) && glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }

        // Check for events
        glfwPollEvents();

        // Update the zombie spawn timer
        zombieSpawnTimer += 0.01f;  // Increment the timer each frame

        // Check if the timer exceeds the spawn interval
        if (zombieSpawnTimer >= zombieSpawnInterval) {
            zombieSpawnTimer = 0.0f; // Reset the timer
            spawnZombie();           // Spawn a new zombie
        }

        // Check for collisions between characters (Barbie or Ken) and zombies
        if (CheckCollisionWithZombies(obiect[0], zombies) || CheckCollisionWithZombies(obiect[1], zombies)) {
            gameOver = true;  // Game over if a collision happens
            std::cout << "Game Over!" << std::endl;
            glfwSetWindowShouldClose(window, true);  // Close the window immediately
        }

        if (gameOver) {
            break;
        }

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT);

        // Use shader program
        glUseProgram(programID);

        // Bind VAO
        glBindVertexArray(vao);



        // Movement speed
        float speed = 0.001;

        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            speed = 0.002;

        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
            speed = 0.002;
        }

        // Character switching
        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && keystate == 1) {
            keystate = 0;
            player_change = (player_change == 0) ? 1 : 0;
        }

        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE) {
            keystate = 1;
        }

        // Character movement
        if (player_change == 0) { // Barbie controls
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
                obiect[0].pozitie.y += speed;
                if (CheckCollision(obiect[0].pozitie, obiect[1].pozitie)) {
                    obiect[0].pozitie.y -= speed;
                }
            }

            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
                obiect[0].pozitie.y -= speed;
                if (CheckCollision(obiect[0].pozitie, obiect[1].pozitie)) {
                    obiect[0].pozitie.y += speed;
                }
            }

            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
                obiect[0].pozitie.x += speed;
                if (CheckCollision(obiect[0].pozitie, obiect[1].pozitie)) {
                    obiect[0].pozitie.x -= speed;
                }
            }

            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
                obiect[0].pozitie.x -= speed;
                if (CheckCollision(obiect[0].pozitie, obiect[1].pozitie)) {
                    obiect[0].pozitie.x += speed;
                }
            }
        }


        if (player_change == 1) { // Ken controls
            if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
                obiect[1].pozitie.y += speed;
                if (CheckCollision(obiect[0].pozitie, obiect[1].pozitie)) {
                    obiect[1].pozitie.y -= speed;
                }
            }

            if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
                obiect[1].pozitie.y -= speed;
                if (CheckCollision(obiect[0].pozitie, obiect[1].pozitie)) {
                    obiect[1].pozitie.y += speed;
                }
            }

            if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
                obiect[1].pozitie.x += speed;
                if (CheckCollision(obiect[0].pozitie, obiect[1].pozitie)) {
                    obiect[1].pozitie.x -= speed;
                }
            }

            if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
                obiect[1].pozitie.x -= speed;
                if (CheckCollision(obiect[0].pozitie, obiect[1].pozitie)) {
                    obiect[1].pozitie.x += speed;
                }
            }
        }

        // Check if the player shoots (C for Barbie, N for Ken)
        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && player_change == 0) {
            glm::vec3 bulletDirection(0.0f, 0.05f, 0.0f); // Bullet direction upwards
            bullets.push_back(Bullet(obiect[0].pozitie, bulletDirection));
        }

        if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS && player_change == 1) {
            glm::vec3 bulletDirection(0.0f, 0.05f, 0.0f); // Bullet direction upwards
            bullets.push_back(Bullet(obiect[1].pozitie, bulletDirection));
        }

        // Update bullet positions and check for collisions with zombies
        for (int i = 0; i < bullets.size(); ++i) {
            if (bullets[i].active) {
                bullets[i].pozitie += bullets[i].direction; // Move bullet upwards

                // Check for collision with zombies
                for (int j = 0; j < zombies.size(); ++j) {
                    if (zombies[j].active && CheckCollision(bullets[i].pozitie, zombies[j].pozitie)) {
                        zombies[j].active = false; // Deactivate zombie
                        bullets[i].active = false; // Deactivate bullet
                        break; // Bullet only hits one zombie
                    }
                }
            }
        }

        // Remove inactive bullets and zombies
        bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](Bullet& b) { return !b.active; }), bullets.end());
        zombies.erase(std::remove_if(zombies.begin(), zombies.end(), [](Zombie& z) { return !z.active; }), zombies.end());

        // Render objects: Barbie, Ken, and the zombies
        for (int i = 0; i < obiect.size(); i++) {
            trans = glm::mat4(1.0f);
            trans = glm::translate(trans, obiect[i].pozitie);

            unsigned int transformLoc = glGetUniformLocation(programID, "transform");
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

            unsigned int transformLoc2 = glGetUniformLocation(programID, "color");
            glm::vec4 color = glm::vec4(obiect[i].culoare, 1.0f);
            glUniform4fv(transformLoc2, 1, glm::value_ptr(color));

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }

        // Use shader program
        glUseProgram(programID);

        // Bind VAO
        glBindVertexArray(vao_bullet);


        // Render bullets
        for (int i = 0; i < bullets.size(); ++i) {
            if (bullets[i].active) {
                trans = glm::mat4(1.0f);
                trans = glm::translate(trans, bullets[i].pozitie);

                unsigned int transformLoc = glGetUniformLocation(programID, "transform");
                glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

                unsigned int transformLoc2 = glGetUniformLocation(programID, "color");
                glm::vec4 color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f); // Red bullets
                glUniform4fv(transformLoc2, 1, glm::value_ptr(color));

                glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
            }
        }
        glBindVertexArray(vao);

        // Render zombies
        for (int i = 0; i < zombies.size(); ++i) {
            if (zombies[i].active) {
                trans = glm::mat4(1.0f);
                trans = glm::translate(trans, zombies[i].pozitie);

                unsigned int transformLoc = glGetUniformLocation(programID, "transform");
                glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

                unsigned int transformLoc2 = glGetUniformLocation(programID, "color");
                glm::vec4 color = glm::vec4(zombies[i].culoare, 1.0f); // Green zombies
                glUniform4fv(transformLoc2, 1, glm::value_ptr(color));

                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            }
        }

        for (int i = 2; i < obiect.size(); i++) {

            if (obiect[i].tag == 1) {
                if (CheckCollision(obiect[0].pozitie, obiect[i].pozitie))
                {
                    obiect.erase(obiect.begin() + i);
                }
            }
            if (obiect[i].tag == 2) {
                if (CheckCollision(obiect[1].pozitie, obiect[i].pozitie))
                {
                    obiect.erase(obiect.begin() + i);
                }
            }

        }

        // Swap buffers
        glfwSwapBuffers(window);
    }

    // Cleanup
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(programID);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}