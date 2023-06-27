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
    m_numVertices = 0;
    m_numTriangles = 0;
    
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
    
    m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
    m_vertexLayout->SetAttrib(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, sizeof(float) * 3);

    m_indexBuffer = Buffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER,
        GL_STATIC_DRAW, indices, sizeof(uint32_t) *36);

    m_numVertices = 24;
    m_numTriangles = 12;
    m_indexCount = 36;
}

void Context::CreateCylinder(int m_cylinderSegments, float m_upperRadius, float m_lowerRadius, float m_cylinderHeight) {
    
    m_numVertices = 0;
    m_numTriangles = 0;
    std::vector<float> vertices;
    std::vector<uint32_t> indices;

    const float pi = 3.141592f;

    //윗면 생성
    vertices.push_back(0.0f);
    vertices.push_back(m_cylinderHeight/2.0f);
    vertices.push_back(0.0f);
    
    vertices.push_back(1.0f);
    vertices.push_back(0.5f);
    
    for (int i = 0; i <= m_cylinderSegments; i++) {
        float angle = (360.0f / m_cylinderSegments * i) * pi /180.0f;
        float x = cosf(angle) * m_upperRadius;
        float z = sinf(angle) * m_upperRadius;
        vertices.push_back(x);
        vertices.push_back(m_cylinderHeight/2.0f);
        vertices.push_back(z);

        vertices.push_back(0.7f);
        vertices.push_back(float(i) / float (m_cylinderSegments) );
        m_numVertices++;
    }
    for (int i = 0; i < m_cylinderSegments; i++) {
        indices.push_back(0);
        indices.push_back(i + 1);
        indices.push_back(i + 2);
        m_numTriangles++;
    }

    //아랫면 생성
    vertices.push_back(0.0f);
    vertices.push_back(m_cylinderHeight/(-2.0f));
    vertices.push_back(0.0f);
    
    vertices.push_back(0.0f);
    vertices.push_back(0.5f);

    for (int i = 0; i <= m_cylinderSegments; i++) {
        float angle = (360.0f / m_cylinderSegments * i) * pi /180.0f;
        float x = cosf(angle) * m_lowerRadius;
        float z = sinf(angle) * m_lowerRadius;
        vertices.push_back(x);
        vertices.push_back(m_cylinderHeight/(-2.0f));
        vertices.push_back(z);
        
        vertices.push_back(0.3f);
        vertices.push_back(float(i) / float (m_cylinderSegments) );
        m_numVertices++;
    }
    for (int i = m_cylinderSegments + 2; i < 2 * m_cylinderSegments + 3; i++) {
        indices.push_back(m_cylinderSegments + 2);
        indices.push_back(i + 1);
        indices.push_back(i + 2);
        m_numTriangles++;
    }

    //옆면 생성
    for (int i = 0; i <= m_cylinderSegments ; i++) {
        float angle = (360.0f / m_cylinderSegments * i) * pi /180.0f;
        float x1 = cosf(angle) * m_upperRadius;
        float z1 = sinf(angle) * m_upperRadius;
        float x2 = cosf(angle) * m_lowerRadius;
        float z2 = sinf(angle) * m_lowerRadius;
        vertices.push_back(x1);
        vertices.push_back(m_cylinderHeight/2.0f);
        vertices.push_back(z1);

        vertices.push_back(0.7f);
        vertices.push_back(float(i) / float(m_cylinderSegments) );

        vertices.push_back(x2);
        vertices.push_back(m_cylinderHeight/(-2.0f));
        vertices.push_back(z2);

        vertices.push_back(0.3f);
        vertices.push_back(float(i) / float(m_cylinderSegments) );
        m_numVertices+=2;
    }
    for (int i = 2 * m_cylinderSegments + 4; i < 4 * m_cylinderSegments + 6; i++) {
        indices.push_back(i);
        indices.push_back(i + 1);
        indices.push_back(i + 2);
        m_numTriangles++;
    }
    
    m_vertexLayout = VertexLayout::Create();
    m_vertexBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER,
        GL_STATIC_DRAW, vertices.data(), sizeof(float) * vertices.size());

    m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
    m_vertexLayout->SetAttrib(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, sizeof(float) * 3);

    m_indexBuffer = Buffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER,
        GL_STATIC_DRAW, indices.data(), sizeof(uint32_t) * indices.size());
    
    m_indexCount = (int)indices.size();
}

