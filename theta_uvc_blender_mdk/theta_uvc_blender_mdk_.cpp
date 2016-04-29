// ChabuCam.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//
// http://suzuichibolgpg.blog.fc2.com/blog-entry-109.html
// http://qiita.com/doxas/items/f3f8bf868f12851ea143
// http://marina.sys.wakayama-u.ac.jp/~tokoi/oglarticles.html

#define VERSION "0.1.1"

#include "stdafx.h"
#include <windows.h>

#define _USE_MATH_DEFINES
#include <math.h>

// �W���w�b�_���C���N���[�h
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>

// GLEW���C���N���[�h
#include <GL/glew.h>

// GLFW���C���N���[�h
#include <GLFW/glfw3.h>
GLFWwindow* window;

// GLM���C���N���[�h
//#include <glm/glm.hpp>
//using namespace glm;

//#include <glut.h>

#include <common/shader.hpp>

/*
#include "opencv2/core/core_c.h"
#include "opencv2/core/core.hpp"
#include "opencv2/flann/miniflann.hpp"
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/photo/photo.hpp"
#include "opencv2/video/video.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/ml/ml.hpp"
#include "opencv2/highgui/highgui_c.h"
#include "opencv2/highgui/highgui.hpp"
//#include "opencv2/contrib/contrib.hpp"
*/

#include <opencv2/opencv.hpp>
#include <opencv_lib.h>

#include <CaptureSender.h>
#include <ycapture.h>


#ifdef _DEBUG
//Debug���[�h�̏ꍇ
#pragma comment(lib,"C:\\opencv_2.4.11\\opencv\\build\\x86\\vc12\\lib\\opencv_core2411d.lib")
#pragma comment(lib,"C:\\opencv_2.4.11\\opencv\\build\\x86\\vc12\\lib\\opencv_imgproc2411d.lib")
#pragma comment(lib,"C:\\opencv_2.4.11\\opencv\\build\\x86\\vc12\\lib\\opencv_highgui2411d.lib")
#pragma comment(lib,"C:\\opencv_2.4.11\\opencv\\build\\x86\\vc12\\lib\\opencv_objdetect2411d.lib")
#pragma comment(lib,"C:\\opencv_2.4.11\\opencv\\build\\x86\\vc12\\lib\\opencv_contrib2411d.lib")
#pragma comment(lib,"C:\\opencv_2.4.11\\opencv\\build\\x86\\vc12\\lib\\opencv_features2d2411d.lib")
#pragma comment(lib,"C:\\opencv_2.4.11\\opencv\\build\\x86\\vc12\\lib\\opencv_flann2411d.lib")
#pragma comment(lib,"C:\\opencv_2.4.11\\opencv\\build\\x86\\vc12\\lib\\opencv_gpu2411d.lib")
#pragma comment(lib,"C:\\opencv_2.4.11\\opencv\\build\\x86\\vc12\\lib\\opencv_legacy2411d.lib")
#pragma comment(lib,"C:\\opencv_2.4.11\\opencv\\build\\x86\\vc12\\lib\\opencv_ts2411d.lib")
#pragma comment(lib,"C:\\opencv_2.4.11\\opencv\\build\\x86\\vc12\\lib\\opencv_video2411d.lib")
#pragma comment(lib,"C:\\opencv_2.4.11\\opencv\\build\\x86\\vc12\\lib\\opencv_calib3d2411d.lib")
#pragma comment(lib,"C:\\opencv_2.4.11\\opencv\\build\\x86\\vc12\\lib\\opencv_nonfree2411d.lib")
#else
//Release���[�h�̏ꍇ
#pragma comment(lib,"C:\\opencv_2.4.11\\opencv\\build\\x86\\vc12\\lib\\opencv_core2411.lib")
#pragma comment(lib,"C:\\opencv_2.4.11\\opencv\\build\\x86\\vc12\\lib\\opencv_imgproc2411.lib")
#pragma comment(lib,"C:\\opencv_2.4.11\\opencv\\build\\x86\\vc12\\lib\\opencv_highgui2411.lib")
#pragma comment(lib,"C:\\opencv_2.4.11\\opencv\\build\\x86\\vc12\\lib\\opencv_objdetect2411.lib")
#pragma comment(lib,"C:\\opencv_2.4.11\\opencv\\build\\x86\\vc12\\lib\\opencv_contrib2411.lib")
#pragma comment(lib,"C:\\opencv_2.4.11\\opencv\\build\\x86\\vc12\\lib\\opencv_features2d2411.lib")
#pragma comment(lib,"C:\\opencv_2.4.11\\opencv\\build\\x86\\vc12\\lib\\opencv_flann2411.lib")
#pragma comment(lib,"C:\\opencv_2.4.11\\opencv\\build\\x86\\vc12\\lib\\opencv_gpu2411.lib")
#pragma comment(lib,"C:\\opencv_2.4.11\\opencv\\build\\x86\\vc12\\lib\\opencv_legacy2411.lib")
#pragma comment(lib,"C:\\opencv_2.4.11\\opencv\\build\\x86\\vc12\\lib\\opencv_ts2411.lib")
#pragma comment(lib,"C:\\opencv_2.4.11\\opencv\\build\\x86\\vc12\\lib\\opencv_video2411.lib")
#pragma comment(lib,"C:\\opencv_2.4.11\\opencv\\build\\x86\\vc12\\lib\\opencv_calib3d2411.lib")
#pragma comment(lib,"C:\\opencv_2.4.11\\opencv\\build\\x86\\vc12\\lib\\opencv_nonfree2411.lib")
#endif

