#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include "common.h"
#include "shader.h"
#include "program.h"
#include "buffer.h"
#include "vertex_layout.h"
#include "texture.h"

CLASS_PTR(Context)
class Context {
public:
    static ContextUPtr Create();

    void CreateBox();
    void CreateCylinder(int m_cylinderSegments, float m_upperRadius, float m_lowerRadius, float m_cylinderHeight);
    void CreateSphere(int m_sphereHeightSeg, int m_sphereWidthSeg);
    void CreateDonut(int m_donutOutSeg, int m_donutInSeg, float m_donutOutRadius, float m_donutInRadius);

    void Render();    
    void ProcessInput(GLFWwindow* window);
    void Reshape(int width, int height);
    void MouseMove(double x, double y);
    void MouseButton(int button, int action, double x, double y);

private:
    Context() {}
    bool Init();
    ProgramUPtr m_program;

    
    VertexLayoutUPtr m_vertexLayout;
    BufferUPtr m_vertexBuffer;
    BufferUPtr m_indexBuffer;
    TextureUPtr m_texture1;
    TextureUPtr m_texture2;
    TextureUPtr m_texture3;
    int m_indexCount { 36 };

    // cylinder
    int m_cylinderSegments { 16 };
    float m_upperRadius { 0.5f };
    float m_lowerRadius { 0.5f };
    float m_cylinderHeight { 1.0f };

    // sphere
    int m_sphereHeightSeg { 16 };
    int m_sphereWidthSeg { 32 };

    // donut
    int m_donutOutSeg { 32 };
    int m_donutInSeg { 16 };
    float m_donutOutRadius { 1.0f };
    float m_donutInRadius { 0.25f };

    //  vertices, triangle number
    int m_numVertices { 24 };
    int m_numTriangles { 12 };
    
    // rotation , scale
    glm::vec3 m_rotation { glm::vec3(0.0f, 0.0f, 0.0f)};
    glm::vec3 m_scale { glm::vec3(1.0f, 1.0f, 1.0f) };   
    glm::vec3 m_rotSpeed { glm::vec3(0.0f, 0.0f, 0.0f) };   

    // clear color
    glm::vec4 m_clearColor { glm::vec4(0.4f, 0.4f, 0.2f, 0.2f) };

    // camera parameter
    bool m_cameraControl { false };
    glm::vec2 m_prevMousePos { glm::vec2(0.0f) };
    float m_cameraPitch { 0.0f };
    float m_cameraYaw { 0.0f };  
    glm::vec3 m_cameraPos { glm::vec3(0.0f, 0.0f, 3.0f) };
    glm::vec3 m_cameraFront { glm::vec3(0.0f, 0.0f, -1.0f) };
    glm::vec3 m_cameraUp { glm::vec3(0.0f, 1.0f, 0.0f) };

    int m_width { WINDOW_WIDTH };
    int m_height { WINDOW_HEIGHT };
};

#endif // __CONTEXT_H__