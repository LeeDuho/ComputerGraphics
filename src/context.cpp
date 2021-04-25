#include "context.h"
#include "image.h"
#include <imgui.h>

ContextUPtr Context::Create() {
    auto context = ContextUPtr(new Context());
    if (!context->Init())
        return nullptr;
    return std::move(context);
}

void Context::ProcessInput(GLFWwindow* window) {
    if (!m_cameraControl)
        return;
    
    const float cameraSpeed = 0.05f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        m_cameraPos += cameraSpeed * m_cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        m_cameraPos -= cameraSpeed * m_cameraFront;

    auto cameraRight = glm::normalize(glm::cross(m_cameraUp, -m_cameraFront));
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        m_cameraPos += cameraSpeed * cameraRight;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        m_cameraPos -= cameraSpeed * cameraRight;    

    auto cameraUp = glm::normalize(glm::cross(-m_cameraFront, cameraRight));
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        m_cameraPos += cameraSpeed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        m_cameraPos -= cameraSpeed * cameraUp;
}

void Context::Reshape(int width, int height) {
    m_width = width;
    m_height = height;
    glViewport(0, 0, m_width, m_height);
}

void Context::MouseMove(double x, double y) {
    if (!m_cameraControl)
        return;
    auto pos = glm::vec2((float)x, (float)y);
    auto deltaPos = pos - m_prevMousePos;

    const float cameraRotSpeed = 0.8f;
    m_cameraYaw -= deltaPos.x * cameraRotSpeed;
    m_cameraPitch -= deltaPos.y * cameraRotSpeed;

    if (m_cameraYaw < 0.0f)   m_cameraYaw += 360.0f;
    if (m_cameraYaw > 360.0f) m_cameraYaw -= 360.0f;

    if (m_cameraPitch > 89.0f)  m_cameraPitch = 89.0f;
    if (m_cameraPitch < -89.0f) m_cameraPitch = -89.0f;

    m_prevMousePos = pos;    
}

void Context::MouseButton(int button, int action, double x, double y) {
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            // 마우스 조작 시작 시점에 현재 마우스 커서 위치 저장
            m_prevMousePos = glm::vec2((float)x, (float)y);
            m_cameraControl = true;
        }
        else if (action == GLFW_RELEASE) {
            m_cameraControl = false;
        }
    }
}

bool Context::Init() {

   float vertices[] = {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
        0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f,

        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,
        0.5f, -0.5f,  0.5f, 1.0f, 0.0f,
        0.5f,  0.5f,  0.5f, 1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f, 0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,

        0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
        0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f,  0.5f, 0.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f,  0.5f, 1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,

        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f,
        0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
        0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, 0.0f, 0.0f,
    };

    uint32_t indices[] = {
        0,  2,  1,  2,  0,  3,
        4,  5,  6,  6,  7,  4,
        8,  9, 10, 10, 11,  8,
        12, 14, 13, 14, 12, 15,
        16, 17, 18, 18, 19, 16,
        20, 22, 21, 22, 20, 23,
    };

    m_vertexLayout = VertexLayout::Create();
    m_vertexBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER,
        GL_STATIC_DRAW, vertices, sizeof(float) * 120);
    	
    //m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0); 
    m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
    m_vertexLayout->SetAttrib(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, sizeof(float) * 3);

    m_indexBuffer = Buffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER,
        GL_STATIC_DRAW, indices, sizeof(uint32_t) *36);
	
    ShaderPtr vertShader = Shader::CreateFromFile("./shader/texture.vs", GL_VERTEX_SHADER);
    ShaderPtr fragShader = Shader::CreateFromFile("./shader/texture.fs", GL_FRAGMENT_SHADER);
    if (!vertShader || !fragShader)
        return false;
    SPDLOG_INFO("vertex shader id: {}", vertShader->Get());
    SPDLOG_INFO("fragment shader id: {}", fragShader->Get());

    
    m_program = Program::Create({fragShader, vertShader});
    if (!m_program)
        return false;
    SPDLOG_INFO("program id: {}", m_program->Get());

    glClearColor(0.4f, 0.4f, 0.2f, 0.2f);

    auto image1 = Image::Load("./image/wood.jpg");
    if (!image1) 
        return false;
    m_texture1 = Texture::CreateFromImage(image1.get());

    auto image2 = Image::Load("./image/metal.jpg");
    m_texture2 = Texture::CreateFromImage(image2.get());
    
    auto image3 = Image::Load("./image/earth.jpg");
    m_texture3 = Texture::CreateFromImage(image3.get());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture1->Get());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_texture2->Get());
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_texture3->Get());

    m_program->Use();
    m_program->SetUniform("tex", 0);

    return true;

}

