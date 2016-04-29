// theta_uvc_blender_mdk.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//
// Thanks to
// OpenGL, GLSL:
//   http://marina.sys.wakayama-u.ac.jp/~tokoi/oglarticles.html
//   http://suzuichibolgpg.blog.fc2.com/blog-entry-109.html
//   http://qiita.com/doxas/items/f3f8bf868f12851ea143
// ycapture:
//   http://yzwlab.net/ycapture/
//   http://izmiz.hateblo.jp/entry/2015/01/12/220133
#define VERSION "0.2.0"

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

// shader helper method
#include <common/shader.hpp>

// opencv 2.4.11
#include <opencv2/opencv.hpp>
#include <opencv_lib.h>

// ycapture
#include <CaptureSender.h>
#include "my_ycapture.h"
#include <ycapture_lib.h>


int init(const int win_width, const int win_height, const int preview_sw)
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
	if(preview_sw == 0)
		glfwWindowHint(GLFW_VISIBLE, GL_FALSE);

	// �E�B���h�E���J���AOpenGL�R���e�L�X�g�����
	window = glfwCreateWindow(win_width, win_height, "theta uvc blender mdk", NULL, NULL);
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

	return 0;
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
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGB, capture_width, capture_height, 0, GL_BGR, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

}

void usage(char* argv[])
{
	fprintf(stdout, "theta_uvc_blender_mdk ver %s\n", VERSION);
	fprintf(stdout, "Usage:\n");
	fprintf(stdout, "theta_uvc_blender_mdk.exe <device_id> <out_cam_width> <out_cam_height> <preview_sw>\n");

	exit(1);
}

int main(int argc, char* argv[])
{
	if (argc != 5)
		usage(argv);

	int device_id = atoi(argv[1]);
	int win_width = atoi(argv[2]);
	int win_height = atoi(argv[3]);
	int preview_sw = atoi(argv[4]);
	double prev_x = 0.0, prev_y = 0.0;
	int mouse_state = 0;

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

	CaptureSender sender(CS_SHARED_PATH, CS_EVENT_WRITE, CS_EVENT_READ);
	DWORD avgTimePF = 1000 / 15;		// 15fps
	int i_frame = 0;

	cv::Mat out_cam_frame = cv::Mat(cv::Size((int)win_width, (int)win_height), CV_8UC3);
	cv::Mat rot_vec = (cv::Mat_<float>(3, 1) << 0.0, 0.0, 0.0);

	// �v���O�����I�����̏�����o�^����
	atexit(glfwTerminate);

	init((int)win_width, (int)win_height, preview_sw);

	// �e�N�X�`������������
	GLuint image;
	prepare_texture(&image, capture_width, capture_height);

	// �쐬�����E�B���h�E�ɑ΂���ݒ�
	glfwSwapInterval(1);

	// �_�[�N�u���[�̔w�i
	//glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	void* dataBuffer = NULL;
	dataBuffer = (GLubyte*)malloc((int)win_width * (int)win_height * 3);

	// �V�F�[�_���g��
	// GLSL�v���O�������V�F�[�_����쐬�A�R���p�C��
	GLuint programID = LoadShaders("SimpleVertexShader.vertexshader", "DFE2EquiShader.fragmentshader");
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

	static const GLfloat g_vertex_buffer_data[][2] = {
		{ -1.0f, -1.0f },
		{  1.0f, -1.0f },
		{  1.0f,  1.0f },
		{ -1.0f,  1.0f }
	};
	static const int vertices(sizeof g_vertex_buffer_data / sizeof g_vertex_buffer_data[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	// �ŏ��̑����o�b�t�@ : ���_
	glEnableVertexAttribArray(0);
	//glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);


	//GLint dfe_alpha_ID = glGetUniformLocation(programID, "DFE_ALPHA_BLENDING_R_RATIO");
	//GLint cam_r_ID = glGetUniformLocation(programID, "CAM_R");
	//GLint cam1_cpx_ID = glGetUniformLocation(programID, "CAM1_CPX");
	//GLint cam1_cpy_ID = glGetUniformLocation(programID, "CAM1_CPY");
	//GLint cam2_cpx_ID = glGetUniformLocation(programID, "CAM2_CPX");
	//GLint cam2_cpy_ID = glGetUniformLocation(programID, "CAM2_CPY");
	GLint width_ID = glGetUniformLocation(programID, "uni_width");
	GLint height_ID = glGetUniformLocation(programID, "uni_height");
	GLint rot_vec_ID = glGetUniformLocation(programID, "uni_rot_vec");
	//glUniform1f(dfe_alpha_ID, 0.950);
	//glUniform1f(cam_r_ID, 283.0);
	//glUniform1f(cam1_cpx_ID, 319.0);
	//glUniform1f(cam1_cpy_ID, 319.0);
	//glUniform1f(cam2_cpx_ID, 959.0);
	//glUniform1f(cam2_cpy_ID, 319.0);
	glUniform1f(width_ID, win_width);
	glUniform1f(height_ID, win_height);
	glUniform3f(rot_vec_ID, rot_vec.at<float>(0), rot_vec.at<float>(1), rot_vec.at<float>(2));

	// �X�N���[�����N���A
	//glClear(GL_COLOR_BUFFER_BIT);
	
	do{

		if (camera.grab())
		{
			// �L���v�`���f������摜��؂�o��
			cv::Mat frame;
			camera.retrieve(frame, 3);
			//cv::flip(frame, frame, 1);

			transport_texture(image, (GLubyte*)dataBuffer, win_width, win_height, frame);

			GLubyte* p = static_cast<GLubyte*>(dataBuffer);
			out_cam_frame.data = p;
			cv::flip(out_cam_frame, out_cam_frame, 1); // �������]

			HRESULT hr = sender.Send(i_frame * avgTimePF, (int)win_width, (int)win_height, out_cam_frame.data);

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
		//glfwWaitEvents();
		// mouse event
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
			if (mouse_state == 0) {
				mouse_state = 1;
				glfwGetCursorPos(window, &prev_x, &prev_y);
			}
			double x, y;
			glfwGetCursorPos(window, &x, &y);
			//printf("%lf,%lf\n", x, y);
			rot_vec.at<float>(2) = rot_vec.at<float>(2) + (x - prev_x) / win_width * 2.0 * M_PI * 2.0;
			rot_vec.at<float>(0) = rot_vec.at<float>(0) + (y - prev_y) / win_height * M_PI * 2.0;
			prev_x = x;
			prev_y = y;
		}else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_RELEASE) {
			mouse_state = 0;
		}


		glUniform3f(rot_vec_ID, rot_vec.at<float>(0), rot_vec.at<float>(1), rot_vec.at<float>(2));
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
