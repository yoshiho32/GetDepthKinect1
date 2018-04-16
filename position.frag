#version 150 core
#extension GL_ARB_explicit_attrib_location : enable
#extension GL_ARB_explicit_uniform_location : enable
#extension GL_NV_shader_buffer_load : enable

#define MILLIMETER 0.001
#define DEPTH_SCALE (-65535.0 * MILLIMETER)
#define DEPTH_MAXIMUM (-10.0)

#define PI 3.14159
#define KERNEL_SIZE 5

// �X�P�[��
const vec2 scale = vec2(
  1.546592,
  1.222434
);

// �e�N�X�`��
layout (location = 0) uniform sampler2D depth;

// �e�N�X�`�����W
in vec2 texcoord;

// �t���[���o�b�t�@�ɏo�͂���f�[�^
layout (location = 0) out vec3 position;

// 臒l
const float threshold = 0.1 * MILLIMETER;

//���U -- �t�B���^�̋��x�̎w��炵��
const float variance = 0.05;

// 臒l����
vec2 f(const in float z)
{
  return vec2(z, 1.0) * step(threshold, z);
}

//(x,y)�̈ʒu�̒l���o���āA���̒l������Ȃ番��ɒǉ�
void f3(inout float wsum, const in int x ,const in int y){
  vec2 z = f(texture(depth, texcoord + vec2(x, y)).r);
  float xy = -1 * (x*x + y*y)/2.0; 
  wsum += 1 / ( 2 * PI ) * exp(xy) * z.g;
}

//�L���l�ɑ΂��ĕ���wsum�Ŋ����āA���݂̈ʒu�����Ƃɂ����d�݂�������
void f4(in float wsum, inout float csum, const in int x ,const in int y){

  vec2 z = f(texture(depth, texcoord + vec2(x, y)).r);
  float xy = -1 * (x*x + y*y) /2.0;
  float tmp = 1 / ( 2 * PI ) * exp(xy) / wsum;

  csum += tmp * z.r *z.g;

}

// �d�ݕt�����ς����߂�
void main()
{
  float wsum = 0;
  float csum = 0;

  f3(wsum, -2, -2);
  f3(wsum, -1, -2);
  f3(wsum,  0, -2);
  f3(wsum,  1, -2);
  f3(wsum,  2, -2);
  f3(wsum, -2, -1);
  f3(wsum, -1, -1);
  f3(wsum,  0, -1);
  f3(wsum,  1, -1);
  f3(wsum,  2, -1);
  f3(wsum, -2,  0);
  f3(wsum, -1,  0);
  f3(wsum,  0,  0);
  f3(wsum,  1,  0);
  f3(wsum,  2,  0);
  f3(wsum, -2,  1);
  f3(wsum, -1,  1);
  f3(wsum,  0,  1);
  f3(wsum,  1,  1);
  f3(wsum,  2,  1);
  f3(wsum, -2,  2);
  f3(wsum, -1,  2);
  f3(wsum,  0,  2);
  f3(wsum,  1,  2);
  f3(wsum,  2,  2);

    
  f4(wsum, csum, -2, -2);
  f4(wsum, csum, -1, -2);
  f4(wsum, csum,  0, -2);
  f4(wsum, csum,  1, -2);
  f4(wsum, csum,  2, -2);
  f4(wsum, csum, -2, -1);
  f4(wsum, csum, -1, -1);
  f4(wsum, csum,  0, -1);
  f4(wsum, csum,  1, -1);
  f4(wsum, csum,  2, -1);
  f4(wsum, csum, -2,  0);
  f4(wsum, csum, -1,  0);
  f4(wsum, csum,  0,  0);
  f4(wsum, csum,  1,  0);
  f4(wsum, csum,  2,  0);
  f4(wsum, csum, -2,  1);
  f4(wsum, csum, -1,  1);
  f4(wsum, csum,  0,  1);
  f4(wsum, csum,  1,  1);
  f4(wsum, csum,  2,  1);
  f4(wsum, csum, -2,  2);
  f4(wsum, csum, -1,  2);
  f4(wsum, csum,  0,  2);
  f4(wsum, csum,  1,  2);
  f4(wsum, csum,  2,  2);
 
  float z = csum > 0 ? csum * DEPTH_SCALE : DEPTH_MAXIMUM;
  //z = z < 10 ? DEPTH_MAXIMUM : z;
  // �f�v�X�l����J�������W�l�����߂�
  position = vec3(-(texcoord - 0.5) * scale * z, z);
  //position = vec3( -texcoord ,stat.g);

}






