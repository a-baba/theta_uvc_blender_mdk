#pragma once

// ycapture.h : �t�B���^�̏���`

// �����͎����̐��i�ɍ��킹�ĕύX���邱��

// �t�B���^����`
#define NAME_CAPTURESOURCE L"theta_uvc_blender_mdk"

// originai
// {5C2CD55C-92AD-4999-8666-912BD3E70001}
//DEFINE_GUID(CLSID_CaptureSource,
//	0x5c2cd55c, 0x92ad, 0x4999, 0x86, 0x66, 0x91, 0x2b, 0xd3, 0xe7, 0x0, 0x1);
DEFINE_GUID(CLSID_CaptureSource,
	0xf679993d, 0x449c, 0x4fc0, 0xab, 0xf8, 0xeb, 0x5e, 0x3, 0x4c, 0x66, 0xd6);

// �C�x���g�E�������}�b�v�h�t�@�C������`
// �t�B���^���E���M���œ���̖��O���g�p���邱��

// �������݂������C�x���g��
#define CS_EVENT_WRITE	L"theta_uvc_blender_mdk_Write"
// �f�[�^�ǂݍ��݂������~���[�e�b�N�X��
#define CS_EVENT_READ	L"theta_uvc_blender_mdk_Read"
// ���L�f�[�^�t�@�C����(�������}�b�v�h�t�@�C��)
#define CS_SHARED_PATH	L"theta_uvc_blender_mdk_Data"