#pragma comment(lib,"C:\\Users\\shin\\Documents\\Visual Studio 2013\\Projects\\ycapture-src-0.1.1\\ycapture\\Release\\ycapture.lib")
#pragma comment(lib,"C:\\Users\\shin\\Documents\\Visual Studio 2013\\Projects\\ycapture-src-0.1.1\\ycapture\\Release\\ycaptureclient.lib")

float g_view_scale[4] = { 1.0, 1.0, 1.0, 1.0 };
float g_view_lng[4] = { 0.0, 0.0, 0.0, 0.0 };
float g_view_lat[4] = { 0.0, 0.0, 0.0, 0.0 };
float g_view_phi[4] = { 0.0, 0.0, 0.0, 0.0 };

void key_event(void)
{
	// key event
	int key_num = -1;
	float lng_step = 4.0;
	float lat_step = 4.0;
	float phi_step = 4.0;
	float scale_step = 0.1;

	if (glfwGetKey(window, GLFW_KEY_1))	key_num = 0;
	else if (glfwGetKey(window, GLFW_KEY_2)) key_num = 1;
	else if (glfwGetKey(window, GLFW_KEY_3)) key_num = 2;
	else if (glfwGetKey(window, GLFW_KEY_4)) key_num = 3;

	// key event
	if (key_num == -1) {
		for (int i = 0; i < 4; i++) {
			if (glfwGetKey(window, GLFW_KEY_LEFT) != GLFW_RELEASE) {
				g_view_lng[i] += M_PI * lng_step / 180.0;
			}
			else if (glfwGetKey(window, GLFW_KEY_RIGHT) != GLFW_RELEASE) {
				g_view_lng[i] -= M_PI * lng_step / 180.0;
			}
			else if (glfwGetKey(window, GLFW_KEY_UP) != GLFW_RELEASE) {
				g_view_lat[i] += M_PI * lat_step / 180.0;
			}
			else if (glfwGetKey(window, GLFW_KEY_DOWN) != GLFW_RELEASE) {
				g_view_lat[i] -= M_PI * lat_step / 180.0;
			}
			else if (glfwGetKey(window, GLFW_KEY_R) != GLFW_RELEASE) {
				g_view_phi[i] += M_PI * phi_step / 180.0;
			}
			else if (glfwGetKey(window, GLFW_KEY_L) != GLFW_RELEASE) {
				g_view_phi[i] -= M_PI * phi_step / 180.0;
			}
			else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) != GLFW_RELEASE) {
				g_view_scale[i] += scale_step;
			}
			else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) != GLFW_RELEASE){
				g_view_scale[i] -= scale_step;
			}
			else if (glfwGetKey(window, GLFW_KEY_SPACE) != GLFW_RELEASE) { // reset
				g_view_lng[i] = 0.0;
				g_view_lat[i] = 0.0;
				g_view_phi[i] = 0.0;
				g_view_scale[i] = 1.0;
			}
		}
	}
	else {
		if (glfwGetKey(window, GLFW_KEY_LEFT) != GLFW_RELEASE) {
			g_view_lng[key_num] += M_PI * lng_step / 180.0;
		}
		else if (glfwGetKey(window, GLFW_KEY_RIGHT) != GLFW_RELEASE) {
			g_view_lng[key_num] -= M_PI * lng_step / 180.0;
		}
		else if (glfwGetKey(window, GLFW_KEY_UP) != GLFW_RELEASE) {
			g_view_lat[key_num] += M_PI * lat_step / 180.0;
		}
		else if (glfwGetKey(window, GLFW_KEY_DOWN) != GLFW_RELEASE) {
			g_view_lat[key_num] -= M_PI * lat_step / 180.0;
		}
		else if (glfwGetKey(window, GLFW_KEY_R) != GLFW_RELEASE) {
			g_view_phi[key_num] += M_PI * phi_step / 180.0;
		}
		else if (glfwGetKey(window, GLFW_KEY_L) != GLFW_RELEASE) {
			g_view_phi[key_num] -= M_PI * phi_step / 180.0;
		}
		else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) != GLFW_RELEASE) {
			g_view_scale[key_num] += scale_step;
		}
		else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) != GLFW_RELEASE){
			g_view_scale[key_num] -= scale_step;
		}
		else if (glfwGetKey(window, GLFW_KEY_SPACE) != GLFW_RELEASE) { // reset
			g_view_lng[key_num] = 0.0;
			g_view_lat[key_num] = 0.0;
			g_view_phi[key_num] = 0.0;
			g_view_scale[key_num] = 1.0;
		}
	}

	// limitter
	for (int i = 0; i < 4; i++)
		if (g_view_scale[i] < 0.2)
			g_view_scale[i] = 0.2;
}

