#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

const std::string FILENAME = "/home/user/CGVC/laboratorio1/bunny.obj";
const float SCALE = 5.0f;
const float YPOS = -0.5f;

std::vector<glm::vec3> readPointsFromFile(const std::string& filename, std::vector<glm::vec3>& points, std::vector<unsigned int>& indexes, unsigned int& sides);

int main()
{
    GLFWwindow *window;
    if (!glfwInit()) {
        std::cerr << "GLFW failed\n";
        return -1;
    }

    window = glfwCreateWindow(600, 600, "Visualizador OBJ Simple", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "GLFW window failed\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW failed\n";
        glfwTerminate();
        return -1;
    }

    glClearColor(0.2, 0.2, 0.2, 0.0);
    std::vector<glm::vec3> points;
    std::vector<unsigned int> indexes;
    unsigned int lados;
    points = readPointsFromFile(FILENAME,points,indexes,lados);

    for (int i = 0; i < points.size(); i++) {
        points[i] *= SCALE;
        points[i].y += YPOS;
    }

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 3 * (points.size()) * sizeof(float), nullptr, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexes.size() * sizeof(unsigned int), &indexes[0], GL_STATIC_DRAW);

    GLuint array;
    glGenVertexArrays(1, &array);
    glBindVertexArray(array);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    const char *vertex_shader =
        "#version 330 core\n"
        "in vec3 vp;"
        "void main(){"
        "gl_Position = vec4(vp, 1.0);"
        "}";

    const char *fragment_shader =
        "#version 330 core\n"
        "out vec4 frag_color;"
        "void main(){"
        "frag_color = vec4(1.0, 1.0, 1.0, 0.0);"
        "}";

    GLuint vs  = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader, nullptr);
    glCompileShader(vs);

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader, nullptr);
    glCompileShader(fs);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, fs);
    glAttachShader(shaderProgram, vs);
    glLinkProgram(shaderProgram);

    glValidateProgram(shaderProgram);
    glUseProgram(shaderProgram);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        glBufferData(GL_ARRAY_BUFFER, 3 * points.size() * sizeof(float), &points[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glEnableVertexAttribArray(0);
        if (lados == 3) {
            glDrawElements(GL_TRIANGLES, indexes.size(), GL_UNSIGNED_INT, 0);
        }
        else{
            glDrawElements(GL_QUADS, indexes.size(), GL_UNSIGNED_INT, 0);
        }
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}

std::vector<glm::vec3> readPointsFromFile(const std::string& filename, std::vector<glm::vec3>& points, std::vector<unsigned int>& indexes, unsigned int& sides)
{
    std::ifstream file(filename);
    std::string line;

    if (!file.is_open()) {
        std::cerr << "Error al abrir el archivo: " << filename << std::endl;
        return points;
    }

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string prefix;
        float x, y, z, a;
        if ((iss >> prefix >> x >> y >> z)) {
            if (prefix == "v") {
                points.push_back(glm::vec3(x, y, z));
            }
            if (prefix == "f") {
                if (iss >> a){
                    indexes.push_back(static_cast<unsigned int>(x)-1);
                    indexes.push_back(static_cast<unsigned int>(y)-1);
                    indexes.push_back(static_cast<unsigned int>(z)-1);
                    indexes.push_back(static_cast<unsigned int>(a)-1);
                    sides = 4;

                }else{
                    indexes.push_back(static_cast<unsigned int>(x)-1);
                    indexes.push_back(static_cast<unsigned int>(y)-1);
                    indexes.push_back(static_cast<unsigned int>(z)-1);
                    sides = 3;
                }
            }
        }
        else{
            std::cerr << "Error al hallar tokens v o f en la línea: " << line << std::endl;
            continue;
        }
    }

    file.close();
    return points;
}
