#include "camera.h"
#include <GLFW/glfw3.h>
#include <cmath>

extern GLFWwindow* g_Window;

static glm::vec4 PlaneFromNormalPoint(glm::vec3 n, glm::vec3 p)
{
    return glm::vec4(n, -glm::dot(n,p));
}

void Camera::Init(float a)
{
    aspect = a;
    Rebuild();
}

void Camera::Update(float dt)
{
    const float speed = 5.0f;
    GLFWwindow* w = g_Window;
    if (glfwGetKey(w, GLFW_KEY_W)) pos += speed*dt*glm::vec3(0,0,-1);
    if (glfwGetKey(w, GLFW_KEY_S)) pos += speed*dt*glm::vec3(0,0,1);
    if (glfwGetKey(w, GLFW_KEY_A)) pos += speed*dt*glm::vec3(-1,0,0);
    if (glfwGetKey(w, GLFW_KEY_D)) pos += speed*dt*glm::vec3(1,0,0);
    Rebuild();
}

void Camera::Rebuild()
{
    glm::mat4 view = glm::lookAt(pos, pos + glm::vec3(0,0,-1), glm::vec3(0,1,0));
    glm::mat4 proj = glm::perspective(glm::radians(60.f), aspect, 0.1f, 500.f);
    gpu.viewProj = proj * view;
    gpu.eye = glm::vec4(pos,1);

    // extract frustum planes in world space (simplistic)
    glm::mat4 vp = gpu.viewProj;
    gpu.frustum[0] = glm::vec4(vp[0][3] + vp[0][0], vp[1][3] + vp[1][0], vp[2][3] + vp[2][0], vp[3][3] + vp[3][0]); // left
    gpu.frustum[1] = glm::vec4(vp[0][3] - vp[0][0], vp[1][3] - vp[1][0], vp[2][3] - vp[2][0], vp[3][3] - vp[3][0]); // right
    gpu.frustum[2] = glm::vec4(vp[0][3] + vp[0][1], vp[1][3] + vp[1][1], vp[2][3] + vp[2][1], vp[3][3] + vp[3][1]); // bottom
    gpu.frustum[3] = glm::vec4(vp[0][3] - vp[0][1], vp[1][3] - vp[1][1], vp[2][3] - vp[2][1], vp[3][3] - vp[3][1]); // top
    gpu.frustum[4] = glm::vec4(vp[0][3] + vp[0][2], vp[1][3] + vp[1][2], vp[2][3] + vp[2][2], vp[3][3] + vp[3][2]); // near
    gpu.frustum[5] = glm::vec4(vp[0][3] - vp[0][2], vp[1][3] - vp[1][2], vp[2][3] - vp[2][2], vp[3][3] - vp[3][2]); // far
    for(int i=0;i<6;++i)
        gpu.frustum[i] /= glm::length(glm::vec3(gpu.frustum[i]));
}