/*
//�J�[�l�����̕��U�����߂�
vec3 Dispersion(){
  
  //�J�[�l�����̕��ϒl
  vec2 base =  f(texture(depth, texcoord + vec2(-2, -2)).r) + f(texture(depth, texcoord + vec2(-1, -2)).r) + f(texture(depth, texcoord + vec2( 0, -2)).r) + f(texture(depth, texcoord + vec2( 1, -2)).r) + f(texture(depth, texcoord + vec2( 2, -2)).r) + 
               f(texture(depth, texcoord + vec2(-2, -1)).r) + f(texture(depth, texcoord + vec2(-1, -1)).r) + f(texture(depth, texcoord + vec2( 0, -1)).r) + f(texture(depth, texcoord + vec2( 1, -1)).r) + f(texture(depth, texcoord + vec2( 2, -1)).r) + 
			   f(texture(depth, texcoord + vec2(-2,  0)).r) + f(texture(depth, texcoord + vec2(-1,  0)).r) + f(texture(depth, texcoord + vec2( 0,  0)).r) + f(texture(depth, texcoord + vec2( 1,  0)).r) + f(texture(depth, texcoord + vec2( 2,  0)).r) +
			   f(texture(depth, texcoord + vec2(-2,  1)).r) + f(texture(depth, texcoord + vec2(-1,  1)).r) + f(texture(depth, texcoord + vec2( 0,  1)).r) + f(texture(depth, texcoord + vec2( 1,  1)).r) + f(texture(depth, texcoord + vec2( 2,  1)).r) + 
			   f(texture(depth, texcoord + vec2(-2,  2)).r) + f(texture(depth, texcoord + vec2(-1,  2)).r) + f(texture(depth, texcoord + vec2( 0,  2)).r) + f(texture(depth, texcoord + vec2( 1,  2)).r) + f(texture(depth, texcoord + vec2( 2,  2)).r);
  float avg = base.r / base.g;

  //���U
  float result = texture(depth, texcoord + vec2( -2, -2)).r*texture(depth, texcoord + vec2( -2, -2)).r + texture(depth, texcoord + vec2( -1, -2)).r*texture(depth, texcoord + vec2( -1, -2)).r + texture(depth, texcoord + vec2( -1, -2)).r*texture(depth, texcoord + vec2( -1, -2)).r + texture(depth, texcoord + vec2(  0, -2)).r*texture(depth, texcoord + vec2(  0, -2)).r + texture(depth, texcoord + vec2( 1, -2)).r*texture(depth, texcoord + vec2( 1, -2)).r + texture(depth, texcoord + vec2( 2, -2)).r*texture(depth, texcoord + vec2( 2, -2)).r + 
				 texture(depth, texcoord + vec2( -2, -1)).r*texture(depth, texcoord + vec2( -2, -1)).r + texture(depth, texcoord + vec2( -1, -1)).r*texture(depth, texcoord + vec2( -1, -1)).r + texture(depth, texcoord + vec2(  0, -1)).r*texture(depth, texcoord + vec2(  0, -1)).r + texture(depth, texcoord + vec2( 1, -1)).r*texture(depth, texcoord + vec2( 1, -1)).r + texture(depth, texcoord + vec2( 2, -1)).r*texture(depth, texcoord + vec2( 2, -1)).r + 
			 	 texture(depth, texcoord + vec2( -2,  0)).r*texture(depth, texcoord + vec2( -2,  0)).r + texture(depth, texcoord + vec2( -1,  0)).r*texture(depth, texcoord + vec2( -1,  0)).r + texture(depth, texcoord + vec2(  0,  0)).r*texture(depth, texcoord + vec2(  0,  0)).r + texture(depth, texcoord + vec2( 1,  0)).r*texture(depth, texcoord + vec2( 1,  0)).r + texture(depth, texcoord + vec2( 2,  0)).r*texture(depth, texcoord + vec2( 2,  0)).r +
				 texture(depth, texcoord + vec2( -2,  1)).r*texture(depth, texcoord + vec2( -2,  1)).r + texture(depth, texcoord + vec2( -1,  1)).r*texture(depth, texcoord + vec2( -1,  1)).r + texture(depth, texcoord + vec2(  0,  1)).r*texture(depth, texcoord + vec2(  0,  1)).r + texture(depth, texcoord + vec2( 1,  1)).r*texture(depth, texcoord + vec2( 1,  1)).r + texture(depth, texcoord + vec2( 2,  1)).r*texture(depth, texcoord + vec2( 2,  1)).r + 
			 	 texture(depth, texcoord + vec2( -2,  2)).r*texture(depth, texcoord + vec2( -2,  2)).r + texture(depth, texcoord + vec2( -1,  2)).r*texture(depth, texcoord + vec2( -1,  2)).r + texture(depth, texcoord + vec2(  0,  2)).r*texture(depth, texcoord + vec2(  0,  2)).r + texture(depth, texcoord + vec2( 1,  2)).r*texture(depth, texcoord + vec2( 1,  2)).r + texture(depth, texcoord + vec2( 2,  2)).r*texture(depth, texcoord + vec2( 2,  2)).r;
  
  exp(KERNEL_SIZE*KERNEL_SIZE / result);

  return vec3(result, avg, base.g);

}
*/