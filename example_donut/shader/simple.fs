#version 330 core
uniform vec4 color;
out vec4 fragColor;

void main() {
    fragColor = color;
}
/*
#version 330 core

in vec4 vertexColor; // vs로부터 입력된 변수 (같은 변수명, 같은 타입)
out vec4 FragColor; // 최종 출력 색상

void main() {
  FragColor = vertexColor;
}


#version 330 core
out vec4 fragColor;

void main() {
  fragColor = vec4(0.7, 0.7, 0.5, 1.0);
}
*/