void Context::CreateSphere(int m_sphereHeightSeg, int m_sphereWidthSeg) {
    
    m_numVertices = 0;
    m_numTriangles = 0;
    std::vector<float> vertices;
    std::vector<uint32_t> indices;

    const float pi = 3.141592f;
    
    //높이 초기값
    float heightAngle = 90.0f * pi / 180.0f;
    //높이 범위
    float angleRange = (180.0f / m_sphereHeightSeg) * pi /180.0f;

    for (int i = 0 ; i < m_sphereHeightSeg; i++) {
        for (int j = 0; j <= m_sphereWidthSeg; j++) {
            float widthAngle = (360.0f / m_sphereWidthSeg * j) * pi /180.0f;
            float x1 = cosf(heightAngle) * cosf(widthAngle);
            float y1 = sinf(heightAngle);
            float z1 = cosf(heightAngle) * sinf(widthAngle);

            float x2 = cosf(heightAngle - angleRange) * cosf(widthAngle);
            float y2 = sinf(heightAngle - angleRange);
            float z2 = cosf(heightAngle - angleRange) * sinf(widthAngle);
            // 점들은 위에서부터 아래로 내려오게 하였고 텍스쳐 코디네잇은 구의 위아래 -1,1 을 0,1 로 바꾸고 
            // j가 각도가 90에서 줄어들때마다 반복되므로 j값을 0,1 사이에서 반복하게 했다
            vertices.push_back(x1);
            vertices.push_back(y1);
            vertices.push_back(z1);

            vertices.push_back(y1/2.0f + 0.5f);
            vertices.push_back(0.0f + float(j) / float(m_sphereWidthSeg) );

            vertices.push_back(x2);
            vertices.push_back(y2);
            vertices.push_back(z2);

            vertices.push_back(y2/2.0f + 0.5f);
            vertices.push_back(0.0f + float(j) / float(m_sphereWidthSeg) );

            m_numVertices +=2;
        }

        heightAngle = heightAngle - angleRange;
    }
    for (int i = 0; i < m_numVertices - 2 ; i++) {
        indices.push_back(i);
        indices.push_back(i + 1);
        indices.push_back(i + 2);
        m_numTriangles++;
    }

    m_vertexLayout = VertexLayout::Create();
    m_vertexBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER,
        GL_STATIC_DRAW, vertices.data(), sizeof(float) * vertices.size());

    m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
    m_vertexLayout->SetAttrib(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, sizeof(float) * 3);

    m_indexBuffer = Buffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER,
        GL_STATIC_DRAW, indices.data(), sizeof(uint32_t) * indices.size());

    m_indexCount = (int)indices.size();
}