void Context::CreateBox() {
}

void Context::CreateCylinder(int m_cylinderSegments, float m_upperRadius, float m_lowerRadius, float m_cylinderHeight) {
    
    const float pi = 3.141592f;
    
    std::vector<float> vertices;
    std::vector<uint32_t> indices;
    
    //윗면 생성
    vertices.push_back(0.0f);
    vertices.push_back(m_cylinderHeight/2.0f);
    vertices.push_back(0.0f);
    for (int i = 0; i <= m_cylinderSegments; i++) {
        float angle = (360.0f / m_cylinderSegments * i) * pi /180.0f;
        float x = cosf(angle) * m_upperRadius;
        float z = sinf(angle) * m_upperRadius;
        vertices.push_back(x);
        vertices.push_back(m_cylinderHeight/2.0f);
        vertices.push_back(z);
    }
    for (int i = 0; i < m_cylinderSegments; i++) {
        indices.push_back(0);
        indices.push_back(i + 1);
        indices.push_back(i + 2);
    }

    //아랫면 생성
    vertices.push_back(0.0f);
    vertices.push_back(m_cylinderHeight/(-2.0f));
    vertices.push_back(0.0f);
    for (int i = 0; i <= m_cylinderSegments; i++) {
        float angle = (360.0f / m_cylinderSegments * i) * pi /180.0f;
        float x = cosf(angle) * m_lowerRadius;
        float z = sinf(angle) * m_lowerRadius;
        vertices.push_back(x);
        vertices.push_back(m_cylinderHeight/(-2.0f));
        vertices.push_back(z);
    }
    for (int i = m_cylinderSegments + 3; i < 2 * m_cylinderSegments + 3; i++) {
        indices.push_back(m_cylinderSegments + 3);
        indices.push_back(i + 1);
        indices.push_back(i + 2);
    }

    //옆면 생성
    for (int i = 0; i <= m_cylinderSegments; i++) {
        float angle = (360.0f / m_cylinderSegments * i) * pi /180.0f;
        float x1 = cosf(angle) * m_upperRadius;
        float z1 = sinf(angle) * m_upperRadius;
        float x2 = cosf(angle) * m_lowerRadius;
        float z2 = sinf(angle) * m_lowerRadius;
        vertices.push_back(x1);
        vertices.push_back(m_cylinderHeight/2.0f);
        vertices.push_back(z1);
        vertices.push_back(x2);
        vertices.push_back(m_cylinderHeight/(-2.0f));
        vertices.push_back(z2);
    }
    for (int i = 2 * m_cylinderSegments + 5; i < 4 * m_cylinderSegments + 6; i++) {
        indices.push_back(i);
        indices.push_back(i + 1);
        indices.push_back(i + 2);
    }
    

    m_vertexLayout = VertexLayout::Create();
    m_vertexBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER,
        GL_STATIC_DRAW, vertices.data(), sizeof(float) * vertices.size());
    
    m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
    // m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
    //m_vertexLayout->SetAttrib(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, sizeof(float) * 3);

    m_indexBuffer = Buffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER,
        GL_STATIC_DRAW, indices.data(), sizeof(uint32_t) * indices.size());
        
    m_indexCount = (int)indices.size();
    
}

