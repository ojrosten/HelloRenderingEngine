#version 330 core

in vec2 ComplexPlanePos;

out vec4 FragColor;

void main()
{
    float x_0 = ComplexPlanePos.x,
          y_0 = ComplexPlanePos.y,
          x   = 0,
          y   = 0;

    int n = 0;

    for(;n<1000; ++n) {
        if(x*x + y*y > 4) break;

        float xNext = x*x - y*y + x_0;
        y = 2*y*x + y_0;
        x = xNext;
    }

    float col = 1.0 / (1.0 + n < 500 ? log(1.0 + log(1.0 + n)) : 1000);

    FragColor = vec4(col*0.1, col*0.1, col, 1.0f);
}