GLuint init(const int win_width, const int win_height)
{
	// GLFW�̏�����
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_VISIBLE, GL_FALSE);

	// �E�B���h�E���J���AOpenGL�R���e�L�X�g�����
	//window = glfwCreateWindow(capture_width, capture_height, "DFE to Equirectangular", NULL, NULL);
	window = glfwCreateWindow(win_width, win_height, "ChabuCam", NULL, NULL);
	//glfwWindowHint(window, GLFW_INVISIBLE);
	//glfwHideWindow(window);
	if (window == NULL){
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// GLEW�̏�����
	glewExperimental = true; // �R�A�v���t�@�C���ŕK�v
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	// ESC���m�p
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	//LoadString(

	// GLSL�v���O�������V�F�[�_����쐬�A�R���p�C��
	//GLuint programID = LoadShaders("SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader");
	GLuint programID = LoadShaders("SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader");





	return programID;

}

void transport_texture(GLuint image, GLubyte* dataBuffer, const int win_width, const int win_height, cv::Mat& frame)
{
	// �؂�o�����摜���e�N�X�`���ɓ]������
	glBindTexture(GL_TEXTURE_RECTANGLE, image);
	glTexSubImage2D(GL_TEXTURE_RECTANGLE, 0, 0, 0, frame.cols, frame.rows, GL_BGR, GL_UNSIGNED_BYTE, frame.data);

	// �ǂݎ��OpneGL�̃o�b�t�@���w�� GL_FRONT:�t�����g�o�b�t�@�@GL_BACK:�o�b�N�o�b�t�@
	glReadBuffer(GL_BACK);
	// OpenGL�ŉ�ʂɕ`�悳��Ă�����e���o�b�t�@�Ɋi�[
	glReadPixels(
		0,                 //�ǂݎ��̈�̍�������x���W
		0,                 //�ǂݎ��̈�̍�������y���W //0 or getCurrentWidth() - 1
		win_width,             //�ǂݎ��̈�̕�
		win_height,            //�ǂݎ��̈�̍���
		GL_RGB, //it means GL_BGR,           //�擾�������F���̌`��
		GL_UNSIGNED_BYTE,  //�ǂݎ�����f�[�^��ۑ�����z��̌^
		dataBuffer      //�r�b�g�}�b�v�̃s�N�Z���f�[�^�i���ۂɂ̓o�C�g�z��j�ւ̃|�C���^
		);
}

