#version 430 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aTexCoord;

out vec4 vertexColor;
out vec2 texCoord;

uniform mat4 transform;

void main()
{
	
	gl_Position = transform * vec4(aPos,1.0) ; 
	// 注： 长方形旋转后会变成平行四边形，是因为是对裁剪空间的坐标进行旋转，而非对屏幕坐标进行旋转
	// 旋转45度时，左上角(-0.5,0.5) -> (-1,0)， 而如果在屏幕空间进行旋转的话,左上角的点只有是宽高比1：1时，
	// 才会在屏幕空间夹角45度，旋转到(-1,0)的位置，此时是正常情况下理解的旋转45度，与裁剪空间旋转恰巧一致。
	vertexColor = vec4(aColor,1.0);
	texCoord = aTexCoord;
}