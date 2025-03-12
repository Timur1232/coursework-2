#include <vector>
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <chrono>

int main()
{
    // Инициализация GLFW и GLEW
    if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(640, 480, "Compute Shader Example", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) return -1;

    // Создание массива данных
    std::vector<float> data(100000, 1.0f); // Массив из 1024 элементов, каждый равен 1.0

  

    // Загрузка и компиляция вычислительного шейдера
    const char* computeShaderSource = R"(
        #version 430
        layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
        layout(std430, binding = 0) buffer Data {
            float data[];
        };
        void main() {
            uint idx = gl_GlobalInvocationID.x;
            data[idx] = data[idx] * 2.0;
        }
    )";

    GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(computeShader, 1, &computeShaderSource, NULL);
    glCompileShader(computeShader);

    // Создание программы и привязка шейдера
    GLuint computeProgram = glCreateProgram();
    glAttachShader(computeProgram, computeShader);
    glLinkProgram(computeProgram);
    glUseProgram(computeProgram);

    GLuint dataBuffer;
    glGenBuffers(1, &dataBuffer);

    auto startTimer = std::chrono::high_resolution_clock::now();

    // Создание буфера
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, dataBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, data.size() * sizeof(float), data.data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, dataBuffer);

    auto timeLoadData = std::chrono::high_resolution_clock::now() - startTimer;
    startTimer = std::chrono::high_resolution_clock::now();

    // Запуск вычислительного шейдера
    for (int i = 0; i < 1; i++)
    {
        glDispatchCompute(data.size(), 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }

    auto timeShader = std::chrono::high_resolution_clock::now() - startTimer;
    startTimer = std::chrono::high_resolution_clock::now();

    // Чтение данных обратно в CPU
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, dataBuffer);
    float* ptr = (float*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
    std::vector<float> resultShader(ptr, ptr + data.size());
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    auto timeReadData = std::chrono::high_resolution_clock::now() - startTimer;


    std::cout << "Shader time:       " << timeShader << std::endl;
    std::cout << "Load time:         " << timeLoadData << std::endl;
    std::cout << "Read time:         " << timeReadData << std::endl;
    std::cout << "Total shader time: " << (timeLoadData + timeShader + timeReadData) << std::endl;

    startTimer = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 1; i++)
    {
        for (float& i : data)
        {
            i *= 2;
        }
    }
    auto timeCPU = std::chrono::high_resolution_clock::now() - startTimer;
    std::cout << "CPU time:          " << timeCPU << std::endl;
    //std::cout << "Time ratio (CPU / Shader): " << (float)timeCPU.count() / timeShader.count() << std::endl;

    // Вывод результата
    /*for (float val : data) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
    for (float val : result) {
        std::cout << val << " ";
    }
    std::cout << std::endl;*/

    // Очистка
    glDeleteProgram(computeProgram);
    glDeleteShader(computeShader);
    glDeleteBuffers(1, &dataBuffer);
    glfwTerminate();
    return 0;
}