void prepare_texture(GLuint *image, const GLsizei capture_width, const GLsizei capture_height)
{
	glGenTextures(1, image);
	glBindTexture(GL_TEXTURE_RECTANGLE, *image);
	//glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGB, capture_width, capture_height, 0, GL_BGR, GL_UNSIGNED_BYTE, NULL);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGB, capture_width, capture_height, 0, GL_BGR, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

}

void copy_buf_to_mat(GLubyte* dataBuffer, cv::Mat& view_frame, const int win_width, const int win_height)
{
	GLubyte* p = static_cast<GLubyte*>(dataBuffer);
	for (unsigned int j = 0; j < win_height; ++j)
	{
		for (unsigned int i = 0; i < win_width; ++i)
		{
			view_frame.data[j * view_frame.step + i * view_frame.elemSize() + 0] = *p;
			view_frame.data[j * view_frame.step + i * view_frame.elemSize() + 1] = *(p + 1);
			view_frame.data[j * view_frame.step + i * view_frame.elemSize() + 2] = *(p + 2);
			p += 3;
		}
	}
}
void usage(char* argv[])
{
	fprintf(stdout, "ChabuCam ver %s\n", VERSION);
	fprintf(stdout, "Usage:\n");
	fprintf(stdout, "%s <device_id> <mode> <out_width> <out_height>\n");
	fprintf(stdout, "  mode:  0=split 2 window\n");
	fprintf(stdout, "         1=split 4 window\n");

	exit(1);
}