void Context::CreateSphere() {

    std::vector<float> verticess;
    std::vector<uint32_t> indicess;

    const float pi = 3.141592f;

    float anglerange = 360.0f;
    float arcspi = 0.0f / 180.0f * pi;
    
    for (int i = 0; i <= 32; i++) {
        float angle = (anglerange / 32 * i) * pi / 180.0f;
        float x1 = cosf(arcspi + angle) * 0.7f;
        float y1 = sinf(arcspi + angle) * 0.7f;
        verticess.push_back(x1);
        verticess.push_back(y1);
        verticess.push_back(0.0f);
        float x2 = cosf(arcspi + angle) * 0.5f;
        float y2 = sinf(arcspi + angle) * 0.5f;
        verticess.push_back(x2);
        verticess.push_back(y2);
        verticess.push_back(0.0f);
    }

    for (int i = 0; i < 32 * 2; i++) {
        indicess.push_back(i);
        indicess.push_back(i + 1);
        indicess.push_back(i + 2);
    }



    m_vertexLayout = VertexLayout::Create();
    m_vertexBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER,
        GL_STATIC_DRAW, verticess.data(), sizeof(float) * verticess.size());

    m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0); 

    m_indexBuffer = Buffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER,
        GL_STATIC_DRAW, indicess.data(), sizeof(uint32_t) * indicess.size());

    m_indexCount = (int)indicess.size();

}

void Context::CreateDonut() {

}




void Context::Render() {	
    if (ImGui::Begin("ui window")) {
        if (ImGui::ColorEdit4("clear color", glm::value_ptr(m_clearColor))) {
            glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);
        }
        ImGui::Separator();
        ImGui::DragFloat3("camera pos", glm::value_ptr(m_cameraPos), 0.01f);
        ImGui::DragFloat("camera yaw", &m_cameraYaw, 0.5f);
        ImGui::DragFloat("camera pitch", &m_cameraPitch, 0.5f, -89.0f, 89.0f);
        ImGui::Separator();
        if (ImGui::Button("reset camera")) {
            m_cameraYaw = 0.0f;
            m_cameraPitch = 0.0f;
            m_cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
        }
        ImGui::Separator();

        static const char * primitiveItems[] = {"box", "cylinder", "sphere", "donut"};
        static const char * textureItems[] = {"wood", "metal", "earth"};
        static int selectedPrimitive = 0;
        static int selectedTexture = 0;
        ImGui::Combo("primitive", &selectedPrimitive, primitiveItems, IM_ARRAYSIZE(primitiveItems));
        ImGui::Combo("texture", &selectedTexture, textureItems, IM_ARRAYSIZE(textureItems));
        
        if(selectedTexture == 0){
            m_program->SetUniform("tex", 0);
        }
        if(selectedTexture == 1){    
            m_program->SetUniform("tex", 1);
        }
        if(selectedTexture == 2){
            m_program->SetUniform("tex", 2);
        }

        if(selectedPrimitive == 0){
            m_primitiveCount = 0;
        }
        if(selectedPrimitive == 1){
            m_primitiveCount = 1;
            ImGui::DragInt("segments", &m_cylinderSegments, 16);
            ImGui::DragFloat("upper radius", &m_upperRadius, 0.5f);
            ImGui::DragFloat("lower radius", &m_lowerRadius, 0.5f);
            ImGui::DragFloat("height", &m_cylinderHeight, 1.0f);
            Context::CreateCylinder(m_cylinderSegments ,m_upperRadius, m_lowerRadius, m_cylinderHeight);

        }
        if(selectedPrimitive == 2){
            Context::CreateSphere();
        }
        if(selectedPrimitive == 3){
        }
    }
    ImGui::End();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    m_cameraFront =
        glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraYaw), glm::vec3(0.0f, 1.0f, 0.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraPitch), glm::vec3(1.0f, 0.0f, 0.0f)) *
        glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);

    auto projection = glm::perspective(glm::radians(45.0f),
        (float)m_width / (float)m_height , 0.01f, 30.0f);

    auto view = glm::lookAt(
      m_cameraPos,
      m_cameraPos + m_cameraFront,
      m_cameraUp);

    auto model = glm::rotate(glm::mat4(1.0f),
        glm::radians((float)glfwGetTime() * 120.0f),
        glm::vec3(1.0f, 0.5f, 0.0f));
    auto transform = projection * view * model;
    m_program->SetUniform("transform", transform);
    glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
   
}