void Context::CreateDonut(int m_donutOutSeg, int m_donutInSeg, float m_donutOutRadius, float m_donutInRadius) {
    
    m_numVertices = 0;
    m_numTriangles = 0;
    std::vector<float> vertices;
    std::vector<uint32_t> indices;

    const float pi = 3.141592f;
    
    float outAngle = 0.0f;
    float outAngleRange = (360.0f / m_donutOutSeg) * pi / 180.0f;
    float inAngleRange = (360.0f / m_donutInSeg) * pi /180.0f;
    
    for (int i =0 ; i <= m_donutOutSeg ; i ++) {
        for (int j =0 ; j <= m_donutInSeg ; j ++) {
            //도넛 내부 원들의 좌표
            float inAngle = (360.0f / m_donutInSeg * j) * pi / 180.0f;
            float x = (m_donutInRadius * cosf(inAngle) + m_donutOutRadius) * cosf(outAngle);
            float y = m_donutInRadius * sinf(inAngle);
            float z = (m_donutInRadius * cosf(inAngle) + m_donutOutRadius) * sinf(outAngle);
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            vertices.push_back(float(j) / float(m_donutInSeg));
            vertices.push_back(float(i) / float(m_donutOutSeg));

            m_numVertices += 1;
        }
        outAngle = outAngle + outAngleRange;
    }

    for (int i = 0; i <= m_numVertices - m_donutInSeg - 2; i++) {
        //원들의 좌표로 도넛 그리기
        indices.push_back(i);
        indices.push_back(i + 1);
        indices.push_back(i + m_donutInSeg + 1);

        indices.push_back(i + 1);
        indices.push_back(i + m_donutInSeg + 1);
        indices.push_back(i + m_donutInSeg + 2);

        m_numTriangles+=2;
    }

    m_vertexLayout = VertexLayout::Create();
    m_vertexBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER,
        GL_STATIC_DRAW, vertices.data(), sizeof(float) * vertices.size());

    m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
    m_vertexLayout->SetAttrib(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, sizeof(float) * 3);

    m_indexBuffer = Buffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER,
        GL_STATIC_DRAW, indices.data(), sizeof(uint32_t) * indices.size());

    m_indexCount = (int)indices.size();
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
        ImGui::LabelText("vertices","%d", m_numVertices);
        ImGui::LabelText("triangles","%d", m_numTriangles);
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
            Context::CreateBox();
        }
        if(selectedPrimitive == 1){
            ImGui::DragInt("segments", &m_cylinderSegments, 16);
            ImGui::DragFloat("upper radius", &m_upperRadius, 0.5f);
            ImGui::DragFloat("lower radius", &m_lowerRadius, 0.5f);
            ImGui::DragFloat("height", &m_cylinderHeight, 1.0f);
            Context::CreateCylinder(m_cylinderSegments ,m_upperRadius, m_lowerRadius, m_cylinderHeight);
        }
        if(selectedPrimitive == 2){
            ImGui::DragInt("lati.seg", &m_sphereHeightSeg, 16);
            ImGui::DragInt("longi.seg", &m_sphereWidthSeg, 32);
            Context::CreateSphere(m_sphereHeightSeg, m_sphereWidthSeg);
        }
        if(selectedPrimitive == 3){
            ImGui::DragInt("outer seg", &m_donutOutSeg, 32);
            ImGui::DragInt("inner seg", &m_donutInSeg, 16);
            ImGui::DragFloat("outer radius", &m_donutOutRadius, 1.0f);
            ImGui::DragFloat("inner radius", &m_donutInRadius, 0.25f);
            Context::CreateDonut(m_donutOutSeg, m_donutInSeg, m_donutOutRadius, m_donutInRadius);
        }
        ImGui::Separator();
        
        ImGui::DragFloat3("rotatinon", glm::value_ptr(m_rotation) , 0.03f);
        
        ImGui::DragFloat3("scale", glm::value_ptr(m_scale), 0.003f);

        static bool selectedAnimation = false;
        ImGui::Checkbox("animation", &selectedAnimation);
        if(selectedAnimation == false ) {
        }
        if(selectedAnimation == true ) {
            m_rotation.x += (float)glfwGetTime() / 1000.0f * m_rotSpeed.x;
            m_rotation.y += (float)glfwGetTime() / 1000.0f * m_rotSpeed.y;
            m_rotation.z += (float)glfwGetTime() / 1000.0f * m_rotSpeed.z;
            if(m_rotation.x > 360.0f) m_rotation.x = 0.0f;
            if(m_rotation.y > 360.0f) m_rotation.y = 0.0f;
            if(m_rotation.z > 360.0f) m_rotation.z = 0.0f;
        }
        
        ImGui::DragFloat3("rot speed", glm::value_ptr(m_rotSpeed), 0.01f);

        if (ImGui::Button("reset transform")) {
            m_rotation.x = 0.0f;
            m_rotation.y = 0.0f;
            m_rotation.z = 0.0f;
            m_scale.x = 1.0f;
            m_scale.y = 1.0f;
            m_scale.z = 1.0f;
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

    //x,y,z축 회전
    auto rotationx = glm::rotate(glm::mat4(1.0f),
        glm::radians( m_rotation.x ),
        glm::vec3(1.0f, 0.0f, 0.0f));
    auto rotationy = glm::rotate(glm::mat4(1.0f),
        glm::radians( m_rotation.y ),
        glm::vec3(0.0f, 1.0f, 0.0f));
    auto rotationz = glm::rotate(glm::mat4(1.0f),
        glm::radians( m_rotation.z ),
        glm::vec3(0.0f, 0.0f, 1.0f));
    //x,y,z축 확대
    auto objectScale = glm::scale(glm::mat4(1.0f), m_scale);

    auto transform = projection * view * rotationx * rotationy * rotationz * objectScale ;
    m_program->SetUniform("transform", transform);
    glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
   
}