int main(int argc, char* argv[])
{
	if (argc != 2)
		usage(argv);

	int device_id = atoi(argv[1]);
	float win_width = 1280;
	float win_height = 720;

	// OpenCV �ɂ��r�f�I�L���v�`��������������
	cv::VideoCapture camera(device_id);
	if (!camera.isOpened())
	{
		std::cerr << "cannot open input" << std::endl;
		exit(1);
	}

	// �J�����̏����ݒ�
	camera.grab();

	const GLsizei capture_width(GLsizei(camera.get(CV_CAP_PROP_FRAME_WIDTH)));
	const GLsizei capture_height(GLsizei(camera.get(CV_CAP_PROP_FRAME_HEIGHT)));
	win_width = capture_width;
	win_height = capture_height;

	CaptureSender sender(CS_SHARED_PATH, CS_EVENT_WRITE, CS_EVENT_READ);
	DWORD avgTimePF = 1000 / 15;		// 15fps
	int i_frame = 0;

	//cv::CascadeClassifier cascade;
	//cascade.load("C:\\opencv_2.4.11\\opencv\\sources\\data\\haarcascades\\haarcascade_frontalface_default.xml");
	std::vector<cv::Rect> faces;
	cv::Mat view_frame = cv::Mat(cv::Size(win_width, win_height), CV_8UC3);
	//cv::namedWindow("face");	

	// �v���O�����I�����̏�����o�^����
	atexit(glfwTerminate);

	GLuint programID = init(win_width, win_height);

	// �e�N�X�`������������
	GLuint image;
	prepare_texture(&image, capture_width, capture_height);


	// �쐬�����E�B���h�E�ɑ΂���ݒ�
	glfwSwapInterval(1);

	// �_�[�N�u���[�̔w�i
	//glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	void* dataBuffer = NULL;
	dataBuffer = (GLubyte*)malloc(win_width * win_height * 3);

	// �V�F�[�_���g��
	glUseProgram(programID);


	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// ���_�z��I�u�W�F�N�g
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);

	/*
	static const GLfloat g_vertex_buffer_data[] = {
	-1.0f, -1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	};
	*/
	static const GLfloat g_vertex_buffer_data[][2] = {
		{ -1.0f, -1.0f },
		{ 1.0f, -1.0f },
		{ 1.0f, 1.0f },
		{ -1.0f, 1.0f }
	};
	static const int vertices(sizeof g_vertex_buffer_data / sizeof g_vertex_buffer_data[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	// �ŏ��̑����o�b�t�@ : ���_
	glEnableVertexAttribArray(0);
	//glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);


	GLint dfe_alpha_ID = glGetUniformLocation(programID, "DFE_ALPHA_BLENDING_R_RATIO");
	GLint cam_r_ID = glGetUniformLocation(programID, "CAM_R");
	GLint cam1_cpx_ID = glGetUniformLocation(programID, "CAM1_CPX");
	GLint cam1_cpy_ID = glGetUniformLocation(programID, "CAM1_CPY");
	GLint cam2_cpx_ID = glGetUniformLocation(programID, "CAM2_CPX");
	GLint cam2_cpy_ID = glGetUniformLocation(programID, "CAM2_CPY");
	glUniform1f(dfe_alpha_ID, 0.950);
	glUniform1f(cam_r_ID, 283.0);
	glUniform1f(cam1_cpx_ID, 319.0);
	glUniform1f(cam1_cpy_ID, 319.0);
	glUniform1f(cam2_cpx_ID, 959.0);
	glUniform1f(cam2_cpy_ID, 319.0);


	//glutMouseFunc(mouse);


	do{
		// �X�N���[�����N���A
		glClear(GL_COLOR_BUFFER_BIT);

		if (camera.grab())
		{
			// �L���v�`���f������摜��؂�o��
			cv::Mat frame;
			camera.retrieve(frame, 3);
			//cv::flip(frame, frame, 0);

			transport_texture(image, (GLubyte*)dataBuffer, win_width, win_height, frame);

			GLubyte* p = static_cast<GLubyte*>(dataBuffer);
			view_frame.data = p;
			//copy_buf_to_mat((GLubyte*)dataBuffer, view_frame, win_width, win_height);

			HRESULT hr = sender.Send(i_frame * avgTimePF, win_width, win_height, view_frame.data);
			printf(".");
			//HRESULT hr = sender.Send(i_frame * avgTimePF, capture_width, capture_height, frame.data);
			//HRESULT hr = sender.Send(i_frame * avgTimePF, 320, 180, shrink_frame.data);
			//HRESULT hr = sender.Send(i * avgTimePF, 1280, 720, );

			i_frame += 1;
		}

		// �e�N�X�`�����j�b�g�ƃe�N�X�`���̎w��
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_RECTANGLE, image);

		// �`��Ɏg�����_�z��I�u�W�F�N�g�̎w��
		glBindVertexArray(vao);

		// �}�`�̕`��
		glDrawArrays(GL_TRIANGLE_FAN, 0, vertices);

		// ���_�z��I�u�W�F�N�g�̎w�����
		glBindVertexArray(0);
		glDisableVertexAttribArray(0);

		// �o�b�t�@����������
		glfwSwapBuffers(window);

		// key event
		glfwPollEvents();


	} // ESC�L�[�������ꂽ���A�E�B���h�E������ꂽ�����m�F����
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
	glfwWindowShouldClose(window) == 0);

	//
	free(dataBuffer);

	// VBO��Еt����
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteProgram(programID);

	// OpenGL�E�B���h�E����Aglfw���I������
	glfwTerminate();

	return 0;
}
