#version 330 core

in vec2  ComplexPlanePos;
out vec4 FragColor;

void main()
{
	float x_0 = ComplexPlanePos.x,
		  y_0 = ComplexPlanePos.y,
		  x = 0,
		  y = 0;

	int i = 0;
		  
    for(;i<1000; i++) {
		float xSqd = x*x,
		      ySqd = y*y;
		if(xSqd + ySqd > 4) break;
		
		float xNext = xSqd - ySqd + x_0;
		y = 2*x*y + y_0;
		x = xNext;
	}
	
	float col = 1.0 / (1 + i < 500 ? log(1 + log(1 + i)) : 1000);
	
    FragColor = vec4(col*0.1, col*0.1, col, 1.0f);
}